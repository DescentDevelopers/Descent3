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

 * $Logfile: /DescentIII/Main/linux/lnxcon.cpp $
 * $Revision: 1.2 $
 * $Date: 2004/02/25 00:04:06 $
 * $Author: ryan $
 *
 * <insert description of file here>
 *
 * $Log: lnxcon.cpp,v $
 * Revision 1.2  2004/02/25 00:04:06  ryan
 * Removed loki_utils dependency and ported to MacOS X (runs, but incomplete).
 *
 * Revision 1.1.1.1  2000/04/18 00:00:39  icculus
 * initial checkin
 *
 *
 * 7     7/19/99 12:54p Jeff
 * created lnxcon_raw.cpp so ncurses is now only used for SVGALib, cuts
 * down on redraw
 *
 * 6     6/24/99 4:43p Jeff
 * removed ncurses dependency
 *
 * 5     4/22/99 2:04a Jeff
 * support for null console
 *
 * 4     4/19/99 4:52p Jeff
 * removed useless calls to curses lib for keyboard setup
 *
 * 3     4/19/99 3:58a Jeff
 * got console working again (keyboard based off ddio)
 *
 * 2     4/14/99 1:59a Jeff
 * fixed case mismatched #includes
 *
 * 1     1/12/99 3:42a Jeff
 *
 * $NoKeywords: $
 */

#include <cstdarg>
#include <cstring>
#include <cstdio>
#include <cctype>

#include "application.h"
#include "AppConsole.h"

//////////////////////////////////////////////////
// Defines
#define CON_MAX_STRINGLEN 768

enum { Console_null, Console_raw } Console_mode;

////////////////////////////////////////////////
// NULL driver functions
void con_null_Printf(const char *fmt, ...);
bool con_null_Input(char *buf, int buflen);
void con_null_Defer();
bool con_null_Create();
void con_null_Destroy();
void con_null_Puts(int window, const char *str);

////////////////////////////////////////////////
// raw driver functions
void con_raw_Printf(const char *fmt, ...);
bool con_raw_Input(char *buf, int buflen);
void con_raw_Defer();
bool con_raw_Create();
void con_raw_Destroy();
void con_raw_Puts(int window, const char *str);

//////////////////////////////////////////////////
// Global Variables
char *Con_read_buf = nullptr;                 // The next buffer of text from user input
bool Con_init = false;                        // Console has been initialized

#ifdef mem_malloc
#undef mem_malloc
#endif
#ifdef mem_free
#undef mem_free
#endif
#define mem_malloc(x) malloc(x)
#define mem_free(x) free(x)

//////////////////////////////////////////////////
// Prototypes
void con_Defer();         // Performs the actions for the frame
//////////////////////////////////////////////////
// Functions

void con_Printf(const char *fmt, ...) {
  if (!Con_init) // console hasn't been initialized yet, int16_t circuit
    return;

  char buf[CON_MAX_STRINGLEN];
  va_list args;

  //	filter out messages
  va_start(args, fmt);
  std::vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);

  //	filter out unprintable characters
  char *p, *fp, filter_buf[CON_MAX_STRINGLEN];
  p = buf;
  fp = filter_buf;

  while (*p) {
    if (*p == 0x01) {
      // this is a color, skip the next 3
      p += 4;
    } else {
      if (isalnum(*p) || ispunct(*p) || (*p == ' ') || (*p == '\n') || (*p == '\r') || (*p == '\b')) {
        *fp = *p;
        fp++;
      }
      p++;
    }
  }
  *fp = '\0';

  switch (Console_mode) {
  case Console_null:
    con_null_Puts(0, filter_buf);
    break;
  case Console_raw:
    con_raw_Puts(0, filter_buf);
    break;
  default:
    break;
  }
}

bool con_Input(char *buf, int buflen) {
  if (!Con_init) { // the console hasn't been initialized yet
    *buf = '\0';
    return false;
  }

  if (Console_mode == Console_null) {
    *buf = '\0';
    return false;
  }

  if (Console_mode == Console_raw) {
    return con_raw_Input(buf, buflen);
  }

  if (!Con_read_buf) { // there is no read buffer...yipes
    *buf = '\0';
    return false;
  }

  if (Con_read_buf[0]) {
    // we have a new buffer of input...send it away
    strncpy(buf, Con_read_buf, buflen - 1);
    buf[buflen - 1] = 0;
    Con_read_buf[0] = 0;
    return true;
  }

  return false;
}

void con_Defer() {
  if (!Con_init) {
    // the console hasn't been initialized yet
    return;
  }

  if (Console_mode == Console_null) {
    con_null_Defer();
    return;
  }

  if (Console_mode == Console_raw) {
    con_raw_Defer();
    return;
  }
}

bool con_Create(int flags) {
  // determine what type of console to open up
  if (flags & APPFLAG_USESERVICE) {
    // use the NULL driver!
    Console_mode = Console_null;
    Con_init = con_null_Create();
    return Con_init;
  } else {
    // use stdout driver
    Console_mode = Console_raw;
    Con_init = con_raw_Create();
    return Con_init;
  }
}

void con_Destroy() {
  if (Console_mode == Console_null) {
    con_null_Destroy();
    return;
  }
  if (Console_mode == Console_raw) {
    con_raw_Destroy();
    return;
  }
}
