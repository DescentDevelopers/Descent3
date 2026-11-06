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

/* C interface to the wasm2c-generated sandbox module. Functions returning int
   return 0, or a wasm2c trap code if the call trapped. */

#ifndef X86SANDBOX_RT_H
#define X86SANDBOX_RT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct w2c_env;
typedef struct x86sb_instance x86sb_instance;

x86sb_instance *x86sb_new(struct w2c_env *env);
int x86sb_instantiate(x86sb_instance *inst, struct w2c_env *env);
void x86sb_delete(x86sb_instance *inst);

int x86sb_init(x86sb_instance *inst, uint32_t mem_size, int32_t *rc);
int x86sb_load(x86sb_instance *inst, uint32_t len, int32_t *rc);
int x86sb_call(x86sb_instance *inst, uint32_t fn, int32_t nargs, int any_address, int32_t *rc);
int x86sb_unload(x86sb_instance *inst, int32_t *rc);
int x86sb_find_export(x86sb_instance *inst, uint32_t *addr);
int x86sb_alloc(x86sb_instance *inst, uint32_t size, uint32_t *addr);
int x86sb_free(x86sb_instance *inst, uint32_t addr);

uint8_t *x86sb_memory(x86sb_instance *inst, uint64_t *size);
uint32_t x86sb_guest_base(x86sb_instance *inst);
uint32_t x86sb_guest_size(x86sb_instance *inst);
uint32_t x86sb_staging(x86sb_instance *inst, uint32_t len);
uint32_t x86sb_namebuf(x86sb_instance *inst);
uint32_t x86sb_argbuf(x86sb_instance *inst);
uint32_t x86sb_error(x86sb_instance *inst);
int x86sb_is_dead(x86sb_instance *inst);
void x86sb_set_budget(x86sb_instance *inst, uint32_t n);
uint32_t x86sb_api_stub(x86sb_instance *inst, uint32_t i);
uint32_t x86sb_eax(x86sb_instance *inst);
uint32_t x86sb_edx(x86sb_instance *inst);
double x86sb_st0(x86sb_instance *inst);
void x86sb_push_f64(x86sb_instance *inst, double v);
void x86sb_set_edx(x86sb_instance *inst, uint32_t v);
void x86sb_host_abort(x86sb_instance *inst, int32_t code);
uint32_t x86sb_import_count(x86sb_instance *inst);
uint32_t x86sb_import_name(x86sb_instance *inst, uint32_t i);
int x86sb_import_supported(x86sb_instance *inst, uint32_t i);

#ifdef __cplusplus
}
#endif

#endif
