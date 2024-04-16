/*
 * $Logfile: /DescentIII/Main/Lib/BYTESWAP.H $
 * $Revision: 1.5 $
 * $Date: 2004/12/05 04:00:20 $
 * $Author: ryan $
 *
 * Byteswapping macros (for big-endian machines)
 *
 * $Log: byteswap.h,v $
 * Revision 1.5  2004/12/05 04:00:20  ryan
 * MacOS X patches.
 *
 * Revision 1.4  2004/02/25 00:04:06  ryan
 * Removed loki_utils dependency and ported to MacOS X (runs, but incomplete).
 *
 * Revision 1.3  2004/02/09 04:14:51  kevinb
 * Added newlines to all headers to reduce number of warnings printed
 *
 * Made some small changes to get everything compiling.
 *
 * All Ready to merge the 1.5 tree.
 *
 * Revision 1.2  2000/06/03 14:33:51  icculus
 * Merge with Outrage 1.4 tree...
 *
 * Revision 1.1  2000/04/18 00:28:25  icculus
 * Used to be capitalized: BYTESWAP.H. Yuck.
 *
 * Revision 1.1.1.1  2000/04/18 00:00:38  icculus
 * initial checkin
 *
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

#ifndef _BYTESWAP_LINUX_H
#define _BYTESWAP_LINUX_H

#include "BYTESWAP.H"

#endif
