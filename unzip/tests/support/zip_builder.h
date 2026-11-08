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
#include <string>
#include <vector>

using Bytes = std::vector<uint8_t>;

constexpr uint16_t kMethodStored = 0;
constexpr uint16_t kMethodDeflate = 8;

Bytes RandomBytes(size_t n, uint32_t seed);
Bytes TextBytes(size_t n);
// n bytes of RandomBytes (seeded with n + 1) if `random`, else of TextBytes.
Bytes SampleBytes(size_t n, bool random);
// The characters of `s` as bytes.
Bytes ToBytes(const std::string &s);
// The chars of `v` as bytes.
Bytes AsBytes(const std::vector<char> &v);
Bytes RawDeflate(const Bytes &in);
uint32_t Crc(const Bytes &data);

struct Entry {
  std::string name;
  Bytes data;
  uint16_t method = kMethodStored;
  Bytes payload;
  bool payload_set = false;
  uint16_t version_needed = 20;
  Bytes local_extra;
  Bytes central_extra;
  std::string comment;
  bool data_descriptor = false; // general purpose bit 3: sizes/CRC are zero in the local header
  bool override_sizes = false;
  uint32_t central_compressed_size = 0;
  uint32_t central_uncompressed_size = 0;
  bool override_offset = false;
  uint32_t central_local_offset = 0;
};

struct Archive {
  Bytes bytes;
  uint32_t cd_offset = 0;
};

Archive BuildZip(std::vector<Entry> entries, const std::string &archive_comment = "");
Entry Stored(const std::string &name, Bytes data);
Entry Deflated(const std::string &name, Bytes data);
