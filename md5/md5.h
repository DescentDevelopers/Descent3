/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
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

#ifndef MD5_H
#define MD5_H

#include <array>
#include <cstdint>

class MD5 {
private:
  std::array<std::uint32_t, 4> sums_{0x67452301UL, 0XEFCDAB89UL, 0x98BADCFEUL, 0x10325476UL};
  std::array<unsigned char, 64> tmpbuf_;
  std::size_t tmpbuf_n_{0};
  std::uint64_t message_len_{0};

  void round(const unsigned char *block) noexcept;
  void round(std::array<std::uint32_t, 4> &sums, const unsigned char *block) const noexcept;
  void place_length(unsigned char *destination) const noexcept;

public:
  void update(const unsigned char *data, std::size_t n) noexcept;
  void update(float val) noexcept;
  void update(int val) noexcept;
  void update(uint32_t val) noexcept;
  void update(unsigned char val) noexcept;
  void update(short val) noexcept;
  std::array<unsigned char, 16> digest() const noexcept;
  void digest(unsigned char *destination) const noexcept;
};

#endif // MD5_H
