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

--- HISTORICAL COMMENTS FOLLOW ---

 * $Logfile: /DescentIII/Main/ddgr_win32/ddgrWin32.h $
 * $Revision: 3 $
 * $Date: 4/23/98 6:38p $
 * $Author: Jason $
 *
 *	DDGR v2.0 Win32 Implementation
 *
 * $Log: /DescentIII/Main/ddgr_win32/ddgrWin32.h $
 *
 * 3     4/23/98 6:38p Jason
 * made bitmaps use 1555 format
 *
 * 2     9/12/97 4:13p Samir
 * Added some private data access functions and more DirectX
 * functionality.
 *
 * 1     6/12/97 6:33p Samir
 * Initial revision
 *
 * $NoKeywords: $
 */

#ifndef DDGRWIN32_H
#define DDGRWIN32_H

#include "ddgr.h"
#include "pstypes.h"

/*	Subsystem constants

        as of v2.0
*/
const int DDGR_GDI_SUBSYSTEM = 1, // GDI subsystem
    DDGR_GDIX_SUBSYSTEM = 2,      // GDIX subsystem (GDI+DirectX)
    DDGR_DX_SUBSYSTEM = 3;        // DIRECTX subsystem

/*	Internal Library Data

*/
struct tDDGRInternalData {
  bool init;     // Is library currently initialized
  int subsystem; // Current active subsystem
};

/*	Global externs
 */
extern tDDGRInternalData DDGR_lib_data;
extern char *DDGR_subsystem_names[];
extern int DDGR_subsystems[];

/*	Macros
 */
#define LIB_DATA(_c) DDGR_lib_data._c

inline unsigned DDGR_MAKERGB16(ddgr_color c) {
  uint8_t r, g, b;
  r = (uint8_t)((c & 0x00ff0000) >> 16);
  g = (uint8_t)((c & 0x0000ff00) >> 8);
  b = (uint8_t)(c & 0x000000ff);

  return (((r >> 3) << 10) + ((g >> 3) << 5) + (b >> 3));
}

inline int DDGR_COLOR_RED(ddgr_color c) {
  unsigned r = (uint8_t)((c & 0x00ff0000) >> 16);
  return (int)r;
}

inline int DDGR_COLOR_GREEN(ddgr_color c) {
  unsigned g = (uint8_t)((c & 0x0000ff00) >> 8);
  return (int)g;
}

inline int DDGR_COLOR_BLUE(ddgr_color c) {
  unsigned b = (uint8_t)(c & 0x000000ff);
  return (int)b;
}

/*	Functions Internal
 */

void ddgr_FatalError(const char *fmt, ...);
void ddgr_PushError(const char *fmt, ...);

#endif
