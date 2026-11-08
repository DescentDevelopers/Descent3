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

// An audio frame segment (opcode 0x08) is laid out as: LE16 sequence index, LE16 stream mask, LE16 stream length
// (bytes of decoded output), then the stream data (see audio_data_handler in mveplay.cpp). Compressed 16-bit stereo
// data is Interplay DPCM: two LE16 initial predictors (left, right), then one byte per output sample, alternating
// channels, each byte indexing a table of deltas that are added to that channel's predictor with saturation.

#include <cstdint>
#include <cstring>
#include <vector>

#include <gtest/gtest.h>

#include "d3_test/byte_writer.h"
#include "mve_audio.h"
#include "support/mve_audio_test_util.h"

using namespace mvetest;

namespace {

TEST(MveAudioTest, CompressedWritesExactlyTheStreamLength) {
  Bytes frame = CompressedFrame(0, 0, {Plus(1), Plus(1), Plus(1), Plus(1)});
  std::vector<uint8_t> out(16, 0xCD);
  mveaudio_process(reinterpret_cast<char *>(out.data()), frame.data(), 2, true);

  // 4 samples * 2 bytes were written.
  for (size_t i = 8; i < out.size(); i++)
    EXPECT_EQ(out[i], 0xCD) << "byte " << i;
}

TEST(MveAudioTest, CompressedChannelsAccumulateIndependently) {
  // Even samples belong to the left channel, odd to the right. The negative predictor must be sign-extended.
  Bytes codes = {Plus(1), Plus(5), Plus(2), Minus(3), Plus(0), Minus(10)};
  std::vector<int16_t> out = DecodeCompressed16(CompressedFrame(1000, -1000, codes), codes.size());

  std::vector<int16_t> expected = {1001, -995, 1003, -998, 1003, -1008};
  EXPECT_EQ(out, expected);
}

TEST(MveAudioTest, CompressedOutputSaturatesInsteadOfWrapping) {
  Bytes codes = {Plus(10), Minus(10), Plus(0), Plus(0), Minus(1), Plus(1)};
  std::vector<int16_t> out = DecodeCompressed16(CompressedFrame(32760, -32760, codes), codes.size());

  // Left: 32760 + 10 clamps to 32767, and the clamped value is the new predictor (so -1 then gives 32766).
  // Right: -32760 - 10 clamps to -32768, then +1 gives -32767.
  std::vector<int16_t> expected = {32767, -32768, 32767, -32768, 32766, -32767};
  EXPECT_EQ(out, expected);
}

TEST(MveAudioTest, CompressedDeltaTableIsSignSymmetric) {
  // The DPCM table is symmetric: code 256-k is the negation of code k for k = 1..127. So on one channel, code k
  // followed by code 256-k returns to the starting predictor.
  for (int k = 1; k <= 127; k++) {
    SCOPED_TRACE(k);
    Bytes codes = {Plus(k), Plus(0), Minus(k), Plus(0)};
    std::vector<int16_t> out = DecodeCompressed16(CompressedFrame(0, 0, codes), codes.size());
    EXPECT_NE(out[0], 0);
    EXPECT_EQ(out[2], 0);
  }
}

} // namespace
