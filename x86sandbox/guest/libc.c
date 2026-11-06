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

/* Minimal C library for the sandbox's own code (not for the guest DLL).
   Memory comes from a fixed arena between __heap_base and the end of the Wasm
   memory; the memory never grows, so host pointers into it stay valid. */

#include <stdarg.h>
#include <stdint.h>

#include "libc/stdio.h"
#include "libc/stdlib.h"
#include "libc/string.h"
#include "libc/time.h"
#include "sb_internal.h"

extern unsigned char __heap_base;

/* ---- string.h ---- */
void *memcpy(void *d, const void *s, size_t n) {
  unsigned char *dp = d;
  const unsigned char *sp = s;
  while (n--)
    *dp++ = *sp++;
  return d;
}

void *memmove(void *d, const void *s, size_t n) {
  unsigned char *dp = d;
  const unsigned char *sp = s;
  if (dp == sp || n == 0)
    return d;
  if (dp < sp) {
    while (n--)
      *dp++ = *sp++;
  } else {
    dp += n;
    sp += n;
    while (n--)
      *--dp = *--sp;
  }
  return d;
}

void *memset(void *d, int c, size_t n) {
  unsigned char *dp = d;
  while (n--)
    *dp++ = (unsigned char)c;
  return d;
}

int memcmp(const void *a, const void *b, size_t n) {
  const unsigned char *x = a, *y = b;
  for (; n; n--, x++, y++)
    if (*x != *y)
      return *x - *y;
  return 0;
}

void *memchr(const void *s, int c, size_t n) {
  const unsigned char *p = s;
  for (; n; n--, p++)
    if (*p == (unsigned char)c)
      return (void *)p;
  return NULL;
}

size_t strlen(const char *s) {
  size_t n = 0;
  while (s[n])
    n++;
  return n;
}

int strcmp(const char *a, const char *b) {
  while (*a && *a == *b)
    a++, b++;
  return (unsigned char)*a - (unsigned char)*b;
}

/* ---- stdlib.h: first-fit allocator with boundary sizes ---- */
typedef struct block {
  size_t size; /* bytes after the header, multiple of 16 */
  struct block *next_free;
  int used;
  int pad;
} block;

static block *g_free_list;
static int g_heap_ready;

static void heap_init(void) {
  uintptr_t start = ((uintptr_t)&__heap_base + 15) & ~(uintptr_t)15;
  uintptr_t end = (uintptr_t)__builtin_wasm_memory_size(0) * 65536u;
  block *b = (block *)start;
  b->size = end - start - sizeof(block);
  b->next_free = NULL;
  b->used = 0;
  g_free_list = b;
  g_heap_ready = 1;
}

void *malloc(size_t n) {
  if (!g_heap_ready)
    heap_init();
  if (n == 0)
    n = 1;
  if (n > 0x7fffffff)
    return NULL;
  n = (n + 15) & ~(size_t)15;
  block **link = &g_free_list;
  for (block *b = g_free_list; b; link = &b->next_free, b = b->next_free) {
    if (b->size < n)
      continue;
    if (b->size >= n + sizeof(block) + 16) {
      block *rest = (block *)((char *)(b + 1) + n);
      rest->size = b->size - n - sizeof(block);
      rest->used = 0;
      rest->next_free = b->next_free;
      b->size = n;
      *link = rest;
    } else {
      *link = b->next_free;
    }
    b->used = 1;
    b->next_free = NULL;
    return b + 1;
  }
  return NULL;
}

void *calloc(size_t n, size_t m) {
  if (m && n > 0x7fffffff / m)
    return NULL;
  void *p = malloc(n * m);
  if (p)
    memset(p, 0, n * m);
  return p;
}

void free(void *p) {
  if (!p)
    return;
  block *b = (block *)p - 1;
  if (!b->used)
    __builtin_trap(); /* double free in sandbox code */
  b->used = 0;
  /* Keep the free list sorted by address and merge neighbours. */
  block **link = &g_free_list;
  while (*link && *link < b)
    link = &(*link)->next_free;
  b->next_free = *link;
  *link = b;
  if (b->next_free && (char *)(b + 1) + b->size == (char *)b->next_free) {
    b->size += sizeof(block) + b->next_free->size;
    b->next_free = b->next_free->next_free;
  }
  if (link != &g_free_list) {
    block *prev = (block *)((char *)link - offsetof(block, next_free));
    if ((char *)(prev + 1) + prev->size == (char *)b) {
      prev->size += sizeof(block) + b->size;
      prev->next_free = b->next_free;
    }
  }
}

_Noreturn void abort(void) { __builtin_trap(); }

/* ---- stdio.h: only used for tiny386 diagnostics ---- */
struct sb_file {
  int unused;
};
static struct sb_file g_stderr, g_stdout;
FILE *stderr = &g_stderr;
FILE *stdout = &g_stdout;

/* tiny386 prints CPU dumps to stderr on unexpected instructions. They are dropped:
   a hostile guest could otherwise flood the game's log. */
int fprintf(FILE *f, const char *fmt, ...) {
  (void)f;
  (void)fmt;
  return 0;
}

int printf(const char *fmt, ...) {
  char buf[256];
  va_list ap;
  va_start(ap, fmt);
  int n = sb_vformat(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  host_log(buf, (uint32_t)strlen(buf));
  return n;
}

/* ---- time.h ---- */
int clock_gettime(int clk, struct timespec *ts) {
  (void)clk;
  uint32_t ms = host_time_ms();
  ts->tv_sec = ms / 1000;
  ts->tv_nsec = (long)(ms % 1000) * 1000000L;
  return 0;
}

/* ---- math.h ---- */
double frexp(double x, int *e) {
  union {
    double d;
    uint64_t u;
  } v = {x};
  int ex = (int)((v.u >> 52) & 0x7ff);
  if (ex == 0) {
    if (x == 0) {
      *e = 0;
      return x;
    }
    v.d = x * 0x1p64;
    ex = (int)((v.u >> 52) & 0x7ff) - 64;
  } else if (ex == 0x7ff) {
    *e = 0;
    return x;
  }
  *e = ex - 1022;
  v.u = (v.u & 0x800fffffffffffffull) | (0x3feull << 52);
  return v.d;
}

/* ---- A small printf for the sandbox's own messages (no floating point). ---- */
static int put_uint(char *out, int cap, int pos, unsigned long long v, int base, int upper) {
  char tmp[24];
  int n = 0;
  const char *digits = upper ? "0123456789ABCDEF" : "0123456789abcdef";
  do {
    tmp[n++] = digits[v % base];
    v /= base;
  } while (v);
  while (n--) {
    if (pos < cap - 1)
      out[pos] = tmp[n];
    pos++;
  }
  return pos;
}

int sb_vformat(char *out, int cap, const char *fmt, __builtin_va_list ap) {
  int pos = 0;
  if (cap <= 0)
    return 0;
  for (const char *p = fmt; *p; p++) {
    if (*p != '%') {
      if (pos < cap - 1)
        out[pos] = *p;
      pos++;
      continue;
    }
    p++;
    int is_long = 0;
    while (*p == 'l') {
      is_long++;
      p++;
    }
    switch (*p) {
    case 'd': {
      long long v = is_long >= 2 ? va_arg(ap, long long) : is_long ? va_arg(ap, long) : va_arg(ap, int);
      if (v < 0) {
        if (pos < cap - 1)
          out[pos] = '-';
        pos++;
        v = -v;
      }
      pos = put_uint(out, cap, pos, (unsigned long long)v, 10, 0);
      break;
    }
    case 'u':
      pos = put_uint(out, cap, pos,
                     is_long >= 2 ? va_arg(ap, unsigned long long)
                     : is_long    ? va_arg(ap, unsigned long)
                                  : va_arg(ap, unsigned int),
                     10, 0);
      break;
    case 'x':
    case 'X':
      pos = put_uint(out, cap, pos,
                     is_long >= 2 ? va_arg(ap, unsigned long long)
                     : is_long    ? va_arg(ap, unsigned long)
                                  : va_arg(ap, unsigned int),
                     16, *p == 'X');
      break;
    case 's': {
      const char *s = va_arg(ap, const char *);
      if (!s)
        s = "(null)";
      while (*s) {
        if (pos < cap - 1)
          out[pos] = *s;
        pos++;
        s++;
      }
      break;
    }
    case 'c':
      if (pos < cap - 1)
        out[pos] = (char)va_arg(ap, int);
      pos++;
      break;
    case '%':
      if (pos < cap - 1)
        out[pos] = '%';
      pos++;
      break;
    case '\0':
      p--;
      break;
    default:
      break;
    }
  }
  out[pos < cap ? pos : cap - 1] = '\0';
  return pos;
}
