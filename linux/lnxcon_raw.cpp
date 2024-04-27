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
 * $Logfile: /DescentIII/Main/linux/lnxcon_raw.cpp $
 * $Revision: 1.2 $
 * $Date: 2004/02/25 00:04:06 $
 * $Author: ryan $
 *
 * stdout console driver
 *
 * $Log: lnxcon_raw.cpp,v $
 * Revision 1.2  2004/02/25 00:04:06  ryan
 * Removed loki_utils dependency and ported to MacOS X (runs, but incomplete).
 *
 * Revision 1.1.1.1  2000/04/18 00:00:39  icculus
 * initial checkin
 *
 *
 * 2     7/19/99 12:54p Jeff
 * created lnxcon_raw.cpp so ncurses is now only used for SVGALib, cuts
 * down on redraw
 *
 * $NoKeywords: $
 */

#include "DDAccess.h"
#include "application.h"
#include "AppConsole.h"
#include "TaskSystem.h"
#include "mono.h"
// #include "local_malloc.h"
#include "pstring.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

#include <algorithm>

static char *Con_raw_read_buf = NULL;                     // The next buffer of text from user input
static char *Con_raw_inp_buf = NULL, Con_raw_inp_pos = 0; // Currently updating input buffer of text (and it's position)
static char Con_raw_last_command[CON_MAX_STRINGLEN];      // The last command entered by the user
static int Con_raw_cols = 0, Con_raw_rows = 0; // The size of the main window (input window is (1 row, Con_cols))
static bool Con_raw_newline = false;

// put some data up on the screen
void con_raw_Puts(int window, const char *str);

void con_raw_Printf(const char *fmt, ...) {
  char buffer[1024];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);
  con_raw_Puts(0, buffer);
}

bool con_raw_Input(char *buf, int buflen) {
  if (!Con_raw_read_buf) { // there is no read buffer...yipes
    *buf = '\0';
    return false;
  }

  if (Con_raw_read_buf[0]) {
    // we have a new buffer of input...send it away
    strncpy(buf, Con_raw_read_buf, buflen - 1);
    buf[buflen - 1] = 0;
    Con_raw_read_buf[0] = 0;
    return true;
  }

  return false;
}

int ddio_KeyInKey();
int ddio_KeyToAscii(int code);

void con_raw_Defer(void) {
  int keypressed = ddio_KeyInKey();

  while (keypressed != 0) {
    // we got a key...handle it
    switch (keypressed) {
    case 0xCB: // KEY_LEFT:       // Left arrow
    case 0xCD: // KEY_RIGHT:      // Right arrow
    case 0xC8: // KEY_UP:			// Up arrow
    case 0xD3: // KEY_DELETE:     // Delete
    case 0x01: // KEY_ESC:        // Escape
      break;
    case 0x0E: // KEY_BACKSP:
    {
      // Move the caret back one space, and then
      // process this like the DEL key.
      if (Con_raw_inp_pos > 0) {
        Con_raw_inp_pos--;

        for (int x = Con_raw_inp_pos; x < Con_raw_cols; x++)
          Con_raw_inp_buf[x] = Con_raw_inp_buf[x + 1];

        con_raw_Puts(-1, "\b \b");
      }
    } break;
    case 0x1C: // KEY_ENTER:
    {
      // Go to the beginning of the next line.
      // The bottom line wraps around to the top.
      strcpy(Con_raw_read_buf, Con_raw_inp_buf);
      strcat(Con_raw_inp_buf, "\n");

      // go to the next line
      con_raw_Puts(-1, "\n");

      // only save the buffer if there is text in the buffer
      char *p = Con_raw_read_buf;
      while (*p) {
        if (isalnum(*p)) {
          strcpy(Con_raw_last_command, Con_raw_read_buf);
          break;
        }
        p++;
      }

      memset(Con_raw_inp_buf, 0, Con_raw_cols + 4);
      Con_raw_inp_pos = 0;
    } break;
    default:
      if (Con_raw_inp_pos < (Con_raw_cols - 2)) {
        // Add the character to the text buffer.
        unsigned char str[2];
        str[0] = ddio_KeyToAscii(keypressed);
        str[1] = 0;
        if (str[0] != 255) {
          Con_raw_inp_buf[Con_raw_inp_pos] = str[0];
          Con_raw_inp_buf[Con_raw_inp_pos + 1] = '\0';
          Con_raw_inp_pos++;
          con_raw_Puts(-1, (const char *)str);
        }
      }
    }

    keypressed = ddio_KeyInKey();
  }
}

bool con_raw_Create(void) {
  Con_raw_cols = 80;
  Con_raw_rows = 24; // one less, since the bottom window takes up one row

  // allocate any memory needed for buffers
  Con_raw_inp_buf = (char *)malloc(sizeof(char) * (Con_raw_cols + 4));
  Con_raw_read_buf = (char *)malloc(sizeof(char) * (Con_raw_cols + 4));
  if (!Con_raw_inp_buf || !Con_raw_read_buf) {
    // error allocating memory
    return false;
  }
  memset(Con_raw_inp_buf, 0, sizeof(char) * (Con_raw_cols + 4));
  memset(Con_raw_read_buf, 0, sizeof(char) * (Con_raw_cols + 4));
  Con_raw_last_command[0] = '\0';

  Con_raw_inp_pos = 0;

  return true;
}

void con_raw_Destroy(void) {
  // free any allocated memory
  if (Con_raw_inp_buf) {
    free(Con_raw_inp_buf);
    Con_raw_inp_buf = NULL;
  }

  if (Con_raw_read_buf) {
    free(Con_raw_read_buf);
    Con_raw_read_buf = NULL;
  }
  Con_raw_cols = Con_raw_rows = 0;
}

// put some data up on the screen
void con_raw_Puts(int window, const char *str) {
  fprintf(stdout, str, strlen(str));
  fflush(stdout);
}
