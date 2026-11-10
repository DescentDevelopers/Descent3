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
#include <cstring>
#include <string_view>
#include <vector>

namespace d3_test {

// Builds a byte image: little-endian by default, the layout of the game's own file formats and network packets, with
// big-endian variants for formats that need them.
class ByteWriter {
public:
  ByteWriter &U8(uint8_t v) {
    bytes_.push_back(v);
    return *this;
  }
  ByteWriter &U16(uint16_t v) { return Int(v, 2); }
  ByteWriter &U32(uint32_t v) { return Int(v, 4); }
  ByteWriter &I16(int16_t v) { return Int(static_cast<uint16_t>(v), 2); }
  ByteWriter &I32(int32_t v) { return Int(static_cast<uint32_t>(v), 4); }
  ByteWriter &U16BE(uint16_t v) { return U8(static_cast<uint8_t>(v >> 8)).U8(static_cast<uint8_t>(v)); }
  ByteWriter &U32BE(uint32_t v) { return U16BE(static_cast<uint16_t>(v >> 16)).U16BE(static_cast<uint16_t>(v)); }
  ByteWriter &F32(float v) {
    uint32_t bits;
    std::memcpy(&bits, &v, sizeof(bits));
    return U32(bits);
  }
  ByteWriter &Bytes(const void *data, size_t size) {
    const auto *p = static_cast<const uint8_t *>(data);
    bytes_.insert(bytes_.end(), p, p + size);
    return *this;
  }
  ByteWriter &Bytes(const std::vector<uint8_t> &data) { return Bytes(data.data(), data.size()); }
  ByteWriter &Fill(size_t count, uint8_t value) {
    bytes_.insert(bytes_.end(), count, value);
    return *this;
  }
  ByteWriter &Zeros(size_t count) { return Fill(count, 0); }
  // The characters of `text`, without a terminator.
  ByteWriter &Text(std::string_view text) { return Bytes(text.data(), text.size()); }
  // `text` in a field of `size` bytes, padded with NULs (and cut short if it does not fit).
  ByteWriter &FixedText(std::string_view text, size_t size) {
    const size_t n = text.size() < size ? text.size() : size;
    Bytes(text.data(), n);
    return Zeros(size - n);
  }

  // Overwrites bytes already written, at `offset`.
  void PatchU16(size_t offset, uint16_t v) { Patch(offset, v, 2); }
  void PatchU32(size_t offset, uint32_t v) { Patch(offset, v, 4); }

  size_t size() const { return bytes_.size(); }
  const std::vector<uint8_t> &bytes() const { return bytes_; }
  operator const std::vector<uint8_t> &() const { return bytes_; }

private:
  ByteWriter &Int(uint32_t v, int size) {
    for (int i = 0; i < size; i++)
      bytes_.push_back(static_cast<uint8_t>(v >> (8 * i)));
    return *this;
  }
  void Patch(size_t offset, uint32_t v, int size) {
    for (int i = 0; i < size; i++)
      bytes_.at(offset + i) = static_cast<uint8_t>(v >> (8 * i));
  }

  std::vector<uint8_t> bytes_;
};

} // namespace d3_test
