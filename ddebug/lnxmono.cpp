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
#include <cstdlib>

#include "debug.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <memory.h>

#include <unistd.h>

static int Debug_logfile = 0;

void Debug_LogClose();

bool Debug_Logfile(const char *filename) {
  if (Debug_logfile == -1) {
    Debug_logfile = open(filename, O_CREAT | O_WRONLY, S_IREAD | S_IWRITE);
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
    write(Debug_logfile, str, strlen(str));
}

void Debug_LogClose() {
  if (Debug_logfile > -1) {
    Debug_LogWrite("\nEND LOG");
    close(Debug_logfile);
    Debug_logfile = -1;
  }
}

#ifdef _DEBUG
#define MAX_MONO_BUFFER 2048
#else
#define MAX_MONO_BUFFER 32
#endif

static char Mono_buffer[MAX_MONO_BUFFER];

void Debug_ConsolePrintf(int n, const char *format, ...) {
  va_list marker;
  va_start(marker, format);
  std::vsnprintf(Mono_buffer, MAX_MONO_BUFFER, format, marker);
  va_end(marker);

  if (n == 0) {
    printf("%s", Mono_buffer);

    int end = strlen(Mono_buffer) - 1;
    if ((end > 1) && (Mono_buffer[end] != 0x0a) && (Mono_buffer[end] != 0x0d)) {
      printf("\n");
    }
  }
}

void Debug_ConsolePrintfAt(int n, int row, int col, const char *format, ...) {}
