#include "MacHeadersD3.pch"

#include <math.h>
#include "Macros.h"
#include <ctype.h>

// Constants
#define MACINTOSH	1

#undef USE_GLIDE
#define USE_OPENGL
#define NUM_MIP_LEVELS 8
#define mprintf(args) //Debug_ConsolePrintf args
#undef STEALTH
#undef DAJ_DEBUG
#undef _DEBUG
#define RELEASE

#define and	andVar
#define or	orVar

#define _fstat		fstat
#define _filelength	filelength

#define __cdecl

#define _MAX_PATH	1024
#define _MAX_FNAME	64	/* max. length of path component*/
#define _MAX_EXT	64 /* max. length of extension component*/

#define TRANSPARENT_COLOR	0

// These are other error types defined in Visual C++.
// The values are made up and hopefullydon't conflict JCA
#define EACCES			102
#define EDEADLOCK		104
#define EMFILE			105
#define ENOENT			106
#define ENFILE			107
#define ENOSPC			108
#define ENOSYS			112


// Types
typedef unsigned char BOOL;
typedef void **HANDLE;
typedef Fixed fix;
typedef long long INT64;
typedef void *HGLOBAL;

// Function Definitions
#define _finite(a)			finite(a)
#define strcmpi(a,b)		stricmp(a,b)
#define strcmpni(a,b,c)		strnicmp(a,b,c)
#define _vsnprintf(a,b,c,d) vsnprintf(a,b,c,d)
#define HeapSize(a,b,c)		0
#define HeapDestroy(a)

int stricmp(const char *inX, const char *inY);
int strnicmp(const char *inX, const char *inY, int len);
char *strdup(const char *str);

void GlobalFree(void *);
void *GlobalAlloc(int flags,int size);
void *GlobalLock(HGLOBAL hMem);
void Sleep(int millis);
char *itoa(int value,char *string,int radix);
char *strupr(char *string);

inline int finite(double a)
{
	return(a != HUGE_VAL);
}

