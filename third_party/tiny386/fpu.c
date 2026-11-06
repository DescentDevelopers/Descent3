// incomplete x87 emulation, use at your own risk!
// no exception, no tag word, no float80
#if defined(USE_AMD64)
#include "amd64.h"
#else
#include "i386.h"
#endif
#include "fpu.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

typedef struct {
	uint32_t mant0;
	uint32_t mant1;
	uint16_t high;
} F80;

union union64 {
	double f;
	uint64_t i;
};

union union32 {
	float f;
	uint32_t i;
};

#ifndef USE_FLOAT80
#include <math.h>
#define BIAS80 16383
#define BIAS64 1023
static F80 tof80(double val)
{
	union union64 u = { .f = val };
	uint64_t v = u.i;
	int sign = v >> 63;
	int exp = (v >> 52) & ((1 << 11) - 1);
	uint64_t mant80 = (v & (((uint64_t) 1 << 52) - 1)) << 11;
	if (exp == 0) {
		// zero or subnormal
		if (mant80 != 0) {
			// subnormal
			int shift = 64 - __builtin_ffsll(mant80);
			mant80 <<= shift;
			exp += BIAS80 - BIAS64 + 1 - shift;
		}
	} else if (exp == (1 << 11) - 1) {
		// inf or nan
		mant80 |= (uint64_t) 1 << 63;
		exp = 0x7fff;
	} else {
		// normal
		mant80 |= (uint64_t) 1 << 63;
		exp += BIAS80 - BIAS64;
	}

	F80 res;
	res.high = (sign << 15) | (uint16_t) exp;
	res.mant1 = mant80 >> 32;
	res.mant0 = mant80;
	return res;
}

static double fromf80(F80 f80)
{
	int sign = f80.high >> 15;
	int exp = f80.high & 0x7fff;
	uint64_t mant80 = f80.mant1;
	mant80 = (mant80 << 32) | f80.mant0;

	uint64_t mant64;
	if (exp == 0) {
		// f80 subnormal/zero => f64 zero
		mant64 = 0;
	} else if (exp == 0x7fff) {
		// inf or nan
		exp = (1 << 11) - 1;
		mant64 = mant80 >> 11;
		if (mant64 == 0 && mant80 != 0)
			mant64 = 1;
	} else {
		// normal
		exp += BIAS64 - BIAS80;
		if (exp <= -52) {
			// => f64 zero
			exp = 0;
			mant64 = 0;
		} else if (exp <= 0) {
			// => f64 subnormal
			mant64 = mant80 >> (12 - exp);
			exp = 0;
		} else if (exp >= (1 << 11) - 1) {
			// => f64 inf
			exp = (1 << 11) - 1;
			mant64 = 0;
		} else {
			mant64 = mant80 >> 11;
			mant64 &= ((uint64_t) 1 << 52) - 1;
		}
	}

	uint64_t res = ((uint64_t) sign << 63) | ((uint64_t) exp << 52) | mant64;
	union union64 u = { .i = res };
	return u.f;
}
#else
// if USE_FLOAT80 is defined, we have native f80 support (e.g. x87)
#include <tgmath.h>

union union80 {
	__float80 f;
	F80 f80;
};

static F80 tof80(long double val)
{
	union union80 u = { .f = val };
	return u.f80;
}

static long double fromf80(F80 f80)
{
	union union80 u = { .f80 = f80 };
	return u.f;
}

#define double long double
#define sincos sincosl
#endif

typedef union {
	uint32_t u32v[4];
	uint64_t u64v[2];
	float f32v[4];
	double f64v[2];

	uint16_t u16v[8];
	uint8_t u8v[16];
	int64_t s64v[2];
	int32_t s32v[4];
	int16_t s16v[8];
	int8_t s8v[16];
} UXMM;

struct FPU {
	u16 cw, sw; // TODO: tag word
	unsigned int top;

	double st[8];
	F80 rawst[8];
	u8 rawtagr;
	u8 rawtagw;

#ifdef I386_ENABLE_SSE
	u32 mxcsr;
#ifndef USE_AMD64
	UXMM xmm[8];
#else
	UXMM xmm[16];
#endif
#endif
};

static u16 getsw(FPU *fpu)
{
	return (fpu->sw & 0xc7ff) | (fpu->top << 11);
}

static void setsw(FPU *fpu, u16 sw)
{
	fpu->sw = sw;
	fpu->top = (sw >> 11) & 7;
}

FPU *fpu_new()
{
	FPU *fpu = malloc(sizeof(FPU));
	memset(fpu, 0, sizeof(FPU));
	fpu->cw = 0x40;
	return fpu;
}

void fpu_delete(FPU *fpu)
{
	free(fpu);
}

static double fpget(FPU *fpu, int i)
{
	unsigned int idx = (fpu->top + i) & 7;
	unsigned int mask = 1 << idx;
	if (!(fpu->rawtagr & mask) && !(fpu->rawtagw & mask)) {
		fpu->st[idx] = fromf80(fpu->rawst[idx]);
		fpu->rawtagr |= mask;
	}
	return fpu->st[idx];
}

static void fpset(FPU *fpu, int i, double val)
{
	unsigned int idx = (fpu->top + i) & 7;
	unsigned int mask = 1 << idx;
	fpu->st[idx] = val;
	fpu->rawtagw |= mask;
}

static void fppush(FPU *fpu, double val)
{
	fpu->top = (fpu->top - 1) & 7;
	// set tw
	fpset(fpu, 0, val);
}

/* FILD m64 keeps the integer's exact 80 bits in rawst (as FRSTOR does), and FISTP m64 stores the integer from
 * there while no instruction has written the register since: through a double (53 bits) 64-bit integers lost
 * their low bits, and memory moved 8 bytes at a time with FILD/FISTP came out changed. */
static void fppushi64(FPU *fpu, int64_t v)
{
	F80 r = { 0, 0, 0 };
	if (v != 0) {
		uint64_t m = v < 0 ? -(uint64_t) v : (uint64_t) v;
		int lz = __builtin_clzll(m);
		m <<= lz;
		r.high = (v < 0 ? 0x8000 : 0) | (16383 + 63 - lz);
		r.mant1 = m >> 32;
		r.mant0 = m;
	}
	fpu->top = (fpu->top - 1) & 7;
	fpu->rawst[fpu->top] = r;
	fpu->rawtagr &= ~(1 << fpu->top);
	fpu->rawtagw &= ~(1 << fpu->top);
}

static bool fpgeti64(FPU *fpu, int64_t *res)
{
	if (fpu->rawtagw & (1 << fpu->top))
		return false;
	F80 f = fpu->rawst[fpu->top];
	int exp = f.high & 0x7fff, neg = f.high >> 15;
	uint64_t m = (uint64_t) f.mant1 << 32 | f.mant0;
	if (m == 0 && exp == 0) {
		*res = 0;
		return true;
	}
	if (!(m >> 63) || exp < 16383 || exp > 16383 + 63)
		return false;
	int shift = 16383 + 63 - exp;
	if (shift && (m << (64 - shift)))	/* not an integer */
		return false;
	uint64_t u = m >> shift;
	if (u >> 63 && (!neg || u != (uint64_t) 1 << 63))	/* beyond the range */
		return false;
	*res = neg ? (int64_t) -u : (int64_t) u;
	return true;
}

static int swapbits(int v, int i, int j)
{
	if (((v >> i) ^ (v >> j)) & 1) {
		return v ^ ((1 << i) | (1 << j));
	}
	return v;
}

static void fpxch(FPU *fpu, int i, int j)
{
	unsigned int ii = (fpu->top + i) & 7;
	unsigned int jj = (fpu->top + j) & 7;
	F80 f80;
	double f64;
	f80 = fpu->rawst[ii];
	f64 = fpu->st[ii];
	fpu->rawst[ii] = fpu->rawst[jj];
	fpu->st[ii] = fpu->st[jj];
	fpu->rawst[jj] = f80;
	fpu->st[jj] = f64;
	fpu->rawtagr = swapbits(fpu->rawtagr, ii, jj);
	fpu->rawtagw = swapbits(fpu->rawtagw, ii, jj);
}

static void fppop(FPU *fpu)
{
	fpu->top = (fpu->top + 1) & 7;
	// set tw
}

static bool fploadf32(void *cpu, int seg, uword addr, double *res)
{
	union union32 u;
	if(!cpu_load32(cpu, seg, addr, &u.i))
		return false;
	*res = u.f;
	return true;
}

static bool fploadf64(void *cpu, int seg, uword addr, double *res)
{
	u32 v1, v2;
	if(!cpu_load32(cpu, seg, addr, &v1))
		return false;
	if(!cpu_load32(cpu, seg, addr + 4, &v2))
		return false;
	uint64_t v = v2;
	v = (v << 32) | v1;
	union union64 u = { .i = v };
	*res = u.f;
	return true;
}

static bool fploadf80(void *cpu, int seg, uword addr, double *res)
{
	F80 f80;
	if(!cpu_load32(cpu, seg, addr, &f80.mant0))
		return false;
	if(!cpu_load32(cpu, seg, addr + 4, &f80.mant1))
		return false;
	if(!cpu_load16(cpu, seg, addr + 8, &f80.high))
		return false;
	*res = fromf80(f80);
	return true;
}

static bool fploadi16(void *cpu, int seg, uword addr, double *res)
{
	u16 v;
	if(!cpu_load16(cpu, seg, addr, &v))
		return false;
	*res = (s16) v;
	return true;
}

static bool fploadi32(void *cpu, int seg, uword addr, double *res)
{
	u32 v;
	if(!cpu_load32(cpu, seg, addr, &v))
		return false;
	*res = (s32) v;
	return true;
}

static int bcd100(u8 b)
{
	return b - (6 * (b >> 4));
}

static bool fploadbcd(void *cpu, int seg, uword addr, double *res)
{
	u32 lo, mi;
	u16 hi;
	if(!cpu_load32(cpu, seg, addr, &lo))
		return false;
	if(!cpu_load32(cpu, seg, addr + 4, &mi))
		return false;
	if(!cpu_load16(cpu, seg, addr + 8, &hi))
		return false;

	uint64_t val = 0;
	int sign = hi & 0x8000;
	hi &= 0x7FFF;
	uint64_t base = 1;
	for (int i = 0; i < 4; i++) {
		val += base * bcd100(lo);
		lo >>= 8;
		base *= 100;
	}
	for (int i = 0; i < 4; i++) {
		val += base * bcd100(mi);
		mi >>= 8;
		base *= 100;
	}
	for (int i = 0; i < 2; i++) {
		val += base * bcd100(hi);
		hi >>= 8;
		base *= 100;
	}
	*res = copysign(val, sign ? -1.0 : 1.0);
	return true;
}

static bool fpstoref32(void *cpu, int seg, uword addr, double val)
{
	union union32 u = { .f = val };
	u32 v = u.i;
	if (!cpu_store32(cpu, seg, addr, v))
		return false;
	return true;
}

static bool fpstoref64(void *cpu, int seg, uword addr, double val)
{
	union union64 u = { .f = val };
	uint64_t v = u.i;
	if (!cpu_store32(cpu, seg, addr, v))
		return false;
	if (!cpu_store32(cpu, seg, addr + 4, v >> 32))
		return false;
	return true;
}

static bool fpstoref80(void *cpu, int seg, uword addr, double val)
{
	F80 f80 = tof80(val);
	if (!cpu_store32(cpu, seg, addr, f80.mant0))
		return false;
	if (!cpu_store32(cpu, seg, addr + 4, f80.mant1))
		return false;
	if (!cpu_store16(cpu, seg, addr + 8, f80.high))
		return false;
	return true;
}

static double fpround(double x, int rc)
{
	switch (rc) {
	case 0:
		// XXX: we assume `fegetround() == 1` here.
		// In C23, we can use `roundeven()` instead.
		return nearbyint(x);
	case 1:
		return floor(x);
	case 2:
		return ceil(x);
	case 3:
		return trunc(x);
	}
	return x;
}

static bool fpstorei16(void *cpu, int seg, uword addr, double val)
{
	s16 v = val < 32768.0 && val >= -32768.0 ? (s16) val : 0x8000;
	if (!cpu_store16(cpu, seg, addr, v))
		return false;
	return true;
}

static bool fpstorei32(void *cpu, int seg, uword addr, double val)
{
	s32 v = val < 2147483648.0 && val >= -2147483648.0 ? (s32) val : 0x80000000;
	if (!cpu_store32(cpu, seg, addr, v))
		return false;
	return true;
}

static bool fpstorei64(void *cpu, int seg, uword addr, double val)
{
	int64_t v = val < 9223372036854775808.0 && val >= -9223372036854775808.0 ?
		(int64_t) val : 0x8000000000000000ll;
	if (!cpu_store32(cpu, seg, addr, v))
		return false;
	if (!cpu_store32(cpu, seg, addr + 4, v >> 32))
		return false;
	return true;
}

static bool fpstorebcd(void *cpu, int seg, uword addr, double val)
{
	int64_t v = val < 9223372036854775808.0 && val >= -9223372036854775808.0 ?
		(int64_t) val : 0x8000000000000000ll;
	int sign = 0;
	if (v < 0) {
		sign = 1;
		v = -v;
	}

	for (int i = 0; i < 9; i++) {
		int res = (v % 10) | ((v / 10) % 10) << 4;
		v /= 100;
		if (!cpu_store8(cpu, seg, addr + i, res))
			return false;
	}

	int res = (v % 10) | ((v / 10) % 10) << 4;
	if (sign)
		res |= 0x80;
	if (!cpu_store8(cpu, seg, addr + 9, res))
		return false;
	return true;
}

enum {
	C0 = 0x100,
	C1 = 0x200,
	C2 = 0x400,
	C3 = 0x4000,
};

static void fparith(FPU *fpu, int group, unsigned int d, double a, double b)
{
	double c;
	switch (group) {
	case 0: // FADD
		c = a + b;
		break;
	case 1: // FMUL
		c = a * b;
		break;
	case 2: // FCOM
	case 3: // FCOMP
		if (isunordered(a, b)) {
			fpu->sw |= C0 | C2 | C3;
		} else if (a == b) {
			fpu->sw |= C3;
			fpu->sw &= ~(C0 | C2);
		} else if (a < b) {
			fpu->sw |= C0;
			fpu->sw &= ~(C2 | C3);
		} else {
			fpu->sw &= ~(C0 | C2 | C3);
		}
		break;
	case 4: // FSUB
		c = a - b;
		break;
	case 5: // FSUBR
		c = b - a;
		break;
	case 6: // FDIV
		c = a / b;
		break;
	case 7: // FDIVR
		c = b / a;
		break;
	default:
		assert(false);
	}
	if (group == 3) {
		fppop(fpu);
	} else if (group != 2) {
		fpset(fpu, d, c);
	}
}

bool fpu_exec2(FPU *fpu, void *cpu, bool opsz16, int op, int group, int seg, uword addr)
{
	double a;
	switch (op) {
	case 0: {
		a = fpget(fpu, 0);
		double b;
		if (!fploadf32(cpu, seg, addr, &b)) {
			return false;
		}
		fparith(fpu, group, 0, a, b);
		break;
	}
	case 1: {
		switch (group) {
		case 0: { // FLD float32
			if (!fploadf32(cpu, seg, addr, &a)) {
				return false;
			}
			fppush(fpu, a);
			break;
		}
		case 1:
			cpu_setexc(cpu, 6, 0);
			return false;
		case 2: { // FST float32
			a = fpget(fpu, 0);
			if (!fpstoref32(cpu, seg, addr, a)) {
				return false;
			}
			break;
		}
		case 3: { // FSTP float32
			a = fpget(fpu, 0);
			if (!fpstoref32(cpu, seg, addr, a)) {
				return false;
			}
			fppop(fpu);
			break;
		}
		case 4: // FLDENV
			if (opsz16) {
				if(!cpu_load16(cpu, seg, addr, &(fpu->cw)))
					return false;
				u16 sw;
				if(!cpu_load16(cpu, seg, addr + 2, &sw))
					return false;
				setsw(fpu, sw);
			} else {
				if(!cpu_load16(cpu, seg, addr, &(fpu->cw)))
					return false;
				u16 sw;
				if(!cpu_load16(cpu, seg, addr + 4, &sw))
					return false;
				setsw(fpu, sw);
			}
			break;
		case 5: // FLDCW
			if(!cpu_load16(cpu, seg, addr, &(fpu->cw)))
				return false;
			break;
		case 6: // FNSTENV
			if (opsz16) {
				if(!cpu_store16(cpu, seg, addr, fpu->cw))
					return false;
				u16 sw = getsw(fpu);
				if(!cpu_store16(cpu, seg, addr + 2, sw))
					return false;
				if(!cpu_store16(cpu, seg, addr + 4, 0 /* tw */))
					return false;
			} else {
				if(!cpu_store32(cpu, seg, addr, fpu->cw))
					return false;
				u16 sw = getsw(fpu);
				if(!cpu_store32(cpu, seg, addr + 4, sw))
					return false;
				if(!cpu_store32(cpu, seg, addr + 8, 0 /* tw */))
					return false;
			}
			break;
		case 7: // FNSTCW
			if(!cpu_store16(cpu, seg, addr, fpu->cw))
				return false;
			break;
		}
		break;
	}
	case 2: {
		a = fpget(fpu, 0);
		double b;
		bool r = fploadi32(cpu, seg, addr, &b);
		if (!r)
			return false;
		fparith(fpu, group, 0, a, b);
		break;
	}
	case 3:
		switch (group) {
		case 0: { // FILD int32
			if (!fploadi32(cpu, seg, addr, &a)) {
				return false;
			}
			fppush(fpu, a);
			break;
		}
		case 1: // FISTTP int32
		case 2: // FIST int32
		case 3: // FISTP int32
		{
			a = fpget(fpu, 0);
			if (group != 1) {
				int rc = (fpu->cw >> 10) & 3;
				a = fpround(a, rc);
			}
			if (!fpstorei32(cpu, seg, addr, a)) {
				return false;
			}
			if (group != 2)
				fppop(fpu);
			break;
		}
		case 4:
			cpu_setexc(cpu, 6, 0);
			return false;
		case 5: { // FLD float80
			if (!fploadf80(cpu, seg, addr, &a)) {
				return false;
			}
			fppush(fpu, a);
			break;
		}
		case 6:
			cpu_setexc(cpu, 6, 0);
			return false;
		case 7: { // FSTP float80
			a = fpget(fpu, 0);
			if (!fpstoref80(cpu, seg, addr, a)) {
				return false;
			}
			fppop(fpu);
			break;
		}
		}
		break;
	case 4: {
		a = fpget(fpu, 0);
		double b;
		bool r = fploadf64(cpu, seg, addr, &b);
		if (!r)
			return false;
		fparith(fpu, group, 0, a, b);
		break;
	}
	case 5:
		switch (group) {
		case 0: { // FLD float64
			if (!fploadf64(cpu, seg, addr, &a)) {
				return false;
			}
			fppush(fpu, a);
			break;
		}
		case 1: { // FISTTP int64
			a = fpget(fpu, 0);
			if (!fpstorei64(cpu, seg, addr, a)) {
				return false;
			}
			fppop(fpu);
			break;
		}
		case 2: { // FST float64
			a = fpget(fpu, 0);
			if (!fpstoref64(cpu, seg, addr, a)) {
				return false;
			}
			break;
		}
		case 3: { // FSTP float64
			a = fpget(fpu, 0);
			if (!fpstoref64(cpu, seg, addr, a)) {
				return false;
			}
			fppop(fpu);
			break;
		}
		case 4: { // FRSTOR
			uword start = addr;
			if (opsz16) {
				if (!cpu_load16(cpu, seg, addr, &(fpu->cw)))
					return false;
				u16 sw;
				if (!cpu_load16(cpu, seg, addr + 2, &sw))
					return false;
				setsw(fpu, sw);
				start += 14;
			} else {
				if (!cpu_load16(cpu, seg, addr, &(fpu->cw)))
					return false;
				u16 sw;
				if (!cpu_load16(cpu, seg, addr + 4, &sw))
					return false;
				setsw(fpu, sw);
				start += 28;
			}
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
				start += 10;
			}
			break;
		}
		case 5:
			cpu_setexc(cpu, 6, 0);
			return false;
		case 6: { // FNSAVE
			uword start = addr;
			if (opsz16) {
				if(!cpu_store16(cpu, seg, addr, fpu->cw))
					return false;
				u16 sw = getsw(fpu);
				if(!cpu_store16(cpu, seg, addr + 2, sw))
					return false;
				if(!cpu_store16(cpu, seg, addr + 4, 0 /* tw */))
					return false;
				start += 14;
			} else {
				if(!cpu_store32(cpu, seg, addr, fpu->cw))
					return false;
				u16 sw = getsw(fpu);
				if(!cpu_store32(cpu, seg, addr + 4, sw))
					return false;
				if(!cpu_store32(cpu, seg, addr + 8, 0 /* tw */))
					return false;
				start += 28;
			}
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
				start += 10;
			}
			fpu->sw = 0;
			fpu->top = 0;
			fpu->cw = 0x37f;
			break;
		}
		case 7: // FNSTSW
			if (!cpu_store16(cpu, seg, addr, getsw(fpu)))
				return false;
			break;
		}
		break;
	case 6: {
		a = fpget(fpu, 0);
		double b;
		bool r = fploadi16(cpu, seg, addr, &b);
		if (!r)
			return false;
		fparith(fpu, group, 0, a, b);
		break;
	}
	case 7:
		switch (group) {
		case 0: { // FILD int16
			if (!fploadi16(cpu, seg, addr, &a)) {
				return false;
			}
			fppush(fpu, a);
			break;
		}
		case 1: // FISTTP int16
		case 2: // FIST int16
		case 3: // FISTP int16
		{
			a = fpget(fpu, 0);
			if (group != 1) {
				int rc = (fpu->cw >> 10) & 3;
				a = fpround(a, rc);
			}
			if (!fpstorei16(cpu, seg, addr, a)) {
				return false;
			}
			if (group != 2)
				fppop(fpu);
			break;
		}
		case 4: { // FBLD
			if (!fploadbcd(cpu, seg, addr, &a)) {
				return false;
			}
			fppush(fpu, a);
			break;
		}
		case 5: { // FILD int64
			u32 lo, hi;
			if (!cpu_load32(cpu, seg, addr, &lo) || !cpu_load32(cpu, seg, addr + 4, &hi))
				return false;
			fppushi64(fpu, (int64_t) ((uint64_t) hi << 32 | lo));
			break;
		}
		case 6: { // FBSTP
			int rc = (fpu->cw >> 10) & 3;
			a = fpget(fpu, 0);
			a = fpround(a, rc);
			if (!fpstorebcd(cpu, seg, addr, a)) {
				return false;
			}
			fppop(fpu);
			break;
		}
		case 7: { // FISTP int64
			int64_t v;
			if (fpgeti64(fpu, &v)) {
				if (!cpu_store32(cpu, seg, addr, v) || !cpu_store32(cpu, seg, addr + 4, (uint64_t) v >> 32))
					return false;
				fppop(fpu);
				break;
			}
			int rc = (fpu->cw >> 10) & 3;
			a = fpget(fpu, 0);
			a = fpround(a, rc);
			if (!fpstorei64(cpu, seg, addr, a)) {
				return false;
			}
			fppop(fpu);
			break;
		}
		}
		break;
	}
	return true;
}

#define PI		3.141592653589793238462L
#define L2E		1.4426950408889634073605L
#define L2T		3.3219280948873623478693L
#define LN2		0.69314718055994530941683L
#define LG2		0.30102999566398119521379L

enum {
	CF = 0x1,
	PF = 0x4,
	AF = 0x10,
	ZF = 0x40,
	SF = 0x80,
	OF = 0x800,
};

static bool cmov_cond(FPU *fpu, void *cpu, int i)
{
	uword flags = cpu_getflags(cpu);
	switch(i) {
	case 0x0: return flags & CF;
	case 0x1: return flags & ZF;
	case 0x2: return flags & (CF | ZF);
	case 0x3: return flags & PF;
	default: assert(false);
	}
}

static void ucomi(FPU *fpu, void *cpu, int i)
{
	double a = fpget(fpu, 0);
	double b = fpget(fpu, i);
	if (isunordered(a, b)) {
		cpu_setflags(cpu, ZF | PF | CF, 0);
	} else if (a == b) {
		cpu_setflags(cpu, ZF, PF | CF);
	} else if (a < b) {
		cpu_setflags(cpu, CF, ZF | PF);
	} else {
		cpu_setflags(cpu, 0, ZF | PF | CF);
	}
}

bool fpu_exec1(FPU *fpu, void *cpu, int op, int group, unsigned int i)
{
	switch (op) {
	case 0: {
		double a = fpget(fpu, 0);
		double b = fpget(fpu, i);
		fparith(fpu, group, 0, a, b);
		break;
	}
	case 1: {
		double temp, temp2;
		switch (group) {
		case 0: // FLD
			temp = fpget(fpu, i);
			fppush(fpu, temp);
			break;
		case 1: // FXCH
			fpxch(fpu, i, 0);
			break;
		case 2: // FNOP
			break;
		case 3: // FSTP
			temp = fpget(fpu, 0);
			fpset(fpu, i, temp);
			fppop(fpu);
			break;
		case 4:
			temp = fpget(fpu, 0);
			switch (i) {
			case 0: // FCHS
				fpset(fpu, 0, copysign(temp,
						       signbit(temp) ? 1.0 : -1.0));
				break;
			case 1: // FABS
				fpset(fpu, 0, fabs(temp));
				break;
			case 2:
			case 3:
			case 6:
			case 7:
				cpu_setexc(cpu, 6, 0);
				return false;
			case 4: // FTST
				fparith(fpu, 2, 0, temp, 0.0);
				break;
			case 5: // FXAM
				if (signbit(temp)) {
					fpu->sw |= C1;
				} else {
					fpu->sw &= ~C1;
				}
				if(temp == 0.0) {
					fpu->sw |= C3;
					fpu->sw &= ~(C0 | C2);
				} else if (isnan(temp)) {
					fpu->sw |= C0;
					fpu->sw &= ~(C2 | C3);
				} else if (isfinite(temp)) {
					fpu->sw |= C2;
					fpu->sw &= ~(C0 | C3);
				} else {
					fpu->sw |= (C0 | C2);
					fpu->sw &= ~C3;
				}
				break;
			}
			break;
		case 5:
			switch (i) {
			case 0: // FLD1
				fppush(fpu, 1.0);
				break;
			case 1: // FLDL2T
				fppush(fpu, L2T);
				break;
			case 2: // FLDL2E
				fppush(fpu, L2E);
				break;
			case 3: // FLDPI
				fppush(fpu, PI);
				break;
			case 4: // FLDLG2
				fppush(fpu, LG2);
				break;
			case 5: // FLDLN2
				fppush(fpu, LN2);
				break;
			case 6: // FLDZ
				fppush(fpu, 0.0);
				break;
			case 7:
				cpu_setexc(cpu, 6, 0);
				return false;
			}
			break;
		case 6:
			temp = fpget(fpu, 0);
			switch (i) {
			case 0: // F2XM1
				fpset(fpu, 0, pow(2.0, temp) - 1);
				break;
			case 1: // FYL2X
				temp2 = fpget(fpu, 1);
				fpset(fpu, 1, temp2 * log2(temp));
				fppop(fpu);
				break;
			case 2: // FPTAN
				fpset(fpu, 0, tan(temp));
				fppush(fpu, 1.0);
				fpu->sw &= ~C2;
				break;
			case 3: // FPATAN
				temp2 = fpget(fpu, 1);
				fpset(fpu, 1, atan2(temp2, temp));
				fppop(fpu);
				break;
			case 4: { // FXTRACT
				int exp;
				double mant = frexp(temp, &exp);
				mant *= 2;
				exp--;
				fpset(fpu, 0, exp);
				fppush(fpu, mant);
				break;
			}
			case 5: { // FPREM1
				temp2 = fpget(fpu, 1);
				int64_t q = nearbyint(temp / temp2); // TODO: overflow
				fpset(fpu, 0, temp - q * temp2);
				fpu->sw &= ~C2;
				if (q & 1) fpu->sw |= C1; else fpu->sw &= ~C1;
				if (q & 2) fpu->sw |= C3; else fpu->sw &= ~C3;
				if (q & 4) fpu->sw |= C0; else fpu->sw &= ~C0;
				break;
			}
			case 6: // FDECSTP
				fpu->top = (fpu->top - 1) & 7;
				break;
			case 7: // FINCSTP
				fpu->top = (fpu->top + 1) & 7;
				break;
			}
			break;
		case 7:
			temp = fpget(fpu, 0);
			switch (i) {
			case 0: { // FPREM
				temp2 = fpget(fpu, 1);
				int64_t q = temp / temp2; // TODO: overflow
				fpset(fpu, 0, temp - q * temp2);
				fpu->sw &= ~C2;
				if (q & 1) fpu->sw |= C1; else fpu->sw &= ~C1;
				if (q & 2) fpu->sw |= C3; else fpu->sw &= ~C3;
				if (q & 4) fpu->sw |= C0; else fpu->sw &= ~C0;
				break;
			}
			case 1: // FYL2XP1
				temp2 = fpget(fpu, 1);
				fpset(fpu, 1, temp2 * log2(1.0 + temp));
				fppop(fpu);
				break;
			case 2: // FSQRT
				fpset(fpu, 0, sqrt(temp));
				break;
			case 3: { // FSINCOS
				double s, c;
#ifdef _GNU_SOURCE
				sincos(temp, &s, &c);
#else
				s = sin(temp);
				c = cos(temp);
#endif
				fpset(fpu, 0, s);
				fppush(fpu, c);
				fpu->sw &= ~C2;
				break;
			}
			case 4: { // FRNDINT
				int rc = (fpu->cw >> 10) & 3;
				fpset(fpu, 0, fpround(temp, rc));
				break;
			}
			case 5: // FSCALE
				fpset(fpu, 0,
				      temp * pow(2.0, trunc(fpget(fpu, 1))));
				break;
			case 6: // FSIN
				fpset(fpu, 0, sin(temp));
				fpu->sw &= ~C2;
				break;
			case 7: // FCOS
				fpset(fpu, 0, cos(temp));
				fpu->sw &= ~C2;
				break;
			}
			break;
		}
		break;
	}
	case 2:
		switch (group) {
		case 5:
			if (i == 1) { // FUCOMPP
				double a = fpget(fpu, 0);
				double b = fpget(fpu, 1);
				if (isunordered(a, b)) {
					fpu->sw |= C0 | C2 | C3;
				} else if (a == b) {
					fpu->sw |= C3;
					fpu->sw &= ~(C0 | C2);
				} else if (a < b) {
					fpu->sw |= C0;
					fpu->sw &= ~(C2 | C3);
				} else {
					fpu->sw &= ~(C0 | C2 | C3);
				}
				fppop(fpu);
				fppop(fpu);
				break;
			}
			cpu_setexc(cpu, 6, 0);
			return false;
		case 0: case 1: case 2: case 3: // FCMOV
			if (cmov_cond(fpu, cpu, group))
				fpset(fpu, 0, fpget(fpu, i));
			break;
		default:
			cpu_setexc(cpu, 6, 0);
			return false;
		}
		break;
	case 3:
		switch (group) {
		case 4:
			switch (i) {
			case 0:
			case 1: // 8087 only
			case 4: // FNSETPM
			case 5: // FRSTPM
				/* nothing */
				break;
			case 2: // FNCLEX
				fpu->sw &= ~0x80ff;
				break;
			case 3: // FNINIT
				fpu->sw = 0;
				fpu->top = 0;
				fpu->cw = 0x37f;
				break;
			case 6:
			case 7:
				cpu_setexc(cpu, 6, 0);
				return false;
			}
			break;
		case 0: case 1: case 2: case 3: // FCMOV
			if (!cmov_cond(fpu, cpu, group))
				fpset(fpu, 0, fpget(fpu, i));
			break;
		case 5: // FUCOMI
		case 6: // FCOMI TODO: raise IA
			ucomi(fpu, cpu, i);
			break;
		default:
			cpu_setexc(cpu, 6, 0);
			return false;
		}
		break;
	case 4: {
		double a = fpget(fpu, 0);
		double b = fpget(fpu, i);
		fparith(fpu, group, i, a, b);
		break;
	}
	case 5: {
		double temp;
		switch (group) {
		case 0: // FFREE
			// tw
			break;
		case 1: // FXCH
			fpxch(fpu, i, 0);
			break;
		case 2: // FST
			temp = fpget(fpu, 0);
			fpset(fpu, i, temp);
			break;
		case 3: // FSTP
			temp = fpget(fpu, 0);
			fpset(fpu, i, temp);
			fppop(fpu);
			break;
		case 4: // FUCOM
		case 5: // FUCOMP
		{
			double a = fpget(fpu, 0);
			double b = fpget(fpu, i);
			if (isunordered(a, b)) {
				fpu->sw |= C0 | C2 | C3;
			} else if (a == b) {
				fpu->sw |= C3;
				fpu->sw &= ~(C0 | C2);
			} else if (a < b) {
				fpu->sw |= C0;
				fpu->sw &= ~(C2 | C3);
			} else {
				fpu->sw &= ~(C0 | C2 | C3);
			}
			if (group == 5)
				fppop(fpu);
			break;
		}
		case 6:
		case 7:
			cpu_setexc(cpu, 6, 0);
			return false;
		}
		break;
	}
	case 6: {
		double a = fpget(fpu, 0);
		double b = fpget(fpu, i);
		fparith(fpu, group, i, a, b);
		fppop(fpu);
		break;
	}
	case 7:
		switch (group) {
		case 0: // FFREEP
			// tw
			fppop(fpu);
			break;
		case 1: { // FXCH
			fpxch(fpu, i, 0);
			break;
		}
		case 2: // FSTP
		case 3: { // FSTP
			double temp;
			temp = fpget(fpu, 0);
			fpset(fpu, i, temp);
			fppop(fpu);
			break;
		}
		case 4:
			if (i == 0) { // FNSTSW
				u16 sw = getsw(fpu);
				cpu_setax(cpu, sw);
			} else {
				cpu_setexc(cpu, 6, 0);
				return false;
			}
			break;
		case 5: // FUCOMIP
		case 6: // FCOMIP TODO: raise IA
			ucomi(fpu, cpu, i);
			fppop(fpu);
			break;
		case 7:
			cpu_setexc(cpu, 6, 0);
			return false;
		}
		break;
	}
	return true;
}

#if defined(I386_ENABLE_MMX) || defined(I386_ENABLE_SSE)
#define SIMD_fpu_c
#include "simd.inc"
#undef SIMD_fpu_c
#endif
