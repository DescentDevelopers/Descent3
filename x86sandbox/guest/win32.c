/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * The Win32 and C runtime functions a script DLL may import, implemented inside
 * the sandbox. They work only on guest memory. None of them touches files, the
 * network or any other host resource; file functions always fail.
 *
 * An import that is not listed here still binds, to a stub that stops the call
 * with SB_ERR_MISSING_IMPORT if it is ever reached.
 */

#include "libc/math.h"
#include "libc/stdlib.h"
#include "libc/string.h"
#include "sb_internal.h"

#define NULL_GUARD SB_STUB_BASE /* addresses below this are null pointers: the guest cannot map them */

/* ------------------------------------------------------------------ */
/* Helpers                                                              */
/* ------------------------------------------------------------------ */

static uint8_t *gp(uint32_t addr, uint32_t len) {
  if (addr < NULL_GUARD || !sb_range_ok(addr, len)) {
    sb_abort(SB_ERR_BAD_ARGS, "import received an invalid pointer");
    return NULL;
  }
  return g_mem + addr;
}

/* Length of a guest string; aborts if it is not terminated inside guest memory. */
static int32_t gstrlen(uint32_t addr) {
  if (addr < NULL_GUARD) {
    sb_abort(SB_ERR_BAD_ARGS, "import received a null string");
    return -1;
  }
  int32_t n = sb_strlen(addr, g_guest_end);
  if (n < 0)
    sb_abort(SB_ERR_BAD_ARGS, "unterminated string");
  return n;
}

static int lower(int c) { return (c >= 'A' && c <= 'Z') ? c + 32 : c; }
static int upper(int c) { return (c >= 'a' && c <= 'z') ? c - 32 : c; }

static struct {
  uint32_t rand_seed;
  uint32_t atexit_fns[64];
  uint32_t natexit;
  uint32_t tls_used; /* bitmap of 32 slots */
  uint32_t vregions[64][2];
  uint32_t cmdline, cmdline_w, env, env_w;
  uint32_t strtok_next;
  uint32_t argv; /* __GetMainArgs: argv[0], NULL, then envp's NULL */
} W;

void sb_win32_reset(void) {
  memset(&W, 0, sizeof(W));
  W.rand_seed = 1;
}

static uint32_t static_string(const char *s) {
  uint32_t n = (uint32_t)strlen(s) + 1;
  uint32_t a = sb_heap_alloc(n);
  if (a)
    memcpy(g_mem + a, s, n);
  return a;
}

static uint32_t static_wstring(const char *s) {
  uint32_t n = (uint32_t)strlen(s) + 1;
  uint32_t a = sb_heap_alloc(2 * n);
  if (a)
    for (uint32_t i = 0; i < n; i++) {
      g_mem[a + 2 * i] = (uint8_t)s[i];
      g_mem[a + 2 * i + 1] = 0;
    }
  return a;
}

/* ------------------------------------------------------------------ */
/* Formatted output over guest arguments                                */
/* ------------------------------------------------------------------ */

typedef struct {
  uint32_t out; /* guest address, or 0 to collect into `local` */
  uint32_t cap; /* max chars including the NUL */
  uint32_t pos;
  char *local;
} gout;

static void gput(gout *o, char c) {
  if (o->pos + 1 < o->cap) {
    if (o->local)
      o->local[o->pos] = c;
    else if (!sb_write8(o->out + o->pos, (uint8_t)c))
      sb_abort(SB_ERR_BAD_ARGS, "formatted output past the end of guest memory");
  }
  o->pos++;
}

static void gterm(gout *o) {
  uint32_t at = o->pos < o->cap ? o->pos : o->cap - 1;
  if (o->cap == 0)
    return;
  if (o->local)
    o->local[at] = 0;
  else
    sb_write8(o->out + at, 0);
}

enum { F_LEFT = 1, F_PLUS = 2, F_SPACE = 4, F_ALT = 8, F_ZERO = 16 };

static void put_padded(gout *o, const char *s, int n, int width, int flags) {
  int pad = width > n ? width - n : 0;
  if (!(flags & F_LEFT))
    while (pad-- > 0)
      gput(o, ' ');
  for (int i = 0; i < n; i++)
    gput(o, s[i]);
  if (flags & F_LEFT)
    while (pad-- > 0)
      gput(o, ' ');
}

static void put_int(gout *o, uint64_t v, bool neg, int base, bool up, int width, int prec, int flags) {
  char digits[72];
  int n = 0;
  const char *set = up ? "0123456789ABCDEF" : "0123456789abcdef";
  if (!(v == 0 && prec == 0)) {
    do {
      digits[n++] = set[v % (uint64_t)base];
      v /= (uint64_t)base;
    } while (v && n < 64);
  }
  while (n < prec && n < 64)
    digits[n++] = '0';
  char sign = neg ? '-' : (flags & F_PLUS) ? '+' : (flags & F_SPACE) ? ' ' : 0;
  char buf[80];
  int m = 0;
  if (sign)
    buf[m++] = sign;
  if ((flags & F_ALT) && base == 16)
    buf[m++] = '0', buf[m++] = up ? 'X' : 'x';
  if ((flags & F_ZERO) && !(flags & F_LEFT) && prec < 0) {
    int total = m + n;
    while (total < width && n < 64) {
      digits[n++] = '0';
      total++;
    }
  }
  while (n)
    buf[m++] = digits[--n];
  put_padded(o, buf, m, width, flags);
}

/* `arg` is the guest address of the first variadic argument; it advances. */
static void gformat(gout *o, uint32_t fmt, uint32_t arg, bool wide_ok) {
  (void)wide_ok;
  int32_t flen = gstrlen(fmt);
  if (flen < 0)
    return;
  for (int32_t i = 0; i < flen; i++) {
    char c = (char)g_mem[fmt + i];
    if (c != '%') {
      gput(o, c);
      continue;
    }
    if (++i >= flen)
      break;
    int flags = 0, width = -1, prec = -1, len = 0;
    for (;; i++) {
      c = (char)g_mem[fmt + i];
      if (c == '-')
        flags |= F_LEFT;
      else if (c == '+')
        flags |= F_PLUS;
      else if (c == ' ')
        flags |= F_SPACE;
      else if (c == '#')
        flags |= F_ALT;
      else if (c == '0')
        flags |= F_ZERO;
      else
        break;
    }
    if (c == '*') {
      uint32_t w;
      sb_read32(arg, &w);
      arg += 4;
      width = (int32_t)w;
      if (width < 0)
        flags |= F_LEFT, width = -width;
      c = (char)g_mem[fmt + ++i];
    } else {
      while (c >= '0' && c <= '9') {
        width = (width < 0 ? 0 : width) * 10 + (c - '0');
        c = (char)g_mem[fmt + ++i];
      }
    }
    if (c == '.') {
      prec = 0;
      c = (char)g_mem[fmt + ++i];
      if (c == '*') {
        uint32_t p;
        sb_read32(arg, &p);
        arg += 4;
        prec = (int32_t)p;
        c = (char)g_mem[fmt + ++i];
      } else {
        while (c >= '0' && c <= '9') {
          prec = prec * 10 + (c - '0');
          c = (char)g_mem[fmt + ++i];
        }
      }
    }
    if (width > 4096)
      width = 4096;
    if (prec > 4096)
      prec = 4096;
    for (;;) {
      if (c == 'h')
        len = 1;
      else if (c == 'l' && len == 2)
        len = 3;
      else if (c == 'l')
        len = 2;
      else if (c == 'L')
        len = 4;
      else if (c == 'I' && i + 2 < flen && g_mem[fmt + i + 1] == '6' && g_mem[fmt + i + 2] == '4') {
        len = 3;
        i += 2;
      } else
        break;
      c = (char)g_mem[fmt + ++i];
    }
    switch (c) {
    case 'd':
    case 'i': {
      uint32_t lo, hi = 0;
      sb_read32(arg, &lo);
      arg += 4;
      int64_t v;
      if (len == 3) {
        sb_read32(arg, &hi);
        arg += 4;
        v = (int64_t)(((uint64_t)hi << 32) | lo);
      } else if (len == 1) {
        v = (int16_t)lo;
      } else {
        v = (int32_t)lo;
      }
      put_int(o, v < 0 ? (uint64_t)(-v) : (uint64_t)v, v < 0, 10, false, width, prec, flags);
      break;
    }
    case 'u':
    case 'x':
    case 'X':
    case 'o': {
      uint32_t lo, hi = 0;
      sb_read32(arg, &lo);
      arg += 4;
      uint64_t v = lo;
      if (len == 3) {
        sb_read32(arg, &hi);
        arg += 4;
        v |= (uint64_t)hi << 32;
      } else if (len == 1) {
        v = (uint16_t)lo;
      }
      put_int(o, v, false, c == 'u' ? 10 : c == 'o' ? 8 : 16, c == 'X', width, prec, flags);
      break;
    }
    case 'p': {
      uint32_t v;
      sb_read32(arg, &v);
      arg += 4;
      put_int(o, v, false, 16, true, 8, 8, 0);
      break;
    }
    case 'c': {
      uint32_t v;
      sb_read32(arg, &v);
      arg += 4;
      char ch = (char)v;
      put_padded(o, &ch, 1, width, flags);
      break;
    }
    case 's': {
      uint32_t s;
      sb_read32(arg, &s);
      arg += 4;
      if (s == 0) {
        put_padded(o, "(null)", 6, width, flags);
        break;
      }
      int32_t n = gstrlen(s);
      if (n < 0)
        return;
      if (prec >= 0 && n > prec)
        n = prec;
      int pad = width > n ? width - n : 0;
      if (!(flags & F_LEFT))
        while (pad-- > 0)
          gput(o, ' ');
      for (int32_t k = 0; k < n; k++)
        gput(o, (char)g_mem[s + k]);
      if (flags & F_LEFT)
        while (pad-- > 0)
          gput(o, ' ');
      break;
    }
    case 'f':
    case 'F':
    case 'e':
    case 'E':
    case 'g':
    case 'G': {
      uint32_t lo, hi;
      sb_read32(arg, &lo);
      sb_read32(arg + 4, &hi);
      arg += 8;
      uint64_t bits = ((uint64_t)hi << 32) | lo;
      double d;
      memcpy(&d, &bits, 8);
      char buf[512];
      int32_t n = host_fmt_double(d, c, prec < 0 ? 6 : prec, flags, width < 0 ? 0 : width, buf, (int32_t)sizeof(buf));
      if (n < 0)
        n = 0;
      if (n > (int32_t)sizeof(buf) - 1)
        n = (int32_t)sizeof(buf) - 1;
      for (int32_t k = 0; k < n; k++)
        gput(o, buf[k]);
      break;
    }
    case 'n':
      /* Writing the count through a guest pointer is not supported. */
      arg += 4;
      break;
    case '%':
      gput(o, '%');
      break;
    default:
      gput(o, '%');
      gput(o, c);
      break;
    }
  }
}

/* ------------------------------------------------------------------ */
/* C runtime: memory                                                    */
/* ------------------------------------------------------------------ */

static void crt_malloc(void) { sb_ret(sb_heap_alloc(sb_arg(0))); }
static void crt_free(void) { sb_heap_free(sb_arg(0)); }
static void crt_calloc(void) {
  uint32_t n = sb_arg(0), m = sb_arg(1);
  if (m && n > 0x7fffffffu / m) {
    sb_ret(0);
    return;
  }
  sb_ret(sb_heap_alloc(n * m));
}
static void crt_realloc(void) { sb_ret(sb_heap_realloc(sb_arg(0), sb_arg(1))); }
static void crt_msize(void) { sb_ret(sb_heap_size(sb_arg(0))); }

/* ------------------------------------------------------------------ */
/* C runtime: strings                                                   */
/* ------------------------------------------------------------------ */

static void crt_strlen(void) {
  int32_t n = gstrlen(sb_arg(0));
  sb_ret(n < 0 ? 0 : (uint32_t)n);
}

static void crt_strcpy(void) {
  uint32_t d = sb_arg(0), s = sb_arg(1);
  int32_t n = gstrlen(s);
  uint8_t *dp = n < 0 ? NULL : gp(d, (uint32_t)n + 1);
  if (dp)
    memmove(dp, g_mem + s, (uint32_t)n + 1);
  sb_ret(d);
}

static void crt_strncpy(void) {
  uint32_t d = sb_arg(0), s = sb_arg(1), cnt = sb_arg(2);
  uint8_t *dp = gp(d, cnt);
  if (!dp && cnt) {
    sb_ret(d);
    return;
  }
  uint32_t i = 0;
  for (; i < cnt; i++) {
    uint8_t c;
    if (!sb_read8(s + i, &c)) {
      sb_abort(SB_ERR_BAD_ARGS, "strncpy source");
      break;
    }
    dp[i] = c;
    if (!c)
      break;
  }
  for (; i < cnt; i++)
    dp[i] = 0;
  sb_ret(d);
}

static void crt_strcat(void) {
  uint32_t d = sb_arg(0), s = sb_arg(1);
  int32_t dn = gstrlen(d), sn = gstrlen(s);
  if (dn >= 0 && sn >= 0) {
    uint8_t *dp = gp(d + (uint32_t)dn, (uint32_t)sn + 1);
    if (dp)
      memmove(dp, g_mem + s, (uint32_t)sn + 1);
  }
  sb_ret(d);
}

static void crt_strncat(void) {
  uint32_t d = sb_arg(0), s = sb_arg(1), cnt = sb_arg(2);
  int32_t dn = gstrlen(d);
  if (dn < 0) {
    sb_ret(d);
    return;
  }
  uint32_t i = 0;
  for (; i < cnt; i++) {
    uint8_t c;
    if (!sb_read8(s + i, &c) || !sb_write8(d + (uint32_t)dn + i, c)) {
      sb_abort(SB_ERR_BAD_ARGS, "strncat");
      break;
    }
    if (!c)
      break;
  }
  if (i == cnt)
    sb_write8(d + (uint32_t)dn + i, 0);
  sb_ret(d);
}

static int gcmp(uint32_t a, uint32_t b, uint32_t max, bool fold) {
  for (uint32_t i = 0; i < max; i++) {
    uint8_t x, y;
    if (!sb_read8(a + i, &x) || !sb_read8(b + i, &y)) {
      sb_abort(SB_ERR_BAD_ARGS, "string compare");
      return 0;
    }
    int cx = fold ? lower(x) : x, cy = fold ? lower(y) : y;
    if (cx != cy)
      return cx - cy;
    if (!x)
      return 0;
  }
  return 0;
}

static void crt_strcmp(void) { sb_ret((uint32_t)gcmp(sb_arg(0), sb_arg(1), 0xffffffffu, false)); }
static void crt_strncmp(void) { sb_ret((uint32_t)gcmp(sb_arg(0), sb_arg(1), sb_arg(2), false)); }
static void crt_stricmp(void) { sb_ret((uint32_t)gcmp(sb_arg(0), sb_arg(1), 0xffffffffu, true)); }
static void crt_strnicmp(void) { sb_ret((uint32_t)gcmp(sb_arg(0), sb_arg(1), sb_arg(2), true)); }

static void crt_strchr(void) {
  uint32_t s = sb_arg(0);
  uint8_t c = (uint8_t)sb_arg(1);
  int32_t n = gstrlen(s);
  for (int32_t i = 0; i <= n; i++)
    if (g_mem[s + i] == c) {
      sb_ret(s + (uint32_t)i);
      return;
    }
  sb_ret(0);
}

static void crt_strrchr(void) {
  uint32_t s = sb_arg(0);
  uint8_t c = (uint8_t)sb_arg(1);
  int32_t n = gstrlen(s);
  for (int32_t i = n; i >= 0; i--)
    if (g_mem[s + i] == c) {
      sb_ret(s + (uint32_t)i);
      return;
    }
  sb_ret(0);
}

static void crt_strstr(void) {
  uint32_t h = sb_arg(0), nd = sb_arg(1);
  int32_t hn = gstrlen(h), nn = gstrlen(nd);
  if (hn < 0 || nn < 0) {
    sb_ret(0);
    return;
  }
  for (int32_t i = 0; i + nn <= hn; i++)
    if (memcmp(g_mem + h + i, g_mem + nd, (uint32_t)nn) == 0) {
      sb_ret(h + (uint32_t)i);
      return;
    }
  sb_ret(0);
}

static void crt_strdup(void) {
  uint32_t s = sb_arg(0);
  int32_t n = gstrlen(s);
  uint32_t d = n < 0 ? 0 : sb_heap_alloc((uint32_t)n + 1);
  if (d)
    memcpy(g_mem + d, g_mem + s, (uint32_t)n + 1);
  sb_ret(d);
}

static void crt_strlwr(void) {
  uint32_t s = sb_arg(0);
  int32_t n = gstrlen(s);
  for (int32_t i = 0; i < n; i++)
    g_mem[s + i] = (uint8_t)lower(g_mem[s + i]);
  sb_ret(s);
}

static void crt_strupr(void) {
  uint32_t s = sb_arg(0);
  int32_t n = gstrlen(s);
  for (int32_t i = 0; i < n; i++)
    g_mem[s + i] = (uint8_t)upper(g_mem[s + i]);
  sb_ret(s);
}

static void crt_memcpy(void) {
  uint32_t d = sb_arg(0), s = sb_arg(1), n = sb_arg(2);
  if (n) {
    uint8_t *dp = gp(d, n), *sp = gp(s, n);
    if (dp && sp)
      memmove(dp, sp, n);
  }
  sb_ret(d);
}

static void crt_memset(void) {
  uint32_t d = sb_arg(0), c = sb_arg(1), n = sb_arg(2);
  if (n) {
    uint8_t *dp = gp(d, n);
    if (dp)
      memset(dp, (int)c, n);
  }
  sb_ret(d);
}

static void crt_memcmp(void) {
  uint32_t a = sb_arg(0), b = sb_arg(1), n = sb_arg(2);
  if (!n) {
    sb_ret(0);
    return;
  }
  uint8_t *ap = gp(a, n), *bp = gp(b, n);
  sb_ret(ap && bp ? (uint32_t)memcmp(ap, bp, n) : 0);
}

static void crt_memchr(void) {
  uint32_t s = sb_arg(0), c = sb_arg(1) & 0xff, n = sb_arg(2);
  uint8_t *p = n ? gp(s, n) : NULL;
  for (uint32_t i = 0; p && i < n; i++)
    if (p[i] == c) {
      sb_ret(s + i);
      return;
    }
  sb_ret(0);
}

static void crt_strtok(void) {
  /* The position is kept per module, like the single-threaded C runtime. */
  uint32_t s = sb_arg(0), delim = sb_arg(1);
  int32_t dn = gstrlen(delim);
  if (dn < 0)
    return;
  if (s == 0)
    s = W.strtok_next;
  if (s == 0) {
    sb_ret(0);
    return;
  }
  uint8_t c;
  /* skip leading delimiters */
  while (sb_read8(s, &c) && c && memchr(g_mem + delim, c, (uint32_t)dn))
    s++;
  if (!sb_read8(s, &c) || !c) {
    W.strtok_next = 0;
    sb_ret(0);
    return;
  }
  uint32_t start = s;
  while (sb_read8(s, &c) && c && !memchr(g_mem + delim, c, (uint32_t)dn))
    s++;
  if (sb_read8(s, &c) && c) {
    sb_write8(s, 0);
    W.strtok_next = s + 1;
  } else {
    W.strtok_next = 0;
  }
  sb_ret(start);
}

/* _splitpath(path, drive, dir, fname, ext): each output may be NULL. Output
   buffers are assumed to be the documented _MAX_* sizes, as on Windows. */
static void copy_part(uint32_t out, uint32_t cap, uint32_t from, uint32_t len) {
  if (!out)
    return;
  if (len >= cap)
    len = cap - 1;
  uint8_t *dp = gp(out, len + 1);
  if (!dp)
    return;
  memmove(dp, g_mem + from, len);
  dp[len] = 0;
}
static void crt_splitpath(void) {
  uint32_t path = sb_arg(0), drive = sb_arg(1), dir = sb_arg(2), fname = sb_arg(3), ext = sb_arg(4);
  int32_t n = gstrlen(path);
  if (n < 0)
    return;
  uint32_t p = path, end = path + (uint32_t)n;
  if (n >= 2 && g_mem[path + 1] == ':') {
    copy_part(drive, 3, path, 2);
    p += 2;
  } else {
    copy_part(drive, 3, path, 0);
  }
  uint32_t last_sep = 0, last_dot = 0;
  for (uint32_t q = p; q < end; q++) {
    if (g_mem[q] == '\\' || g_mem[q] == '/')
      last_sep = q + 1;
    if (g_mem[q] == '.')
      last_dot = q;
  }
  uint32_t name_start = last_sep ? last_sep : p;
  if (last_dot < name_start)
    last_dot = end;
  copy_part(dir, 256, p, name_start - p);
  copy_part(fname, 256, name_start, last_dot - name_start);
  copy_part(ext, 256, last_dot, end - last_dot);
}

/* ------------------------------------------------------------------ */
/* C runtime: characters and numbers                                    */
/* ------------------------------------------------------------------ */

static void crt_toupper(void) { sb_ret((uint32_t)upper((int)sb_arg(0))); }
static void crt_tolower(void) { sb_ret((uint32_t)lower((int)sb_arg(0))); }
#define CTYPE(name, expr)                                                                                    \
  static void crt_##name(void) {                                                                             \
    int c = (int)sb_arg(0);                                                                                  \
    sb_ret((expr) ? 1u : 0u);                                                                                \
  }
CTYPE(isalpha, (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
CTYPE(isdigit, c >= '0' && c <= '9')
CTYPE(isspace, c == ' ' || (c >= 9 && c <= 13))
CTYPE(isalnum, (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
CTYPE(isupper, c >= 'A' && c <= 'Z')
CTYPE(islower, c >= 'a' && c <= 'z')
CTYPE(isxdigit, (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
CTYPE(isprint, c >= 32 && c < 127)
CTYPE(ispunct, c > 32 && c < 127 && !((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')))

static int64_t parse_int(uint32_t s, uint32_t *end, int base, bool *neg_out) {
  uint32_t p = s;
  uint8_t c;
  while (sb_read8(p, &c) && (c == ' ' || (c >= 9 && c <= 13)))
    p++;
  bool neg = false;
  if (sb_read8(p, &c) && (c == '-' || c == '+')) {
    neg = c == '-';
    p++;
  }
  if ((base == 0 || base == 16) && sb_read8(p, &c) && c == '0') {
    uint8_t x;
    if (sb_read8(p + 1, &x) && (x == 'x' || x == 'X')) {
      base = 16;
      p += 2;
    } else if (base == 0) {
      base = 8;
    }
  }
  if (base == 0)
    base = 10;
  uint64_t v = 0;
  uint32_t start = p;
  while (sb_read8(p, &c)) {
    int d = (c >= '0' && c <= '9') ? c - '0' : (c >= 'a' && c <= 'z') ? c - 'a' + 10 : (c >= 'A' && c <= 'Z') ? c - 'A' + 10 : 99;
    if (d >= base)
      break;
    if (v < (1ull << 40))
      v = v * (uint64_t)base + (uint64_t)d;
    p++;
  }
  if (end)
    *end = p == start ? s : p;
  *neg_out = neg;
  return (int64_t)v;
}

static void crt_atoi(void) {
  bool neg;
  int64_t v = parse_int(sb_arg(0), NULL, 10, &neg);
  sb_ret((uint32_t)(int32_t)(neg ? -v : v));
}

static void crt_strtol(void) {
  uint32_t s = sb_arg(0), endp = sb_arg(1);
  int base = (int)sb_arg(2);
  uint32_t end;
  bool neg;
  int64_t v = parse_int(s, &end, base, &neg);
  if (neg)
    v = -v;
  if (v > 0x7fffffff)
    v = 0x7fffffff;
  if (v < -0x7fffffff - 1)
    v = -0x7fffffff - 1;
  if (endp)
    sb_write32(endp, end);
  sb_ret((uint32_t)(int32_t)v);
}

static void crt_strtoul(void) {
  uint32_t s = sb_arg(0), endp = sb_arg(1);
  int base = (int)sb_arg(2);
  uint32_t end;
  bool neg;
  int64_t v = parse_int(s, &end, base, &neg);
  if (v > 0xffffffffll)
    v = 0xffffffffll;
  if (neg)
    v = -v;
  if (endp)
    sb_write32(endp, end);
  sb_ret((uint32_t)v);
}

static double parse_double_at(uint32_t s, uint32_t *end) {
  int32_t n = gstrlen(s);
  if (n < 0) {
    *end = s;
    return 0;
  }
  if (n > 512)
    n = 512;
  char buf[513];
  memcpy(buf, g_mem + s, (uint32_t)n);
  buf[n] = 0;
  int32_t used = 0;
  double d = host_parse_double(buf, n, &used);
  if (used < 0 || used > n)
    used = 0;
  *end = s + (uint32_t)used;
  return d;
}

static void crt_atof(void) {
  uint32_t end;
  sb_ret_f64(parse_double_at(sb_arg(0), &end));
}

static void crt_strtod(void) {
  uint32_t s = sb_arg(0), endp = sb_arg(1), end;
  double d = parse_double_at(s, &end);
  if (endp)
    sb_write32(endp, end);
  sb_ret_f64(d);
}

static void crt_itoa(void) {
  int32_t v = (int32_t)sb_arg(0);
  uint32_t buf = sb_arg(1);
  int base = (int)sb_arg(2);
  if (base < 2 || base > 36) {
    sb_ret(buf);
    return;
  }
  char tmp[40];
  int n = 0;
  bool neg = base == 10 && v < 0;
  uint32_t u = neg ? (uint32_t)(-(int64_t)v) : (uint32_t)v;
  do {
    int d = (int)(u % (uint32_t)base);
    tmp[n++] = (char)(d < 10 ? '0' + d : 'a' + d - 10);
    u /= (uint32_t)base;
  } while (u);
  if (neg)
    tmp[n++] = '-';
  uint8_t *p = gp(buf, (uint32_t)n + 1);
  if (p) {
    for (int i = 0; i < n; i++)
      p[i] = (uint8_t)tmp[n - 1 - i];
    p[n] = 0;
  }
  sb_ret(buf);
}

static void crt_abs(void) {
  int32_t v = (int32_t)sb_arg(0);
  sb_ret((uint32_t)(v < 0 ? -v : v));
}

/* ------------------------------------------------------------------ */
/* C runtime: formatted output                                          */
/* ------------------------------------------------------------------ */

/* sprintf needs the address of its third argument; read it from the stack. */
static uint32_t va_start_addr(int fixed);

static void crt_sprintf_real(void) {
  uint32_t buf = sb_arg(0);
  if (buf < NULL_GUARD || !sb_range_ok(buf, 1)) {
    sb_abort(SB_ERR_BAD_ARGS, "sprintf buffer");
    return;
  }
  gout o = {buf, sb_guest_extent(buf), 0, NULL};
  gformat(&o, sb_arg(1), va_start_addr(2), false);
  gterm(&o);
  sb_ret(o.pos);
}

static void ms_snprintf(uint32_t buf, uint32_t count, uint32_t fmt, uint32_t va) {
  /* Microsoft _snprintf: returns -1 when the output does not fit, and only
     writes the terminating NUL if there is room for it. */
  if (count && (buf < NULL_GUARD || !sb_range_ok(buf, count))) {
    sb_abort(SB_ERR_BAD_ARGS, "_snprintf buffer");
    return;
  }
  gout o = {buf, count + 1, 0, NULL};
  gformat(&o, fmt, va, false);
  if (o.pos < count)
    sb_write8(buf + o.pos, 0);
  sb_ret(o.pos <= count ? o.pos : 0xffffffffu);
}

static void crt_snprintf(void) { ms_snprintf(sb_arg(0), sb_arg(1), sb_arg(2), va_start_addr(3)); }
static void crt_vsnprintf(void) { ms_snprintf(sb_arg(0), sb_arg(1), sb_arg(2), sb_arg(3)); }

static void crt_vsprintf(void) {
  uint32_t buf = sb_arg(0);
  if (buf < NULL_GUARD || !sb_range_ok(buf, 1)) {
    sb_abort(SB_ERR_BAD_ARGS, "vsprintf buffer");
    return;
  }
  gout o = {buf, sb_guest_extent(buf), 0, NULL};
  gformat(&o, sb_arg(1), sb_arg(2), false);
  gterm(&o);
  sb_ret(o.pos);
}

static void log_formatted(uint32_t fmt, uint32_t va) {
  char line[512];
  gout o = {0, sizeof(line), 0, line};
  gformat(&o, fmt, va, false);
  gterm(&o);
  host_log(line, (uint32_t)strlen(line));
  sb_ret(o.pos);
}

static void crt_printf(void) { log_formatted(sb_arg(0), va_start_addr(1)); }

static void crt_puts(void) {
  uint32_t s = sb_arg(0);
  int32_t n = gstrlen(s);
  if (n >= 0)
    host_log((const char *)g_mem + s, (uint32_t)(n > 1024 ? 1024 : n));
  sb_ret(0);
}

static void user32_wsprintfA(void) {
  uint32_t buf = sb_arg(0);
  if (buf < NULL_GUARD || !sb_range_ok(buf, 1024)) {
    sb_abort(SB_ERR_BAD_ARGS, "wsprintfA buffer");
    return;
  }
  gout o = {buf, 1024, 0, NULL};
  gformat(&o, sb_arg(1), va_start_addr(2), false);
  gterm(&o);
  sb_ret(o.pos < 1024 ? o.pos : 1023);
}

/* ------------------------------------------------------------------ */
/* C runtime: math                                                      */
/* ------------------------------------------------------------------ */

#define MATH1(name, expr)                                                                                    \
  static void crt_##name(void) {                                                                             \
    double x = sb_arg_f64(0);                                                                                \
    sb_ret_f64(expr);                                                                                        \
  }
MATH1(sin, sin(x))
MATH1(cos, cos(x))
MATH1(tan, tan(x))
MATH1(asin, sb_asin(x))
MATH1(acos, sb_acos(x))
MATH1(atan, sb_atan(x))
MATH1(sqrt, sqrt(x))
MATH1(exp, sb_exp(x))
MATH1(log, sb_log(x))
MATH1(log10, sb_log10(x))
MATH1(fabs, fabs(x))
MATH1(floor, floor(x))
MATH1(ceil, ceil(x))
static void crt_atan2(void) { sb_ret_f64(atan2(sb_arg_f64(0), sb_arg_f64(2))); }
static void crt_pow(void) { sb_ret_f64(pow(sb_arg_f64(0), sb_arg_f64(2))); }
static void crt_fmod(void) { sb_ret_f64(sb_fmod(sb_arg_f64(0), sb_arg_f64(2))); }

/* The _CI* helpers take their arguments on the x87 stack. One-argument forms
   replace ST(0); two-argument forms compute f(ST(1), ST(0)) and pop once. */
static FPU *fpu(void) { return sbcpu_fpu(g_cpu); }
#define CI1(name, expr)                                                                                      \
  static void crt_CI##name(void) {                                                                           \
    double x = sbfpu_st(fpu(), 0);                                                                           \
    sbfpu_set_st(fpu(), 0, expr);                                                                            \
  }
#define CI2(name, expr)                                                                                      \
  static void crt_CI##name(void) {                                                                           \
    double y = sbfpu_st(fpu(), 0), x = sbfpu_st(fpu(), 1);                                                   \
    sbfpu_pop(fpu());                                                                                        \
    sbfpu_set_st(fpu(), 0, expr);                                                                            \
  }
CI1(sin, sin(x))
CI1(cos, cos(x))
CI1(tan, tan(x))
CI1(asin, sb_asin(x))
CI1(acos, sb_acos(x))
CI1(atan, sb_atan(x))
CI1(sqrt, sqrt(x))
CI1(exp, sb_exp(x))
CI1(log, sb_log(x))
CI1(log10, sb_log10(x))
CI2(atan2, atan2(x, y))
CI2(pow, pow(x, y))
CI2(fmod, sb_fmod(x, y))

/* _ftol: convert ST(0) to a 64-bit integer (truncating), pop it, return in EDX:EAX. */
static void crt_ftol(void) {
  double x = sbfpu_st(fpu(), 0);
  sbfpu_pop(fpu());
  int64_t v;
  if (x != x || x >= 9223372036854775808.0 || x < -9223372036854775808.0)
    v = (int64_t)0x8000000000000000ull;
  else
    v = (int64_t)x;
  sb_ret64((uint32_t)v, (uint32_t)((uint64_t)v >> 32));
}

static void crt_controlfp(void) {
  uint32_t nw = sb_arg(0), mask = sb_arg(1);
  uint16_t cw = sbfpu_cw(fpu());
  /* Only precision and rounding control bits map directly; keep it simple. */
  uint32_t ms = 0x0009001fu; /* report: exceptions masked, 53-bit, round to nearest */
  if (mask & 0x00030000u) {
    uint32_t pc = (nw >> 16) & 3; /* _PC_24=0x20000,_PC_53=0x10000,_PC_64=0 */
    uint16_t x87pc = pc == 2 ? 0 : pc == 1 ? 2 : 3;
    cw = (uint16_t)((cw & ~0x300) | (x87pc << 8));
  }
  if (mask & 0x00000300u) {
    uint32_t rc = (nw >> 8) & 3;
    cw = (uint16_t)((cw & ~0xc00) | (rc << 10));
  }
  sbfpu_set_cw(fpu(), cw);
  sb_ret(ms);
}

static void crt_isnan(void) {
  double x = sb_arg_f64(0);
  sb_ret(x != x);
}
static void crt_finite(void) {
  double x = sb_arg_f64(0);
  sb_ret(x == x && x - x == 0);
}
static void crt_ret0(void) { sb_ret(0); }
static void crt_ret1(void) { sb_ret(1); }

static void crt_rand(void) {
  W.rand_seed = W.rand_seed * 214013u + 2531011u;
  sb_ret((W.rand_seed >> 16) & 0x7fff);
}
static void crt_srand(void) { W.rand_seed = sb_arg(0); }
static void crt_time(void) {
  uint32_t t = 946684800u + host_time_ms() / 1000u;
  uint32_t p = sb_arg(0);
  if (p)
    sb_write32(p, t);
  sb_ret(t);
}
static void crt_clock(void) { sb_ret(host_time_ms()); }

/* ------------------------------------------------------------------ */
/* C runtime: startup, shutdown and callbacks                           */
/* ------------------------------------------------------------------ */

static void crt_initterm(void) {
  uint32_t b = sb_arg(0), e = sb_arg(1);
  for (uint32_t p = b; p < e && p >= b; p += 4) {
    uint32_t fn;
    if (!sb_read32(p, &fn)) {
      sb_abort(SB_ERR_BAD_ARGS, "_initterm table");
      return;
    }
    if (fn && sb_call_guest(fn, NULL, 0, NULL) != SB_OK)
      return;
  }
}

static void register_atexit(uint32_t fn) {
  if (W.natexit < 64)
    W.atexit_fns[W.natexit++] = fn;
}
static void crt_onexit(void) {
  register_atexit(sb_arg(0));
  sb_ret(sb_arg(0));
}
static void crt_dllonexit(void) {
  register_atexit(sb_arg(0));
  sb_ret(sb_arg(0));
}
static void crt_atexit(void) {
  register_atexit(sb_arg(0));
  sb_ret(0);
}

void sb_run_atexit(void) {
  while (W.natexit) {
    uint32_t fn = W.atexit_fns[--W.natexit];
    if (fn && sb_call_guest(fn, NULL, 0, NULL) != SB_OK)
      return;
  }
}

/* qsort as a heapsort: O(n log n) comparisons, in place. The comparison
   function is guest code and may return anything or change the array while the
   sort runs; the element indices stay below n and every loop ends whatever it
   returns. Like MSVC's qsort, the sort is not stable. */
struct qs {
  uint32_t base, w, cmp;
  bool failed;
};

/* True if element a sorts after element b. */
static bool qs_after(struct qs *q, uint32_t a, uint32_t b) {
  uint32_t args[2] = {q->base + a * q->w, q->base + b * q->w}, r = 0;
  if (q->failed || sb_call_guest(q->cmp, args, 2, &r) != SB_OK) {
    q->failed = true;
    return false;
  }
  return (int32_t)r > 0;
}

static void qs_swap(struct qs *q, uint32_t a, uint32_t b) {
  uint8_t tmp[4096];
  uint8_t *pa = g_mem + q->base + a * q->w, *pb = g_mem + q->base + b * q->w;
  memcpy(tmp, pa, q->w);
  memcpy(pa, pb, q->w);
  memcpy(pb, tmp, q->w);
}

/* Moves element `root` down the heap of the first `end` elements. Each pass
   moves root to a child, so the loop ends after at most log2(end) passes. */
static void qs_sift(struct qs *q, uint32_t root, uint32_t end) {
  while (!q->failed) {
    uint32_t child = 2 * root + 1;
    if (child >= end)
      return;
    if (child + 1 < end && qs_after(q, child + 1, child))
      child++;
    if (!qs_after(q, child, root))
      return;
    qs_swap(q, root, child);
    root = child;
  }
}

static void crt_qsort(void) {
  uint32_t base = sb_arg(0), n = sb_arg(1), w = sb_arg(2), cmp = sb_arg(3);
  if (n < 2 || w == 0)
    return;
  if (w > 4096 || n > 0x7fffffffu / w || !gp(base, n * w))
    return;
  struct qs q = {base, w, cmp, false};
  for (uint32_t i = n / 2; i-- > 0;)
    qs_sift(&q, i, n);
  for (uint32_t end = n - 1; end > 0 && !q.failed; end--) {
    qs_swap(&q, 0, end);
    qs_sift(&q, 0, end);
  }
}

static void crt_fatal(void) { sb_abort(SB_ERR_ABORTED, "guest called a fatal runtime function"); }
static void crt_purecall(void) { sb_abort(SB_ERR_ABORTED, "pure virtual function call"); }

/* ------------------------------------------------------------------ */
/* kernel32                                                             */
/* ------------------------------------------------------------------ */

#define HEAP_HANDLE 0x00050000u

/* Module handles for LoadLibraryA, GetModuleHandleA and GetProcAddress: the
   loaded DLL's image base, or one of these pseudo handles for the system DLLs
   the sandbox implements. Every C runtime name maps to msvcrt. */
static const char *const kModules[] = {"kernel32", "msvcrt", "crtdll", "user32"};
#define MODULE_HANDLE_BASE 0x7e000000u

/* The handle for the DLL named at guest address `addr`, or 0. */
static uint32_t module_handle(uint32_t addr) {
  int32_t n = gstrlen(addr);
  if (n < 0)
    return 0;
  char name[64];
  uint32_t start = 0;
  for (int32_t i = 0; i < n; i++)
    if (g_mem[addr + i] == '\\' || g_mem[addr + i] == '/' || g_mem[addr + i] == ':')
      start = (uint32_t)i + 1; /* a path: only the file name matters */
  uint32_t len = 0;
  for (uint32_t i = start; i < (uint32_t)n && len < sizeof(name) - 1; i++)
    name[len++] = (char)lower(g_mem[addr + i]);
  name[len] = 0;
  if (len > 4 && strcmp(name + len - 4, ".dll") == 0)
    name[len - 4] = 0;
  if (name[0] == 'm' && name[1] == 's' && name[2] == 'v' && name[3] == 'c' && name[4] == 'r')
    return MODULE_HANDLE_BASE + (1u << 16);
  for (uint32_t i = 0; i < sizeof(kModules) / sizeof(kModules[0]); i++)
    if (strcmp(name, kModules[i]) == 0)
      return MODULE_HANDLE_BASE + (i << 16);
  return 0;
}

static void k32_GetModuleHandleA(void) { sb_ret(sb_arg(0) ? module_handle(sb_arg(0)) : g_image_base); }
static void k32_LoadLibraryA(void) { sb_ret(module_handle(sb_arg(0))); }

/* GetProcAddress: NULL for a function the sandbox does not implement, as on
   Windows for a function a DLL does not export. Lookups by ordinal fail. */
static void k32_GetProcAddress(void) {
  uint32_t h = sb_arg(0), name = sb_arg(1);
  if (name < NULL_GUARD) {
    sb_ret(0);
    return;
  }
  int32_t n = gstrlen(name);
  if (n < 0)
    return;
  char fname[64];
  if (n >= (int32_t)sizeof(fname)) {
    sb_ret(0);
    return;
  }
  memcpy(fname, g_mem + name, (size_t)n + 1);
  uint32_t i = (h - MODULE_HANDLE_BASE) >> 16;
  if (h == g_image_base)
    sb_ret(sb_find_own_export(fname));
  else if (h >= MODULE_HANDLE_BASE && (h & 0xffffu) == 0 && i < sizeof(kModules) / sizeof(kModules[0]))
    sb_ret(sb_resolve_import(kModules[i], fname));
  else
    sb_ret(0);
}
static void k32_GetTickCount(void) { sb_ret(host_time_ms()); }
static void k32_QueryPerformanceCounter(void) {
  uint64_t v = (uint64_t)host_time_ms() * 1000u;
  uint32_t p = sb_arg(0);
  sb_ret(sb_write32(p, (uint32_t)v) && sb_write32(p + 4, (uint32_t)(v >> 32)));
}
static void k32_QueryPerformanceFrequency(void) {
  uint32_t p = sb_arg(0);
  sb_ret(sb_write32(p, 1000000u) && sb_write32(p + 4, 0));
}
static void k32_OutputDebugStringA(void) {
  uint32_t s = sb_arg(0);
  int32_t n = gstrlen(s);
  if (n >= 0)
    host_log((const char *)g_mem + s, (uint32_t)(n > 1024 ? 1024 : n));
}
static void k32_GetLastError(void) {
  uint32_t v = 0;
  sb_read32(SB_TEB_ADDR + 0x34, &v);
  sb_ret(v);
}
static void k32_SetLastError(void) { sb_write32(SB_TEB_ADDR + 0x34, sb_arg(0)); }

static void k32_InterlockedIncrement(void) {
  uint32_t p = sb_arg(0), v;
  if (!gp(p, 4))
    return;
  sb_read32(p, &v);
  sb_write32(p, ++v);
  sb_ret(v);
}
static void k32_InterlockedDecrement(void) {
  uint32_t p = sb_arg(0), v;
  if (!gp(p, 4))
    return;
  sb_read32(p, &v);
  sb_write32(p, --v);
  sb_ret(v);
}
static void k32_InterlockedExchange(void) {
  uint32_t p = sb_arg(0), v;
  if (!gp(p, 4))
    return;
  sb_read32(p, &v);
  sb_write32(p, sb_arg(1));
  sb_ret(v);
}
static void k32_InterlockedCompareExchange(void) {
  uint32_t p = sb_arg(0), v;
  if (!gp(p, 4))
    return;
  sb_read32(p, &v);
  if (v == sb_arg(2))
    sb_write32(p, sb_arg(1));
  sb_ret(v);
}
static void k32_InterlockedExchangeAdd(void) {
  uint32_t p = sb_arg(0), v;
  if (!gp(p, 4))
    return;
  sb_read32(p, &v);
  sb_write32(p, v + sb_arg(1));
  sb_ret(v);
}

static void k32_TlsAlloc(void) {
  for (uint32_t i = 0; i < 32; i++)
    if (!(W.tls_used & (1u << i))) {
      W.tls_used |= 1u << i;
      sb_write32(SB_TLS_ADDR + 4 * i, 0);
      sb_ret(i);
      return;
    }
  sb_ret(0xffffffffu);
}
static void k32_TlsFree(void) {
  uint32_t i = sb_arg(0);
  if (i < 32)
    W.tls_used &= ~(1u << i);
  sb_ret(i < 32);
}
static void k32_TlsGetValue(void) {
  uint32_t i = sb_arg(0), v = 0;
  if (i < 32)
    sb_read32(SB_TLS_ADDR + 4 * i, &v);
  sb_ret(v);
}
static void k32_TlsSetValue(void) {
  uint32_t i = sb_arg(0);
  if (i < 32)
    sb_write32(SB_TLS_ADDR + 4 * i, sb_arg(1));
  sb_ret(i < 32);
}

static void k32_HeapCreate(void) { sb_ret(HEAP_HANDLE); }
static void k32_GetProcessHeap(void) { sb_ret(HEAP_HANDLE); }
static void k32_HeapAlloc(void) { sb_ret(sb_heap_alloc(sb_arg(2))); }
static void k32_HeapFree(void) {
  sb_heap_free(sb_arg(2));
  sb_ret(1);
}
static void k32_HeapReAlloc(void) {
  uint32_t flags = sb_arg(1), addr = sb_arg(2), size = sb_arg(3);
  if (flags & 0x10) { /* HEAP_REALLOC_IN_PLACE_ONLY: the block must not move */
    uint32_t have = sb_heap_size(addr);
    sb_ret(have && size <= have ? addr : 0);
    return;
  }
  sb_ret(sb_heap_realloc(addr, size));
}
static void k32_HeapSize(void) {
  uint32_t n = sb_heap_size(sb_arg(2));
  sb_ret(n ? n : 0xffffffffu);
}

static void k32_VirtualAlloc(void) {
  uint32_t addr = sb_arg(0), size = sb_arg(1);
  if (addr) {
    /* Committing inside a region reserved earlier: memory is already there. */
    for (uint32_t i = 0; i < 64; i++)
      if (W.vregions[i][1] && addr >= W.vregions[i][0] && addr - W.vregions[i][0] < W.vregions[i][1] &&
          size <= W.vregions[i][1] - (addr - W.vregions[i][0])) {
        sb_ret(addr);
        return;
      }
    sb_ret(0);
    return;
  }
  if (size == 0 || size > (64u << 20)) {
    sb_ret(0);
    return;
  }
  uint32_t raw = sb_heap_alloc(size + 0xffffu);
  if (!raw) {
    sb_ret(0);
    return;
  }
  uint32_t aligned = (raw + 0xffffu) & ~0xffffu;
  for (uint32_t i = 0; i < 64; i++)
    if (!W.vregions[i][1]) {
      W.vregions[i][0] = aligned;
      W.vregions[i][1] = size;
      sb_ret(aligned);
      return;
    }
  sb_heap_free(raw);
  sb_ret(0);
}
static void k32_VirtualFree(void) { sb_ret(1); /* the memory stays in the guest heap */ }

static void k32_GetVersion(void) { sb_ret(0x0a280105u); /* Windows XP */ }
static void k32_GetVersionExA(void) {
  uint32_t p = sb_arg(0);
  if (!gp(p, 148)) {
    sb_ret(0);
    return;
  }
  sb_write32(p + 4, 5);
  sb_write32(p + 8, 1);
  sb_write32(p + 12, 2600);
  sb_write32(p + 16, 2);
  sb_ret(1);
}
static void k32_GetCurrentThreadId(void) { sb_ret(0x104); }
static void k32_GetCurrentProcessId(void) { sb_ret(0x100); }
static void k32_GetCurrentProcess(void) { sb_ret(0xffffffffu); }
static void k32_GetCurrentThread(void) { sb_ret(0xfffffffeu); }
static void k32_IsBadReadPtr(void) { sb_ret(!(sb_arg(0) >= NULL_GUARD && sb_range_ok(sb_arg(0), sb_arg(1)))); }
static void k32_IsBadCodePtr(void) { sb_ret(!(sb_arg(0) >= NULL_GUARD && sb_range_ok(sb_arg(0), 1))); }
static void k32_GetCommandLineA(void) {
  if (!W.cmdline)
    W.cmdline = static_string("descent3.exe");
  sb_ret(W.cmdline);
}
static void k32_GetCommandLineW(void) {
  if (!W.cmdline_w)
    W.cmdline_w = static_wstring("descent3.exe");
  sb_ret(W.cmdline_w);
}
static void k32_GetEnvironmentStrings(void) {
  if (!W.env)
    W.env = sb_heap_alloc(4);
  sb_ret(W.env);
}
static void k32_GetEnvironmentStringsW(void) {
  if (!W.env_w)
    W.env_w = sb_heap_alloc(4);
  sb_ret(W.env_w);
}
static void k32_GetStartupInfoA(void) {
  uint32_t p = sb_arg(0);
  uint8_t *b = gp(p, 68);
  if (b) {
    memset(b, 0, 68);
    b[0] = 68;
  }
}
static void k32_GetStdHandle(void) { sb_ret(0x10u + ((-(int32_t)sb_arg(0)) - 10)); }
static void k32_GetFileType(void) { sb_ret(2); }
static void k32_SetHandleCount(void) { sb_ret(sb_arg(0)); }
static void k32_GetACP(void) { sb_ret(1252); }
static void k32_GetOEMCP(void) { sb_ret(437); }
static void k32_GetCPInfo(void) {
  uint8_t *b = gp(sb_arg(1), 20);
  if (!b) {
    sb_ret(0);
    return;
  }
  memset(b, 0, 20);
  b[0] = 1;
  b[4] = '?';
  sb_ret(1);
}
static void k32_MultiByteToWideChar(void) {
  uint32_t src = sb_arg(2), dst = sb_arg(4), dstlen = sb_arg(5);
  int32_t n = (int32_t)sb_arg(3);
  if (n < 0) {
    n = gstrlen(src);
    if (n < 0)
      return;
    n++;
  }
  if (dstlen == 0) {
    sb_ret((uint32_t)n);
    return;
  }
  if ((uint32_t)n > dstlen || !gp(src, (uint32_t)n) || !gp(dst, 2u * (uint32_t)n)) {
    sb_ret(0);
    return;
  }
  for (int32_t i = 0; i < n; i++) {
    g_mem[dst + 2 * i] = g_mem[src + i];
    g_mem[dst + 2 * i + 1] = 0;
  }
  sb_ret((uint32_t)n);
}
static void k32_WideCharToMultiByte(void) {
  uint32_t src = sb_arg(2), dst = sb_arg(4), dstlen = sb_arg(5);
  int32_t n = (int32_t)sb_arg(3);
  if (n < 0) {
    n = 0;
    uint8_t lo, hi;
    while (sb_read8(src + 2 * n, &lo) && sb_read8(src + 2 * n + 1, &hi) && (lo || hi))
      n++;
    n++;
  }
  if (dstlen == 0) {
    sb_ret((uint32_t)n);
    return;
  }
  if ((uint32_t)n > dstlen || !gp(src, 2u * (uint32_t)n) || !gp(dst, (uint32_t)n)) {
    sb_ret(0);
    return;
  }
  for (int32_t i = 0; i < n; i++) {
    uint8_t hi = g_mem[src + 2 * i + 1];
    g_mem[dst + i] = hi ? '?' : g_mem[src + 2 * i];
  }
  sb_ret((uint32_t)n);
}
static void k32_GetModuleFileNameA(void) {
  static const char name[] = "C:\\Descent3\\script.dll";
  uint32_t buf = sb_arg(1), cap = sb_arg(2);
  uint32_t n = sizeof(name) - 1;
  if (cap == 0 || !gp(buf, cap)) {
    sb_ret(0);
    return;
  }
  if (n >= cap)
    n = cap - 1;
  memcpy(g_mem + buf, name, n);
  g_mem[buf + n] = 0;
  sb_ret(n);
}
static void k32_WriteFile(void) {
  uint32_t buf = sb_arg(1), n = sb_arg(2), written = sb_arg(3);
  if (n && gp(buf, n))
    host_log((const char *)g_mem + buf, n > 1024 ? 1024 : n);
  if (written)
    sb_write32(written, n);
  sb_ret(1);
}
static void k32_GetSystemTimeAsFileTime(void) {
  uint64_t t = 125911584000000000ull + (uint64_t)host_time_ms() * 10000u;
  uint32_t p = sb_arg(0);
  if (gp(p, 8)) {
    sb_write32(p, (uint32_t)t);
    sb_write32(p + 4, (uint32_t)(t >> 32));
  }
}
static void k32_GetLocalTime(void) {
  uint8_t *b = gp(sb_arg(0), 16);
  if (b) {
    memset(b, 0, 16);
    b[0] = 2000 & 0xff;
    b[1] = 2000 >> 8;
    b[2] = 1;
    b[6] = 1;
  }
}
static void k32_CreateFileA(void) { sb_ret(0xffffffffu); /* no file access from scripts */ }
static void k32_GetTimeZoneInformation(void) {
  uint8_t *b = gp(sb_arg(0), 172);
  if (b)
    memset(b, 0, 172);
  sb_ret(0);
}

static void user32_MessageBoxA(void) {
  uint32_t text = sb_arg(1);
  int32_t n = gstrlen(text);
  if (n >= 0)
    host_log((const char *)g_mem + text, (uint32_t)(n > 1024 ? 1024 : n));
  sb_ret(1);
}

/* ------------------------------------------------------------------ */
/* Import table                                                         */
/* ------------------------------------------------------------------ */

/* crtdll.dll (the C runtime GCC-built scripts use): fills argc, argv and envp
   for the DLL startup code. argv is {"descent3.exe", NULL}; the environment is empty. */
static void crtdll_GetMainArgs(void) {
  uint32_t pargc = sb_arg(0), pargv = sb_arg(1), penv = sb_arg(2);
  if (!W.cmdline)
    W.cmdline = static_string("descent3.exe");
  if (!W.argv) {
    W.argv = sb_heap_alloc(12); /* argv[0], argv[1] = NULL, envp[0] = NULL */
    if (!W.argv) {
      sb_abort(SB_ERR_NO_MEMORY, "__GetMainArgs");
      return;
    }
    sb_write32(W.argv, W.cmdline);
    sb_write32(W.argv + 4, 0);
    sb_write32(W.argv + 8, 0);
  }
  if (!gp(pargc, 4) || !gp(pargv, 4) || !gp(penv, 4))
    return;
  sb_write32(pargc, 1);
  sb_write32(pargv, W.argv);
  sb_write32(penv, W.argv + 8);
}

static void k32_DebugBreak(void) { sb_abort(SB_ERR_ABORTED, "guest called DebugBreak"); }

/* One character of a CompareString argument: unit is 1 (A) or 2 (W). */
static bool cs_char(uint32_t s, uint32_t i, uint32_t unit, uint32_t *c) {
  uint8_t lo, hi = 0;
  if (!sb_read8(s + i * unit, &lo) || (unit == 2 && !sb_read8(s + i * unit + 1, &hi))) {
    sb_abort(SB_ERR_BAD_ARGS, "CompareString");
    return false;
  }
  *c = lo | (uint32_t)hi << 8;
  return true;
}

/* CompareStringA/W(locale, flags, s1, n1, s2, n2): ordinal comparison, case-folded
   for ASCII with NORM_IGNORECASE. A count of -1 means NUL-terminated. Returns
   CSTR_LESS_THAN (1), CSTR_EQUAL (2) or CSTR_GREATER_THAN (3). */
static void compare_string(uint32_t unit) {
  uint32_t flags = sb_arg(1), s1 = sb_arg(2), s2 = sb_arg(4);
  int32_t n1 = (int32_t)sb_arg(3), n2 = (int32_t)sb_arg(5);
  bool fold = flags & 1; /* NORM_IGNORECASE */
  if (s1 < NULL_GUARD || s2 < NULL_GUARD || n1 < -1 || n2 < -1) {
    sb_ret(0);
    return;
  }
  for (uint32_t i = 0;; i++) {
    uint32_t a = 0, b = 0;
    bool end1 = n1 >= 0 && i >= (uint32_t)n1, end2 = n2 >= 0 && i >= (uint32_t)n2;
    if (!end1 && !cs_char(s1, i, unit, &a))
      return;
    if (!end2 && !cs_char(s2, i, unit, &b))
      return;
    end1 = end1 || (n1 < 0 && a == 0);
    end2 = end2 || (n2 < 0 && b == 0);
    if (end1 || end2) {
      sb_ret(end1 && end2 ? 2 : end1 ? 1 : 3);
      return;
    }
    if (fold) {
      a = a < 128 ? (uint32_t)lower((int)a) : a;
      b = b < 128 ? (uint32_t)lower((int)b) : b;
    }
    if (a != b) {
      sb_ret(a < b ? 1 : 3);
      return;
    }
  }
}
static void k32_CompareStringA(void) { compare_string(1); }
static void k32_CompareStringW(void) { compare_string(2); }

static void k32_GetUserDefaultLCID(void) { sb_ret(0x409); } /* en-US */

#define C(dll, name, fn) {dll, name, SB_IMP_CDECL, 0, fn, 0}
#define S(dll, name, bytes, fn) {dll, name, SB_IMP_STDCALL, bytes, fn, 0}
#define D(dll, name, size) {dll, name, SB_IMP_CDECL, 0, NULL, size}

static const sb_import_def g_table[] = {
    /* memory */
    C("crtdll", "__GetMainArgs", crtdll_GetMainArgs),
    C("msvcrt", "malloc", crt_malloc),
    C("msvcrt", "free", crt_free),
    C("msvcrt", "calloc", crt_calloc),
    C("msvcrt", "realloc", crt_realloc),
    C("msvcrt", "_msize", crt_msize),
    C("msvcrt", "??2@YAPAXI@Z", crt_malloc),  /* operator new */
    C("msvcrt", "??3@YAXPAX@Z", crt_free),    /* operator delete */
    C("msvcrt", "??_U@YAPAXI@Z", crt_malloc), /* operator new[] */
    C("msvcrt", "??_V@YAXPAX@Z", crt_free),   /* operator delete[] */
    /* strings */
    C("msvcrt", "strlen", crt_strlen),
    C("msvcrt", "strcpy", crt_strcpy),
    C("msvcrt", "strncpy", crt_strncpy),
    C("msvcrt", "strcat", crt_strcat),
    C("msvcrt", "strncat", crt_strncat),
    C("msvcrt", "strcmp", crt_strcmp),
    C("msvcrt", "strncmp", crt_strncmp),
    C("msvcrt", "_stricmp", crt_stricmp),
    C("msvcrt", "stricmp", crt_stricmp),
    C("msvcrt", "_strcmpi", crt_stricmp),
    C("msvcrt", "strcmpi", crt_stricmp),
    C("msvcrt", "_strnicmp", crt_strnicmp),
    C("msvcrt", "strnicmp", crt_strnicmp),
    C("msvcrt", "strchr", crt_strchr),
    C("msvcrt", "strrchr", crt_strrchr),
    C("msvcrt", "strstr", crt_strstr),
    C("msvcrt", "strtok", crt_strtok),
    C("msvcrt", "_splitpath", crt_splitpath),
    C("msvcrt", "_strdup", crt_strdup),
    C("msvcrt", "strdup", crt_strdup),
    C("msvcrt", "_strlwr", crt_strlwr),
    C("msvcrt", "strlwr", crt_strlwr),
    C("msvcrt", "_strupr", crt_strupr),
    C("msvcrt", "strupr", crt_strupr),
    C("msvcrt", "memcpy", crt_memcpy),
    C("msvcrt", "memmove", crt_memcpy),
    C("msvcrt", "memset", crt_memset),
    C("msvcrt", "memcmp", crt_memcmp),
    C("msvcrt", "memchr", crt_memchr),
    /* characters and numbers */
    C("msvcrt", "toupper", crt_toupper),
    C("msvcrt", "tolower", crt_tolower),
    C("msvcrt", "isalpha", crt_isalpha),
    C("msvcrt", "isdigit", crt_isdigit),
    C("msvcrt", "isspace", crt_isspace),
    C("msvcrt", "isalnum", crt_isalnum),
    C("msvcrt", "isupper", crt_isupper),
    C("msvcrt", "islower", crt_islower),
    C("msvcrt", "isxdigit", crt_isxdigit),
    C("msvcrt", "isprint", crt_isprint),
    C("msvcrt", "ispunct", crt_ispunct),
    C("msvcrt", "atoi", crt_atoi),
    C("msvcrt", "atol", crt_atoi),
    C("msvcrt", "atof", crt_atof),
    C("msvcrt", "strtol", crt_strtol),
    C("msvcrt", "strtoul", crt_strtoul),
    C("msvcrt", "strtod", crt_strtod),
    C("msvcrt", "_itoa", crt_itoa),
    C("msvcrt", "itoa", crt_itoa),
    C("msvcrt", "_ltoa", crt_itoa),
    C("msvcrt", "abs", crt_abs),
    C("msvcrt", "labs", crt_abs),
    /* formatted output */
    C("msvcrt", "sprintf", crt_sprintf_real),
    C("msvcrt", "_snprintf", crt_snprintf),
    C("msvcrt", "vsprintf", crt_vsprintf),
    C("msvcrt", "_vsnprintf", crt_vsnprintf),
    C("msvcrt", "printf", crt_printf),
    C("msvcrt", "puts", crt_puts),
    /* math */
    C("msvcrt", "sin", crt_sin),
    C("msvcrt", "cos", crt_cos),
    C("msvcrt", "tan", crt_tan),
    C("msvcrt", "asin", crt_asin),
    C("msvcrt", "acos", crt_acos),
    C("msvcrt", "atan", crt_atan),
    C("msvcrt", "atan2", crt_atan2),
    C("msvcrt", "sqrt", crt_sqrt),
    C("msvcrt", "pow", crt_pow),
    C("msvcrt", "exp", crt_exp),
    C("msvcrt", "log", crt_log),
    C("msvcrt", "log10", crt_log10),
    C("msvcrt", "fabs", crt_fabs),
    C("msvcrt", "floor", crt_floor),
    C("msvcrt", "ceil", crt_ceil),
    C("msvcrt", "fmod", crt_fmod),
    C("msvcrt", "_CIsin", crt_CIsin),
    C("msvcrt", "_CIcos", crt_CIcos),
    C("msvcrt", "_CItan", crt_CItan),
    C("msvcrt", "_CIasin", crt_CIasin),
    C("msvcrt", "_CIacos", crt_CIacos),
    C("msvcrt", "_CIatan", crt_CIatan),
    C("msvcrt", "_CIatan2", crt_CIatan2),
    C("msvcrt", "_CIsqrt", crt_CIsqrt),
    C("msvcrt", "_CIpow", crt_CIpow),
    C("msvcrt", "_CIexp", crt_CIexp),
    C("msvcrt", "_CIlog", crt_CIlog),
    C("msvcrt", "_CIlog10", crt_CIlog10),
    C("msvcrt", "_CIfmod", crt_CIfmod),
    C("msvcrt", "_ftol", crt_ftol),
    C("msvcrt", "_ftol2", crt_ftol),
    C("msvcrt", "_ftol2_sse", crt_ftol),
    C("msvcrt", "_controlfp", crt_controlfp),
    C("msvcrt", "_control87", crt_controlfp),
    C("msvcrt", "_clearfp", crt_ret0),
    C("msvcrt", "_fpreset", crt_ret0),
    C("msvcrt", "_isnan", crt_isnan),
    C("msvcrt", "_finite", crt_finite),
    C("msvcrt", "rand", crt_rand),
    C("msvcrt", "srand", crt_srand),
    C("msvcrt", "time", crt_time),
    C("msvcrt", "clock", crt_clock),
    /* startup, shutdown, callbacks */
    C("msvcrt", "_initterm", crt_initterm),
    C("msvcrt", "_onexit", crt_onexit),
    C("msvcrt", "__dllonexit", crt_dllonexit),
    C("msvcrt", "atexit", crt_atexit),
    C("msvcrt", "qsort", crt_qsort),
    C("msvcrt", "_purecall", crt_purecall),
    C("msvcrt", "_amsg_exit", crt_fatal),
    C("msvcrt", "exit", crt_fatal),
    C("msvcrt", "_exit", crt_fatal),
    C("msvcrt", "abort", crt_fatal),
    C("msvcrt", "_CxxThrowException", crt_fatal),
    C("msvcrt", "__CxxFrameHandler", crt_fatal),
    C("msvcrt", "_except_handler3", crt_fatal),
    C("msvcrt", "_XcptFilter", crt_fatal),
    C("msvcrt", "fopen", crt_ret0), /* no file access from scripts */
    C("msvcrt", "fclose", crt_ret0),
    D("msvcrt", "_adjust_fdiv", 4),
    D("msvcrt", "__mb_cur_max", 4),
    D("msvcrt", "_iob", 96),
    D("msvcrt", "_pctype", 4 + 2 * 257),
    D("msvcrt", "_HUGE", 8),
    /* kernel32 (stdcall) */
    S("kernel32", "GetModuleHandleA", 4, k32_GetModuleHandleA),
    S("kernel32", "GetProcAddress", 8, k32_GetProcAddress),
    S("kernel32", "LoadLibraryA", 4, k32_LoadLibraryA),
    S("kernel32", "FreeLibrary", 4, crt_ret1),
    S("kernel32", "DisableThreadLibraryCalls", 4, crt_ret1),
    S("kernel32", "GetTickCount", 0, k32_GetTickCount),
    S("kernel32", "QueryPerformanceCounter", 4, k32_QueryPerformanceCounter),
    S("kernel32", "QueryPerformanceFrequency", 4, k32_QueryPerformanceFrequency),
    S("kernel32", "Sleep", 4, crt_ret0),
    S("kernel32", "OutputDebugStringA", 4, k32_OutputDebugStringA),
    S("kernel32", "GetLastError", 0, k32_GetLastError),
    S("kernel32", "SetLastError", 4, k32_SetLastError),
    S("kernel32", "InterlockedIncrement", 4, k32_InterlockedIncrement),
    S("kernel32", "InterlockedDecrement", 4, k32_InterlockedDecrement),
    S("kernel32", "InterlockedExchange", 8, k32_InterlockedExchange),
    S("kernel32", "InterlockedCompareExchange", 12, k32_InterlockedCompareExchange),
    S("kernel32", "InterlockedExchangeAdd", 8, k32_InterlockedExchangeAdd),
    S("kernel32", "InitializeCriticalSection", 4, crt_ret0),
    S("kernel32", "InitializeCriticalSectionAndSpinCount", 8, crt_ret1),
    S("kernel32", "EnterCriticalSection", 4, crt_ret0),
    S("kernel32", "LeaveCriticalSection", 4, crt_ret0),
    S("kernel32", "DeleteCriticalSection", 4, crt_ret0),
    S("kernel32", "TlsAlloc", 0, k32_TlsAlloc),
    S("kernel32", "TlsFree", 4, k32_TlsFree),
    S("kernel32", "TlsGetValue", 4, k32_TlsGetValue),
    S("kernel32", "TlsSetValue", 8, k32_TlsSetValue),
    S("kernel32", "HeapCreate", 12, k32_HeapCreate),
    S("kernel32", "HeapDestroy", 4, crt_ret1),
    S("kernel32", "GetProcessHeap", 0, k32_GetProcessHeap),
    S("kernel32", "HeapAlloc", 12, k32_HeapAlloc),
    S("kernel32", "HeapFree", 12, k32_HeapFree),
    S("kernel32", "HeapReAlloc", 16, k32_HeapReAlloc),
    S("kernel32", "HeapSize", 12, k32_HeapSize),
    S("kernel32", "HeapValidate", 12, crt_ret1),
    S("kernel32", "VirtualAlloc", 16, k32_VirtualAlloc),
    S("kernel32", "VirtualFree", 12, k32_VirtualFree),
    S("kernel32", "GetVersion", 0, k32_GetVersion),
    S("kernel32", "GetVersionExA", 4, k32_GetVersionExA),
    S("kernel32", "GetCurrentThreadId", 0, k32_GetCurrentThreadId),
    S("kernel32", "GetCurrentProcessId", 0, k32_GetCurrentProcessId),
    S("kernel32", "GetCurrentProcess", 0, k32_GetCurrentProcess),
    S("kernel32", "GetCurrentThread", 0, k32_GetCurrentThread),
    S("kernel32", "CloseHandle", 4, crt_ret1),
    S("kernel32", "IsBadReadPtr", 8, k32_IsBadReadPtr),
    S("kernel32", "IsBadWritePtr", 8, k32_IsBadReadPtr),
    S("kernel32", "IsBadCodePtr", 4, k32_IsBadCodePtr),
    S("kernel32", "GetCommandLineA", 0, k32_GetCommandLineA),
    S("kernel32", "GetCommandLineW", 0, k32_GetCommandLineW),
    S("kernel32", "GetEnvironmentStrings", 0, k32_GetEnvironmentStrings),
    S("kernel32", "GetEnvironmentStringsA", 0, k32_GetEnvironmentStrings),
    S("kernel32", "GetEnvironmentStringsW", 0, k32_GetEnvironmentStringsW),
    S("kernel32", "FreeEnvironmentStringsA", 4, crt_ret1),
    S("kernel32", "FreeEnvironmentStringsW", 4, crt_ret1),
    S("kernel32", "GetEnvironmentVariableA", 12, crt_ret0),
    S("kernel32", "GetStartupInfoA", 4, k32_GetStartupInfoA),
    S("kernel32", "GetStdHandle", 4, k32_GetStdHandle),
    S("kernel32", "GetFileType", 4, k32_GetFileType),
    S("kernel32", "SetHandleCount", 4, k32_SetHandleCount),
    S("kernel32", "GetACP", 0, k32_GetACP),
    S("kernel32", "GetOEMCP", 0, k32_GetOEMCP),
    S("kernel32", "GetCPInfo", 8, k32_GetCPInfo),
    S("kernel32", "IsValidCodePage", 4, crt_ret1),
    S("kernel32", "MultiByteToWideChar", 24, k32_MultiByteToWideChar),
    S("kernel32", "WideCharToMultiByte", 32, k32_WideCharToMultiByte),
    S("kernel32", "LCMapStringA", 24, crt_ret0),
    S("kernel32", "LCMapStringW", 24, crt_ret0),
    S("kernel32", "GetStringTypeA", 20, crt_ret0),
    S("kernel32", "GetStringTypeW", 16, crt_ret0),
    S("kernel32", "GetModuleFileNameA", 12, k32_GetModuleFileNameA),
    S("kernel32", "ExitProcess", 4, crt_fatal),
    S("kernel32", "TerminateProcess", 8, crt_fatal),
    S("kernel32", "FatalAppExitA", 8, crt_fatal),
    S("kernel32", "RaiseException", 16, crt_fatal),
    S("kernel32", "RtlUnwind", 16, crt_fatal),
    S("kernel32", "UnhandledExceptionFilter", 4, crt_ret0),
    S("kernel32", "SetUnhandledExceptionFilter", 4, crt_ret0),
    S("kernel32", "WriteFile", 20, k32_WriteFile),
    S("kernel32", "FlushFileBuffers", 4, crt_ret1),
    S("kernel32", "SetFilePointer", 16, crt_ret0),
    S("kernel32", "SetStdHandle", 8, crt_ret1),
    S("kernel32", "GetSystemTimeAsFileTime", 4, k32_GetSystemTimeAsFileTime),
    S("kernel32", "GetLocalTime", 4, k32_GetLocalTime),
    S("kernel32", "GetSystemTime", 4, k32_GetLocalTime),
    S("kernel32", "IsProcessorFeaturePresent", 4, crt_ret0),
    S("kernel32", "IsDebuggerPresent", 0, crt_ret0),
    S("kernel32", "CreateFileA", 28, k32_CreateFileA),
    S("kernel32", "ReadFile", 20, crt_ret0),
    S("kernel32", "GetTimeZoneInformation", 4, k32_GetTimeZoneInformation),
    S("kernel32", "DebugBreak", 0, k32_DebugBreak),
    /* Locale and console functions the static MSVC runtime imports. There is one
       locale (the C locale): locale queries fail and setlocale reports that. */
    S("kernel32", "CompareStringA", 24, k32_CompareStringA),
    S("kernel32", "CompareStringW", 24, k32_CompareStringW),
    S("kernel32", "GetLocaleInfoA", 16, crt_ret0),
    S("kernel32", "GetLocaleInfoW", 16, crt_ret0),
    S("kernel32", "IsValidLocale", 8, crt_ret0),
    S("kernel32", "EnumSystemLocalesA", 8, crt_ret0),
    S("kernel32", "GetUserDefaultLCID", 0, k32_GetUserDefaultLCID),
    S("kernel32", "SetConsoleCtrlHandler", 8, crt_ret1),
    /* The environment is empty and stays empty; setting a variable reports success. */
    S("kernel32", "SetEnvironmentVariableA", 8, crt_ret1),
    S("kernel32", "SetEndOfFile", 4, crt_ret0),
    /* user32 */
    S("user32", "MessageBoxA", 16, user32_MessageBoxA),
    C("user32", "wsprintfA", user32_wsprintfA),
};

static uint32_t va_start_addr(int fixed) {
  /* cdecl: the first variadic argument follows the fixed ones on the stack. */
  uint32_t esp = sbcpu_reg(g_cpu, 4);
  return esp + 4 + 4u * (uint32_t)fixed;
}

const sb_import_def *sb_find_import(const char *dll, const char *name) {
  /* The C runtime may be msvcrt, msvcrt20, msvcr70 and so on, or crtdll (which
     GCC-built scripts use); treat them alike. Entries listed under crtdll exist
     only there. */
  bool crt = (dll[0] == 'm' && dll[1] == 's' && dll[2] == 'v' && dll[3] == 'c' && dll[4] == 'r') ||
             strcmp(dll, "crtdll") == 0;
  for (uint32_t i = 0; i < sizeof(g_table) / sizeof(g_table[0]); i++) {
    const sb_import_def *d = &g_table[i];
    if (strcmp(d->name, name) != 0)
      continue;
    if (strcmp(d->dll, dll) == 0 || (crt && strcmp(d->dll, "msvcrt") == 0))
      return d;
  }
  return NULL;
}

void sb_win32_init_data(const sb_import_def *def, uint32_t addr) {
  if (strcmp(def->name, "__mb_cur_max") == 0) {
    sb_write32(addr, 1);
  } else if (strcmp(def->name, "_pctype") == 0) {
    /* pointer to a classic C-locale ctype table (257 entries, the first for EOF) */
    uint32_t table = addr + 4;
    sb_write32(addr, table + 2);
    for (int c = 0; c < 256; c++) {
      uint16_t m = 0;
      if (c >= 'A' && c <= 'Z')
        m |= 0x1 | 0x100;
      if (c >= 'a' && c <= 'z')
        m |= 0x2 | 0x100;
      if (c >= '0' && c <= '9')
        m |= 0x4;
      if (c == ' ' || (c >= 9 && c <= 13))
        m |= 0x8;
      if (c > 32 && c < 127 && !(m & (0x100 | 0x4)))
        m |= 0x10;
      if (c < 32 || c == 127)
        m |= 0x20;
      if (c == ' ')
        m |= 0x40;
      if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
        m |= 0x80;
      g_mem[table + 2 + 2 * c] = (uint8_t)m;
      g_mem[table + 2 + 2 * c + 1] = (uint8_t)(m >> 8);
    }
  } else if (strcmp(def->name, "_HUGE") == 0) {
    sb_write32(addr, 0);
    sb_write32(addr + 4, 0x7ff00000u);
  }
}
