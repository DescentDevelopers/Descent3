#ifndef __LINUX_FIX_H_
#define __LINUX_FIX_H_

#include <sys/stat.h>


#define LOKI_VERSION ""


#define HGLOBAL void*

void GlobalFree(void *);
void *GlobalAlloc(int flags,int size);
void *GlobalLock(HGLOBAL hMem);
void Sleep(int millis);
char *itoa(int value,char *string,int radix);
char *strupr(char *string);
// Replace missing defines from stdlib.h
#define _MAX_PATH	260	/* max. length of full pathname*/
#define _MAX_FNAME	256	/* max. length of path component*/
#define _MAX_EXT	256 /* max. length of extension component*/
#if !defined(MAX_PATH)
#define MAX_PATH _MAX_PATH
#endif
#define HANDLE int
// _cdecl replacement
#define __cdecl __attribute__((cdecl))
// _stdcall replacement
#define _stdcall __attribute__((stdcall))

inline int _filelength(int fd)
{
	struct stat statbuf;

	if(fstat(fd,&statbuf)!=0)
	{
		return -1;
	}

	return statbuf.st_size;	
}

#ifndef stricmp
#define stricmp(a,b) strcasecmp(a,b)
#endif

#define strnicmp(a,b,c) strncasecmp(a,b,c)
#define _fstat(a,b) fstat(a,b)
#define _fileno(a) fileno(a)
#define strcmpi(a,b) stricmp(a,b)
#define strcmpni(a,b,c) strnicmp(a,b,c)
#define _chmod(a,b) chmod(a,b)
#define _finite(a) finite(a)
#define _min(a,b) min(a,b)
#define _max(a,b) max(a,b)
#define __min(a,b) min(a,b)
#define __max(a,b) max(a,b)

#endif


