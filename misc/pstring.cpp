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
 * $Logfile: /DescentIII/Main/misc/pstring.cpp $
 * $Revision: 4 $
 * $Date: 4/15/99 1:51a $
 * $Author: Jeff $
 *
 * Safe string manipulation and creation functions
 *
 * $Log: /DescentIII/Main/misc/pstring.cpp $
 *
 * 4     4/15/99 1:51a Jeff
 * changes for linux compile
 *
 * 3     12/16/98 1:57p Samir
 * Replaced CleanupString2 with CleanupStr
 *
 * 2     11/01/98 1:56a Jeff
 * added pstring.cpp/.h
 *
 * $NoKeywords: $
 */

#include <string.h>

#include "pstring.h"

// CleanupStr
//		this function strips all leading and trailing spaces, keeping internal spaces.  this goes
//		for tabs too.
int CleanupStr(char *dest, const char *src, int destlen) {
  int i, j, err, begin = 0, end = 0, len;

  err = 0;

  len = strlen(src);
  for (i = 0; i < len; i++) {
    char ch;
    ch = src[i];

    //	 mark beginning.
    if ((ch > ' ' && ch > '\t') && err < 1) {
      err = 1;
      begin = i;
      end = i;
    } else if (ch == ' ' && err == 1) {
      err = 2;
      end = i;
    } else if (ch > ' ' && err >= 1) {
      end = i;
    }
  }

  j = 0;
  for (i = begin; i < (end + 1); i++) {
    char ch;
    ch = src[i];
    if (j == destlen - 1)
      break;
    if (ch != '\"')
      dest[j++] = ch;
  }

  dest[j] = 0;

  return 0;
}
