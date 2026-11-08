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

#include <cstdio>
#include <cstring>

#include <gtest/gtest.h>

#include "dmfc_fakes.h"

namespace dmfc_test {

void TestOpenCFILE(CFILE **handle, const std::filesystem::path &filename, const char *mode) {
  *handle = cfopen(filename, mode);
}

uint32_t TestHostAddressFromNumbers(char *str) {
  unsigned a = 0, b = 0, c = 0, d = 0;
  EXPECT_EQ(sscanf(str, "%u.%u.%u.%u", &a, &b, &c, &d), 4) << str;
  uint8_t bytes[4] = {static_cast<uint8_t>(a), static_cast<uint8_t>(b), static_cast<uint8_t>(c),
                      static_cast<uint8_t>(d)};
  uint32_t result;
  memcpy(&result, bytes, 4);
  return result;
}

std::vector<std::string> g_kick_calls;
std::vector<std::string> g_other_calls;

void KickHandler(const char *s) { g_kick_calls.emplace_back(s); }
void OtherHandler(const char *s) { g_other_calls.emplace_back(s); }

int PackDummy(void *, uint8_t *) { return 0; }

int FixedWidthTextLineWidth(const char *str) {
  const int n = static_cast<int>(strlen(str));
  return n ? 5 * n - 1 : 0;
}

} // namespace dmfc_test
