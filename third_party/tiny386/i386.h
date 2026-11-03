#ifndef I386_H
#define I386_H

#ifndef I386_IN_AMD64
#include <stdbool.h>
#include <stdint.h>

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

typedef u32 uword;
typedef s32 sword;
#else
#define CPU_CB CPUI386_CB
#define uword u32
#define sword s32
#endif

typedef struct CPUI386 CPUI386;

typedef struct {
	void *pic;
	int (*pic_read_irq)(void *);

	void *io;
	u8 (*io_read8)(void *, int);
	void (*io_write8)(void *, int, u8);
	u16 (*io_read16)(void *, int);
	void (*io_write16)(void *, int, u16);
	u32 (*io_read32)(void *, int);
	void (*io_write32)(void *, int, u32);
	int (*io_read_string)(void *, int, uint8_t *, int, int);
	int (*io_write_string)(void *, int, uint8_t *, int, int);

	void *iomem;
	u8 (*iomem_read8)(void *, uword);
	void (*iomem_write8)(void *, uword, u8);
	u16 (*iomem_read16)(void *, uword);
	void (*iomem_write16)(void *, uword, u16);
	u32 (*iomem_read32)(void *, uword);
	void (*iomem_write32)(void *, uword, u32);
	bool (*iomem_write_string)(void *, uword, uint8_t *, int);
} CPU_CB;

typedef struct {
	u32 gpr[8];
	u32 seg[8];
	u32 ip;
	u32 flags;
	u32 gdt_base, gdt_limit;
	u32 idt_base, idt_limit;
	u32 cr0, cr2, cr3;
	uint64_t efer;
	// TODO: fpu state...
} CPUI386_State;

void cpui386_get_state(CPUI386 *cpu, CPUI386_State *state);
int cpui386_get_excno(CPUI386 *cpu);

CPUI386 *cpui386_new(int gen, char *phys_mem, long phys_mem_size, CPU_CB **cb);
void cpui386_delete(CPUI386 *cpu);
void cpui386_enable_fpu(CPUI386 *cpu);
void cpui386_reset(CPUI386 *cpu);
void cpui386_reset_pm(CPUI386 *cpu, uint32_t start_addr);
void cpui386_step(CPUI386 *cpu, int stepcount);
void cpui386_raise_irq(CPUI386 *cpu);
void cpui386_set_gpr(CPUI386 *cpu, int i, u32 val);
long cpui386_get_cycle(CPUI386 *cpu);

#ifndef I386_IN_AMD64
bool cpu_load8(CPUI386 *cpu, int seg, uword addr, u8 *res);
bool cpu_store8(CPUI386 *cpu, int seg, uword addr, u8 val);
bool cpu_load16(CPUI386 *cpu, int seg, uword addr, u16 *res);
bool cpu_store16(CPUI386 *cpu, int seg, uword addr, u16 val);
bool cpu_load32(CPUI386 *cpu, int seg, uword addr, u32 *res);
bool cpu_store32(CPUI386 *cpu, int seg, uword addr, u32 val);
void cpu_setax(CPUI386 *cpu, u16 ax);
u16 cpu_getax(CPUI386 *cpu);
void cpu_setexc(CPUI386 *cpu, int excno, uword excerr);
void cpu_setflags(CPUI386 *cpu, uword set_mask, uword clear_mask);
uword cpu_getflags(CPUI386 *cpu);
void cpu_abort(CPUI386 *cpu, int code);
#else
#undef CPU_CB
#undef uword
#undef sword
#endif

#endif /* I386_H */
