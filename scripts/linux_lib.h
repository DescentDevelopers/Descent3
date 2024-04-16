#ifndef __LINUX_LIB_H_
#define __LINUX_LIB_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void _splitpath(const char *path, char *drive, char *dir, char *fname, char *ext);
int _vsnprintf(char *buffer, size_t count, const char *format, va_list argptr);
int stricmp(const char *string1, const char *string2);

#endif