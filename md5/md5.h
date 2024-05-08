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

#pragma once

#include <openssl/evp.h>

#include "log.h"

class MD5 {
private:
  OSSL_LIB_CTX *library_context = nullptr;
  EVP_MD *message_digest = nullptr;
  EVP_MD_CTX *digest_context = nullptr;

public:
  MD5() {
    if ((library_context = OSSL_LIB_CTX_new()) == nullptr)
      spdlog::get("console")->error("OSSL_LIB_CTX_new() returned NULL.");

    if ((message_digest = EVP_MD_fetch(library_context, "MD5", nullptr)) == nullptr)
      spdlog::get("console")->error("EVP_MD_fetch could not find MD5.");

    if ((digest_context = EVP_MD_CTX_new()) == nullptr)
      spdlog::get("console")->error("EVP_MD_CTX_new returned NULL.");

    if (EVP_DigestInit(digest_context, message_digest) != 1)
      spdlog::get("console")->error("EVP_DigestInit failed.");
  }

  MD5(MD5 *md5) {
#if (OPENSSL_VERSION_MAJOR > 3) || ((OPENSSL_VERSION_MAJOR == 3) && (OPENSSL_VERSION_MINOR > 0))
    if ((digest_context = EVP_MD_CTX_dup(md5->digest_context)) == nullptr)
      spdlog::get("console")->error("EVP_MD_CTX_dup returned NULL.");
#else
    if (EVP_MD_CTX_copy(digest_context, md5->digest_context) == 0)
      spdlog::get("console")->error("EVP_MD_CTX_copy returned NULL.");
#endif
  }

  ~MD5() {
    EVP_MD_CTX_free(digest_context);
    EVP_MD_free(message_digest);
    OSSL_LIB_CTX_free(library_context);
  }

  void MD5Init() {}

  template <typename T> void MD5Update(T value) { MD5Update((unsigned char *)&value, sizeof(T)); }

  void MD5Update(unsigned char *message, std::size_t count) {
    if (EVP_DigestUpdate(digest_context, message, count) != 1)
      spdlog::get("console")->error("EVP_DigestUpdate failed.");
  }

  void MD5Final(unsigned char *digest) {
    unsigned int digest_length = 0;
    if ((digest_length = EVP_MD_get_size(message_digest)) <= 0)
      spdlog::get("console")->error("EVP_MD_get_size returned invalid size.");

    if (EVP_DigestFinal(digest_context, digest, &digest_length) != 1)
      spdlog::get("console")->error("EVP_DigestFinal() failed.");
  }

  MD5 *Clone() { return new MD5{this}; }

  static void Destroy(MD5 *md5) { delete md5; }
};
