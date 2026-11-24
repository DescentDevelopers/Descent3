/* Test shim: C runtime declarations for building the level1 test data as a Win32 DLL. */
#pragma once
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
size_t strlen(const char *);
char *strcpy(char *, const char *);
char *strncpy(char *, const char *, size_t);
char *strcat(char *, const char *);
char *strncat(char *, const char *, size_t);
int strcmp(const char *, const char *);
int strncmp(const char *, const char *, size_t);
int stricmp(const char *, const char *);
int _stricmp(const char *, const char *);
int strnicmp(const char *, const char *, size_t);
int _strnicmp(const char *, const char *, size_t);
char *strchr(const char *, int);
char *strrchr(const char *, int);
char *strstr(const char *, const char *);
char *strtok(char *, const char *);
char *_strdup(const char *);
void *memcpy(void *, const void *, size_t);
void *memmove(void *, const void *, size_t);
void *memset(void *, int, size_t);
int memcmp(const void *, const void *, size_t);
#ifdef __cplusplus
}
#endif
