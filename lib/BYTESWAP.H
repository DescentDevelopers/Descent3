/*
 * $Logfile: /DescentIII/Main/lib/BYTESWAP.H $
 * $Revision: 8 $
 * $Date: 4/19/00 5:32p $
 * $Author: Matt $
 *
 * Byteswapping macros (for big-endian machines)
 *
 * $Log: /DescentIII/Main/lib/BYTESWAP.H $
 *
 * 8     4/19/00 5:32p Matt
 * From Duane for 1.4
 * Added casts to SWAPSHORT and SWAPINT macros
 *
 * 7     10/21/99 9:27p Jeff
 * B.A. Macintosh code merge
 *
 * 6     5/09/99 11:41p Jeff
 * function free
 *
 * 5     5/05/99 5:27a Jeff
 * renamed endian.h to psendian.h
 *
 * 4     5/01/99 2:52p Jeff
 * added automatic endian detection of the system
 *
 * 3     4/17/99 7:49p Jeff
 * for some reason Linux thinks it's big endian, temp fix (undef) until I
 * get around to writting a endian check function
 *
 * 2     1/09/99 4:38p Jeff
 * added some ifdefs and fixes to get files to compile under Linux
 *
 * 5     5/15/97 2:22p Matt
 * Fixed (hopefully; it's not tested yet) byteswapping for floats
 *
 * 4     2/10/97 2:22p Matt
 * Added cast
 *
 * 3     2/10/97 2:14p Matt
 * Added BIG_ENDIAN define, & INT_FLOAT() macro
 *
 * $NoKeywords: $
 */

#ifndef _BYTESWAP_H
#define _BYTESWAP_H

#include "pstypes.h"

/*
#include "psendian.h"

#define SWAPSHORT(x)	(	((x) << 8) | (((ushort)(x)) >> 8) )
#define SWAPINT(x)	(	((x) << 24) | (((ulong)(x)) >> 24) | (((x) & 0x0000ff00) << 8) | (((x) & 0x00ff0000) >>
8)	)

//Stupid function to trick the compiler into letting me byteswap a float
inline float SWAPFLOAT(float x)
{
        int i = SWAPINT(*((int *) &(x)));
        return *((float *) &(i));
}

// INTEL_ assumes the returned value will be in "Little Endian Format"
#define INTEL_INT(x)		Endian_SwapInt(x)
#define INTEL_SHORT(x)		Endian_SwapShort(x)
#define INTEL_FLOAT(x)		Endian_SwapFloat(x)

// MOTOROLA_ assumes the returned value will be in "Big Endian Format"
#define MOTOROLA_INT(x)		SWAPINT(Endian_SwapInt(x))
#define MOTOROLA_SHORT(x)	SWAPSHORT(Endian_SwapShort(x))
#define MOTOROLA_FLOAT(x)	SWAPFLOAT(Endian_SwapFloat(x))
*/
#define SWAPSHORT(x) (short)(0xFFFF & (((x) << 8) | (((ushort)(x)) >> 8)))
#define SWAPINT(x) (int)(((x) << 24) | (((ulong)(x)) >> 24) | (((x)&0x0000ff00) << 8) | (((x)&0x00ff0000) >> 8))

// Stupid function to trick the compiler into letting me byteswap a float
inline float SWAPFLOAT(float x) {
  int i = SWAPINT(*((int *)&(x)));
  return *((float *)&(i));
}

// Default is little endian, so change for Macintosh
#if (MACOSX && MACOSXPPC)
#define OUTRAGE_BIG_ENDIAN
#endif

#if (defined __LINUX__) && (!defined(MACOSX))
#include <endian.h>

#if BYTE_ORDER == BIG_ENDIAN
#define OUTRAGE_BIG_ENDIAN
#endif
#endif

#ifndef OUTRAGE_BIG_ENDIAN

#define INTEL_INT(x) x
#define INTEL_SHORT(x) x
#define INTEL_FLOAT(x) x
#define MOTOROLA_INT(x) SWAPINT(x)
#define MOTOROLA_SHORT(x) SWAPSHORT(x)
#define MOTOROLA_FLOAT(x) SWAPFLOAT(x)
#else
#define INTEL_INT(x) SWAPINT(x)
#define INTEL_SHORT(x) SWAPSHORT(x)
#define INTEL_FLOAT(x) SWAPFLOAT(x)
#define MOTOROLA_INT(x) x
#define MOTOROLA_SHORT(x) x
#define MOTOROLA_FLOAT(x) x
#endif

#endif
