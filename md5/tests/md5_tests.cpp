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

#include <gtest/gtest.h>

#include <sstream>
#include <iomanip>

#include "md5.h"

namespace {

std::string hexdigest_after_n_rounds_of_fuzz(int n) {
  MD5 md5;
  for (int i = 0; i < n; ++i) {
    md5.update((float)1.0f);
    md5.update((int)2);
    md5.update((int16_t)3);
    md5.update((uint32_t)4U);
    md5.update((uint8_t)5);
  }

  std::ostringstream hexdigest;
  hexdigest << std::hex;
  auto digest = md5.digest();

  for (int i = 0; i < 16; ++i)
    hexdigest << std::setw(2) << std::setfill('0') << std::nouppercase << static_cast<uint32_t>(digest[i]);

  return hexdigest.str();
}

}; // namespace

TEST(D3, MD5) {
  EXPECT_EQ(hexdigest_after_n_rounds_of_fuzz(0), "d41d8cd98f00b204e9800998ecf8427e");
  EXPECT_EQ(hexdigest_after_n_rounds_of_fuzz(1), "c56c005504f5b9b17df9d83f1106e9b2");
  EXPECT_EQ(hexdigest_after_n_rounds_of_fuzz(2), "98327a3f4ee311c9bdc8613508922c95");
  EXPECT_EQ(hexdigest_after_n_rounds_of_fuzz(4), "f2343ce526646c087df92d708e883675");
  EXPECT_EQ(hexdigest_after_n_rounds_of_fuzz(8), "73396b338a0a769c4e6ae5b7b0bc187e");
  EXPECT_EQ(hexdigest_after_n_rounds_of_fuzz(100), "99b30c1dcc42d97c3d914c26a14bb1d7");
  EXPECT_EQ(hexdigest_after_n_rounds_of_fuzz(1999), "82eeff8c7d574c8232b0ca6ca2c9dd40");
}
