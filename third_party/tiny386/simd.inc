#ifndef USE_AMD64
#define MODSIB(_) TRY(modsib(cpu, adsz16, mod, rm, &addr, &curr_seg))
#define IN_AMD64(...)
#else
#define MODSIB(_o) TRY(modsib_off(cpu, adsz32, rex, mod, rm, &addr, _o))
#define IN_AMD64(...) __VA_ARGS__
#define code16 false
#define adsz16 false
#endif
#ifdef SIMD_i386ins_def
#ifdef I2
// MMX
I2(C(0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
     0x68, 0x69, 0x6a, 0x6b, 0x74, 0x75, 0x76),       _, 0, SIMD_PqQq_VxWx)
I2(C(      0xd1, 0xd2, 0xd3, 0xd4, 0xd5,
     0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf), _, 0, SIMD_PqQq_VxWx)
I2(C(0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5,
     0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef), _, 0, SIMD_PqQq_VxWx)
I2(C(      0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6,
     0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe),       _, 0, SIMD_PqQq_VxWx)
I2(C(0x71, 0x72, 0x73), _, 0, SIMD_GRPA)
I2(C(0x6e), _, 0, SIMD_LOAD)
I2(C(0x7e), _, 0, SIMD_STORE)
I2(C(0x6c, 0x6d, 0x6f), _, 1, SIMD_PqQq_VxWx)
I2(C(0x7f, 0xe7), _, 2, SIMD_QqPq_WxVx)
I2(C(0x77), _,  0, EMMS)
I2(C(0xc4), _, 0, SIMD_PINSRW)
I2(C(0xc5), _, 0, SIMD_PEXTRW)
I2(C(0x70), _, 0, SIMD_PSHUF)
I2(C(0xd7), _, 0, SIMD_PMOVMSKB)
I2(C(0xf7), _, 0, SIMD_MASKMOVQ_DQU)

// SSE
#ifdef I386_ENABLE_SSE
I2(C(0x10,       0x12,             0x14, 0x15, 0x16, 0x28), _, 0, SIMD_VxWx)
I2(C(0x2e, 0x2f), _, 0, SIMD_VxWx_S)
I2(C(      0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
     0x58, 0x59,             0x5c, 0x5d, 0x5e, 0x5f), _, 0, SIMD_VxWx)
I2(C(0x11, 0x13, 0x17, 0x29, 0x2b), _, 0, SIMD_WxVx)
I2(C(0xc2, 0xc6), _, 0, SIMD_VxWxIb)
I2(C(0x2a), _, 0, SIMD_CVTI2S_I2D)
I2(C(0x2c, 0x2d), _, 0, SIMD_CVTS2I_D2I)
I2(C(0x50), _, 0, SIMD_MOVMSKP)
I2(C(0x18), _, 0, SIMD_PREFETCH)
I2(C(0xae), _, 0, SIMD_GRP15)
#endif /* I386_ENABLE_SSE */

#ifdef I386_ENABLE_SSE2
I2(C(0xc3), _, 0, MOVNTI)
I2(C(0xd6), _, 0, MOVQ_DQ)
I2(C(0x5a), _, 0, CVTS2D_D2S)
I2(C(0x5b), _, 0, CVTDQ_PS)
I2(C(0xe6), _, 0, CVTDQ_PD)
#endif /* I386_ENABLE_SSE2 */

#ifdef I386_ENABLE_SSE3
I2(C(0xd0, 0x7c, 0x7d), _, 0, SIMD_VxWx_P)
I2(C(0xf0), _, 0, LDDQU)
#endif /* I386_ENABLE_SSE3 */
#endif /* I2 */

#ifdef I386_ENABLE_SSSE3
#ifdef I3_38
I3_38(C(0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0a, 0x0b, 0x1c, 0x1d, 0x1e), _, 0, SIMD3_PqQq_VxWx)
#endif
#ifdef I3_3a
I3_3a(C(0x0f), _, 0, SIMD_PALIGNR)
#endif
#endif /* I386_ENABLE_SSSE3 */

#endif /* SIMD_i386ins_def */

#if defined(SIMD_i386_c) || defined(SIMD_fpu_c)
// MMX
bool fpu_mmx_rr(FPU *fpu, void *cpu, int op, int dst, int src);
bool fpu_mmx_rm(FPU *fpu, void *cpu, int op, int dst, int seg, uword addr);
bool fpu_mmx_ri(FPU *fpu, void *cpu, int op, int func, int dst, int imm);
bool fpu_mmx_mr(FPU *fpu, void *cpu, int op, int src, int seg, uword addr);
uint64_t fpu_mmx_get(FPU *fpu, int i);
void fpu_mmx_set(FPU *fpu, int i, uint64_t v);

// SSE
#ifdef I386_ENABLE_SSE
void fpu_xmmsf32_set_s32(FPU *fpu, int dst, uint32_t v);
void fpu_xmmsf32_set_s64(FPU *fpu, int dst, uint64_t v);
void fpu_xmmpf32_set_s32(FPU *fpu, int dst, uint64_t v);

float *fpu_xmm_get_f32(FPU *fpu, int i);
uint32_t *fpu_xmm_get_u32(FPU *fpu, int i);

void fpu_xmmsf64_set_s32(FPU *fpu, int dst, uint32_t v);
void fpu_xmmsf64_set_s64(FPU *fpu, int dst, uint64_t v);
void fpu_xmmpf64_set_s32(FPU *fpu, int dst, uint64_t v);
double *fpu_xmm_get_f64(FPU *fpu, int i);
uint64_t *fpu_xmm_get_u64(FPU *fpu, int i);
uint16_t *fpu_xmm_get_u16(FPU *fpu, int i);

bool fpu_xmmsf32_rr(FPU *fpu, void *cpu, int op, int dst, int src);
bool fpu_xmmsf32_rm(FPU *fpu, void *cpu, int op, int dst, int seg, uword addr);
bool fpu_xmmsf32_rri(FPU *fpu, void *cpu, int op, int func, int dst, int imm);
bool fpu_xmmsf32_mr(FPU *fpu, void *cpu, int op, int src, int seg, uword addr);
bool fpu_xmmpf32_rr(FPU *fpu, void *cpu, int op, int dst, int src);
bool fpu_xmmpf32_rm(FPU *fpu, void *cpu, int op, int dst, int seg, uword addr);
bool fpu_xmmpf32_rri(FPU *fpu, void *cpu, int op, int func, int dst, int imm);
bool fpu_xmmpf32_mr(FPU *fpu, void *cpu, int op, int src, int seg, uword addr);
bool fpu_xmm_grp15(FPU *fpu, void *cpu, int op, int seg, uword addr);
#endif /* I386_ENABLE_SSE */

#ifdef I386_ENABLE_SSE2
bool fpu_xmmsf64_rr(FPU *fpu, void *cpu, int op, int dst, int src);
bool fpu_xmmsf64_rm(FPU *fpu, void *cpu, int op, int dst, int seg, uword addr);
bool fpu_xmmsf64_rri(FPU *fpu, void *cpu, int op, int func, int dst, int imm);
bool fpu_xmmsf64_mr(FPU *fpu, void *cpu, int op, int src, int seg, uword addr);
bool fpu_xmmpf64_rr(FPU *fpu, void *cpu, int op, int dst, int src);
bool fpu_xmmpf64_rm(FPU *fpu, void *cpu, int op, int dst, int seg, uword addr);
bool fpu_xmmpf64_rri(FPU *fpu, void *cpu, int op, int func, int dst, int imm);
bool fpu_xmmpf64_mr(FPU *fpu, void *cpu, int op, int src, int seg, uword addr);

bool fpu_xmm_rr(FPU *fpu, void *cpu, int op, int dst, int src);
bool fpu_xmm_rm(FPU *fpu, void *cpu, int op, int dst, int seg, uword addr);
bool fpu_xmm_ri(FPU *fpu, void *cpu, int op, int func, int dst, int imm);
bool fpu_xmm_mr(FPU *fpu, void *cpu, int op, int src, int seg, uword addr);
#endif /* I386_ENABLE_SSE2 */

#endif /* defined(SIMD_i386_c) || defined(SIMD_fpu_c) */

#ifdef SIMD_i386_c
// MMX
#define SIMD_PqQq_VxWx() \
	if (cpu->cr0 & 0xc) THROW0(EX_NM); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	IN_AMD64(if (rex & REX_R) reg |= 8;) \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (rep && b1 != 0x6f) THROW0(EX_UD); \
	if (mod == 3) { \
		IN_AMD64(if (rex & REX_B) rm |= 8;) \
		if (opsz16 == code16 && !rep) /* NP */ { \
			TRY(fpu_mmx_rr(cpu->fpu, cpu, b1, reg & 7, rm & 7)); \
		} else /* 66 */ { \
			TRY_SSE2(fpu_xmm_rr(cpu->fpu, cpu, b1, reg, rm)); \
		} \
	} else { \
		MODSIB(0); \
		if (opsz16 == code16 && !rep) /* NP */ { \
			TRY(fpu_mmx_rm(cpu->fpu, cpu, b1, reg & 7, curr_seg, addr)); \
		} else /* 66 */ { \
			TRY_SSE2(fpu_xmm_rm(cpu->fpu, cpu, b1, reg, curr_seg, addr)); \
		} \
	}

#define SIMD_QqPq_WxVx() \
	if (cpu->cr0 & 0xc) THROW0(EX_NM); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	IN_AMD64(if (rex & REX_R) reg |= 8;) \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (rep && b1 != 0x7f) THROW0(EX_UD); \
	if (mod == 3) { \
		IN_AMD64(if (rex & REX_B) rm |= 8;) \
		if (opsz16 == code16 && !rep) /* NP */ { \
			TRY(fpu_mmx_rr(cpu->fpu, cpu, b1, rm & 7, reg & 7)); \
		} else /* 66 */ { \
			TRY_SSE2(fpu_xmm_rr(cpu->fpu, cpu, b1, rm, reg)); \
		} \
	} else { \
		MODSIB(0); \
		if (opsz16 == code16 && !rep) /* NP */ { \
			TRY(fpu_mmx_mr(cpu->fpu, cpu, b1, reg & 7, curr_seg, addr)); \
		} else /* 66 */ { \
			TRY_SSE2(fpu_xmm_mr(cpu->fpu, cpu, b1, reg, curr_seg, addr)); \
		} \
	}

#define SIMD_GRPA() \
	if (cpu->cr0 & 0xc) THROW0(EX_NM); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	IN_AMD64(if (rex & REX_R) reg |= 8;) \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	u8 imm8; \
	if (rep) THROW0(EX_UD); \
	if (mod == 3) { \
		IN_AMD64(if (rex & REX_B) rm |= 8;) \
		TRY(fetch8(cpu, &imm8)); \
		if (opsz16 == code16) /* NP */ { \
			TRY(fpu_mmx_ri(cpu->fpu, cpu, b1, reg & 7, rm & 7, imm8)); \
		} else /* 66 */ { \
			TRY_SSE2(fpu_xmm_ri(cpu->fpu, cpu, b1, reg, rm, imm8)); \
		} \
	} else { \
		THROW0(EX_UD); \
	}

#define SIMD_LOAD() \
	if (cpu->cr0 & 0xc) THROW0(EX_NM); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	IN_AMD64(if (rex & REX_R) reg |= 8;) \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (rep) THROW0(EX_UD); \
	if (mod == 3) { \
		IN_AMD64(if (rex & REX_B) rm |= 8;) \
		if (opsz16 == code16) /* NP */ { \
			uword v = REGi(rm); \
			IN_AMD64(if (!(rex & REX_W)) v &= 0xffffffff;) \
			fpu_mmx_set(cpu->fpu, reg & 7, v); \
		} else /* 66 */ { \
			uint32_t *x; \
			(void) x; \
			TRY_SSE2(x = fpu_xmm_get_u32(cpu->fpu, reg), \
				 x[0] = REGi(rm), x[1] = 0, \
				 x[2] = 0, x[3] = 0, 1); \
			IN_AMD64(if (rex & REX_W) x[1] = REGi(rm) >> 32;) \
		} \
	} else { \
		uint32_t lo; \
		MODSIB(0); \
		TRY(cpu_load32(cpu, curr_seg, addr, &lo)); \
		if (opsz16 == code16) /* NP */ { \
			fpu_mmx_set(cpu->fpu, reg & 7, lo); \
		} else /* 66 */ { \
			uint32_t *x; \
			(void) x; \
			TRY_SSE2(x = fpu_xmm_get_u32(cpu->fpu, reg), \
				 x[0] = lo, x[1] = 0, \
				 x[2] = 0, x[3] = 0, 1); \
		} \
		bool rexw = false; \
		IN_AMD64(if (rex & REX_W) rexw = true;) \
		if (rexw) { \
		uint32_t hi; \
		TRY(cpu_load32(cpu, curr_seg, addr + 4, &hi)); \
		if (opsz16 == code16) /* NP */ { \
			fpu_mmx_set(cpu->fpu, reg & 7, ((uint64_t) hi << 32) | lo); \
		} else /* 66 */ { \
			uint32_t *x; \
			(void) x; \
			TRY_SSE2(x = fpu_xmm_get_u32(cpu->fpu, reg), \
				 x[0] = lo, x[1] = hi, \
				 x[2] = 0, x[3] = 0, 1); \
		} \
		} \
	}

#define SIMD_STORE() \
	if (cpu->cr0 & 0xc) THROW0(EX_NM); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	IN_AMD64(if (rex & REX_R) reg |= 8;) \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (mod == 3) { \
		IN_AMD64(if (rex & REX_B) rm |= 8;) \
		if (rep == 0) { \
			if (opsz16 == code16) /* NP */ { \
				REGi(rm) = fpu_mmx_get(cpu->fpu, reg & 7); \
				IN_AMD64(if (!(rex & REX_W)) REGi(rm) &= 0xffffffff;) \
			} else /* 66 */ { \
				IN_AMD64(if ((rex & REX_W)) REGi(rm) = fpu_xmm_get_u64(cpu->fpu, reg)[0]; else) \
				{ TRY_SSE2(REGi(rm) = fpu_xmm_get_u32(cpu->fpu, reg)[0], 1); } \
			} \
		} else if (rep == 1) /* F3 */ { \
			/* MOVQ xmm1, xmm2 */ \
			TRY_SSE2(fpu_xmm_get_u64(cpu->fpu, reg)[0] = fpu_xmm_get_u64(cpu->fpu, rm)[0], 1); \
			TRY_SSE2(fpu_xmm_get_u64(cpu->fpu, reg)[1] = 0, 1); \
		} else /* F2 */ { \
			THROW0(EX_NM); \
		} \
	} else { \
		MODSIB(0); \
		if (rep == 0) { \
			if (opsz16 == code16) /* NP */ { \
				uint64_t v = fpu_mmx_get(cpu->fpu, reg & 7); \
				TRY(cpu_store32(cpu, curr_seg, addr, v)); \
				IN_AMD64(if (rex & REX_W) TRY(cpu_store32(cpu, curr_seg, addr + 4, v >> 32));) \
			} else /* 66 */ { \
				TRY_SSE2(cpu_store32(cpu, curr_seg, addr, \
					 fpu_xmm_get_u32(cpu->fpu, reg)[0])); \
				IN_AMD64(if (rex & REX_W) cpu_store32(cpu, curr_seg, addr + 4, \
					 fpu_xmm_get_u32(cpu->fpu, reg)[1]);) \
			} \
		} else if (rep == 1) /* F3 */ { \
			/* MOVQ xmm1, m64 */ \
			uint32_t lo, hi, *x; \
			(void) lo, (void) hi, (void) x; \
			TRY_SSE2(cpu_load32(cpu, curr_seg, addr, &lo)); \
			TRY_SSE2(cpu_load32(cpu, curr_seg, addr + 4, &hi)); \
			TRY_SSE2(x = fpu_xmm_get_u32(cpu->fpu, reg), \
				 x[0] = lo, x[1] = hi, x[2] = 0, x[3] = 0, 1); \
		} else /* F2 */ { \
			THROW0(EX_NM); \
		} \
	}

#define EMMS()

// actually SSE
#define SIMD_PINSRW() \
	if (cpu->cr0 & 0xc) THROW0(EX_NM); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	IN_AMD64(if (rex & REX_R) reg |= 8;) \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	u8 imm8; \
	if (rep) THROW0(EX_UD); \
	if (opsz16 == code16) /* NP */ { \
		if (mod == 3) { \
			IN_AMD64(if (rex & REX_B) rm |= 8;) \
			TRY(fetch8(cpu, &imm8)); \
			int sh = (imm8 & 3) * 16; \
			uint64_t mask = 0xffffull << sh; \
			uint64_t v = (((uint64_t) REGi(rm)) << sh) & mask; \
			uint64_t w = fpu_mmx_get(cpu->fpu, reg & 7) & ~mask; \
			fpu_mmx_set(cpu->fpu, reg & 7, v | w); \
		} else { \
			MODSIB(1); \
			TRY(fetch8(cpu, &imm8)); \
			TRY(translate16(cpu, &meml, 1, curr_seg, addr)); \
			int sh = (imm8 & 3) * 16; \
			uint64_t mask = 0xffffull << sh; \
			uint64_t v = (((uint64_t) laddr16(&meml)) << sh) & mask; \
			uint64_t w = fpu_mmx_get(cpu->fpu, reg & 7) & ~mask; \
			fpu_mmx_set(cpu->fpu, reg & 7, v | w); \
		} \
	} else /* 66 */ { \
		uint16_t *w; \
		(void) w; \
		TRY_SSE2(w = fpu_xmm_get_u16(cpu->fpu, reg), 1); \
		if (mod == 3) { \
			IN_AMD64(if (rex & REX_B) rm |= 8;) \
			TRY(fetch8(cpu, &imm8)); \
			int sh = (imm8 & 7); \
			w[sh] = REGi(rm); \
		} else { \
			MODSIB(1); \
			TRY(fetch8(cpu, &imm8)); \
			TRY(translate16(cpu, &meml, 1, curr_seg, addr)); \
			int sh = (imm8 & 7); \
			w[sh] = laddr16(&meml); \
		} \
	}

#define SIMD_PEXTRW() \
	if (cpu->cr0 & 0xc) THROW0(EX_NM); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	IN_AMD64(if (rex & REX_R) reg |= 8;) \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	u8 imm8; \
	if (rep) THROW0(EX_UD); \
	if (mod == 3) { \
		IN_AMD64(if (rex & REX_B) rm |= 8;) \
		TRY(fetch8(cpu, &imm8)); \
		if (opsz16 == code16) /* NP */ { \
			uint64_t v = fpu_mmx_get(cpu->fpu, rm & 7); \
			REGi(reg) = (v >> ((imm8 & 3) * 16)) & 0xffff; \
		} else /* 66 */ { \
			uint16_t *w; \
			(void) w; \
			TRY_SSE2(w = fpu_xmm_get_u16(cpu->fpu, rm), 1); \
			REGi(reg) = w[imm8 & 7]; \
		} \
	} else { \
		THROW0(EX_UD); \
	}

#define SIMD_PSHUF() \
	if (rep == 0) { \
		if (opsz16 == code16) /* NP */ { \
			SIMD_PSHUFW(); \
		} else /* 66 */ { \
			SSE2_PSHUFD(); \
		} \
	} else if (rep == 1) /* F3 */ { \
		SSE2_PSHUFHW(); \
	} else /* F2 */ { \
		SSE2_PSHUFLW(); \
	}

#define SIMD_PSHUFW() \
	if (cpu->cr0 & 0xc) THROW0(EX_NM); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	IN_AMD64(if (rex & REX_R) reg |= 8;) \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	u8 imm8; \
	if (mod == 3) { \
		IN_AMD64(if (rex & REX_B) rm |= 8;) \
		TRY(fetch8(cpu, &imm8)); \
		uint64_t s = fpu_mmx_get(cpu->fpu, rm & 7); \
		uint64_t w = 0; \
		w |= (s >> (((imm8 >> 0) & 3) * 16) & 0xffff) << 0; \
		w |= (s >> (((imm8 >> 2) & 3) * 16) & 0xffff) << 16; \
		w |= (s >> (((imm8 >> 4) & 3) * 16) & 0xffff) << 32; \
		w |= (s >> (((imm8 >> 6) & 3) * 16) & 0xffff) << 48; \
		fpu_mmx_set(cpu->fpu, reg & 7, w); \
	} else { \
		uint64_t s; \
		MODSIB(1); \
		TRY(fetch8(cpu, &imm8)); \
		TRY(translate32(cpu, &meml, 1, curr_seg, addr)); \
		s = laddr32(&meml); \
		TRY(translate32(cpu, &meml, 1, curr_seg, addr + 4)); \
		s |= ((uint64_t) laddr32(&meml)) << 32; \
		uint64_t w = 0; \
		w |= (s >> (((imm8 >> 0) & 3) * 16) & 0xffff) << 0; \
		w |= (s >> (((imm8 >> 2) & 3) * 16) & 0xffff) << 16; \
		w |= (s >> (((imm8 >> 4) & 3) * 16) & 0xffff) << 32; \
		w |= (s >> (((imm8 >> 6) & 3) * 16) & 0xffff) << 48; \
		fpu_mmx_set(cpu->fpu, reg & 7, w); \
	}

#define SIMD_PMOVMSKB() \
	if (cpu->cr0 & 0xc) THROW0(EX_NM); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	IN_AMD64(if (rex & REX_R) reg |= 8;) \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (rep) THROW0(EX_UD); \
	if (mod == 3) { \
		IN_AMD64(if (rex & REX_B) rm |= 8;) \
		if (opsz16 == code16) /* NP */ { \
			uint64_t v = fpu_mmx_get(cpu->fpu, rm & 7); \
			REGi(reg) = \
				((v >> 7) & 1) | ((v >> 14) & 2) | \
				((v >> 21) & 4) |((v >> 28) & 8) | \
				((v >> 35) & 16) | ((v >> 42) & 32) | \
				((v >> 49) & 64) |((v >> 56) & 128); \
		} else /* 66 */ { \
			uint64_t *u; \
			(void) u; \
			TRY_SSE2(u = fpu_xmm_get_u64(cpu->fpu, rm), 1); \
			uint64_t v = u[1]; \
			REGi(reg) = \
				((v >> 7) & 1) | ((v >> 14) & 2) | \
				((v >> 21) & 4) |((v >> 28) & 8) | \
				((v >> 35) & 16) | ((v >> 42) & 32) | \
				((v >> 49) & 64) |((v >> 56) & 128); \
			REGi(reg) <<= 8; \
			v = u[0]; \
			REGi(reg) |= \
				((v >> 7) & 1) | ((v >> 14) & 2) | \
				((v >> 21) & 4) |((v >> 28) & 8) | \
				((v >> 35) & 16) | ((v >> 42) & 32) | \
				((v >> 49) & 64) |((v >> 56) & 128); \
		} \
	} else { \
		THROW0(EX_UD); \
	}

#define SIMD_MASKMOVQ_DQU() \
	if (cpu->cr0 & 0xc) THROW0(EX_NM); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	IN_AMD64(if (rex & REX_R) reg |= 8;) \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (rep) THROW0(EX_UD); \
	if (mod == 3) { \
		IN_AMD64(if (rex & REX_B) rm |= 8;) \
		if (opsz16 == code16) /* NP */ { \
			/* MASKMOVQ */ \
			uint64_t mm1 = fpu_mmx_get(cpu->fpu, reg & 7); \
			uint64_t mm2 = fpu_mmx_get(cpu->fpu, rm & 7); \
			for (int i = 0; i < 8; i++) { \
				if ((mm2 >> 7) & 1) { \
					uword addr = REGi(7) + i; \
					if (adsz16) addr = addr & 0xffff; \
					TRY(cpu_store8(cpu, SEG_DS, addr, mm1)); \
				} \
				mm1 >>= 8; \
				mm2 >>= 8; \
			} \
		} else /* 66 */ { \
			/* MASKMOVDQU */ \
			SSE2_MASKMOVDQU(); \
		} \
	} else { \
		THROW0(EX_UD); \
	}

// SSE
#ifdef I386_ENABLE_SSE
#define SIMD_VxWx() \
	if (cpu->cr0 & 0xc) THROW0(EX_NM); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	IN_AMD64(if (rex & REX_R) reg |= 8;) \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (mod == 3) { \
		IN_AMD64(if (rex & REX_B) rm |= 8;) \
		if (rep == 0) { \
			if (opsz16 == code16) /* NP */ { \
				TRY(fpu_xmmpf32_rr(cpu->fpu, cpu, b1, reg, rm)); \
			} else /* 66 */ { \
				TRY_SSE2(fpu_xmmpf64_rr(cpu->fpu, cpu, b1, reg, rm)); \
			} \
		} else if (rep == 1) /* F3 */ { \
			TRY(fpu_xmmsf32_rr(cpu->fpu, cpu, b1, reg, rm)); \
		} else /* F2 */ { \
			TRY_SSE2(fpu_xmmsf64_rr(cpu->fpu, cpu, b1, reg, rm)); \
		} \
	} else { \
		MODSIB(0); \
		if (rep == 0) { \
			if (opsz16 == code16) /* NP */ { \
				TRY(fpu_xmmpf32_rm(cpu->fpu, cpu, b1, reg, curr_seg, addr)); \
			} else /* 66 */ { \
				TRY_SSE2(fpu_xmmpf64_rm(cpu->fpu, cpu, b1, reg, curr_seg, addr)); \
			} \
		} else if (rep == 1) /* F3 */ { \
			TRY(fpu_xmmsf32_rm(cpu->fpu, cpu, b1, reg, curr_seg, addr)); \
		} else /* F2 */ { \
			TRY_SSE2(fpu_xmmsf64_rm(cpu->fpu, cpu, b1, reg, curr_seg, addr)); \
		} \
	}

#define SIMD_VxWx_S() \
	if (cpu->cr0 & 0xc) THROW0(EX_NM); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	IN_AMD64(if (rex & REX_R) reg |= 8;) \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (mod == 3) { \
		IN_AMD64(if (rex & REX_B) rm |= 8;) \
		if (rep == 0) { \
			if (opsz16 == code16) /* NP */ { \
				TRY(fpu_xmmsf32_rr(cpu->fpu, cpu, b1, reg, rm)); \
			} else /* 66 */ { \
				TRY_SSE2(fpu_xmmsf64_rr(cpu->fpu, cpu, b1, reg, rm)); \
			} \
		} else { \
			THROW0(EX_UD); \
		} \
	} else { \
		MODSIB(0); \
		if (rep == 0) { \
			if (opsz16 == code16) /* NP */ { \
				TRY(fpu_xmmsf32_rm(cpu->fpu, cpu, b1, reg, curr_seg, addr)); \
			} else /* 66 */ { \
				TRY_SSE2(fpu_xmmsf64_rm(cpu->fpu, cpu, b1, reg, curr_seg, addr)); \
			} \
		} else { \
			THROW0(EX_UD); \
		} \
	}

#define SIMD_VxWx_P() \
	if (cpu->cr0 & 0xc) THROW0(EX_NM); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	IN_AMD64(if (rex & REX_R) reg |= 8;) \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (mod == 3) { \
		IN_AMD64(if (rex & REX_B) rm |= 8;) \
		if (rep == 0) { \
			if (opsz16 == code16) /* NP */ { \
				THROW0(EX_UD); \
			} else /* 66 */ { \
				TRY_SSE2(fpu_xmmpf64_rr(cpu->fpu, cpu, b1, reg, rm)); \
			} \
		} else if (rep == 1) /* F3 */ { \
			THROW0(EX_UD); \
		} else /* F2 */ { \
			TRY(fpu_xmmpf32_rr(cpu->fpu, cpu, b1, reg, rm)); \
		} \
	} else { \
		MODSIB(0); \
		if (rep == 0) { \
			if (opsz16 == code16) /* NP */ { \
				THROW0(EX_UD); \
			} else /* 66 */ { \
				TRY_SSE2(fpu_xmmpf64_rm(cpu->fpu, cpu, b1, reg, curr_seg, addr)); \
			} \
		} else if (rep == 1) /* F3 */ { \
			THROW0(EX_UD); \
		} else /* F2 */ { \
			TRY(fpu_xmmpf32_rm(cpu->fpu, cpu, b1, reg, curr_seg, addr)); \
		} \
	}

#define SIMD_WxVx() \
	if (cpu->cr0 & 0xc) THROW0(EX_NM); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	IN_AMD64(if (rex & REX_R) reg |= 8;) \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (mod == 3) { \
		IN_AMD64(if (rex & REX_B) rm |= 8;) \
		if (rep == 0) { \
			if (opsz16 == code16) /* NP */ { \
				TRY(fpu_xmmpf32_rr(cpu->fpu, cpu, b1, rm, reg)); \
			} else /* 66 */ { \
				TRY_SSE2(fpu_xmmpf64_rr(cpu->fpu, cpu, b1, rm, reg)); \
			} \
		} else if (rep == 1) /* F3 */ { \
			TRY(fpu_xmmsf32_rr(cpu->fpu, cpu, b1, rm, reg)); \
		} else /* F2 */ { \
			TRY_SSE2(fpu_xmmsf64_rr(cpu->fpu, cpu, b1, rm, reg)); \
		} \
	} else { \
		MODSIB(0); \
		if (rep == 0) { \
			if (opsz16 == code16) /* NP */ { \
				TRY(fpu_xmmpf32_mr(cpu->fpu, cpu, b1, reg, curr_seg, addr)); \
			} else /* 66 */ { \
				TRY_SSE2(fpu_xmmpf64_mr(cpu->fpu, cpu, b1, reg, curr_seg, addr)); \
			} \
		} else if (rep == 1) /* F3 */ { \
			TRY(fpu_xmmsf32_mr(cpu->fpu, cpu, b1, reg, curr_seg, addr)); \
		} else /* F2 */ { \
			TRY_SSE2(fpu_xmmsf64_mr(cpu->fpu, cpu, b1, reg, curr_seg, addr)); \
		} \
	}

#define SIMD_VxWxIb() \
	if (cpu->cr0 & 0xc) THROW0(EX_NM); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	IN_AMD64(if (rex & REX_R) reg |= 8;) \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	u8 imm8; \
	if (mod == 3) { \
		IN_AMD64(if (rex & REX_B) rm |= 8;) \
		TRY(fetch8(cpu, &imm8)); \
		if (rep == 0) { \
			if (opsz16 == code16) /* NP */ { \
				TRY(fpu_xmmpf32_rri(cpu->fpu, cpu, b1, reg, rm, imm8)); \
			} else /* 66 */ { \
				TRY_SSE2(fpu_xmmpf64_rri(cpu->fpu, cpu, b1, reg, rm, imm8)); \
			} \
		} else if (rep == 1) /* F3 */ { \
			TRY(fpu_xmmsf32_rri(cpu->fpu, cpu, b1, reg, rm, imm8)); \
		} else /* F2 */ { \
			TRY_SSE2(fpu_xmmsf64_rri(cpu->fpu, cpu, b1, reg, rm, imm8)); \
		} \
	} else { \
		THROW0(EX_UD); \
	}

#define SIMD_CVTI2S_I2D() \
	if (cpu->cr0 & 0xc) THROW0(EX_NM); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	IN_AMD64(if (rex & REX_R) reg |= 8;) \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (mod == 3) { \
		IN_AMD64(if (rex & REX_B) rm |= 8;) \
		if (rep == 0) { \
			if (opsz16 == code16) /* NP */ { \
				uint64_t v = fpu_mmx_get(cpu->fpu, rm & 7); \
				fpu_xmmpf32_set_s32(cpu->fpu, reg, v); \
			} else /* 66 */ { \
				/* CVTPI2PD (SSE2) */ \
				uint64_t v = fpu_mmx_get(cpu->fpu, rm & 7); \
				fpu_xmmpf64_set_s32(cpu->fpu, reg, v); \
			} \
		} else if (rep == 1) /* F3 */ { \
			IN_AMD64(if (rex & REX_W) fpu_xmmsf32_set_s64(cpu->fpu, reg, REGi(rm)); else) \
			fpu_xmmsf32_set_s32(cpu->fpu, reg, REGi(rm)); \
		} else { \
			/* CVTSI2SD (SSE2) */ \
			IN_AMD64(if (rex & REX_W) fpu_xmmsf64_set_s64(cpu->fpu, reg, REGi(rm)); else) \
			fpu_xmmsf64_set_s32(cpu->fpu, reg, REGi(rm)); \
		} \
	} else { \
		MODSIB(0); \
		if (rep == 0) { \
			if (opsz16 == code16) /* NP */ { \
				uint32_t lo, hi; \
				TRY(cpu_load32(cpu, curr_seg, addr, &lo)); \
				TRY(cpu_load32(cpu, curr_seg, addr + 4, &hi)); \
				uint64_t v = (((uint64_t) hi) << 32) | lo; \
				fpu_xmmpf32_set_s32(cpu->fpu, reg, v); \
			} else /* 66 */ { \
				/* CVTPI2PD (SSE2) */ \
				uint32_t lo, hi; \
				TRY(cpu_load32(cpu, curr_seg, addr, &lo)); \
				TRY(cpu_load32(cpu, curr_seg, addr + 4, &hi)); \
				uint64_t v = (((uint64_t) hi) << 32) | lo; \
				fpu_xmmpf64_set_s32(cpu->fpu, reg, v); \
			} \
		} else if (rep == 1) /* F3 */ { \
			uint32_t v; \
			TRY(cpu_load32(cpu, curr_seg, addr, &v)); \
			bool rexw = false; \
			IN_AMD64(if (rex & REX_W) rexw = true;) \
			if (!rexw) { \
				fpu_xmmsf32_set_s32(cpu->fpu, reg, v); \
			} else { \
				uint32_t u; \
				TRY(cpu_load32(cpu, curr_seg, addr + 4, &u)); \
				fpu_xmmsf32_set_s64(cpu->fpu, reg, ((uint64_t) u << 32) | v); \
			} \
		} else { \
			/* CVTSI2SD (SSE2) */ \
			uint32_t v; \
			TRY(cpu_load32(cpu, curr_seg, addr, &v)); \
			bool rexw = false; \
			IN_AMD64(if (rex & REX_W) rexw = true;) \
			if (!rexw) { \
				fpu_xmmsf64_set_s32(cpu->fpu, reg, v); \
			} else { \
				uint32_t u; \
				TRY(cpu_load32(cpu, curr_seg, addr + 4, &u)); \
				fpu_xmmsf64_set_s64(cpu->fpu, reg, ((uint64_t) u << 32) | v); \
			} \
		} \
	}

#include <math.h>
static int32_t trunci32f(float val)
{
	return val < 2147483648.0 && val >= -2147483648.0 ? (int32_t) val : 0x80000000;
}

static int32_t trunci32(double val)
{
	return val < 2147483648.0 && val >= -2147483648.0 ? (int32_t) val : 0x80000000;
}

#ifdef USE_AMD64
static int64_t trunci64f(float val)
{
	return val < 9223372036854775808.0 && val >= -9223372036854775808.0 ? (int64_t) val : 0x8000000000000000ll;
}

static int64_t trunci64(double val)
{
	return val < 9223372036854775808.0 && val >= -9223372036854775808.0 ? (int64_t) val : 0x8000000000000000ll;
}
#endif

#define SIMD_CVTS2I_D2I() \
	if (cpu->cr0 & 0xc) THROW0(EX_NM); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	IN_AMD64(if (rex & REX_R) reg |= 8;) \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (mod == 3) { \
		IN_AMD64(if (rex & REX_B) rm |= 8;) \
		if (rep == 0) { \
			if (opsz16 == code16) /* NP */ { \
				float *vv = fpu_xmm_get_f32(cpu->fpu, rm); \
				float d[2]; \
				d[0] = vv[0]; \
				d[1] = vv[1]; \
				/* TODO: respect mxcsr */ \
				if (b1 == 0x2d) { \
					d[0] = nearbyintf(d[0]); \
					d[1] = nearbyintf(d[1]); \
				} \
				uint32_t lo = trunci32f(d[0]); \
				uint32_t hi = trunci32f(d[1]); \
				uint64_t v = (((uint64_t) hi) << 32) | lo; \
				fpu_mmx_set(cpu->fpu, reg & 7, v); \
			} else /* 66 */ { \
				/* CVTPD2PI (SSE2) */ \
				double *vv = fpu_xmm_get_f64(cpu->fpu, rm); \
				double d[2]; \
				d[0] = vv[0]; \
				d[1] = vv[1]; \
				/* TODO: respect mxcsr */ \
				if (b1 == 0x2d) { \
					d[0] = nearbyint(d[0]); \
					d[1] = nearbyint(d[1]); \
				} \
				uint32_t lo = trunci32(d[0]); \
				uint32_t hi = trunci32(d[1]); \
				uint64_t v = (((uint64_t) hi) << 32) | lo; \
				fpu_mmx_set(cpu->fpu, reg & 7, v); \
			} \
		} else if (rep == 1) /* F3 */ { \
			float v = fpu_xmm_get_f32(cpu->fpu, rm)[0]; \
			/* TODO: respect mxcsr */ \
			if (b1 == 0x2d) { \
				v = nearbyintf(v); \
			} \
			IN_AMD64(if (rex & REX_W) REGi(reg) = trunci64f(v); else) \
			REGi(reg) = (uint32_t) trunci32f(v); \
		} else /* F2 */ { \
			/* CVTSD2SI (SSE2) */ \
			double v = fpu_xmm_get_f64(cpu->fpu, rm)[0]; \
			/* TODO: respect mxcsr */ \
			if (b1 == 0x2d) { \
				v = nearbyint(v); \
			} \
			IN_AMD64(if (rex & REX_W) REGi(reg) = trunci64(v); else) \
			REGi(reg) = (uint32_t) trunci32(v); \
		} \
	} else { \
		MODSIB(0); \
		if (rep == 0) { \
			if (opsz16 == code16) /* NP */ { \
				union {float f; uint32_t v;} u[2]; \
				TRY(cpu_load32(cpu, curr_seg, addr, &(u[0].v))); \
				TRY(cpu_load32(cpu, curr_seg, addr + 4, &(u[1].v))); \
				/* TODO: respect mxcsr */ \
				if (b1 == 0x2d) { \
					u[0].f = nearbyintf(u[0].f); \
					u[1].f = nearbyintf(u[1].f); \
				} \
				uint32_t lo = trunci32f(u[0].f); \
				uint32_t hi = trunci32f(u[1].f); \
				uint64_t v = (((uint64_t) hi) << 32) | lo; \
				fpu_mmx_set(cpu->fpu, reg & 7, v); \
			} else /* 66 */ { \
				/* CVTPD2PI (SSE2) */ \
				union {double f; uint32_t v[2];} u[2]; \
				TRY(cpu_load32(cpu, curr_seg, addr, &(u[0].v[0]))); \
				TRY(cpu_load32(cpu, curr_seg, addr + 4, &(u[0].v[1]))); \
				TRY(cpu_load32(cpu, curr_seg, addr + 8, &(u[1].v[0]))); \
				TRY(cpu_load32(cpu, curr_seg, addr + 12, &(u[1].v[1]))); \
				/* TODO: respect mxcsr */ \
				if (b1 == 0x2d) { \
					u[0].f = nearbyint(u[0].f); \
					u[1].f = nearbyint(u[1].f); \
				} \
				uint32_t lo = trunci32(u[0].f); \
				uint32_t hi = trunci32(u[1].f); \
				uint64_t v = (((uint64_t) hi) << 32) | lo; \
				fpu_mmx_set(cpu->fpu, reg & 7, v); \
			} \
		} else if (rep == 1) /* F3 */ { \
			union {float f; uint32_t v;} u; \
			TRY(cpu_load32(cpu, curr_seg, addr, &(u.v))); \
			/* TODO: respect mxcsr */ \
			if (b1 == 0x2d) { \
				u.f = nearbyintf(u.f); \
			} \
			IN_AMD64(if (rex & REX_W) REGi(reg) = trunci64f(u.f); else) \
			REGi(reg) = (uint32_t) trunci32f(u.f); \
		} else /* F2 */ { \
			/* CVTSD2SI (SSE2) */ \
			union {double f; uint32_t v[2];} u; \
			TRY(cpu_load32(cpu, curr_seg, addr, &(u.v[0]))); \
			TRY(cpu_load32(cpu, curr_seg, addr + 4, &(u.v[1]))); \
			/* TODO: respect mxcsr */ \
			if (b1 == 0x2d) { \
				u.f = nearbyint(u.f); \
			} \
			IN_AMD64(if (rex & REX_W) REGi(reg) = trunci64(u.f); else) \
			REGi(reg) = (uint32_t) trunci32(u.f); \
		} \
	}

#define SIMD_MOVMSKP() \
	if (cpu->cr0 & 0xc) THROW0(EX_NM); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	IN_AMD64(if (rex & REX_R) reg |= 8;) \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (mod == 3) { \
		IN_AMD64(if (rex & REX_B) rm |= 8;) \
		if (rep == 0) { \
			if (opsz16 == code16) /* NP */ { \
				uint32_t *v = fpu_xmm_get_u32(cpu->fpu, rm); \
				REGi(reg) = \
					((v[0] >> 31) & 1) | \
					((v[1] >> 30) & 2) | \
					((v[2] >> 29) & 4) | \
					((v[3] >> 28) & 8); \
			} else /* 66 */ { \
				uint32_t *v = fpu_xmm_get_u32(cpu->fpu, rm); \
				REGi(reg) = \
					((v[1] >> 31) & 1) | \
					((v[3] >> 30) & 2); \
			} \
		} else { \
			THROW0(EX_UD); \
		} \
	} else { \
		THROW0(EX_UD); \
	}

#define SIMD_GRP15() \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	IN_AMD64(if (rex & REX_R) reg |= 8;) \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (modrm == 0xf8) { \
		/* SFENCE */ \
	} else if (modrm == 0xe8) { \
		/* SSE2 LFENCE */ \
	} else if (modrm == 0xf0) { \
		/* SSE2 MFENCE */ \
	} else if (mod == 3) { \
		THROW0(EX_UD); \
	} else { \
		if ((cpu->cr0 & 0xc) && reg < 2) THROW0(EX_NM); \
		MODSIB(0); \
		TRY(fpu_xmm_grp15(cpu->fpu, cpu, reg, curr_seg, addr)); \
	}

#define SIMD_PREFETCH() \
	TRY(fetch8(cpu, &modrm)); \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (mod != 3) { \
		MODSIB(0); \
	}

#endif /* I386_ENABLE_SSE */

#ifdef I386_ENABLE_SSE2
#define TRY_SSE2(...) TRY((__VA_ARGS__))

#define MOVNTI() \
	if (opsz16 != code16 || rep) THROW0(EX_UD); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	IN_AMD64(if (rex & REX_R) reg |= 8;) \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (mod == 3) { \
		THROW0(EX_UD); \
	} else { \
		MODSIB(0); \
		IN_AMD64(if (rex & REX_W) { \
			TRY(translate64(cpu, &meml, 2, curr_seg, addr)); \
			saddr64(&meml, lreg64(reg)); \
		} else) \
		{ \
			TRY(translate32(cpu, &meml, 2, curr_seg, addr)); \
			saddr32(&meml, lreg32(reg)); \
		} \
	}

#define MOVQ_DQ() \
	if (cpu->cr0 & 0xc) THROW0(EX_NM); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	IN_AMD64(if (rex & REX_R) reg |= 8;) \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (mod == 3) { \
		IN_AMD64(if (rex & REX_B) rm |= 8;) \
		if (rep == 0) { \
			if (opsz16 == code16) /* NP */ { \
				THROW0(EX_UD); \
			} else /* 66 */ { \
				uint64_t *x = fpu_xmm_get_u64(cpu->fpu, rm); \
				x[0] = fpu_xmm_get_u64(cpu->fpu, reg)[0]; \
				x[1] = 0; \
			} \
		} else if (rep == 1) /* F3 */ { \
			/* MOVQ2DQ */ \
			uint64_t *x = fpu_xmm_get_u64(cpu->fpu, reg); \
			x[0] = fpu_mmx_get(cpu->fpu, rm & 7); \
			x[1] = 0; \
		} else /* F2 */ { \
			/* MOVDQ2Q */ \
			uint64_t *x = fpu_xmm_get_u64(cpu->fpu, rm); \
			fpu_mmx_set(cpu->fpu, reg & 7, x[0]); \
		} \
	} else { \
		MODSIB(0); \
		if (rep == 0) { \
			if (opsz16 == code16) /* NP */ { \
				THROW0(EX_UD); \
			} else /* 66 */ { \
				uint32_t *x = fpu_xmm_get_u32(cpu->fpu, reg); \
				TRY(cpu_store32(cpu, curr_seg, addr, x[0])); \
				TRY(cpu_store32(cpu, curr_seg, addr + 4, x[1])); \
			} \
		} else { \
			THROW0(EX_NM); \
		} \
	}

#define SSE2_MASKMOVDQU() \
	uint64_t *xmm1 = fpu_xmm_get_u64(cpu->fpu, reg); \
	uint64_t *xmm2 = fpu_xmm_get_u64(cpu->fpu, rm); \
	for (int k = 0; k < 2; k++) { \
		uint64_t mm1 = xmm1[k]; \
		uint64_t mm2 = xmm2[k]; \
		for (int i = 0; i < 8; i++) { \
			if ((mm2 >> 7) & 1) { \
				uword addr = REGi(7) + 8 * k + i; \
				if (adsz16) addr = addr & 0xffff; \
				TRY(cpu_store8(cpu, SEG_DS, addr, mm1)); \
			} \
			mm1 >>= 8; \
			mm2 >>= 8; \
		} \
	}

#define SSE2_PSHUFD() \
	if (cpu->cr0 & 0xc) THROW0(EX_NM); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	IN_AMD64(if (rex & REX_R) reg |= 8;) \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	u8 imm8; \
	if (mod == 3) { \
		IN_AMD64(if (rex & REX_B) rm |= 8;) \
		TRY(fetch8(cpu, &imm8)); \
		uint32_t *s = fpu_xmm_get_u32(cpu->fpu, rm); \
		uint32_t *d = fpu_xmm_get_u32(cpu->fpu, reg); \
		uint32_t w[4]; \
		w[0] = s[(imm8 >> 0) & 3]; \
		w[1] = s[(imm8 >> 2) & 3]; \
		w[2] = s[(imm8 >> 4) & 3]; \
		w[3] = s[(imm8 >> 6) & 3]; \
		d[0] = w[0]; \
		d[1] = w[1]; \
		d[2] = w[2]; \
		d[3] = w[3]; \
	} else { \
		uint32_t s[4]; \
		MODSIB(1); \
		TRY(fetch8(cpu, &imm8)); \
		TRY(cpu_load32(cpu, curr_seg, addr, s)); \
		TRY(cpu_load32(cpu, curr_seg, addr + 4, s + 1)); \
		TRY(cpu_load32(cpu, curr_seg, addr + 8, s + 2)); \
		TRY(cpu_load32(cpu, curr_seg, addr + 12, s + 3)); \
		uint32_t *d = fpu_xmm_get_u32(cpu->fpu, reg); \
		d[0] = s[(imm8 >> 0) & 3]; \
		d[1] = s[(imm8 >> 2) & 3]; \
		d[2] = s[(imm8 >> 4) & 3]; \
		d[3] = s[(imm8 >> 6) & 3]; \
	}

#define SSE2_PSHUFLW() \
	if (cpu->cr0 & 0xc) THROW0(EX_NM); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	IN_AMD64(if (rex & REX_R) reg |= 8;) \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	u8 imm8; \
	if (mod == 3) { \
		IN_AMD64(if (rex & REX_B) rm |= 8;) \
		TRY(fetch8(cpu, &imm8)); \
		uint16_t *s = fpu_xmm_get_u16(cpu->fpu, rm); \
		uint16_t *d = fpu_xmm_get_u16(cpu->fpu, reg); \
		uint16_t w[4]; \
		w[0] = s[(imm8 >> 0) & 3]; \
		w[1] = s[(imm8 >> 2) & 3]; \
		w[2] = s[(imm8 >> 4) & 3]; \
		w[3] = s[(imm8 >> 6) & 3]; \
		d[0] = w[0]; \
		d[1] = w[1]; \
		d[2] = w[2]; \
		d[3] = w[3]; \
		d[4] = s[4]; \
		d[5] = s[5]; \
		d[6] = s[6]; \
		d[7] = s[7]; \
	} else { \
		union { uint16_t w[8]; uint32_t v[4]; } s; \
		MODSIB(1); \
		TRY(fetch8(cpu, &imm8)); \
		TRY(cpu_load32(cpu, curr_seg, addr, s.v)); \
		TRY(cpu_load32(cpu, curr_seg, addr + 4, s.v + 1)); \
		TRY(cpu_load32(cpu, curr_seg, addr + 8, s.v + 2)); \
		TRY(cpu_load32(cpu, curr_seg, addr + 12, s.v + 3)); \
		uint16_t *d = fpu_xmm_get_u16(cpu->fpu, reg); \
		d[0] = s.w[(imm8 >> 0) & 3]; \
		d[1] = s.w[(imm8 >> 2) & 3]; \
		d[2] = s.w[(imm8 >> 4) & 3]; \
		d[3] = s.w[(imm8 >> 6) & 3]; \
		d[4] = s.w[4]; \
		d[5] = s.w[5]; \
		d[6] = s.w[6]; \
		d[7] = s.w[7]; \
	}

#define SSE2_PSHUFHW() \
	if (cpu->cr0 & 0xc) THROW0(EX_NM); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	IN_AMD64(if (rex & REX_R) reg |= 8;) \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	u8 imm8; \
	if (mod == 3) { \
		IN_AMD64(if (rex & REX_B) rm |= 8;) \
		TRY(fetch8(cpu, &imm8)); \
		uint16_t *s = fpu_xmm_get_u16(cpu->fpu, rm); \
		uint16_t *d = fpu_xmm_get_u16(cpu->fpu, reg); \
		uint16_t w[4]; \
		w[0] = s[4 + ((imm8 >> 0) & 3)]; \
		w[1] = s[4 + ((imm8 >> 2) & 3)]; \
		w[2] = s[4 + ((imm8 >> 4) & 3)]; \
		w[3] = s[4 + ((imm8 >> 6) & 3)]; \
		d[0] = s[0]; \
		d[1] = s[1]; \
		d[2] = s[2]; \
		d[3] = s[3]; \
		d[4] = w[0]; \
		d[5] = w[1]; \
		d[6] = w[2]; \
		d[7] = w[3]; \
	} else { \
		union { uint16_t w[8]; uint32_t v[4]; } s; \
		MODSIB(1); \
		TRY(fetch8(cpu, &imm8)); \
		TRY(cpu_load32(cpu, curr_seg, addr, s.v)); \
		TRY(cpu_load32(cpu, curr_seg, addr + 4, s.v + 1)); \
		TRY(cpu_load32(cpu, curr_seg, addr + 8, s.v + 2)); \
		TRY(cpu_load32(cpu, curr_seg, addr + 12, s.v + 3)); \
		uint16_t *d = fpu_xmm_get_u16(cpu->fpu, reg); \
		d[0] = s.w[0]; \
		d[1] = s.w[1]; \
		d[2] = s.w[2]; \
		d[3] = s.w[3]; \
		d[0] = s.w[4 + ((imm8 >> 0) & 3)]; \
		d[1] = s.w[4 + ((imm8 >> 2) & 3)]; \
		d[2] = s.w[4 + ((imm8 >> 4) & 3)]; \
		d[3] = s.w[4 + ((imm8 >> 6) & 3)]; \
	}

#define CVTS2D_D2S() \
	if (cpu->cr0 & 0xc) THROW0(EX_NM); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	IN_AMD64(if (rex & REX_R) reg |= 8;) \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (mod == 3) { \
		IN_AMD64(if (rex & REX_B) rm |= 8;) \
		if (rep == 0) { \
			if (opsz16 == code16) /* NP */ { \
				/* CVTPS2PD */ \
				float *ps = fpu_xmm_get_f32(cpu->fpu, rm); \
				double *pd = fpu_xmm_get_f64(cpu->fpu, reg); \
				float ps0 = ps[0]; \
				float ps1 = ps[1]; \
				pd[0] = (double) ps0; \
				pd[1] = (double) ps1; \
			} else /* 66 */ { \
				/* CVTPD2PS */ \
				double *pd = fpu_xmm_get_f64(cpu->fpu, rm); \
				float *ps = fpu_xmm_get_f32(cpu->fpu, reg); \
				double pd0 = pd[0]; \
				double pd1 = pd[1]; \
				ps[0] = (float) pd0; \
				ps[1] = (float) pd1; \
				ps[2] = 0.0; \
				ps[3] = 0.0; \
			} \
		} else if (rep == 1) /* F3 */ { \
			/* CVTSS2SD */ \
			float ss = fpu_xmm_get_f32(cpu->fpu, rm)[0]; \
			double *sd = fpu_xmm_get_f64(cpu->fpu, reg); \
			*sd = (double) ss; \
		} else /* F2 */ { \
			/* CVTSD2SS */ \
			double sd = fpu_xmm_get_f64(cpu->fpu, rm)[0]; \
			float *ss = fpu_xmm_get_f32(cpu->fpu, reg); \
			*ss = (float) sd; \
		} \
	} else { \
		MODSIB(0); \
		if (rep == 0) { \
			if (opsz16 == code16) /* NP */ { \
				/* CVTPS2PD */ \
				union {float f; uint32_t v;} ps[2]; \
				TRY(cpu_load32(cpu, curr_seg, addr, &(ps[0].v))); \
				TRY(cpu_load32(cpu, curr_seg, addr + 4, &(ps[1].v))); \
				double *pd = fpu_xmm_get_f64(cpu->fpu, reg); \
				pd[0] = (double) ps[0].f; \
				pd[1] = (double) ps[1].f; \
			} else /* 66 */ { \
				/* CVTPD2PS */ \
				union {double f; uint32_t v[2];} pd[2]; \
				TRY(cpu_load32(cpu, curr_seg, addr, &(pd[0].v[0]))); \
				TRY(cpu_load32(cpu, curr_seg, addr + 4, &(pd[0].v[1]))); \
				TRY(cpu_load32(cpu, curr_seg, addr + 8, &(pd[1].v[0]))); \
				TRY(cpu_load32(cpu, curr_seg, addr + 12, &(pd[1].v[1]))); \
				float *ps = fpu_xmm_get_f32(cpu->fpu, reg); \
				ps[0] = (float) pd[0].f; \
				ps[1] = (float) pd[1].f; \
				ps[2] = 0.0; \
				ps[3] = 0.0; \
			} \
		} else if (rep == 1) /* F3 */ { \
			/* CVTSS2SD */ \
			union {float f; uint32_t v;} ss; \
			TRY(cpu_load32(cpu, curr_seg, addr, &(ss.v))); \
			double *sd = fpu_xmm_get_f64(cpu->fpu, reg); \
			*sd = (double) ss.f; \
		} else /* F2 */ { \
			/* CVTSD2SS */ \
			union {double f; uint32_t v[2];} sd; \
			TRY(cpu_load32(cpu, curr_seg, addr, &(sd.v[0]))); \
			TRY(cpu_load32(cpu, curr_seg, addr + 4, &(sd.v[1]))); \
			float *ss = fpu_xmm_get_f32(cpu->fpu, reg); \
			*ss = (float) sd.f; \
		} \
	}

#define CVTDQ_PS() \
	if (cpu->cr0 & 0xc) THROW0(EX_NM); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	IN_AMD64(if (rex & REX_R) reg |= 8;) \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (mod == 3) { \
		IN_AMD64(if (rex & REX_B) rm |= 8;) \
		if (rep == 0) { \
			if (opsz16 == code16) /* NP */ { \
				/* CVTDQ2PS */ \
				uint32_t *dq = fpu_xmm_get_u32(cpu->fpu, rm); \
				float *ps = fpu_xmm_get_f32(cpu->fpu, reg); \
				int32_t dq0 = dq[0]; \
				int32_t dq1 = dq[1]; \
				int32_t dq2 = dq[2]; \
				int32_t dq3 = dq[3]; \
				ps[0] = dq0; \
				ps[1] = dq1; \
				ps[2] = dq2; \
				ps[3] = dq3; \
			} else /* 66 */ { \
				/* CVTPS2DQ */ \
				float *ps = fpu_xmm_get_f32(cpu->fpu, rm); \
				uint32_t *dq = fpu_xmm_get_u32(cpu->fpu, reg); \
				/* TODO: respect mxcsr */ \
				float ps0 = nearbyintf(ps[0]); \
				float ps1 = nearbyintf(ps[1]); \
				float ps2 = nearbyintf(ps[2]); \
				float ps3 = nearbyintf(ps[3]); \
				dq[0] = trunci32f(ps0); \
				dq[1] = trunci32f(ps1);	\
				dq[2] = trunci32f(ps2);	\
				dq[3] = trunci32f(ps3);	\
			} \
		} else if (rep == 1) /* F3 */ { \
			/* CVTTPS2DQ */ \
			float *ps = fpu_xmm_get_f32(cpu->fpu, rm); \
			uint32_t *dq = fpu_xmm_get_u32(cpu->fpu, reg); \
			float ps0 = ps[0]; \
			float ps1 = ps[1]; \
			float ps2 = ps[2]; \
			float ps3 = ps[3]; \
			dq[0] = trunci32f(ps0); \
			dq[1] = trunci32f(ps1);	\
			dq[2] = trunci32f(ps2);	\
			dq[3] = trunci32f(ps3);	\
		} else /* F2 */ { \
			THROW0(EX_UD); \
		} \
	} else { \
		MODSIB(0); \
		if (rep == 0) { \
			if (opsz16 == code16) /* NP */ { \
				/* CVTDQ2PS */ \
				uint32_t dq[4]; \
				TRY(cpu_load32(cpu, curr_seg, addr, &(dq[0]))); \
				TRY(cpu_load32(cpu, curr_seg, addr + 4, &(dq[1]))); \
				TRY(cpu_load32(cpu, curr_seg, addr + 8, &(dq[2]))); \
				TRY(cpu_load32(cpu, curr_seg, addr + 12, &(dq[3]))); \
				float *ps = fpu_xmm_get_f32(cpu->fpu, reg); \
				int32_t dq0 = dq[0]; \
				int32_t dq1 = dq[1]; \
				int32_t dq2 = dq[2]; \
				int32_t dq3 = dq[3]; \
				ps[0] = dq0; \
				ps[1] = dq1; \
				ps[2] = dq2; \
				ps[3] = dq3; \
			} else /* 66 */ { \
				/* CVTPS2DQ */ \
				union { float f; uint32_t v; } ps[4]; \
				TRY(cpu_load32(cpu, curr_seg, addr, &(ps[0].v))); \
				TRY(cpu_load32(cpu, curr_seg, addr + 4, &(ps[1].v))); \
				TRY(cpu_load32(cpu, curr_seg, addr + 8, &(ps[2].v))); \
				TRY(cpu_load32(cpu, curr_seg, addr + 12, &(ps[3].v))); \
				uint32_t *dq = fpu_xmm_get_u32(cpu->fpu, reg); \
				/* TODO: respect mxcsr */ \
				float ps0 = nearbyintf(ps[0].f); \
				float ps1 = nearbyintf(ps[1].f); \
				float ps2 = nearbyintf(ps[2].f); \
				float ps3 = nearbyintf(ps[3].f); \
				dq[0] = trunci32f(ps0); \
				dq[1] = trunci32f(ps1);	\
				dq[2] = trunci32f(ps2);	\
				dq[3] = trunci32f(ps3);	\
			} \
		} else if (rep == 1) /* F3 */ { \
			/* CVTTPS2DQ */ \
			union { float f; uint32_t v; } ps[4]; \
			TRY(cpu_load32(cpu, curr_seg, addr, &(ps[0].v))); \
			TRY(cpu_load32(cpu, curr_seg, addr + 4, &(ps[1].v))); \
			TRY(cpu_load32(cpu, curr_seg, addr + 8, &(ps[2].v))); \
			TRY(cpu_load32(cpu, curr_seg, addr + 12, &(ps[3].v))); \
			uint32_t *dq = fpu_xmm_get_u32(cpu->fpu, reg); \
			float ps0 = ps[0].f; \
			float ps1 = ps[1].f; \
			float ps2 = ps[2].f; \
			float ps3 = ps[3].f; \
			dq[0] = trunci32f(ps0); \
			dq[1] = trunci32f(ps1);	\
			dq[2] = trunci32f(ps2);	\
			dq[3] = trunci32f(ps3);	\
		} else /* F2 */ { \
			THROW0(EX_UD); \
		} \
	}

#define CVTDQ_PD() \
	if (cpu->cr0 & 0xc) THROW0(EX_NM); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	IN_AMD64(if (rex & REX_R) reg |= 8;) \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (mod == 3) { \
		IN_AMD64(if (rex & REX_B) rm |= 8;) \
		if (rep == 0) { \
			if (opsz16 == code16) /* NP */ { \
				THROW0(EX_UD); \
			} else /* 66 */ { \
				/* CVTTPD2DQ */ \
				double *pd = fpu_xmm_get_f64(cpu->fpu, rm); \
				uint32_t *dq = fpu_xmm_get_u32(cpu->fpu, reg); \
				double pd0 = pd[0]; \
				double pd1 = pd[1]; \
				dq[0] = trunci32(pd0); \
				dq[1] = trunci32(pd1); \
				dq[2] = 0; \
				dq[3] = 0; \
			} \
		} else if (rep == 1) /* F3 */ { \
			/* CVTDQ2PD */ \
			uint32_t *dq = fpu_xmm_get_u32(cpu->fpu, rm); \
			double *pd = fpu_xmm_get_f64(cpu->fpu, reg); \
			int32_t dq0 = dq[0]; \
			int32_t dq1 = dq[1]; \
			pd[0] = dq0; \
			pd[1] = dq1; \
		} else /* F2 */ { \
			/* CVTPD2DQ */ \
			double *pd = fpu_xmm_get_f64(cpu->fpu, rm); \
			uint32_t *dq = fpu_xmm_get_u32(cpu->fpu, reg); \
			double pd0 = nearbyint(pd[0]); \
			double pd1 = nearbyint(pd[1]); \
			dq[0] = trunci32(pd0); \
			dq[1] = trunci32(pd1); \
			dq[2] = 0; \
			dq[3] = 0; \
		} \
	} else { \
		MODSIB(0); \
		if (rep == 0) { \
			if (opsz16 == code16) /* NP */ { \
				THROW0(EX_UD); \
			} else /* 66 */ { \
				/* CVTTPD2DQ */ \
				union { double f; uint32_t v[2]; } pd[2]; \
				TRY(cpu_load32(cpu, curr_seg, addr, &(pd[0].v[0]))); \
				TRY(cpu_load32(cpu, curr_seg, addr + 4, &(pd[0].v[1]))); \
				TRY(cpu_load32(cpu, curr_seg, addr + 8, &(pd[1].v[0]))); \
				TRY(cpu_load32(cpu, curr_seg, addr + 12, &(pd[1].v[1]))); \
				uint32_t *dq = fpu_xmm_get_u32(cpu->fpu, reg); \
				double pd0 = pd[0].f; \
				double pd1 = pd[1].f; \
				dq[0] = trunci32(pd0); \
				dq[1] = trunci32(pd1); \
				dq[2] = 0; \
				dq[3] = 0; \
			} \
		} else if (rep == 1) /* F3 */ { \
			/* CVTDQ2PD */ \
			uint32_t dq[2]; \
			TRY(cpu_load32(cpu, curr_seg, addr, &(dq[0]))); \
			TRY(cpu_load32(cpu, curr_seg, addr + 4, &(dq[1]))); \
			double *pd = fpu_xmm_get_f64(cpu->fpu, reg); \
			int32_t dq0 = dq[0]; \
			int32_t dq1 = dq[1]; \
			pd[0] = dq0; \
			pd[1] = dq1; \
		} else /* F2 */ { \
			/* CVTPD2DQ */ \
			union { double f; uint32_t v[2]; } pd[2]; \
			TRY(cpu_load32(cpu, curr_seg, addr, &(pd[0].v[0]))); \
			TRY(cpu_load32(cpu, curr_seg, addr + 4, &(pd[0].v[1]))); \
			TRY(cpu_load32(cpu, curr_seg, addr + 8, &(pd[1].v[0]))); \
			TRY(cpu_load32(cpu, curr_seg, addr + 12, &(pd[1].v[1]))); \
			uint32_t *dq = fpu_xmm_get_u32(cpu->fpu, reg); \
			/* TODO: respect mxcsr */ \
			double pd0 = nearbyint(pd[0].f); \
			double pd1 = nearbyint(pd[1].f); \
			dq[0] = trunci32(pd0); \
			dq[1] = trunci32(pd1); \
			dq[2] = 0; \
			dq[3] = 0; \
		} \
	}
#else
#define TRY_SSE2(...) THROW0(EX_UD)
#define SSE2_MASKMOVDQU() THROW0(EX_UD)
#define SSE2_PSHUFD() THROW0(EX_UD)
#define SSE2_PSHUFLW() THROW0(EX_UD)
#define SSE2_PSHUFHW() THROW0(EX_UD)
#endif /* I386_ENABLE_SSE2 */

#ifdef I386_ENABLE_SSE3
#define LDDQU() \
	if (cpu->cr0 & 0xc) THROW0(EX_NM); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	IN_AMD64(if (rex & REX_R) reg |= 8;) \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	if (mod == 3) { \
		THROW0(EX_UD); \
	} else { \
		MODSIB(0); \
		if (rep != 2) { \
			THROW0(EX_UD); \
		} else /* F2 */ { \
			uint32_t *dq = fpu_xmm_get_u32(cpu->fpu, reg); \
			TRY(cpu_load32(cpu, curr_seg, addr, &(dq[0]))); \
			TRY(cpu_load32(cpu, curr_seg, addr + 4, &(dq[1]))); \
			TRY(cpu_load32(cpu, curr_seg, addr + 8, &(dq[2]))); \
			TRY(cpu_load32(cpu, curr_seg, addr + 12, &(dq[3]))); \
		} \
	}
#endif /* I386_ENABLE_SSE3 */

#ifdef I386_ENABLE_SSSE3
static uint64_t PALIGNR_helper(uint64_t d, uint64_t s, int i)
{
	if (i < 8) {
		int sh = i * 8;
		return (d >> sh) | (s << (64 - sh));
	} else if (i == 8) {
		return d;
	} else if (i < 16) {
		int sh = (i - 8) * 8;
		return d >> sh;
	} else {
		return 0;
	}
}

static void XMM_PALIGNR_helper(uint64_t *d, uint64_t *s, int i)
{
	/* d[1] d[0] s[1] s[0] */
	if (i < 8) {
		int sh1 = i * 8;
		int sh2 = (8 - i) * 8;
		d[1] = (s[1] >> sh1) | (d[0] << sh2);
		d[0] = (s[0] >> sh1) | (s[1] << sh2);
	} else if (i == 8) {
		d[1] = d[0];
		d[0] = s[1];
	} else if (i < 16) {
		int sh1 = (i - 8) * 8;
		int sh2 = (16 - i) * 8;
		d[1] = (d[0] >> sh1) | (d[1] << sh2);
		d[0] = (s[1] >> sh1) | (d[0] << sh2);
	} else if (i == 16) {
	} else if (i < 24) {
		int sh1 = (i - 16) * 8;
		int sh2 = (24 - i) * 8;
		d[0] = (d[0] >> sh1) | (d[1] << sh2);
		d[1] = (d[1] >> sh1);
	} else if (i == 24) {
		d[0] = d[1];
		d[1] = 0;
	} else if (i < 32) {
		int sh1 = (i - 24) * 8;
		d[0] = (d[1] >> sh1);
		d[1] = 0;
	} else {
		d[0] = 0;
		d[1] = 0;
	}
}

#define SIMD3_PqQq_VxWx() \
	if (cpu->cr0 & 0xc) THROW0(EX_NM); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	IN_AMD64(if (rex & REX_R) reg |= 8;) \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	int b2 = b1 + 0x100; \
	if (rep) THROW0(EX_UD); \
	if (mod == 3) { \
		IN_AMD64(if (rex & REX_B) rm |= 8;) \
		if (opsz16 == code16) /* NP */ { \
			TRY(fpu_mmx_rr(cpu->fpu, cpu, b2, reg & 7, rm & 7)); \
		} else /* 66 */ { \
			TRY(fpu_xmm_rr(cpu->fpu, cpu, b2, reg, rm)); \
		} \
	} else { \
		MODSIB(0); \
		if (opsz16 == code16) /* NP */ { \
			TRY(fpu_mmx_rm(cpu->fpu, cpu, b2, reg & 7, curr_seg, addr)); \
		} else /* 66 */ { \
			TRY(fpu_xmm_rm(cpu->fpu, cpu, b2, reg, curr_seg, addr)); \
		} \
	}

#define SIMD_PALIGNR() \
	if (cpu->cr0 & 0xc) THROW0(EX_NM); \
	TRY(fetch8(cpu, &modrm)); \
	int reg = (modrm >> 3) & 7; \
	IN_AMD64(if (rex & REX_R) reg |= 8;) \
	int mod = modrm >> 6; \
	int rm = modrm & 7; \
	u8 imm8; \
	if (mod == 3) { \
		IN_AMD64(if (rex & REX_B) rm |= 8;) \
		TRY(fetch8(cpu, &imm8)); \
		if (rep == 0) { \
			if (opsz16 == code16) /* NP */ { \
				uint64_t v = fpu_mmx_get(cpu->fpu, reg & 7); \
				uint64_t w = fpu_mmx_get(cpu->fpu, rm & 7); \
				fpu_mmx_set(cpu->fpu, reg & 7, PALIGNR_helper(v, w, imm8)); \
			} else /* 66 */ { \
				uint64_t *v = fpu_xmm_get_u64(cpu->fpu, reg); \
				uint64_t *w = fpu_xmm_get_u64(cpu->fpu, rm); \
				XMM_PALIGNR_helper(v, w, imm8); \
			} \
		} else { \
			THROW0(EX_UD); \
		} \
	} else { \
		MODSIB(1); \
		TRY(fetch8(cpu, &imm8)); \
		if (rep == 0) { \
			if (opsz16 == code16) /* NP */ { \
				uint64_t v = fpu_mmx_get(cpu->fpu, reg & 7); \
				uint32_t lo, hi; \
				TRY(cpu_load32(cpu, curr_seg, addr, &lo)); \
				TRY(cpu_load32(cpu, curr_seg, addr + 4, &hi)); \
				uint64_t w = lo | (((uint64_t) hi) << 32); \
				fpu_mmx_set(cpu->fpu, reg & 7, PALIGNR_helper(v, w, imm8)); \
			} else /* 66 */ { \
				union {uint64_t v64[2]; uint32_t v[4]; } u; \
				uint64_t *v = fpu_xmm_get_u64(cpu->fpu, reg); \
				TRY(cpu_load32(cpu, curr_seg, addr, &(u.v[0]))); \
				TRY(cpu_load32(cpu, curr_seg, addr + 4, &(u.v[1]))); \
				TRY(cpu_load32(cpu, curr_seg, addr + 8, &(u.v[2]))); \
				TRY(cpu_load32(cpu, curr_seg, addr + 12, &(u.v[3]))); \
				XMM_PALIGNR_helper(v, u.v64, imm8); \
			} \
		} else { \
			THROW0(EX_UD); \
		} \
	}
#endif /* I386_ENABLE_SSSE3 */
#endif /* SIMD_i386_c */

#ifdef SIMD_fpu_c
// MMX
static F80 *fpu_mm(FPU *fpu, int i)
{
	if (fpu->rawtagw & (1 << i)) {
		fpu->rawst[i] = tof80(fpu->st[i]);
		fpu->rawtagw &= ~(1 << i);
	}
	fpu->rawtagr &= ~(1 << i);
	return fpu->rawst + i;
}

typedef union {
	uint64_t v;
	uint64_t u64v[1];
	uint32_t u32v[2];
	uint16_t u16v[4];
	uint8_t u8v[8];
	int64_t s64v[1];
	int32_t s32v[2];
	int16_t s16v[4];
	int8_t s8v[8];
} UMMX;

#define MMX_v8_v(us, NAME) \
static uint64_t NAME ## 8(uint64_t a, uint64_t b) \
{ \
	UMMX v = { .v = a }; \
	UMMX w = { .v = b }; \
	NAME ## _(8, v. us ## 8v[0], w. us ## 8v[0]) \
	NAME ## _(8, v. us ## 8v[1], w. us ## 8v[1]) \
	NAME ## _(8, v. us ## 8v[2], w. us ## 8v[2]) \
	NAME ## _(8, v. us ## 8v[3], w. us ## 8v[3]) \
	NAME ## _(8, v. us ## 8v[4], w. us ## 8v[4]) \
	NAME ## _(8, v. us ## 8v[5], w. us ## 8v[5]) \
	NAME ## _(8, v. us ## 8v[6], w. us ## 8v[6]) \
	NAME ## _(8, v. us ## 8v[7], w. us ## 8v[7]) \
	return v.v; \
}

#define MMX_v16_v(us, NAME) \
static uint64_t NAME ## 16(uint64_t a, uint64_t b) \
{ \
	UMMX v = { .v = a }; \
	UMMX w = { .v = b }; \
	NAME ## _(16, v. us ## 16v[0], w. us ## 16v[0]) \
	NAME ## _(16, v. us ## 16v[1], w. us ## 16v[1]) \
	NAME ## _(16, v. us ## 16v[2], w. us ## 16v[2]) \
	NAME ## _(16, v. us ## 16v[3], w. us ## 16v[3]) \
	return v.v; \
}

#define MMX_v32_v(us, NAME) \
static uint64_t NAME ## 32(uint64_t a, uint64_t b) \
{ \
	UMMX v = { .v = a }; \
	UMMX w = { .v = b }; \
	NAME ## _(32, v. us ## 32v[0], w. us ## 32v[0]) \
	NAME ## _(32, v. us ## 32v[1], w. us ## 32v[1]) \
	return v.v; \
}

#define MMX_v64_v(us, NAME) \
static uint64_t NAME ## 64(uint64_t a, uint64_t b) \
{ \
	UMMX v = { .v = a }; \
	UMMX w = { .v = b }; \
	NAME ## _(64, v. us ## 64v[0], w. us ## 64v[0]) \
	return v.v; \
}

#define MMX_v_v(us, NAME) \
	MMX_v8_v(us, NAME) MMX_v16_v(us, NAME) MMX_v32_v(us, NAME)

#define MMX_v16_i(us, NAME) \
static uint64_t NAME ## 16(uint64_t a, int b) \
{ \
	UMMX v = { .v = a }; \
	NAME ## _(16, v. us ## 16v[0], b) \
	NAME ## _(16, v. us ## 16v[1], b) \
	NAME ## _(16, v. us ## 16v[2], b) \
	NAME ## _(16, v. us ## 16v[3], b) \
	return v.v; \
}

#define MMX_v32_i(us, NAME) \
static uint64_t NAME ## 32(uint64_t a, int b) \
{ \
	UMMX v = { .v = a }; \
	NAME ## _(32, v. us ## 32v[0], b) \
	NAME ## _(32, v. us ## 32v[1], b) \
	return v.v; \
}

#define MMX_v64_i(us, NAME) \
static uint64_t NAME ## 64(uint64_t a, int b) \
{ \
	UMMX v = { .v = a }; \
	NAME ## _(64, v. us ## 64v[0], b) \
	return v.v; \
}

#define MMX_v_i(us, NAME) \
	MMX_v16_i(us, NAME) MMX_v32_i(us, NAME) MMX_v64_i(us, NAME)

static inline uint8_t satu8(int a)
{
	return a > 255 ? 255 : a < 0 ? 0 : a;
}

static inline uint16_t satu16(int a)
{
	return a > 65535 ? 65535 : a < 0 ? 0 : a;
}

static inline int8_t sats8(int a)
{
	return a > 127 ? 127 : a < -128 ? -128 : a;
}

static inline int16_t sats16(int a)
{
	return a > 32767 ? 32767 : a < -32768 ? -32768 : a;
}

static inline int16_t his16(int a)
{
	return a >> 16;
}

static inline uint16_t hiu16(unsigned a)
{
	return a >> 16;
}

#define PADD_(B, a, b) a = a + b;
#define PSUB_(B, a, b) a = a - b;
#define PADDS_(B, a, b) a = sats ## B((int) a + (int) b);
#define PADDUS_(B, a, b) a = satu ## B((int) a + (int) b);
#define PSUBS_(B, a, b) a = sats ## B((int) a - (int) b);
#define PSUBUS_(B, a, b) a = satu ## B((int) a - (int) b);
#define PMULL_(B, a, b) a = a * b;
#define PMULH_(B, a, b) a = his ## B((int) a * (int) b);
#define PMULHU_(B, a, b) a = hiu ## B((unsigned) a * (unsigned) b);
#define PAND_(B, a, b) a = a & b;
#define PANDN_(B, a, b) a = (~a) & b;
#define POR_(B, a, b) a = a | b;
#define PXOR_(B, a, b) a = a ^ b;
#define PSLL_(B, a, b) a = (b >= B ? 0 : a << b);
#define PSRA_(B, a, b) a = (b >= B ? a >> (B - 1) : a >> b);
#define PSRL_(B, a, b) a = (b >= B ? 0 : a >> b);
#define PCMPEQ_(B, a, b) a = (a == b ? -1 : 0);
#define PCMPGT_(B, a, b) a = (a > b ? -1 : 0);

MMX_v_v(u, PADD)
MMX_v_v(u, PSUB)
MMX_v8_v(s, PADDS)
MMX_v16_v(s, PADDS)
MMX_v8_v(u, PADDUS)
MMX_v16_v(u, PADDUS)
MMX_v8_v(s, PSUBS)
MMX_v16_v(s, PSUBS)
MMX_v8_v(u, PSUBUS)
MMX_v16_v(u, PSUBUS)
MMX_v16_v(s, PMULL)
MMX_v16_v(s, PMULH)
MMX_v64_v(u, PAND)
MMX_v64_v(u, PANDN)
MMX_v64_v(u, POR)
MMX_v64_v(u, PXOR)
MMX_v_i(u, PSLL)
MMX_v16_i(s, PSRA)
MMX_v32_i(s, PSRA)
MMX_v_i(u, PSRL)
MMX_v_v(s, PCMPEQ)
MMX_v_v(s, PCMPGT)
MMX_v64_v(u, PADD) // SSE2
MMX_v64_v(u, PSUB) // SSE2

#define PAVG_(B, a, b) a = ((int) a + (int) b + 1) >> 1;
#define PMAX_(B, a, b) a = a > b ? a : b;
#define PMIN_(B, a, b) a = a < b ? a : b;
#define _AD_(B, a, b) a = abs(a - b);
MMX_v8_v(u, PAVG)
MMX_v16_v(u, PAVG)
MMX_v8_v(u, PMAX) // u8
MMX_v16_v(s, PMAX) // s16
MMX_v8_v(u, PMIN) // u8
MMX_v16_v(s, PMIN) // s16
MMX_v16_v(u, PMULHU)
MMX_v8_v(u, _AD)

#define PSIGN_(B, a, b) a = (b == 0 ? 0 : (b < 0 ? -a : a));
#define PABS_(B, a, b) a = (b < 0 ? -b : b);
MMX_v_v(s, PSIGN)
MMX_v_v(s, PABS)

static uint64_t PMADDWD(uint64_t a, uint64_t b)
{
	UMMX v = { .v = a };
	UMMX w = { .v = b };
	int t[4];
	t[0] = (int) v.s16v[0] * (int) w.s16v[0];
	t[1] = (int) v.s16v[1] * (int) w.s16v[1];
	t[2] = (int) v.s16v[2] * (int) w.s16v[2];
	t[3] = (int) v.s16v[3] * (int) w.s16v[3];

	UMMX r;
	r.s32v[0] = t[0] + t[1];
	r.s32v[1] = t[2] + t[3];
	return r.v;
}

static uint64_t PACKSSWB(uint64_t a, uint64_t b)
{
	UMMX v = { .v = a };
	UMMX w = { .v = b };
	UMMX r;
	r.s8v[0] = sats8(v.s16v[0]);
	r.s8v[1] = sats8(v.s16v[1]);
	r.s8v[2] = sats8(v.s16v[2]);
	r.s8v[3] = sats8(v.s16v[3]);
	r.s8v[4] = sats8(w.s16v[0]);
	r.s8v[5] = sats8(w.s16v[1]);
	r.s8v[6] = sats8(w.s16v[2]);
	r.s8v[7] = sats8(w.s16v[3]);
	return r.v;
}

static uint64_t PUNPCKLBW(uint64_t a, uint64_t b)
{
	UMMX v = { .v = a };
	UMMX w = { .v = b };
	UMMX r;
	r.s8v[0] = v.s8v[0];
	r.s8v[1] = w.s8v[0];
	r.s8v[2] = v.s8v[1];
	r.s8v[3] = w.s8v[1];
	r.s8v[4] = v.s8v[2];
	r.s8v[5] = w.s8v[2];
	r.s8v[6] = v.s8v[3];
	r.s8v[7] = w.s8v[3];
	return r.v;
}

static uint64_t PUNPCKLWD(uint64_t a, uint64_t b)
{
	UMMX v = { .v = a };
	UMMX w = { .v = b };
	UMMX r;
	r.s16v[0] = v.s16v[0];
	r.s16v[1] = w.s16v[0];
	r.s16v[2] = v.s16v[1];
	r.s16v[3] = w.s16v[1];
	return r.v;
}

static uint64_t PUNPCKLDQ(uint64_t a, uint64_t b)
{
	UMMX v = { .v = a };
	UMMX w = { .v = b };
	UMMX r;
	r.s32v[0] = v.s32v[0];
	r.s32v[1] = w.s32v[0];
	return r.v;
}

static uint64_t PUNPCKHBW(uint64_t a, uint64_t b)
{
	UMMX v = { .v = a };
	UMMX w = { .v = b };
	UMMX r;
	r.s8v[0] = v.s8v[4];
	r.s8v[1] = w.s8v[4];
	r.s8v[2] = v.s8v[5];
	r.s8v[3] = w.s8v[5];
	r.s8v[4] = v.s8v[6];
	r.s8v[5] = w.s8v[6];
	r.s8v[6] = v.s8v[7];
	r.s8v[7] = w.s8v[7];
	return r.v;
}

static uint64_t PUNPCKHWD(uint64_t a, uint64_t b)
{
	UMMX v = { .v = a };
	UMMX w = { .v = b };
	UMMX r;
	r.s16v[0] = v.s16v[2];
	r.s16v[1] = w.s16v[2];
	r.s16v[2] = v.s16v[3];
	r.s16v[3] = w.s16v[3];
	return r.v;
}

static uint64_t PUNPCKHDQ(uint64_t a, uint64_t b)
{
	UMMX v = { .v = a };
	UMMX w = { .v = b };
	UMMX r;
	r.s32v[0] = v.s32v[1];
	r.s32v[1] = w.s32v[1];
	return r.v;
}

static uint64_t PACKUSWB(uint64_t a, uint64_t b)
{
	UMMX v = { .v = a };
	UMMX w = { .v = b };
	UMMX r;
	r.u8v[0] = satu8(v.s16v[0]);
	r.u8v[1] = satu8(v.s16v[1]);
	r.u8v[2] = satu8(v.s16v[2]);
	r.u8v[3] = satu8(v.s16v[3]);
	r.u8v[4] = satu8(w.s16v[0]);
	r.u8v[5] = satu8(w.s16v[1]);
	r.u8v[6] = satu8(w.s16v[2]);
	r.u8v[7] = satu8(w.s16v[3]);
	return r.v;
}

static uint64_t PACKSSDW(uint64_t a, uint64_t b)
{
	UMMX v = { .v = a };
	UMMX w = { .v = b };
	UMMX r;
	r.s16v[0] = sats16(v.s32v[0]);
	r.s16v[1] = sats16(v.s32v[1]);
	r.s16v[2] = sats16(w.s32v[0]);
	r.s16v[3] = sats16(w.s32v[1]);
	return r.v;
}

static uint64_t PSADBW(uint64_t a, uint64_t b)
{
	UMMX v;
	v.v = _AD8(a, b);
	uint16_t s = 0;
	s += v.u8v[0];
	s += v.u8v[1];
	s += v.u8v[2];
	s += v.u8v[3];
	s += v.u8v[4];
	s += v.u8v[5];
	s += v.u8v[6];
	s += v.u8v[7];
	return s;
}

static uint64_t PMULUDQ(uint64_t a, uint64_t b) // SSE2
{
	return (a & 0xffffffff) * (b & 0xffffffff);
}

static uint64_t PSHUFB(uint64_t a, uint64_t b)
{
	UMMX v = { .v = a };
	UMMX w = { .v = b };
	UMMX r;
	for (int i = 0; i < 8; i++) {
		if (w.s8v[i] < 0)
			r.u8v[i] = 0;
		else
			r.u8v[i] = v.u8v[w.s8v[i] & 7];
	}
	return r.v;
}

static uint64_t PHADDW(uint64_t a, uint64_t b)
{
	UMMX v = { .v = a };
	UMMX w = { .v = b };
	UMMX r;
	r.u16v[0] = v.u16v[0] + v.u16v[1];
	r.u16v[1] = v.u16v[2] + v.u16v[3];
	r.u16v[2] = w.u16v[0] + w.u16v[1];
	r.u16v[3] = w.u16v[2] + w.u16v[3];
	return r.v;
}

static uint64_t PHADDD(uint64_t a, uint64_t b)
{
	UMMX v = { .v = a };
	UMMX w = { .v = b };
	UMMX r;
	r.u32v[0] = v.u32v[0] + v.u32v[1];
	r.u32v[1] = w.u32v[0] + w.u32v[1];
	return r.v;
}

static uint64_t PHADDSW(uint64_t a, uint64_t b)
{
	UMMX v = { .v = a };
	UMMX w = { .v = b };
	UMMX r;
	r.s16v[0] = sats16((int) v.s16v[0] + (int) v.s16v[1]);
	r.s16v[1] = sats16((int) v.s16v[2] + (int) v.s16v[3]);
	r.s16v[2] = sats16((int) w.s16v[0] + (int) w.s16v[1]);
	r.s16v[3] = sats16((int) w.s16v[2] + (int) w.s16v[3]);
	return r.v;
}

static uint64_t PMADDUBSW(uint64_t a, uint64_t b)
{
	UMMX v = { .v = a };
	UMMX w = { .v = b };
	UMMX r;
	r.s16v[0] = sats16((int) v.u8v[0] * (int) w.u8v[0] + (int) v.u8v[1] * (int) w.u8v[1]);
	r.s16v[1] = sats16((int) v.u8v[2] * (int) w.u8v[2] + (int) v.u8v[3] * (int) w.u8v[3]);
	r.s16v[2] = sats16((int) v.u8v[4] * (int) w.u8v[4] + (int) v.u8v[5] * (int) w.u8v[5]);
	r.s16v[3] = sats16((int) v.u8v[6] * (int) w.u8v[6] + (int) v.u8v[7] * (int) w.u8v[7]);
	return r.v;
}

static uint64_t PHSUBW(uint64_t a, uint64_t b)
{
	UMMX v = { .v = a };
	UMMX w = { .v = b };
	UMMX r;
	r.u16v[0] = v.u16v[0] - v.u16v[1];
	r.u16v[1] = v.u16v[2] - v.u16v[3];
	r.u16v[2] = w.u16v[0] - w.u16v[1];
	r.u16v[3] = w.u16v[2] - w.u16v[3];
	return r.v;
}

static uint64_t PHSUBD(uint64_t a, uint64_t b)
{
	UMMX v = { .v = a };
	UMMX w = { .v = b };
	UMMX r;
	r.u32v[0] = v.u32v[0] - v.u32v[1];
	r.u32v[1] = w.u32v[0] - w.u32v[1];
	return r.v;
}

static uint64_t PHSUBSW(uint64_t a, uint64_t b)
{
	UMMX v = { .v = a };
	UMMX w = { .v = b };
	UMMX r;
	r.s16v[0] = sats16((int) v.s16v[0] - (int) v.s16v[1]);
	r.s16v[1] = sats16((int) v.s16v[2] - (int) v.s16v[3]);
	r.s16v[2] = sats16((int) w.s16v[0] - (int) w.s16v[1]);
	r.s16v[3] = sats16((int) w.s16v[2] - (int) w.s16v[3]);
	return r.v;
}

static uint64_t PMULHRSW(uint64_t a, uint64_t b)
{
	UMMX v = { .v = a };
	UMMX w = { .v = b };
	UMMX r;
	r.s16v[0] = ((((int) v.s16v[0] * (int) w.s16v[0]) >> 14) + 1) >> 1;
	r.s16v[1] = ((((int) v.s16v[1] * (int) w.s16v[1]) >> 14) + 1) >> 1;
	r.s16v[2] = ((((int) v.s16v[2] * (int) w.s16v[2]) >> 14) + 1) >> 1;
	r.s16v[3] = ((((int) v.s16v[3] * (int) w.s16v[3]) >> 14) + 1) >> 1;
	return r.v;
}

bool fpu_mmx_rr(FPU *fpu, void *cpu, int op, int dst, int src)
{
	uint64_t d = fpu_mmx_get(fpu, dst);
	uint64_t s = fpu_mmx_get(fpu, src);
	switch (op) {
	case 0x60: d = PUNPCKLBW(d, s); break;
	case 0x61: d = PUNPCKLWD(d, s); break;
	case 0x62: d = PUNPCKLDQ(d, s); break;
	case 0x63: d = PACKSSWB(d, s); break;
	case 0x64: d = PCMPGT8(d, s); break;
	case 0x65: d = PCMPGT16(d, s); break;
	case 0x66: d = PCMPGT32(d, s); break;
	case 0x67: d = PACKUSWB(d, s); break;
	case 0x68: d = PUNPCKHBW(d, s); break;
	case 0x69: d = PUNPCKHWD(d, s); break;
	case 0x6a: d = PUNPCKHDQ(d, s); break;
	case 0x6b: d = PACKSSDW(d, s); break;
	case 0x74: d = PCMPEQ8(d, s); break;
	case 0x75: d = PCMPEQ16(d, s); break;
	case 0x76: d = PCMPEQ32(d, s); break;
	case 0xd1: d = PSRL16(d, s); break;
	case 0xd2: d = PSRL32(d, s); break;
	case 0xd3: d = PSRL64(d, s); break;
	case 0xe1: d = PSRA16(d, s); break;
	case 0xe2: d = PSRA32(d, s); break;
	case 0xf1: d = PSLL16(d, s); break;
	case 0xf2: d = PSLL32(d, s); break;
	case 0xf3: d = PSLL64(d, s); break;
	case 0xd5: d = PMULL16(d, s); break;
	case 0xe5: d = PMULH16(d, s); break;
	case 0xf5: d = PMADDWD(d, s); break;
	case 0xd8: d = PSUBUS8(d, s); break;
	case 0xd9: d = PSUBUS16(d, s); break;
	case 0xe8: d = PSUBS8(d, s); break;
	case 0xe9: d = PSUBS16(d, s); break;
	case 0xf8: d = PSUB8(d, s); break;
	case 0xf9: d = PSUB16(d, s); break;
	case 0xfa: d = PSUB32(d, s); break;
	case 0xdb: d = PAND64(d, s); break;
	case 0xeb: d = POR64(d, s); break;
	case 0xdc: d = PADDUS8(d, s); break;
	case 0xdd: d = PADDUS16(d, s); break;
	case 0xec: d = PADDS8(d, s); break;
	case 0xed: d = PADDS16(d, s); break;
	case 0xfc: d = PADD8(d, s); break;
	case 0xfd: d = PADD16(d, s); break;
	case 0xfe: d = PADD32(d, s); break;
	case 0xdf: d = PANDN64(d, s); break;
	case 0xef: d = PXOR64(d, s); break;
	case 0x6f: case 0x7f: d = s; break;
	/* SSE */
	case 0xda: d = PMIN8(d, s); break;
	case 0xde: d = PMAX8(d, s); break;
	case 0xe0: d = PAVG8(d, s); break;
	case 0xe3: d = PAVG16(d, s); break;
	case 0xe4: d = PMULHU16(d, s); break;
	case 0xea: d = PMIN16(d, s); break;
	case 0xee: d = PMAX16(d, s); break;
	case 0xf6: d = PSADBW(d, s); break;
	/* SSE2 */
	case 0xd4: d = PADD64(d, s); break;
	case 0xfb: d = PSUB64(d, s); break;
	case 0xf4: d = PMULUDQ(d, s); break;
#ifdef I386_ENABLE_SSSE3
	/* SSSE3 */
	case 0x100: d = PSHUFB(d, s); break;
	case 0x101: d = PHADDW(d, s); break;
	case 0x102: d = PHADDD(d, s); break;
	case 0x103: d = PHADDSW(d, s); break;
	case 0x104: d = PMADDUBSW(d, s); break;
	case 0x105: d = PHSUBW(d, s); break;
	case 0x106: d = PHSUBD(d, s); break;
	case 0x107: d = PHSUBSW(d, s); break;
	case 0x108: d = PSIGN8(d, s); break;
	case 0x109: d = PSIGN16(d, s); break;
	case 0x10a: d = PSIGN32(d, s); break;
	case 0x10b: d = PMULHRSW(d, s); break;
	case 0x11c: d = PABS8(d, s); break;
	case 0x11d: d = PABS16(d, s); break;
	case 0x11e: d = PABS32(d, s); break;
#endif
	default:
		cpu_setexc(cpu, 6, 0);
		return false;
	}
	fpu_mmx_set(fpu, dst, d);
	return true;
}

bool fpu_mmx_rm(FPU *fpu, void *cpu, int op, int dst, int seg, uword addr)
{
	uint64_t d = fpu_mmx_get(fpu, dst);
	uint32_t lo, hi;
	if(!cpu_load32(cpu, seg, addr, &lo))
		return false;
	if(!cpu_load32(cpu, seg, addr + 4, &hi))
		return false;
	uint64_t s = (((uint64_t) hi) << 32) | lo;

	switch (op) {
	case 0x60: d = PUNPCKLBW(d, s); break;
	case 0x61: d = PUNPCKLWD(d, s); break;
	case 0x62: d = PUNPCKLDQ(d, s); break;
	case 0x63: d = PACKSSWB(d, s); break;
	case 0x64: d = PCMPGT8(d, s); break;
	case 0x65: d = PCMPGT16(d, s); break;
	case 0x66: d = PCMPGT32(d, s); break;
	case 0x67: d = PACKUSWB(d, s); break;
	case 0x68: d = PUNPCKHBW(d, s); break;
	case 0x69: d = PUNPCKHWD(d, s); break;
	case 0x6a: d = PUNPCKHDQ(d, s); break;
	case 0x6b: d = PACKSSDW(d, s); break;
	case 0x74: d = PCMPEQ8(d, s); break;
	case 0x75: d = PCMPEQ16(d, s); break;
	case 0x76: d = PCMPEQ32(d, s); break;
	case 0xd1: d = PSRL16(d, s); break;
	case 0xd2: d = PSRL32(d, s); break;
	case 0xd3: d = PSRL64(d, s); break;
	case 0xe1: d = PSRA16(d, s); break;
	case 0xe2: d = PSRA32(d, s); break;
	case 0xf1: d = PSLL16(d, s); break;
	case 0xf2: d = PSLL32(d, s); break;
	case 0xf3: d = PSLL64(d, s); break;
	case 0xd5: d = PMULL16(d, s); break;
	case 0xe5: d = PMULH16(d, s); break;
	case 0xf5: d = PMADDWD(d, s); break;
	case 0xd8: d = PSUBUS8(d, s); break;
	case 0xd9: d = PSUBUS16(d, s); break;
	case 0xe8: d = PSUBS8(d, s); break;
	case 0xe9: d = PSUBS16(d, s); break;
	case 0xf8: d = PSUB8(d, s); break;
	case 0xf9: d = PSUB16(d, s); break;
	case 0xfa: d = PSUB32(d, s); break;
	case 0xdb: d = PAND64(d, s); break;
	case 0xeb: d = POR64(d, s); break;
	case 0xdc: d = PADDUS8(d, s); break;
	case 0xdd: d = PADDUS16(d, s); break;
	case 0xec: d = PADDS8(d, s); break;
	case 0xed: d = PADDS16(d, s); break;
	case 0xfc: d = PADD8(d, s); break;
	case 0xfd: d = PADD16(d, s); break;
	case 0xfe: d = PADD32(d, s); break;
	case 0xdf: d = PANDN64(d, s); break;
	case 0xef: d = PXOR64(d, s); break;
	case 0x6f: d = s; break;
	/* SSE */
	case 0xda: d = PMIN8(d, s); break;
	case 0xde: d = PMAX8(d, s); break;
	case 0xe0: d = PAVG8(d, s); break;
	case 0xe3: d = PAVG16(d, s); break;
	case 0xe4: d = PMULHU16(d, s); break;
	case 0xea: d = PMIN16(d, s); break;
	case 0xee: d = PMAX16(d, s); break;
	case 0xf6: d = PSADBW(d, s); break;
	/* SSE2 */
	case 0xd4: d = PADD64(d, s); break;
	case 0xfb: d = PSUB64(d, s); break;
	case 0xf4: d = PMULUDQ(d, s); break;
#ifdef I386_ENABLE_SSSE3
	/* SSSE3 */
	case 0x100: d = PSHUFB(d, s); break;
	case 0x101: d = PHADDW(d, s); break;
	case 0x102: d = PHADDD(d, s); break;
	case 0x103: d = PHADDSW(d, s); break;
	case 0x104: d = PMADDUBSW(d, s); break;
	case 0x105: d = PHSUBW(d, s); break;
	case 0x106: d = PHSUBD(d, s); break;
	case 0x107: d = PHSUBSW(d, s); break;
	case 0x108: d = PSIGN8(d, s); break;
	case 0x109: d = PSIGN16(d, s); break;
	case 0x10a: d = PSIGN32(d, s); break;
	case 0x10b: d = PMULHRSW(d, s); break;
	case 0x11c: d = PABS8(d, s); break;
	case 0x11d: d = PABS16(d, s); break;
	case 0x11e: d = PABS32(d, s); break;
#endif
	default:
		cpu_setexc(cpu, 6, 0);
		return false;
	}
	fpu_mmx_set(fpu, dst, d);
	return true;
}

bool fpu_mmx_ri(FPU *fpu, void *cpu, int op, int func, int dst, int imm)
{
	uint64_t v = fpu_mmx_get(fpu, dst);
	switch (op) {
	case 0x71:
		switch (func) {
		case 2: v = PSRL16(v, imm); break;
		case 4: v = PSRA16(v, imm); break;
		case 6: v = PSLL16(v, imm); break;
		default:
			cpu_setexc(cpu, 6, 0);
			return false;
		}
		break;
	case 0x72:
		switch (func) {
		case 2: v = PSRL32(v, imm); break;
		case 4: v = PSRA32(v, imm); break;
		case 6: v = PSLL32(v, imm); break;
		default:
			cpu_setexc(cpu, 6, 0);
			return false;
		}
		break;
	case 0x73:
		switch (func) {
		case 2: v = PSRL64(v, imm); break;
		case 6: v = PSLL64(v, imm); break;
		default:
			cpu_setexc(cpu, 6, 0);
			return false;
		}
		break;
	default:
		cpu_setexc(cpu, 6, 0);
		return false;
	}
	fpu_mmx_set(fpu, dst, v);
	return true;
}

bool fpu_mmx_mr(FPU *fpu, void *cpu, int op, int src, int seg, uword addr)
{
	if (op == 0x7f || op == 0xe7) { // MOVQ/MOVNTQ
		F80 *v = fpu_mm(fpu, src);
		if (!cpu_store32(cpu, seg, addr, v->mant0))
			return false;
		if (!cpu_store32(cpu, seg, addr + 4, v->mant1))
			return false;
		return true;
	}
	cpu_setexc(cpu, 6, 0);
	return false;
}

uint64_t fpu_mmx_get(FPU *fpu, int i)
{
	F80 *v = fpu_mm(fpu, i);
	return v->mant0 | ((((uint64_t) v->mant1) << 32));
}

void fpu_mmx_set(FPU *fpu, int i, uint64_t v)
{
	F80 *d = fpu_mm(fpu, i);
	d->mant0 = v;
	d->mant1 = v >> 32;
	d->high = 0xffff;
}

// SSE
#ifdef I386_ENABLE_SSE
#define XMM_sf32(NAME) \
static UXMM NAME ## sf32(UXMM a, UXMM b) \
{ \
	NAME ## _(32, a.f32v[0], b.f32v[0]) \
	return a; \
}

#define XMM_sf32i(NAME) \
static UXMM NAME ## sf32(UXMM a, UXMM b) \
{ \
	NAME ## _(32, a.u32v[0], a.f32v[0], b.f32v[0]) \
	return a; \
}

#define XMM_pf32(NAME) \
static UXMM NAME ## pf32(UXMM a, UXMM b) \
{ \
	NAME ## _(32, a.f32v[0], b.f32v[0]) \
	NAME ## _(32, a.f32v[1], b.f32v[1]) \
	NAME ## _(32, a.f32v[2], b.f32v[2]) \
	NAME ## _(32, a.f32v[3], b.f32v[3]) \
	return a; \
}

#define XMM_pf32i(NAME) \
static UXMM NAME ## pf32(UXMM a, UXMM b) \
{ \
	NAME ## _(32, a.u32v[0], a.f32v[0], b.f32v[0]) \
	NAME ## _(32, a.u32v[1], a.f32v[1], b.f32v[1]) \
	NAME ## _(32, a.u32v[2], a.f32v[2], b.f32v[2]) \
	NAME ## _(32, a.u32v[3], a.f32v[3], b.f32v[3]) \
	return a; \
}

#define XMM_pu32(NAME) \
static UXMM NAME ## pu32(UXMM a, UXMM b) \
{ \
	NAME ## _(32, a.u32v[0], b.u32v[0]) \
	NAME ## _(32, a.u32v[1], b.u32v[1]) \
	NAME ## _(32, a.u32v[2], b.u32v[2]) \
	NAME ## _(32, a.u32v[3], b.u32v[3]) \
	return a; \
}

#define ADD_(B, a, b) a = a + b;
#define SUB_(B, a, b) a = a - b;
#define MUL_(B, a, b) a = a * b;
#define DIV_(B, a, b) a = a / b;
#define RCP_(B, a, b) a = 1.0 / b;
#define SQRT_(B, a, b) a = sqrt(b);
#define MAX_(B, a, b) a = a > b ? a : b;
#define MIN_(B, a, b) a = a < b ? a : b;
#define RSQRT_(B, a, b) a = 1.0 / sqrt(b);

#define AND_(B, a, b) a = a & b;
#define ANDN_(B, a, b) a = (~a) & b;
#define OR_(B, a, b) a = a | b;
#define XOR_(B, a, b) a = a ^ b;

#define CMPEQ_(B, c, a, b) c = (a == b ? -1 : 0);
#define CMPLT_(B, c, a, b) c = (a < b ? -1 : 0);
#define CMPLE_(B, c, a, b) c = (a <= b ? -1 : 0);
#define CMPNORD_(B, c, a, b) c = (isunordered(a, b) ? -1 : 0);
#define CMPNEQ_(B, c, a, b) c = (a != b ? -1 : 0);
#define CMPNLT_(B, c, a, b) c = (!(a < b) ? -1 : 0);
#define CMPNLE_(B, c, a, b) c = (!(a <= b) ? -1 : 0);
#define CMPORD_(B, c, a, b) c = (!isunordered(a, b) ? -1 : 0);

XMM_sf32(ADD)
XMM_sf32(SUB)
XMM_sf32(MUL)
XMM_sf32(DIV)
XMM_sf32(RCP)
XMM_sf32(SQRT)
XMM_sf32(MAX)
XMM_sf32(MIN)
XMM_sf32(RSQRT)

XMM_pf32(ADD)
XMM_pf32(SUB)
XMM_pf32(MUL)
XMM_pf32(DIV)
XMM_pf32(RCP)
XMM_pf32(SQRT)
XMM_pf32(MAX)
XMM_pf32(MIN)
XMM_pf32(RSQRT)

XMM_pu32(AND)
XMM_pu32(ANDN)
XMM_pu32(OR)
XMM_pu32(XOR)

XMM_sf32i(CMPEQ)
XMM_sf32i(CMPLT)
XMM_sf32i(CMPLE)
XMM_sf32i(CMPNORD)
XMM_sf32i(CMPNEQ)
XMM_sf32i(CMPNLT)
XMM_sf32i(CMPNLE)
XMM_sf32i(CMPORD)
XMM_pf32i(CMPEQ)
XMM_pf32i(CMPLT)
XMM_pf32i(CMPLE)
XMM_pf32i(CMPNORD)
XMM_pf32i(CMPNEQ)
XMM_pf32i(CMPNLT)
XMM_pf32i(CMPNLE)
XMM_pf32i(CMPORD)

static UXMM SHUFpf32(UXMM a, UXMM b, int imm)
{
	UXMM r;
	r.u32v[0] = a.u32v[(imm >> 0) & 3];
	r.u32v[1] = a.u32v[(imm >> 2) & 3];
	r.u32v[2] = b.u32v[(imm >> 4) & 3];
	r.u32v[3] = b.u32v[(imm >> 6) & 3];
	return r;
}

static UXMM UNPCKLpf32(UXMM a, UXMM b)
{
	UXMM r;
	r.u32v[0] = a.u32v[0];
	r.u32v[1] = b.u32v[0];
	r.u32v[2] = a.u32v[1];
	r.u32v[3] = b.u32v[1];
	return r;
}

static UXMM UNPCKHpf32(UXMM a, UXMM b)
{
	UXMM r;
	r.u32v[0] = a.u32v[2];
	r.u32v[1] = b.u32v[2];
	r.u32v[2] = a.u32v[3];
	r.u32v[3] = b.u32v[3];
	return r;
}

static void UCOMIsf32(void *cpu, UXMM d, UXMM s)
{
	float a = d.f32v[0];
	float b = s.f32v[0];
	if (isunordered(a, b)) {
		cpu_setflags(cpu, ZF | PF | CF, OF | AF | SF);
	} else if (a == b) {
		cpu_setflags(cpu, ZF, PF | CF | OF | AF | SF);
	} else if (a < b) {
		cpu_setflags(cpu, CF, ZF | PF | OF | AF | SF);
	} else {
		cpu_setflags(cpu, 0, ZF | PF | CF | OF | AF | SF);
	}
}

#ifdef I386_ENABLE_SSE3
static UXMM MOVSLDUP(UXMM a)
{
	UXMM r;
	r.u32v[0] = a.u32v[0];
	r.u32v[1] = a.u32v[0];
	r.u32v[2] = a.u32v[2];
	r.u32v[3] = a.u32v[2];
	return r;
}

static UXMM MOVSHDUP(UXMM a)
{
	UXMM r;
	r.u32v[0] = a.u32v[1];
	r.u32v[1] = a.u32v[1];
	r.u32v[2] = a.u32v[3];
	r.u32v[3] = a.u32v[3];
	return r;
}

static UXMM ADDSUBpf32(UXMM a, UXMM b)
{
	UXMM r;
	r.u32v[0] = a.u32v[0] - b.u32v[0];
	r.u32v[1] = a.u32v[1] + b.u32v[1];
	r.u32v[2] = a.u32v[2] - b.u32v[2];
	r.u32v[3] = a.u32v[3] - b.u32v[3];
	return r;
}

static UXMM HADDpf32(UXMM a, UXMM b)
{
	UXMM r;
	r.u32v[0] = a.u32v[0] + a.u32v[1];
	r.u32v[1] = a.u32v[2] + a.u32v[3];
	r.u32v[2] = b.u32v[0] + b.u32v[1];
	r.u32v[3] = b.u32v[2] + b.u32v[3];
	return r;
}

static UXMM HSUBpf32(UXMM a, UXMM b)
{
	UXMM r;
	r.u32v[0] = a.u32v[0] - a.u32v[1];
	r.u32v[1] = a.u32v[2] - a.u32v[3];
	r.u32v[2] = b.u32v[0] - b.u32v[1];
	r.u32v[3] = b.u32v[2] - b.u32v[3];
	return r;
}
#endif

bool fpu_xmmsf32_rr(FPU *fpu, void *cpu, int op, int dst, int src)
{
	UXMM d = fpu->xmm[dst];
	UXMM s = fpu->xmm[src];
	switch (op) {
	case 0x10: d.u32v[0] = s.u32v[0]; break; // MOVSS
	case 0x2e: case 0x2f: UCOMIsf32(cpu, d, s); return true;

	case 0x58: d = ADDsf32(d, s); break;
	case 0x5c: d = SUBsf32(d, s); break;
	case 0x59: d = MULsf32(d, s); break;
	case 0x5e: d = DIVsf32(d, s); break;
	case 0x53: d = RCPsf32(d, s); break;
	case 0x51: d = SQRTsf32(d, s); break;
	case 0x5f: d = MAXsf32(d, s); break;
	case 0x5d: d = MINsf32(d, s); break;
	case 0x52: d = RSQRTsf32(d, s); break;
#ifdef I386_ENABLE_SSE3
	case 0x12: d = MOVSLDUP(s); break;
	case 0x16: d = MOVSHDUP(s); break;
#endif
	default:
		cpu_setexc(cpu, 6, 0);
		return false;
	}
	fpu->xmm[dst] = d;
	return true;
}

bool fpu_xmmsf32_rm(FPU *fpu, void *cpu, int op, int dst, int seg, uword addr)
{
	UXMM d = fpu->xmm[dst];
	UXMM s;
	if(!cpu_load32(cpu, seg, addr, s.u32v))
		return false;

	switch (op) {
	case 0x10: d.u32v[0] = s.u32v[0]; break; // MOVSS
	case 0x2e: case 0x2f: UCOMIsf32(cpu, d, s); return true;

	case 0x58: d = ADDsf32(d, s); break;
	case 0x5c: d = SUBsf32(d, s); break;
	case 0x59: d = MULsf32(d, s); break;
	case 0x5e: d = DIVsf32(d, s); break;
	case 0x53: d = RCPsf32(d, s); break;
	case 0x51: d = SQRTsf32(d, s); break;
	case 0x5f: d = MAXsf32(d, s); break;
	case 0x5d: d = MINsf32(d, s); break;
	case 0x52: d = RSQRTsf32(d, s); break;
#ifdef I386_ENABLE_SSE3
	case 0x12: d = MOVSLDUP(s); break;
	case 0x16: d = MOVSHDUP(s); break;
#endif
	default:
		cpu_setexc(cpu, 6, 0);
		return false;
	}
	fpu->xmm[dst] = d;
	return true;
}

bool fpu_xmmsf32_rri(FPU *fpu, void *cpu, int op, int dst, int src, int imm)
{
	UXMM d = fpu->xmm[dst];
	UXMM s = fpu->xmm[src];
	switch (op) {
	case 0xc2:
		switch(imm) {
		case 0: d = CMPEQsf32(d, s); break;
		case 1: d = CMPLTsf32(d, s); break;
		case 2: d = CMPLEsf32(d, s); break;
		case 3: d = CMPNORDsf32(d, s); break;
		case 4: d = CMPNEQsf32(d, s); break;
		case 5: d = CMPNLTsf32(d, s); break;
		case 6: d = CMPNLEsf32(d, s); break;
		case 7: d = CMPORDsf32(d, s); break;
		default:
			cpu_setexc(cpu, 6, 0);
			return false;
		}
		break;
	default:
		cpu_setexc(cpu, 6, 0);
		return false;
	}
	fpu->xmm[dst] = d;
	return true;
}

bool fpu_xmmsf32_mr(FPU *fpu, void *cpu, int op, int src, int seg, uword addr)
{
	UXMM d;
	UXMM s = fpu->xmm[src];
	switch (op) {
	case 0x11: d.u32v[0] = s.u32v[0]; break; // MOVSS
	default:
		cpu_setexc(cpu, 6, 0);
		return false;
	}
	if (!cpu_store32(cpu, seg, addr, d.u32v[0]))
		return false;
	return true;
}

bool fpu_xmmpf32_rr(FPU *fpu, void *cpu, int op, int dst, int src)
{
	UXMM d = fpu->xmm[dst];
	UXMM s = fpu->xmm[src];
	switch (op) {
	case 0x10: case 0x28: d = s; break; // MOVUPS/MOVAPS
	case 0x12: d.u32v[0] = s.u32v[2]; d.u32v[1] = s.u32v[3]; break; // MOVHLPS
	case 0x16: d.u32v[2] = s.u32v[0]; d.u32v[3] = s.u32v[1]; break; // MOVLHPS
	case 0x14: d = UNPCKLpf32(d, s); break;
	case 0x15: d = UNPCKHpf32(d, s); break;

	case 0x58: d = ADDpf32(d, s); break;
	case 0x5c: d = SUBpf32(d, s); break;
	case 0x59: d = MULpf32(d, s); break;
	case 0x5e: d = DIVpf32(d, s); break;
	case 0x53: d = RCPpf32(d, s); break;
	case 0x51: d = SQRTpf32(d, s); break;
	case 0x5f: d = MAXpf32(d, s); break;
	case 0x5d: d = MINpf32(d, s); break;
	case 0x52: d = RSQRTpf32(d, s); break;

	case 0x54: d = ANDpu32(d, s); break;
	case 0x55: d = ANDNpu32(d, s); break;
	case 0x56: d = ORpu32(d, s); break;
	case 0x57: d = XORpu32(d, s); break;
#ifdef I386_ENABLE_SSE3
	case 0xd0: d = ADDSUBpf32(d, s); break;
	case 0x7c: d = HADDpf32(d, s); break;
	case 0x7d: d = HSUBpf32(d, s); break;
#endif
	default:
		cpu_setexc(cpu, 6, 0);
		return false;
	}
	fpu->xmm[dst] = d;
	return true;
}

bool fpu_xmmpf32_rm(FPU *fpu, void *cpu, int op, int dst, int seg, uword addr)
{
	UXMM d = fpu->xmm[dst];
	UXMM s;
	int n = 4;
	if (op == 0x12 || op == 0x16) n = 2;
	for (int i = 0; i < n; i++)
		if(!cpu_load32(cpu, seg, addr + 4 * i, s.u32v + i))
			return false;

	switch (op) {
	case 0x10: case 0x28: d = s; break; // MOVUPS/MOVAPS
	case 0x12: d.u32v[0] = s.u32v[0]; d.u32v[1] = s.u32v[1]; break; // MOVLPS
	case 0x16: d.u32v[2] = s.u32v[0]; d.u32v[3] = s.u32v[1]; break; // MOVHPS
	case 0x14: d = UNPCKLpf32(d, s); break;
	case 0x15: d = UNPCKHpf32(d, s); break;

	case 0x58: d = ADDpf32(d, s); break;
	case 0x5c: d = SUBpf32(d, s); break;
	case 0x59: d = MULpf32(d, s); break;
	case 0x5e: d = DIVpf32(d, s); break;
	case 0x53: d = RCPpf32(d, s); break;
	case 0x51: d = SQRTpf32(d, s); break;
	case 0x5f: d = MAXpf32(d, s); break;
	case 0x5d: d = MINpf32(d, s); break;
	case 0x52: d = RSQRTpf32(d, s); break;

	case 0x54: d = ANDpu32(d, s); break;
	case 0x55: d = ANDNpu32(d, s); break;
	case 0x56: d = ORpu32(d, s); break;
	case 0x57: d = XORpu32(d, s); break;
#ifdef I386_ENABLE_SSE3
	case 0xd0: d = ADDSUBpf32(d, s); break;
	case 0x7c: d = HADDpf32(d, s); break;
	case 0x7d: d = HSUBpf32(d, s); break;
#endif
	default:
		cpu_setexc(cpu, 6, 0);
		return false;
	}
	fpu->xmm[dst] = d;
	return true;
}

bool fpu_xmmpf32_rri(FPU *fpu, void *cpu, int op, int dst, int src, int imm)
{
	UXMM d = fpu->xmm[dst];
	UXMM s = fpu->xmm[src];
	switch (op) {
	case 0xc2:
		switch(imm) {
		case 0: d = CMPEQpf32(d, s); break;
		case 1: d = CMPLTpf32(d, s); break;
		case 2: d = CMPLEpf32(d, s); break;
		case 3: d = CMPNORDpf32(d, s); break;
		case 4: d = CMPNEQpf32(d, s); break;
		case 5: d = CMPNLTpf32(d, s); break;
		case 6: d = CMPNLEpf32(d, s); break;
		case 7: d = CMPORDpf32(d, s); break;
		default:
			cpu_setexc(cpu, 6, 0);
			return false;
		}
		break;
	case 0xc6:
		d = SHUFpf32(d, s, imm); break;
	default:
		cpu_setexc(cpu, 6, 0);
		return false;
	}
	fpu->xmm[dst] = d;
	return true;
}

bool fpu_xmmpf32_mr(FPU *fpu, void *cpu, int op, int src, int seg, uword addr)
{
	UXMM d;
	UXMM s = fpu->xmm[src];
	switch (op) {
	case 0x11: case 0x29: case 0x2b: d = s; break; // MOVUPS/MOVAPS/MOVNTPS
	case 0x13: d.u32v[0] = s.u32v[0]; d.u32v[1] = s.u32v[1]; break; // MOVLPS
	case 0x17: d.u32v[0] = s.u32v[2]; d.u32v[1] = s.u32v[3]; break; // MOVHPS
	default:
		cpu_setexc(cpu, 6, 0);
		return false;
	}
	int n = 4;
	if (op == 0x13 || op == 0x17) n = 2;
	for (int i = 0; i < n; i++)
		if (!cpu_store32(cpu, seg, addr + 4 * i, d.u32v[i]))
			return false;
	return true;
}

void fpu_xmmsf32_set_s32(FPU *fpu, int dst, uint32_t v)
{
	fpu->xmm[dst].f32v[0] = (float) (int32_t) v;
}

void fpu_xmmsf32_set_s64(FPU *fpu, int dst, uint64_t v)
{
	fpu->xmm[dst].f32v[0] = (float) (int64_t) v;
}

void fpu_xmmpf32_set_s32(FPU *fpu, int dst, uint64_t v)
{
	UMMX w = {.v = v};
	fpu->xmm[dst].f32v[0] = (float) w.s32v[0];
	fpu->xmm[dst].f32v[1] = (float) w.s32v[1];
}

float *fpu_xmm_get_f32(FPU *fpu, int i)
{
	return fpu->xmm[i].f32v;
}

uint32_t *fpu_xmm_get_u32(FPU *fpu, int i)
{
	return fpu->xmm[i].u32v;
}

void fpu_xmmsf64_set_s32(FPU *fpu, int dst, uint32_t v)
{
	fpu->xmm[dst].f64v[0] = (double) (int32_t) v;
}

void fpu_xmmsf64_set_s64(FPU *fpu, int dst, uint64_t v)
{
	fpu->xmm[dst].f64v[0] = (double) (int64_t) v;
}

void fpu_xmmpf64_set_s32(FPU *fpu, int dst, uint64_t v)
{
	UMMX w = {.v = v};
	fpu->xmm[dst].f64v[0] = (double) w.s32v[0];
	fpu->xmm[dst].f64v[1] = (double) w.s32v[1];
}

double *fpu_xmm_get_f64(FPU *fpu, int i)
{
	return fpu->xmm[i].f64v;
}

uint64_t *fpu_xmm_get_u64(FPU *fpu, int i)
{
	return fpu->xmm[i].u64v;
}

uint16_t *fpu_xmm_get_u16(FPU *fpu, int i)
{
	return fpu->xmm[i].u16v;
}

bool fpu_xmm_grp15(FPU *fpu, void *cpu, int op, int seg, uword addr)
{
#ifndef USE_AMD64
#define N_XMM 8
#else
#define N_XMM 16
#endif
	if (!fpu) {
		cpu_setexc(cpu, 6, 0);
		return false;
	}
	switch (op) {
	case 0: { // FXSAVE
		uword start = addr;
		if(!cpu_store16(cpu, seg, addr, fpu->cw))
			return false;
		u16 sw = getsw(fpu);
		if(!cpu_store16(cpu, seg, addr + 2, sw))
			return false;
		if(!cpu_store32(cpu, seg, addr + 16 + 8, fpu->mxcsr))
			return false;
		start += 32;
		for (int j = 0; j < 8; j++) {
			if (fpu->rawtagw & (1 << j)) {
				fpu->rawst[j] = tof80(fpu->st[j]);
				fpu->rawtagw &= ~(1 << j);
			}
			if (!cpu_store32(cpu, seg, start,
					 fpu->rawst[j].mant0))
				return false;
			if (!cpu_store32(cpu, seg, start + 4,
					 fpu->rawst[j].mant1))
				return false;
			if (!cpu_store16(cpu, seg, start + 8,
					 fpu->rawst[j].high))
				return false;
			start += 16;
		}
		for (int j = 0; j < N_XMM; j++) {
			for (int k = 0; k < 4; k++) {
				if (!cpu_store32(cpu, seg, start, fpu->xmm[j].u32v[k]))
					return false;
				start += 4;
			}
		}
		return true;
	}
	case 1: { // FXRSTOR
		uword start = addr;
		if (!cpu_load16(cpu, seg, addr, &(fpu->cw)))
			return false;
		u16 sw;
		if (!cpu_load16(cpu, seg, addr + 2, &sw))
			return false;
		setsw(fpu, sw);
		start += 32;
		for (int j = 0; j < 8; j++) {
			if (!cpu_load32(cpu, seg, start,
					&fpu->rawst[j].mant0))
				return false;
			if (!cpu_load32(cpu, seg, start + 4,
					&fpu->rawst[j].mant1))
				return false;
			if (!cpu_load16(cpu, seg, start + 8,
					&fpu->rawst[j].high))
				return false;
			fpu->rawtagr &= ~(1 << j);
			fpu->rawtagw &= ~(1 << j);
			start += 16;
		}
		for (int j = 0; j < N_XMM; j++) {
			for (int k = 0; k < 4; k++) {
				if (!cpu_load32(cpu, seg, start, &fpu->xmm[j].u32v[k]))
					return false;
				start += 4;
			}
		}
		return true;
	}
	case 2: { // LDMXCSR
		uint32_t mxcsr;
		if (!cpu_load32(cpu, seg, addr, &mxcsr))
			return false;
		fpu->mxcsr = mxcsr;
		return true;
	}
	case 3: { // STMXCSR
		return cpu_store32(cpu, seg, addr, fpu->mxcsr);
	}
	/* SSE2 */
	case 7: // CLFLUSH
		return true;
	}
	cpu_setexc(cpu, 6, 0);
	return false;
#undef N_XMM
}
#endif /* I386_ENABLE_SSE */

#ifdef I386_ENABLE_SSE2
#define XMM_sf64(NAME) \
static UXMM NAME ## sf64(UXMM a, UXMM b) \
{ \
	NAME ## _(64, a.f64v[0], b.f64v[0]) \
	return a; \
}

#define XMM_sf64i(NAME) \
static UXMM NAME ## sf64(UXMM a, UXMM b) \
{ \
	NAME ## _(64, a.u64v[0], a.f64v[0], b.f64v[0]) \
	return a; \
}

#define XMM_pf64(NAME) \
static UXMM NAME ## pf64(UXMM a, UXMM b) \
{ \
	NAME ## _(64, a.f64v[0], b.f64v[0]) \
	NAME ## _(64, a.f64v[1], b.f64v[1]) \
	return a; \
}

#define XMM_pf64i(NAME) \
static UXMM NAME ## pf64(UXMM a, UXMM b) \
{ \
	NAME ## _(64, a.u64v[0], a.f64v[0], b.f64v[0]) \
	NAME ## _(64, a.u64v[1], a.f64v[1], b.f64v[1]) \
	return a; \
}

#define XMM_pu64(NAME) \
static UXMM NAME ## pu64(UXMM a, UXMM b) \
{ \
	NAME ## _(64, a.u64v[0], b.u64v[0]) \
	NAME ## _(64, a.u64v[1], b.u64v[1]) \
	return a; \
}

XMM_sf64(ADD)
XMM_sf64(SUB)
XMM_sf64(MUL)
XMM_sf64(DIV)
XMM_sf64(SQRT)
XMM_sf64(MAX)
XMM_sf64(MIN)

XMM_pf64(ADD)
XMM_pf64(SUB)
XMM_pf64(MUL)
XMM_pf64(DIV)
XMM_pf64(SQRT)
XMM_pf64(MAX)
XMM_pf64(MIN)

XMM_pu64(AND)
XMM_pu64(ANDN)
XMM_pu64(OR)
XMM_pu64(XOR)

XMM_sf64i(CMPEQ)
XMM_sf64i(CMPLT)
XMM_sf64i(CMPLE)
XMM_sf64i(CMPNORD)
XMM_sf64i(CMPNEQ)
XMM_sf64i(CMPNLT)
XMM_sf64i(CMPNLE)
XMM_sf64i(CMPORD)
XMM_pf64i(CMPEQ)
XMM_pf64i(CMPLT)
XMM_pf64i(CMPLE)
XMM_pf64i(CMPNORD)
XMM_pf64i(CMPNEQ)
XMM_pf64i(CMPNLT)
XMM_pf64i(CMPNLE)
XMM_pf64i(CMPORD)

static UXMM SHUFpf64(UXMM a, UXMM b, int imm)
{
	UXMM r;
	r.u64v[0] = a.u64v[(imm >> 0) & 1];
	r.u64v[1] = b.u64v[(imm >> 1) & 1];
	return r;
}

static UXMM UNPCKLpf64(UXMM a, UXMM b)
{
	UXMM r;
	r.u64v[0] = a.u64v[0];
	r.u64v[1] = b.u64v[0];
	return r;
}

static UXMM UNPCKHpf64(UXMM a, UXMM b)
{
	UXMM r;
	r.u64v[0] = a.u64v[1];
	r.u64v[1] = b.u64v[1];
	return r;
}

static void UCOMIsf64(void *cpu, UXMM d, UXMM s)
{
	double a = d.f64v[0];
	double b = s.f64v[0];
	if (isunordered(a, b)) {
		cpu_setflags(cpu, ZF | PF | CF, OF | AF | SF);
	} else if (a == b) {
		cpu_setflags(cpu, ZF, PF | CF | OF | AF | SF);
	} else if (a < b) {
		cpu_setflags(cpu, CF, ZF | PF | OF | AF | SF);
	} else {
		cpu_setflags(cpu, 0, ZF | PF | CF | OF | AF | SF);
	}
}

#ifdef I386_ENABLE_SSE3
static UXMM MOVDDUP(UXMM a)
{
	UXMM r;
	r.u64v[0] = a.u64v[0];
	r.u64v[1] = a.u64v[0];
	return r;
}

static UXMM ADDSUBpf64(UXMM a, UXMM b)
{
	UXMM r;
	r.u64v[0] = a.u64v[0] - b.u64v[0];
	r.u64v[1] = a.u64v[1] + b.u64v[1];
	return r;
}

static UXMM HADDpf64(UXMM a, UXMM b)
{
	UXMM r;
	r.u64v[0] = a.u64v[0] + a.u64v[1];
	r.u64v[1] = b.u64v[0] + b.u64v[1];
	return r;
}

static UXMM HSUBpf64(UXMM a, UXMM b)
{
	UXMM r;
	r.u64v[0] = a.u64v[0] - a.u64v[1];
	r.u64v[1] = b.u64v[0] - b.u64v[1];
	return r;
}
#endif

bool fpu_xmmsf64_rr(FPU *fpu, void *cpu, int op, int dst, int src)
{
	UXMM d = fpu->xmm[dst];
	UXMM s = fpu->xmm[src];
	switch (op) {
	case 0x10: d.u64v[0] = s.u64v[0]; break; // MOVSD
	case 0x2e: case 0x2f: UCOMIsf64(cpu, d, s); return true;

	case 0x58: d = ADDsf64(d, s); break;
	case 0x5c: d = SUBsf64(d, s); break;
	case 0x59: d = MULsf64(d, s); break;
	case 0x5e: d = DIVsf64(d, s); break;
	case 0x51: d = SQRTsf64(d, s); break;
	case 0x5f: d = MAXsf64(d, s); break;
	case 0x5d: d = MINsf64(d, s); break;
#ifdef I386_ENABLE_SSE3
	case 0x12: d = MOVDDUP(s); break;
#endif
	default:
		cpu_setexc(cpu, 6, 0);
		return false;
	}
	fpu->xmm[dst] = d;
	return true;
}

bool fpu_xmmsf64_rm(FPU *fpu, void *cpu, int op, int dst, int seg, uword addr)
{
	UXMM d = fpu->xmm[dst];
	UXMM s;
	if(!cpu_load32(cpu, seg, addr, s.u32v))
		return false;
	if(!cpu_load32(cpu, seg, addr + 4, s.u32v + 1))
		return false;

	switch (op) {
	case 0x10: d.u64v[0] = s.u64v[0]; break; // MOVSD
	case 0x2e: case 0x2f: UCOMIsf64(cpu, d, s); return true;

	case 0x58: d = ADDsf64(d, s); break;
	case 0x5c: d = SUBsf64(d, s); break;
	case 0x59: d = MULsf64(d, s); break;
	case 0x5e: d = DIVsf64(d, s); break;
	case 0x51: d = SQRTsf64(d, s); break;
	case 0x5f: d = MAXsf64(d, s); break;
	case 0x5d: d = MINsf64(d, s); break;
#ifdef I386_ENABLE_SSE3
	case 0x12: d = MOVDDUP(s); break;
#endif
	default:
		cpu_setexc(cpu, 6, 0);
		return false;
	}
	fpu->xmm[dst] = d;
	return true;
}

bool fpu_xmmsf64_rri(FPU *fpu, void *cpu, int op, int dst, int src, int imm)
{
	UXMM d = fpu->xmm[dst];
	UXMM s = fpu->xmm[src];
	switch (op) {
	case 0xc2:
		switch(imm) {
		case 0: d = CMPEQsf64(d, s); break;
		case 1: d = CMPLTsf64(d, s); break;
		case 2: d = CMPLEsf64(d, s); break;
		case 3: d = CMPNORDsf64(d, s); break;
		case 4: d = CMPNEQsf64(d, s); break;
		case 5: d = CMPNLTsf64(d, s); break;
		case 6: d = CMPNLEsf64(d, s); break;
		case 7: d = CMPORDsf64(d, s); break;
		default:
			cpu_setexc(cpu, 6, 0);
			return false;
		}
		break;
	default:
		cpu_setexc(cpu, 6, 0);
		return false;
	}
	fpu->xmm[dst] = d;
	return true;
}

bool fpu_xmmsf64_mr(FPU *fpu, void *cpu, int op, int src, int seg, uword addr)
{
	UXMM d;
	UXMM s = fpu->xmm[src];
	switch (op) {
	case 0x11: d.u64v[0] = s.u64v[0]; break; // MOVSD
	default:
		cpu_setexc(cpu, 6, 0);
		return false;
	}
	if (!cpu_store32(cpu, seg, addr, d.u32v[0]))
		return false;
	if (!cpu_store32(cpu, seg, addr + 4, d.u32v[1]))
		return false;
	return true;
}

bool fpu_xmmpf64_rr(FPU *fpu, void *cpu, int op, int dst, int src)
{
	UXMM d = fpu->xmm[dst];
	UXMM s = fpu->xmm[src];
	switch (op) {
	case 0x10: case 0x28: d = s; break; // MOVUPD/MOVAPD
	case 0x14: d = UNPCKLpf64(d, s); break;
	case 0x15: d = UNPCKHpf64(d, s); break;

	case 0x58: d = ADDpf64(d, s); break;
	case 0x5c: d = SUBpf64(d, s); break;
	case 0x59: d = MULpf64(d, s); break;
	case 0x5e: d = DIVpf64(d, s); break;
	case 0x51: d = SQRTpf64(d, s); break;
	case 0x5f: d = MAXpf64(d, s); break;
	case 0x5d: d = MINpf64(d, s); break;

	case 0x54: d = ANDpu64(d, s); break;
	case 0x55: d = ANDNpu64(d, s); break;
	case 0x56: d = ORpu64(d, s); break;
	case 0x57: d = XORpu64(d, s); break;
#ifdef I386_ENABLE_SSE3
	case 0xd0: d = ADDSUBpf64(d, s); break;
	case 0x7c: d = HADDpf64(d, s); break;
	case 0x7d: d = HSUBpf64(d, s); break;
#endif
	default:
		cpu_setexc(cpu, 6, 0);
		return false;
	}
	fpu->xmm[dst] = d;
	return true;
}

bool fpu_xmmpf64_rm(FPU *fpu, void *cpu, int op, int dst, int seg, uword addr)
{
	UXMM d = fpu->xmm[dst];
	UXMM s;
	int n = 4;
	if (op == 0x12 || op == 0x16) n = 2;
	for (int i = 0; i < n; i++)
		if(!cpu_load32(cpu, seg, addr + 4 * i, s.u32v + i))
			return false;

	switch (op) {
	case 0x10: case 0x28: d = s; break; // MOVUPD/MOVAPD
	case 0x12: d.u32v[0] = s.u32v[0]; d.u32v[1] = s.u32v[1]; break; // MOVLPD
	case 0x16: d.u32v[2] = s.u32v[0]; d.u32v[3] = s.u32v[1]; break; // MOVHPD
	case 0x14: d = UNPCKLpf64(d, s); break;
	case 0x15: d = UNPCKHpf64(d, s); break;

	case 0x58: d = ADDpf64(d, s); break;
	case 0x5c: d = SUBpf64(d, s); break;
	case 0x59: d = MULpf64(d, s); break;
	case 0x5e: d = DIVpf64(d, s); break;
	case 0x51: d = SQRTpf64(d, s); break;
	case 0x5f: d = MAXpf64(d, s); break;
	case 0x5d: d = MINpf64(d, s); break;

	case 0x54: d = ANDpu64(d, s); break;
	case 0x55: d = ANDNpu64(d, s); break;
	case 0x56: d = ORpu64(d, s); break;
	case 0x57: d = XORpu64(d, s); break;
#ifdef I386_ENABLE_SSE3
	case 0xd0: d = ADDSUBpf64(d, s); break;
	case 0x7c: d = HADDpf64(d, s); break;
	case 0x7d: d = HSUBpf64(d, s); break;
#endif
	default:
		cpu_setexc(cpu, 6, 0);
		return false;
	}
	fpu->xmm[dst] = d;
	return true;
}

bool fpu_xmmpf64_rri(FPU *fpu, void *cpu, int op, int dst, int src, int imm)
{
	UXMM d = fpu->xmm[dst];
	UXMM s = fpu->xmm[src];
	switch (op) {
	case 0xc2:
		switch(imm) {
		case 0: d = CMPEQpf64(d, s); break;
		case 1: d = CMPLTpf64(d, s); break;
		case 2: d = CMPLEpf64(d, s); break;
		case 3: d = CMPNORDpf64(d, s); break;
		case 4: d = CMPNEQpf64(d, s); break;
		case 5: d = CMPNLTpf64(d, s); break;
		case 6: d = CMPNLEpf64(d, s); break;
		case 7: d = CMPORDpf64(d, s); break;
		default:
			cpu_setexc(cpu, 6, 0);
			return false;
		}
		break;
	case 0xc6:
		d = SHUFpf64(d, s, imm); break;
	default:
		cpu_setexc(cpu, 6, 0);
		return false;
	}
	fpu->xmm[dst] = d;
	return true;
}

bool fpu_xmmpf64_mr(FPU *fpu, void *cpu, int op, int src, int seg, uword addr)
{
	UXMM d;
	UXMM s = fpu->xmm[src];
	switch (op) {
	case 0x11: case 0x29: case 0x2b: d = s; break; // MOVUPD/MOVAPD/MOVNTPD
	case 0x13: d.u32v[0] = s.u32v[0]; d.u32v[1] = s.u32v[1]; break; // MOVLPD
	case 0x17: d.u32v[0] = s.u32v[2]; d.u32v[1] = s.u32v[3]; break; // MOVHPD
	default:
		cpu_setexc(cpu, 6, 0);
		return false;
	}
	int n = 4;
	if (op == 0x13 || op == 0x17) n = 2;
	for (int i = 0; i < n; i++)
		if (!cpu_store32(cpu, seg, addr + 4 * i, d.u32v[i]))
			return false;
	return true;
}

#define XMM_v8_v(us, NAME) \
static UXMM XMM_ ## NAME ## 8(UXMM v, UXMM w) \
{ \
	NAME ## _(8, v. us ## 8v[0], w. us ## 8v[0]) \
	NAME ## _(8, v. us ## 8v[1], w. us ## 8v[1]) \
	NAME ## _(8, v. us ## 8v[2], w. us ## 8v[2]) \
	NAME ## _(8, v. us ## 8v[3], w. us ## 8v[3]) \
	NAME ## _(8, v. us ## 8v[4], w. us ## 8v[4]) \
	NAME ## _(8, v. us ## 8v[5], w. us ## 8v[5]) \
	NAME ## _(8, v. us ## 8v[6], w. us ## 8v[6]) \
	NAME ## _(8, v. us ## 8v[7], w. us ## 8v[7]) \
	NAME ## _(8, v. us ## 8v[8], w. us ## 8v[8]) \
	NAME ## _(8, v. us ## 8v[9], w. us ## 8v[9]) \
	NAME ## _(8, v. us ## 8v[10], w. us ## 8v[10]) \
	NAME ## _(8, v. us ## 8v[11], w. us ## 8v[11]) \
	NAME ## _(8, v. us ## 8v[12], w. us ## 8v[12]) \
	NAME ## _(8, v. us ## 8v[13], w. us ## 8v[13]) \
	NAME ## _(8, v. us ## 8v[14], w. us ## 8v[14]) \
	NAME ## _(8, v. us ## 8v[15], w. us ## 8v[15]) \
	return v; \
}

#define XMM_v16_v(us, NAME) \
static UXMM XMM_ ## NAME ## 16(UXMM v, UXMM w) \
{ \
	NAME ## _(16, v. us ## 16v[0], w. us ## 16v[0]) \
	NAME ## _(16, v. us ## 16v[1], w. us ## 16v[1]) \
	NAME ## _(16, v. us ## 16v[2], w. us ## 16v[2]) \
	NAME ## _(16, v. us ## 16v[3], w. us ## 16v[3]) \
	NAME ## _(16, v. us ## 16v[4], w. us ## 16v[4]) \
	NAME ## _(16, v. us ## 16v[5], w. us ## 16v[5]) \
	NAME ## _(16, v. us ## 16v[6], w. us ## 16v[6]) \
	NAME ## _(16, v. us ## 16v[7], w. us ## 16v[7]) \
	return v; \
}

#define XMM_v32_v(us, NAME) \
static UXMM XMM_ ## NAME ## 32(UXMM v, UXMM w) \
{ \
	NAME ## _(32, v. us ## 32v[0], w. us ## 32v[0]) \
	NAME ## _(32, v. us ## 32v[1], w. us ## 32v[1]) \
	NAME ## _(32, v. us ## 32v[2], w. us ## 32v[2]) \
	NAME ## _(32, v. us ## 32v[3], w. us ## 32v[3]) \
	return v; \
}

#define XMM_v64_v(us, NAME) \
static UXMM XMM_ ## NAME ## 64(UXMM v, UXMM w) \
{ \
	NAME ## _(64, v. us ## 64v[0], w. us ## 64v[0]) \
	NAME ## _(64, v. us ## 64v[1], w. us ## 64v[1]) \
	return v; \
}

#define XMM_v_v(us, NAME) \
	XMM_v8_v(us, NAME) XMM_v16_v(us, NAME) XMM_v32_v(us, NAME)

#define XMM_v16_i(us, NAME) \
static UXMM XMM_ ## NAME ## 16(UXMM v, int b) \
{ \
	NAME ## _(16, v. us ## 16v[0], b) \
	NAME ## _(16, v. us ## 16v[1], b) \
	NAME ## _(16, v. us ## 16v[2], b) \
	NAME ## _(16, v. us ## 16v[3], b) \
	NAME ## _(16, v. us ## 16v[4], b) \
	NAME ## _(16, v. us ## 16v[5], b) \
	NAME ## _(16, v. us ## 16v[6], b) \
	NAME ## _(16, v. us ## 16v[7], b) \
	return v; \
}

#define XMM_v32_i(us, NAME) \
static UXMM XMM_ ## NAME ## 32(UXMM v, int b) \
{ \
	NAME ## _(32, v. us ## 32v[0], b) \
	NAME ## _(32, v. us ## 32v[1], b) \
	NAME ## _(32, v. us ## 32v[2], b) \
	NAME ## _(32, v. us ## 32v[3], b) \
	return v; \
}

#define XMM_v64_i(us, NAME) \
static UXMM XMM_ ## NAME ## 64(UXMM v, int b) \
{ \
	NAME ## _(64, v. us ## 64v[0], b) \
	NAME ## _(64, v. us ## 64v[1], b) \
	return v; \
}

#define XMM_v_i(us, NAME) \
	XMM_v16_i(us, NAME) XMM_v32_i(us, NAME) XMM_v64_i(us, NAME)

XMM_v_v(u, PADD)
XMM_v_v(u, PSUB)
XMM_v8_v(s, PADDS)
XMM_v16_v(s, PADDS)
XMM_v8_v(u, PADDUS)
XMM_v16_v(u, PADDUS)
XMM_v8_v(s, PSUBS)
XMM_v16_v(s, PSUBS)
XMM_v8_v(u, PSUBUS)
XMM_v16_v(u, PSUBUS)
XMM_v16_v(s, PMULL)
XMM_v16_v(s, PMULH)
XMM_v64_v(u, PAND)
XMM_v64_v(u, PANDN)
XMM_v64_v(u, POR)
XMM_v64_v(u, PXOR)
XMM_v_i(u, PSLL)
XMM_v16_i(s, PSRA)
XMM_v32_i(s, PSRA)
XMM_v_i(u, PSRL)
XMM_v_v(s, PCMPEQ)
XMM_v_v(s, PCMPGT)
XMM_v64_v(u, PADD)
XMM_v64_v(u, PSUB)

XMM_v8_v(u, PAVG)
XMM_v16_v(u, PAVG)
XMM_v8_v(u, PMAX) // u8
XMM_v16_v(s, PMAX) // s16
XMM_v8_v(u, PMIN) // u8
XMM_v16_v(s, PMIN) // s16
XMM_v16_v(u, PMULHU)
XMM_v8_v(u, _AD)

XMM_v_v(s, PSIGN)
XMM_v_v(s, PABS)

static UXMM XMM_PMADDWD(UXMM v, UXMM w)
{
	int t[8];
	t[0] = (int) v.s16v[0] * (int) w.s16v[0];
	t[1] = (int) v.s16v[1] * (int) w.s16v[1];
	t[2] = (int) v.s16v[2] * (int) w.s16v[2];
	t[3] = (int) v.s16v[3] * (int) w.s16v[3];
	t[4] = (int) v.s16v[4] * (int) w.s16v[4];
	t[5] = (int) v.s16v[5] * (int) w.s16v[5];
	t[6] = (int) v.s16v[6] * (int) w.s16v[6];
	t[7] = (int) v.s16v[7] * (int) w.s16v[7];

	UXMM r;
	r.s32v[0] = t[0] + t[1];
	r.s32v[1] = t[2] + t[3];
	r.s32v[2] = t[4] + t[5];
	r.s32v[3] = t[6] + t[7];
	return r;
}

static UXMM XMM_PACKSSWB(UXMM v, UXMM w)
{
	UXMM r;
	r.s8v[0] = sats8(v.s16v[0]);
	r.s8v[1] = sats8(v.s16v[1]);
	r.s8v[2] = sats8(v.s16v[2]);
	r.s8v[3] = sats8(v.s16v[3]);
	r.s8v[4] = sats8(v.s16v[4]);
	r.s8v[5] = sats8(v.s16v[5]);
	r.s8v[6] = sats8(v.s16v[6]);
	r.s8v[7] = sats8(v.s16v[7]);
	r.s8v[8] = sats8(w.s16v[0]);
	r.s8v[9] = sats8(w.s16v[1]);
	r.s8v[10] = sats8(w.s16v[2]);
	r.s8v[11] = sats8(w.s16v[3]);
	r.s8v[12] = sats8(w.s16v[4]);
	r.s8v[13] = sats8(w.s16v[5]);
	r.s8v[14] = sats8(w.s16v[6]);
	r.s8v[15] = sats8(w.s16v[7]);
	return r;
}

static UXMM XMM_PUNPCKLBW(UXMM v, UXMM w)
{
	UXMM r;
	r.s8v[0] = v.s8v[0];
	r.s8v[1] = w.s8v[0];
	r.s8v[2] = v.s8v[1];
	r.s8v[3] = w.s8v[1];
	r.s8v[4] = v.s8v[2];
	r.s8v[5] = w.s8v[2];
	r.s8v[6] = v.s8v[3];
	r.s8v[7] = w.s8v[3];
	r.s8v[8] = v.s8v[4];
	r.s8v[9] = w.s8v[4];
	r.s8v[10] = v.s8v[5];
	r.s8v[11] = w.s8v[5];
	r.s8v[12] = v.s8v[6];
	r.s8v[13] = w.s8v[6];
	r.s8v[14] = v.s8v[7];
	r.s8v[15] = w.s8v[7];
	return r;
}

static UXMM XMM_PUNPCKLWD(UXMM v, UXMM w)
{
	UXMM r;
	r.s16v[0] = v.s16v[0];
	r.s16v[1] = w.s16v[0];
	r.s16v[2] = v.s16v[1];
	r.s16v[3] = w.s16v[1];
	r.s16v[4] = v.s16v[2];
	r.s16v[5] = w.s16v[2];
	r.s16v[6] = v.s16v[3];
	r.s16v[7] = w.s16v[3];
	return r;
}

static UXMM XMM_PUNPCKLDQ(UXMM v, UXMM w)
{
	UXMM r;
	r.s32v[0] = v.s32v[0];
	r.s32v[1] = w.s32v[0];
	r.s32v[2] = v.s32v[1];
	r.s32v[3] = w.s32v[1];
	return r;
}

static UXMM XMM_PUNPCKLQDQ(UXMM v, UXMM w)
{
	UXMM r;
	r.s64v[0] = v.s64v[0];
	r.s64v[1] = w.s64v[0];
	return r;
}

static UXMM XMM_PUNPCKHBW(UXMM v, UXMM w)
{
	UXMM r;
	r.s8v[0] = v.s8v[8];
	r.s8v[1] = w.s8v[8];
	r.s8v[2] = v.s8v[9];
	r.s8v[3] = w.s8v[9];
	r.s8v[4] = v.s8v[10];
	r.s8v[5] = w.s8v[10];
	r.s8v[6] = v.s8v[11];
	r.s8v[7] = w.s8v[11];
	r.s8v[8] = v.s8v[12];
	r.s8v[9] = w.s8v[12];
	r.s8v[10] = v.s8v[13];
	r.s8v[11] = w.s8v[13];
	r.s8v[12] = v.s8v[14];
	r.s8v[13] = w.s8v[14];
	r.s8v[14] = v.s8v[15];
	r.s8v[15] = w.s8v[15];
	return r;
}

static UXMM XMM_PUNPCKHWD(UXMM v, UXMM w)
{
	UXMM r;
	r.s16v[0] = v.s16v[4];
	r.s16v[1] = w.s16v[4];
	r.s16v[2] = v.s16v[5];
	r.s16v[3] = w.s16v[5];
	r.s16v[4] = v.s16v[6];
	r.s16v[5] = w.s16v[6];
	r.s16v[6] = v.s16v[7];
	r.s16v[7] = w.s16v[7];
	return r;
}

static UXMM XMM_PUNPCKHDQ(UXMM v, UXMM w)
{
	UXMM r;
	r.s32v[0] = v.s32v[2];
	r.s32v[1] = w.s32v[2];
	r.s32v[2] = v.s32v[3];
	r.s32v[3] = w.s32v[3];
	return r;
}

static UXMM XMM_PUNPCKHQDQ(UXMM v, UXMM w)
{
	UXMM r;
	r.s64v[0] = v.s64v[1];
	r.s64v[1] = w.s64v[1];
	return r;
}

static UXMM XMM_PACKUSWB(UXMM v, UXMM w)
{
	UXMM r;
	r.u8v[0] = satu8(v.s16v[0]);
	r.u8v[1] = satu8(v.s16v[1]);
	r.u8v[2] = satu8(v.s16v[2]);
	r.u8v[3] = satu8(v.s16v[3]);
	r.u8v[4] = satu8(v.s16v[4]);
	r.u8v[5] = satu8(v.s16v[5]);
	r.u8v[6] = satu8(v.s16v[6]);
	r.u8v[7] = satu8(v.s16v[7]);
	r.u8v[8] = satu8(w.s16v[0]);
	r.u8v[9] = satu8(w.s16v[1]);
	r.u8v[10] = satu8(w.s16v[2]);
	r.u8v[11] = satu8(w.s16v[3]);
	r.u8v[12] = satu8(w.s16v[4]);
	r.u8v[13] = satu8(w.s16v[5]);
	r.u8v[14] = satu8(w.s16v[6]);
	r.u8v[15] = satu8(w.s16v[7]);
	return r;
}

static UXMM XMM_PACKSSDW(UXMM v, UXMM w)
{
	UXMM r;
	r.s16v[0] = sats16(v.s32v[0]);
	r.s16v[1] = sats16(v.s32v[1]);
	r.s16v[2] = sats16(v.s32v[2]);
	r.s16v[3] = sats16(v.s32v[3]);
	r.s16v[4] = sats16(w.s32v[0]);
	r.s16v[5] = sats16(w.s32v[1]);
	r.s16v[6] = sats16(w.s32v[2]);
	r.s16v[7] = sats16(w.s32v[3]);
	return r;
}

static UXMM XMM_PSADBW(UXMM a, UXMM b)
{
	UXMM v;
	v = XMM__AD8(a, b);
	uint32_t s = 0;
	s += v.u8v[0];
	s += v.u8v[1];
	s += v.u8v[2];
	s += v.u8v[3];
	s += v.u8v[4];
	s += v.u8v[5];
	s += v.u8v[6];
	s += v.u8v[7];
	a.u64v[0] = s;

	s = 0;
	s += v.u8v[8];
	s += v.u8v[9];
	s += v.u8v[10];
	s += v.u8v[11];
	s += v.u8v[12];
	s += v.u8v[13];
	s += v.u8v[14];
	s += v.u8v[15];
	a.u64v[1] = s;
	return a;
}

static UXMM XMM_PMULUDQ(UXMM a, UXMM b)
{
	UXMM v;
	v.u64v[0] = (a.u64v[0] & 0xffffffff) * (b.u64v[0] & 0xffffffff);
	v.u64v[1] = (a.u64v[1] & 0xffffffff) * (b.u64v[1] & 0xffffffff);
	return v;
}

static UXMM XMM_PSHUFB(UXMM v, UXMM w)
{
	UXMM r;
	for (int i = 0; i < 16; i++) {
		if (w.s8v[i] < 0)
			r.u8v[i] = 0;
		else
			r.u8v[i] = v.u8v[w.s8v[i] & 15];
	}
	return r;
}

static UXMM XMM_PHADDW(UXMM v, UXMM w)
{
	UXMM r;
	r.u16v[0] = v.u16v[0] + v.u16v[1];
	r.u16v[1] = v.u16v[2] + v.u16v[3];
	r.u16v[2] = v.u16v[4] + v.u16v[5];
	r.u16v[3] = v.u16v[6] + v.u16v[7];
	r.u16v[4] = w.u16v[0] + w.u16v[1];
	r.u16v[5] = w.u16v[2] + w.u16v[3];
	r.u16v[6] = w.u16v[4] + w.u16v[5];
	r.u16v[7] = w.u16v[6] + w.u16v[7];
	return r;
}

static UXMM XMM_PHADDD(UXMM v, UXMM w)
{
	UXMM r;
	r.u32v[0] = v.u32v[0] + v.u32v[1];
	r.u32v[1] = v.u32v[2] + v.u32v[3];
	r.u32v[2] = w.u32v[0] + w.u32v[1];
	r.u32v[3] = w.u32v[2] + w.u32v[3];
	return r;
}

static UXMM XMM_PHADDSW(UXMM v, UXMM w)
{
	UXMM r;
	r.u16v[0] = sats16((int) v.u16v[0] + (int) v.u16v[1]);
	r.u16v[1] = sats16((int) v.u16v[2] + (int) v.u16v[3]);
	r.u16v[2] = sats16((int) v.u16v[4] + (int) v.u16v[5]);
	r.u16v[3] = sats16((int) v.u16v[6] + (int) v.u16v[7]);
	r.u16v[4] = sats16((int) w.u16v[0] + (int) w.u16v[1]);
	r.u16v[5] = sats16((int) w.u16v[2] + (int) w.u16v[3]);
	r.u16v[6] = sats16((int) w.u16v[4] + (int) w.u16v[5]);
	r.u16v[7] = sats16((int) w.u16v[6] + (int) w.u16v[7]);
	return r;
}

static UXMM XMM_PMADDUBSW(UXMM v, UXMM w)
{
	UXMM r;
	r.s16v[0] = sats16((int) v.u8v[0] * (int) w.u8v[0] + (int) v.u8v[1] * (int) w.u8v[1]);
	r.s16v[1] = sats16((int) v.u8v[2] * (int) w.u8v[2] + (int) v.u8v[3] * (int) w.u8v[3]);
	r.s16v[2] = sats16((int) v.u8v[4] * (int) w.u8v[4] + (int) v.u8v[5] * (int) w.u8v[5]);
	r.s16v[3] = sats16((int) v.u8v[6] * (int) w.u8v[6] + (int) v.u8v[7] * (int) w.u8v[7]);
	r.s16v[4] = sats16((int) v.u8v[8] * (int) w.u8v[8] + (int) v.u8v[9] * (int) w.u8v[9]);
	r.s16v[5] = sats16((int) v.u8v[10] * (int) w.u8v[10] + (int) v.u8v[11] * (int) w.u8v[11]);
	r.s16v[6] = sats16((int) v.u8v[12] * (int) w.u8v[12] + (int) v.u8v[13] * (int) w.u8v[13]);
	r.s16v[7] = sats16((int) v.u8v[14] * (int) w.u8v[14] + (int) v.u8v[15] * (int) w.u8v[15]);
	return r;
}

static UXMM XMM_PHSUBW(UXMM v, UXMM w)
{
	UXMM r;
	r.u16v[0] = v.u16v[0] - v.u16v[1];
	r.u16v[1] = v.u16v[2] - v.u16v[3];
	r.u16v[2] = v.u16v[4] - v.u16v[5];
	r.u16v[3] = v.u16v[6] - v.u16v[7];
	r.u16v[4] = w.u16v[0] - w.u16v[1];
	r.u16v[5] = w.u16v[2] - w.u16v[3];
	r.u16v[6] = w.u16v[4] - w.u16v[5];
	r.u16v[7] = w.u16v[6] - w.u16v[7];
	return r;
}

static UXMM XMM_PHSUBD(UXMM v, UXMM w)
{
	UXMM r;
	r.u32v[0] = v.u32v[0] - v.u32v[1];
	r.u32v[1] = v.u32v[2] - v.u32v[3];
	r.u32v[2] = w.u32v[0] - w.u32v[1];
	r.u32v[3] = w.u32v[2] - w.u32v[3];
	return r;
}

static UXMM XMM_PHSUBSW(UXMM v, UXMM w)
{
	UXMM r;
	r.u16v[0] = sats16((int) v.u16v[0] - (int) v.u16v[1]);
	r.u16v[1] = sats16((int) v.u16v[2] - (int) v.u16v[3]);
	r.u16v[2] = sats16((int) v.u16v[4] - (int) v.u16v[5]);
	r.u16v[3] = sats16((int) v.u16v[6] - (int) v.u16v[7]);
	r.u16v[4] = sats16((int) w.u16v[0] - (int) w.u16v[1]);
	r.u16v[5] = sats16((int) w.u16v[2] - (int) w.u16v[3]);
	r.u16v[6] = sats16((int) w.u16v[4] - (int) w.u16v[5]);
	r.u16v[7] = sats16((int) w.u16v[6] - (int) w.u16v[7]);
	return r;
}

static UXMM XMM_PMULHRSW(UXMM v, UXMM w)
{
	UXMM r;
	r.s16v[0] = ((((int) v.s16v[0] * (int) w.s16v[0]) >> 14) + 1) >> 1;
	r.s16v[1] = ((((int) v.s16v[1] * (int) w.s16v[1]) >> 14) + 1) >> 1;
	r.s16v[2] = ((((int) v.s16v[2] * (int) w.s16v[2]) >> 14) + 1) >> 1;
	r.s16v[3] = ((((int) v.s16v[3] * (int) w.s16v[3]) >> 14) + 1) >> 1;
	r.s16v[4] = ((((int) v.s16v[4] * (int) w.s16v[4]) >> 14) + 1) >> 1;
	r.s16v[5] = ((((int) v.s16v[5] * (int) w.s16v[5]) >> 14) + 1) >> 1;
	r.s16v[6] = ((((int) v.s16v[6] * (int) w.s16v[6]) >> 14) + 1) >> 1;
	r.s16v[7] = ((((int) v.s16v[7] * (int) w.s16v[7]) >> 14) + 1) >> 1;
	return r;
}

bool fpu_xmm_rr(FPU *fpu, void *cpu, int op, int dst, int src)
{
	UXMM d = fpu->xmm[dst];
	UXMM s = fpu->xmm[src];
	switch (op) {
	case 0x60: d = XMM_PUNPCKLBW(d, s); break;
	case 0x61: d = XMM_PUNPCKLWD(d, s); break;
	case 0x62: d = XMM_PUNPCKLDQ(d, s); break;
	case 0x63: d = XMM_PACKSSWB(d, s); break;
	case 0x64: d = XMM_PCMPGT8(d, s); break;
	case 0x65: d = XMM_PCMPGT16(d, s); break;
	case 0x66: d = XMM_PCMPGT32(d, s); break;
	case 0x67: d = XMM_PACKUSWB(d, s); break;
	case 0x68: d = XMM_PUNPCKHBW(d, s); break;
	case 0x69: d = XMM_PUNPCKHWD(d, s); break;
	case 0x6a: d = XMM_PUNPCKHDQ(d, s); break;
	case 0x6b: d = XMM_PACKSSDW(d, s); break;
	case 0x74: d = XMM_PCMPEQ8(d, s); break;
	case 0x75: d = XMM_PCMPEQ16(d, s); break;
	case 0x76: d = XMM_PCMPEQ32(d, s); break;
	case 0xd1: d = XMM_PSRL16(d, s.u32v[0]); break;
	case 0xd2: d = XMM_PSRL32(d, s.u32v[0]); break;
	case 0xd3: d = XMM_PSRL64(d, s.u32v[0]); break;
	case 0xe1: d = XMM_PSRA16(d, s.u32v[0]); break;
	case 0xe2: d = XMM_PSRA32(d, s.u32v[0]); break;
	case 0xf1: d = XMM_PSLL16(d, s.u32v[0]); break;
	case 0xf2: d = XMM_PSLL32(d, s.u32v[0]); break;
	case 0xf3: d = XMM_PSLL64(d, s.u32v[0]); break;
	case 0xd5: d = XMM_PMULL16(d, s); break;
	case 0xe5: d = XMM_PMULH16(d, s); break;
	case 0xf5: d = XMM_PMADDWD(d, s); break;
	case 0xd8: d = XMM_PSUBUS8(d, s); break;
	case 0xd9: d = XMM_PSUBUS16(d, s); break;
	case 0xe8: d = XMM_PSUBS8(d, s); break;
	case 0xe9: d = XMM_PSUBS16(d, s); break;
	case 0xf8: d = XMM_PSUB8(d, s); break;
	case 0xf9: d = XMM_PSUB16(d, s); break;
	case 0xfa: d = XMM_PSUB32(d, s); break;
	case 0xdb: d = XMM_PAND64(d, s); break;
	case 0xeb: d = XMM_POR64(d, s); break;
	case 0xdc: d = XMM_PADDUS8(d, s); break;
	case 0xdd: d = XMM_PADDUS16(d, s); break;
	case 0xec: d = XMM_PADDS8(d, s); break;
	case 0xed: d = XMM_PADDS16(d, s); break;
	case 0xfc: d = XMM_PADD8(d, s); break;
	case 0xfd: d = XMM_PADD16(d, s); break;
	case 0xfe: d = XMM_PADD32(d, s); break;
	case 0xdf: d = XMM_PANDN64(d, s); break;
	case 0xef: d = XMM_PXOR64(d, s); break;
	case 0x6f: case 0x7f: d = s; break;
	case 0xda: d = XMM_PMIN8(d, s); break;
	case 0xde: d = XMM_PMAX8(d, s); break;
	case 0xe0: d = XMM_PAVG8(d, s); break;
	case 0xe3: d = XMM_PAVG16(d, s); break;
	case 0xe4: d = XMM_PMULHU16(d, s); break;
	case 0xea: d = XMM_PMIN16(d, s); break;
	case 0xee: d = XMM_PMAX16(d, s); break;
	case 0xf6: d = XMM_PSADBW(d, s); break;
	case 0x6c: d = XMM_PUNPCKLQDQ(d, s); break;
	case 0x6d: d = XMM_PUNPCKHQDQ(d, s); break;
	case 0xd4: d = XMM_PADD64(d, s); break;
	case 0xfb: d = XMM_PSUB64(d, s); break;
	case 0xf4: d = XMM_PMULUDQ(d, s); break;
#ifdef I386_ENABLE_SSSE3
	/* SSSE3 */
	case 0x100: d = XMM_PSHUFB(d, s); break;
	case 0x101: d = XMM_PHADDW(d, s); break;
	case 0x102: d = XMM_PHADDD(d, s); break;
	case 0x103: d = XMM_PHADDSW(d, s); break;
	case 0x104: d = XMM_PMADDUBSW(d, s); break;
	case 0x105: d = XMM_PHSUBW(d, s); break;
	case 0x106: d = XMM_PHSUBD(d, s); break;
	case 0x107: d = XMM_PHSUBSW(d, s); break;
	case 0x108: d = XMM_PSIGN8(d, s); break;
	case 0x109: d = XMM_PSIGN16(d, s); break;
	case 0x10a: d = XMM_PSIGN32(d, s); break;
	case 0x10b: d = XMM_PMULHRSW(d, s); break;
	case 0x11c: d = XMM_PABS8(d, s); break;
	case 0x11d: d = XMM_PABS16(d, s); break;
	case 0x11e: d = XMM_PABS32(d, s); break;
#endif
	default:
		cpu_setexc(cpu, 6, 0);
		return false;
	}
	fpu->xmm[dst] = d;
	return true;
}

bool fpu_xmm_rm(FPU *fpu, void *cpu, int op, int dst, int seg, uword addr)
{
	UXMM d = fpu->xmm[dst];
	UXMM s;
	for (int i = 0; i < 4; i++)
		if(!cpu_load32(cpu, seg, addr + 4 * i, s.u32v + i))
			return false;

	switch (op) {
	case 0x60: d = XMM_PUNPCKLBW(d, s); break;
	case 0x61: d = XMM_PUNPCKLWD(d, s); break;
	case 0x62: d = XMM_PUNPCKLDQ(d, s); break;
	case 0x63: d = XMM_PACKSSWB(d, s); break;
	case 0x64: d = XMM_PCMPGT8(d, s); break;
	case 0x65: d = XMM_PCMPGT16(d, s); break;
	case 0x66: d = XMM_PCMPGT32(d, s); break;
	case 0x67: d = XMM_PACKUSWB(d, s); break;
	case 0x68: d = XMM_PUNPCKHBW(d, s); break;
	case 0x69: d = XMM_PUNPCKHWD(d, s); break;
	case 0x6a: d = XMM_PUNPCKHDQ(d, s); break;
	case 0x6b: d = XMM_PACKSSDW(d, s); break;
	case 0x74: d = XMM_PCMPEQ8(d, s); break;
	case 0x75: d = XMM_PCMPEQ16(d, s); break;
	case 0x76: d = XMM_PCMPEQ32(d, s); break;
	case 0xd1: d = XMM_PSRL16(d, s.u32v[0]); break;
	case 0xd2: d = XMM_PSRL32(d, s.u32v[0]); break;
	case 0xd3: d = XMM_PSRL64(d, s.u32v[0]); break;
	case 0xe1: d = XMM_PSRA16(d, s.u32v[0]); break;
	case 0xe2: d = XMM_PSRA32(d, s.u32v[0]); break;
	case 0xf1: d = XMM_PSLL16(d, s.u32v[0]); break;
	case 0xf2: d = XMM_PSLL32(d, s.u32v[0]); break;
	case 0xf3: d = XMM_PSLL64(d, s.u32v[0]); break;
	case 0xd5: d = XMM_PMULL16(d, s); break;
	case 0xe5: d = XMM_PMULH16(d, s); break;
	case 0xf5: d = XMM_PMADDWD(d, s); break;
	case 0xd8: d = XMM_PSUBUS8(d, s); break;
	case 0xd9: d = XMM_PSUBUS16(d, s); break;
	case 0xe8: d = XMM_PSUBS8(d, s); break;
	case 0xe9: d = XMM_PSUBS16(d, s); break;
	case 0xf8: d = XMM_PSUB8(d, s); break;
	case 0xf9: d = XMM_PSUB16(d, s); break;
	case 0xfa: d = XMM_PSUB32(d, s); break;
	case 0xdb: d = XMM_PAND64(d, s); break;
	case 0xeb: d = XMM_POR64(d, s); break;
	case 0xdc: d = XMM_PADDUS8(d, s); break;
	case 0xdd: d = XMM_PADDUS16(d, s); break;
	case 0xec: d = XMM_PADDS8(d, s); break;
	case 0xed: d = XMM_PADDS16(d, s); break;
	case 0xfc: d = XMM_PADD8(d, s); break;
	case 0xfd: d = XMM_PADD16(d, s); break;
	case 0xfe: d = XMM_PADD32(d, s); break;
	case 0xdf: d = XMM_PANDN64(d, s); break;
	case 0xef: d = XMM_PXOR64(d, s); break;
	case 0x6f: d = s; break; // MOVDQA/MOVDQU
	case 0xda: d = XMM_PMIN8(d, s); break;
	case 0xde: d = XMM_PMAX8(d, s); break;
	case 0xe0: d = XMM_PAVG8(d, s); break;
	case 0xe3: d = XMM_PAVG16(d, s); break;
	case 0xe4: d = XMM_PMULHU16(d, s); break;
	case 0xea: d = XMM_PMIN16(d, s); break;
	case 0xee: d = XMM_PMAX16(d, s); break;
	case 0xf6: d = XMM_PSADBW(d, s); break;
	case 0x6c: d = XMM_PUNPCKLQDQ(d, s); break;
	case 0x6d: d = XMM_PUNPCKHQDQ(d, s); break;
	case 0xd4: d = XMM_PADD64(d, s); break;
	case 0xfb: d = XMM_PSUB64(d, s); break;
	case 0xf4: d = XMM_PMULUDQ(d, s); break;
#ifdef I386_ENABLE_SSSE3
	/* SSSE3 */
	case 0x100: d = XMM_PSHUFB(d, s); break;
	case 0x101: d = XMM_PHADDW(d, s); break;
	case 0x102: d = XMM_PHADDD(d, s); break;
	case 0x103: d = XMM_PHADDSW(d, s); break;
	case 0x104: d = XMM_PMADDUBSW(d, s); break;
	case 0x105: d = XMM_PHSUBW(d, s); break;
	case 0x106: d = XMM_PHSUBD(d, s); break;
	case 0x107: d = XMM_PHSUBSW(d, s); break;
	case 0x108: d = XMM_PSIGN8(d, s); break;
	case 0x109: d = XMM_PSIGN16(d, s); break;
	case 0x10a: d = XMM_PSIGN32(d, s); break;
	case 0x10b: d = XMM_PMULHRSW(d, s); break;
	case 0x11c: d = XMM_PABS8(d, s); break;
	case 0x11d: d = XMM_PABS16(d, s); break;
	case 0x11e: d = XMM_PABS32(d, s); break;
#endif
	default:
		cpu_setexc(cpu, 6, 0);
		return false;
	}
	fpu->xmm[dst] = d;
	return true;
}

bool fpu_xmm_ri(FPU *fpu, void *cpu, int op, int func, int dst, int imm)
{
	UXMM v = fpu->xmm[dst];
	switch (op) {
	case 0x71:
		switch (func) {
		case 2: v = XMM_PSRL16(v, imm); break;
		case 4: v = XMM_PSRA16(v, imm); break;
		case 6: v = XMM_PSLL16(v, imm); break;
		default:
			cpu_setexc(cpu, 6, 0);
			return false;
		}
		break;
	case 0x72:
		switch (func) {
		case 2: v = XMM_PSRL32(v, imm); break;
		case 4: v = XMM_PSRA32(v, imm); break;
		case 6: v = XMM_PSLL32(v, imm); break;
		default:
			cpu_setexc(cpu, 6, 0);
			return false;
		}
		break;
	case 0x73:
		switch (func) {
		case 2: v = XMM_PSRL64(v, imm); break;
		case 3:
			imm *= 8;
			if (imm >= 128) {
				v.u64v[0] = 0;
				v.u64v[1] = 0;
			} else if (imm >= 64) {
				v.u64v[0] = v.u64v[1] >> (imm - 64);
				v.u64v[1] = 0;
			} else {
				v.u64v[0] = (v.u64v[0] >> imm) |
					(v.u64v[1] << (64 - imm));
				v.u64v[1] >>= imm;
			}
			break;
		case 6: v = XMM_PSLL64(v, imm); break;
		case 7:
			imm *= 8;
			if (imm >= 128) {
				v.u64v[1] = 0;
				v.u64v[0] = 0;
			} else if (imm >= 64) {
				v.u64v[1] = v.u64v[0] << (imm - 64);
				v.u64v[0] = 0;
			} else {
				v.u64v[1] = (v.u64v[0] >> (64 - imm)) |
					     (v.u64v[1] << imm);
				v.u64v[0] <<= imm;
			}
			break;
		default:
			cpu_setexc(cpu, 6, 0);
			return false;
		}
		break;
	default:
		cpu_setexc(cpu, 6, 0);
		return false;
	}
	fpu->xmm[dst] = v;
	return true;
}

bool fpu_xmm_mr(FPU *fpu, void *cpu, int op, int src, int seg, uword addr)
{
	if (op == 0x7f || op == 0xe7) { // MOVDQA/MOVNTDQ
		UXMM v = fpu->xmm[src];
		for (int i = 0; i < 4; i++)
			if (!cpu_store32(cpu, seg, addr + 4 * i, v.u32v[i]))
				return false;
		return true;
	}
	cpu_setexc(cpu, 6, 0);
	return false;
}
#endif /* I386_ENABLE_SSE2 */
#endif /* SIMD_fpu_c */
#undef MODSIB
#undef IN_AMD64
#undef code16
#undef adsz16
