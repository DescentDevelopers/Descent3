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

 * $Logfile: /DescentIII/Main/linux/lnxdebug.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2000/04/18 00:00:39 $
 * $Author: icculus $
 *
 * Linux debugging routines
 *
 * $Log: lnxdebug.cpp,v $
 * Revision 1.1.1.1  2000/04/18 00:00:39  icculus
 * initial checkin
 *
 *
 * 8     7/22/99 1:21p Jeff
 * added 3rd debugging window
 *
 * 7     7/14/99 9:09p Jeff
 * added comment header
 *
 * $NoKeywords: $
 */

#include <cstdio>
#include <SDL3/SDL_assert.h>

#include "debug.h"

///////////////////////////////////////////////////////////////////////////////

// Does a messagebox with a stack dump
// Messagebox shows topstring, then stack dump, then bottomstring
// Return types are the same as the Windows return values
int Debug_ErrorBox(int type, const char *topstring, const char *title, const char *bottomstring) {
  int answer = 0;

  fprintf(stderr, "\n%s (%s)\n\n%s\n\n%s\n", topstring, title, "System Error", bottomstring);

  SDL_TriggerBreakpoint();

  return answer;
}

// displays a message box
// Returns the same values as the Win32 MessageBox() function
int Debug_MessageBox(int type, const char *title, const char *str) {
  return Debug_ErrorBox(type, str, "Descent 3 Message", "");
}

///////////////////////////////////////////////////////////////////////////////
