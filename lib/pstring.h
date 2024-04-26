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
 * $Logfile: /DescentIII/Main/lib/pstring.h $
 * $Revision: 3 $
 * $Date: 12/16/98 1:57p $
 * $Author: Samir $
 *
 * Safe string manipulation and creation functions
 *
 * $Log: /DescentIII/Main/lib/pstring.h $
 *
 * 3     12/16/98 1:57p Samir
 * Replaced CleanupString2 with CleanupStr
 *
 * 2     11/01/98 1:56a Jeff
 * added pstring.cpp/.h
 *
 * $NoKeywords: $
 */

#ifndef _PSTRING_H_
#define _PSTRING_H_

#include <stdarg.h>

//	Pvsprintf
//		Similar to vsprintf/_vsnprintf, however handles the case of a buffer overflow.  Arguments are the
//	same as _vsnprintf.  In the case of count, pass in the size of the buffer, in the case where there is
//	a buffer overflow, a \0 will be tacked on as the last byte in the buffer, ensuring a valid string.
int Pvsprintf(char *buffer, int count, const char *format, va_list argptr);

//	Psprintf
//		Similar to sprintf/_snprintf, however handles the case of a buffer overflow.  Arguments are the
//	same as _snprintf.  In the case of count, pass in the size of the buffer, in the case where there is
//	a buffer overflow, a \0 will be tacked on as the last byte in the buffer, ensuring a valid string.
int Psprintf(char *buffer, int count, const char *format, ...);

// CleanupStr
//		this function strips all leading and trailing spaces, keeping internal spaces.  this goes
//		for tabs too.
int CleanupStr(char *dest, const char *src, int destlen);

// tStringTok
//		you may start a string tokenization object by calling the start function
//		then call next, to get the following tokens.
//		note that this class uses it's own copy of the string to ensure that strtok doesn't
//		get corrupted.

class tStringTok {
  char *m_strbuf;
  char *m_curptr;

public:
  tStringTok() : m_strbuf(0), m_curptr(0){};
  ~tStringTok();

  char *start(const char *str, const char *tokens);
  char *next(const char *tokens);
};

#endif
