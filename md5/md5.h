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

        This is the C++ implementation of the MD5 Message-Digest
        Algorithm desrcipted in RFC 1321.
        I translated the C code from this RFC to C++.
        There is now warranty.

        Feb. 12. 2005
        Benjamin Grüdelbach
*/

/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
 */

#pragma once

typedef unsigned char *POINTER;
typedef unsigned int uint32_t;
/* MD5 context. */
typedef struct _context_md5_t {
  uint32_t buf[4];
  uint32_t bits[2];
  unsigned char in[64];
} context_md5_t;

typedef context_md5_t MD5_CTX;

class MD5 {

private:
  MD5_CTX ctx;
  void MD5Transform(uint32_t buf[4], uint32_t const in[16]);
  void Encode(unsigned char *, unsigned int *, unsigned int);
  void Decode(unsigned int *, unsigned char *, unsigned int);
  void MD5_memcpy(POINTER, POINTER, unsigned int);
  void MD5_memset(POINTER, int, unsigned int);

public:
  void MD5Init();

  void MD5Update(unsigned char *, unsigned int);
  void MD5Update(float val);
  void MD5Update(int val);
  void MD5Update(unsigned int val);
  void MD5Update(unsigned char val);
  void MD5Update(short val);

  void MD5Final(unsigned char[16]);

  ~MD5();
  MD5(){};

  MD5 *Clone();
  static void Destroy(MD5 *obj);
};
