/*
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