/*
* Descent 3 
* Copyright (C) 2024 Parallax Software
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * $Logfile: $
 * $Revision: $
 * $Date: $
 * $Author:  $
 *
 * <insert description of file here>
 *
 * $Log:  $
 *
 * $NoKeywords: $
 */

/* adler32.c -- compute the Adler-32 checksum of a data stream
 * Copyright (C) 1995-1998 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* @(#) $Id$ */

#include "zlib.h"

#define BASE 65521L /* largest prime smaller than 65536 */
#define NMAX 5552
/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */

#define DO1(buf, i)                                                                                                    \
  {                                                                                                                    \
    s1 += buf[i];                                                                                                      \
    s2 += s1;                                                                                                          \
  }
#define DO2(buf, i)                                                                                                    \
  DO1(buf, i);                                                                                                         \
  DO1(buf, i + 1);
#define DO4(buf, i)                                                                                                    \
  DO2(buf, i);                                                                                                         \
  DO2(buf, i + 2);
#define DO8(buf, i)                                                                                                    \
  DO4(buf, i);                                                                                                         \
  DO4(buf, i + 4);
#define DO16(buf)                                                                                                      \
  DO8(buf, 0);                                                                                                         \
  DO8(buf, 8);

/* ========================================================================= */
uLong ZEXPORT adler32(uLong adler, const Bytef *buf, uInt len) {
  unsigned long s1 = adler & 0xffff;
  unsigned long s2 = (adler >> 16) & 0xffff;
  int k;

  if (buf == Z_NULL)
    return 1L;

  while (len > 0) {
    k = len < NMAX ? len : NMAX;
    len -= k;
    while (k >= 16) {
      DO16(buf);
      buf += 16;
      k -= 16;
    }
    if (k != 0)
      do {
        s1 += *buf++;
        s2 += s1;
      } while (--k);
    s1 %= BASE;
    s2 %= BASE;
  }
  return (s2 << 16) | s1;
}
