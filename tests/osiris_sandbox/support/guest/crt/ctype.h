/* Test shim: C runtime declarations for building the level1 test data as a Win32 DLL. */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif
int toupper(int); int tolower(int); int isalpha(int); int isdigit(int); int isspace(int); int isalnum(int);
int isupper(int); int islower(int); int isxdigit(int); int isprint(int); int ispunct(int);
#ifdef __cplusplus
}
#endif
