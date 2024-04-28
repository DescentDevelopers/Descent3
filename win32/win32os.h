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

/*	Win32 OS Specific internal header to win32 library */

#ifndef WIN32OS_H
#define WIN32OS_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "gameos.h"

typedef struct win32oss_info {
  HWND hwnd;
  HINSTANCE hinst;
} win32oss_info;

#endif
