#ifndef SB_STDLIB_H
#define SB_STDLIB_H
#include <stddef.h>
void *malloc(size_t n);
void *calloc(size_t n, size_t m);
void free(void *p);
_Noreturn void abort(void);
#endif
