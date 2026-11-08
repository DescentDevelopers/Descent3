/*
 * Descent 3
 * Copyright (C) 2026 Descent Developers
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

#include <cstdint>
#include <random>
#include <vector>

#include <gtest/gtest.h>

#include "encryption.h"

#include "support/ice_util.h"

// IceKey is an implementation of Matthew Kwan's ICE block cipher. The expected ciphertexts below are the
// published reference test vectors for the cipher (plaintext fedcba9876543210 at levels 0, 1 and 2), so they
// are independent of this implementation.

using namespace dmfc_test;

namespace {

constexpr Block kVectorPlaintext = {0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10};

} // namespace

// Level 0 ("Thin-ICE") is what DMFC's remote administration uses (IceKey ice(0) in dmfcremote.cpp).
TEST(IceKey, ThinIceMatchesPublishedTestVector) {
  const uint8_t key_bytes[8] = {0xde, 0xad, 0xbe, 0xef, 0x01, 0x23, 0x45, 0x67};

  IceKey key(0);
  key.set(key_bytes);

  EXPECT_EQ(Hex(Encrypt(key, kVectorPlaintext)), "de240d83a00a9cc0");

  const Block published_ciphertext = {0xde, 0x24, 0x0d, 0x83, 0xa0, 0x0a, 0x9c, 0xc0};
  EXPECT_EQ(Decrypt(key, published_ciphertext), kVectorPlaintext);
}

TEST(IceKey, Level1MatchesPublishedTestVector) {
  const uint8_t key_bytes[8] = {0xde, 0xad, 0xbe, 0xef, 0x01, 0x23, 0x45, 0x67};

  IceKey key(1);
  key.set(key_bytes);

  EXPECT_EQ(Hex(Encrypt(key, kVectorPlaintext)), "7d6ef1ef30d47a96");
}

TEST(IceKey, Level2MatchesPublishedTestVector) {
  const uint8_t key_bytes[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                                 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

  IceKey key(2);
  key.set(key_bytes);

  EXPECT_EQ(Hex(Encrypt(key, kVectorPlaintext)), "f94840d86972f21c");
}

TEST(IceKey, DecryptInvertsEncryptForAllLevels) {
  std::mt19937 rng(12345);
  std::uniform_int_distribution<int> byte(0, 255);

  for (int level = 0; level <= 3; level++) {
    IceKey key(level);
    std::vector<uint8_t> key_bytes(key.keySize());

    for (int trial = 0; trial < 16; trial++) {
      for (auto &b : key_bytes)
        b = static_cast<uint8_t>(byte(rng));
      key.set(key_bytes.data());

      Block plain;
      for (auto &b : plain)
        b = static_cast<uint8_t>(byte(rng));

      const Block cipher = Encrypt(key, plain);
      EXPECT_NE(cipher, plain) << "level " << level << " trial " << trial;
      EXPECT_EQ(Decrypt(key, cipher), plain) << "level " << level << " trial " << trial;
    }
  }
}
