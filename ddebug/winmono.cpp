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
#include <cstdlib>

#include "Debug.h"

#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

// ---------------------------------------------------------------------------
// console debugging functions

#ifndef RELEASE
#define MAX_MONO_LENGTH 2048
#else
#define MAX_MONO_LENGTH 256
#endif

static char Mono_buffer[MAX_MONO_LENGTH];

static int Debug_logfile = -1;

void Debug_LogClose();

bool Debug_Logfile(const char *filename) {
  if (Debug_logfile == -1) {
    Debug_logfile = _open(filename, _O_CREAT | _O_WRONLY | _O_TEXT, _S_IREAD | _S_IWRITE);
    if (Debug_logfile == -1) {
      Debug_MessageBox(OSMBOX_OK, "Debug", "FYI Logfile couldn't be created.");
      return false;
    }
    atexit(Debug_LogClose);
  }
  Debug_LogWrite("BEGINNING LOG\n\n");

  return true;
}

void Debug_LogWrite(const char *str) {
  if (Debug_logfile > -1)
    _write(Debug_logfile, str, strlen(str));
}

void Debug_LogClose() {
  if (Debug_logfile > -1) {
    Debug_LogWrite("\nEND LOG");
    _close(Debug_logfile);
    Debug_logfile = -1;
  }
}

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
