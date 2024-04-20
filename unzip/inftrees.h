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
 * $Logfile: /DescentIII/Main/unzip/inftrees.h $
 * $Revision: 2 $
 * $Date: 8/13/99 8:01p $
 * $Author: Jeff $
 *
 * <insert description of file here>
 *
 * $Log: /DescentIII/Main/unzip/inftrees.h $
 *
 * 2     8/13/99 8:01p Jeff
 * initial creation of zip class
 *
 * $NoKeywords: $
 */

/* inftrees.h -- header to use inftrees.c
 * Copyright (C) 1995-1998 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* WARNING: this file should *not* be used by applications. It is
   part of the implementation of the compression library and is
   subject to change. Applications should only use zlib.h.
 */

/* Huffman code lookup table entry--this entry is four bytes for machines
   that have 16-bit pointers (e.g. PC's in the small or medium model). */

typedef struct inflate_huft_s FAR inflate_huft;

struct inflate_huft_s {
  union {
    struct {
      Byte Exop; /* number of extra bits or operation */
      Byte Bits; /* number of bits in this code or subcode */
    } what;
    uInt pad; /* pad structure to a power of 2 (4 bytes for */
  } word;     /*  16-bit, 8 bytes for 32-bit int's) */
  uInt base;  /* literal, length base, distance base,
                 or table offset */
};

/* Maximum size of dynamic tree.  The maximum found in a long but non-
   exhaustive search was 1004 huft structures (850 for length/literals
   and 154 for distances, the latter actually the result of an
   exhaustive search).  The actual maximum is not known, but the
   value below is more than safe. */
#define MANY 1440

extern int inflate_trees_bits OF((uIntf *,             /* 19 code lengths */
                                  uIntf *,             /* bits tree desired/actual depth */
                                  inflate_huft *FAR *, /* bits tree result */
                                  inflate_huft *,      /* space for trees */
                                  z_streamp));         /* for messages */

extern int inflate_trees_dynamic OF((uInt,                /* number of literal/length codes */
                                     uInt,                /* number of distance codes */
                                     uIntf *,             /* that many (total) code lengths */
                                     uIntf *,             /* literal desired/actual bit depth */
                                     uIntf *,             /* distance desired/actual bit depth */
                                     inflate_huft *FAR *, /* literal/length tree result */
                                     inflate_huft *FAR *, /* distance tree result */
                                     inflate_huft *,      /* space for trees */
                                     z_streamp));         /* for messages */

extern int inflate_trees_fixed OF((uIntf *,             /* literal desired/actual bit depth */
                                   uIntf *,             /* distance desired/actual bit depth */
                                   inflate_huft *FAR *, /* literal/length tree result */
                                   inflate_huft *FAR *, /* distance tree result */
                                   z_streamp));         /* for memory allocation */
