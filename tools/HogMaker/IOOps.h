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
#pragma once

#include <cstdint>
#include <ostream>

#include "byteswap.h"

namespace D3 {

// std::byteswap from C++23
template <typename T> constexpr T byteswap(T n) {
  T m;
  for (size_t i = 0; i < sizeof(T); i++)
    reinterpret_cast<uint8_t *>(&m)[i] = reinterpret_cast<uint8_t *>(&n)[sizeof(T) - 1 - i];
  return m;
}

/**
 * Convert integer to/from BE order
 */
template <typename T> constexpr T convert_be(T val) {
#ifndef OUTRAGE_BIG_ENDIAN
  return byteswap(val);
#else
  return (val);
#endif
}

/**
 * Convert integer to/from LE order
 */
template <typename T> constexpr T convert_le(T val) {
#ifndef OUTRAGE_BIG_ENDIAN
  return (val);
#else
  return byteswap(val);
#endif
}

template <class T>
inline std::ostream &bin_write(std::ostream &output, T &value, bool is_little_endian = true, size_t n = sizeof(T)) {
  value = is_little_endian ? convert_le(value) : convert_be(value);
  output.write(reinterpret_cast<char *>(&value), n);

  return output;
}

} // namespace D3
