/*
 * Descent 3
 * Copyright (C) 2024 Descent Developers
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

#include <vector>

#include <gtest/gtest.h>
#include "gamespyutils.h"

#define VALIDATE_SIZE 6

TEST(D3, GameSpyUtilsTests) {
  const char *key = "KEY";
  const char *result = "R1/G/K+T";
  uint8_t encrypted_val[VALIDATE_SIZE];
  uint8_t encoded_val[(VALIDATE_SIZE * 4) / 3 + 1];

  uint8_t input[VALIDATE_SIZE] = "TEST";

  memset(encoded_val, 0, (VALIDATE_SIZE * 4) / 3 + 1);
  memset(encrypted_val, 0 , VALIDATE_SIZE);

  gs_encrypt((const uint8_t*)key, input, VALIDATE_SIZE);
  gs_encode(input, VALIDATE_SIZE, encoded_val);

  EXPECT_STREQ((char *)encoded_val, result);
}

// Expected strings are the RFC 4648 section 10 test vectors plus a group that uses both non-alphanumeric characters
// ('+' = 62, '/' = 63).
TEST(D3, GameSpyEncodeMatchesBase64ForWholeGroups) {
  struct Case {
    std::vector<uint8_t> in;
    const char *out;
  } cases[] = {
      {{}, ""},
      {{'f', 'o', 'o'}, "Zm9v"},
      {{'f', 'o', 'o', 'b', 'a', 'r'}, "Zm9vYmFy"},
      {{0xFB, 0xFF, 0xBF}, "+/+/"},
      {{0x00, 0x00, 0x00}, "AAAA"},
  };

  for (auto &c : cases) {
    std::vector<uint8_t> out(c.in.size() * 4 / 3 + 1, 0xAA);
    gs_encode(c.in.data(), static_cast<int>(c.in.size()), out.data());
    EXPECT_STREQ(reinterpret_cast<char *>(out.data()), c.out);
  }
}
