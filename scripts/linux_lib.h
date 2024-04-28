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

#ifndef __LINUX_LIB_H_
#define __LINUX_LIB_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void _splitpath(const char *path, char *drive, char *dir, char *fname, char *ext);
int _vsnprintf(char *buffer, size_t count, const char *format, va_list argptr);
int stricmp(const char *string1, const char *string2);

#endif
