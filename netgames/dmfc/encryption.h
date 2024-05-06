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

 * $Logfile: /DescentIII/Main/dmfc/encryption.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:57:21 $
 * $Author: kevinb $
 *
 * header file for ICE encryption class
 *
 * $Log: encryption.h,v $
 * Revision 1.1.1.1  2003/08/26 03:57:21  kevinb
 * initial 1.5 import
 *
 *
 * 2     7/08/99 2:37a Jeff
 * created encryption class based off of ICE encryption
 *
 * $NoKeywords: $
 */

#ifndef __ICEENCRYPT_H_
#define __ICEENCRYPT_H_

#include "pstypes.h"

class IceSubkey;

class IceKey {
public:
  IceKey(int n);
  ~IceKey();

  void set(const ubyte *key);

  void encrypt(const ubyte *plaintext, ubyte *ciphertext) const;
  void decrypt(const ubyte *ciphertext, ubyte *plaintext) const;

  int keySize(void) const;
  int blockSize(void) const;

private:
  void scheduleBuild(ushort *k, int n, const int *keyrot);

  int _size;
  int _rounds;
  IceSubkey *_keysched;
};

#endif
