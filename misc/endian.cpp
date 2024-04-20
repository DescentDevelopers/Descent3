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
 * $Logfile: /DescentIII/Main/misc/endian.cpp $
 * $Revision: 5 $
 * $Date: 10/21/99 2:29p $
 * $Author: Kevin $
 *
 * Endian detection and usage routines
 *
 * $Log: /DescentIII/Main/misc/endian.cpp $
 *
 * 5     10/21/99 2:29p Kevin
 * Mac Merge
 *
 * 4     5/05/99 5:27a Jeff
 * renamed endian.h to psendian.h
 *
 * 3     5/01/99 8:47p Jeff
 * removed 2 unused functions
 *
 * 2     5/01/99 2:52p Jeff
 * added automatic endian detection of the system
 *
 * $NoKeywords: $
 */

#include "psendian.h"
#include "mono.h"

#define SWAPSHORT(x) ((0xffff & ((x) << 8) | (((unsigned short)(x)) >> 8)))
#define SWAPINT(x) (((x) << 24) | (((unsigned long)(x)) >> 24) | (((x) & 0x0000ff00) << 8) | (((x) & 0x00ff0000) >> 8))
inline float SWAPFLOAT(float x) {
  int i = SWAPINT(*((int *)&(x)));
  return *((float *)&(i));
}

#define ENDIAN_BIG_ENDIAN 0
#define ENDIAN_LITTLE_ENDIAN 1
signed char Endian_type = -1;

// Endian_IsLittleEndian
//
//	Returns true if the machine is Little Endian (i.e. 80x86)
//	Returns false if the machine is Big Endian (i.e. Macintosh)
bool Endian_IsLittleEndian(void) {
  char c_vals[2] = {0x00, 0x01};
  short *s_val;

  s_val = (short *)c_vals;

  if ((*s_val) == 0x01) {
    // we have a Big Endian machine
    mprintf((0, "Machine is BIG ENDIAN format\n"));
    Endian_type = ENDIAN_BIG_ENDIAN;
    return false;
  }

  // else s_val == 256
  // we have a Little Endian machine
  mprintf((0, "Machine is LITTLE ENDIAN format\n"));
  Endian_type = ENDIAN_LITTLE_ENDIAN;
  return true;
}

// Swaps (if needed) a short value (2 bytes) (assumes incoming value is in little endian format)
short Endian_SwapShort(short value) {
  if (Endian_type == -1)
    Endian_IsLittleEndian(); // detect endianness

  if (Endian_type == ENDIAN_BIG_ENDIAN)
    value = SWAPSHORT(value);

  return value;
}

// Swaps (if needed) an int value (4 bytes) (assumes incoming value is in little endian format)
int Endian_SwapInt(int value) {
  if (Endian_type == -1)
    Endian_IsLittleEndian(); // detect endianness

  if (Endian_type == ENDIAN_BIG_ENDIAN)
    value = SWAPINT(value);

  return value;
}

// Swaps (if needed) a float value (4 bytes) (assumes incoming value is in little endian format)
float Endian_SwapFloat(float value) {
  if (Endian_type == -1)
    Endian_IsLittleEndian(); // detect endianness

  if (Endian_type == ENDIAN_BIG_ENDIAN)
    value = SWAPFLOAT(value);

  return value;
}
