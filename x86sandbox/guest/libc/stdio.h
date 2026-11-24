/* Minimal stdio for the x86 sandbox guest (wasm32). Only what tiny386 uses. */
#ifndef SB_STDIO_H
#define SB_STDIO_H
#include <stddef.h>
#include <stdarg.h>
typedef struct sb_file FILE;
extern FILE *stderr;
extern FILE *stdout;
int fprintf(FILE *f, const char *fmt, ...);
int printf(const char *fmt, ...);
#endif
