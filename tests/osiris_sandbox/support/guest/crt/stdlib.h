/* Test shim: C runtime declarations for building the level1 test data as a Win32 DLL. */
#pragma once
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
void *malloc(size_t);
void *calloc(size_t, size_t);
void *realloc(void *, size_t);
void free(void *);
int atoi(const char *);
double atof(const char *);
int rand(void);
void srand(unsigned);
int abs(int);
long labs(long);
void qsort(void *, size_t, size_t, int (*)(const void *, const void *));
#define RAND_MAX 0x7fff
#define _MAX_PATH 260
void _splitpath(const char *, char *, char *, char *, char *);
#ifdef __cplusplus
}
#endif
