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

/* Accessors for tiny386's CPU state. This file includes i386.c so it can reach
   the CPU struct, which tiny386 keeps private to that file. */

#define I386_ENABLE_FPU 1
#include "i386.c"

#include "sb_internal.h"

CPUI386 *sbcpu_new(char *mem, long mem_size) {
  CPUI386 *cpu = cpui386_new(5, mem, mem_size, NULL);
  if (!cpu)
    return NULL;
  cpui386_enable_fpu(cpu);
  if (!cpu->fpu)
    return NULL;
  return cpu;
}

static void set_flat(CPUI386 *cpu, int seg, uword sel, uword base, uword limit, uword flags) {
  cpu->seg[seg].sel = sel;
  cpu->seg[seg].base = base;
  cpu->seg[seg].limit = limit;
  cpu->seg[seg].flags = flags;
}

static uint32_t g_page_directory;

void sbcpu_set_page_directory(CPUI386 *cpu, uint32_t cr3) {
  g_page_directory = cr3;
  tlb_clear(cpu);
}

uint32_t sbcpu_fault_address(CPUI386 *cpu) { return cpu->cr2; }

/* Puts the CPU in 32-bit protected mode at privilege level 3 with flat segments
   and paging on, with the page tables sbcpu_set_page_directory set.
   There is no GDT or IDT: loading a segment register, a privileged instruction or a
   software interrupt raises an exception, which ends the call. IOPL is 3 so the
   guest's OUT instructions reach the sandbox. FS points at the fake TEB. */
void sbcpu_enter_user(CPUI386 *cpu, uint32_t eip, uint32_t esp, uint32_t fs_base) {
  const uword code = SEG_D_BIT | 0x5b | (3 << 5);
  const uword data = SEG_B_BIT | 0x53 | (3 << 5);
  cpu->cr0 = 1 | 0x10 | CR0_PG; /* PE, ET, paging */
  cpu->cr3 = g_page_directory;
  cpu->gdt.base = 0;
  cpu->gdt.limit = 0;
  cpu->idt.base = 0;
  cpu->idt.limit = 0;
  set_flat(cpu, SEG_CS, 0x1b, 0, 0xffffffff, code);
  set_flat(cpu, SEG_SS, 0x23, 0, 0xffffffff, data);
  set_flat(cpu, SEG_DS, 0x23, 0, 0xffffffff, data);
  set_flat(cpu, SEG_ES, 0x23, 0, 0xffffffff, data);
  set_flat(cpu, SEG_GS, 0x23, 0, 0xffffffff, data);
  set_flat(cpu, SEG_FS, 0x3b, fs_base, 0xfff, data);
  cpu->cpl = 3;
  cpu->code16 = false;
  cpu->sp_mask = 0xffffffff;
  cpu->halt = false;
  cpu->intr = false;
  cpu->cc.mask = 0;
  cpu->flags = 0x2 | IOPL; /* IF clear: there are no interrupts */
  for (int i = 0; i < 8; i++)
    REGi(i) = 0;
  REGi(4) = esp;
  cpu->ip = eip;
  cpu->next_ip = eip;
  cpu->ifetch.paddr = 0;
  cpu->excno = 0;
}

bool sbcpu_exec(CPUI386 *cpu, int steps, uint32_t *executed, int *excno) {
  long before = cpu->cycle;
  bool ok = cpu_exec1(cpu, steps);
  cpu->ifetch.paddr = 0;
  *executed = (uint32_t)(cpu->cycle - before);
  if (!ok) {
    *excno = cpu->excno;
    cpu->next_ip = cpu->ip;
  }
  return ok;
}

uint32_t sbcpu_reg(CPUI386 *cpu, int i) { return REGi(i & 7); }
void sbcpu_set_reg(CPUI386 *cpu, int i, uint32_t v) { REGi(i & 7) = v; }
uint32_t sbcpu_eip(CPUI386 *cpu) { return cpu->ip; }
void sbcpu_redirect(CPUI386 *cpu, uint32_t next_eip) {
  cpu->next_ip = next_eip;
  cpu->ifetch.paddr = 0;
}
FPU *sbcpu_fpu(CPUI386 *cpu) { return cpu->fpu; }
CPU_CB *sbcpu_callbacks(CPUI386 *cpu) { return &cpu->cb; }

size_t sbcpu_state_size(void) { return sizeof(CPUI386) + sbfpu_state_size(); }

void sbcpu_save(CPUI386 *cpu, void *buf) {
  memcpy(buf, cpu, sizeof(CPUI386));
  sbfpu_save(cpu->fpu, (char *)buf + sizeof(CPUI386));
}

void sbcpu_restore(CPUI386 *cpu, const void *buf) {
  memcpy(cpu, buf, sizeof(CPUI386));
  sbfpu_restore(cpu->fpu, (const char *)buf + sizeof(CPUI386));
  cpu->ifetch.paddr = 0;
}
