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
 * $Logfile: /DescentIII/Main/lib/mac/fixmac.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:58:13 $
 * $Author: kevinb $
 *
 * $Log: fixmac.h,v $
 * Revision 1.1.1.1  2003/08/26 03:58:13  kevinb
 * initial 1.5 import
 *
 *
 * 2     5/10/99 10:55p Ardussi
 * changes to compile on Mac
 *
 * 2     2/27/97 5:09 PM Jeremy
 * added inline definitions of fixmul/div.  Taken from Descent II's fixc.c
 *
 * 1     2/27/97 4:44 PM Jeremy
 * inline assembly language functions for PowerPC for
 * fixmul/fixdiv/fixmuldiv
 *
 * $NoKeywords: $
 */

#ifndef _FIXMAC_H
#define _FIXMAC_H

#ifdef NEEDED
inline asm fix FixMul(fix a, fix b) {
  mullw r5, r3,
      r4 // multiply low word
          mulhw r6,
      r3,
      r4 // multiply high word (signed)
          rlwinm r3,
      r5, 16, 16,
      31 // mask off low portion of result
      rlwimi r3,
      r6, 16, 0,
      15  // insert high portion of result
      blr // return to caller
}

inline fix FixDiv(fix a, fix b) { return (fix)(((double)a * 65536.0) / (double)b); }

inline fix FixMulDiv(fix a, fix b, fix c) {
  double d;

  d = (double)a * (double)b;
  return (fix)(d / (double)c);
}
#endif

#endif
