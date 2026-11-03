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

#include "digest.h"

#include <iomanip>
#include <sstream>
#include <vector>

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

std::string to_hex(const std::array<uint8_t, 16> &digest) {
  std::ostringstream hex;
  hex << std::hex << std::nouppercase;
  for (uint8_t b : digest)
    hex << std::setw(2) << std::setfill('0') << static_cast<uint32_t>(b);
  return hex.str();
}

std::string md5_hex(const std::string &s) {
  MD5 md5;
  md5.update(reinterpret_cast<const uint8_t *>(s.data()), s.size());
  return to_hex(md5.digest());
}

std::string digest_of_bytes(std::initializer_list<uint8_t> bytes) {
  std::vector<uint8_t> v(bytes);
  MD5 md5;
  md5.update(v.data(), v.size());
  return to_hex(md5.digest());
}
