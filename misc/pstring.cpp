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

#include <cctype>

#include "pstring.h"

// CleanupStr
//    This function strips all leading and trailing spaces, keeping internal spaces. This goes for tabs too.
std::size_t CleanupStr(char *dest, const char *src, std::size_t destlen) {
  if (destlen == 0)
    return 0;

  const char *end;
  std::size_t out_size;

  // Trim leading space
  while (std::isspace((uint8_t)*src))
    src++;

  // All spaces?
  if (*src == '\0') {
    *dest = '\0';
    return 1;
  }

  // Trim trailing space
  end = src + std::strlen(src) - 1;
  while (end > src && std::isspace((uint8_t)*end))
    end--;
  end++;

  // Set output size to minimum of trimmed string length and buffer size minus 1
  out_size = (end - src) < destlen - 1 ? (end - src) : destlen - 1;

  // Copy trimmed string and add null terminator
  std::memcpy(dest, src, out_size);
  dest[out_size] = '\0';

  return out_size;
}
