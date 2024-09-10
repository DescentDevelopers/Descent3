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

 * $Logfile: /DescentIII/Main/linux/lnxmono.cpp $
 * $Revision: 1.3 $
 * $Date: 2004/02/23 03:03:48 $
 * $Author: ryan $
 *
 * Linux monochrome routines
 *
 * $Log: lnxmono.cpp,v $
 * Revision 1.3  2004/02/23 03:03:48  ryan
 * Patched to compile with gcc3 and a modern linux distro...
 *
 * Revision 1.2  2000/05/29 05:39:59  icculus
 * Changed some exit() calls to _exit().
 *
 * Revision 1.1.1.1  2000/04/18 00:00:39  icculus
 * initial checkin
 *
 *
 * 13    8/22/99 5:52a Jeff
 * remote monochrome server code added
 *
 * 12    7/14/99 9:09p Jeff
 * added comment header
 *
 * $NoKeywords: $
 */

#include <cstdarg>
#include <cstdio>
#include <memory.h>

#include "debug.h"
#include "mono.h"

static char Mono_buffer[MAX_MONO_LENGTH];

void Debug_ConsolePrintf(int n, const char *format, ...) {
  va_list marker;
  va_start(marker, format);
  std::vsnprintf(Mono_buffer, MAX_MONO_LENGTH, format, marker);
  va_end(marker);

  if (n == 0) {
    printf("%s", Mono_buffer);
  }
}

void Debug_ConsolePrintfAt(int n, int row, int col, const char *format, ...) {}
