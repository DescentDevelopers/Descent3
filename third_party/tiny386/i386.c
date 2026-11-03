#include "i386.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#ifdef BUILD_ESP32
#include "esp_attr.h"
#define noinline __attribute__((noinline))
#else
#define IRAM_ATTR
#define IRAM_ATTR_CPU_EXEC1
#define DRAM_ATTR
#define noinline
#endif

#define I386_OPT1
#ifndef __wasm__
#define I386_OPT2
#endif

#ifdef I386_ENABLE_FPU
#include "fpu.h"
#else
#define fpu_new(...) NULL
#define fpu_exec1(...) false
#define fpu_exec2(...) false
#define fpu_delete(...)
typedef void FPU;
#endif

struct CPUI386 {
#ifdef I386_OPT1
	union {
		u32 r32;
		u16 r16;
		u8 r8[2];
	} gprx[8];
#else
	uword gpr[8];
#endif
	uword ip, next_ip;
	uword flags;
	uword flags_mask;
	int cpl;
	bool code16;
	uword sp_mask;
	bool halt;

	FPU *fpu;

	struct {
		uword sel;
		uword base;
		uword limit;
		uword flags;
	} seg[8];

	struct {
		uword base;
		uword limit;
	} idt, gdt;

	uword cr0, cr2, cr3;

	uword dr[8];

	struct {
		unsigned long laddr;
		uword xaddr;
		uword paddr;
	} ifetch;

	struct {
		int op;
		uword dst;
		uword dst2;
		uword src1;
		uword src2;
		uword mask;
	} cc;

	struct {
		int size;
		struct tlb_entry {
			uword lpgno;
			uword xaddr;
			int (*pte_lookup)[2];
			u8 *ppte;
		} *tab;
	} tlb;

	u8 *phys_mem;
	long phys_mem_size;

	long cycle;

	int excno;
	uword excerr;

	bool intr;
	CPU_CB cb;

	int gen;
	struct {
		uword cs, eip, esp;
	} sysenter;

#ifdef AMD64_ENABLE_LEG32
	struct {
		u32 efer;
	} lm;
#define INIT_LM_EFER(cpu) ((cpu)->lm.efer = 0)
#define GET_LM_EFER(cpu) ((cpu)->lm.efer)
#define SET_LM_EFER(cpu, x) ((cpu)->lm.efer = (x))
#define CPUID_LM0_2 0x2048
#define CPUID_LM80_0 0x80000004
#define CPUID_LM81_2 ((1<<29) | (1<<11))
#else
#define INIT_LM_EFER(cpu)
#define GET_LM_EFER(cpu) 0
#define SET_LM_EFER(cpu, x) THROW(EX_GP, 0)
#define CPUID_LM0_2 0
#define CPUID_LM80_0 0
#define CPUID_LM81_2 0
#endif
};

#define dolog(...) fprintf(stderr, __VA_ARGS__)
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#define wordmask ((uword) ((sword) -1))
#define TRY(f) if(!(f)) { return false; }
#define TRYL(f) if(unlikely(!(f))) { return false; }
#define TRY1(f) if(unlikely(!(f))) { dolog("@ %s %s %d\n", __FILE__, __FUNCTION__, __LINE__); cpu_abort(cpu, -1); }
#define THROW(ex, err) do { cpu->excno = (ex); cpu->excerr = (err); return false; } while(0)
#define THROW0(ex) do { cpu->excno = (ex); return false; } while(0)

// the second branchless version works better on gcc
//#define SET_BIT(w, f, m) ((w) ^= ((-(uword)(f)) ^ (w)) & (m))
#define SET_BIT(w, f, m) ((w) = ((w) & ~((uword)(m))) | ((-(uword)(f)) & (m)))
//#define SET_BIT(w, f, m) do { if (f) (w) |= (m); else (w) &= ~(m); } while (0)

enum {
	EX_DE,
	EX_DB,
	EX_NMI,
	EX_BP,
	EX_OF,
	EX_BR,
	EX_UD,
	EX_NM,
	EX_DF,
	EX_INT9,
	EX_TS,
	EX_NP,
	EX_SS,
	EX_GP,
	EX_PF,
};

enum {
	CF = 0x1,
	/* 1 0x2 */
	PF = 0x4,
	/* 0 0x8 */
	AF = 0x10,
	/* 0 0x20 */
	ZF = 0x40,
	SF = 0x80,
	TF = 0x100,
	IF = 0x200,
	DF = 0x400,
	OF = 0x800,
	IOPL = 0x3000,
	NT = 0x4000,
	/* 0 0x8000 */
	RF = 0x10000,
	VM = 0x20000,
};

enum {
	SEG_ES = 0,
	SEG_CS,
	SEG_SS,
	SEG_DS,
	SEG_FS,
	SEG_GS,
	SEG_LDT,
	SEG_TR,
};

enum {
	SEG_D_BIT = 1 << 14,
	SEG_B_BIT = 1 << 14,
};

#ifdef I386_OPT1
#define REGi(x) (cpu->gprx[x].r32)
#else
#define REGi(x) (cpu->gpr[x])
#endif
#define SEGi(x) (cpu->seg[x].sel)

static void cpu_debug(CPUI386 *cpu);

void cpu_abort(CPUI386 *cpu, int code)
{
	dolog("abort: %d %x cycle %ld\n", code, code, cpu->cycle);
	cpu_debug(cpu);
	abort();
}

static uword sext8(u8 a)
{
	return (sword) (s8) a;
}

static uword sext16(u16 a)
{
	return (sword) (s16) a;
}

static uword sext32(u32 a)
{
	return (sword) (s32) a;
}

static inline u8 pload8(CPUI386 *cpu, uword addr)
{
	return cpu->phys_mem[addr];
}

static inline void pstore8(CPUI386 *cpu, uword addr, u8 val)
{
	cpu->phys_mem[addr] = val;
}

#ifdef I386_OPT1
/* only works on hosts that are little-endian; where unaligned loads and
   stores trap or are slow, the packed accesses below let the compiler take
   them apart (on MIPS always, elsewhere with -DI386_ALIGNED_ONLY) */
#if defined(__mips__) || defined(I386_ALIGNED_ONLY)
static inline u16 pload16(CPUI386 *cpu, uword addr)
{
	const struct { u16 v; } __attribute__((packed))
		*q = (void *) &(cpu->phys_mem[addr]);
	return q->v;
}

static inline u32 pload32(CPUI386 *cpu, uword addr)
{
	const struct { u32 v; } __attribute__((packed))
		*q = (void *) &(cpu->phys_mem[addr]);
	return q->v;
}

static inline void pstore16(CPUI386 *cpu, uword addr, u16 val)
{
	struct { u16 v; } __attribute__((packed))
		*q = (void *) &(cpu->phys_mem[addr]);
	q->v = val;
}

static inline void pstore32(CPUI386 *cpu, uword addr, u32 val)
{
	struct { u32 v; } __attribute__((packed))
		*q = (void *) &(cpu->phys_mem[addr]);
	q->v = val;
}
#else
static inline u16 pload16(CPUI386 *cpu, uword addr)
{
	return *(u16 *)&(cpu->phys_mem[addr]);
}

static inline u32 pload32(CPUI386 *cpu, uword addr)
{
	return *(u32 *)&(cpu->phys_mem[addr]);
}

static inline void pstore16(CPUI386 *cpu, uword addr, u16 val)
{
	*(u16 *)&(cpu->phys_mem[addr]) = val;
}

static inline void pstore32(CPUI386 *cpu, uword addr, u32 val)
{
	*(u32 *)&(cpu->phys_mem[addr]) = val;
}
#endif
#else
static inline u16 pload16(CPUI386 *cpu, uword addr)
{
	u8 *mem = (u8 *) cpu->phys_mem;
	return mem[addr] | (mem[addr + 1] << 8);
}

static inline u32 pload32(CPUI386 *cpu, uword addr)
{
	u8 *mem = (u8 *) cpu->phys_mem;
	return mem[addr] | (mem[addr + 1] << 8) |
		(mem[addr + 2] << 16) | (mem[addr + 3] << 24);
}

static inline void pstore16(CPUI386 *cpu, uword addr, u16 val)
{
	cpu->phys_mem[addr] = val;
	cpu->phys_mem[addr + 1] = val >> 8;
}

static inline void pstore32(CPUI386 *cpu, uword addr, u32 val)
{
	cpu->phys_mem[addr] = val;
	cpu->phys_mem[addr + 1] = val >> 8;
	cpu->phys_mem[addr + 2] = val >> 16;
	cpu->phys_mem[addr + 3] = val >> 24;
}
#endif

/* lazy flags */
enum {
	CC_ADC, CC_ADD,	CC_SBB, CC_SUB,
	CC_NEG8, CC_NEG16, CC_NEG32,
	CC_DEC8, CC_DEC16, CC_DEC32,
	CC_INC8, CC_INC16, CC_INC32,
	CC_IMUL8, CC_IMUL16, CC_IMUL32,	CC_MUL8, CC_MUL16, CC_MUL32,
	CC_SAR, CC_SHL, CC_SHR,
	CC_SHLD, CC_SHRD, CC_BSF, CC_BSR,
	CC_AND, CC_OR, CC_XOR,
};

static int get_CF(CPUI386 *cpu)
{
	if (cpu->cc.mask & CF) {
		switch(cpu->cc.op) {
		case CC_ADC:
			return cpu->cc.dst <= cpu->cc.src2;
		case CC_ADD:
			return cpu->cc.dst < cpu->cc.src2;
		case CC_SBB:
			return cpu->cc.src1 <= cpu->cc.src2;
		case CC_SUB:
			return cpu->cc.src1 < cpu->cc.src2;
		case CC_NEG8: case CC_NEG16: case CC_NEG32:
			return cpu->cc.dst != 0;
		case CC_DEC8: case CC_DEC16: case CC_DEC32:
		case CC_INC8: case CC_INC16: case CC_INC32:
			assert(false); // should not happen
		case CC_IMUL8:
			return sext8(cpu->cc.dst) != cpu->cc.dst;
		case CC_IMUL16:
			return sext16(cpu->cc.dst) != cpu->cc.dst;
		case CC_IMUL32:
			return (((s32) cpu->cc.dst) >> 31) != cpu->cc.dst2;
		case CC_MUL8:
			return (cpu->cc.dst >> 8) != 0;
		case CC_MUL16:
			return (cpu->cc.dst >> 16) != 0;
		case CC_MUL32:
			return (cpu->cc.dst2) != 0;
		case CC_SHL:
		case CC_SHR:
		case CC_SAR:
			return cpu->cc.dst2 & 1;
		case CC_SHLD:
			return cpu->cc.dst2 >> 31;
		case CC_SHRD:
			return cpu->cc.dst2 & 1;
		case CC_BSF:
		case CC_BSR:
			return 0;
		case CC_AND:
		case CC_OR:
		case CC_XOR:
			return 0;
		}
	} else {
		return !!(cpu->flags & CF);
	}
	assert(false);
}

const static u8 parity_tab[256] = {
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
  1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1
};

static int get_PF(CPUI386 *cpu)
{
	if (cpu->cc.mask & PF) {
		return parity_tab[cpu->cc.dst & 0xff];
	} else {
		return !!(cpu->flags & PF);
	}
}

static int get_AF(CPUI386 *cpu)
{
	if (cpu->cc.mask & AF) {
		switch(cpu->cc.op) {
		case CC_ADC:
		case CC_ADD:
		case CC_SBB:
		case CC_SUB:
			return ((cpu->cc.src1 ^ cpu->cc.src2 ^ cpu->cc.dst) >> 4) & 1;
		case CC_NEG8: case CC_NEG16: case CC_NEG32:
			return (cpu->cc.dst & 0xf) != 0;
		case CC_DEC8: case CC_DEC16: case CC_DEC32:
			return (cpu->cc.dst & 0xf) == 0xf;
		case CC_INC8: case CC_INC16: case CC_INC32:
			return (cpu->cc.dst & 0xf) == 0;
		case CC_IMUL8: case CC_IMUL16: case CC_IMUL32:
		case CC_MUL8: case CC_MUL16: case CC_MUL32:
			return 0;
		case CC_SAR:
		case CC_SHL:
		case CC_SHR:
		case CC_SHLD:
		case CC_SHRD:
		case CC_BSF:
		case CC_BSR:
		case CC_AND:
		case CC_OR:
		case CC_XOR:
			return 0;
		}
	} else {
		return !!(cpu->flags & AF);
	}
	assert(false);
}

static int IRAM_ATTR get_ZF(CPUI386 *cpu)
{
	if (cpu->cc.mask & ZF) {
		return cpu->cc.dst == 0;
	} else {
		return !!(cpu->flags & ZF);
	}
}

static int IRAM_ATTR get_SF(CPUI386 *cpu)
{
	if (cpu->cc.mask & SF) {
		return cpu->cc.dst >> (sizeof(uword) * 8 - 1);
	} else {
		return !!(cpu->flags & SF);
	}
}

static int get_OF(CPUI386 *cpu)
{
	if (cpu->cc.mask & OF) {
		switch(cpu->cc.op) {
		case CC_ADC:
		case CC_ADD:
			return (~(cpu->cc.src1 ^ cpu->cc.src2) & (cpu->cc.dst ^ cpu->cc.src2)) >> (sizeof(uword) * 8 - 1);
		case CC_SBB:
		case CC_SUB:
			return ((cpu->cc.src1 ^ cpu->cc.src2) & (cpu->cc.dst ^ cpu->cc.src1)) >> (sizeof(uword) * 8 - 1);
		case CC_DEC8:
			return cpu->cc.dst == sext8((u8) ~(1u << 7));
		case CC_DEC16:
			return cpu->cc.dst == sext16((u16) ~(1u << 15));
		case CC_DEC32:
			return cpu->cc.dst == sext32((u32) ~(1u << 31));
		case CC_INC8: case CC_NEG8:
			return cpu->cc.dst == sext8(1u << 7);
		case CC_INC16: case CC_NEG16:
			return cpu->cc.dst == sext16(1u << 15);
		case CC_INC32: case CC_NEG32:
			return cpu->cc.dst == sext32(1u << 31);
		case CC_IMUL8: case CC_IMUL16: case CC_IMUL32:
		case CC_MUL8: case CC_MUL16: case CC_MUL32:
			return get_CF(cpu);
		case CC_SAR:
			return 0;
		case CC_SHL:
			return (cpu->cc.dst >> (sizeof(uword) * 8 - 1)) ^ (cpu->cc.dst2 & 1);
		case CC_SHR:
			return (cpu->cc.src1 >> (sizeof(uword) * 8 - 1));
		case CC_SHLD:
		case CC_SHRD:
			return (cpu->cc.src1 ^ cpu->cc.dst) >> (sizeof(uword) * 8 - 1);
		case CC_BSF:
		case CC_BSR:
			return 0;
		case CC_AND:
		case CC_OR:
		case CC_XOR:
			return 0;
		}
		assert(false);
	} else {
		return !!(cpu->flags & OF);
	}
	assert(false);
}

static void refresh_flags(CPUI386 *cpu)
{
	SET_BIT(cpu->flags, get_CF(cpu), CF);
	SET_BIT(cpu->flags, get_PF(cpu), PF);
	SET_BIT(cpu->flags, get_AF(cpu), AF);
	SET_BIT(cpu->flags, get_ZF(cpu), ZF);
	SET_BIT(cpu->flags, get_SF(cpu), SF);
	SET_BIT(cpu->flags, get_OF(cpu), OF);
}

static inline int get_IOPL(CPUI386 *cpu)
{
	return (cpu->flags & IOPL) >> 12;
}

/* MMU */
#define CR0_PG (1<<31)
#define CR0_WP (0x10000)
#ifdef BUILD_ESP32
#define tlb_size 256
#else
#define tlb_size 512
#endif
typedef struct {
	enum {
		ADDR_OK1,
		ADDR_OK2,
	} res;
	uword addr1;
	uword addr2;
} OptAddr;

static unsigned tlb_gen; /* counts TLB clears (fast/fast.h keeps a TLB of its own) */

static void tlb_clear(CPUI386 *cpu)
{
	tlb_gen++;
	for (int i = 0; i < tlb_size; i++) {
		cpu->tlb.tab[i].lpgno = -1;
	}
	cpu->ifetch.laddr = -1;
	cpu->ifetch.paddr = 0;
}

static int pte_lookup[2][4][2][2] = { //[wp != 0][(pte >> 1) & 3][cpl == 3][rwm > 1]
	{ // wp == 0
		{ {0, 0}, {1, 1} }, // s,r
		{ {0, 0}, {1, 1} }, // s,w
		{ {0, 0}, {0, 1} }, // u,r
		{ {0, 0}, {0, 0} }, // u,w
	},
	{ // wp == 1
		{ {0, 1}, {1, 1} }, // s,r
		{ {0, 0}, {1, 1} }, // s,w
		{ {0, 1}, {0, 1} }, // u,r
		{ {0, 0}, {0, 0} }, // u,w
	}
};

static bool IRAM_ATTR tlb_refill(CPUI386 *cpu, struct tlb_entry *ent, uword lpgno)
{
	if (unlikely(GET_LM_EFER(cpu))) {
		static u8 _[8];
		ent->lpgno = lpgno;
		ent->xaddr = 0;
		ent->pte_lookup = pte_lookup[1][3];
		ent->ppte = _;
		return true;
	}

	uword base_addr = cpu->cr3 & ~0xfff;
	uword i = lpgno >> 10;
	uword j = lpgno & 1023;

	u8 *mem = (u8 *) cpu->phys_mem;
	uword pde = pload32(cpu, base_addr + i * 4);
	if (!(pde & 1))
		return false;
	mem[base_addr + i * 4] |= 1 << 5; // accessed

	uword base_addr2 = pde & ~0xfff;
	uword pte = pload32(cpu, base_addr2 + j * 4);
	if (!(pte & 1))
		return false;

	mem[base_addr2 + j * 4] |= 1 << 5; // accessed
//	mem[base_addr2 + j * 4] |= 1 << 6; // dirty

	ent->lpgno = lpgno;
	ent->xaddr = (pte & ~0xfff) ^ (lpgno << 12);
	pte = pte & ((pde & 7) | 0xfffffff8);
	ent->pte_lookup = pte_lookup[!!(cpu->cr0 & CR0_WP)][(pte >> 1) & 3];
	ent->ppte = &(mem[base_addr2 + j * 4]);
	return true;
}

static bool IRAM_ATTR translate_lpgno(CPUI386 *cpu, int rwm, uword lpgno, uword laddr, int cpl, uword *paddr)
{
	struct tlb_entry *ent = &(cpu->tlb.tab[lpgno % tlb_size]);
	if (ent->lpgno != lpgno) {
		if (!tlb_refill(cpu, ent, lpgno)) {
			cpu->cr2 = laddr;
			cpu->excno = EX_PF;
			cpu->excerr = 0;
			if (rwm & 2)
				cpu->excerr |= 2;
			if (cpl == 3)
				cpu->excerr |= 4;
			return false;
		}
	}
	if (ent->pte_lookup[cpl == 3][rwm > 1]) {
		cpu->cr2 = laddr;
		cpu->excno = EX_PF;
		cpu->excerr = 1;
		if (rwm & 2)
			cpu->excerr |= 2;
		if (cpl == 3)
			cpu->excerr |= 4;
		ent->lpgno = -1;
		return false;
	}
	*paddr = ent->xaddr ^ laddr;
	if (rwm & 2) {
		*(ent->ppte) |= 1 << 6; // dirty
//		pstore8(cpu, ent->ppte,
//			pload8(cpu, ent->ppte) | (1 << 6)); // dirty
	}
	return true;
}

static bool IRAM_ATTR translate_laddr(CPUI386 *cpu, OptAddr *res, int rwm, uword laddr, int size, int cpl)
{
	if (cpu->cr0 & CR0_PG) {
		uword lpgno = laddr >> 12;
		uword paddr;
		TRY(translate_lpgno(cpu, rwm, lpgno, laddr, cpl, &paddr));
		res->res = ADDR_OK1;
		res->addr1 = paddr;
		if ((laddr & 0xfff) > 0x1000 - size) {
			lpgno++;
			TRY(translate_lpgno(cpu, rwm, lpgno, lpgno << 12, cpl, &paddr));
			res->res = ADDR_OK2;
			res->addr2 = paddr;
		}
	} else {
		res->res = ADDR_OK1;
		res->addr1 = laddr;
	}
	return true;
}

static bool IRAM_ATTR segcheck(CPUI386 *cpu, int rwm, int seg, uword addr, int size)
{
	if (cpu->cr0 & 1) {
		/* null selector check */
		if (cpu->seg[seg].limit == 0 && (cpu->seg[seg].sel & ~0x3) == 0) {
//			dolog("segcheck: seg %d is null %x\n", seg, cpu->seg[seg].sel);
			THROW(EX_GP, 0);
		}
#if 0
		/* limit check */
		bool expand_down = (cpu->seg[seg].flags & 0xc) == 0x4;
		bool over = addr + size - 1 > cpu->seg[seg].limit;
		if (expand_down)
			over = addr <= cpu->seg[seg].limit;
		if (over) {
			dolog("over: addr %08x size %08x limit %08x\n", addr, size, cpu->seg[seg].limit);
			THROW(EX_GP, 0);
		}
		/* todo: readonly check */
#endif
	}
	return true;
}

static bool IRAM_ATTR translate(CPUI386 *cpu, OptAddr *res, int rwm, int seg, uword addr, int size, int cpl)
{
	assert(seg != -1);
	uword laddr = cpu->seg[seg].base + addr;

	TRYL(segcheck(cpu, rwm, seg, addr, size));

	return translate_laddr(cpu, res, rwm, laddr, size, cpl);
}

static bool IRAM_ATTR translate8r(CPUI386 *cpu, OptAddr *res, int seg, uword addr)
{
	assert(seg != -1);
	uword laddr = cpu->seg[seg].base + addr;

	TRYL(segcheck(cpu, 1, seg, addr, 1));

	if (cpu->cr0 & CR0_PG) {
		uword lpgno = laddr >> 12;
		struct tlb_entry *ent = &(cpu->tlb.tab[lpgno % tlb_size]);
		if (ent->lpgno != lpgno) {
			if (!tlb_refill(cpu, ent, lpgno)) {
				cpu->cr2 = laddr;
				cpu->excno = EX_PF;
				cpu->excerr = 0;
				if (cpu->cpl == 3)
					cpu->excerr |= 4;
				return false;
			}
		}
		if (ent->pte_lookup[cpu->cpl == 3][0]) {
			cpu->cr2 = laddr;
			cpu->excno = EX_PF;
			cpu->excerr = 1;
			if (cpu->cpl == 3)
				cpu->excerr |= 4;
			ent->lpgno = -1;
			return false;
		}
		res->res = ADDR_OK1;
		res->addr1 = ent->xaddr ^ laddr;
	} else {
		res->res = ADDR_OK1;
		res->addr1 = laddr;
	}

	return true;
}

static inline bool translate8(CPUI386 *cpu, OptAddr *res, int rwm, int seg, uword addr)
{
	return translate(cpu, res, rwm, seg, addr, 1, cpu->cpl);
}

static inline bool translate16(CPUI386 *cpu, OptAddr *res, int rwm, int seg, uword addr)
{
	return translate(cpu, res, rwm, seg, addr, 2, cpu->cpl);
}

static inline bool translate32(CPUI386 *cpu, OptAddr *res, int rwm, int seg, uword addr)
{
	return translate(cpu, res, rwm, seg, addr, 4, cpu->cpl);
}

static inline bool in_iomem(uword addr)
{
	return (addr >= 0xa0000 && addr < 0xc0000) || addr >= 0xe0000000;
}

static u8 IRAM_ATTR load8(CPUI386 *cpu, OptAddr *res)
{
	uword addr = res->addr1;
	if (in_iomem(addr) && cpu->cb.iomem_read8)
		return cpu->cb.iomem_read8(cpu->cb.iomem, addr);
	if (unlikely(addr >= cpu->phys_mem_size)) {
		return 0;
	}
	return pload8(cpu, addr);
}

static u16 IRAM_ATTR load16(CPUI386 *cpu, OptAddr *res)
{
	if (in_iomem(res->addr1) && cpu->cb.iomem_read16)
		return cpu->cb.iomem_read16(cpu->cb.iomem, res->addr1);
	if (unlikely(res->addr1 >= cpu->phys_mem_size)) {
		return 0;
	}
	if (likely(res->res == ADDR_OK1))
		return pload16(cpu, res->addr1);
	else
		return pload8(cpu, res->addr1) | (pload8(cpu, res->addr2) << 8);
}

static u32 IRAM_ATTR load32(CPUI386 *cpu, OptAddr *res)
{
	if (in_iomem(res->addr1) && cpu->cb.iomem_read32)
		return cpu->cb.iomem_read32(cpu->cb.iomem, res->addr1);
	if (unlikely(res->addr1 >= cpu->phys_mem_size)) {
		return 0;
	}
	if (likely(res->res == ADDR_OK1)) {
		return pload32(cpu, res->addr1);
	} else {
		switch(res->addr1 & 0xf) {
		case 0xf:
			return pload8(cpu, res->addr1) | (pload16(cpu, res->addr2) << 8) |
				(pload8(cpu, res->addr2 + 2) << 24);
		case 0xe:
			return pload16(cpu, res->addr1) | (pload16(cpu, res->addr2) << 16);
		case 0xd:
			return pload8(cpu, res->addr1) | (pload16(cpu, res->addr1 + 1) << 8) |
				(pload8(cpu, res->addr2) << 24);
		}
	}
	assert(false);
}

static void IRAM_ATTR store8(CPUI386 *cpu, OptAddr *res, u8 val)
{
	uword addr = res->addr1;
	if (in_iomem(addr) && cpu->cb.iomem_write8) {
		cpu->cb.iomem_write8(cpu->cb.iomem, addr, val);
		return;
	}
	if (unlikely(addr >= cpu->phys_mem_size)) {
		return;
	}
	pstore8(cpu, addr, val);
}

static void IRAM_ATTR store16(CPUI386 *cpu, OptAddr *res, u16 val)
{
	if (in_iomem(res->addr1) && cpu->cb.iomem_write16) {
		cpu->cb.iomem_write16(cpu->cb.iomem, res->addr1, val);
		return;
	}
	if (unlikely(res->addr1 >= cpu->phys_mem_size)) {
		return;
	}
	if (likely(res->res == ADDR_OK1)) {
		pstore16(cpu, res->addr1, val);
	} else {
		pstore8(cpu, res->addr1, val);
		pstore8(cpu, res->addr2, val >> 8);
	}
}

static void IRAM_ATTR store32(CPUI386 *cpu, OptAddr *res, u32 val)
{
	if (in_iomem(res->addr1) && cpu->cb.iomem_write32) {
		cpu->cb.iomem_write32(cpu->cb.iomem, res->addr1, val);
		return;
	}
	if (unlikely(res->addr1 >= cpu->phys_mem_size)) {
		return;
	}
	if (likely(res->res == ADDR_OK1)) {
		pstore32(cpu, res->addr1, val);
	} else {
		switch(res->addr1 & 0xf) {
		case 0xf:
			pstore8(cpu, res->addr1, val);
			pstore16(cpu, res->addr2, val >> 8);
			pstore8(cpu, res->addr2 + 2, val >> 24);
			break;
		case 0xe:
			pstore16(cpu, res->addr1, val);
			pstore16(cpu, res->addr2, val >> 16);
			break;
		case 0xd:
			pstore8(cpu, res->addr1, val);
			pstore16(cpu, res->addr1 + 1, val >> 8);
			pstore8(cpu, res->addr2, val >> 24);
			break;
		}
	}
}

#define LOADSTORE(BIT) \
bool cpu_load ## BIT(CPUI386 *cpu, int seg, uword addr, u ## BIT *res) \
{ \
	OptAddr o; \
	TRY(translate ## BIT(cpu, &o, 1, seg, addr)); \
	*res = load ## BIT(cpu, &o); \
	return true; \
} \
\
bool cpu_store ## BIT(CPUI386 *cpu, int seg, uword addr, u ## BIT val) \
{ \
	OptAddr o; \
	TRY(translate ## BIT(cpu, &o, 2, seg, addr)); \
	store ## BIT(cpu, &o, val); \
	return true; \
} \

LOADSTORE(8)
LOADSTORE(16)
LOADSTORE(32)

static bool IRAM_ATTR peek8(CPUI386 *cpu, u8 *val)
{
	uword laddr = cpu->seg[SEG_CS].base + cpu->next_ip;
	if (likely((laddr ^ cpu->ifetch.laddr) < 4096)) {
		*val = pload8(cpu, cpu->ifetch.xaddr ^ laddr);
		return true;
	}
	OptAddr res;
	TRY(translate8r(cpu, &res, SEG_CS, cpu->next_ip));
	uword paddr = res.addr1;
	if (in_iomem(paddr) && cpu->cb.iomem_read8) {
		*val = cpu->cb.iomem_read8(cpu->cb.iomem, paddr);
		return true;
	}
	if (unlikely(paddr >= cpu->phys_mem_size)) {
		*val = 0;
		return true;
	}
	*val = pload8(cpu, paddr);
	if (likely(paddr < cpu->phys_mem_size - 4096 && !in_iomem(paddr + 16))) {
		cpu->ifetch.laddr = laddr & (~4095ul);
		cpu->ifetch.xaddr = paddr ^ laddr;
	}
	return true;
}

static bool IRAM_ATTR peek8a(CPUI386 *cpu, u8 *val)
{
	if (likely(cpu->ifetch.paddr)) {
		*val = pload8(cpu, cpu->ifetch.paddr);
		return true;
	}
	TRY(peek8(cpu, val));
	return true;
}

static bool IRAM_ATTR fetch8(CPUI386 *cpu, u8 *val)
{
	if (likely(cpu->ifetch.paddr)) {
		*val = pload8(cpu, cpu->ifetch.paddr);
		cpu->ifetch.paddr++;
		cpu->next_ip++;
		return true;
	}
	TRY(peek8(cpu, val));
	cpu->next_ip++;
	return true;
}

static bool IRAM_ATTR fetch8pf(CPUI386 *cpu, u8 *val)
{
	uword laddr = cpu->seg[SEG_CS].base + cpu->next_ip;
	if (likely((laddr ^ cpu->ifetch.laddr) < 4096 - 16)) {
		uword paddr = cpu->ifetch.xaddr ^ laddr;
		*val = pload8(cpu, paddr);
		cpu->ifetch.paddr = paddr + 1;
		cpu->next_ip++;
		return true;
	}
	cpu->ifetch.paddr = 0;
	TRY(peek8(cpu, val));
	cpu->next_ip++;
	return true;
}

static bool IRAM_ATTR fetch16(CPUI386 *cpu, u16 *val)
{
	if (likely(cpu->ifetch.paddr)) {
		*val = pload16(cpu, cpu->ifetch.paddr);
		cpu->ifetch.paddr += 2;
		cpu->next_ip += 2;
		return true;
	}
	uword laddr = cpu->seg[SEG_CS].base + cpu->next_ip;
	if (likely((laddr ^ cpu->ifetch.laddr) < 4095)) {
		*val = pload16(cpu, cpu->ifetch.xaddr ^ laddr);
	} else {
		OptAddr res;
		TRY(translate16(cpu, &res, 1, SEG_CS, cpu->next_ip));
		*val = load16(cpu, &res);
	}
	cpu->next_ip += 2;
	return true;
}

static bool IRAM_ATTR fetch32(CPUI386 *cpu, u32 *val)
{
	if (likely(cpu->ifetch.paddr)) {
		*val = pload32(cpu, cpu->ifetch.paddr);
		cpu->ifetch.paddr += 4;
		cpu->next_ip += 4;
		return true;
	}
	uword laddr = cpu->seg[SEG_CS].base + cpu->next_ip;
	if (likely((laddr ^ cpu->ifetch.laddr) < 4093)) {
		*val = pload32(cpu, cpu->ifetch.xaddr ^ laddr);
	} else {
		OptAddr res;
		TRY(translate32(cpu, &res, 1, SEG_CS, cpu->next_ip));
		*val = load32(cpu, &res);
	}
	cpu->next_ip += 4;
	return true;
}

/* insts decode && execute */
static inline bool modsib32(CPUI386 *cpu, int mod, int rm, uword *addr, int *seg)
{
	if (rm == 4) {
		u8 sib;
		TRY(fetch8(cpu, &sib));
		int b = sib & 7;
		if (b == 5 && mod == 0) {
			TRY(fetch32(cpu, addr));
		} else {
			*addr = REGi(b);
			// sp bp as base register
			if ((b == 4 || b == 5) && *seg == -1)
				*seg = SEG_SS;
		}
		int i = (sib >> 3) & 7;
		if (i != 4)
			*addr += REGi(i) << (sib >> 6);
	} else if (rm == 5 && mod == 0) {
		TRY(fetch32(cpu, addr));
	} else {
		*addr = REGi(rm);
		// bp as base register
		if (rm == 5 && *seg == -1)
			*seg = SEG_SS;
	}
	if (mod == 1) {
		u8 imm8;
		TRY(fetch8(cpu, &imm8));
		*addr += (s8) imm8;
	} else if (mod == 2) {
		u32 imm32;
		TRY(fetch32(cpu, &imm32));
		*addr += (s32) imm32;
	}
	if (*seg == -1)
		*seg = SEG_DS;
	return true;
}

static inline bool modsib16(CPUI386 *cpu, int mod, int rm, uword *addr, int *seg)
{
	if (rm == 6 && mod == 0) {
		u16 imm16;
		TRY(fetch16(cpu, &imm16));
		*addr = imm16;
	} else {
		switch(rm) {
		case 0: *addr = REGi(3) + REGi(6); break;
		case 1: *addr = REGi(3) + REGi(7); break;
		case 2: *addr = REGi(5) + REGi(6); break;
		case 3: *addr = REGi(5) + REGi(7); break;
		case 4: *addr = REGi(6); break;
		case 5: *addr = REGi(7); break;
		case 6: *addr = REGi(5); break;
		case 7: *addr = REGi(3); break;
		}
		if (mod == 1) {
			u8 imm8;
			TRY(fetch8(cpu, &imm8));
			*addr += (s8) imm8;
		} else if (mod == 2) {
			u16 imm16;
			TRY(fetch16(cpu, &imm16));
			*addr += imm16;
		}
		*addr &= 0xffff;
	}
	if (*seg == -1) {
		if (rm == 2 || rm == 3)
			*seg = SEG_SS;
		else if (mod != 0 && rm == 6)
			*seg = SEG_SS;
		else
			*seg = SEG_DS;
	}
	return true;
}

static bool IRAM_ATTR modsib(CPUI386 *cpu, int adsz16, int mod, int rm, uword *addr, int *seg)
{
	if (adsz16) return modsib16(cpu, mod, rm, addr, seg);
	else return modsib32(cpu, mod, rm, addr, seg);
}

static bool read_desc(CPUI386 *cpu, int sel, uword *w1, uword *w2)
{
	OptAddr meml;
	sel = sel & 0xffff;
	uword off = sel & ~0x7;
	uword base;
	uword limit;
	if (sel & 0x4) {
		base = cpu->seg[SEG_LDT].base;
		limit = cpu->seg[SEG_LDT].limit;
	} else {
		base = cpu->gdt.base;
		limit = cpu->gdt.limit;
	}

	if (off + 7 > limit) {
		dolog("read_desc: sel %04x base %x limit %x off %x\n", sel, base, limit, off);
		THROW(EX_GP, sel & ~0x3);
	}
	if (w1) {
		TRY(translate_laddr(cpu, &meml, 1, base + off, 4, 0));
		*w1 = load32(cpu, &meml);
	}
	TRY(translate_laddr(cpu, &meml, 1, base + off + 4, 4, 0));
	*w2 = load32(cpu, &meml);
	return true;
}

static bool set_seg(CPUI386 *cpu, int seg, int sel)
{
	sel = sel & 0xffff;
	if (!(cpu->cr0 & 1) || (cpu->flags & VM)) {
		cpu->seg[seg].sel = sel;
		cpu->seg[seg].base = sel << 4;
		cpu->seg[seg].limit = 0xffff;
		cpu->seg[seg].flags = 0; // D_BIT is not set
		if (seg == SEG_CS) {
			cpu->cpl = cpu->flags & VM ? 3 : 0;
			cpu->code16 = true;
		}
		if (seg == SEG_SS) {
			cpu->sp_mask = 0xffff;
		}
		return true;
	}

	uword w1, w2;
	TRY(read_desc(cpu, sel, &w1, &w2));

	// TODO: various permission checks
	bool s = (w2 >> 12) & 1;
	bool p = (w2 >> 15) & 1;
	if (sel & ~0x3) {
		switch(seg) {
		case SEG_DS: case SEG_ES: case SEG_FS: case SEG_GS:
			if (!s) {
				// to avoid win95 BSOD...
				THROW(EX_GP, sel & ~0x3);
			}
		}

		if (!p) THROW((seg == SEG_SS ? EX_SS : EX_NP), sel & ~0x3);
	}

	cpu->seg[seg].sel = sel;
	cpu->seg[seg].base = (w1 >> 16) | ((w2 & 0xff) << 16) | (w2 & 0xff000000);
	cpu->seg[seg].limit = (w2 & 0xf0000) | (w1 & 0xffff);
	if (w2 & 0x00800000)
		cpu->seg[seg].limit = (cpu->seg[seg].limit << 12) | 0xfff;
	cpu->seg[seg].flags = (w2 >> 8) & 0xffff; // (w2 >> 20) & 0xf;
	if (seg == SEG_CS) {
//		if ((sel & 3) != cpu->cpl)
//			dolog("set_seg: PVL %d => %d\n", cpu->cpl, sel & 3);
#ifdef AMD64_ENABLE_LEG32
		// go to long mode
		if (w2 & (1<< 21)) {
			THROW(0x8664, 0);
		}
#endif
		cpu->cpl = sel & 3;
		cpu->code16 = !(cpu->seg[SEG_CS].flags & SEG_D_BIT);
	}
	if (seg == SEG_SS) {
		cpu->sp_mask = cpu->seg[SEG_SS].flags & SEG_B_BIT ? 0xffffffff : 0xffff;
	}
	return true;
}

static inline void clear_segs(CPUI386 *cpu)
{
	int segs[] = { SEG_DS, SEG_ES, SEG_FS, SEG_GS };
	for (int i = 0; i < 4; i++) {
		uword w2 = cpu->seg[segs[i]].flags << 8;
		bool is_dataseg = !((w2 >> 11) & 1);
		int dpl = (w2 >> 13) & 0x3;
		bool conforming = (w2 >> 8) & 0x4;
		if (is_dataseg || !conforming) {
			if (dpl < cpu->cpl) {
				cpu->seg[segs[i]].sel = 0;
				cpu->seg[segs[i]].base = 0;
				cpu->seg[segs[i]].limit = 0;
				cpu->seg[segs[i]].flags = 0;
			}
		}
	}
}

/*
 * addressing modes
 */
#define _(rwm, inst) inst()

#define E_helper(BIT, SUFFIX, rwm, INST) \
	TRY(fetch8(cpu, &modrm)); \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (mod == 3) { \
		INST ## SUFFIX(rm, lreg ## BIT, sreg ## BIT) \
	} else { \
		TRY(modsib(cpu, adsz16, mod, rm, &addr, &curr_seg)); \
		TRY(translate ## BIT(cpu, &meml, rwm, curr_seg, addr)); \
		INST ## SUFFIX(&meml, laddr ## BIT, saddr ## BIT) \
	}

#define Eb(...) E_helper(8, , __VA_ARGS__)
#define Ev(...) if (opsz16) { E_helper(16, w, __VA_ARGS__) } else { E_helper(32, d, __VA_ARGS__) }

#define EG_helper(PM, BT, BIT, SUFFIX, rwm, INST) \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (PM && (!(cpu->cr0 & 1) || (cpu->flags & VM))) THROW0(EX_UD); \
	if (mod == 3) { \
		INST ## SUFFIX(rm, reg, lreg ## BIT, sreg ## BIT, lreg ## BIT, sreg ## BIT) \
	} else { \
		TRY(modsib(cpu, adsz16, mod, rm, &addr, &curr_seg)); \
		if (BT) addr += lreg ## BIT(reg) / BIT * (BIT / 8); \
		TRY(translate ## BIT(cpu, &meml, rwm, curr_seg, addr)); \
		INST ## SUFFIX(&meml, reg, laddr ## BIT, saddr ## BIT, lreg ## BIT, sreg ## BIT) \
	}

#define EbGb(...) EG_helper(false, false, 8, , __VA_ARGS__)
#define EwGw(...) EG_helper(false, false, 16, , __VA_ARGS__)
#define PMEwGw(...) EG_helper(true, false, 16, , __VA_ARGS__)
#define EvGv(...) if (opsz16) { EG_helper(false, false, 16, w, __VA_ARGS__) } else { EG_helper(false, false, 32, d, __VA_ARGS__) }
#define BTEvGv(...) if (opsz16) { EG_helper(false, true, 16, w, __VA_ARGS__) } else { EG_helper(false, true, 32, d, __VA_ARGS__) }

#define EGIb_helper(BIT, SUFFIX, rwm, INST) \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	u8 imm8; \
	if (mod == 3) { \
		TRY(fetch8(cpu, &imm8)); \
		INST ## SUFFIX(rm, reg, imm8, lreg ## BIT, sreg ## BIT, lreg ## BIT, sreg ## BIT, limm, 0) \
	} else { \
		TRY(modsib(cpu, adsz16, mod, rm, &addr, &curr_seg)); \
		TRY(fetch8(cpu, &imm8)); \
		TRY(translate ## BIT(cpu, &meml, rwm, curr_seg, addr)); \
		INST ## SUFFIX(&meml, reg, imm8, laddr ## BIT, saddr ## BIT, lreg ## BIT, sreg ## BIT, limm, 0) \
	}

#define EvGvIb(...) if (opsz16) { EGIb_helper(16, w, __VA_ARGS__) } else { EGIb_helper(32, d, __VA_ARGS__) }

#define EGCL_helper(BIT, SUFFIX, rwm, INST) \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (mod == 3) { \
		INST ## SUFFIX(rm, reg, 1, lreg ## BIT, sreg ## BIT, lreg ## BIT, sreg ## BIT, lreg8, sreg8) \
	} else { \
		TRY(modsib(cpu, adsz16, mod, rm, &addr, &curr_seg)); \
		TRY(translate ## BIT(cpu, &meml, rwm, curr_seg, addr)); \
		INST ## SUFFIX(&meml, reg, 1, laddr ## BIT, saddr ## BIT, lreg ## BIT, sreg ## BIT, lreg8, sreg8) \
	}

#define EvGvCL(...) if (opsz16) { EGCL_helper(16, w, __VA_ARGS__) } else { EGCL_helper(32, d, __VA_ARGS__) }

#define EI_helper(BIT, SUFFIX, rwm, INST) \
	TRY(fetch8(cpu, &modrm)); \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	u ## BIT imm ## BIT; \
	if (mod == 3) { \
		TRY(fetch ## BIT(cpu, &imm ## BIT)); \
		INST ## SUFFIX(rm, imm ## BIT, lreg ## BIT, sreg ## BIT, limm, 0) \
	} else { \
		TRY(modsib(cpu, adsz16, mod, rm, &addr, &curr_seg)); \
		TRY(fetch ## BIT(cpu, &imm ## BIT)); \
		TRY(translate ## BIT(cpu, &meml, rwm, curr_seg, addr)); \
		INST ## SUFFIX(&meml, imm ## BIT, laddr ## BIT, saddr ## BIT, limm, 0) \
	}

#define EbIb(...) EI_helper(8, , __VA_ARGS__)
#define EvIv(...) if (opsz16) { EI_helper(16, w, __VA_ARGS__) } else { EI_helper(32, d, __VA_ARGS__) }

#define EIb_helper(BT, BIT, SUFFIX, rwm, INST) \
	TRY(fetch8(cpu, &modrm)); \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	u8 imm8; \
	u ## BIT imm ## BIT; \
	if (mod == 3) { \
		TRY(fetch8(cpu, &imm8)); \
		imm ## BIT = (s ## BIT) ((s8) imm8); \
		INST ## SUFFIX(rm, imm ## BIT, lreg ## BIT, sreg ## BIT, limm, 0) \
	} else { \
		TRY(modsib(cpu, adsz16, mod, rm, &addr, &curr_seg)); \
		TRY(fetch8(cpu, &imm8)); \
		imm ## BIT = (s ## BIT) ((s8) imm8); \
		if (BT) addr += imm ## BIT / BIT * (BIT / 8); \
		TRY(translate ## BIT(cpu, &meml, rwm, curr_seg, addr)); \
		INST ## SUFFIX(&meml, imm ## BIT, laddr ## BIT, saddr ## BIT, limm, 0) \
	}

#define EvIb(...) if (opsz16) { EIb_helper(false, 16, w, __VA_ARGS__) } else { EIb_helper(false, 32, d, __VA_ARGS__) }
#define BTEvIb(...) if (opsz16) { EIb_helper(true, 16, w, __VA_ARGS__) } else { EIb_helper(true, 32, d, __VA_ARGS__) }

#define E1_helper(BIT, SUFFIX, rwm, INST) \
	TRY(fetch8(cpu, &modrm)); \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (mod == 3) { \
		INST ## SUFFIX(rm, 1, lreg ## BIT, sreg ## BIT, limm, 0) \
	} else { \
		TRY(modsib(cpu, adsz16, mod, rm, &addr, &curr_seg)); \
		TRY(translate ## BIT(cpu, &meml, rwm, curr_seg, addr)); \
		INST ## SUFFIX(&meml, 1, laddr ## BIT, saddr ## BIT, limm, 0) \
	}

#define Eb1(...) E1_helper(8, , __VA_ARGS__)
#define Ev1(...) if (opsz16) { E1_helper(16, w, __VA_ARGS__) } else { E1_helper(32, d, __VA_ARGS__) }

#define ECL_helper(BIT, SUFFIX, rwm, INST) \
	TRY(fetch8(cpu, &modrm)); \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (mod == 3) { \
		INST ## SUFFIX(rm, 1, lreg ## BIT, sreg ## BIT, lreg8, sreg8) \
	} else { \
		TRY(modsib(cpu, adsz16, mod, rm, &addr, &curr_seg)); \
		TRY(translate ## BIT(cpu, &meml, rwm, curr_seg, addr)); \
		INST ## SUFFIX(&meml, 1, laddr ## BIT, saddr ## BIT, lreg8, sreg8) \
	}

#define EbCL(...) ECL_helper(8, , __VA_ARGS__)
#define EvCL(...) if (opsz16) { ECL_helper(16, w, __VA_ARGS__) } else { ECL_helper(32, d, __VA_ARGS__) }

#define GE_helper(BIT, SUFFIX, rwm, INST) \
	int reg; \
	u ## BIT opr; \
	TRY(__GE_helper ## BIT(cpu, adsz16, curr_seg, &reg, &opr)); \
	INST ## SUFFIX(reg, opr, lreg ## BIT, sreg ## BIT, limm, 0)

#define GbEb(...) GE_helper(8, , __VA_ARGS__)
#define GvEv(...) if (opsz16) { GE_helper(16, w, __VA_ARGS__) } else { GE_helper(32, d, __VA_ARGS__) }

#define GvM_helper(BIT, SUFFIX, rwm, INST) \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (mod == 3) { \
		INST ## SUFFIX(reg, rm, lreg ## BIT, sreg ## BIT, lreg ## BIT, sreg ## BIT) \
	} else { \
		TRY(modsib(cpu, adsz16, mod, rm, &addr, &curr_seg)); \
		INST ## SUFFIX(reg, addr, lreg ## BIT, sreg ## BIT, limm, 0) \
	}
#define GvM(...) if (opsz16) { GvM_helper(16, w, __VA_ARGS__) } else { GvM_helper(32, d, __VA_ARGS__) }

#define GvMp_helper(BIT, SUFFIX, rwm, INST) \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (mod == 3) THROW0(EX_UD); \
	else { \
		TRY(modsib(cpu, adsz16, mod, rm, &addr, &curr_seg)); \
		INST ## SUFFIX(reg, addr, lreg ## BIT, sreg ## BIT, limm, 0) \
	}
#define GvMp(...) if (opsz16) { GvMp_helper(16, w, __VA_ARGS__) } else { GvMp_helper(32, d, __VA_ARGS__) }

#define GE_helper2(BIT, SUFFIX, BIT2, SUFFIX2, rwm, INST) \
	int reg; \
	u ## BIT2 opr; \
	TRY(__GE_helper ## BIT2(cpu, adsz16, curr_seg, &reg, &opr)); \
	INST ## SUFFIX ## SUFFIX2(reg, opr, lreg ## BIT, sreg ## BIT, limm, 0)

#define GvEb(...) if (opsz16) { GE_helper2(16, w, 8, b, __VA_ARGS__) } else { GE_helper2(32, d, 8, b, __VA_ARGS__) }
#define GvEw(...) if (opsz16) { GE_helper2(16, w, 16, w, __VA_ARGS__) } else { GE_helper2(32, d, 16, w, __VA_ARGS__) }

#define GEI_helperI2(BIT, SUFFIX, BIT2, SUFFIX2, rwm, INST) \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (mod == 3) { \
		u ## BIT2 imm ## BIT2; \
		TRY(fetch ## BIT2(cpu, &imm ## BIT2)); \
		INST ## SUFFIX ## I ## SUFFIX2(reg, rm, imm ## BIT2, lreg ## BIT, sreg ## BIT, lreg ## BIT, sreg ## BIT) \
	} else { \
		TRY(modsib(cpu, adsz16, mod, rm, &addr, &curr_seg)); \
		u ## BIT2 imm ## BIT2; \
		TRY(fetch ## BIT2(cpu, &imm ## BIT2)); \
		TRY(translate ## BIT(cpu, &meml, rwm, curr_seg, addr)); \
		INST ## SUFFIX ## I ## SUFFIX2(reg, &meml, imm ## BIT2, lreg ## BIT, sreg ## BIT, laddr ## BIT, saddr ## BIT) \
	}

#define GvEvIb(...) if (opsz16) { GEI_helperI2(16, w, 8, b, __VA_ARGS__) } else { GEI_helperI2(32, d, 8, b, __VA_ARGS__) }
#define GvEvIv(...) if (opsz16) { GEI_helperI2(16, w, 16, w, __VA_ARGS__) } else { GEI_helperI2(32, d, 32, d, __VA_ARGS__) }

#define ALIb(rwm, INST) \
	u8 imm8; \
	TRY(fetch8(cpu, &imm8)); \
	INST(0, imm8, lreg8, sreg8, limm, 0)

#define AXIb(rwm, INST) \
	if (opsz16) { \
		u8 imm8; \
		TRY(fetch8(cpu, &imm8)); \
		INST ## w(0, imm8, lreg16, sreg16, limm, 0) \
	} else { \
		u8 imm8; \
		TRY(fetch8(cpu, &imm8)); \
		INST ## d(0, imm8, lreg32, sreg32, limm, 0) \
	}

#define IbAL(rwm, INST) \
	u8 imm8; \
	TRY(fetch8(cpu, &imm8)); \
	INST(imm8, 0, limm, 0, lreg8, sreg8)

#define IbAX(rwm, INST) \
	if (opsz16) { \
		u8 imm8; \
		TRY(fetch8(cpu, &imm8)); \
		INST ## w(imm8, 0, limm, 0, lreg16, sreg16) \
	} else { \
		u8 imm8; \
		TRY(fetch8(cpu, &imm8)); \
		INST ## d(imm8, 0, limm, 0, lreg32, sreg32) \
	}

#define DXAL(rwm, INST) \
	INST(2, 0, lreg16, sreg16, lreg8, sreg8)

#define DXAX(rwm, INST) \
	if (opsz16) { \
		INST ## w(2, 0, lreg16, sreg16, lreg16, sreg16) \
	} else { \
		INST ## d(2, 0, lreg16, sreg16, lreg32, sreg32) \
	}

#define ALDX(rwm, INST) \
	INST(0, 2, lreg8, sreg8, lreg16, sreg16)

#define AXDX(rwm, INST) \
	if (opsz16) { \
		INST ## w(0, 2, lreg16, sreg16, lreg16, sreg16) \
	} else { \
		INST ## d(0, 2, lreg32, sreg32, lreg16, sreg16) \
	}

#define AXIv(rwm, INST) \
	if (opsz16) { \
		u16 imm16; \
		TRY(fetch16(cpu, &imm16)); \
		INST ## w(0, imm16, lreg16, sreg16, limm, 0) \
	} else { \
		u32 imm32; \
		TRY(fetch32(cpu, &imm32)); \
		INST ## d(0, imm32, lreg32, sreg32, limm, 0) \
	}

#define ALOb(rwm, INST) \
	if (adsz16) { \
		u16 addr16; \
		TRY(fetch16(cpu, &addr16)); \
		addr = addr16; \
	} else { \
		TRY(fetch32(cpu, &addr)); \
	} \
	if (curr_seg == -1) curr_seg = SEG_DS; \
	TRY(translate8(cpu, &meml, rwm, curr_seg, addr)); \
	INST(0, &meml, lreg8, sreg8, laddr8, saddr8)

#define AXOv(rwm, INST) \
	if (adsz16) { \
		u16 addr16; \
		TRY(fetch16(cpu, &addr16)); \
		addr = addr16; \
	} else { \
		TRY(fetch32(cpu, &addr)); \
	} \
	if (curr_seg == -1) curr_seg = SEG_DS; \
	if (opsz16) { \
		TRY(translate16(cpu, &meml, rwm, curr_seg, addr)); \
		INST ## w(0, &meml, lreg16, sreg16, laddr16, saddr16) \
	} else { \
		TRY(translate32(cpu, &meml, rwm, curr_seg, addr)); \
		INST ## d(0, &meml, lreg32, sreg32, laddr32, saddr32) \
	}

#define ObAL(rwm, INST) \
	if (adsz16) { \
		u16 addr16; \
		TRY(fetch16(cpu, &addr16)); \
		addr = addr16; \
	} else { \
		TRY(fetch32(cpu, &addr)); \
	} \
	if (curr_seg == -1) curr_seg = SEG_DS; \
	TRY(translate8(cpu, &meml, rwm, curr_seg, addr)); \
	INST(&meml, 0, laddr8, saddr8, lreg8, sreg8)

#define OvAX(rwm, INST) \
	if (adsz16) { \
		u16 addr16; \
		TRY(fetch16(cpu, &addr16)); \
		addr = addr16; \
	} else { \
		TRY(fetch32(cpu, &addr)); \
	} \
	if (curr_seg == -1) curr_seg = SEG_DS; \
	if (opsz16) { \
		TRY(translate16(cpu, &meml, rwm, curr_seg, addr)); \
		INST ## w(&meml, 0, laddr16, saddr16, lreg16, sreg16) \
	} else { \
		TRY(translate32(cpu, &meml, rwm, curr_seg, addr)); \
		INST ## d(&meml, 0, laddr32, saddr32, lreg32, sreg32) \
	}

#define PlusRegv(rwm, INST) \
	if (opsz16) { \
		INST ## w((b1 & 7), lreg16, sreg16) \
	} else { \
		INST ## d((b1 & 7), lreg32, sreg32) \
	}

#define PlusRegIb(rwm, INST) \
	u8 imm8; \
	TRY(fetch8(cpu, &imm8)); \
	INST((b1 & 7), imm8, lreg8, sreg8, limm, 0)

#define PlusRegIv(rwm, INST) \
	if (opsz16) { \
		u16 imm16; \
		TRY(fetch16(cpu, &imm16)); \
		INST ## w((b1 & 7), imm16, lreg16, sreg16, limm, 0) \
	} else { \
		u32 imm32; \
		TRY(fetch32(cpu, &imm32)); \
		INST ## d((b1 & 7), imm32, lreg32, sreg32, limm, 0) \
	}

#define Ib(rwm, INST) \
	u8 imm8; \
	TRY(fetch8(cpu, &imm8)); \
	INST(imm8, limm, 0)
#define Jb Ib

#define Iw(rwm, INST) \
	u16 imm16; \
	TRY(fetch16(cpu, &imm16)); \
	INST(imm16, limm, 0)

#define IwIb(rwm, INST) \
	u16 imm16; \
	TRY(fetch16(cpu, &imm16)); \
	u8 imm8; \
	TRY(fetch8(cpu, &imm8)); \
	INST(imm16, imm8, limm, 0, limm, 0)

#define Iv(rwm, INST) \
	if (opsz16) { \
		u16 imm16; \
		TRY(fetch16(cpu, &imm16)); \
		INST ## w(imm16, limm, 0) \
	} else { \
		u32 imm32; \
		TRY(fetch32(cpu, &imm32)); \
		INST ## d(imm32, limm, 0) \
	}

#define Jv(rwm, INST) \
	if (opsz16) { \
		u16 imm16; \
		TRY(fetch16(cpu, &imm16)); \
		INST ## w(imm16, limm, 0); \
	} else { \
		u32 imm32; \
		TRY(fetch32(cpu, &imm32)); \
		INST ## d(imm32, limm, 0); \
	}
#define Av Iv

#define Ap(rwm, INST) \
	u16 seg; \
	if (opsz16) { \
		u16 addr16; \
		TRY(fetch16(cpu, &addr16)); \
		addr = addr16; \
	} else { \
		TRY(fetch32(cpu, &addr)); \
	} \
	TRY(fetch16(cpu, &seg)); \
	INST(addr, seg)

#define Ep(rwm, INST) \
	TRY(fetch8(cpu, &modrm)); \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (mod == 3) THROW0(EX_UD); \
	else { \
		u16 seg; \
		u32 off; \
		OptAddr moff, mseg; \
		TRY(modsib(cpu, adsz16, mod, rm, &addr, &curr_seg)); \
		if (opsz16) { \
			TRY(translate16(cpu, &moff, rwm, curr_seg, addr)); \
			TRY(translate16(cpu, &mseg, rwm, curr_seg, addr + 2)); \
			off = laddr16(&moff); \
			seg = laddr16(&mseg); \
		} else { \
			TRY(translate32(cpu, &moff, rwm, curr_seg, addr)); \
			TRY(translate16(cpu, &mseg, rwm, curr_seg, addr + 4)); \
			off = laddr32(&moff); \
			seg = laddr16(&mseg); \
		} \
		INST(off, seg) \
	}

#define Ms(rwm, INST) \
	TRY(fetch8(cpu, &modrm)); \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (mod == 3) THROW0(EX_UD); \
	else { \
		TRY(modsib(cpu, adsz16, mod, rm, &addr, &curr_seg)); \
		INST(addr) \
	}

#define Ew(rwm, INST) \
	TRY(fetch8(cpu, &modrm)); \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (mod == 3) { \
		if (opsz16) { \
			INST(rm, lreg16, sreg16) \
		} else { \
			INST(rm, lreg32, sreg32) \
		} \
	} else { \
		TRY(modsib(cpu, adsz16, mod, rm, &addr, &curr_seg)); \
		TRY(translate16(cpu, &meml, rwm, curr_seg, addr)); \
		INST(&meml, laddr16, saddr16) \
	}

#define EwSw(rwm, INST) \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (mod == 3) { \
		if (opsz16) { \
			INST(rm, reg, lreg16, sreg16, lseg, 0) \
		} else { \
			INST(rm, reg, lreg32, sreg32, lseg, 0) \
		} \
	} else { \
		TRY(modsib(cpu, adsz16, mod, rm, &addr, &curr_seg)); \
		TRY(translate16(cpu, &meml, rwm, curr_seg, addr)); \
		INST(&meml, reg, laddr16, saddr16, lseg, 0) \
	}

#define SwEw(rwm, INST) \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (mod == 3) { \
		INST(reg, rm, lseg, 0, lreg16, sreg16) \
	} else { \
		TRY(modsib(cpu, adsz16, mod, rm, &addr, &curr_seg)); \
		TRY(translate16(cpu, &meml, rwm, curr_seg, addr)); \
		INST(reg, &meml,lseg, 0, laddr16, saddr16) \
	}

#define limm(i) i
#ifdef I386_OPT1
#define lreg8(i) ((i) > 3 ? cpu->gprx[i - 4].r8[1] : cpu->gprx[i].r8[0])
#define sreg8(i, v) ((i) > 3 ? (cpu->gprx[i - 4].r8[1] = (v)) : (cpu->gprx[i].r8[0] = (v)))
#define lreg16(i) (cpu->gprx[i].r16)
#define sreg16(i, v) (cpu->gprx[i].r16 = (v))
#define lreg32(i) (REGi(i))
#define sreg32(i, v) ((REGi(i)) = (v))
#else
#define lreg8(i) ((u8) ((i) > 3 ? REGi((i) - 4) >> 8 : REGi((i))))
#define sreg8(i, v) ((i) > 3 ? \
		     (REGi((i) - 4) = (REGi((i) - 4) & (wordmask ^ 0xff00)) | (((v) & 0xff) << 8)) : \
		     (REGi((i)) = (REGi((i)) & (wordmask ^ 0xff)) | ((v) & 0xff)))
#define lreg16(i) ((u16) REGi((i)))
#define sreg16(i, v) (REGi((i)) = (REGi((i)) & (wordmask ^ 0xffff)) | ((v) & 0xffff))
#define lreg32(i) ((u32) REGi((i)))
#define sreg32(i, v) (REGi((i)) = (REGi((i)) & (wordmask ^ 0xffffffff)) | ((v) & 0xffffffff))
#endif
#define laddr8(addr) load8(cpu, addr)
#define saddr8(addr, v) store8(cpu, addr, v)
#define laddr16(addr) load16(cpu, addr)
#define saddr16(addr, v) store16(cpu, addr, v)
#define laddr32(addr) load32(cpu, addr)
#define saddr32(addr, v) store32(cpu, addr, v)
#define lseg(i) ((u16) SEGi((i)))
#define set_sp(v, mask) (sreg32(4, ((v) & mask) | (lreg32(4) & ~mask)))

#define GEN___GE_helper(BIT) \
static bool IRAM_ATTR __GE_helper ## BIT \
(CPUI386 *cpu, int adsz16, int curr_seg, int *reg, u ## BIT *opr) \
{ \
	uword addr; \
	OptAddr meml; \
	u8 modrm; \
	TRY(fetch8(cpu, &modrm)); \
	*reg = (modrm >> 3) & 7; \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (mod == 3) { \
		*opr = lreg ## BIT(rm); \
	} else { \
		TRY(modsib(cpu, adsz16, mod, rm, &addr, &curr_seg)); \
		TRY(translate ## BIT(cpu, &meml, 1, curr_seg, addr)); \
		*opr = laddr ## BIT(&meml); \
	} \
	return true; \
}
GEN___GE_helper(8)
GEN___GE_helper(16)
GEN___GE_helper(32)

/*
 * instructions
 */
#define ACOP_helper(NAME1, NAME2, BIT, OP, a, b, la, sa, lb, sb) \
	int cf = get_CF(cpu); \
	cpu->cc.src1 = sext ## BIT(la(a)); \
	cpu->cc.src2 = sext ## BIT(lb(b)); \
	cpu->cc.dst = sext ## BIT(cpu->cc.src1 OP cpu->cc.src2 OP cf); \
	cpu->cc.op = cf ? CC_ ## NAME1 : CC_ ## NAME2; \
	cpu->cc.mask = CF | PF | AF | ZF | SF | OF; \
	sa(a, cpu->cc.dst);

#define AOP0_helper(NAME, BIT, OP, a, b, la, sa, lb, sb) \
	cpu->cc.src1 = sext ## BIT(la(a)); \
	cpu->cc.src2 = sext ## BIT(lb(b)); \
	cpu->cc.dst = sext ## BIT(cpu->cc.src1 OP cpu->cc.src2); \
	cpu->cc.op = CC_ ## NAME; \
	cpu->cc.mask = CF | PF | AF | ZF | SF | OF;

#define LOP0_helper(NAME, BIT, OP, a, b, la, sa, lb, sb) \
	cpu->cc.dst = sext ## BIT(la(a) OP lb(b)); \
	cpu->cc.op = CC_ ## NAME; \
	cpu->cc.mask = CF | PF | ZF | SF | OF;

#define AOP_helper(NAME1, BIT, OP, a, b, la, sa, lb, sb) \
	AOP0_helper(NAME1, BIT, OP, a, b, la, sa, lb, sb) \
	sa(a, cpu->cc.dst);

#define LOP_helper(NAME1, BIT, OP, a, b, la, sa, lb, sb) \
	LOP0_helper(NAME1, BIT, OP, a, b, la, sa, lb, sb) \
	sa(a, cpu->cc.dst);

#define INCDEC_helper(NAME, BIT, OP, a, la, sa) \
	int cf = get_CF(cpu); \
	cpu->cc.dst = sext ## BIT(sext ## BIT(la(a)) OP 1); \
	cpu->cc.op = CC_ ## NAME ## BIT; \
	SET_BIT(cpu->flags, cf, CF); \
	cpu->cc.mask = PF | AF | ZF | SF | OF; \
	sa(a, cpu->cc.dst);

#define NEG_helper(BIT, a, la, sa) \
	cpu->cc.src1 = sext ## BIT(la(a)); \
	cpu->cc.dst = sext ## BIT(-cpu->cc.src1); \
	cpu->cc.op = CC_NEG ## BIT; \
	cpu->cc.mask = CF | PF | AF | ZF | SF | OF; \
	sa(a, cpu->cc.dst);

noinline void IRAM_ATTR try_jcc8(CPUI386 *cpu)
{
	if (likely(cpu->ifetch.paddr)) {
		u8 op = pload8(cpu, cpu->ifetch.paddr);
		// catches ~60%
		if ((op & ~1) == 0x74) {
			if ((cpu->cc.dst == 0) ^ (op & 1)) {
				sword d = sext8(pload8(cpu, cpu->ifetch.paddr + 1));
				cpu->next_ip += d + 2;
				if (cpu->code16) cpu->next_ip &= 0xffff;
			}
		}
	}
}

#define CMP_helper(NAME, BIT, OP, a, b, la, sa, lb, sb) \
	AOP0_helper(NAME, BIT, OP, a, b, la, sa, lb, sb) \
	try_jcc8(cpu);

#define TEST_helper(NAME, BIT, OP, a, b, la, sa, lb, sb) \
	LOP0_helper(NAME, BIT, OP, a, b, la, sa, lb, sb) \
	try_jcc8(cpu);

#define ADCb(...) ACOP_helper(ADC, ADD,  8, +, __VA_ARGS__)
#define ADCw(...) ACOP_helper(ADC, ADD, 16, +, __VA_ARGS__)
#define ADCd(...) ACOP_helper(ADC, ADD, 32, +, __VA_ARGS__)
#define SBBb(...) ACOP_helper(SBB, SUB,  8, -, __VA_ARGS__)
#define SBBw(...) ACOP_helper(SBB, SUB, 16, -, __VA_ARGS__)
#define SBBd(...) ACOP_helper(SBB, SUB, 32, -, __VA_ARGS__)
#define ADDb(...) AOP_helper(ADD,  8, +, __VA_ARGS__)
#define ADDw(...) AOP_helper(ADD, 16, +, __VA_ARGS__)
#define ADDd(...) AOP_helper(ADD, 32, +, __VA_ARGS__)
#define SUBb(...) AOP_helper(SUB,  8, -, __VA_ARGS__)
#define SUBw(...) AOP_helper(SUB, 16, -, __VA_ARGS__)
#define SUBd(...) AOP_helper(SUB, 32, -, __VA_ARGS__)
#define ORb(...)  LOP_helper(OR,   8, |, __VA_ARGS__)
#define ORw(...)  LOP_helper(OR,  16, |, __VA_ARGS__)
#define ORd(...)  LOP_helper(OR,  32, |, __VA_ARGS__)
#define ANDb(...) LOP_helper(AND,  8, &, __VA_ARGS__)
#define ANDw(...) LOP_helper(AND, 16, &, __VA_ARGS__)
#define ANDd(...) LOP_helper(AND, 32, &, __VA_ARGS__)
#define XORb(...) LOP_helper(XOR,  8, ^, __VA_ARGS__)
#define XORw(...) LOP_helper(XOR, 16, ^, __VA_ARGS__)
#define XORd(...) LOP_helper(XOR, 32, ^, __VA_ARGS__)
#define CMPb(...)  CMP_helper(SUB,  8, -, __VA_ARGS__)
#define CMPw(...)  CMP_helper(SUB, 16, -, __VA_ARGS__)
#define CMPd(...)  CMP_helper(SUB, 32, -, __VA_ARGS__)
#define TESTb(...) TEST_helper(AND,  8, &, __VA_ARGS__)
#define TESTw(...) TEST_helper(AND, 16, &, __VA_ARGS__)
#define TESTd(...) TEST_helper(AND, 32, &, __VA_ARGS__)
#define INCb(...) INCDEC_helper(INC,  8, +, __VA_ARGS__)
#define INCw(...) INCDEC_helper(INC, 16, +, __VA_ARGS__)
#define INCd(...) INCDEC_helper(INC, 32, +, __VA_ARGS__)
#define DECb(...) INCDEC_helper(DEC,  8, -, __VA_ARGS__)
#define DECw(...) INCDEC_helper(DEC, 16, -, __VA_ARGS__)
#define DECd(...) INCDEC_helper(DEC, 32, -, __VA_ARGS__)
#define NOTb(a, la, sa) sa(a, ~la(a));
#define NOTw(a, la, sa) sa(a, ~la(a));
#define NOTd(a, la, sa) sa(a, ~la(a));
#define NEGb(...) NEG_helper(8,  __VA_ARGS__)
#define NEGw(...) NEG_helper(16, __VA_ARGS__)
#define NEGd(...) NEG_helper(32, __VA_ARGS__)

#define SHL_helper(BIT, a, b, la, sa, lb, sb) \
	uword x = la(a); \
	uword y = (lb(b)) & 0x1f; \
	if (y) { \
		cpu->cc.dst = sext ## BIT(x << y); \
		cpu->cc.dst2 = ((x >> (BIT - y)) & 1); \
		cpu->cc.op = CC_SHL; \
		cpu->cc.mask = CF | PF | ZF | SF | OF; \
		sa(a, cpu->cc.dst); \
	}

#define SHLb(...) SHL_helper(8, __VA_ARGS__)
#define SHLw(...) SHL_helper(16, __VA_ARGS__)
#define SHLd(...) SHL_helper(32, __VA_ARGS__)

#define ROL_helper(BIT, a, b, la, sa, lb, sb) \
	uword x = la(a); \
	uword y0 = lb(b); \
	uword y = y0 & (BIT - 1); \
	uword res = x; \
	if (y) { \
		res = sext ## BIT((x << y) | (x >> (BIT - y))); \
		sa(a, res); \
	} \
	if (y0) { \
		int cf1 = res & 1; \
		int of1 = (res >> (sizeof(uword) * 8 - 1)) ^ cf1; \
		SET_BIT(cpu->flags, cf1, CF); \
		SET_BIT(cpu->flags, of1, OF); \
		cpu->cc.mask &= ~(CF | OF); \
	}

#define ROLb(...) ROL_helper(8, __VA_ARGS__)
#define ROLw(...) ROL_helper(16, __VA_ARGS__)
#define ROLd(...) ROL_helper(32, __VA_ARGS__)

#define RCL_helper(BIT, a, b, la, sa, lb, sb) \
	uword x = la(a); \
	uword y = ((lb(b)) & 0x1f) % (BIT + 1); \
	if (y) { \
		uword cf = get_CF(cpu); \
		uword res = sext ## BIT((x << y) | (cf << (y - 1)) | (y != 1 ? (x >> (BIT + 1 - y)) : 0)); \
		int cf1 = (x >> (BIT - y)) & 1; \
		int of1 = (res >> (sizeof(uword) * 8 - 1)) ^ cf1; \
		SET_BIT(cpu->flags, cf1, CF); \
		SET_BIT(cpu->flags, of1, OF); \
		cpu->cc.mask &= ~(CF | OF); \
		sa(a, res); \
	}

#define RCLb(...) RCL_helper(8, __VA_ARGS__)
#define RCLw(...) RCL_helper(16, __VA_ARGS__)
#define RCLd(...) RCL_helper(32, __VA_ARGS__)

#define RCR_helper(BIT, a, b, la, sa, lb, sb) \
	uword x = la(a); \
	uword y = ((lb(b)) & 0x1f) % (BIT + 1); \
	if (y) { \
		uword cf = get_CF(cpu); \
		uword res = sext ## BIT((x >> y) | (cf << (BIT - y)) | (y != 1 ? (x << (BIT + 1 - y)) : 0)); \
		int cf1 = (sext ## BIT(x << (BIT - y)) >> (BIT - 1)) & 1; \
		int of1 = ((res ^ (res << 1)) >> (BIT - 1)) & 1; \
		SET_BIT(cpu->flags, cf1, CF); \
		SET_BIT(cpu->flags, of1, OF); \
		cpu->cc.mask &= ~(CF | OF); \
		sa(a, res); \
	}

#define RCRb(...) RCR_helper(8, __VA_ARGS__)
#define RCRw(...) RCR_helper(16, __VA_ARGS__)
#define RCRd(...) RCR_helper(32, __VA_ARGS__)

#define ROR_helper(BIT, a, b, la, sa, lb, sb) \
	uword x = la(a); \
	uword y0 = lb(b); \
	uword y = y0 & (BIT - 1); \
	uword res = x; \
	if (y) { \
		res = sext ## BIT((x >> y) | (x << (BIT - y))); \
		sa(a, res); \
	} \
	if (y0) { \
		int cf1 = (res >> (BIT - 1)) & 1; \
		int of1 = ((res ^ (res << 1)) >> (BIT - 1)) & 1; \
		SET_BIT(cpu->flags, cf1, CF); \
		SET_BIT(cpu->flags, of1, OF); \
		cpu->cc.mask &= ~(CF | OF); \
	}

#define RORb(...) ROR_helper(8, __VA_ARGS__)
#define RORw(...) ROR_helper(16, __VA_ARGS__)
#define RORd(...) ROR_helper(32, __VA_ARGS__)

#define SHR_helper(BIT, a, b, la, sa, lb, sb) \
	uword x = la(a); \
	uword y = (lb(b)) & 0x1f; \
	if (y) { \
		cpu->cc.src1 = sext ## BIT(x); \
		cpu->cc.dst = sext ## BIT(x >> y); \
		cpu->cc.dst2 = (x >> (y - 1)) & 1; \
		cpu->cc.op = CC_SHR; \
		cpu->cc.mask = CF | PF | ZF | SF | OF; \
		sa(a, cpu->cc.dst); \
	}

#define SHRb(...) SHR_helper(8, __VA_ARGS__)
#define SHRw(...) SHR_helper(16, __VA_ARGS__)
#define SHRd(...) SHR_helper(32, __VA_ARGS__)

#define SHLD_helper(BIT, a, b, c, la, sa, lb, sb, lc, sc) \
	int count = (lc(c)) & 0x1f; \
	uword x = la(a); \
	uword y = lb(b); \
	if (count) { \
		cpu->cc.src1 = sext ## BIT(x); \
		if (BIT < count) {  /* undocumented */ \
			uword z = x; x = y; y = z; \
			count -= BIT; \
		} \
		cpu->cc.dst = sext ## BIT((x << count) | (y >> (BIT - count))); \
		if (count == 1) { \
			cpu->cc.dst2 = sext ## BIT(x); \
		} else { \
			cpu->cc.dst2 = sext ## BIT((x << (count - 1)) | (count == 1 ? 0 : (y >> (BIT - (count - 1))))); \
		} \
		cpu->cc.op = CC_SHLD; \
		cpu->cc.mask = CF | PF | ZF | SF | OF; \
		sa(a, cpu->cc.dst); \
	}

#define SHLDw(...) SHLD_helper(16, __VA_ARGS__)
#define SHLDd(...) SHLD_helper(32, __VA_ARGS__)

#define SHRD_helper(BIT, a, b, c, la, sa, lb, sb, lc, sc) \
	int count = (lc(c)) & 0x1f; \
	uword x = la(a); \
	uword y = lb(b); \
	if (count) { \
		if (BIT < count) {  /* undocumented */ \
			uword z = x; x = y; y = z; \
			count -= BIT; \
		} \
		cpu->cc.src1 = sext ## BIT(x); \
		cpu->cc.dst = sext ## BIT((x >> count) | (y << (BIT - count))); \
		if (count == 1) { \
			cpu->cc.dst2 = sext ## BIT(x); \
		} else { \
			cpu->cc.dst2 = sext ## BIT((x >> (count - 1)) | (y << (BIT - (count - 1)))); \
		} \
		cpu->cc.op = CC_SHRD; \
		cpu->cc.mask = CF | PF | ZF | SF | OF; \
		sa(a, cpu->cc.dst); \
	}

#define SHRDw(...) SHRD_helper(16, __VA_ARGS__)
#define SHRDd(...) SHRD_helper(32, __VA_ARGS__)

// ">>"
#define SAR_helper(BIT, a, b, la, sa, lb, sb) \
	sword x = sext ## BIT(la(a)); \
	sword y = (lb(b)) & 0x1f; \
	if (y) { \
		cpu->cc.dst = x >> y; \
		cpu->cc.dst2 = (x >> (y - 1)) & 1; \
		cpu->cc.op = CC_SAR; \
		cpu->cc.mask = CF | PF | ZF | SF | OF; \
		sa(a, cpu->cc.dst); \
	}

#define SARb(...) SAR_helper(8, __VA_ARGS__)
#define SARw(...) SAR_helper(16, __VA_ARGS__)
#define SARd(...) SAR_helper(32, __VA_ARGS__)

#define IMUL2w(a, b, la, sa, lb, sb) \
	cpu->cc.src1 = sext16(la(a)); \
	cpu->cc.src2 = sext16(lb(b)); \
	cpu->cc.dst = cpu->cc.src1 * cpu->cc.src2; \
	cpu->cc.op = CC_IMUL16; \
	cpu->cc.mask = CF | PF | AF | ZF | SF | OF; \
	sa(a, cpu->cc.dst);

#define IMUL2d(a, b, la, sa, lb, sb) \
	cpu->cc.src1 = sext32(la(a)); \
	cpu->cc.src2 = sext32(lb(b)); \
	int64_t res = (int64_t) (s32) cpu->cc.src1 * (int64_t) (s32) cpu->cc.src2; \
	cpu->cc.dst = res; \
	cpu->cc.dst2 = res >> 32; \
	cpu->cc.op = CC_IMUL32; \
	cpu->cc.mask = CF | PF | AF | ZF | SF | OF; \
	sa(a, cpu->cc.dst);

#define IMUL2wI_helper(BIT, BITI, a, b, c, la, sa, lb, sb) \
	cpu->cc.src1 = sext ## BIT(lb(b)); \
	cpu->cc.src2 = sext ## BITI(c); \
	cpu->cc.dst = cpu->cc.src1 * cpu->cc.src2; \
	cpu->cc.op = CC_IMUL ## BIT; \
	cpu->cc.mask = CF | PF | AF | ZF | SF | OF; \
	sa(a, cpu->cc.dst);

#define IMUL2dI_helper(BIT, BITI, a, b, c, la, sa, lb, sb) \
	cpu->cc.src1 = sext ## BIT(lb(b)); \
	cpu->cc.src2 = sext ## BITI(c); \
	int64_t res = (int64_t) (s32) cpu->cc.src1 * (int64_t) (s32) cpu->cc.src2; \
	cpu->cc.dst = res; \
	cpu->cc.dst2 = res >> 32; \
	cpu->cc.op = CC_IMUL ## BIT; \
	cpu->cc.mask = CF | PF | AF | ZF | SF | OF; \
	sa(a, cpu->cc.dst);

#define IMUL2wIb(...) IMUL2wI_helper(16, 8, __VA_ARGS__)
#define IMUL2wIw(...) IMUL2wI_helper(16, 16, __VA_ARGS__)
#define IMUL2dIb(...) IMUL2dI_helper(32, 8, __VA_ARGS__)
#define IMUL2dId(...) IMUL2dI_helper(32, 32, __VA_ARGS__)

#define IMULb(a, la, sa) \
	cpu->cc.src1 = sext8(lreg8(0)); \
	cpu->cc.src2 = sext8(la(a)); \
	cpu->cc.dst = cpu->cc.src1 * cpu->cc.src2; \
	cpu->cc.op = CC_IMUL8; \
	cpu->cc.mask = CF | PF | AF | ZF | SF | OF; \
	sreg16(0, cpu->cc.dst);

#define IMULw(a, la, sa) \
	cpu->cc.src1 = sext16(lreg16(0)); \
	cpu->cc.src2 = sext16(la(a)); \
	cpu->cc.dst = cpu->cc.src1 * cpu->cc.src2; \
	cpu->cc.op = CC_IMUL16; \
	cpu->cc.mask = CF | PF | AF | ZF | SF | OF; \
	sreg16(0, cpu->cc.dst); \
	sreg16(2, (cpu->cc.dst >> 16));

#define IMULd(a, la, sa) \
	cpu->cc.src1 = sext32(lreg32(0)); \
	cpu->cc.src2 = sext32(la(a)); \
	int64_t res = (int64_t) (s32) cpu->cc.src1 * (int64_t) (s32) cpu->cc.src2; \
	cpu->cc.dst = res; \
	cpu->cc.dst2 = res >> 32; \
	cpu->cc.op = CC_IMUL32; \
	cpu->cc.mask = CF | PF | AF | ZF | SF | OF; \
	sreg32(0, cpu->cc.dst); \
	sreg32(2, cpu->cc.dst2);

#define MULb(a, la, sa) \
	cpu->cc.src1 = lreg8(0); \
	cpu->cc.src2 = la(a); \
	cpu->cc.dst = sext16(cpu->cc.src1 * cpu->cc.src2); \
	cpu->cc.op = CC_MUL8; \
	cpu->cc.mask = CF | PF | AF | ZF | SF | OF; \
	sreg16(0, cpu->cc.dst);

#define MULw(a, la, sa) \
	cpu->cc.src1 = lreg16(0); \
	cpu->cc.src2 = la(a); \
	cpu->cc.dst = cpu->cc.src1 * cpu->cc.src2; \
	cpu->cc.op = CC_MUL16; \
	cpu->cc.mask = CF | PF | AF | ZF | SF | OF; \
	sreg16(0, cpu->cc.dst); \
	sreg16(2, (cpu->cc.dst >> 16));

#define MULd(a, la, sa) \
	cpu->cc.src1 = lreg32(0); \
	cpu->cc.src2 = la(a); \
	uint64_t res = (uint64_t) cpu->cc.src1 * (uint64_t) cpu->cc.src2; \
	cpu->cc.dst = res; \
	cpu->cc.dst2 = res >> 32; \
	cpu->cc.op = CC_MUL32; \
	cpu->cc.mask = CF | PF | AF | ZF | SF | OF; \
	sreg32(0, cpu->cc.dst); \
	sreg32(2, cpu->cc.dst2);

#define IDIVb(a, la, sa) \
	sword src1 = sext16(lreg16(0)); \
	sword src2 = sext8(la(a)); \
	if (src2 == 0) THROW0(EX_DE); \
	sword res = src1 / src2; \
	if (res > 127 || res < -128) THROW0(EX_DE); \
	sreg8(0, res); \
	sreg8(4, src1 % src2);

#define IDIVw(a, la, sa) \
	sword src1 = sext32(lreg16(0) | (lreg16(2)<< 16)); \
	sword src2 = sext16(la(a)); \
	if (src2 == 0) THROW0(EX_DE); \
	sword res = src1 / src2; \
	if (res > 32767 || res < -32768) THROW0(EX_DE); \
	sreg16(0, res); \
	sreg16(2, src1 % src2);

#define IDIVd(a, la, sa) \
	int64_t src1 = (((uint64_t) lreg32(2)) << 32) | lreg32(0); \
	int64_t src2 = (sword) (la(a));	\
	if (src2 == 0) THROW0(EX_DE); \
	int64_t res = src1 / src2; \
	if (res > 2147483647 || res < -2147483648) THROW0(EX_DE); \
	sreg32(0, res); \
	sreg32(2, src1 % src2);

#define DIVb(a, la, sa) \
	uword src1 = lreg16(0); \
	uword src2 = la(a); \
	if (src2 == 0) THROW0(EX_DE); \
	uword res = src1 / src2; \
	if (res > 0xff) THROW0(EX_DE); \
	/* bypass the Cyrix 5/2 test */ \
	if (src1 == 0x5 && src2 == 0x2) { cpu->cc.mask &= ~ZF; cpu->flags |= ZF; } \
	sreg8(0, res); \
	sreg8(4, src1 % src2);

#define DIVw(a, la, sa) \
	uword src1 = lreg16(0) | (lreg16(2)<< 16); \
	uword src2 = la(a); \
	if (src2 == 0) THROW0(EX_DE); \
	uword res = src1 / src2; \
	if (res > 0xffff) THROW0(EX_DE); \
	/* bypass the NexGen 0x5555/2 test */ \
	if (src1 == 0x5555 && src2 == 0x2) { cpu->cc.mask &= ~ZF; cpu->flags &= ~ZF; } \
	sreg16(0, res); \
	sreg16(2, src1 % src2);

#define DIVd(a, la, sa) \
	uint64_t src1 = (((uint64_t) lreg32(2)) << 32) | lreg32(0); \
	uint64_t src2 = la(a); \
	if (src2 == 0) THROW0(EX_DE); \
	uint64_t res = src1 / src2; \
	if (res > 0xffffffff) THROW0(EX_DE); \
	sreg32(0, res); \
	sreg32(2, src1 % src2);

#define BT_helper(BIT, a, b, la, sa, lb, sb) \
	int bb = lb(b) % BIT; \
	bool bit = (la(a) >> bb) & 1; \
	cpu->cc.mask &= ~CF; \
	SET_BIT(cpu->flags, bit, CF);

#define BTw(...) BT_helper(16, __VA_ARGS__)
#define BTd(...) BT_helper(32, __VA_ARGS__)

#define BTX_helper(BIT, OP, a, b, la, sa, lb, sb) \
	int bb = lb(b) % BIT; \
	bool bit = (la(a) >> bb) & 1; \
	sa(a, la(a) OP (1 << bb)); \
	cpu->cc.mask &= ~CF; \
	SET_BIT(cpu->flags, bit, CF);

#define BTSw(...) BTX_helper(16, |, __VA_ARGS__)
#define BTSd(...) BTX_helper(32, |, __VA_ARGS__)
#define BTRw(...) BTX_helper(16, & ~, __VA_ARGS__)
#define BTRd(...) BTX_helper(32, & ~, __VA_ARGS__)
#define BTCw(...) BTX_helper(16, ^, __VA_ARGS__)
#define BTCd(...) BTX_helper(32, ^, __VA_ARGS__)

#define BSF_helper(BIT, a, b, la, sa, lb, sb) \
	u ## BIT src = lb(b); \
	u ## BIT temp = 0; \
	cpu->cc.mask = 0; \
	if (src == 0) { \
		cpu->flags |= ZF; \
	} else { \
		cpu->flags &= ~ZF; \
		while ((src & 1) == 0) { \
			temp++; \
			src >>= 1; \
		} \
		sa(a, temp); \
	}

#define BSFw(...) BSF_helper(16, __VA_ARGS__)
#define BSFd(...) BSF_helper(32, __VA_ARGS__)

#define BSR_helper(BIT, a, b, la, sa, lb, sb) \
	s ## BIT src = lb(b); \
	u ## BIT temp = BIT - 1; \
	cpu->cc.mask = 0; \
	if (src == 0) { \
		cpu->flags |= ZF; \
	} else { \
		cpu->flags &= ~ZF; \
		while (src >= 0) { \
			temp--; \
			src <<= 1; \
		} \
		sa(a, temp); \
	}

#define BSRw(...) BSR_helper(16, __VA_ARGS__)
#define BSRd(...) BSR_helper(32, __VA_ARGS__)

#define MOVb(a, b, la, sa, lb, sb) sa(a, lb(b));
#define MOVw(a, b, la, sa, lb, sb) sa(a, lb(b));
#define MOVd(a, b, la, sa, lb, sb) sa(a, lb(b));
#define MOVSeg(a, b, la, sa, lb, sb) \
	if (a == SEG_CS) THROW0(EX_UD); \
	if (a == SEG_SS) stepcount++; \
	TRY(set_seg(cpu, a, lb(b)));
#define MOVZXdb(a, b, la, sa, lb, sb) sa(a, lb(b));
#define MOVZXwb(a, b, la, sa, lb, sb) sa(a, lb(b));
#define MOVZXww(a, b, la, sa, lb, sb) sa(a, lb(b));
#define MOVZXdw(a, b, la, sa, lb, sb) sa(a, lb(b));
#define MOVSXdb(a, b, la, sa, lb, sb) sa(a, sext8(lb(b)));
#define MOVSXwb(a, b, la, sa, lb, sb) sa(a, sext8(lb(b)));
#define MOVSXww(a, b, la, sa, lb, sb) sa(a, lb(b));
#define MOVSXdw(a, b, la, sa, lb, sb) sa(a, sext16(lb(b)));

#define XCHG(a, b, la, sa, lb, sb) \
	uword tmp = lb(b); \
	sb(b, la(a)); \
	sa(a, tmp);
#define XCHGb(...) XCHG(__VA_ARGS__)
#define XCHGw(...) XCHG(__VA_ARGS__)
#define XCHGd(...) XCHG(__VA_ARGS__)

#define XCHGAX() \
	if (opsz16) { \
		int reg = b1 & 7; \
		uword tmp = lreg16(reg); \
		sreg16(reg, lreg16(0)); \
		sreg16(0, tmp); \
	} else { \
		int reg = b1 & 7; \
		uword tmp = lreg32(reg); \
		sreg32(reg, lreg32(0)); \
		sreg32(0, tmp); \
	}

#define LEAd(a, b, la, sa, lb, sb) \
	if (mod == 3) THROW0(EX_UD); \
	sa(a, lb(b));
#define LEAw LEAd

#define CBW_CWDE() \
	if (opsz16) sreg16(0, sext8(lreg8(0))); \
	else sreg32(0, sext16(lreg16(0)));

#define CWD_CDQ() \
	if (opsz16) sreg16(2, sext16(-(sext16(lreg16(0)) >> 31))); \
	else sreg32(2, sext32(-(sext32(lreg32(0)) >> 31)));

#define MOVFC() \
	if (cpu->cpl != 0) THROW(EX_GP, 0); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	int rm = modrm & 7; \
	if (reg == 0) { \
		sreg32(rm, cpu->cr0); \
	} else if (reg == 2) { \
		sreg32(rm, cpu->cr2); \
	} else if (reg == 3) { \
		sreg32(rm, cpu->cr3); \
	} else if (reg == 4) { \
		sreg32(rm, 0); \
	} else THROW0(EX_UD);

#define MOVTC() \
	if (cpu->cpl != 0) THROW(EX_GP, 0); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	int rm = modrm & 7; \
	if (reg == 0) { \
		u32 new_cr0 = lreg32(rm); \
		if ((new_cr0 ^ cpu->cr0) & (CR0_PG | CR0_WP | 1)) \
			tlb_clear(cpu); \
		if (cpu->fpu) new_cr0 |= 0x10; \
		cpu->cr0 = new_cr0; \
	} else if (reg == 2) { \
		cpu->cr2 = lreg32(rm); \
	} else if (reg == 3) { \
		cpu->cr3 = lreg32(rm); \
		tlb_clear(cpu); \
	} else if (reg == 4) { \
	} else THROW0(EX_UD);

#define INT3() \
	cpu->ip = cpu->next_ip; \
	THROW0(EX_BP);

#define INTO() \
	if (get_OF(cpu)) { \
		cpu->ip = cpu->next_ip; \
		THROW0(EX_OF); \
	}

static bool call_isr(CPUI386 *cpu, int no, bool pusherr, int ext);

#define INT(i, li, _) \
	/*dolog("int %02x %08x %04x:%08x\n", li(i), REGi[0], SEGi(SEG_CS), cpu->ip);*/ \
	if ((cpu->flags & VM)) { \
		if(get_IOPL(cpu) < 3) THROW(EX_GP, 0); \
	} \
	uword oldip = cpu->ip; \
	cpu->ip = cpu->next_ip; \
	if (!call_isr(cpu, li(i), false, 0)) { \
		cpu->ip = oldip; \
		return false; \
	}

#define IRET() \
	if ((cpu->cr0 & 1) && (!(cpu->flags & VM) || get_IOPL(cpu) < 3)) { \
		TRY(pmret(cpu, opsz16, 0, true)); \
	} else { \
		if (!opsz16) cpu_abort(cpu, -201); \
		OptAddr meml1, meml2, meml3; \
		uword sp = lreg32(4); \
		/* ip */ TRY(translate16(cpu, &meml1, 1, SEG_SS, sp & sp_mask)); \
		uword newip = laddr16(&meml1); \
		/* cs */ TRY(translate16(cpu, &meml2, 1, SEG_SS, (sp + 2) & sp_mask)); \
		int newcs = laddr16(&meml2); \
		/* flags */ TRY(translate16(cpu, &meml3, 1, SEG_SS, (sp + 4) & sp_mask)); \
		uword oldflags = cpu->flags; \
		if (cpu->flags & VM) cpu->flags = (cpu->flags & (0xffff0000 | IOPL)) | (laddr16(&meml3) & ~IOPL); \
		else cpu->flags = (cpu->flags & 0xffff0000) | laddr16(&meml3); \
		cpu->flags &= EFLAGS_MASK; \
		cpu->flags |= 0x2; \
		if (!set_seg(cpu, SEG_CS, newcs)) { cpu->flags = oldflags; return false; } \
		cpu->cc.mask = 0; \
		set_sp(sp + 6, sp_mask); \
		cpu->next_ip = newip; \
	} \
	if (cpu->intr && (cpu->flags & IF)) return true;

#define RETFARw(i, li, _) \
	if ((cpu->cr0 & 1) && !(cpu->flags & VM)) { \
		TRY(pmret(cpu, opsz16, li(i), false)); \
	} else { \
		if (opsz16) { \
			OptAddr meml1, meml2; \
			uword sp = lreg32(4); \
			/* ip */ TRY(translate16(cpu, &meml1, 1, SEG_SS, sp & sp_mask)); \
			uword newip = laddr16(&meml1); \
			/* cs */ TRY(translate16(cpu, &meml2, 1, SEG_SS, (sp + 2) & sp_mask)); \
			int newcs = laddr16(&meml2); \
			TRY(set_seg(cpu, SEG_CS, newcs)); \
			set_sp(sp + 4 + li(i), sp_mask); \
			cpu->next_ip = newip; \
		} else { \
			OptAddr meml1, meml2; \
			uword sp = lreg32(4); \
			/* ip */ TRY(translate32(cpu, &meml1, 1, SEG_SS, sp & sp_mask)); \
			uword newip = laddr32(&meml1); \
			/* cs */ TRY(translate32(cpu, &meml2, 1, SEG_SS, (sp + 4) & sp_mask)); \
			int newcs = laddr32(&meml2); \
			TRY(set_seg(cpu, SEG_CS, newcs)); \
			set_sp(sp + 8 + li(i), sp_mask); \
			cpu->next_ip = newip; \
		} \
	}

#define RETFAR() RETFARw(0, limm, 0)

#define HLT() \
	if (cpu->cpl != 0) THROW(EX_GP, 0); \
	cpu->halt = true; return true;
#define NOP()

#define LAHF() \
	refresh_flags(cpu); \
	cpu->cc.mask = 0; \
	sreg8(4, cpu->flags);

#define SAHF() \
	cpu->cc.mask &= OF; \
	cpu->flags = (cpu->flags & (wordmask ^ 0xff)) | lreg8(4); \
	cpu->flags &= EFLAGS_MASK; \
	cpu->flags |= 0x2;

#define CMC() \
	int cf = get_CF(cpu); \
	cpu->cc.mask &= ~CF; \
	SET_BIT(cpu->flags, !cf, CF);

#define CLC() \
	cpu->cc.mask &= ~CF; \
	cpu->flags &= ~CF;

#define STC() \
	cpu->cc.mask &= ~CF; \
	cpu->flags |= CF;

#define CLI() \
	if (get_IOPL(cpu) < cpu->cpl) THROW(EX_GP, 0); \
	cpu->flags &= ~IF;

/* STI: interrupts enabled at the end of the **next** instruction */
#define STI() \
	if (get_IOPL(cpu) < cpu->cpl) THROW(EX_GP, 0); \
	cpu->flags |= IF; \
	if (cpu->intr || stepcount < 2) stepcount = 2;

#define CLD() \
	cpu->flags &= ~DF;

#define STD() \
	cpu->flags |= DF;

#define PUSHb(a, la, sa) \
	OptAddr meml1; \
	uword sp = lreg32(4); \
	uword val = sext8(la(a)); \
	if (opsz16) { \
		TRY(translate16(cpu, &meml1, 2, SEG_SS, (sp - 2) & sp_mask)); \
		set_sp(sp - 2, sp_mask); \
		saddr16(&meml1, val); \
	} else { \
		TRY(translate32(cpu, &meml1, 2, SEG_SS, (sp - 4) & sp_mask)); \
		set_sp(sp - 4, sp_mask); \
		saddr32(&meml1, val); \
	}

#define PUSHw(a, la, sa) \
	OptAddr meml1; \
	uword sp = lreg32(4); \
	uword val = sext16(la(a)); \
	TRY(translate16(cpu, &meml1, 2, SEG_SS, (sp - 2) & sp_mask)); \
	set_sp(sp - 2, sp_mask); \
	saddr16(&meml1, val);

#define PUSHd(a, la, sa) \
	OptAddr meml1; \
	uword sp = lreg32(4); \
	uword val = sext32(la(a)); \
	TRY(translate32(cpu, &meml1, 2, SEG_SS, (sp - 4) & sp_mask)); \
	set_sp(sp - 4, sp_mask); \
	saddr32(&meml1, val);

#define POPRegw(a, la, sa) \
	OptAddr meml1; \
	uword sp = lreg32(4); \
	TRY(translate16(cpu, &meml1, 1, SEG_SS, sp & sp_mask)); \
	u16 src = laddr16(&meml1); \
	set_sp(sp + 2, sp_mask); \
	sa(a, src);

#define POPRegd(a, la, sa) \
	OptAddr meml1; \
	uword sp = lreg32(4); \
	TRY(translate32(cpu, &meml1, 1, SEG_SS, sp & sp_mask)); \
	u32 src = laddr32(&meml1); \
	set_sp(sp + 4, sp_mask); \
	sa(a, src);

#define POP_helper(BIT) \
	OptAddr meml1; \
	TRY(fetch8(cpu, &modrm)); \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	uword sp = lreg32(4); \
	TRY(translate ## BIT(cpu, &meml1, 1, SEG_SS, sp & sp_mask)); \
	u ## BIT src = laddr ## BIT(&meml1); \
	set_sp(sp + BIT / 8, sp_mask); \
	if (mod == 3) { \
		sreg ## BIT(rm, src); \
	} else { \
		if (!modsib(cpu, adsz16, mod, rm, &addr, &curr_seg) || \
		    !translate ## BIT(cpu, &meml, 2, curr_seg, addr)) { \
			set_sp(sp, sp_mask); \
			return false; \
		} \
		saddr ## BIT(&meml, src); \
	}

#define POP() if (opsz16) { POP_helper(16) } else { POP_helper(32) }

#define PUSHF() \
	if ((cpu->flags & VM) && get_IOPL(cpu) < 3) THROW(EX_GP, 0); \
	if (opsz16) { \
		uword sp = lreg32(4); \
		TRY(translate16(cpu, &meml, 2, SEG_SS, (sp - 2) & sp_mask)); \
		refresh_flags(cpu); \
		cpu->cc.mask = 0; \
		set_sp(sp - 2, sp_mask); \
		saddr16(&meml, cpu->flags); \
	} else { \
		uword sp = lreg32(4); \
		TRY(translate32(cpu, &meml, 2, SEG_SS, (sp - 4) & sp_mask)); \
		refresh_flags(cpu); \
		cpu->cc.mask = 0; \
		set_sp(sp - 4, sp_mask); \
		saddr32(&meml, cpu->flags & ~(RF | VM)); \
	}

#define EFLAGS_MASK_386 0x37fd7
#define EFLAGS_MASK_486 0x77fd7
#define EFLAGS_MASK_586 0x277fd7
#define EFLAGS_MASK (cpu->flags_mask)

#define POPF() \
	if ((cpu->flags & VM) && get_IOPL(cpu) < 3) THROW(EX_GP, 0); \
	uword mask = VM; \
	if (cpu->cr0 & 1) { \
		if (cpu->cpl > 0) mask |= IOPL; \
		if (get_IOPL(cpu) < cpu->cpl) mask |= IF; \
	} \
	if (opsz16) { \
		uword sp = lreg32(4); \
		TRY(translate16(cpu, &meml, 1, SEG_SS, sp & sp_mask)); \
		set_sp(sp + 2, sp_mask); \
		cpu->flags = (cpu->flags & (0xffff0000 | mask)) | (laddr16(&meml) & ~mask); \
	} else { \
		uword sp = lreg32(4); \
		TRY(translate32(cpu, &meml, 1, SEG_SS, sp & sp_mask)); \
		set_sp(sp + 4, sp_mask); \
		cpu->flags = (cpu->flags & mask) | (laddr32(&meml) & ~mask); \
	} \
	cpu->flags &= EFLAGS_MASK; \
	cpu->flags |= 0x2; \
	cpu->cc.mask = 0; \
	if (cpu->intr && (cpu->flags & IF)) return true;

#define PUSHSeg(seg) \
	if (opsz16) { \
		uword sp = lreg32(4); \
		TRY(translate16(cpu, &meml, 2, SEG_SS, (sp - 2) & sp_mask)); \
		set_sp(sp - 2, sp_mask); \
		saddr16(&meml, lseg(seg)); \
	} else { \
		uword sp = lreg32(4); \
		TRY(translate16(cpu, &meml, 2, SEG_SS, (sp - 4) & sp_mask)); \
		set_sp(sp - 4, sp_mask); \
		saddr16(&meml, lseg(seg)); \
	}
#define PUSH_ES() PUSHSeg(SEG_ES)
#define PUSH_CS() PUSHSeg(SEG_CS)
#define PUSH_SS() PUSHSeg(SEG_SS)
#define PUSH_DS() PUSHSeg(SEG_DS)
#define PUSH_FS() PUSHSeg(SEG_FS)
#define PUSH_GS() PUSHSeg(SEG_GS)

#define POPSeg(seg) \
	if (opsz16) { \
		uword sp = lreg32(4); \
		TRY(translate16(cpu, &meml, 1, SEG_SS, sp & sp_mask)); \
		TRY(set_seg(cpu, seg, laddr16(&meml))); \
		set_sp(sp + 2, sp_mask); \
	} else { \
		uword sp = lreg32(4); \
		TRY(translate16(cpu, &meml, 1, SEG_SS, sp & sp_mask)); \
		TRY(set_seg(cpu, seg, laddr16(&meml))); \
		set_sp(sp + 4, sp_mask); \
	}
#define POP_ES() POPSeg(SEG_ES)
#define POP_SS() POPSeg(SEG_SS) stepcount++;
#define POP_DS() POPSeg(SEG_DS)
#define POP_FS() POPSeg(SEG_FS)
#define POP_GS() POPSeg(SEG_GS)

#define PUSHA_helper(BIT, BYTE) \
	uword sp = lreg32(4); \
	OptAddr meml1, meml2, meml3, meml4; \
	OptAddr meml5, meml6, meml7, meml8; \
	TRY(translate ## BIT(cpu, &meml1, 2, SEG_SS, (sp - BYTE * 1) & sp_mask)); \
	TRY(translate ## BIT(cpu, &meml2, 2, SEG_SS, (sp - BYTE * 2) & sp_mask)); \
	TRY(translate ## BIT(cpu, &meml3, 2, SEG_SS, (sp - BYTE * 3) & sp_mask)); \
	TRY(translate ## BIT(cpu, &meml4, 2, SEG_SS, (sp - BYTE * 4) & sp_mask)); \
	TRY(translate ## BIT(cpu, &meml5, 2, SEG_SS, (sp - BYTE * 5) & sp_mask)); \
	TRY(translate ## BIT(cpu, &meml6, 2, SEG_SS, (sp - BYTE * 6) & sp_mask)); \
	TRY(translate ## BIT(cpu, &meml7, 2, SEG_SS, (sp - BYTE * 7) & sp_mask)); \
	TRY(translate ## BIT(cpu, &meml8, 2, SEG_SS, (sp - BYTE * 8) & sp_mask)); \
	saddr ## BIT(&meml1, lreg ## BIT(0)); \
	saddr ## BIT(&meml2, lreg ## BIT(1)); \
	saddr ## BIT(&meml3, lreg ## BIT(2)); \
	saddr ## BIT(&meml4, lreg ## BIT(3)); \
	saddr ## BIT(&meml5, sp); \
	saddr ## BIT(&meml6, lreg ## BIT(5)); \
	saddr ## BIT(&meml7, lreg ## BIT(6)); \
	saddr ## BIT(&meml8, lreg ## BIT(7)); \
	set_sp(sp - BYTE * 8, sp_mask);
#define PUSHA() if (opsz16) { PUSHA_helper(16, 2) } else { PUSHA_helper(32, 4) }

#define POPA_helper(BIT, BYTE) \
	uword sp = lreg32(4); \
	OptAddr meml1, meml2, meml3, meml4; \
	OptAddr meml5, meml6, meml7; \
	TRY(translate ## BIT(cpu, &meml1, 1, SEG_SS, (sp + BYTE * 0) & sp_mask)); \
	TRY(translate ## BIT(cpu, &meml2, 1, SEG_SS, (sp + BYTE * 1) & sp_mask)); \
	TRY(translate ## BIT(cpu, &meml3, 1, SEG_SS, (sp + BYTE * 2) & sp_mask)); \
	TRY(translate ## BIT(cpu, &meml4, 1, SEG_SS, (sp + BYTE * 4) & sp_mask)); \
	TRY(translate ## BIT(cpu, &meml5, 1, SEG_SS, (sp + BYTE * 5) & sp_mask)); \
	TRY(translate ## BIT(cpu, &meml6, 1, SEG_SS, (sp + BYTE * 6) & sp_mask)); \
	TRY(translate ## BIT(cpu, &meml7, 1, SEG_SS, (sp + BYTE * 7) & sp_mask)); \
	sreg ## BIT(7, laddr ## BIT(&meml1)); \
	sreg ## BIT(6, laddr ## BIT(&meml2)); \
	sreg ## BIT(5, laddr ## BIT(&meml3)); \
	sreg ## BIT(3, laddr ## BIT(&meml4)); \
	sreg ## BIT(2, laddr ## BIT(&meml5)); \
	sreg ## BIT(1, laddr ## BIT(&meml6)); \
	sreg ## BIT(0, laddr ## BIT(&meml7)); \
	set_sp(sp + BYTE * 8, sp_mask);
#define POPA() if (opsz16) { POPA_helper(16, 2) } else { POPA_helper(32, 4) }

// string operations
#define stdi(BIT, ABIT) \
	TRY(translate ## BIT(cpu, &meml, 2, SEG_ES, lreg ## ABIT(7))); \
	saddr ## BIT(&meml, ax); \
	sreg ## ABIT(7, lreg ## ABIT(7) + dir);

#define ldsi(BIT, ABIT) \
	TRY(translate ## BIT(cpu, &meml, 1, curr_seg, lreg ## ABIT(6))); \
	ax = laddr ## BIT(&meml); \
	sreg ## ABIT(6, lreg ## ABIT(6) + dir);

#define lddi(BIT, ABIT) \
	TRY(translate ## BIT(cpu, &meml, 1, SEG_ES, lreg ## ABIT(7))); \
	ax = laddr ## BIT(&meml); \
	sreg ## ABIT(7, lreg ## ABIT(7) + dir);

#define ldsistdi(BIT, ABIT) \
	TRY(translate ## BIT(cpu, &meml, 1, curr_seg, lreg ## ABIT(6))); \
	ax = laddr ## BIT(&meml); \
	TRY(translate ## BIT(cpu, &meml, 2, SEG_ES, lreg ## ABIT(7))); \
	saddr ## BIT(&meml, ax); \
	sreg ## ABIT(6, lreg ## ABIT(6) + dir); \
	sreg ## ABIT(7, lreg ## ABIT(7) + dir);

#define ldsilddi(BIT, ABIT) \
	TRY(translate ## BIT(cpu, &meml, 1, curr_seg, lreg ## ABIT(6))); \
	ax0 = laddr ## BIT(&meml); \
	TRY(translate ## BIT(cpu, &meml, 1, SEG_ES, lreg ## ABIT(7))); \
	ax = laddr ## BIT(&meml); \
	sreg ## ABIT(6, lreg ## ABIT(6) + dir); \
	sreg ## ABIT(7, lreg ## ABIT(7) + dir);

#define xdir8 int dir = (cpu->flags & DF) ? -1 : 1;
#define xdir16 int dir = (cpu->flags & DF) ? -2 : 2;
#define xdir32 int dir = (cpu->flags & DF) ? -4 : 4;

#define STOS_helper2(BIT, ABIT) \
	OptAddr memld; \
	uword cx = lreg ## ABIT(1); \
	while (cx) { \
		TRY(translate ## BIT(cpu, &memld, 2, SEG_ES, lreg ## ABIT(7))); \
		if (memld.addr1 % (BIT / 8)) { \
			/* slow path */ \
			while (lreg ## ABIT(1)) { \
				stdi(BIT, ABIT) \
				sreg ## ABIT(1, lreg ## ABIT(1) - 1); \
			} \
			break; \
		} \
		uword count = cx; \
		int countd; \
		if (dir > 0) countd = (4096 - (memld.addr1 & 4095)) / (BIT / 8); \
		else countd = 1 + (memld.addr1 & 4095) / (BIT / 8); \
		if (countd < count) \
			count = countd; \
		for (uword i = 0; i <= count - 1; i++) { \
			saddr ## BIT(&memld, ax); \
			memld.addr1 += dir; \
		} \
		sreg ## ABIT(7, lreg ## ABIT(7) + count * dir); \
		sreg ## ABIT(1, cx - count); \
		cx = lreg ## ABIT(1); \
	}

#define STOS_helper(BIT) \
	if (curr_seg == -1) curr_seg = SEG_DS; \
	xdir ## BIT \
	u ## BIT ax = REGi(0); \
	if (rep == 0) { \
		if (adsz16) { stdi(BIT, 16) } else { stdi(BIT, 32) } \
	} else { \
		if (adsz16) { STOS_helper2(BIT, 16) } else { STOS_helper2(BIT, 32) } \
	}

#define LODS_helper(BIT) \
	if (curr_seg == -1) curr_seg = SEG_DS; \
	xdir ## BIT \
	u ## BIT ax; \
	if (rep == 0) { \
		if (adsz16) { ldsi(BIT, 16) } else { ldsi(BIT, 32) } \
		sreg ## BIT(0, ax); \
	} else { \
		if (adsz16) { \
			while (lreg16(1)) { \
				ldsi(BIT, 16) \
				sreg ## BIT(0, ax); \
				sreg16(1, lreg16(1) - 1); \
			} \
		} else { \
			while (lreg32(1)) { \
				ldsi(BIT, 32) \
				sreg ## BIT(0, ax); \
				sreg32(1, lreg32(1) - 1); \
			} \
		} \
	}

#define SCAS_helper(BIT) \
	if (curr_seg == -1) curr_seg = SEG_DS; \
	xdir ## BIT \
	u ## BIT ax0 = REGi(0); \
	u ## BIT ax; \
	if (rep == 0) { \
		if (adsz16) { lddi(BIT, 16) } else { lddi(BIT, 32) } \
		cpu->cc.src1 = sext ## BIT(ax0); \
		cpu->cc.src2 = sext ## BIT(ax); \
		cpu->cc.dst = sext ## BIT(cpu->cc.src1 - cpu->cc.src2); \
		cpu->cc.op = CC_SUB; \
		cpu->cc.mask = CF | PF | AF | ZF | SF | OF; \
	} else { \
		if (adsz16) { \
			while (lreg16(1)) { \
				lddi(BIT, 16) \
				sreg16(1, lreg16(1) - 1); \
				cpu->cc.src1 = sext ## BIT(ax0); \
				cpu->cc.src2 = sext ## BIT(ax); \
				cpu->cc.dst = sext ## BIT(cpu->cc.src1 - cpu->cc.src2); \
				cpu->cc.op = CC_SUB; \
				cpu->cc.mask = CF | PF | AF | ZF | SF | OF; \
				bool zf = get_ZF(cpu); \
				if ((zf && rep == 2) || (!zf && rep == 1)) break; \
			} \
		} else { \
			while (lreg32(1)) { \
				lddi(BIT, 32) \
				sreg32(1, lreg32(1) - 1); \
				cpu->cc.src1 = sext ## BIT(ax0); \
				cpu->cc.src2 = sext ## BIT(ax); \
				cpu->cc.dst = sext ## BIT(cpu->cc.src1 - cpu->cc.src2); \
				cpu->cc.op = CC_SUB; \
				cpu->cc.mask = CF | PF | AF | ZF | SF | OF; \
				bool zf = get_ZF(cpu); \
				if ((zf && rep == 2) || (!zf && rep == 1)) break; \
			} \
		} \
	}

#define MOVS_helper2(BIT, ABIT) \
	OptAddr memls, memld; \
	uword cx = lreg ## ABIT(1); \
	while (cx) { \
		TRY(translate ## BIT(cpu, &memls, 1, curr_seg, lreg ## ABIT(6))); \
		TRY(translate ## BIT(cpu, &memld, 2, SEG_ES, lreg ## ABIT(7))); \
		if (memls.addr1 % (BIT / 8) || memld.addr1 % (BIT / 8)) { \
			/* slow path */ \
			while (lreg ## ABIT(1)) { \
				ldsistdi(BIT, ABIT) \
				sreg ## ABIT(1, lreg ## ABIT(1) - 1); \
			} \
			break; \
		} \
		uword count = cx; \
		int counts, countd; \
		if (dir > 0) { \
			counts = (4096 - (memls.addr1 & 4095)) / (BIT / 8); \
			countd = (4096 - (memld.addr1 & 4095)) / (BIT / 8); \
		} else { \
			counts = 1 + (memls.addr1 & 4095) / (BIT / 8); \
			countd = 1 + (memld.addr1 & 4095) / (BIT / 8); \
		} \
		if (counts < count) \
			count = counts; \
		if (countd < count) \
			count = countd; \
		if (cpu->cb.iomem_write_string && in_iomem(memld.addr1) && \
		    dir > 0  && in_iomem(memld.addr1 + count - 1) && \
		    (memls.addr1 | 4095) < cpu->phys_mem_size && \
		    !in_iomem(memls.addr1) && !in_iomem(memls.addr1 | 4095)) { \
			if (cpu->cb.iomem_write_string( \
				    cpu->cb.iomem, memld.addr1, \
				    cpu->phys_mem + memls.addr1, count * dir)) { \
				sreg ## ABIT(6, lreg ## ABIT(6) + count * dir); \
				sreg ## ABIT(7, lreg ## ABIT(7) + count * dir); \
				sreg ## ABIT(1, cx - count); \
				cx = lreg ## ABIT(1); \
				continue; \
			} \
		} \
		for (uword i = 0; i <= count - 1; i++) { \
			store ## BIT(cpu, &memld, load ## BIT(cpu, &memls)); \
			memld.addr1 += dir; \
			memls.addr1 += dir; \
		} \
		sreg ## ABIT(6, lreg ## ABIT(6) + count * dir); \
		sreg ## ABIT(7, lreg ## ABIT(7) + count * dir); \
		sreg ## ABIT(1, cx - count); \
		cx = lreg ## ABIT(1); \
	}

#define MOVS_helper(BIT) \
	if (curr_seg == -1) curr_seg = SEG_DS; \
	xdir ## BIT \
	u ## BIT ax; \
	if (rep == 0) { \
		if (adsz16) { ldsistdi(BIT, 16) } else { ldsistdi(BIT, 32) } \
	} else { \
		if (adsz16) { MOVS_helper2(BIT, 16) } else { MOVS_helper2(BIT, 32) } \
	}

#define CMPS_helper(BIT) \
	if (curr_seg == -1) curr_seg = SEG_DS; \
	xdir ## BIT \
	u ## BIT ax0, ax; \
	if (rep == 0) { \
		if (adsz16) { ldsilddi(BIT, 16) } else { ldsilddi(BIT, 32) } \
		cpu->cc.src1 = sext ## BIT(ax0); \
		cpu->cc.src2 = sext ## BIT(ax); \
		cpu->cc.dst = sext ## BIT(cpu->cc.src1 - cpu->cc.src2); \
		cpu->cc.op = CC_SUB; \
		cpu->cc.mask = CF | PF | AF | ZF | SF | OF; \
	} else { \
		if (adsz16) { \
			while (lreg16(1)) { \
				ldsilddi(BIT, 16) \
				sreg16(1, lreg16(1) - 1); \
				cpu->cc.src1 = sext ## BIT(ax0); \
				cpu->cc.src2 = sext ## BIT(ax); \
				cpu->cc.dst = sext ## BIT(cpu->cc.src1 - cpu->cc.src2); \
				cpu->cc.op = CC_SUB; \
				cpu->cc.mask = CF | PF | AF | ZF | SF | OF; \
				bool zf = get_ZF(cpu); \
				if ((zf && rep == 2) || (!zf && rep == 1)) break; \
			} \
		} else { \
			while (lreg32(1)) { \
				ldsilddi(BIT, 32) \
				sreg32(1, lreg32(1) - 1); \
				cpu->cc.src1 = sext ## BIT(ax0); \
				cpu->cc.src2 = sext ## BIT(ax); \
				cpu->cc.dst = sext ## BIT(cpu->cc.src1 - cpu->cc.src2); \
				cpu->cc.op = CC_SUB; \
				cpu->cc.mask = CF | PF | AF | ZF | SF | OF; \
				bool zf = get_ZF(cpu); \
				if ((zf && rep == 2) || (!zf && rep == 1)) break; \
			} \
		} \
	}

#define STOSb() STOS_helper(8)
#define LODSb() LODS_helper(8)
#define SCASb() SCAS_helper(8)
#define MOVSb() MOVS_helper(8)
#define CMPSb() CMPS_helper(8)
#define STOS() if (opsz16) { STOS_helper(16) } else { STOS_helper(32) }
#define LODS() if (opsz16) { LODS_helper(16) } else { LODS_helper(32) }
#define SCAS() if (opsz16) { SCAS_helper(16) } else { SCAS_helper(32) }
#define MOVS() if (opsz16) { MOVS_helper(16) } else { MOVS_helper(32) }
#define CMPS() if (opsz16) { CMPS_helper(16) } else { CMPS_helper(32) }

#define indxstdi(BIT, ABIT) \
	TRY(translate ## BIT(cpu, &meml, 2, SEG_ES, lreg ## ABIT(7))); \
	ax = cpu->cb.io_read ## BIT(cpu->cb.io, lreg16(2)); \
	saddr ## BIT(&meml, ax); \
	sreg ## ABIT(7, lreg ## ABIT(7) + dir);

#define INS_helper2(BIT, ABIT) \
	OptAddr memld; \
	uword cx = lreg ## ABIT(1); \
	while (cx) { \
		TRY(translate ## BIT(cpu, &memld, 2, SEG_ES, lreg ## ABIT(7))); \
		if (memld.addr1 % (BIT / 8)) { \
			/* slow path */ \
			while (lreg ## ABIT(1)) { \
				indxstdi(BIT, ABIT) \
				sreg ## ABIT(1, lreg ## ABIT(1) - 1); \
			} \
			break; \
		} \
		uword count = cx; \
		int countd; \
		if (dir > 0) countd = (4096 - (memld.addr1 & 4095)) / (BIT / 8); \
		else countd = 1 + (memld.addr1 & 4095) / (BIT / 8); \
		if (countd < count) \
			count = countd; \
		if (cpu->cb.io_read_string && dir > 0 && \
		    (memld.addr1 | 4095) < cpu->phys_mem_size && \
		    !in_iomem(memld.addr1) && !in_iomem(memld.addr1 | 4095)) { \
			int count1 = cpu->cb.io_read_string( \
				cpu->cb.io, lreg16(2), \
				cpu->phys_mem + memld.addr1, dir, count); \
			if (count1 > 0) { \
				count = count1; \
				sreg ## ABIT(7, lreg ## ABIT(7) + count * dir); \
				sreg ## ABIT(1, cx - count); \
				cx = lreg ## ABIT(1); \
				continue; \
			} \
		} \
		for (uword i = 0; i <= count - 1; i++) { \
			ax = cpu->cb.io_read ## BIT(cpu->cb.io, lreg16(2)); \
			saddr ## BIT(&memld, ax); \
			memld.addr1 += dir; \
		} \
		sreg ## ABIT(7, lreg ## ABIT(7) + count * dir); \
		sreg ## ABIT(1, cx - count); \
		cx = lreg ## ABIT(1); \
	}

#define INS_helper(BIT) \
	TRY(check_ioperm(cpu, lreg16(2), BIT)); \
	xdir ## BIT \
	u ## BIT ax; \
	if (rep == 0) { \
		if (adsz16) { indxstdi(BIT, 16) } else { indxstdi(BIT, 32) } \
	} else { \
		if (rep != 1) THROW0(EX_UD); \
		if (adsz16) { INS_helper2(BIT, 16) } else { INS_helper2(BIT, 32) } \
	}

#define INSb() INS_helper(8)
#define INS() if (opsz16) { INS_helper(16) } else { INS_helper(32) }

#define ldsioutdx(BIT, ABIT) \
	TRY(translate ## BIT(cpu, &meml, 1, curr_seg, lreg ## ABIT(6))); \
	ax = laddr ## BIT(&meml); \
	cpu->cb.io_write ## BIT(cpu->cb.io, lreg16(2), ax); \
	sreg ## ABIT(6, lreg ## ABIT(6) + dir);

#define OUTS_helper2(BIT, ABIT) \
	OptAddr memls; \
	uword cx = lreg ## ABIT(1); \
	while (cx) { \
		TRY(translate ## BIT(cpu, &memls, 1, curr_seg, lreg ## ABIT(6))); \
		if (memls.addr1 % (BIT / 8)) { \
			/* slow path */ \
			while (lreg ## ABIT(1)) { \
				ldsioutdx(BIT, ABIT) \
				sreg ## ABIT(1, lreg ## ABIT(1) - 1); \
			} \
			break; \
		} \
		uword count = cx; \
		int counts; \
		if (dir > 0) counts = (4096 - (memls.addr1 & 4095)) / (BIT / 8); \
		else counts = 1 + (memls.addr1 & 4095) / (BIT / 8); \
		if (counts < count) \
			count = counts; \
		if (cpu->cb.io_write_string && dir > 0 && \
		    (memls.addr1 | 4095) < cpu->phys_mem_size && \
		    !in_iomem(memls.addr1) && !in_iomem(memls.addr1 | 4095)) { \
			int count1 = cpu->cb.io_write_string( \
				cpu->cb.io, lreg16(2), \
				cpu->phys_mem + memls.addr1, dir, count); \
			if (count1 > 0) { \
				count = count1; \
				sreg ## ABIT(6, lreg ## ABIT(6) + count * dir); \
				sreg ## ABIT(1, cx - count); \
				cx = lreg ## ABIT(1); \
				continue; \
			} \
		} \
		for (uword i = 0; i <= count - 1; i++) { \
			ax = laddr ## BIT(&memls); \
			cpu->cb.io_write ## BIT(cpu->cb.io, lreg16(2), ax); \
			memls.addr1 += dir; \
		} \
		sreg ## ABIT(6, lreg ## ABIT(6) + count * dir); \
		sreg ## ABIT(1, cx - count); \
		cx = lreg ## ABIT(1); \
	}

#define OUTS_helper(BIT) \
	if (curr_seg == -1) curr_seg = SEG_DS; \
	TRY(check_ioperm(cpu, lreg16(2), BIT)); \
	xdir ## BIT \
	u ## BIT ax; \
	if (rep == 0) { \
		if (adsz16) { ldsioutdx(BIT, 16) } else { ldsioutdx(BIT, 32) } \
	} else { \
		if (rep != 1) THROW0(EX_UD); \
		if (adsz16) { \
			OUTS_helper2(BIT, 16) \
		} else { \
			OUTS_helper2(BIT, 32) \
		} \
	}

#define OUTSb() OUTS_helper(8)
#define OUTS() if (opsz16) { OUTS_helper(16) } else { OUTS_helper(32) }

#define JCXZb(i, li, _) \
	sword d = sext8(li(i)); \
	if (adsz16) { \
		if (lreg16(1) == 0) { cpu->next_ip += d; if (code16) cpu->next_ip &= 0xffff; } \
	} else { \
		if (lreg32(1) == 0) { cpu->next_ip += d; if (code16) cpu->next_ip &= 0xffff; } \
	}

#define LOOPb(i, li, _) \
	sword d = sext8(li(i)); \
	if (adsz16) { \
		sreg16(1, lreg16(1) - 1); \
		if (lreg16(1)) { cpu->next_ip += d; if (code16) cpu->next_ip &= 0xffff; } \
	} else { \
		sreg32(1, lreg32(1) - 1); \
		if (lreg32(1)) { cpu->next_ip += d; if (code16) cpu->next_ip &= 0xffff; } \
	}

#define LOOPEb(i, li, _) \
	sword d = sext8(li(i)); \
	if (adsz16) { \
		sreg16(1, lreg16(1) - 1); \
		if (lreg16(1) && get_ZF(cpu)) { cpu->next_ip += d; if (code16) cpu->next_ip &= 0xffff; } \
	} else { \
		sreg32(1, lreg32(1) - 1); \
		if (lreg32(1) && get_ZF(cpu)) { cpu->next_ip += d; if (code16) cpu->next_ip &= 0xffff; } \
	}

#define LOOPNEb(i, li, _) \
	sword d = sext8(li(i)); \
	if (adsz16) { \
		sreg16(1, lreg16(1) - 1); \
		if (lreg16(1) && !get_ZF(cpu)) { cpu->next_ip += d; if (code16) cpu->next_ip &= 0xffff; } \
	} else { \
		sreg32(1, lreg32(1) - 1); \
		if (lreg32(1) && !get_ZF(cpu)) { cpu->next_ip += d; if (code16) cpu->next_ip &= 0xffff; } \
	}

#define COND() \
	int cond; \
	switch(b1 & 0xf) { \
	case 0x0: cond =  get_OF(cpu); break; \
	case 0x1: cond = !get_OF(cpu); break; \
	case 0x2: cond =  get_CF(cpu); break; \
	case 0x3: cond = !get_CF(cpu); break; \
	case 0x4: cond =  get_ZF(cpu); break; \
	case 0x5: cond = !get_ZF(cpu); break; \
	case 0x6: cond =  get_ZF(cpu) ||  get_CF(cpu); break; \
	case 0x7: cond = !get_ZF(cpu) && !get_CF(cpu); break; \
	case 0x8: cond =  get_SF(cpu); break; \
	case 0x9: cond = !get_SF(cpu); break; \
	case 0xa: cond =  get_PF(cpu); break; \
	case 0xb: cond = !get_PF(cpu); break; \
	case 0xc: cond =  get_SF(cpu) != get_OF(cpu); break; \
	case 0xd: cond =  get_SF(cpu) == get_OF(cpu); break; \
	case 0xe: cond =  get_ZF(cpu) || get_SF(cpu) != get_OF(cpu); break; \
	case 0xf: cond = !get_ZF(cpu) && get_SF(cpu) == get_OF(cpu); break; \
	}

#define JCC_common(d) \
	COND() \
	if (cond) { cpu->next_ip += d; if (code16) cpu->next_ip &= 0xffff; } \

#define SETCCb(a, la, sa) \
	COND() \
	sa(a, cond);

#define JCCb(i, li, _) \
	sword d = sext8(li(i)); \
	JCC_common(d)

#define JCCw(i, li, _) \
	sword d = sext16(li(i)); \
	JCC_common(d)

#define JCCd(i, li, _) \
	sword d = sext32(li(i)); \
	JCC_common(d)

#define JMPb(i, li, _) \
	sword d = sext8(li(i)); \
	cpu->next_ip += d; if (code16) cpu->next_ip &= 0xffff;

#define JMPw(i, li, _) \
	sword d = sext16(li(i)); \
	cpu->next_ip += d; if (code16) cpu->next_ip &= 0xffff;

#define JMPd(i, li, _) \
	sword d = sext32(li(i)); \
	cpu->next_ip += d; if (code16) cpu->next_ip &= 0xffff;

#define JMPABSw(i, li, _) \
	cpu->next_ip = li(i);

#define JMPABSd(i, li, _) \
	cpu->next_ip = li(i);

#define JMPFAR(addr, seg) \
	if ((cpu->cr0 & 1) && !(cpu->flags & VM)) { \
		TRY(pmcall(cpu, opsz16, addr, seg, true)); \
	} else { \
	TRY(set_seg(cpu, SEG_CS, seg)); \
	cpu->next_ip = addr; \
	}

#define CALLFAR(addr, seg) \
	if ((cpu->cr0 & 1) && !(cpu->flags & VM)) { \
		TRY(pmcall(cpu, opsz16, addr, seg, false)); \
	} else { \
	OptAddr meml1, meml2; \
	uword sp = lreg32(4); \
	if (opsz16) { \
		TRY(translate16(cpu, &meml1, 2, SEG_SS, (sp - 2) & sp_mask)); \
		TRY(translate16(cpu, &meml2, 2, SEG_SS, (sp - 4) & sp_mask)); \
		set_sp(sp - 4, sp_mask); \
		saddr16(&meml1, cpu->seg[SEG_CS].sel); \
		saddr16(&meml2, cpu->next_ip); \
	} else { \
		TRY(translate32(cpu, &meml1, 2, SEG_SS, (sp - 4) & sp_mask)); \
		TRY(translate32(cpu, &meml2, 2, SEG_SS, (sp - 8) & sp_mask)); \
		set_sp(sp - 8, sp_mask); \
		saddr32(&meml1, cpu->seg[SEG_CS].sel); \
		saddr32(&meml2, cpu->next_ip); \
	} \
	TRY(set_seg(cpu, SEG_CS, seg)); \
	cpu->next_ip = addr; \
	}

#define CALLw(i, li, _) \
	sword d = sext16(li(i)); \
	uword sp = lreg32(4); \
	TRY(translate16(cpu, &meml, 2, SEG_SS, (sp - 2) & sp_mask)); \
	set_sp(sp - 2, sp_mask); \
	saddr16(&meml, cpu->next_ip); \
	cpu->next_ip += d; if (code16) cpu->next_ip &= 0xffff;

#define CALLd(i, li, _) \
	sword d = sext32(li(i)); \
	uword sp = lreg32(4); \
	TRY(translate32(cpu, &meml, 2, SEG_SS, (sp - 4) & sp_mask)); \
	set_sp(sp - 4, sp_mask); \
	saddr32(&meml, cpu->next_ip); \
	cpu->next_ip += d; if (code16) cpu->next_ip &= 0xffff;

#define CALLABSw(i, li, _) \
	uword nip = li(i); \
	uword sp = lreg32(4); \
	TRY(translate16(cpu, &meml, 2, SEG_SS, (sp - 2) & sp_mask)); \
	set_sp(sp - 2, sp_mask); \
	saddr16(&meml, cpu->next_ip); \
	cpu->next_ip = nip;

#define CALLABSd(i, li, _) \
	uword nip = li(i); \
	uword sp = lreg32(4); \
	TRY(translate32(cpu, &meml, 2, SEG_SS, (sp - 4) & sp_mask)); \
	set_sp(sp - 4, sp_mask); \
	saddr32(&meml, cpu->next_ip); \
	cpu->next_ip = nip;

#define RETw(i, li, _) \
	if (opsz16) { \
		uword sp = lreg32(4); \
		TRY(translate16(cpu, &meml, 1, SEG_SS, sp & sp_mask)); \
		set_sp(sp + 2 + li(i), sp_mask); \
		cpu->next_ip = laddr16(&meml); \
	} else { \
		uword sp = lreg32(4); \
		TRY(translate32(cpu, &meml, 1, SEG_SS, sp & sp_mask)); \
		set_sp(sp + 4 + li(i), sp_mask); \
		cpu->next_ip = laddr32(&meml); \
	}

#define RET() RETw(0, limm, 0)

static bool enter_helper(CPUI386 *cpu, bool opsz16, uword sp_mask,
			 int level, int allocsz)
{
	assert(level != 0);
	uword temp;
	OptAddr meml1;

	uword sp = lreg32(4);
	if (opsz16) {
		TRY(translate16(cpu, &meml1, 2, SEG_SS, (sp - 2) & sp_mask));
		set_sp(sp - 2, sp_mask);
		saddr16(&meml1, lreg16(5));
		temp = lreg16(4);
	} else {
		TRY(translate32(cpu, &meml1, 2, SEG_SS, (sp - 4) & sp_mask));
		set_sp(sp - 4, sp_mask);
		saddr32(&meml1, lreg32(5));
		temp = lreg32(4);
	}

	for (int i = 0; i < level - 1; i++) {
		if (opsz16) {
			if (sp_mask == 0xffff) {
				sreg16(5, lreg16(5) - 2);
			} else {
				sreg32(5, lreg32(5) - 2);
			}
			TRY(translate16(cpu, &meml1, 2, SEG_SS, (sp - 2) & sp_mask));
			set_sp(sp - 2, sp_mask);
			saddr16(&meml1, lreg16(5));
		} else {
			if (sp_mask == 0xffff) {
				sreg16(5, lreg16(5) - 4);
			} else {
				sreg32(5, lreg32(5) - 4);
			}
			TRY(translate32(cpu, &meml1, 2, SEG_SS, (sp - 4) & sp_mask));
			set_sp(sp - 4, sp_mask);
			saddr32(&meml1, lreg32(5));
		}
	}

	if (opsz16) {
		TRY(translate16(cpu, &meml1, 2, SEG_SS, (sp - 2) & sp_mask));
		set_sp(sp - 2 - allocsz, sp_mask);
		saddr16(&meml1, temp);
		sreg16(5, temp);
	} else {
		TRY(translate32(cpu, &meml1, 2, SEG_SS, (sp - 4) & sp_mask));
		set_sp(sp - 4 - allocsz, sp_mask);
		saddr32(&meml1, temp);
		sreg32(5, temp);
	}
	return true;
}

#define ENTER(i16, i8, l16, s16, l8, s8) \
	OptAddr meml1; \
	int level = l8(i8) % 32; \
	if (level == 0) { \
	uword sp = lreg32(4); \
	if (opsz16) { \
		TRY(translate16(cpu, &meml1, 2, SEG_SS, (sp - 2) & sp_mask)); \
		set_sp(sp - 2 - l16(i16), sp_mask); \
		saddr16(&meml1, lreg16(5)); \
		sreg16(5, (sp - 2) & sp_mask); \
	} else { \
		TRY(translate32(cpu, &meml1, 2, SEG_SS, (sp - 4) & sp_mask)); \
		set_sp(sp - 4 - l16(i16), sp_mask); \
		saddr32(&meml1, lreg32(5)); \
		sreg32(5, (sp - 4) & sp_mask); \
	} \
	} else { \
		TRY(enter_helper(cpu, opsz16, sp_mask, level, l16(i16))); \
	}

#define LEAVE() \
	uword sp = lreg32(5); \
	if (opsz16) { \
		TRY(translate16(cpu, &meml, 1, SEG_SS, sp & sp_mask)); \
		set_sp(sp + 2, sp_mask); \
		sreg16(5, laddr16(&meml)); \
	} else { \
		TRY(translate32(cpu, &meml, 1, SEG_SS, sp & sp_mask)); \
		set_sp(sp + 4, sp_mask); \
		sreg32(5, laddr32(&meml)); \
	}

#define SXXX(addr) \
	OptAddr meml1, meml2; \
	TRY(translate16(cpu, &meml1, 2, curr_seg, addr)); \
	TRY(translate32(cpu, &meml2, 2, curr_seg, addr + 2)); \

#define SGDT(addr) \
	SXXX(addr) \
	store16(cpu, &meml1, cpu->gdt.limit); \
	store32(cpu, &meml2, cpu->gdt.base);

#define SIDT(addr) \
	SXXX(addr) \
	store16(cpu, &meml1, cpu->idt.limit); \
	store32(cpu, &meml2, cpu->idt.base);

#define LXXX(addr) \
	if (cpu->cpl != 0) THROW(EX_GP, 0); \
	OptAddr meml1, meml2; \
	TRY(translate16(cpu, &meml1, 1, curr_seg, addr)); \
	TRY(translate32(cpu, &meml2, 1, curr_seg, addr + 2)); \
	u16 limit = load16(cpu, &meml1); \
	u32 base = load32(cpu, &meml2); \
	if (opsz16) base &= 0xffffff;

#define LGDT(addr) \
	LXXX(addr) \
	cpu->gdt.base = base; \
	cpu->gdt.limit = limit;

#define LIDT(addr) \
	LXXX(addr) \
	cpu->idt.base = base; \
	cpu->idt.limit = limit;

#define LLDT(a, la, sa) \
	if (cpu->cpl != 0) THROW(EX_GP, 0); \
	TRY(set_seg(cpu, SEG_LDT, la(a)));

#define SLDT(a, la, sa) \
	sa(a, cpu->seg[SEG_LDT].sel);

#define LTR(a, la, sa) \
	if (cpu->cpl != 0) THROW(EX_GP, 0); \
	TRY(set_seg(cpu, SEG_TR, la(a)));

#define STR(a, la, sa) \
	sa(a, cpu->seg[SEG_TR].sel);

#define MOVFD() \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	int rm = modrm & 7; \
	sreg32(rm, cpu->dr[reg]);

#define MOVTD() \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	int rm = modrm & 7; \
	cpu->dr[reg] = lreg32(rm);

#define MOVFT() \
	TRY(fetch8(cpu, &modrm));
#define MOVTT() \
	TRY(fetch8(cpu, &modrm));

#define SMSW(addr, laddr, saddr) \
	saddr(addr, cpu->cr0 & 0xffff);

#define LMSW(addr, laddr, saddr) \
	if (cpu->cpl != 0) THROW(EX_GP, 0); \
	cpu->cr0 = (cpu->cr0 & ((~0xf) | 1)) | (laddr(addr) & 0xf);

noinline static bool __LSEGd_helper(CPUI386 *cpu, int adsz16, int seg, int reg, int curr_seg, uword addr)
{
	OptAddr meml1, meml2; \
	if (adsz16) addr = addr & 0xffff; \
	TRY(translate32(cpu, &meml1, 1, curr_seg, addr)); \
	TRY(translate16(cpu, &meml2, 1, curr_seg, addr + 4)); \
	u32 r = load32(cpu, &meml1); \
	u32 s = load16(cpu, &meml2); \
	TRY(set_seg(cpu, seg, s)); \
	sreg32(reg, r);
	return true;
}

noinline static bool __LSEGw_helper(CPUI386 *cpu, int adsz16, int seg, int reg, int curr_seg, uword addr)
{
	OptAddr meml1, meml2; \
	if (adsz16) addr = addr & 0xffff; \
	TRY(translate16(cpu, &meml1, 1, curr_seg, addr)); \
	TRY(translate16(cpu, &meml2, 1, curr_seg, addr + 2)); \
	u32 r = load16(cpu, &meml1); \
	u32 s = load16(cpu, &meml2); \
	TRY(set_seg(cpu, seg, s)); \
	sreg16(reg, r);
	return true;
}

#define LSEGd(NAME, reg, addr, lreg32, sreg32, laddr32, saddr32) TRY(__LSEGd_helper(cpu, adsz16, SEG_ ## NAME, reg, curr_seg, addr));
#define LSEGw(NAME, reg, addr, lreg16, sreg16, laddr16, saddr16) TRY(__LSEGw_helper(cpu, adsz16, SEG_ ## NAME, reg, curr_seg, addr));
#define LESd(...) LSEGd(ES, __VA_ARGS__)
#define LSSd(...) LSEGd(SS, __VA_ARGS__)
#define LDSd(...) LSEGd(DS, __VA_ARGS__)
#define LFSd(...) LSEGd(FS, __VA_ARGS__)
#define LGSd(...) LSEGd(GS, __VA_ARGS__)
#define LESw(...) LSEGw(ES, __VA_ARGS__)
#define LSSw(...) LSEGw(SS, __VA_ARGS__)
#define LDSw(...) LSEGw(DS, __VA_ARGS__)
#define LFSw(...) LSEGw(FS, __VA_ARGS__)
#define LGSw(...) LSEGw(GS, __VA_ARGS__)

static bool check_ioperm(CPUI386 *cpu, int port, int bit)
{
	bool allow = true;
	if ((cpu->cr0 & 1) && (cpu->cpl > get_IOPL(cpu) || (cpu->flags & VM))) {
		allow = false;
		if (cpu->seg[SEG_TR].limit >= 103) {
			OptAddr meml;
			TRY(translate(cpu, &meml, 1, SEG_TR, 102, 2, 0));
			u32 iobase = load16(cpu, &meml);
			if (iobase + port / 8 < cpu->seg[SEG_TR].limit) {
				TRY(translate(cpu, &meml, 1, SEG_TR, iobase + port / 8, 2, 0));
				u16 perm = load16(cpu, &meml);
				int len = bit / 8;
				unsigned bit_index = port & 0x7;
				unsigned mask = (1 << len) - 1;
				if (!((perm >> bit_index) & mask))
					allow = true;
			}
		}
	}

	if (!allow) THROW(EX_GP, 0);
	return true;
}

#define INb(a, b, la, sa, lb, sb) \
	int port = lb(b); \
	TRY(check_ioperm(cpu, port, 8)); \
	sa(a, cpu->cb.io_read8(cpu->cb.io, port));

#define INw(a, b, la, sa, lb, sb) \
	int port = lb(b); \
	TRY(check_ioperm(cpu, port, 16)); \
	sa(a, cpu->cb.io_read16(cpu->cb.io, port));

#define INd(a, b, la, sa, lb, sb) \
	int port = lb(b); \
	TRY(check_ioperm(cpu, port, 32)); \
	sa(a, cpu->cb.io_read32(cpu->cb.io, port));

#define OUTb(a, b, la, sa, lb, sb) \
	int port = la(a); \
	TRY(check_ioperm(cpu, port, 8)); \
	cpu->cb.io_write8(cpu->cb.io, port, lb(b));

#define OUTw(a, b, la, sa, lb, sb) \
	int port = la(a); \
	TRY(check_ioperm(cpu, port, 16)); \
	cpu->cb.io_write16(cpu->cb.io, port, lb(b));

#define OUTd(a, b, la, sa, lb, sb) \
	int port = la(a); \
	TRY(check_ioperm(cpu, port, 32)); \
	cpu->cb.io_write32(cpu->cb.io, port, lb(b));

#define CLTS() \
	cpu->cr0 &= ~(1 << 3);

#define ESC() \
	if (cpu->cr0 & 0xc) THROW0(EX_NM); \
	else { \
		TRY(fetch8(cpu, &modrm)); \
		int mod = modrm >> 6; \
		int rm = modrm & 7; \
		int op = b1 - 0xd8; \
		int group = (modrm >> 3) & 7; \
		if (mod != 3) { \
			TRY(modsib(cpu, adsz16, mod, rm, &addr, &curr_seg)); \
			if (cpu->fpu) { \
				TRY(fpu_exec2(cpu->fpu, cpu, opsz16, op, group, curr_seg, addr)); \
			} \
		} else { \
			int reg = modrm & 7; \
			if (cpu->fpu) { \
				TRY(fpu_exec1(cpu->fpu, cpu, op, group, reg)); \
			} \
		} \
	}

#define WAIT() \
	if ((cpu->cr0 & 0xa) == 0xa) THROW0(EX_NM);

// ...
noinline static void __AAD_helper(CPUI386 *cpu, u8 i)
{
	u8 al = lreg8(0); \
	u8 ah = lreg8(4); \
	u8 imm = i; \
	u8 res = al + ah * imm; \
	sreg8(0, res); \
	sreg8(4, 0); \
	cpu->flags &= ~(OF | AF | CF); /* undocumented */ \
	cpu->cc.dst = sext8(res); \
	cpu->cc.mask = ZF | SF | PF;
}
#define AAD(i, li, _) __AAD_helper(cpu, li(i));

noinline static void __AAM_helper(CPUI386 *cpu, u8 i)
{
	u8 al = lreg8(0); \
	u8 imm = i; \
	u8 res = al % imm; \
	sreg8(4, al / imm); \
	sreg8(0, res); \
	cpu->flags &= ~(OF | AF | CF); /* undocumented */ \
	cpu->cc.dst = sext8(res); \
	cpu->cc.mask = ZF | SF | PF;
}
#define AAM(i, li, _) __AAM_helper(cpu, li(i));

#define SALC() \
	if (get_CF(cpu)) sreg8(0, 0xff); else sreg8(0, 0x00);

noinline static bool __XLAT_helper(CPUI386 *cpu, int adsz16, int curr_seg)
{
	OptAddr meml;
	uword addr;
	if (curr_seg == -1) curr_seg = SEG_DS; \
	if (adsz16) { \
		addr = lreg16(3) + lreg8(0); \
		addr &= 0xffff; \
		TRY(translate8(cpu, &meml, 1, curr_seg, addr)); \
		sreg8(0, laddr8(&meml)); \
	} else { \
		addr = lreg32(3) + lreg8(0); \
		TRY(translate8(cpu, &meml, 1, curr_seg, addr)); \
		sreg8(0, laddr8(&meml)); \
	}
	return true;
}
#define XLAT() TRY(__XLAT_helper(cpu, adsz16, curr_seg));

noinline static void __DAA_helper(CPUI386 *cpu)
{
	u8 al = lreg8(0); \
	int cf = get_CF(cpu); \
	cpu->flags &= ~CF; \
	if ((al & 0xf) > 9 || get_AF(cpu)) { \
		sreg8(0, al + 6); \
		if (cf || al > 0xff - 6) cpu->flags |= CF; \
		cpu->flags |= AF; \
	} else { \
		cpu->flags &= ~AF; \
	} \
	if (al > 0x99 || cf) { \
		sreg8(0, lreg8(0) + 0x60); \
		cpu->flags |= CF; \
	} \
	cpu->cc.dst = sext8(lreg8(0)); \
	cpu->cc.mask = ZF | SF | PF;
}
#define DAA() __DAA_helper(cpu);

noinline static void __DAS_helper(CPUI386 *cpu)
{
	u8 al = lreg8(0);     \
	int cf = get_CF(cpu); \
	cpu->flags &= ~CF; \
	if ((al & 0xf) > 9 || get_AF(cpu)) { \
		sreg8(0, al - 6); \
		if (cf || al < 6) cpu->flags |= CF; \
		cpu->flags |= AF; \
	} else { \
		cpu->flags &= ~AF; \
	} \
	if (al > 0x99 || cf) { \
		sreg8(0, lreg8(0) - 0x60); \
		cpu->flags |= CF; \
	} \
	cpu->cc.dst = sext8(lreg8(0)); \
	cpu->cc.mask = ZF | SF | PF;
}
#define DAS() __DAS_helper(cpu);

noinline static void __AAA_helper(CPUI386 *cpu)
{
	if ((lreg8(0) & 0xf) > 9 || get_AF(cpu)) { \
		sreg16(0, lreg16(0) + 0x106); \
		cpu->flags |= AF | CF; \
	} else { \
		cpu->flags &= ~(AF | CF); \
	} \
	cpu->cc.mask = ZF | SF | PF; \
	sreg8(0, lreg8(0) & 0xf);
}
#define AAA() __AAA_helper(cpu);

noinline static void __AAS_helper(CPUI386 *cpu)
{
	if ((lreg8(0) & 0xf) > 9 || get_AF(cpu)) { \
		sreg16(0, lreg16(0) - 6); \
		sreg8(4, lreg8(4) - 1); \
		cpu->flags |= AF | CF; \
	} else { \
		cpu->flags &= ~(AF | CF); \
	} \
	cpu->cc.mask = ZF | SF | PF; \
	sreg8(0, lreg8(0) & 0xf);
}
#define AAS() __AAS_helper(cpu);

static bool larsl_helper(CPUI386 *cpu, int sel, uword *ar, uword *sl, int *zf)
{
	sel = sel & 0xffff;

	if (!(cpu->cr0 & 1) || (cpu->flags & VM))
		THROW0(EX_UD);

	if ((sel & ~0x3) == 0) {
		*zf = 0;
		return true;
	}

	uword w1, w2;
	if (!read_desc(cpu, sel, &w1, &w2)) {
		*zf = 0;
		return true;
	}

	if ((w2 >> 12) & 1) {
		int dpl = (w2 >> 13) & 0x3;
		if (((w2 >> 10) & 0x3) != 0x3 && (cpu->cpl > dpl || (sel & 0x3) > dpl)) {
			*zf = 0;
			return true;
		}
	} else {
		int type = (w2 >> 8) & 0xf;
		if (ar) {
			switch (type) {
			case 0: case 6: case 7: case 8: case 10:
			case 13: case 14: case 15:
				*zf = 0;
				return true;
			}
		}
		if (sl) {
			switch (type) {
			case 0: case 4: case 5: case 6: case 7: case 8:
			case 10: case 12: case 13: case 14: case 15:
				*zf = 0;
				return true;
			}
		}
	}

	if (ar)
		*ar = w2 & 0x00ffff00;
	if (sl) {
		*sl = (w2 & 0xf0000) | (w1 & 0xffff);
		if (w2 & 0x00800000)
			*sl = (*sl << 12) | 0xfff;
	}

	*zf = 1;
	return true;
}

static bool verrw_helper(CPUI386 *cpu, int sel, int wr, int *zf)
{
	sel = sel & 0xffff;

	if (!(cpu->cr0 & 1) || (cpu->flags & VM))
		THROW0(EX_UD);

	if ((sel & ~0x3) == 0) {
		*zf = 0;
		return true;
	}

	uword w1, w2;
	if (!read_desc(cpu, sel, &w1, &w2)) {
		*zf = 0;
		return true;
	}

	if (((w2 >> 12) & 0x1) == 0) {
		*zf = 0;
		return true;
	}

	int dpl = (w2 >> 13) & 0x3;
	if (((w2 >> 10) & 0x3) != 0x3 && (cpu->cpl > dpl || (sel & 0x3) > dpl)) {
		*zf = 0;
		return true;
	}

	if (((w2 >> 11) & 0x1) == 0) {
		/* data */
		if (wr && ((w2 >> 9) & 0x1) == 0) {
			*zf = 0;
			return true;
		}
	} else {
		/* code */
		if (!wr && ((w2 >> 9) & 0x1) == 0) {
			*zf = 0;
			return true;
		}
	}

	*zf = 1;
	return true;
}

#define LARdw(a, b, la, sa, lb, sb) \
	uword res; \
	int zf; \
	TRY(larsl_helper(cpu, lb(b), &res, NULL, &zf)); \
	if (zf) { \
		sa(a, res); \
		cpu->flags |= ZF; \
	} else { \
		cpu->flags &= ~ZF; \
	} \
	cpu->cc.mask &= ~ZF;
#define LARww LARdw

#define LSLdw(a, b, la, sa, lb, sb) \
	uword res; \
	int zf; \
	TRY(larsl_helper(cpu, lb(b), NULL, &res, &zf)); \
	if (zf) { \
		sa(a, res); \
		cpu->flags |= ZF; \
	} else { \
		cpu->flags &= ~ZF; \
	} \
	cpu->cc.mask &= ~ZF;
#define LSLww LSLdw

#define VERR(a, la, sa) \
	int zf; \
	TRY(verrw_helper(cpu, la(a), 0, &zf)); \
	cpu->cc.mask &= ~ZF; \
	SET_BIT(cpu->flags, zf, ZF);

#define VERW(a, la, sa) \
	int zf; \
	TRY(verrw_helper(cpu, la(a), 1, &zf)); \
	cpu->cc.mask &= ~ZF; \
	SET_BIT(cpu->flags, zf, ZF);

#define ARPL(a, b, la, sa, lb, sb) \
	if (!(cpu->cr0 & 1) || (cpu->flags & VM)) THROW0(EX_UD); \
	u16 dst = la(a); \
	u16 src = lb(b); \
	if ((dst & 3) < (src & 3)) { \
		cpu->flags |= ZF; \
		sa(a, ((dst & ~3) | (src & 3))); \
	} else { \
		cpu->flags &= ~ZF; \
	} \
	cpu->cc.mask &= ~ZF;

#define GvMa GvM
#define BOUND_helper(BIT, a, b, la, sa, lb, sb) \
	OptAddr meml1, meml2; \
	s ## BIT idx = la(a); \
	uword addr1 = lb(b); \
	TRY(translate ## BIT(cpu, &meml1, 3, curr_seg, addr1)); \
	TRY(translate ## BIT(cpu, &meml2, 3, curr_seg, addr1 + BIT / 8)); \
	s ## BIT lo = load ## BIT(cpu, &meml1); \
	s ## BIT hi = load ## BIT(cpu, &meml2); \
	if (idx < lo || idx > hi) THROW0(EX_BR);
#define BOUNDw(...) BOUND_helper(16, __VA_ARGS__)
#define BOUNDd(...) BOUND_helper(32, __VA_ARGS__)

// 486...
#define CMPXCH_helper(BIT, a, b, la, sa, lb, sb) \
	cpu->cc.src1 = sext ## BIT(la(a)); \
	cpu->cc.src2 = sext ## BIT(lreg ## BIT(0)); \
	cpu->cc.dst = sext ## BIT(cpu->cc.src1 - cpu->cc.src2); \
	cpu->cc.op = CC_SUB; \
	cpu->cc.mask = CF | PF | AF | ZF | SF | OF; \
	if (cpu->cc.dst == 0) sa(a, lb(b)); else sreg ## BIT(0, cpu->cc.src1); 

#define XADD_helper(BIT, a, b, la, sa, lb, sb) \
	u ## BIT dst = la(a); \
	cpu->cc.src1 = sext ## BIT(la(a)); \
	cpu->cc.src2 = sext ## BIT(lb(b)); \
	cpu->cc.dst = sext ## BIT(cpu->cc.src1 + cpu->cc.src2); \
	cpu->cc.op = CC_ADD; \
	cpu->cc.mask = CF | PF | AF | ZF | SF | OF; \
	sb(b, dst); \
	sa(a, cpu->cc.dst);

#define CMPXCHb(...) CMPXCH_helper(8, __VA_ARGS__)
#define CMPXCHw(...) CMPXCH_helper(16, __VA_ARGS__)
#define CMPXCHd(...) CMPXCH_helper(32, __VA_ARGS__)
#define XADDb(...) XADD_helper(8, __VA_ARGS__)
#define XADDw(...) XADD_helper(16, __VA_ARGS__)
#define XADDd(...) XADD_helper(32, __VA_ARGS__)

#define INVLPG(addr) tlb_clear(cpu);

#define BSWAPw(a, la, sa) THROW0(EX_UD);

#define BSWAPd(a, la, sa) \
	u32 src = la(a); \
	u32 dst = ((src & 0xff) << 24) | (((src >> 8) & 0xff) << 16) | (((src >> 16) & 0xff) << 8) | ((src >> 24) & 0xff); \
	sa(a, dst);

#define WBINVD()

// 586 and later...
#define UD0() THROW0(EX_UD);

#if defined(I386_ENABLE_SSSE3)
#define CPUID_SIMD_FEATURE2 0x201
#elif defined(I386_ENABLE_SSE3)
#define CPUID_SIMD_FEATURE2 0x1
#else
#define CPUID_SIMD_FEATURE2 0x0
#endif
#if defined(I386_ENABLE_SSE2)
#define CPUID_SIMD_FEATURE 0x7800000
#elif defined(I386_ENABLE_SSE)
#define CPUID_SIMD_FEATURE 0x3800000
#elif defined(I386_ENABLE_MMX)
#define CPUID_SIMD_FEATURE 0x800000
#else
#define CPUID_SIMD_FEATURE 0x0
#endif

#define CPUID() \
	switch (REGi(0)) { \
	case 0: \
		REGi(0) = 1; \
		REGi(3) = 0x594e4954; \
		REGi(2) = 0x20363833; \
		REGi(1) = 0x20555043; \
		break; \
	case 1: \
		REGi(0) = 0 | (0 << 4) | (cpu->gen << 8); \
		REGi(3) = 0; \
		REGi(2) = 0x110; \
		REGi(1) = 0; \
		if (cpu->fpu) REGi(2) |= 1; \
		if (cpu->gen > 5) REGi(2) |= 0x8820; \
		if (cpu->gen > 5 && cpu->fpu) { \
			REGi(2) |= CPUID_SIMD_FEATURE; \
			REGi(1) |= CPUID_SIMD_FEATURE2; \
		} \
		REGi(2) |= CPUID_LM0_2; \
		break; \
	case 0x80000000: \
		REGi(0) = CPUID_LM80_0; \
		REGi(3) = 0; \
		REGi(2) = 0; \
		REGi(1) = 0; \
		break; \
	case 0x80000001: \
		REGi(0) = 0; \
		REGi(3) = 0; \
		REGi(2) = CPUID_LM81_2; \
		REGi(1) = 1; \
		break; \
	default: \
		REGi(0) = 0; \
		REGi(3) = 0; \
		REGi(2) = 0; \
		REGi(1) = 0; \
		break; \
	}

#include <time.h>
static uint64_t get_nticks()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ((uint64_t) ts.tv_sec * 1000000000ull +
		(uint64_t) ts.tv_nsec);
}

#define RDTSC() \
	uint64_t tsc = get_nticks(); \
	REGi(0) = tsc; \
	REGi(2) = tsc >> 32;

#define Mq Ms
#define CMPXCH8B(addr) \
	OptAddr meml1, meml2; \
	TRY(translate32(cpu, &meml1, 3, curr_seg, addr)); \
	TRY(translate32(cpu, &meml2, 3, curr_seg, addr + 4)); \
	uword lo = load32(cpu, &meml1); \
	uword hi = load32(cpu, &meml2); \
	if (REGi(0) == lo && REGi(2) == hi) { \
		cpu->flags |= ZF; \
		store32(cpu, &meml1, REGi(3)); \
		store32(cpu, &meml2, REGi(1)); \
	} else { \
		cpu->flags &= ~ZF; \
		REGi(0) = lo; \
		REGi(2) = hi; \
	} \
	cpu->cc.mask &= ~ZF;

#define CMOVw(a, b, la, sa, lb, sb) \
	COND() \
	if (cond) sa(a, lb(b));

#define CMOVd(a, b, la, sa, lb, sb) \
	COND() \
	if (cond) sa(a, lb(b));

#define WRMSR() \
	if (cpu->cpl != 0) THROW(EX_GP, 0); \
	switch (REGi(1)) { \
	case 0x174: cpu->sysenter.cs = REGi(0); break; \
	case 0x176: cpu->sysenter.eip = REGi(0); break; \
	case 0x175: cpu->sysenter.esp = REGi(0); break; \
	case 0xc0000080: SET_LM_EFER(cpu,REGi(0) & ~(1<<10)); break; \
	default: cpu_debug(cpu); THROW(EX_GP, 0); \
	}

#define RDMSR() \
	if (cpu->cpl != 0) THROW(EX_GP, 0); \
	switch (REGi(1)) { \
	case 0x174: REGi(0) = cpu->sysenter.cs; REGi(2) = 0; break; \
	case 0x176: REGi(0) = cpu->sysenter.eip; REGi(2) = 0; break; \
	case 0x175: REGi(0) = cpu->sysenter.esp; REGi(2) = 0; break; \
	case 0xc0000080: REGi(0) = GET_LM_EFER(cpu); REGi(2) = 0; break; \
	default: cpu_debug(cpu); THROW(EX_GP, 0); \
	}

static void __sysenter(CPUI386 *cpu, int pl, int cs)
{
	cpu->seg[SEG_CS].sel = (cs & 0xfffc) | pl;
	cpu->seg[SEG_CS].base = 0;
	cpu->seg[SEG_CS].limit = 0xffffffff;
	cpu->seg[SEG_CS].flags = SEG_D_BIT | 0x5b | (pl << 5);
	cpu->cpl = pl;
	cpu->code16 = false;
	cpu->sp_mask = 0xffffffff;
	cpu->seg[SEG_SS].sel = ((cs + 8) & 0xfffc) | pl;
	cpu->seg[SEG_SS].base = 0;
	cpu->seg[SEG_SS].limit = 0xffffffff;
	cpu->seg[SEG_SS].flags = SEG_B_BIT | 0x53 | (pl << 5);
}

#define SYSENTER() \
	if (!(cpu->cr0 & 1) || (cpu->sysenter.cs & ~0x3) == 0) THROW(EX_GP, 0); \
	cpu->flags &= ~(VM | IF); \
	__sysenter(cpu, 0, cpu->sysenter.cs); \
	REGi(4) = cpu->sysenter.esp; \
	cpu->next_ip = cpu->sysenter.eip;

#define SYSEXIT() \
	if (!(cpu->cr0 & 1) || (cpu->sysenter.cs & ~0x3) == 0 || cpu->cpl) THROW(EX_GP, 0); \
	__sysenter(cpu, 3, cpu->sysenter.cs + 16); \
	REGi(4) = REGi(1); \
	cpu->next_ip = REGi(2);

#if defined(I386_ENABLE_MMX) || defined(I386_ENABLE_SSE)
#define SIMD_i386_c
#include "simd.inc"
#undef SIMD_i386_c
#endif

static bool pmcall(CPUI386 *cpu, bool opsz16, uword addr, int sel, bool isjmp);
static bool pmret(CPUI386 *cpu, bool opsz16, int off, bool isiret);

static bool verbose;

#define ARGCOUNT_IMPL(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, ...) _17
#define ARGCOUNT(...) ARGCOUNT_IMPL(~, ## __VA_ARGS__, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define PASTE0(a, b) a ## b
#define PASTE(a, b) PASTE0(a, b)
#define C_1(_1)      CX(_1)
#define C_2(_1, ...) CX(_1) C_1(__VA_ARGS__)
#define C_3(_1, ...) CX(_1) C_2(__VA_ARGS__)
#define C_4(_1, ...) CX(_1) C_3(__VA_ARGS__)
#define C_5(_1, ...) CX(_1) C_4(__VA_ARGS__)
#define C_6(_1, ...) CX(_1) C_5(__VA_ARGS__)
#define C_7(_1, ...) CX(_1) C_6(__VA_ARGS__)
#define C_8(_1, ...) CX(_1) C_7(__VA_ARGS__)
#define C_9(_1, ...) CX(_1) C_8(__VA_ARGS__)
#define C_10(_1, ...) CX(_1) C_9(__VA_ARGS__)
#define C_11(_1, ...) CX(_1) C_10(__VA_ARGS__)
#define C_12(_1, ...) CX(_1) C_11(__VA_ARGS__)
#define C_13(_1, ...) CX(_1) C_12(__VA_ARGS__)
#define C_14(_1, ...) CX(_1) C_13(__VA_ARGS__)
#define C_15(_1, ...) CX(_1) C_14(__VA_ARGS__)
#define C_16(_1, ...) CX(_1) C_15(__VA_ARGS__)
#define C(...) PASTE(C_, ARGCOUNT(__VA_ARGS__))(__VA_ARGS__)

static bool IRAM_ATTR_CPU_EXEC1 cpu_exec1(CPUI386 *cpu, int stepcount)
{
#ifndef I386_OPT2
#define eswitch(b) switch(b)
#define ecase(a)   case a
#define ebreak     break
#define edefault   default
#define default_ud cpu_debug(cpu); THROW0(EX_UD)
#undef CX
#define CX(_1) case _1:
#else
#define eswitch(b)
#define ecase(a)   f ## a
#define ebreak     continue
#define edefault   f0xf1
#define default_ud THROW0(EX_UD)
#undef CX
#define CX(_1) f ## _1:
#endif

	u8 b1;
	u8 modrm;
	OptAddr meml;
	uword addr;
	for (; stepcount > 0; stepcount--) {
	bool code16 = cpu->code16;
	uword sp_mask = cpu->sp_mask;

	cpu->ip = cpu->next_ip;
	TRY(fetch8pf(cpu, &b1));
	cpu->cycle++;

#ifndef I386_OPT1
	if (verbose) {
		cpu_debug(cpu);
	}
#endif
	// prefix
	bool opsz16 = code16;
	bool adsz16 = code16;
	int rep = 0;
	/*bool lock = false;*/
	int curr_seg = -1;
#ifndef I386_OPT2
	for (;;) {
#define HANDLE_PREFIX(C, STMT) \
		if (b1 == C) { \
			STMT; \
			TRY(fetch8(cpu, &b1)); \
			continue; \
		}
		HANDLE_PREFIX(0x26, curr_seg = SEG_ES)
		HANDLE_PREFIX(0x2e, curr_seg = SEG_CS)
		HANDLE_PREFIX(0x36, curr_seg = SEG_SS)
		HANDLE_PREFIX(0x3e, curr_seg = SEG_DS)
		HANDLE_PREFIX(0x64, curr_seg = SEG_FS)
		HANDLE_PREFIX(0x65, curr_seg = SEG_GS)
		HANDLE_PREFIX(0x66, opsz16 = !code16)
		HANDLE_PREFIX(0x67, adsz16 = !code16)
		HANDLE_PREFIX(0xf3, rep = 1) // REP
		HANDLE_PREFIX(0xf2, rep = 2) // REPNE
		HANDLE_PREFIX(0xf0, /*lock = true*/)
#undef HANDLE_PREFIX
		break;
	}
#else
	static const DRAM_ATTR void *pfxlabel[] = {
/* 0x00 */	&&f0x00, &&f0x01, &&f0x02, &&f0x03, &&f0x04, &&f0x05, &&f0x06, &&f0x07,
/* 0x08 */	&&f0x08, &&f0x09, &&f0x0a, &&f0x0b, &&f0x0c, &&f0x0d, &&f0x0e, &&f0x0f,
/* 0x10 */	&&f0x10, &&f0x11, &&f0x12, &&f0x13, &&f0x14, &&f0x15, &&f0x16, &&f0x17,
/* 0x18 */	&&f0x18, &&f0x19, &&f0x1a, &&f0x1b, &&f0x1c, &&f0x1d, &&f0x1e, &&f0x1f,
/* 0x20 */	&&f0x20, &&f0x21, &&f0x22, &&f0x23, &&f0x24, &&f0x25, &&pfx26, &&f0x27,
/* 0x28 */	&&f0x28, &&f0x29, &&f0x2a, &&f0x2b, &&f0x2c, &&f0x2d, &&pfx2e, &&f0x2f,
/* 0x30 */	&&f0x30, &&f0x31, &&f0x32, &&f0x33, &&f0x34, &&f0x35, &&pfx36, &&f0x37,
/* 0x38 */	&&f0x38, &&f0x39, &&f0x3a, &&f0x3b, &&f0x3c, &&f0x3d, &&pfx3e, &&f0x3f,
/* 0x40 */	&&f0x40, &&f0x41, &&f0x42, &&f0x43, &&f0x44, &&f0x45, &&f0x46, &&f0x47,
/* 0x48 */	&&f0x48, &&f0x49, &&f0x4a, &&f0x4b, &&f0x4c, &&f0x4d, &&f0x4e, &&f0x4f,
/* 0x50 */	&&f0x50, &&f0x51, &&f0x52, &&f0x53, &&f0x54, &&f0x55, &&f0x56, &&f0x57,
/* 0x58 */	&&f0x58, &&f0x59, &&f0x5a, &&f0x5b, &&f0x5c, &&f0x5d, &&f0x5e, &&f0x5f,
/* 0x60 */	&&f0x60, &&f0x61, &&f0x62, &&f0x63, &&pfx64, &&pfx65, &&pfx66, &&pfx67,
/* 0x68 */	&&f0x68, &&f0x69, &&f0x6a, &&f0x6b, &&f0x6c, &&f0x6d, &&f0x6e, &&f0x6f,
/* 0x70 */	&&f0x70, &&f0x71, &&f0x72, &&f0x73, &&f0x74, &&f0x75, &&f0x76, &&f0x77,
/* 0x78 */	&&f0x78, &&f0x79, &&f0x7a, &&f0x7b, &&f0x7c, &&f0x7d, &&f0x7e, &&f0x7f,
/* 0x80 */	&&f0x80, &&f0x81, &&f0x82, &&f0x83, &&f0x84, &&f0x85, &&f0x86, &&f0x87,
/* 0x88 */	&&f0x88, &&f0x89, &&f0x8a, &&f0x8b, &&f0x8c, &&f0x8d, &&f0x8e, &&f0x8f,
/* 0x90 */	&&f0x90, &&f0x91, &&f0x92, &&f0x93, &&f0x94, &&f0x95, &&f0x96, &&f0x97,
/* 0x98 */	&&f0x98, &&f0x99, &&f0x9a, &&f0x9b, &&f0x9c, &&f0x9d, &&f0x9e, &&f0x9f,
/* 0xa0 */	&&f0xa0, &&f0xa1, &&f0xa2, &&f0xa3, &&f0xa4, &&f0xa5, &&f0xa6, &&f0xa7,
/* 0xa8 */	&&f0xa8, &&f0xa9, &&f0xaa, &&f0xab, &&f0xac, &&f0xad, &&f0xae, &&f0xaf,
/* 0xb0 */	&&f0xb0, &&f0xb1, &&f0xb2, &&f0xb3, &&f0xb4, &&f0xb5, &&f0xb6, &&f0xb7,
/* 0xb8 */	&&f0xb8, &&f0xb9, &&f0xba, &&f0xbb, &&f0xbc, &&f0xbd, &&f0xbe, &&f0xbf,
/* 0xc0 */	&&f0xc0, &&f0xc1, &&f0xc2, &&f0xc3, &&f0xc4, &&f0xc5, &&f0xc6, &&f0xc7,
/* 0xc8 */	&&f0xc8, &&f0xc9, &&f0xca, &&f0xcb, &&f0xcc, &&f0xcd, &&f0xce, &&f0xcf,
/* 0xd0 */	&&f0xd0, &&f0xd1, &&f0xd2, &&f0xd3, &&f0xd4, &&f0xd5, &&f0xd6, &&f0xd7,
/* 0xd8 */	&&f0xd8, &&f0xd9, &&f0xda, &&f0xdb, &&f0xdc, &&f0xdd, &&f0xde, &&f0xdf,
/* 0xe0 */	&&f0xe0, &&f0xe1, &&f0xe2, &&f0xe3, &&f0xe4, &&f0xe5, &&f0xe6, &&f0xe7,
/* 0xe8 */	&&f0xe8, &&f0xe9, &&f0xea, &&f0xeb, &&f0xec, &&f0xed, &&f0xee, &&f0xef,
/* 0xf0 */	&&pfxf0, &&f0xf1, &&pfxf2, &&pfxf3, &&f0xf4, &&f0xf5, &&f0xf6, &&f0xf7,
/* 0xf8 */	&&f0xf8, &&f0xf9, &&f0xfa, &&f0xfb, &&f0xfc, &&f0xfd, &&f0xfe, &&f0xff,
	};
	goto *pfxlabel[b1];
#define HANDLE_PREFIX(C, STMT) \
		pfx ## C: { \
			STMT; \
			TRY(fetch8(cpu, &b1)); \
			goto *pfxlabel[b1]; \
		}
		HANDLE_PREFIX(26, curr_seg = SEG_ES)
		HANDLE_PREFIX(2e, curr_seg = SEG_CS)
		HANDLE_PREFIX(36, curr_seg = SEG_SS)
		HANDLE_PREFIX(3e, curr_seg = SEG_DS)
		HANDLE_PREFIX(64, curr_seg = SEG_FS)
		HANDLE_PREFIX(65, curr_seg = SEG_GS)
		HANDLE_PREFIX(66, opsz16 = !code16)
		HANDLE_PREFIX(67, adsz16 = !code16)
		HANDLE_PREFIX(f3, rep = 1) // REP
		HANDLE_PREFIX(f2, rep = 2) // REPNE
		HANDLE_PREFIX(f0, /*lock = true*/)
#undef HANDLE_PREFIX
#endif
	eswitch(b1) {
#define I(_case, _rm, _rwm, _op) _case { _rm(_rwm, _op); ebreak; }
#include "i386ins.def"
#undef I

#undef CX
#define CX(_1) case _1:
#define GRPBEG TRY(peek8a(cpu, &modrm)); switch((modrm >> 3) & 7) {
#define GRPCASE(_case, _rm, _rwm, _op) _case { _rm(_rwm, _op); ebreak; }
#define GRPEND default: default_ud; } ebreak;

	ecase(0x80): ecase(0x82): { // G1b
GRPBEG
#define IG1b GRPCASE
#include "i386ins.def"
#undef IG1b
GRPEND
	}

	ecase(0x81): { // G1v
GRPBEG
#define IG1v GRPCASE
#include "i386ins.def"
#undef IG1v
GRPEND
	}

	ecase(0x83): { // G1vIb
GRPBEG
#define IG1vIb GRPCASE
#include "i386ins.def"
#undef IG1vIb
GRPEND
	}

	ecase(0xc0): { // G2b
GRPBEG
#define IG2b GRPCASE
#include "i386ins.def"
#undef IG2b
GRPEND
	}

	ecase(0xc1): { // G2v
GRPBEG
#define IG2v GRPCASE
#include "i386ins.def"
#undef IG2v
GRPEND
	}

	ecase(0xd0): { // G2b1
GRPBEG
#define IG2b1 GRPCASE
#include "i386ins.def"
#undef IG2b1
GRPEND
	}

	ecase(0xd1): { // G2v1
GRPBEG
#define IG2v1 GRPCASE
#include "i386ins.def"
#undef IG2v1
GRPEND
	}

	ecase(0xd2): { // G2bC
GRPBEG
#define IG2bC GRPCASE
#include "i386ins.def"
#undef IG2bC
GRPEND
	}

	ecase(0xd3): { // G2v1
GRPBEG
#define IG2vC GRPCASE
#include "i386ins.def"
#undef IG2vC
GRPEND
	}

	ecase(0xf6): { // G3b
GRPBEG
#define IG3b GRPCASE
#include "i386ins.def"
#undef IG3b
GRPEND
	}

	ecase(0xf7): { // G3v
GRPBEG
#define IG3v GRPCASE
#include "i386ins.def"
#undef IG3v
GRPEND
	}

	ecase(0xfe): { // G4
GRPBEG
#define IG4 GRPCASE
#include "i386ins.def"
#undef IG4
GRPEND
	}

	ecase(0xff): { // G5
GRPBEG
#define IG5 GRPCASE
#include "i386ins.def"
#undef IG5
GRPEND
	}

	ecase(0x0f): { // two byte
		TRY(fetch8(cpu, &b1));
		switch(b1) {
#define I2(_case, _rm, _rwm, _op) _case { _rm(_rwm, _op); ebreak; }
#include "i386ins.def"
#undef I2

		case 0x00: { // G6
GRPBEG
#define IG6 GRPCASE
#include "i386ins.def"
#undef IG6
GRPEND
		}

		case 0x01: { // G7
GRPBEG
#define IG7 GRPCASE
#include "i386ins.def"
#undef IG7
GRPEND
		}

		case 0xba: { // G8
GRPBEG
#define IG8 GRPCASE
#include "i386ins.def"
#undef IG8
GRPEND
		}

		case 0xc7: { // G9
GRPBEG
#define IG9 GRPCASE
#include "i386ins.def"
#undef IG9
GRPEND
		}
		// three byte
		case 0x38: {
			TRY(fetch8(cpu, &b1));
			switch(b1) {
#define I3_38(_case, _rm, _rwm, _op) _case { _rm(_rwm, _op); ebreak; }
#include "i386ins.def"
#undef I3_38
			default: default_ud;
			}
			ebreak;
		}
		case 0x3a: {
			TRY(fetch8(cpu, &b1));
			switch(b1) {
#define I3_3a(_case, _rm, _rwm, _op) _case { _rm(_rwm, _op); ebreak; }
#include "i386ins.def"
#undef I3_3a
			default: default_ud;
			}
			ebreak;
		}
		default: default_ud;
		}
		ebreak;
	}

	edefault: default_ud;
	}
	}
	return true;
}

// XXX: incomplete
enum { TS_JMP, TS_CALL, TS_IRET };
static bool task_switch(CPUI386 *cpu, int tss, int sw_type)
{
	OptAddr meml;
	int oldtss = cpu->seg[SEG_TR].sel;
	int tr_type = cpu->seg[SEG_TR].flags & 0xf;
	assert (tr_type == 9 || tr_type == 11);

	TRY1(translate(cpu, &meml, 2, SEG_TR, 0x20, 4, 0));
	store32(cpu, &meml, cpu->next_ip);

	refresh_flags(cpu);
	TRY1(translate(cpu, &meml, 2, SEG_TR, 0x24, 4, 0));
	if (sw_type == TS_IRET)
		store32(cpu, &meml, cpu->flags & ~NT);
	else
		store32(cpu, &meml, cpu->flags);

	for (int i = 0; i < 8; i++) {
		TRY1(translate(cpu, &meml, 2, SEG_TR, 0x28 + 4 * i, 4, 0));
		store32(cpu, &meml, REGi(i));
	}

	for (int i = 0; i < 6; i++) {
		TRY1(translate(cpu, &meml, 2, SEG_TR, 0x48 + 4 * i, 4, 0));
		store32(cpu, &meml, cpu->seg[i].sel);
	}

	// clear busy bit
	if (sw_type == TS_JMP || sw_type == TS_IRET) {
		uword addr = cpu->gdt.base + (cpu->seg[SEG_TR].sel & ~0x7);
		TRY1(translate_laddr(cpu, &meml, 3, addr + 4, 4, 0));
		store32(cpu, &meml, load32(cpu, &meml) & ~(1 << 9));
	}

	TRY1(set_seg(cpu, SEG_TR, tss));
	int new_tr_type = cpu->seg[SEG_TR].flags & 0xf;
	assert(new_tr_type == 9 || new_tr_type == 11);

	// set busy bit
	if (sw_type == TS_JMP || sw_type == TS_CALL) {
		uword addr = cpu->gdt.base + (tss & ~0x7);
		TRY1(translate_laddr(cpu, &meml, 3, addr + 4, 4, 0));
		store32(cpu, &meml, load32(cpu, &meml) | (1 << 9));
		cpu->seg[SEG_TR].flags |= 2;
	}

	cpu->cr0 |= 1 << 3; // set TS bit

	TRY1(translate(cpu, &meml, 1, SEG_TR, 0x60, 4, 0));
	TRY1(set_seg(cpu, SEG_LDT, load32(cpu, &meml)));

	for (int i = 0; i < 8; i++) {
		TRY1(translate(cpu, &meml, 1, SEG_TR, 0x28 + 4 * i, 4, 0));
		REGi(i) = load32(cpu, &meml);
	}

	for (int i = 0; i < 6; i++) {
		TRY1(translate(cpu, &meml, 1, SEG_TR, 0x48 + 4 * i, 4, 0));
		TRY1(set_seg(cpu, i, load32(cpu, &meml)));
	}

	TRY1(translate(cpu, &meml, 1, SEG_TR, 0x20, 4, 0));
	cpu->next_ip = load32(cpu, &meml);

	TRY1(translate(cpu, &meml, 1, SEG_TR, 0x24, 4, 0));
	cpu->flags = load32(cpu, &meml);
	cpu->flags &= EFLAGS_MASK;
	cpu->flags |= 0x2;
	cpu->cc.mask = 0;	/* the new task's flags, not the old one's last operation's */
	if (sw_type == TS_CALL) {
		TRY1(translate(cpu, &meml, 2, SEG_TR, 0, 4, 0));
		store32(cpu, &meml, oldtss);
		cpu->flags |= NT;
	}

	TRY1(translate(cpu, &meml, 1, SEG_TR, 0x1c, 4, 0));
	cpu->cr3 = load32(cpu, &meml);
	tlb_clear(cpu);

	return true;
}

static bool pmcall(CPUI386 *cpu, bool opsz16, uword addr, int sel, bool isjmp)
{
	sel = sel & 0xffff;
	uword sp_mask = cpu->seg[SEG_SS].flags & SEG_B_BIT ? 0xffffffff : 0xffff;

	if ((sel & ~0x3) == 0) THROW(EX_GP, 0);

	uword w1, w2;
	TRY(read_desc(cpu, sel, &w1, &w2));

	int s = (w2 >> 12) & 1;
	int dpl = (w2 >> 13) & 0x3;
	int p = (w2 >> 15) & 1;
	if (!p) {
//		dolog("pmcall: seg not present %04x\n", sel);
		THROW(EX_NP, sel & ~0x3);
	}

	if (s) {
		bool code = (w2 >> 8) & 0x8;
		bool conforming = (w2 >> 8) & 0x4;
		if (!code) THROW(EX_GP, sel & ~0x3);
		if (conforming) {
			// call conforming code segment
			if (dpl > cpu->cpl) THROW(EX_GP, sel & ~0x3);
			sel = (sel & 0xfffc) | cpu->cpl;
		} else {
			// call nonconforming code segment
			if ((sel & 0x3) > cpu->cpl || dpl != cpu->cpl)
				THROW(EX_GP, sel & ~0x3);
			sel = (sel & 0xfffc) | cpu->cpl;
		}

		if (!isjmp) {
			OptAddr meml1, meml2;
			uword sp = lreg32(4);
			if (opsz16) {
				TRY(translate(cpu, &meml1, 2, SEG_SS, (sp - 2) & sp_mask, 2, 0));
				TRY(translate(cpu, &meml2, 2, SEG_SS, (sp - 4) & sp_mask, 2, 0));
				set_sp(sp - 4, sp_mask);
				saddr16(&meml1, cpu->seg[SEG_CS].sel);
				saddr16(&meml2, cpu->next_ip);
			} else {
				TRY(translate(cpu, &meml1, 2, SEG_SS, (sp - 4) & sp_mask, 4, 0));
				TRY(translate(cpu, &meml2, 2, SEG_SS, (sp - 8) & sp_mask, 4, 0));
				set_sp(sp - 8, sp_mask);
				saddr32(&meml1, cpu->seg[SEG_CS].sel);
				saddr32(&meml2, cpu->next_ip);
			}
		}
//		if ((sel & 3) != cpu->cpl)
//			dolog("pmcall PVL %d => %d\n", cpu->cpl, sel & 3);
		if (!set_seg(cpu, SEG_CS, sel)) {
#ifdef AMD64_ENABLE_LEG32
			if (cpu->excno == 0x8664) {
				cpu->ip = addr;
				return false;
			}
#endif
			TRY1(false);
		}
		cpu->next_ip = addr;
	} else {
		int newcs = w1 >> 16;
		uword newip = (w1 & 0xffff) | (w2 & 0xffff0000);
		int gt = (w2 >> 8) & 0xf;
		int wc = w2 & 31;

		if (dpl < cpu->cpl || dpl < (sel & 3))
			THROW(EX_GP, sel & ~0x3);

		// only 32bit TSS is supported now
		int tr_type = cpu->seg[SEG_TR].flags & 0xf;
		if (tr_type == 9 || tr_type == 11) {
			if (gt == 9) {
				// 32 bit TSS avail segs
				return task_switch(cpu, sel,
						   isjmp ? TS_JMP : TS_CALL);
			}

			if (gt == 5) {
				// task gates
				return task_switch(cpu, newcs,
						   isjmp ? TS_JMP : TS_CALL);
			}
		}

		if (gt != 4 && gt != 12) {
			fprintf(stderr, "gate type = %d\n", gt);
			cpu_abort(cpu, -203);
		}

		// call gates
		// examine code segment selector in call gate descriptor
		if ((newcs & ~0x3) == 0) THROW(EX_GP, 0);
		uword neww2;
		TRY(read_desc(cpu, newcs, NULL, &neww2));

		// if not code segment
		if (((neww2 >> 11) & 0x3) != 0x3)
			THROW(EX_GP, newcs & ~0x3);

		int newdpl = (neww2 >> 13) & 0x3;
		int newp = (neww2 >> 15) & 1;
		if (!newp) THROW(EX_NP, newcs & ~0x3);
		if (newdpl > cpu->cpl) THROW(EX_GP, newcs & ~0x3);

		bool conforming = (neww2 >> 8) & 0x4;
		bool gate16 = (gt == 4);
		if (gate16) newip &= 0xffff;
		if (!conforming && newdpl < cpu->cpl) {
			// more privilege
			OptAddr msp0, mss0;
			uword oldss = cpu->seg[SEG_SS].sel;
			uword oldsp = REGi(4);
			uword params[31];
			uword sp_mask = cpu->seg[SEG_SS].flags & SEG_B_BIT ? 0xffffffff : 0xffff;

			if (!gate16) {
				for (int i = 0; i < wc; i++) {
					OptAddr meml;
					TRY(translate(cpu, &meml, 1, SEG_SS, (oldsp + 4 * i) & sp_mask, 4, 0));
					params[i] = laddr32(&meml);
				}
			} else {
				for (int i = 0; i < wc; i++) {
					OptAddr meml;
					TRY(translate(cpu, &meml, 1, SEG_SS, (oldsp + 2 * i) & sp_mask, 2, 0));
					params[i] = laddr16(&meml);
				}
			}

			if (!(cpu->seg[SEG_TR].flags & 0x8)) {
				TRY(translate(cpu, &msp0, 1, SEG_TR, 2 + 4 * newdpl, 2, 0));
				TRY(translate(cpu, &mss0, 1, SEG_TR, 4 + 4 * newdpl, 2, 0));
				// TODO: Check SS...
				REGi(4) = load16(cpu, &msp0);
				TRY(set_seg(cpu, SEG_SS, load16(cpu, &mss0)));
			} else {
				TRY(translate(cpu, &msp0, 1, SEG_TR, 4 + 8 * newdpl, 4, 0));
				TRY(translate(cpu, &mss0, 1, SEG_TR, 8 + 8 * newdpl, 4, 0));
				// TODO: Check SS...
				REGi(4) = load32(cpu, &msp0);
				TRY(set_seg(cpu, SEG_SS, load32(cpu, &mss0)));
			}
			sp_mask = cpu->seg[SEG_SS].flags & SEG_B_BIT ? 0xffffffff : 0xffff;

			if (!isjmp) {
			if (!gate16) {
				OptAddr meml1, meml2, meml3, meml4;
				uword sp = lreg32(4);
				TRY1(translate(cpu, &meml1, 2, SEG_SS, (sp - 4 * 1) & sp_mask, 4, 0));
				TRY1(translate(cpu, &meml2, 2, SEG_SS, (sp - 4 * 2) & sp_mask, 4, 0));
				TRY1(translate(cpu, &meml3, 2, SEG_SS, (sp - 4 * (3 + wc)) & sp_mask, 4, 0));
				TRY1(translate(cpu, &meml4, 2, SEG_SS, (sp - 4 * (4 + wc)) & sp_mask, 4, 0));

				for (int i = 0; i < wc; i++) {
					OptAddr meml;
					TRY1(translate(cpu, &meml, 2, SEG_SS, (sp - 4 * (2 + wc - i)) & sp_mask, 4, 0));
					saddr32(&meml, params[i]);
				}

				saddr32(&meml1, oldss);
				saddr32(&meml2, oldsp);
				saddr32(&meml3, cpu->seg[SEG_CS].sel);
				saddr32(&meml4, cpu->next_ip);
				set_sp(sp - 4 * (4 + wc), sp_mask);
			} else {
				OptAddr meml1, meml2, meml3, meml4;
				uword sp = lreg32(4);
				TRY1(translate(cpu, &meml1, 2, SEG_SS, (sp - 2 * 1) & sp_mask, 2, 0));
				TRY1(translate(cpu, &meml2, 2, SEG_SS, (sp - 2 * 2) & sp_mask, 2, 0));
				TRY1(translate(cpu, &meml3, 2, SEG_SS, (sp - 2 * (3 + wc)) & sp_mask, 2, 0));
				TRY1(translate(cpu, &meml4, 2, SEG_SS, (sp - 2 * (4 + wc)) & sp_mask, 2, 0));

				for (int i = 0; i < wc; i++) {
					OptAddr meml;
					TRY1(translate(cpu, &meml, 2, SEG_SS, (sp - 2 * (2 + wc - i)) & sp_mask, 2, 0));
					saddr16(&meml, params[i]);
				}

				saddr16(&meml1, oldss);
				saddr16(&meml2, oldsp);
				saddr16(&meml3, cpu->seg[SEG_CS].sel);
				saddr16(&meml4, cpu->next_ip);
				set_sp(sp - 2 * (4 + wc), sp_mask);
			}
			}
			newcs = (newcs & 0xfffc) | newdpl;
		} else {
			// same privilege
			if (!isjmp) {
			OptAddr meml1, meml2;
			uword sp = lreg32(4);
			uword sp_mask = cpu->seg[SEG_SS].flags & SEG_B_BIT ? 0xffffffff : 0xffff;
			if (gate16) {
				TRY(translate(cpu, &meml1, 2, SEG_SS, (sp - 2 * 1) & sp_mask, 2, 0));
				TRY(translate(cpu, &meml2, 2, SEG_SS, (sp - 2 * 2) & sp_mask, 2, 0));
				saddr16(&meml1, cpu->seg[SEG_CS].sel);
				saddr16(&meml2, cpu->next_ip);
				set_sp(sp - 2 * 2, sp_mask);
			} else {
				TRY(translate(cpu, &meml1, 2, SEG_SS, (sp - 4 * 1) & sp_mask, 4, 0));
				TRY(translate(cpu, &meml2, 2, SEG_SS, (sp - 4 * 2) & sp_mask, 4, 0));
				saddr32(&meml1, cpu->seg[SEG_CS].sel);
				saddr32(&meml2, cpu->next_ip);
				set_sp(sp - 4 * 2, sp_mask);
			}
			}
			newcs = (newcs & 0xfffc) | cpu->cpl;
		}

		TRY1(set_seg(cpu, SEG_CS, newcs));

		cpu->next_ip = newip;
	}
	return true;
}

// 0: exception
// 1: intra PVL
// 2: inter PVL
// 3: from v8086
static int __call_isr_check_cs(CPUI386 *cpu, int sel, int ext, int *csdpl)
{
	sel = sel & 0xffff;
	OptAddr meml;
	uword off = sel & ~0x7;
	uword base;
	uword limit;
	if (sel & 0x4) {
		base = cpu->seg[SEG_LDT].base;
		limit = cpu->seg[SEG_LDT].limit;
	} else {
		base = cpu->gdt.base;
		limit = cpu->gdt.limit;
	}
	if ((sel & ~0x3) == 0 || off + 7 > limit)
		THROW(EX_GP, ext);

	TRY1(translate_laddr(cpu, &meml, 1, base + off + 4, 4, 0));
	uword w2 = load32(cpu, &meml);
	int s = (w2 >> 12) & 1;
	bool code = (w2 >> 8) & 0x8;
	bool conforming = (w2 >> 8) & 0x4;
	int dpl = (w2 >> 13) & 0x3;
	int p = (w2 >> 15) & 1;
	*csdpl = dpl;
	if (!s || !code || dpl > cpu->cpl)
		THROW(EX_GP, (sel & ~0x3) | ext);

	if (!p) THROW(EX_NP, sel & ~0x3);

	if (!conforming && dpl < cpu->cpl) {
		if (!(cpu->flags & VM)) {
			return 2;
		} else {
			if (dpl != 0) {
				dolog("__call_isr_check_cs fail1: %d %d %d\n", conforming, dpl, cpu->cpl);
				THROW(EX_GP, (sel & ~0x3) | ext);
			} else {
				return 3;
			}
		}
	} else {
		if (cpu->flags & VM) {
			THROW(EX_GP, (sel & ~0x3) | ext);
		} else {
			if (conforming || dpl == cpu->cpl) {
				return 1;
			} else {
				dolog("__call_isr_check_cs fail2: %d %d %d\n", conforming, dpl, cpu->cpl);
				THROW(EX_GP, (sel & ~0x3) | ext);
			}
		}
	}
}

static bool IRAM_ATTR call_isr(CPUI386 *cpu, int no, bool pusherr, int ext)
{
	if (!(cpu->cr0 & 1)) {
		/* REAL-ADDRESS-MODE */
		uword sp_mask = cpu->seg[SEG_SS].flags & SEG_B_BIT ? 0xffffffff : 0xffff;
		OptAddr meml;
		uword base = cpu->idt.base;
		int off = no * 4;
		TRY1(translate_laddr(cpu, &meml, 1, base + off, 4, 0));
		uword w1 = load32(cpu, &meml);
		int newcs = w1 >> 16;
		uword newip = w1 & 0xffff;

		OptAddr meml1, meml2, meml3;
		uword sp = lreg32(4);
		TRY1(translate(cpu, &meml1, 2, SEG_SS, (sp - 2 * 1) & sp_mask, 2, 0));
		TRY1(translate(cpu, &meml2, 2, SEG_SS, (sp - 2 * 2) & sp_mask, 2, 0));
		TRY1(translate(cpu, &meml3, 2, SEG_SS, (sp - 2 * 3) & sp_mask, 2, 0));
		refresh_flags(cpu);
		cpu->cc.mask = 0;
		saddr16(&meml1, cpu->flags);
		saddr16(&meml2, cpu->seg[SEG_CS].sel);
		saddr16(&meml3, cpu->ip);
		sreg32(4, (sp - 2 * 3) & sp_mask);

		TRY1(set_seg(cpu, SEG_CS, newcs));
		cpu->next_ip = newip;
		cpu->ip = newip;
		cpu->flags &= ~(IF|TF);
		return true;
	}

	/* PROTECTED-MODE */
	OptAddr meml;
	uword base = cpu->idt.base;
	int off = no << 3;
	if (off + 7 > cpu->idt.limit) {
		dolog("call_isr error0 %d %d\n", off, cpu->idt.limit);
		THROW(EX_GP, off | 2 | ext);
	}

	TRY1(translate_laddr(cpu, &meml, 1, base + off, 4, 0));
	uword w1 = load32(cpu, &meml);
	TRY1(translate_laddr(cpu, &meml, 1, base + off + 4, 4, 0));
	uword w2 = load32(cpu, &meml);

	int gt = (w2 >> 8) & 0xf;
	if (gt != 6 && gt != 7 && gt != 0xe && gt != 0xf && gt != 5) {
//		dolog("call_isr error1 gt=%d\n", gt);
		THROW(EX_GP, off | 2 | ext);
	}

	int dpl = (w2 >> 13) & 0x3;
	if (!ext && dpl < cpu->cpl) THROW(EX_GP, off | 2);

	int p = (w2 >> 15) & 1;
	if (!p) {
		dolog("call_isr error3\n");
		THROW(EX_NP, off | 2 | ext);
	}

	/* task gate */
	if (gt == 5) {
		TRY(task_switch(cpu, w1 >> 16, TS_CALL));
		/* an exception's error code goes onto the new task's stack (a 32-bit TSS's: 4 bytes) */
		if (pusherr) {
			OptAddr meml1;
			uword sp = lreg32(4);
			uword sp_mask = cpu->seg[SEG_SS].flags & SEG_B_BIT ? 0xffffffff : 0xffff;
			TRY(translate(cpu, &meml1, 2, SEG_SS, (sp - 4) & sp_mask, 4, cpu->cpl));
			saddr32(&meml1, cpu->excerr);
			set_sp(sp - 4, sp_mask);
		}
		return true;
	}

	/* TRAP-OR-INTERRUPT-GATE */
	int newcs = w1 >> 16;
	uword newip = (w1 & 0xffff) | (w2 & 0xffff0000);
	bool gate16 = gt == 6 || gt == 7;
	if (gate16) newip &= 0xffff;

	int csdpl;
	switch(__call_isr_check_cs(cpu, newcs, ext, &csdpl)) {
	case 0: {
		return false;
	}
	case 1: /* intra PVL */ {
		OptAddr meml1, meml2, meml3, meml4;
		uword sp = lreg32(4);
		uword sp_mask = cpu->seg[SEG_SS].flags & SEG_B_BIT ? 0xffffffff : 0xffff;
		if (gate16) {
			TRY(translate(cpu, &meml1, 2, SEG_SS, (sp - 2 * 1) & sp_mask, 2, 0));
			TRY(translate(cpu, &meml2, 2, SEG_SS, (sp - 2 * 2) & sp_mask, 2, 0));
			TRY(translate(cpu, &meml3, 2, SEG_SS, (sp - 2 * 3) & sp_mask, 2, 0));
			if (pusherr) {
				TRY(translate(cpu, &meml4, 2, SEG_SS, (sp - 2 * 4) & sp_mask, 2, 0));
			}

			refresh_flags(cpu);
			cpu->cc.mask = 0;
			saddr16(&meml1, cpu->flags);

			saddr16(&meml2, cpu->seg[SEG_CS].sel);
			saddr16(&meml3, cpu->ip);
			if (pusherr) {
				saddr16(&meml4, cpu->excerr);
				set_sp(sp - 2 * 4, sp_mask);
			} else {
				set_sp(sp - 2 * 3, sp_mask);
			}
		} else {
			TRY(translate(cpu, &meml1, 2, SEG_SS, (sp - 4 * 1) & sp_mask, 4, 0));
			TRY(translate(cpu, &meml2, 2, SEG_SS, (sp - 4 * 2) & sp_mask, 4, 0));
			TRY(translate(cpu, &meml3, 2, SEG_SS, (sp - 4 * 3) & sp_mask, 4, 0));
			if (pusherr) {
				TRY(translate(cpu, &meml4, 2, SEG_SS, (sp - 4 * 4) & sp_mask, 4, 0));
			}

			refresh_flags(cpu);
			cpu->cc.mask = 0;
			saddr32(&meml1, cpu->flags);

			saddr32(&meml2, cpu->seg[SEG_CS].sel);
			saddr32(&meml3, cpu->ip);
			if (pusherr) {
				saddr32(&meml4, cpu->excerr);
				set_sp(sp - 4 * 4, sp_mask);
			} else {
				set_sp(sp - 4 * 3, sp_mask);
			}
		}
		newcs = (newcs & (~3)) | cpu->cpl;
		break;
	}
	case 2: /* inter PVL */ {
//		dolog("call_isr %d %x PVL %d => %d\n", no, no, cpu->cpl, csdpl);
		OptAddr msp0, mss0;
		int newpl = csdpl;
		uword oldss = cpu->seg[SEG_SS].sel;
		uword oldsp = REGi(4);
		uword newss, newsp;
		if (cpu->seg[SEG_TR].flags & 0x8) {
			TRY(translate(cpu, &msp0, 1, SEG_TR, 4 + 8 * newpl, 4, 0));
			TRY(translate(cpu, &mss0, 1, SEG_TR, 8 + 8 * newpl, 4, 0));
			newsp = load32(cpu, &msp0);
			newss = load32(cpu, &mss0) & 0xffff;
		} else {
			TRY(translate(cpu, &msp0, 1, SEG_TR, 2 + 4 * newpl, 2, 0));
			TRY(translate(cpu, &mss0, 1, SEG_TR, 4 + 4 * newpl, 2, 0));
			newsp = load16(cpu, &msp0);
			newss = load16(cpu, &mss0);
		}

		REGi(4) = newsp;
		TRY(set_seg(cpu, SEG_SS, newss));
		uword sp_mask = cpu->seg[SEG_SS].flags & SEG_B_BIT ? 0xffffffff : 0xffff;
		OptAddr meml1, meml2, meml3, meml4, meml5, meml6;
		uword sp = lreg32(4);
		if (gate16) {
			TRY(translate(cpu, &meml1, 2, SEG_SS, (sp - 2 * 1) & sp_mask, 2, 0));
			TRY(translate(cpu, &meml2, 2, SEG_SS, (sp - 2 * 2) & sp_mask, 2, 0));
			TRY(translate(cpu, &meml3, 2, SEG_SS, (sp - 2 * 3) & sp_mask, 2, 0));
			TRY(translate(cpu, &meml4, 2, SEG_SS, (sp - 2 * 4) & sp_mask, 2, 0));
			TRY(translate(cpu, &meml5, 2, SEG_SS, (sp - 2 * 5) & sp_mask, 2, 0));
			if (pusherr) {
				TRY(translate(cpu, &meml6, 2, SEG_SS, (sp - 2 * 6) & sp_mask, 2, 0));
			}
			saddr16(&meml1, oldss);
			saddr16(&meml2, oldsp);

			refresh_flags(cpu);
			cpu->cc.mask = 0;
			saddr16(&meml3, cpu->flags);

			saddr16(&meml4, cpu->seg[SEG_CS].sel);
			saddr16(&meml5, cpu->ip);
			if (pusherr) {
				saddr16(&meml6, cpu->excerr);
				set_sp(sp - 2 * 6, sp_mask);
			} else {
				set_sp(sp - 2 * 5, sp_mask);
			}
		} else {
			TRY(translate(cpu, &meml1, 2, SEG_SS, (sp - 4 * 1) & sp_mask, 4, 0));
			TRY(translate(cpu, &meml2, 2, SEG_SS, (sp - 4 * 2) & sp_mask, 4, 0));
			TRY(translate(cpu, &meml3, 2, SEG_SS, (sp - 4 * 3) & sp_mask, 4, 0));
			TRY(translate(cpu, &meml4, 2, SEG_SS, (sp - 4 * 4) & sp_mask, 4, 0));
			TRY(translate(cpu, &meml5, 2, SEG_SS, (sp - 4 * 5) & sp_mask, 4, 0));
			if (pusherr) {
				TRY(translate(cpu, &meml6, 2, SEG_SS, (sp - 4 * 6) & sp_mask, 4, 0));
			}
			saddr32(&meml1, oldss);
			saddr32(&meml2, oldsp);

			refresh_flags(cpu);
			cpu->cc.mask = 0;
			saddr32(&meml3, cpu->flags);

			saddr32(&meml4, cpu->seg[SEG_CS].sel);
			saddr32(&meml5, cpu->ip);
			if (pusherr) {
				saddr32(&meml6, cpu->excerr);
				sreg32(4, sp - 4 * 6);
			} else {
				sreg32(4, sp - 4 * 5);
			}
		}
		newcs = (newcs & (~3)) | newpl;
		break;
	}
	case 3: /* from v8086 */ {
//		dolog("int from v8086\n");
		if (csdpl != 0) cpu_abort(cpu, -205);
		if (gate16) cpu_abort(cpu, -206);
//		dolog("call_isr %d %x PVL %d => 0\n", no, no, cpu->cpl, csdpl);
		OptAddr msp0, mss0;
		int newpl = 0;
		uword oldss = cpu->seg[SEG_SS].sel;
		uword oldsp = REGi(4);
		uword newss, newsp;
		if (!(cpu->seg[SEG_TR].flags & 0x8)) cpu_abort(cpu, -207);
		TRY(translate(cpu, &msp0, 1, SEG_TR, 4 + 8 * newpl, 4, 0));
		TRY(translate(cpu, &mss0, 1, SEG_TR, 8 + 8 * newpl, 4, 0));
		newsp = load32(cpu, &msp0);
		newss = load32(cpu, &mss0) & 0xffff;
		uword oldflags = cpu->flags;
		cpu->flags &= ~VM;
		REGi(4) = newsp;
		if (!set_seg(cpu, SEG_SS, newss)) {
			cpu->flags = oldflags;
			REGi(4) = oldsp;
			return false;
		}

		uword sp_mask = cpu->seg[SEG_SS].flags & SEG_B_BIT ? 0xffffffff : 0xffff;
		OptAddr memlg, memlf, memld, memle;
		OptAddr meml1, meml2, meml3, meml4, meml5, meml6;
		uword sp = lreg32(4);
		TRY1(translate(cpu, &memlg, 2, SEG_SS, (sp - 4 * 1) & sp_mask, 4, 0));
		TRY1(translate(cpu, &memlf, 2, SEG_SS, (sp - 4 * 2) & sp_mask, 4, 0));
		TRY1(translate(cpu, &memld, 2, SEG_SS, (sp - 4 * 3) & sp_mask, 4, 0));
		TRY1(translate(cpu, &memle, 2, SEG_SS, (sp - 4 * 4) & sp_mask, 4, 0));
		TRY1(translate(cpu, &meml1, 2, SEG_SS, (sp - 4 * 5) & sp_mask, 4, 0));
		TRY1(translate(cpu, &meml2, 2, SEG_SS, (sp - 4 * 6) & sp_mask, 4, 0));
		TRY1(translate(cpu, &meml3, 2, SEG_SS, (sp - 4 * 7) & sp_mask, 4, 0));
		TRY1(translate(cpu, &meml4, 2, SEG_SS, (sp - 4 * 8) & sp_mask, 4, 0));
		TRY1(translate(cpu, &meml5, 2, SEG_SS, (sp - 4 * 9) & sp_mask, 4, 0));
		if (pusherr) {
			TRY(translate(cpu, &meml6, 2, SEG_SS, (sp - 4 * 10) & sp_mask, 4, 0));
		}
		saddr32(&memlg, cpu->seg[SEG_GS].sel);
		saddr32(&memlf, cpu->seg[SEG_FS].sel);
		saddr32(&memld, cpu->seg[SEG_DS].sel);
		saddr32(&memle, cpu->seg[SEG_ES].sel);
		saddr32(&meml1, oldss);
		saddr32(&meml2, oldsp);

		refresh_flags(cpu);
		cpu->cc.mask = 0;
		saddr32(&meml3, cpu->flags | VM);

		saddr32(&meml4, cpu->seg[SEG_CS].sel);
		saddr32(&meml5, cpu->ip);
		if (pusherr) {
			saddr32(&meml6, cpu->excerr);
			set_sp(sp - 4 * 10, sp_mask);
		} else {
			set_sp(sp - 4 * 9, sp_mask);
		}

		newcs = (newcs & (~3)) | newpl;
		TRY1(set_seg(cpu, SEG_DS, 0));
		TRY1(set_seg(cpu, SEG_ES, 0));
		TRY1(set_seg(cpu, SEG_FS, 0));
		TRY1(set_seg(cpu, SEG_GS, 0));
		cpu->flags &= ~(TF | RF | NT);
		TRY1(set_seg(cpu, SEG_CS, newcs));
		cpu->next_ip = newip;
		cpu->ip = newip;
		if (gt == 0x6 || gt == 0xe)
			cpu->flags &= ~IF;
		return true;
	}
	default: assert(false);
	}
	TRY1(set_seg(cpu, SEG_CS, newcs));
	cpu->next_ip = newip;
	cpu->ip = newip;
	cpu->flags &= ~(TF | RF | NT);
	if (gt == 0x6 || gt == 0xe)
		cpu->flags &= ~IF;
	return true;
}

static bool __pmiret_check_cs_same(CPUI386 *cpu, int sel)
{
	sel = sel & 0xffff;
	if ((sel & ~0x3) == 0) {
		dolog("__pmiret_check_cs_same: sel %04x\n", sel);
		THROW(EX_GP, sel & ~0x3);
	}
	uword w2;
	TRY(read_desc(cpu, sel, NULL, &w2));

	int s = (w2 >> 12) & 1;
	bool code = (w2 >> 8) & 0x8;
	bool conforming = (w2 >> 8) & 0x4;
	int dpl = (w2 >> 13) & 0x3;
	int p = (w2 >> 15) & 1;

	if (!s || !code) THROW(EX_GP, sel & ~0x3);

	if (!conforming) {
		if (dpl != cpu->cpl) THROW(EX_GP, sel & ~0x3);
	} else {
		if (dpl > cpu->cpl) THROW(EX_GP, sel & ~0x3);
	}

	if (!p) {
//		dolog("__pmiret_check_cs_same: seg not present %04x\n", sel);
		THROW(EX_NP, sel & ~0x3);
	}
	return true;
}

static bool __pmiret_check_cs_outer(CPUI386 *cpu, int sel)
{
	sel = sel & 0xffff;
	if ((sel & ~0x3) == 0) {
		dolog("__pmiret_check_cs_outer: sel %04x\n", sel);
		THROW(EX_GP, sel & ~0x3);
	}
	uword w2;
	TRY(read_desc(cpu, sel, NULL, &w2));

	int s = (w2 >> 12) & 1;
	bool code = (w2 >> 8) & 0x8;
	bool conforming = (w2 >> 8) & 0x4;
	int dpl = (w2 >> 13) & 0x3;
	int p = (w2 >> 15) & 1;
	int rpl = sel & 3;

	if (!s || !code) THROW(EX_GP, sel & ~0x3);

	if (!conforming) {
		if (dpl != rpl) THROW(EX_GP, sel & ~0x3);
	} else {
		if (dpl <= cpu->cpl) THROW(EX_GP, sel & ~0x3);
	}

	if (!p) {
//		dolog("__pmiret_check_cs_outer: seg not present %04x\n", sel);
		THROW(EX_NP, sel & ~0x3);
	}
	return true;
}

static bool pmret(CPUI386 *cpu, bool opsz16, int off, bool isiret)
{
	if (isiret) {
		if ((cpu->flags & VM)) THROW(EX_GP, 0);
		if ((cpu->flags & NT)) {
			OptAddr meml;
			TRY(translate(cpu, &meml, 1, SEG_TR, 0, 2, 0));
			int tssback = laddr16(&meml);
			dolog("IRET NT: tss curr: %04x back: %04x\n",
			      cpu->seg[SEG_TR].sel, tssback);
			// win2000 needs it...
			if (tssback == 0) THROW(EX_TS, 0);
			return task_switch(cpu, tssback, TS_IRET);
		}
		if (opsz16)
			off += 2;
		else
			off += 4;
	}

	OptAddr meml1, meml2, meml3, meml4, meml5;
	uword sp_mask = cpu->seg[SEG_SS].flags & SEG_B_BIT ? 0xffffffff : 0xffff;
	uword sp = lreg32(4);
	uword oldflags = cpu->flags;
	uword newip;
	int newcs;
	uword newflags = 0; // make the compiler happy
	if (opsz16) {
		/* ip */ TRY(translate(cpu, &meml1, 1, SEG_SS, sp & sp_mask, 2, 0));
		/* cs */ TRY(translate(cpu, &meml2, 1, SEG_SS, (sp + 2) & sp_mask, 2, 0));
		if (isiret) {
			/* flags */ TRY(translate(cpu, &meml3, 1, SEG_SS, (sp + 4) & sp_mask, 2, 0));
			newflags = (oldflags & 0xffff0000) | laddr16(&meml3);
		}
		newip = laddr16(&meml1);
		newcs = laddr16(&meml2);
	} else {
		/* ip */ TRY(translate(cpu, &meml1, 1, SEG_SS, sp & sp_mask, 4, 0));
		/* cs */ TRY(translate(cpu, &meml2, 1, SEG_SS, (sp + 4) & sp_mask, 4, 0));
		if (isiret) {
			/* flags */ TRY(translate(cpu, &meml3, 1, SEG_SS, (sp + 8) & sp_mask, 4, 0));
			newflags = laddr32(&meml3);
		}
		newip = laddr32(&meml1);
		newcs = laddr32(&meml2);
	}

	if (isiret) {
		uword mask = 0;
		if (cpu->cpl > 0) mask |= IOPL;
		if (get_IOPL(cpu) < cpu->cpl) mask |= IF;
		newflags = (oldflags & mask) | (newflags & ~mask);
		newflags &= EFLAGS_MASK;
		newflags |= 0x2;
	}

	if (isiret && (newflags & VM)) {
		if (cpu->cpl != 0) cpu_abort(cpu, -208);
		// return to v8086
//		dolog("pmiret PVL %d => %d (vm) %04x:%08x\n", cpu->cpl, 3, newcs, newip);
		OptAddr meml_vmes, meml_vmds, meml_vmfs, meml_vmgs;
		if (opsz16) cpu_abort(cpu, -209);
		TRY(translate(cpu, &meml4, 1, SEG_SS, (sp + 12) & sp_mask, 4, 0));
		TRY(translate(cpu, &meml5, 1, SEG_SS, (sp + 16) & sp_mask, 4, 0));
		TRY(translate(cpu, &meml_vmes, 1, SEG_SS, (sp + 20) & sp_mask, 4, 0));
		TRY(translate(cpu, &meml_vmds, 1, SEG_SS, (sp + 24) & sp_mask, 4, 0));
		TRY(translate(cpu, &meml_vmfs, 1, SEG_SS, (sp + 28) & sp_mask, 4, 0));
		TRY(translate(cpu, &meml_vmgs, 1, SEG_SS, (sp + 32) & sp_mask, 4, 0));
		cpu->flags = newflags;
		TRY1(set_seg(cpu, SEG_CS, newcs));
		set_sp(sp + 12, sp_mask);
		cpu->next_ip = newip & 0xffff;
		TRY1(set_seg(cpu, SEG_SS, laddr32(&meml5)));
		TRY1(set_seg(cpu, SEG_ES, laddr32(&meml_vmes)));
		TRY1(set_seg(cpu, SEG_DS, laddr32(&meml_vmds)));
		TRY1(set_seg(cpu, SEG_FS, laddr32(&meml_vmfs)));
		TRY(set_seg(cpu, SEG_GS, laddr32(&meml_vmgs)));
		set_sp(laddr32(&meml4), 0xffffffff);
	} else {
		int rpl = newcs & 3;
		if (rpl < cpu->cpl) THROW(EX_GP, newcs & ~0x3);
		if (rpl == cpu->cpl) {
			// return to same level
			TRY(__pmiret_check_cs_same(cpu, newcs));
//			dolog("pmiret PVL %d => %d %04x:%08x\n", cpu->cpl, newcs & 3, newcs, newip);
			if (isiret)
				cpu->flags = newflags;
			if (!set_seg(cpu, SEG_CS, newcs)) {
#ifdef AMD64_ENABLE_LEG32
				if (cpu->excno == 0x8664) {
					cpu->ip = newip;
					set_sp(sp + 8 + off, sp_mask);
					if (isiret)
						cpu->cc.mask = 0;
					return false;
				}
#endif
				TRY1(false);
			}

			if (opsz16) {
				set_sp(sp + 4 + off, sp_mask);
			} else {
				set_sp(sp + 8 + off, sp_mask);
			}
			cpu->next_ip = newip;
		} else {
			// return to outer level
			TRY(__pmiret_check_cs_outer(cpu, newcs));
			uword newsp;
			uword newss;
//			dolog("pmiret PVL %d => %d %04x:%08x\n", cpu->cpl, newcs & 3, newcs, newip);
			if (opsz16) {
				/* sp */ TRY(translate(cpu, &meml4, 1, SEG_SS, (sp + 4 + off) & sp_mask, 2, 0));
				/* ss */ TRY(translate(cpu, &meml5, 1, SEG_SS, (sp + 6 + off) & sp_mask, 2, 0));
				newsp = laddr16(&meml4);
				newss = laddr16(&meml5);
			} else {
				/* sp */ TRY(translate(cpu, &meml4, 1, SEG_SS, (sp + 8 + off) & sp_mask, 4, 0));
				/* ss */ TRY(translate(cpu, &meml5, 1, SEG_SS, (sp + 12 + off) & sp_mask, 4, 0));
				newsp = laddr32(&meml4);
				newss = laddr32(&meml5);
			}
			if (!isiret)
				newsp += off;

			if (isiret)
				cpu->flags = newflags;
			TRY1(set_seg(cpu, SEG_CS, newcs));
			TRY1(set_seg(cpu, SEG_SS, newss));
			uword newsp_mask = cpu->seg[SEG_SS].flags & SEG_B_BIT ? 0xffffffff : 0xffff;
			set_sp(newsp, newsp_mask);
			cpu->next_ip = newip;
			clear_segs(cpu);
		}
	}
	if (isiret)
		cpu->cc.mask = 0;
	return true;
}

void cpui386_step(CPUI386 *cpu, int stepcount)
{
	if ((cpu->flags & IF) && cpu->intr) {
		cpu->intr = false;
		cpu->halt = false;
		int no = cpu->cb.pic_read_irq(cpu->cb.pic);
		cpu->ip = cpu->next_ip;
		TRY1(call_isr(cpu, no, false, 1));
	}

	if (cpu->halt) {
		usleep(1);
		return;
	}

	int ret = cpu_exec1(cpu, stepcount);
	cpu->ifetch.paddr = 0;
	if (!ret) {
#ifdef AMD64_ENABLE_LEG32
		if (cpu->excno == 0x8664)
			return;
#endif
		bool pusherr = false;
		switch (cpu->excno) {
		case EX_DF: case EX_TS: case EX_NP: case EX_SS: case EX_GP:
		case EX_PF:
			pusherr = true;
		}
		cpu->next_ip = cpu->ip;

		TRY1(call_isr(cpu, cpu->excno, pusherr, 1));
	}
}

void cpu_setax(CPUI386 *cpu, u16 ax)
{
	sreg16(0, ax);
}

u16 cpu_getax(CPUI386 *cpu)
{
	return lreg16(0);
}

void cpu_setexc(CPUI386 *cpu, int excno, uword excerr)
{
	cpu->excno = excno;
	cpu->excerr = excerr;
}

void cpu_setflags(CPUI386 *cpu, uword set_mask, uword clear_mask)
{
	if (cpu->cc.mask & (set_mask | clear_mask)) {
		refresh_flags(cpu);
		cpu->cc.mask = 0;
	}
	cpu->flags |= set_mask;
	cpu->flags &= ~clear_mask;
	cpu->flags &= EFLAGS_MASK;
}

uword cpu_getflags(CPUI386 *cpu)
{
	if (cpu->cc.mask) {
		refresh_flags(cpu);
		cpu->cc.mask = 0;
	}
	return cpu->flags;
}

void cpui386_reset(CPUI386 *cpu)
{
	for (int i = 0; i < 8; i++) {
		REGi(i) = 0;
	}
	cpu->flags = 0x2;
	cpu->cpl = 0;
	cpu->code16 = true;
	cpu->sp_mask = 0xffff;
	cpu->halt = false;

	for (int i = 0; i < 8; i++) {
		cpu->seg[i].sel = 0;
		cpu->seg[i].base = 0;
		cpu->seg[i].limit = 0;
		cpu->seg[i].flags = 0;
	}
	cpu->seg[2].flags = (1 << 22);
	cpu->seg[1].flags = (1 << 22);

	cpu->ip = 0xfff0;
	cpu->next_ip = cpu->ip;
	cpu->seg[SEG_CS].sel = 0xf000;
	cpu->seg[SEG_CS].base = 0xf0000;

	cpu->idt.base = 0;
	cpu->idt.limit = 0x3ff;
	cpu->gdt.base = 0;
	cpu->gdt.limit = 0;

	cpu->cr0 = cpu->fpu ? 0x10 : 0;
	cpu->cr2 = 0;
	cpu->cr3 = 0;
	for (int i = 0; i < 8; i++)
		cpu->dr[i] = 0;

	cpu->cc.mask = 0;
	tlb_clear(cpu);
	cpu->excno = 0;
	cpu->excerr = 0;

	cpu->sysenter.cs = 0;
	cpu->sysenter.eip = 0;
	cpu->sysenter.esp = 0;
	INIT_LM_EFER(cpu);
}

void cpui386_reset_pm(CPUI386 *cpu, uint32_t start_addr)
{
	cpui386_reset(cpu);
	cpu->cr0 = 1;
	cpu->seg[SEG_CS].sel = 0x8;
	cpu->seg[SEG_CS].base = 0;
	cpu->seg[SEG_CS].limit = 0xffffffff;
	cpu->seg[SEG_CS].flags = SEG_D_BIT;
	cpu->next_ip = start_addr;
	cpu->cpl = 0;
	cpu->code16 = false;
	cpu->sp_mask = 0xffffffff;
	cpu->seg[SEG_SS].sel = 0x10;
	cpu->seg[SEG_SS].base = 0;
	cpu->seg[SEG_SS].limit = 0xffffffff;
	cpu->seg[SEG_SS].flags = SEG_B_BIT;

	cpu->seg[SEG_DS] = cpu->seg[SEG_SS];
	cpu->seg[SEG_ES] = cpu->seg[SEG_SS];
}

void IRAM_ATTR cpui386_raise_irq(CPUI386 *cpu)
{
	cpu->intr = true;
}

void cpui386_set_gpr(CPUI386 *cpu, int i, u32 val)
{
	sreg32(i, val);
}

long IRAM_ATTR cpui386_get_cycle(CPUI386 *cpu)
{
	return cpu->cycle;
}

CPUI386 *cpui386_new(int gen, char *phys_mem, long phys_mem_size, CPU_CB **cb)
{
	CPUI386 *cpu = malloc(sizeof(CPUI386));
	switch (gen) {
	case 3: cpu->flags_mask = EFLAGS_MASK_386; break;
	case 4: cpu->flags_mask = EFLAGS_MASK_486; break;
	case 5: case 6: cpu->flags_mask = EFLAGS_MASK_586; break;
	default: assert(false);
	}
	cpu->gen = gen;

	cpu->tlb.size = tlb_size;
#ifdef BUILD_ESP32
	{
		extern void *pcmalloc(long size);
		size_t tlb_bytes = sizeof(struct tlb_entry) * tlb_size;
		cpu->tlb.tab = malloc(tlb_bytes);
		if (!cpu->tlb.tab)
			cpu->tlb.tab = pcmalloc(tlb_bytes);
	}
#else
	cpu->tlb.tab = malloc(sizeof(struct tlb_entry) * tlb_size);
#endif

	cpu->phys_mem = (u8 *) phys_mem;
	cpu->phys_mem_size = phys_mem_size;

	cpu->cycle = 0;

	cpu->intr = false;

	cpu->fpu = NULL;

	cpui386_reset(cpu);

	memset(&(cpu->cb), 0, sizeof(CPU_CB));
	if (cb)
		*cb = &(cpu->cb);
	return cpu;
}

void cpui386_enable_fpu(CPUI386 *cpu)
{
	if (!cpu->fpu)
		cpu->fpu = fpu_new();
}

void cpui386_delete(CPUI386 *cpu)
{
	if (cpu->fpu)
		fpu_delete(cpu->fpu);
#ifndef BUILD_ESP32
	free(cpu->tlb.tab);
#endif
	free(cpu);
}

void cpui386_get_state(CPUI386 *cpu, CPUI386_State *state)
{
	refresh_flags(cpu);
	for (int i = 0; i < 8; i++)
		state->gpr[i] = REGi(i);
	for (int i = 0; i < 8; i++)
		state->seg[i] = cpu->seg[i].sel;
	state->ip = cpu->ip;
	state->flags = cpu->flags;
	state->gdt_base = cpu->gdt.base;
	state->gdt_limit = cpu->gdt.limit;
	state->idt_base = cpu->idt.base;
	state->idt_limit = cpu->idt.limit;
	state->cr0 = cpu->cr0;
	state->cr2 = cpu->cr2;
	state->cr3 = cpu->cr3;
	state->efer = GET_LM_EFER(cpu);
}

int cpui386_get_excno(CPUI386 *cpu)
{
	return cpu->excno;
}

#if !defined(_WIN32) && !defined(__wasm__)
void cpui386_set_verbose() // for debugging
{
	verbose = true;
	freopen("/tmp/xlog", "w", stderr);
	setlinebuf(stderr);
}
#endif

static void cpu_debug(CPUI386 *cpu)
{
	static int nest;
	if (nest >= 1)
		return;
	nest++;
	bool code32 = cpu->seg[SEG_CS].flags & SEG_D_BIT;
	bool stack32 = cpu->seg[SEG_SS].flags & SEG_B_BIT;

	dolog("IP %08x|AX %08x|CX %08x|DX %08x|BX %08x|SP %08x|BP %08x|SI %08x|DI %08x|FL %08x|CS %04x|DS %04x|SS %04x|ES %04x|FS %04x|GS %04x|CR0 %08x|CR2 %08x|CR3 %08x|CPL %d|IOPL %d|CSBASE %08x/%08x|DSBASE %08x/%08x|SSBASE %08x/%08x|ESBASE %08x/%08x|GSBASE %08x/%08x %c%c\n",
		cpu->ip, REGi(0), REGi(1), REGi(2), REGi(3),
		REGi(4), REGi(5), REGi(6), REGi(7),
		cpu->flags, SEGi(SEG_CS), SEGi(SEG_DS), SEGi(SEG_SS),
		SEGi(SEG_ES), SEGi(SEG_FS), SEGi(SEG_GS),
		cpu->cr0, cpu->cr2, cpu->cr3, cpu->cpl, get_IOPL(cpu),
		cpu->seg[SEG_CS].base, cpu->seg[SEG_CS].limit,
		cpu->seg[SEG_DS].base, cpu->seg[SEG_DS].limit,
		cpu->seg[SEG_SS].base, cpu->seg[SEG_SS].limit,
		cpu->seg[SEG_ES].base, cpu->seg[SEG_ES].limit,
		cpu->seg[SEG_GS].base, cpu->seg[SEG_GS].limit,
		code32 ? 'D' : ' ', stack32 ? 'B' : ' ');
	uword cr2, excno, excerr;
	cr2 = cpu->cr2;
	excno = cpu->excno;
	excerr = cpu->excerr;
	dolog("code: ");
	for (int i = 0; i < 32; i++) {
		OptAddr res;
		if(translate8(cpu, &res, 1, SEG_CS, cpu->ip + i))
			dolog(" %02x", load8(cpu, &res));
		else
			dolog(" ??");
	}
	dolog("\n");
	dolog("stack: ");
	uword sp_mask = cpu->seg[SEG_SS].flags & SEG_B_BIT ? 0xffffffff : 0xffff;
	for (int i = 0; i < 32; i++) {
		OptAddr res;
		if(translate8(cpu, &res, 1, SEG_SS, (REGi(4) + i) & sp_mask))
			dolog(" %02x", load8(cpu, &res));
		else
			dolog(" ??");
	}
	dolog("\n");
	dolog("stkf : ");
	for (int i = 0; i < 32; i++) {
		OptAddr res;
		if(translate8(cpu, &res, 1, SEG_SS, (REGi(5) + i) & sp_mask))
			dolog(" %02x", load8(cpu, &res));
		else
			dolog(" ??");
	}
	dolog("\n");

	cpu->cr2 = cr2;
	cpu->excno = excno;
	cpu->excerr = excerr;
	nest--;
}
