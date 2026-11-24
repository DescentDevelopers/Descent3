/* Test shim: C runtime declarations for building the level1 test data as a Win32 DLL. */
#pragma once
#include <stdarg.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
int sprintf(char *, const char *, ...);
int _snprintf(char *, size_t, const char *, ...);
int vsprintf(char *, const char *, va_list);
int _vsnprintf(char *, size_t, const char *, va_list);
int printf(const char *, ...);
#ifdef __cplusplus
}
#endif
