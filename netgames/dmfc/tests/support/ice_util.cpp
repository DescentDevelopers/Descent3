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

#include "ice_util.h"

#include <cstdio>

namespace dmfc_test {

Block Encrypt(const IceKey &key, const Block &in) {
  Block out{};
  key.encrypt(in.data(), out.data());
  return out;
}

Block Decrypt(const IceKey &key, const Block &in) {
  Block out{};
  key.decrypt(in.data(), out.data());
  return out;
}

std::string Hex(const Block &block) {
  std::string result;
  char digits[3];
  for (uint8_t b : block) {
    snprintf(digits, sizeof(digits), "%02x", b);
    result += digits;
  }
  return result;
}

} // namespace dmfc_test
