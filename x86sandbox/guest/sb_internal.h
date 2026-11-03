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
 * Internal declarations for the x86 sandbox guest.
 *
 * Everything under x86sandbox/guest is compiled to wasm32, then converted to C
 * with wasm2c (see x86sandbox/regenerate.sh). It runs a 32-bit Win32 DLL in the
 * tiny386 CPU emulator. Guest memory is one flat buffer inside the Wasm linear
 * memory; guest address 0 is the start of that buffer.
 */

#ifndef SB_INTERNAL_H
#define SB_INTERNAL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define SB_EXPORT(name) __attribute__((export_name(name)))
#define SB_IMPORT(name) __attribute__((import_module("env"), import_name(name)))

/* ---- Guest memory layout (guest addresses) ---- */
#define SB_STUB_BASE 0x00010000u    /* return/abort stubs, API stubs, import stubs */
#define SB_RET_STUB (SB_STUB_BASE + 0x0)
#define SB_ABORT_STUB (SB_STUB_BASE + 0x10)
#define SB_API_STUB_BASE (SB_STUB_BASE + 0x100)
#define SB_API_STUB_SIZE 16u
#define SB_MAX_API 256u
#define SB_IMPORT_STUB_BASE (SB_API_STUB_BASE + SB_MAX_API * SB_API_STUB_SIZE)
#define SB_IMPORT_STUB_SIZE 16u
#define SB_MAX_IMPORTS 1024u
#define SB_TEB_ADDR 0x00030000u
#define SB_PEB_ADDR 0x00031000u
#define SB_TLS_ADDR 0x00032000u     /* TLS slot array, 64 slots */
#define SB_DATA_BASE 0x00033000u    /* data imports (e.g. _adjust_fdiv), up to SB_DATA_END */
#define SB_DATA_END 0x00040000u
#define SB_STACK_LIMIT 0x00100000u  /* stack grows down from SB_STACK_TOP to here */
#define SB_STACK_TOP 0x00200000u
#define SB_IMAGE_BASE 0x00400000u   /* where DLLs with relocations are loaded */
/* 0x000A0000-0x000BFFFF is tiny386's VGA I/O window and is never used. */

/* ---- I/O ports used as the guest's exits ---- */
#define SB_PORT_HOSTCALL 0xE0 /* EAX = API index; the host performs the call */
#define SB_PORT_RETURN 0xE1   /* the function called by the host has returned */
#define SB_PORT_IMPORT 0xE2   /* EAX = import index; handled inside the sandbox */

/* ---- Status codes (also returned to the host) ---- */
enum {
  SB_OK = 0,
  SB_ERR_NOT_INITIALIZED = 1,
  SB_ERR_NO_MEMORY = 2,
  SB_ERR_BAD_IMAGE = 3,
  SB_ERR_EXCEPTION = 4,     /* CPU exception in guest code */
  SB_ERR_BUDGET = 5,        /* instruction budget exhausted */
  SB_ERR_ABORTED = 6,       /* the host or an import asked to stop the guest */
  SB_ERR_MEMFAULT = 7,      /* access to the I/O window */
  SB_ERR_DEAD = 8,          /* the module failed earlier and is no longer usable */
  SB_ERR_NESTING = 9,       /* too many nested calls */
  SB_ERR_MISSING_IMPORT = 10,
  SB_ERR_BAD_ARGS = 11,
};

/* ---- CPU glue (cpu_glue.c, compiled together with tiny386's i386.c) ---- */
typedef struct CPUI386 CPUI386;
typedef struct FPU FPU;

CPUI386 *sbcpu_new(char *mem, long mem_size);
void sbcpu_enter_user(CPUI386 *cpu, uint32_t eip, uint32_t esp, uint32_t fs_base);
/* Runs up to `steps` instructions. Returns false if a CPU exception stopped it. */
bool sbcpu_exec(CPUI386 *cpu, int steps, uint32_t *executed, int *excno);
uint32_t sbcpu_reg(CPUI386 *cpu, int i); /* 0=EAX 1=ECX 2=EDX 3=EBX 4=ESP 5=EBP 6=ESI 7=EDI */
void sbcpu_set_reg(CPUI386 *cpu, int i, uint32_t v);
uint32_t sbcpu_eip(CPUI386 *cpu);
void sbcpu_redirect(CPUI386 *cpu, uint32_t next_eip);
FPU *sbcpu_fpu(CPUI386 *cpu);
size_t sbcpu_state_size(void);
void sbcpu_save(CPUI386 *cpu, void *buf);
void sbcpu_restore(CPUI386 *cpu, const void *buf);

/* ---- FPU glue (fpu_glue.c, compiled together with tiny386's fpu.c) ---- */
size_t sbfpu_state_size(void);
void sbfpu_save(FPU *fpu, void *buf);
void sbfpu_restore(FPU *fpu, const void *buf);
void sbfpu_push(FPU *fpu, double v);
double sbfpu_st(FPU *fpu, int i);
void sbfpu_pop(FPU *fpu);
void sbfpu_set_st(FPU *fpu, int i, double v);
void sbfpu_clear_stack(FPU *fpu);
uint16_t sbfpu_cw(FPU *fpu);
void sbfpu_set_cw(FPU *fpu, uint16_t cw);

/* ---- Sandbox core (sandbox.c) ---- */
extern CPUI386 *g_cpu;
extern uint8_t *g_mem;
extern uint32_t g_mem_size;
extern uint32_t g_image_base, g_image_size;

/* Guest memory access. All return false (and do nothing) if the range is outside guest memory. */
bool sb_range_ok(uint32_t addr, uint32_t len);
void *sb_ptr(uint32_t addr, uint32_t len); /* NULL if out of range */
bool sb_read32(uint32_t addr, uint32_t *v);
bool sb_write32(uint32_t addr, uint32_t v);
bool sb_read8(uint32_t addr, uint8_t *v);
bool sb_write8(uint32_t addr, uint8_t v);
/* Length of the NUL-terminated string at addr, or -1 if it runs past the end of guest memory or max. */
int32_t sb_strlen(uint32_t addr, uint32_t max);

/* Stack arguments of the current import/API call: arg(0) is the first argument. */
uint32_t sb_arg(int i);
double sb_arg_f64(int i); /* a double occupying two slots starting at slot i */
float sb_arg_f32(int i);

/* Guest heap (inside guest memory). */
uint32_t sb_heap_alloc(uint32_t size);
void sb_heap_free(uint32_t addr);
uint32_t sb_heap_size(uint32_t addr);
uint32_t sb_heap_realloc(uint32_t addr, uint32_t size);

/* Stop the guest; the current sb_call returns `code`. */
void sb_abort(int code, const char *why);
/* Call a guest function from inside the sandbox (nested). Returns SB_OK or an error. */
int sb_call_guest(uint32_t fn, const uint32_t *args, int nargs, uint32_t *eax);

/* Diagnostics */
void sb_logf(const char *fmt, ...);
void sb_set_error(const char *fmt, ...);
int sb_vformat(char *out, int cap, const char *fmt, __builtin_va_list ap);

/* ---- Host imports ---- */
SB_IMPORT("host_call") int32_t host_call(uint32_t index, uint32_t esp);
SB_IMPORT("host_log") void host_log(const char *msg, uint32_t len);
SB_IMPORT("host_time_ms") uint32_t host_time_ms(void);
SB_IMPORT("fmt_double")
int32_t host_fmt_double(double v, int32_t conv, int32_t prec, int32_t flags, int32_t width, char *out,
                        int32_t cap);
SB_IMPORT("parse_double") double host_parse_double(const char *s, int32_t len, int32_t *consumed);
SB_IMPORT("m_sin") double sin(double);
SB_IMPORT("m_cos") double cos(double);
SB_IMPORT("m_tan") double tan(double);
SB_IMPORT("m_asin") double sb_asin(double);
SB_IMPORT("m_acos") double sb_acos(double);
SB_IMPORT("m_atan") double sb_atan(double);
SB_IMPORT("m_atan2") double atan2(double, double);
SB_IMPORT("m_pow") double pow(double, double);
SB_IMPORT("m_exp") double sb_exp(double);
SB_IMPORT("m_log") double sb_log(double);
SB_IMPORT("m_log10") double sb_log10(double);
SB_IMPORT("m_log2") double log2(double);
SB_IMPORT("m_fmod") double sb_fmod(double, double);

/* ---- Win32/CRT imports (win32.c) ---- */
typedef enum { SB_IMP_CDECL, SB_IMP_STDCALL } sb_conv;
typedef void (*sb_import_fn)(void);
typedef struct {
  const char *dll;  /* lowercase, without extension */
  const char *name;
  sb_conv conv;
  uint8_t arg_bytes;   /* stdcall: bytes popped by the callee */
  sb_import_fn fn;     /* NULL for data imports */
  uint32_t data_size;  /* data imports: size of the object to allocate */
} sb_import_def;

const sb_import_def *sb_find_import(const char *dll, const char *name);
void sb_win32_reset(void);
/* Return values from an import handler. */
void sb_ret(uint32_t eax);
void sb_ret64(uint32_t eax, uint32_t edx);
void sb_ret_f64(double v); /* pushes onto the guest FPU stack */
void sb_run_atexit(void);

#endif
