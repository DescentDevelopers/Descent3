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

#ifndef LIBMVE_TESTS_MVE_AUDIO_TEST_UTIL_H_
#define LIBMVE_TESTS_MVE_AUDIO_TEST_UTIL_H_

#include <cstddef>
#include <cstdint>
#include <vector>

#include "d3_test/byte_writer.h"
#include "mve_audio.h"
#include "mve_test_util.h"

namespace mvetest {

inline Bytes CompressedFrame(int16_t left, int16_t right, const Bytes &codes) {
  return d3_test::ByteWriter()
      .U16(1)                                       // sequence index
      .U16(1)                                       // stream mask
      .U16(static_cast<uint16_t>(codes.size() * 2)) // decoded length in bytes (16-bit samples)
      .I16(left)
      .I16(right)
      .Bytes(codes)
      .bytes();
}

// Decodes a compressed 16-bit stereo frame into `samples` samples.
inline std::vector<int16_t> DecodeCompressed16(Bytes frame, size_t samples) {
  std::vector<int16_t> out(samples);
  mveaudio_process(reinterpret_cast<char *>(out.data()), frame.data(), 2, true);
  return out;
}

// Delta codes from the Interplay DPCM table: codes 0..43 add their own value, and code 256-k adds -k for those k.
constexpr uint8_t Plus(int k) { return static_cast<uint8_t>(k); }
constexpr uint8_t Minus(int k) { return static_cast<uint8_t>(256 - k); }

} // namespace mvetest

#endif // LIBMVE_TESTS_MVE_AUDIO_TEST_UTIL_H_
