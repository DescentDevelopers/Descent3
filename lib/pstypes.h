#ifndef _TYPES_H
#define _TYPES_H


//define unsigned types;
typedef unsigned char ubyte;
typedef signed char sbyte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

#ifdef _MSC_VER	//only Visual C++ has __int64
typedef __int64	longlong;
#else
typedef long long longlong;
#endif

#ifndef NULL
#define NULL 0
#endif

//The maximum length for a file name, including extension.  It *does not* include the
//terminating NULL, so a buffer to hold a filename needs to be PSFILENAME_LEN+1 bytes long.
#define PSFILENAME_LEN		35

//The maximum length of a path (or path+filename).  The seems (from looking at the code) to
//include the terminating NULL. lengthened to 512 to prevent problems with some long pathnames.
#define PSPATHNAME_LEN		512

#if !defined(__APPLE__)
#define HOST_BIGENDIAN @HOST_BIGENDIAN@
#else
#  if defined(__BIG_ENDIAN__)
#    define HOST_BIGENDIAN 1
#    define MACOSXPPC 1
#  elif  defined (__LITTLE_ENDIAN__)
#    define HOST_BIGENDIAN 0
#    define MACOSX86 1
#  else
#    error "Unknown HOST_BIGENDIAN!"
#  endif
#endif

#endif

