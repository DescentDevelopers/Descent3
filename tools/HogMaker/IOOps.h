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

#include "portable_endian.h"

namespace D3
{
  static inline std::ostream &bin_write(std::ostream &output, int8_t value)
  {
    output.write(reinterpret_cast<const char *>(&value), 1);
    return output;
  }

  static inline std::ostream &bin_write(std::ostream &output, uint8_t value)
  {
    output.write(reinterpret_cast<const char *>(&value), 1);
    return output;
  }

  static inline std::ostream &bin_write(std::ostream &output, int16_t value, bool is_little_endian = true)
  {
    value = is_little_endian ? htole16(value) : htobe16(value);
    output.write(reinterpret_cast<const char *>(&value), sizeof(int16_t));
    return output;
  }

  static inline std::ostream &bin_write(std::ostream &output, uint16_t value, bool is_little_endian = true)
  {
    value = is_little_endian ? htole16(value) : htobe16(value);
    output.write(reinterpret_cast<const char *>(&value), sizeof(uint16_t));
    return output;
  }

  static inline std::ostream &bin_write(std::ostream &output, int32_t value, bool is_little_endian = true)
  {
    value = is_little_endian ? htole32(value) : htobe32(value);
    output.write(reinterpret_cast<const char *>(&value), sizeof(int32_t));
    return output;
  }

  static inline std::ostream &bin_write(std::ostream &output, uint32_t value, bool is_little_endian = true)
  {
    value = is_little_endian ? htole32(value) : htobe32(value);
    output.write(reinterpret_cast<const char *>(&value), sizeof(uint32_t));
    return output;
  }

  static inline std::ostream &bin_write(std::ostream &output, int64_t value, bool is_little_endian = true)
  {
    value = is_little_endian ? htole64(value) : htobe64(value);
    output.write(reinterpret_cast<const char *>(&value), sizeof(int64_t));
    return output;
  }

  static inline std::ostream &bin_write(std::ostream &output, uint64_t value, bool is_little_endian = true)
  {
    value = is_little_endian ? htole64(value) : htobe64(value);
    output.write(reinterpret_cast<const char *>(&value), sizeof(uint64_t));
    return output;
  }

} // namespace D3
