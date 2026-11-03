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
 * x86 sandbox core: guest memory, the call/return protocol, the PE loader and the
 * guest heap. One instance of the Wasm module holds one loaded DLL.
 *
 * Exits from guest code are OUT instructions to reserved ports (see sb_internal.h):
 *  - API stubs (the function table given to the DLL) do OUT SB_PORT_HOSTCALL; the
 *    host performs the call and the result goes back in EAX or ST0.
 *  - Import stubs (the DLL's Win32/CRT imports) do OUT SB_PORT_IMPORT and are
 *    handled by win32.c inside the sandbox.
 *  - Functions called from outside return to a stub that does OUT SB_PORT_RETURN
 *    followed by HLT, which faults at privilege level 3 and stops the CPU loop.
 */

#include <stdarg.h>

#include "libc/stdlib.h"
#include "libc/string.h"
#include "sb_internal.h"
#include "i386.h"

CPU_CB *sbcpu_callbacks(CPUI386 *cpu);
void sb_win32_init_data(const sb_import_def *def, uint32_t addr);

#define SB_CHUNK 4096
#define SB_MAX_DEPTH 16
#define SB_MIN_HEAP (4u << 20)
#define SB_DEFAULT_BUDGET 200000000u

CPUI386 *g_cpu;
uint8_t *g_mem;
uint32_t g_mem_size;
uint32_t g_image_base, g_image_size;

typedef struct {
  const sb_import_def *def;
  char name[64]; /* "dll!function" for diagnostics */
} import_slot;

static struct {
  bool initialized;
  bool loaded;
  bool dead;
  bool returned;
  bool memfault;
  uint32_t fault_eip;
  int abort_code;
  int depth;
  uint32_t budget_per_call;
  uint32_t budget_left;
  uint32_t cur_esp; /* ESP at the current OUT; arguments start at cur_esp + 4 */
  uint32_t res_eax, res_edx;
  double res_st0;
  uint32_t export_rva, export_size;
  uint32_t entry;
  uint32_t data_next;
  uint8_t *staging;
  uint32_t staging_cap;
  import_slot imports[SB_MAX_IMPORTS];
  uint32_t nimports;
  char error[512];
  char namebuf[256];
  uint32_t argbuf[32];
} S;

/* ------------------------------------------------------------------ */
/* Diagnostics                                                          */
/* ------------------------------------------------------------------ */

void sb_logf(const char *fmt, ...) {
  char buf[512];
  va_list ap;
  va_start(ap, fmt);
  sb_vformat(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  host_log(buf, (uint32_t)strlen(buf));
}

void sb_set_error(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  sb_vformat(S.error, sizeof(S.error), fmt, ap);
  va_end(ap);
}

/* ------------------------------------------------------------------ */
/* Guest memory                                                         */
/* ------------------------------------------------------------------ */

bool sb_range_ok(uint32_t addr, uint32_t len) {
  return g_mem && addr <= g_mem_size && len <= g_mem_size - addr;
}

void *sb_ptr(uint32_t addr, uint32_t len) { return sb_range_ok(addr, len) ? g_mem + addr : NULL; }

bool sb_read32(uint32_t addr, uint32_t *v) {
  if (!sb_range_ok(addr, 4))
    return false;
  memcpy(v, g_mem + addr, 4);
  return true;
}

bool sb_write32(uint32_t addr, uint32_t v) {
  if (!sb_range_ok(addr, 4))
    return false;
  memcpy(g_mem + addr, &v, 4);
  return true;
}

bool sb_read8(uint32_t addr, uint8_t *v) {
  if (!sb_range_ok(addr, 1))
    return false;
  *v = g_mem[addr];
  return true;
}

bool sb_write8(uint32_t addr, uint8_t v) {
  if (!sb_range_ok(addr, 1))
    return false;
  g_mem[addr] = v;
  return true;
}

int32_t sb_strlen(uint32_t addr, uint32_t max) {
  for (uint32_t i = 0; i < max; i++) {
    if (addr + i < addr || addr + i >= g_mem_size)
      return -1;
    if (g_mem[addr + i] == 0)
      return (int32_t)i;
  }
  return -1;
}

uint32_t sb_arg(int i) {
  uint32_t v = 0;
  if (!sb_read32(S.cur_esp + 4 + 4u * (uint32_t)i, &v))
    sb_abort(SB_ERR_BAD_ARGS, "argument outside guest memory");
  return v;
}

double sb_arg_f64(int i) {
  uint32_t lo = sb_arg(i), hi = sb_arg(i + 1);
  uint64_t bits = ((uint64_t)hi << 32) | lo;
  double d;
  memcpy(&d, &bits, 8);
  return d;
}

float sb_arg_f32(int i) {
  uint32_t bits = sb_arg(i);
  float f;
  memcpy(&f, &bits, 4);
  return f;
}

/* ------------------------------------------------------------------ */
/* Guest heap. The block list lives in sandbox memory, not guest memory, */
/* so the guest cannot corrupt the allocator's bookkeeping.             */
/* ------------------------------------------------------------------ */

typedef struct {
  uint32_t addr, size;
  uint32_t used;
} hblock;

static hblock *g_blocks;
static uint32_t g_nblocks, g_cap_blocks;

static bool heap_reserve(uint32_t n) {
  if (n <= g_cap_blocks)
    return true;
  uint32_t cap = g_cap_blocks ? g_cap_blocks * 2 : 256;
  while (cap < n)
    cap *= 2;
  hblock *nb = malloc(cap * sizeof(hblock));
  if (!nb)
    return false;
  if (g_blocks) {
    memcpy(nb, g_blocks, g_nblocks * sizeof(hblock));
    free(g_blocks);
  }
  g_blocks = nb;
  g_cap_blocks = cap;
  return true;
}

static void heap_init(uint32_t start, uint32_t end) {
  g_nblocks = 0;
  if (!heap_reserve(1) || end <= start)
    return;
  g_blocks[0].addr = start;
  g_blocks[0].size = end - start;
  g_blocks[0].used = 0;
  g_nblocks = 1;
}

static int heap_find(uint32_t addr) {
  int lo = 0, hi = (int)g_nblocks - 1;
  while (lo <= hi) {
    int mid = (lo + hi) / 2;
    if (g_blocks[mid].addr == addr)
      return mid;
    if (g_blocks[mid].addr < addr)
      lo = mid + 1;
    else
      hi = mid - 1;
  }
  return -1;
}

uint32_t sb_heap_alloc(uint32_t size) {
  if (size == 0)
    size = 1;
  if (size > 0x7fffffffu)
    return 0;
  size = (size + 15u) & ~15u;
  for (uint32_t i = 0; i < g_nblocks; i++) {
    hblock *b = &g_blocks[i];
    if (b->used || b->size < size)
      continue;
    if (b->size > size) {
      if (!heap_reserve(g_nblocks + 1))
        return 0;
      b = &g_blocks[i];
      memmove(&g_blocks[i + 2], &g_blocks[i + 1], (g_nblocks - i - 1) * sizeof(hblock));
      g_blocks[i + 1].addr = b->addr + size;
      g_blocks[i + 1].size = b->size - size;
      g_blocks[i + 1].used = 0;
      g_nblocks++;
      b->size = size;
    }
    b->used = 1;
    memset(g_mem + b->addr, 0, size);
    return b->addr;
  }
  return 0;
}

static void heap_merge(int i) {
  if (i < 0 || (uint32_t)i + 1 >= g_nblocks)
    return;
  if (!g_blocks[i].used && !g_blocks[i + 1].used) {
    g_blocks[i].size += g_blocks[i + 1].size;
    memmove(&g_blocks[i + 1], &g_blocks[i + 2], (g_nblocks - i - 2) * sizeof(hblock));
    g_nblocks--;
  }
}

void sb_heap_free(uint32_t addr) {
  if (addr == 0)
    return;
  int i = heap_find(addr);
  if (i < 0 || !g_blocks[i].used) {
    sb_logf("x86sandbox: guest freed an invalid pointer %x", addr);
    return;
  }
  g_blocks[i].used = 0;
  heap_merge(i);
  heap_merge(i - 1);
}

uint32_t sb_heap_size(uint32_t addr) {
  int i = heap_find(addr);
  return (i >= 0 && g_blocks[i].used) ? g_blocks[i].size : 0;
}

uint32_t sb_heap_realloc(uint32_t addr, uint32_t size) {
  if (addr == 0)
    return sb_heap_alloc(size);
  if (size == 0) {
    sb_heap_free(addr);
    return 0;
  }
  uint32_t old = sb_heap_size(addr);
  if (old == 0)
    return 0;
  if (old >= size)
    return addr;
  uint32_t n = sb_heap_alloc(size);
  if (!n)
    return 0;
  memcpy(g_mem + n, g_mem + addr, old);
  sb_heap_free(addr);
  return n;
}

/* ------------------------------------------------------------------ */
/* Calls into guest code                                                */
/* ------------------------------------------------------------------ */

void sb_abort(int code, const char *why) {
  if (!S.abort_code) {
    S.abort_code = code;
    sb_set_error("%s", why);
  }
  if (g_cpu)
    sbcpu_redirect(g_cpu, SB_ABORT_STUB);
}

static int run(void) {
  for (;;) {
    if (S.abort_code)
      return S.abort_code;
    if (S.budget_left == 0) {
      sb_set_error("instruction budget exhausted at EIP %x", sbcpu_eip(g_cpu));
      return SB_ERR_BUDGET;
    }
    int steps = S.budget_left < SB_CHUNK ? (int)S.budget_left : SB_CHUNK;
    uint32_t done = 0;
    int exc = 0;
    bool ok = sbcpu_exec(g_cpu, steps, &done, &exc);
    S.budget_left -= done < S.budget_left ? done : S.budget_left;
    if (S.returned) {
      S.returned = false;
      return SB_OK;
    }
    if (S.abort_code)
      return S.abort_code;
    if (S.memfault) {
      sb_set_error("guest accessed the I/O window at EIP %x", S.fault_eip);
      return SB_ERR_MEMFAULT;
    }
    if (!ok) {
      sb_set_error("CPU exception %d at EIP %x", exc, sbcpu_eip(g_cpu));
      return SB_ERR_EXCEPTION;
    }
  }
}

int sb_call_guest(uint32_t fn, const uint32_t *args, int nargs, uint32_t *eax) {
  if (!S.loaded && !S.initialized)
    return SB_ERR_NOT_INITIALIZED;
  if (S.dead)
    return SB_ERR_DEAD;
  if (S.depth >= SB_MAX_DEPTH)
    return SB_ERR_NESTING;
  if (nargs < 0 || nargs > 32)
    return SB_ERR_BAD_ARGS;

  bool nested = S.depth > 0;
  void *saved = NULL;
  uint32_t saved_esp = S.cur_esp;
  uint32_t esp = SB_STACK_TOP;
  if (nested) {
    saved = malloc(sbcpu_state_size());
    if (!saved)
      return SB_ERR_NO_MEMORY;
    sbcpu_save(g_cpu, saved);
    esp = (sbcpu_reg(g_cpu, 4) - 256u) & ~15u;
  } else {
    S.budget_left = S.budget_per_call;
    S.abort_code = 0;
    S.memfault = false;
  }

  int rc = SB_OK;
  if (esp < SB_STACK_LIMIT + 4096u + 4u * (uint32_t)nargs || esp > SB_STACK_TOP) {
    rc = SB_ERR_NESTING;
    sb_set_error("guest stack exhausted by nested calls");
  } else {
    esp -= 4u * (uint32_t)nargs;
    for (int i = 0; i < nargs; i++)
      sb_write32(esp + 4u * (uint32_t)i, args[i]);
    esp -= 4;
    sb_write32(esp, SB_RET_STUB);
    sbcpu_enter_user(g_cpu, fn, esp, SB_TEB_ADDR);
    sbfpu_clear_stack(sbcpu_fpu(g_cpu));
    S.returned = false;
    S.depth++;
    rc = run();
    S.depth--;
  }

  S.res_eax = sbcpu_reg(g_cpu, 0);
  S.res_edx = sbcpu_reg(g_cpu, 2);
  S.res_st0 = sbfpu_st(sbcpu_fpu(g_cpu), 0);
  if (eax)
    *eax = S.res_eax;

  if (nested) {
    sbcpu_restore(g_cpu, saved);
    free(saved);
    S.cur_esp = saved_esp;
  }
  if (rc != SB_OK) {
    S.dead = true;
    if (nested) {
      /* The outer guest frame cannot continue either. */
      int code = rc;
      S.abort_code = 0;
      char msg[sizeof(S.error)];
      memcpy(msg, S.error, sizeof(msg));
      sb_abort(code, msg);
    }
  }
  return rc;
}

/* ------------------------------------------------------------------ */
/* I/O callbacks from tiny386                                           */
/* ------------------------------------------------------------------ */

static void handle_hostcall(uint32_t index) {
  uint32_t outer_esp = S.cur_esp;
  S.cur_esp = sbcpu_reg(g_cpu, 4);
  int32_t r = host_call(index, S.cur_esp);
  if (!S.abort_code)
    sbcpu_set_reg(g_cpu, 0, (uint32_t)r);
  S.cur_esp = outer_esp;
}

static void handle_import(uint32_t index) {
  if (index >= S.nimports) {
    sb_abort(SB_ERR_ABORTED, "bad import index");
    return;
  }
  const import_slot *slot = &S.imports[index];
  if (!slot->def || !slot->def->fn) {
    char msg[128];
    char *p = msg;
    const char *a = "called unsupported import ";
    while (*a)
      *p++ = *a++;
    for (const char *b = slot->name; *b && p < msg + sizeof(msg) - 1; b++)
      *p++ = *b;
    *p = 0;
    sb_abort(SB_ERR_MISSING_IMPORT, msg);
    return;
  }
  uint32_t outer_esp = S.cur_esp;
  S.cur_esp = sbcpu_reg(g_cpu, 4);
  slot->def->fn();
  S.cur_esp = outer_esp;
}

void sb_ret(uint32_t eax) {
  if (!S.abort_code)
    sbcpu_set_reg(g_cpu, 0, eax);
}

void sb_ret64(uint32_t eax, uint32_t edx) {
  if (!S.abort_code) {
    sbcpu_set_reg(g_cpu, 0, eax);
    sbcpu_set_reg(g_cpu, 2, edx);
  }
}

void sb_ret_f64(double v) {
  if (!S.abort_code)
    sbfpu_push(sbcpu_fpu(g_cpu), v);
}

static u8 io_read8(void *o, int port) {
  (void)o;
  (void)port;
  return 0xff;
}
static u16 io_read16(void *o, int port) {
  (void)o;
  (void)port;
  return 0xffff;
}
static u32 io_read32(void *o, int port) {
  (void)o;
  (void)port;
  return 0xffffffffu;
}
static void io_write8(void *o, int port, u8 v) {
  (void)o;
  (void)port;
  (void)v;
  sb_abort(SB_ERR_ABORTED, "8-bit OUT");
}
static void io_write16(void *o, int port, u16 v) {
  (void)o;
  (void)port;
  (void)v;
  sb_abort(SB_ERR_ABORTED, "16-bit OUT");
}
static void io_write32(void *o, int port, u32 v) {
  (void)o;
  switch (port) {
  case SB_PORT_HOSTCALL:
    handle_hostcall(v);
    break;
  case SB_PORT_RETURN:
    S.returned = true;
    break;
  case SB_PORT_IMPORT:
    handle_import(v);
    break;
  default:
    sb_abort(SB_ERR_ABORTED, "OUT to an unknown port");
    break;
  }
}
static int io_read_string(void *o, int port, uint8_t *buf, int size, int count) {
  (void)o;
  (void)port;
  (void)buf;
  (void)size;
  (void)count;
  sb_abort(SB_ERR_ABORTED, "string I/O");
  return 0;
}
static int io_write_string(void *o, int port, uint8_t *buf, int size, int count) {
  return io_read_string(o, port, buf, size, count);
}
static u8 mem_read8(void *o, uword a) {
  (void)o;
  (void)a;
  if (!S.memfault)
    S.fault_eip = sbcpu_eip(g_cpu);
  S.memfault = true;
  sbcpu_redirect(g_cpu, SB_ABORT_STUB);
  return 0;
}
static void mem_write8(void *o, uword a, u8 v) {
  (void)v;
  mem_read8(o, a);
}
static u16 mem_read16(void *o, uword a) { return mem_read8(o, a); }
static void mem_write16(void *o, uword a, u16 v) {
  (void)v;
  mem_read8(o, a);
}
static u32 mem_read32(void *o, uword a) { return mem_read8(o, a); }
static void mem_write32(void *o, uword a, u32 v) {
  (void)v;
  mem_read8(o, a);
}
static bool mem_write_string(void *o, uword a, uint8_t *buf, int len) {
  (void)buf;
  (void)len;
  mem_read8(o, a);
  return true;
}
static int pic_read_irq(void *o) {
  (void)o;
  return -1;
}

static void install_callbacks(void) {
  CPU_CB *cb = sbcpu_callbacks(g_cpu);
  cb->pic = NULL;
  cb->pic_read_irq = pic_read_irq;
  cb->io = NULL;
  cb->io_read8 = io_read8;
  cb->io_write8 = io_write8;
  cb->io_read16 = io_read16;
  cb->io_write16 = io_write16;
  cb->io_read32 = io_read32;
  cb->io_write32 = io_write32;
  cb->io_read_string = io_read_string;
  cb->io_write_string = io_write_string;
  cb->iomem = NULL;
  cb->iomem_read8 = mem_read8;
  cb->iomem_write8 = mem_write8;
  cb->iomem_read16 = mem_read16;
  cb->iomem_write16 = mem_write16;
  cb->iomem_read32 = mem_read32;
  cb->iomem_write32 = mem_write32;
  cb->iomem_write_string = mem_write_string;
}

/* ------------------------------------------------------------------ */
/* Stubs and the fake TEB                                               */
/* ------------------------------------------------------------------ */

static void put_bytes(uint32_t addr, const uint8_t *b, uint32_t n) { memcpy(g_mem + addr, b, n); }

static void write_stub(uint32_t addr, uint8_t port, uint32_t index, uint16_t pop_bytes) {
  uint8_t code[12] = {0xB8, (uint8_t)index, (uint8_t)(index >> 8), (uint8_t)(index >> 16), (uint8_t)(index >> 24),
                      0xE7, port};
  uint32_t n = 7;
  if (pop_bytes) {
    code[n++] = 0xC2;
    code[n++] = (uint8_t)pop_bytes;
    code[n++] = (uint8_t)(pop_bytes >> 8);
  } else {
    code[n++] = 0xC3;
  }
  put_bytes(addr, code, n);
}

static void write_fixed_code(void) {
  /* HLT at privilege level 3 raises #GP quietly, which stops the CPU loop. */
  static const uint8_t ret_stub[] = {0xE7, SB_PORT_RETURN, 0xF4};
  static const uint8_t abort_stub[] = {0xF4};
  put_bytes(SB_RET_STUB, ret_stub, sizeof(ret_stub));
  put_bytes(SB_ABORT_STUB, abort_stub, sizeof(abort_stub));
  for (uint32_t i = 0; i < SB_MAX_API; i++)
    write_stub(SB_API_STUB_BASE + i * SB_API_STUB_SIZE, SB_PORT_HOSTCALL, i, 0);

  /* TEB */
  sb_write32(SB_TEB_ADDR + 0x00, 0xffffffffu); /* end of the SEH chain */
  sb_write32(SB_TEB_ADDR + 0x04, SB_STACK_TOP);
  sb_write32(SB_TEB_ADDR + 0x08, SB_STACK_LIMIT);
  sb_write32(SB_TEB_ADDR + 0x18, SB_TEB_ADDR);
  sb_write32(SB_TEB_ADDR + 0x20, 0x100); /* process id */
  sb_write32(SB_TEB_ADDR + 0x24, 0x104); /* thread id */
  sb_write32(SB_TEB_ADDR + 0x2c, SB_TLS_ADDR);
  sb_write32(SB_TEB_ADDR + 0x30, SB_PEB_ADDR);
  sb_write32(SB_PEB_ADDR + 0x18, 0x00050000u); /* ProcessHeap handle (see win32.c) */
}

/* ------------------------------------------------------------------ */
/* PE loader                                                            */
/* ------------------------------------------------------------------ */

static uint16_t rd16(const uint8_t *p) { return (uint16_t)(p[0] | (p[1] << 8)); }
static uint32_t rd32(const uint8_t *p) { return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24); }

static bool img32(uint32_t rva, uint32_t *v) {
  if (rva > g_image_size || g_image_size - rva < 4)
    return false;
  return sb_read32(g_image_base + rva, v);
}

/* Copies the NUL-terminated string at image RVA `rva` into out. */
static bool img_str(uint32_t rva, char *out, uint32_t cap) {
  for (uint32_t i = 0; i < cap; i++) {
    if (rva + i >= g_image_size)
      return false;
    out[i] = (char)g_mem[g_image_base + rva + i];
    if (!out[i])
      return true;
  }
  return false;
}

static void lowercase_dll(char *s) {
  for (char *p = s; *p; p++)
    if (*p >= 'A' && *p <= 'Z')
      *p = (char)(*p - 'A' + 'a');
  uint32_t n = (uint32_t)strlen(s);
  if (n > 4 && strcmp(s + n - 4, ".dll") == 0)
    s[n - 4] = 0;
}

static int bad(const char *why) {
  sb_set_error("bad image: %s", why);
  return SB_ERR_BAD_IMAGE;
}

static uint32_t new_import(const char *dll, const char *name, const sb_import_def *def) {
  if (S.nimports >= SB_MAX_IMPORTS)
    return 0;
  uint32_t idx = S.nimports++;
  import_slot *slot = &S.imports[idx];
  slot->def = def;
  uint32_t p = 0;
  for (const char *a = dll; *a && p < sizeof(slot->name) - 2; a++)
    slot->name[p++] = *a;
  slot->name[p++] = '!';
  for (const char *a = name; *a && p < sizeof(slot->name) - 1; a++)
    slot->name[p++] = *a;
  slot->name[p] = 0;

  if (def && !def->fn) {
    /* Data import: the IAT gets the address of an object in guest memory. */
    uint32_t size = (def->data_size + 15u) & ~15u;
    if (size == 0 || S.data_next + size > SB_DATA_END)
      return 0;
    uint32_t addr = S.data_next;
    S.data_next += size;
    return addr;
  }
  uint32_t stub = SB_IMPORT_STUB_BASE + idx * SB_IMPORT_STUB_SIZE;
  write_stub(stub, SB_PORT_IMPORT, idx, (def && def->conv == SB_IMP_STDCALL) ? def->arg_bytes : 0);
  return stub;
}

static int bind_imports(uint32_t dir_rva, uint32_t dir_size) {
  if (dir_rva == 0 || dir_size == 0)
    return SB_OK;
  for (uint32_t d = dir_rva;; d += 20) {
    uint32_t oft, name_rva, ft;
    if (!img32(d + 0, &oft) || !img32(d + 12, &name_rva) || !img32(d + 16, &ft))
      return bad("import directory out of range");
    if (oft == 0 && name_rva == 0 && ft == 0)
      break;
    char dll[64];
    if (!img_str(name_rva, dll, sizeof(dll)))
      return bad("import DLL name");
    lowercase_dll(dll);
    uint32_t lookup = oft ? oft : ft;
    for (uint32_t i = 0;; i++) {
      uint32_t entry;
      if (!img32(lookup + 4 * i, &entry))
        return bad("import lookup table");
      if (entry == 0)
        break;
      char fname[64];
      const sb_import_def *def = NULL;
      if (entry & 0x80000000u) {
        uint32_t ord = entry & 0xffff;
        uint32_t n = 0;
        fname[n++] = '#';
        char tmp[8];
        uint32_t t = 0;
        do {
          tmp[t++] = (char)('0' + ord % 10);
          ord /= 10;
        } while (ord);
        while (t)
          fname[n++] = tmp[--t];
        fname[n] = 0;
      } else {
        if (!img_str(entry + 2, fname, sizeof(fname)))
          return bad("import name");
        def = sb_find_import(dll, fname);
      }
      uint32_t target = new_import(dll, fname, def);
      if (!target)
        return bad("too many imports");
      if (def && !def->fn)
        sb_win32_init_data(def, target);
      if (ft + 4 * i >= g_image_size || g_image_size - (ft + 4 * i) < 4)
        return bad("import address table");
      sb_write32(g_image_base + ft + 4 * i, target);
    }
  }
  return SB_OK;
}

static int apply_relocations(uint32_t dir_rva, uint32_t dir_size, uint32_t delta) {
  uint32_t off = 0;
  while (off + 8 <= dir_size) {
    uint32_t page, block;
    if (!img32(dir_rva + off, &page) || !img32(dir_rva + off + 4, &block))
      return bad("relocation block");
    if (block < 8 || block > dir_size - off)
      return bad("relocation block size");
    for (uint32_t e = 8; e + 2 <= block; e += 2) {
      uint32_t at = g_image_base + dir_rva + off + e;
      if (!sb_range_ok(at, 2))
        return bad("relocation entry");
      uint16_t v = rd16(g_mem + at);
      uint32_t type = v >> 12, rva = page + (v & 0xfff);
      if (type == 0)
        continue;
      if (type != 3)
        return bad("unsupported relocation type");
      uint32_t word;
      if (!img32(rva, &word))
        return bad("relocation target");
      sb_write32(g_image_base + rva, word + delta);
    }
    off += block;
  }
  return SB_OK;
}

static int load_pe(const uint8_t *f, uint32_t len) {
#define NEED(off, n)                                                                                         \
  if ((uint64_t)(off) + (uint64_t)(n) > len)                                                                 \
  return bad("truncated file")
  NEED(0, 64);
  if (f[0] != 'M' || f[1] != 'Z')
    return bad("no MZ header");
  uint32_t pe = rd32(f + 0x3c);
  NEED(pe, 24);
  if (memcmp(f + pe, "PE\0\0", 4) != 0)
    return bad("no PE signature");
  if (rd16(f + pe + 4) != 0x14c)
    return bad("not a 32-bit x86 image");
  uint32_t nsec = rd16(f + pe + 6), opt_size = rd16(f + pe + 20);
  uint32_t opt = pe + 24;
  NEED(opt, opt_size);
  if (opt_size < 96 || rd16(f + opt) != 0x10b)
    return bad("not a PE32 optional header");
  uint32_t entry_rva = rd32(f + opt + 16), pref_base = rd32(f + opt + 28);
  uint32_t size_image = rd32(f + opt + 56), size_headers = rd32(f + opt + 60);
  uint32_t ndirs = rd32(f + opt + 92);
  if (ndirs > 16)
    ndirs = 16;
  if (opt_size < 96 + 8 * ndirs)
    return bad("optional header too small");
  uint32_t dir[16][2] = {{0}};
  for (uint32_t i = 0; i < ndirs; i++) {
    dir[i][0] = rd32(f + opt + 96 + 8 * i);
    dir[i][1] = rd32(f + opt + 100 + 8 * i);
  }

  uint32_t max_image = g_mem_size - SB_IMAGE_BASE - SB_MIN_HEAP;
  if (size_image == 0 || size_image > max_image)
    return bad("image too large");
  if (size_headers > size_image || size_headers > len)
    return bad("headers");

  uint32_t base;
  if (dir[5][1] != 0) {
    base = SB_IMAGE_BASE;
  } else if (pref_base >= SB_IMAGE_BASE && pref_base <= g_mem_size - SB_MIN_HEAP - size_image) {
    base = pref_base;
  } else {
    return bad("no relocations and the preferred base is outside guest memory");
  }
  g_image_base = base;
  g_image_size = size_image;
  memset(g_mem + base, 0, size_image);
  memcpy(g_mem + base, f, size_headers);

  uint32_t sec = opt + opt_size;
  NEED(sec, 40 * nsec);
  for (uint32_t i = 0; i < nsec; i++) {
    const uint8_t *s = f + sec + 40 * i;
    uint32_t vsize = rd32(s + 8), va = rd32(s + 12), raw = rd32(s + 16), rawptr = rd32(s + 20);
    uint32_t n = raw;
    if (vsize && vsize < n)
      n = vsize;
    if (n == 0)
      continue;
    if (va > size_image || n > size_image - va)
      return bad("section outside the image");
    NEED(rawptr, n);
    memcpy(g_mem + base + va, f + rawptr, n);
  }

  if (base != pref_base) {
    int rc = apply_relocations(dir[5][0], dir[5][1], base - pref_base);
    if (rc != SB_OK)
      return rc;
  }
  int rc = bind_imports(dir[1][0], dir[1][1]);
  if (rc != SB_OK)
    return rc;
  if (dir[9][1] != 0)
    sb_logf("x86sandbox: image has a TLS directory; TLS callbacks are not run");

  S.export_rva = dir[0][0];
  S.export_size = dir[0][1];
  S.entry = entry_rva ? base + entry_rva : 0;
  sb_write32(SB_PEB_ADDR + 0x08, base);

  uint32_t heap_start = (base + size_image + 0x1ffffu) & ~0xffffu;
  heap_init(heap_start, g_mem_size);
  return SB_OK;
#undef NEED
}

static uint32_t find_export(const char *name) {
  if (!S.loaded || S.export_rva == 0)
    return 0;
  uint32_t nfuncs, nnames, funcs, names, ords;
  if (!img32(S.export_rva + 20, &nfuncs) || !img32(S.export_rva + 24, &nnames) ||
      !img32(S.export_rva + 28, &funcs) || !img32(S.export_rva + 32, &names) ||
      !img32(S.export_rva + 36, &ords))
    return 0;
  for (uint32_t i = 0; i < nnames && i < 65536; i++) {
    uint32_t name_rva;
    char buf[128];
    if (!img32(names + 4 * i, &name_rva) || !img_str(name_rva, buf, sizeof(buf)))
      continue;
    if (strcmp(buf, name) != 0)
      continue;
    if (ords + 2 * i >= g_image_size)
      return 0;
    uint16_t ord = rd16(g_mem + g_image_base + ords + 2 * i);
    uint32_t rva;
    if (ord >= nfuncs || !img32(funcs + 4u * ord, &rva))
      return 0;
    if (rva >= S.export_rva && rva < S.export_rva + S.export_size)
      return 0; /* forwarded export: not supported */
    if (rva == 0 || rva >= g_image_size)
      return 0;
    return g_image_base + rva;
  }
  return 0;
}

/* ------------------------------------------------------------------ */
/* Exports to the host                                                  */
/* ------------------------------------------------------------------ */

SB_EXPORT("sb_init") int32_t sb_init(uint32_t mem_size) {
  if (S.initialized)
    return SB_OK;
  if (mem_size < SB_IMAGE_BASE + 2 * SB_MIN_HEAP || mem_size > 0x40000000u)
    return SB_ERR_BAD_ARGS;
  g_mem = calloc(1, mem_size);
  if (!g_mem)
    return SB_ERR_NO_MEMORY;
  g_mem_size = mem_size;
  g_cpu = sbcpu_new((char *)g_mem, (long)mem_size);
  if (!g_cpu)
    return SB_ERR_NO_MEMORY;
  install_callbacks();
  sbfpu_set_cw(sbcpu_fpu(g_cpu), 0x027f); /* Win32 default: 53-bit precision, exceptions masked */
  write_fixed_code();
  S.budget_per_call = SB_DEFAULT_BUDGET;
  S.data_next = SB_DATA_BASE;
  sb_win32_reset();
  S.initialized = true;
  return SB_OK;
}

SB_EXPORT("sb_guest_base") uint32_t sb_guest_base(void) { return (uint32_t)(uintptr_t)g_mem; }
SB_EXPORT("sb_guest_size") uint32_t sb_guest_size(void) { return g_mem_size; }
SB_EXPORT("sb_error") const char *sb_error(void) { return S.error; }
SB_EXPORT("sb_namebuf") char *sb_namebuf(void) { return S.namebuf; }
SB_EXPORT("sb_argbuf") uint32_t *sb_argbuf(void) { return S.argbuf; }
SB_EXPORT("sb_is_dead") int32_t sb_is_dead(void) { return S.dead; }
SB_EXPORT("sb_set_budget") void sb_set_budget(uint32_t n) { S.budget_per_call = n ? n : 1; }
SB_EXPORT("sb_api_stub") uint32_t sb_api_stub(uint32_t index) {
  return index < SB_MAX_API ? SB_API_STUB_BASE + index * SB_API_STUB_SIZE : 0;
}

SB_EXPORT("sb_staging") uint8_t *sb_staging(uint32_t len) {
  if (len == 0 || len > 16u * 1024 * 1024)
    return NULL;
  if (S.staging && S.staging_cap >= len)
    return S.staging;
  free(S.staging);
  S.staging = malloc(len);
  S.staging_cap = S.staging ? len : 0;
  return S.staging;
}

SB_EXPORT("sb_load") int32_t sb_load(uint32_t len) {
  if (!S.initialized)
    return SB_ERR_NOT_INITIALIZED;
  if (S.loaded || S.dead)
    return SB_ERR_DEAD;
  if (!S.staging || len > S.staging_cap)
    return SB_ERR_BAD_ARGS;
  int rc = load_pe(S.staging, len);
  free(S.staging);
  S.staging = NULL;
  S.staging_cap = 0;
  if (rc != SB_OK) {
    S.dead = true;
    return rc;
  }
  S.loaded = true;
  if (S.entry) {
    uint32_t args[3] = {g_image_base, 1 /* DLL_PROCESS_ATTACH */, 0};
    uint32_t ok = 0;
    rc = sb_call_guest(S.entry, args, 3, &ok);
    if (rc != SB_OK)
      return rc;
    if (!ok) {
      S.dead = true;
      sb_set_error("DllMain returned FALSE");
      return SB_ERR_ABORTED;
    }
  }
  return SB_OK;
}

SB_EXPORT("sb_find_export") uint32_t sb_find_export(void) {
  S.namebuf[sizeof(S.namebuf) - 1] = 0;
  return find_export(S.namebuf);
}

SB_EXPORT("sb_call") int32_t sb_call(uint32_t fn, int32_t nargs) {
  if (!S.loaded)
    return SB_ERR_NOT_INITIALIZED;
  if (fn < g_image_base || fn >= g_image_base + g_image_size)
    return SB_ERR_BAD_ARGS;
  return sb_call_guest(fn, S.argbuf, nargs, NULL);
}

SB_EXPORT("sb_call_any") int32_t sb_call_any(uint32_t fn, int32_t nargs) {
  /* Like sb_call, for guest function pointers the host received from the guest
     (callbacks). Any address is allowed; a bad one faults inside the guest. */
  if (!S.loaded)
    return SB_ERR_NOT_INITIALIZED;
  return sb_call_guest(fn, S.argbuf, nargs, NULL);
}

SB_EXPORT("sb_eax") uint32_t sb_eax(void) { return S.res_eax; }
SB_EXPORT("sb_edx") uint32_t sb_edx(void) { return S.res_edx; }
SB_EXPORT("sb_st0") double sb_st0(void) { return S.res_st0; }

SB_EXPORT("sb_alloc") uint32_t sb_alloc(uint32_t size) { return S.loaded ? sb_heap_alloc(size) : 0; }
SB_EXPORT("sb_free") void sb_free(uint32_t addr) {
  if (S.loaded)
    sb_heap_free(addr);
}

/* Called by the host while it handles host_call(). */
SB_EXPORT("sb_push_f64") void sb_push_f64(double v) { sb_ret_f64(v); }
SB_EXPORT("sb_set_edx") void sb_set_edx(uint32_t v) {
  if (!S.abort_code)
    sbcpu_set_reg(g_cpu, 2, v);
}
SB_EXPORT("sb_host_abort") void sb_host_abort(int32_t code) {
  sb_abort(code ? code : SB_ERR_ABORTED, "stopped by the host");
}
SB_EXPORT("sb_import_count") uint32_t sb_import_count(void) { return S.nimports; }
SB_EXPORT("sb_import_name") const char *sb_import_name(uint32_t i) { return i < S.nimports ? S.imports[i].name : NULL; }
SB_EXPORT("sb_import_supported") int32_t sb_import_supported(uint32_t i) {
  return i < S.nimports && S.imports[i].def != NULL;
}

SB_EXPORT("sb_unload") int32_t sb_unload(void) {
  if (!S.loaded || S.dead)
    return SB_OK;
  sb_run_atexit();
  if (S.entry && !S.dead) {
    uint32_t args[3] = {g_image_base, 0 /* DLL_PROCESS_DETACH */, 0};
    return sb_call_guest(S.entry, args, 3, NULL);
  }
  return SB_OK;
}
