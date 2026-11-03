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

/* Accessors for tiny386's x87 state. This file includes fpu.c so it can reach the
   FPU struct, which tiny386 keeps private to that file. */

#define I386_ENABLE_FPU 1
#include "fpu.c"

#include "sb_internal.h"

size_t sbfpu_state_size(void) { return sizeof(FPU); }
void sbfpu_save(FPU *fpu, void *buf) { memcpy(buf, fpu, sizeof(FPU)); }
void sbfpu_restore(FPU *fpu, const void *buf) { memcpy(fpu, buf, sizeof(FPU)); }
void sbfpu_push(FPU *fpu, double v) { fppush(fpu, v); }
double sbfpu_st(FPU *fpu, int i) { return fpget(fpu, i & 7); }
void sbfpu_pop(FPU *fpu) { fppop(fpu); }
void sbfpu_set_st(FPU *fpu, int i, double v) { fpset(fpu, i & 7, v); }
void sbfpu_clear_stack(FPU *fpu) {
  fpu->top = 0;
  fpu->rawtagr = 0;
  fpu->rawtagw = 0;
}
uint16_t sbfpu_cw(FPU *fpu) { return fpu->cw; }
void sbfpu_set_cw(FPU *fpu, uint16_t cw) { fpu->cw = cw; }
