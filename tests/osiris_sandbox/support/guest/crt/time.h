/* Test shim: C runtime declarations for building the level1 test data as a Win32 DLL. */
#pragma once
typedef long time_t;
#ifdef __cplusplus
extern "C" {
#endif
time_t time(time_t *);
long clock(void);
#ifdef __cplusplus
}
#endif
