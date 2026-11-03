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

#include <cstddef>
#include <cstdint>
#include <vector>

inline std::vector<uint8_t> Repeat(const std::vector<uint8_t> &v, int times) {
  std::vector<uint8_t> out;
  for (int i = 0; i < times; i++)
    out.insert(out.end(), v.begin(), v.end());
  return out;
}

inline std::vector<uint8_t> MakePattern(size_t size, uint8_t seed) {
  std::vector<uint8_t> data(size);
  uint32_t x = seed * 2654435761u + 1;
  for (auto &b : data) {
    x = x * 1664525u + 1013904223u;
    b = static_cast<uint8_t>(x >> 24);
  }
  return data;
}
