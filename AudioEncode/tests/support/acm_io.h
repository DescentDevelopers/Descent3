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

#ifndef AUDIOENCODE_TESTS_ACM_IO_H
#define AUDIOENCODE_TESTS_ACM_IO_H

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <vector>

struct PcmSource {
  const std::vector<int16_t> *samples;
  size_t pos;
};

int32_t ReadPcm(void *data);

struct ByteSource {
  const std::vector<uint8_t> *bytes;
  size_t pos = 0;
  size_t max_chunk = std::numeric_limits<size_t>::max();
};

int32_t ReadBytes(void *data, void *buffer, uint32_t amount);

struct Decoded {
  bool ok = false;
  uint32_t channels = 0;
  uint32_t rate = 0;
  uint32_t sample_count = 0;
  std::vector<int16_t> pcm;
};

Decoded Decode(const std::vector<uint8_t> &bytes, size_t source_chunk = std::numeric_limits<size_t>::max(),
               uint32_t read_chunk = 1 << 16);

// Header fields that CreateDecoder does not expose, read with libacm directly.
struct AcmLayout {
  bool ok = false;
  unsigned level = 0;
  unsigned rows = 0;
};

int AcmRead(void *ptr, int size, int n, void *datasrc);

AcmLayout ReadLayout(const std::vector<uint8_t> &bytes);

// Encodes pcm with AudioEncode into file, expecting success and all input consumed, and returns the file's bytes.
std::vector<uint8_t> Encode(const std::filesystem::path &file, const std::vector<int16_t> &pcm, unsigned channels,
                            unsigned rate, int levels, int rows, float comp_ratio, float volume = 1.0f);

// Writes samples as raw 16-bit little-endian PCM.
void WriteRawPcm(const std::filesystem::path &p, const std::vector<int16_t> &pcm);

#endif
