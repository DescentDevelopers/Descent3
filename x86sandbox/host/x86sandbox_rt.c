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

/* Every call into the wasm2c module that can trap goes through a function in this
   file. A trap longjmps back here, so no C++ frame is ever skipped by it. Calls may
   nest (guest -> host_call -> guest), so each wrapper saves and restores the
   runtime's jump buffer around its own try. */

#include <stdlib.h>
#include <string.h>

#include "wasm-rt-exceptions.h"
#include "wasm-rt-impl.h"
#include "x86sandbox_rt.h"
#include "x86sandbox_wasm.h"

struct x86sb_instance {
  w2c_x86sandbox module;
};

static int g_runtime_ready;

#define GUARDED(stmt)                                                                                        \
  wasm_rt_jmp_buf saved_buf;                                                                                 \
  memcpy(&saved_buf, &g_wasm_rt_jmp_buf, sizeof(saved_buf));                                                 \
  uint32_t saved_depth = wasm_rt_saved_call_stack_depth;                                                     \
  int trap = (int)wasm_rt_impl_try();                                                                        \
  if (trap == WASM_RT_TRAP_NONE) {                                                                           \
    stmt;                                                                                                    \
  }                                                                                                          \
  memcpy(&g_wasm_rt_jmp_buf, &saved_buf, sizeof(saved_buf));                                                 \
  wasm_rt_saved_call_stack_depth = saved_depth;                                                              \
  return trap

x86sb_instance *x86sb_new(struct w2c_env *env) {
  if (!g_runtime_ready) {
    wasm_rt_init();
    g_runtime_ready = 1;
  }
  x86sb_instance *inst = (x86sb_instance *)calloc(1, sizeof(x86sb_instance));
  if (!inst)
    return NULL;
  if (x86sb_instantiate(inst, env) != 0) {
    free(inst);
    return NULL;
  }
  return inst;
}

int x86sb_instantiate(x86sb_instance *inst, struct w2c_env *env) {
  GUARDED(wasm2c_x86sandbox_instantiate(&inst->module, env));
}

void x86sb_delete(x86sb_instance *inst) {
  if (!inst)
    return;
  wasm2c_x86sandbox_free(&inst->module);
  free(inst);
}

int x86sb_init(x86sb_instance *inst, uint32_t mem_size, int32_t *rc) {
  GUARDED(*rc = (int32_t)w2c_x86sandbox_sb_init(&inst->module, mem_size));
}

int x86sb_load(x86sb_instance *inst, uint32_t len, int32_t *rc) {
  GUARDED(*rc = (int32_t)w2c_x86sandbox_sb_load(&inst->module, len));
}

int x86sb_call(x86sb_instance *inst, uint32_t fn, int32_t nargs, int any_address, int32_t *rc) {
  GUARDED(*rc = (int32_t)(any_address ? w2c_x86sandbox_sb_call_any(&inst->module, fn, (uint32_t)nargs)
                                      : w2c_x86sandbox_sb_call(&inst->module, fn, (uint32_t)nargs)));
}

int x86sb_unload(x86sb_instance *inst, int32_t *rc) {
  GUARDED(*rc = (int32_t)w2c_x86sandbox_sb_unload(&inst->module));
}

int x86sb_find_export(x86sb_instance *inst, uint32_t *addr) {
  GUARDED(*addr = w2c_x86sandbox_sb_find_export(&inst->module));
}

int x86sb_alloc(x86sb_instance *inst, uint32_t size, uint32_t *addr) {
  GUARDED(*addr = w2c_x86sandbox_sb_alloc(&inst->module, size));
}

int x86sb_free(x86sb_instance *inst, uint32_t addr) { GUARDED(w2c_x86sandbox_sb_free(&inst->module, addr)); }

/* The functions below only read or write small fields of the sandbox state and
   cannot trap; they are called without a guard. */

uint8_t *x86sb_memory(x86sb_instance *inst, uint64_t *size) {
  wasm_rt_memory_t *m = w2c_x86sandbox_memory(&inst->module);
  *size = m->size;
  return m->data;
}

uint32_t x86sb_guest_base(x86sb_instance *inst) { return w2c_x86sandbox_sb_guest_base(&inst->module); }
uint32_t x86sb_guest_size(x86sb_instance *inst) { return w2c_x86sandbox_sb_guest_size(&inst->module); }
uint32_t x86sb_staging(x86sb_instance *inst, uint32_t len) { return w2c_x86sandbox_sb_staging(&inst->module, len); }
uint32_t x86sb_namebuf(x86sb_instance *inst) { return w2c_x86sandbox_sb_namebuf(&inst->module); }
uint32_t x86sb_argbuf(x86sb_instance *inst) { return w2c_x86sandbox_sb_argbuf(&inst->module); }
uint32_t x86sb_error(x86sb_instance *inst) { return w2c_x86sandbox_sb_error(&inst->module); }
int x86sb_is_dead(x86sb_instance *inst) { return (int)w2c_x86sandbox_sb_is_dead(&inst->module); }
void x86sb_set_budget(x86sb_instance *inst, uint32_t n) { w2c_x86sandbox_sb_set_budget(&inst->module, n); }
uint32_t x86sb_api_stub(x86sb_instance *inst, uint32_t i) { return w2c_x86sandbox_sb_api_stub(&inst->module, i); }
uint32_t x86sb_eax(x86sb_instance *inst) { return w2c_x86sandbox_sb_eax(&inst->module); }
uint32_t x86sb_edx(x86sb_instance *inst) { return w2c_x86sandbox_sb_edx(&inst->module); }
double x86sb_st0(x86sb_instance *inst) { return w2c_x86sandbox_sb_st0(&inst->module); }
void x86sb_push_f64(x86sb_instance *inst, double v) { w2c_x86sandbox_sb_push_f64(&inst->module, v); }
void x86sb_set_edx(x86sb_instance *inst, uint32_t v) { w2c_x86sandbox_sb_set_edx(&inst->module, v); }
void x86sb_host_abort(x86sb_instance *inst, int32_t code) {
  w2c_x86sandbox_sb_host_abort(&inst->module, (uint32_t)code);
}
uint32_t x86sb_import_count(x86sb_instance *inst) { return w2c_x86sandbox_sb_import_count(&inst->module); }
uint32_t x86sb_import_name(x86sb_instance *inst, uint32_t i) {
  return w2c_x86sandbox_sb_import_name(&inst->module, i);
}
int x86sb_import_supported(x86sb_instance *inst, uint32_t i) {
  return (int)w2c_x86sandbox_sb_import_supported(&inst->module, i);
}
