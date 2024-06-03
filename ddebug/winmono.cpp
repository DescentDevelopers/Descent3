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

#ifndef RELEASE

#include <cstdarg>
#include <cstdio>

#include <windows.h>
#include <fcntl.h>

#include "debug.h"
#include "mono.h"

// ---------------------------------------------------------------------------
// console debugging functions

static char Mono_buffer[MAX_MONO_LENGTH];

void Debug_ConsolePrintf(int n, const char *format, ...) {
  char *ptr = Mono_buffer;
  std::va_list args;

  va_start(args, format);
  std::vsnprintf(ptr, MAX_MONO_LENGTH, format, args);
  va_end(args);

  if (strlen(ptr) >= MAX_MONO_LENGTH) {
    return;
  }

  Debug_LogWrite(Mono_buffer);

  if (n == 0) {
    OutputDebugString(Mono_buffer);
  }
}

void Debug_ConsolePrintfAt(int n, int row, int col, const char *format, ...) {}

#endif
