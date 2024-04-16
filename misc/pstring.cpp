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
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "pstring.h"

#ifdef __LINUX__
#include "lnxfix.h"
#endif

//	Pvsprintf
//		Similar to vsprintf/_vsnprintf, however handles the case of a buffer overflow.  Arguments are the
//	same as _vsnprintf.  In the case of count, pass in the size of the buffer, in the case where there is
//	a buffer overflow, a \0 will be tacked on as the last byte in the buffer, ensuring a valid string.
int Pvsprintf(char *buffer, int count, const char *format, va_list argptr) {
  int ret = _vsnprintf(buffer, count - 1, format, argptr);
  if (ret == -1)
    buffer[count - 1] = '\0';
  return ret;
}

//	Psprintf
//		Similar to sprintf/_snprintf, however handles the case of a buffer overflow.  Arguments are the
//	same as _snprintf.  In the case of count, pass in the size of the buffer, in the case where there is
//	a buffer overflow, a \0 will be tacked on as the last byte in the buffer, ensuring a valid string.
int Psprintf(char *buffer, int count, const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  int ret = Pvsprintf(buffer, count, format, ap);
  va_end(ap);
  return ret;
}

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

// tStringTok
//		you may start a string tokenization object by calling the start function
//		then call next, to get the following tokens.
//		note that this class uses it's own copy of the string to ensure that strtok doesn't
//		get corrupted.

tStringTok::~tStringTok() {
  if (m_strbuf)
    delete[] m_strbuf;
}

char *tStringTok::start(const char *str, const char *tokens) {
  // if we pass a null string, then reset this object
  if (str == NULL) {
    if (m_strbuf)
      delete[] m_strbuf;
    m_strbuf = NULL;
    m_curptr = NULL;
    return NULL;
  }

  char *new_str = new char[strlen(str) + 1];

  //	copy string into new string buffer.  AFTER THIS, delete the current string buffer, since the pointer
  //	passed in could point to the current m_strbuf ptr.
  strcpy(new_str, str);

  if (m_strbuf)
    delete[] m_strbuf;
  m_strbuf = new_str;
  m_curptr = m_strbuf;

  return this->next(tokens);
}

char *tStringTok::next(const char *tokens) {
  // create string by terminating m_strbuf when a token is it.
  char *cur_str, *end_str;
  int slen2, j;

  if (!m_curptr)
    return m_curptr;

  slen2 = strlen(tokens);
  cur_str = NULL;

  for (j = 0; j < slen2; j++) {
    end_str = strchr(m_curptr, tokens[j]);
    if (end_str) {
      *end_str = 0;
      cur_str = m_curptr;
      m_curptr = end_str + 1;
      return cur_str;
    }
  }
  //	at this point, we found no tokens, so m_curptr will point to the string we want to return.
  // then we set m_curptr to NULL, telling any subsequent calls to next to return NULL.

  cur_str = m_curptr;
  m_curptr = NULL;

  return cur_str;
}
