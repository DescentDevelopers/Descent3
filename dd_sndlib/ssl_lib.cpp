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
 * $Source: $
 * $Revision: 3 $
 * $Author: Chris $
 * $Date: 10/08/99 4:28p $
 *
 * This takes care of any code not exclusive to Win32 Ds3dlib.cpp but part of library
 *
 * $Log: /DescentIII/Main/dd_sndlib/ssl_lib.cpp $
 *
 * 3     10/08/99 4:28p Chris
 * Added the forcefield and glass breaking override options
 *
 * 2     4/06/99 8:29p Samir
 * added error check system.
 *
 * 1     4/06/99 8:16p Samir
 * Initial rev.
 *
 */

#include <cstdarg>
#include <cstdio>

#include "ssl_lib.h"
#include "pserror.h"

#include <string.h>

llsSystem::llsSystem() {
  m_lib_error_code = SSL_OK;
  m_geometry = NULL;
}

void llsSystem::CheckForErrors() {
  // if a fatal error occurred, quit and display an error
  // non fatal errors should be put inside a logfile, or just mprinted out.

  strcpy(m_error_text, "Internal Sound Error: ");
  m_lib_error_code = SSL_OK;
}

void llsSystem::ErrorText(const char *fmt, ...) {
  std::va_list arglist;
  char buf[128];
  int len, slen;

  slen = strlen(m_error_text);

  if ((slen + 2) > sizeof(m_error_text)) {
    return;
  }

  strcat(m_error_text, "\n");
  va_start(arglist, fmt);
  len = std::vsnprintf(buf, 128, fmt, arglist);
  va_end(arglist);
  if (len < 0)
    return;

  if ((slen + strlen(buf) + 1) > sizeof(m_error_text)) {
    return;
  }
  strcat(m_error_text, buf);
}
