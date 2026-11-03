/* Math for the sandbox guest. Operations Wasm has instructions for use builtins;
   the rest are imported from the host as pure double -> double functions. */
#ifndef SB_MATH_H
#define SB_MATH_H
#define SB_IMPORT(name) __attribute__((import_module("env"), import_name(name)))
SB_IMPORT("m_sin") double sin(double);
SB_IMPORT("m_cos") double cos(double);
SB_IMPORT("m_tan") double tan(double);
SB_IMPORT("m_atan2") double atan2(double, double);
SB_IMPORT("m_pow") double pow(double, double);
SB_IMPORT("m_log2") double log2(double);
static inline double sqrt(double x) { return __builtin_sqrt(x); }
static inline double fabs(double x) { return __builtin_fabs(x); }
static inline double floor(double x) { return __builtin_floor(x); }
static inline double ceil(double x) { return __builtin_ceil(x); }
static inline double trunc(double x) { return __builtin_trunc(x); }
static inline double nearbyint(double x) { return __builtin_nearbyint(x); }
static inline double copysign(double x, double y) { return __builtin_copysign(x, y); }
static inline void sincos(double x, double *s, double *c) { *s = sin(x); *c = cos(x); }
double frexp(double x, int *e);
#define isnan(x) __builtin_isnan(x)
#define isinf(x) __builtin_isinf(x)
#define isfinite(x) __builtin_isfinite(x)
#define isunordered(a, b) __builtin_isunordered(a, b)
#define signbit(x) __builtin_signbit(x)
#define NAN __builtin_nan("")
#define INFINITY __builtin_inf()
#endif
