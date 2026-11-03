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

#pragma once

#include <array>
#include <cstdint>
#include <initializer_list>
#include <string>

#include "md5.h"

// Hex digest of `n` rounds of typed updates (float, int, int16_t, uint32_t, uint8_t).
std::string hexdigest_after_n_rounds_of_fuzz(int n);

// Lowercase hex form of `digest`.
std::string to_hex(const std::array<uint8_t, 16> &digest);

// Hex digest of the bytes of `s`.
std::string md5_hex(const std::string &s);

// Hex digest of a single typed update with `value`.
template <typename T> std::string digest_of_typed(T value) {
  MD5 md5;
  md5.update(value);
  return to_hex(md5.digest());
}

// Hex digest of `bytes`.
std::string digest_of_bytes(std::initializer_list<uint8_t> bytes);
