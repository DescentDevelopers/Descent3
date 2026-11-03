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

// The decoders work on 8x8 pixel blocks. The code map holds one 4-bit opcode per block, two blocks per byte (low
// nibble first), blocks in raster order. The decoders write into g_vBackBuf1 (the frame being built) and read the
// previous frame from g_vBackBuf2; like mveplay.cpp, both live in one allocation.

#ifndef LIBMVE_TESTS_DECODER_TEST_UTIL_H_
#define LIBMVE_TESTS_DECODER_TEST_UTIL_H_

#include <cstdint>
#include <cstdio>
#include <map>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "d3_test/byte_writer.h"
#include "decoders.h"
#include "mve_test_util.h"

namespace mvetest {

constexpr uint8_t kOpUnchanged = 0x1;

template <typename Pixel> class FrameFixture : public ::testing::Test {
protected:
  void InitFrame(int w, int h) {
    w_ = w;
    h_ = h;
    buf_.assign(2 * w * h, 0);
    g_width = w;
    g_height = h;
    g_vBackBuf1 = buf_.data();
    g_vBackBuf2 = buf_.data() + w * h;
    map_.assign((w / 8) * (h / 8) / 2, kOpUnchanged | (kOpUnchanged << 4));
  }

  void SetOp(int bx, int by, uint8_t op) {
    int index = by * (w_ / 8) + bx;
    uint8_t &m = map_[index / 2];
    if (index % 2 == 0)
      m = (m & 0xf0) | op;
    else
      m = (m & 0x0f) | (op << 4);
  }

  Pixel &Cur(int x, int y) { return buf_[y * w_ + x]; }
  Pixel &Prev(int x, int y) { return buf_[w_ * h_ + y * w_ + x]; }

  void FillFrames() {
    for (int y = 0; y < h_; y++) {
      for (int x = 0; x < w_; x++) {
        Cur(x, y) = static_cast<Pixel>(x * 3 + y * 37 + 1);
        Prev(x, y) = static_cast<Pixel>(x * 5 + y * 11 + 200);
      }
    }
  }

  std::vector<Pixel> BlockAt(const std::vector<Pixel> &frame, int x, int y) const {
    std::vector<Pixel> out;
    for (int j = 0; j < 8; j++)
      for (int i = 0; i < 8; i++)
        out.push_back(frame[(y + j) * w_ + (x + i)]);
    return out;
  }
  std::vector<Pixel> CurBlock(int bx, int by) const { return BlockAt(buf_, bx * 8, by * 8); }
  std::vector<Pixel> SnapshotCur() const { return std::vector<Pixel>(buf_.begin(), buf_.begin() + w_ * h_); }
  std::vector<Pixel> SnapshotPrev() const { return std::vector<Pixel>(buf_.begin() + w_ * h_, buf_.end()); }

  static std::string Hex(unsigned value) {
    char cell[8];
    snprintf(cell, sizeof(cell), sizeof(Pixel) == 1 ? "%02x" : "%04x", value);
    return cell;
  }

  std::string BlockText(int bx, int by) const {
    std::string text = "\n";
    for (int j = 0; j < 8; j++) {
      for (int i = 0; i < 8; i++) {
        if (i)
          text += ' ';
        text += Hex(buf_[(by * 8 + j) * w_ + bx * 8 + i]);
      }
      text += '\n';
    }
    return text;
  }

  static std::string Expand(const char *pattern, const std::map<char, unsigned> &legend) {
    std::string text;
    bool in_row = false;
    for (const char *c = pattern; *c; c++) {
      if (*c == '\n') {
        text += '\n';
        in_row = false;
      } else {
        if (in_row)
          text += ' ';
        text += Hex(legend.at(*c));
        in_row = true;
      }
    }
    return text;
  }

  void ExpectSolid(int bx, int by, Pixel value) {
    EXPECT_EQ(CurBlock(bx, by), std::vector<Pixel>(64, value)) << "block (" << bx << "," << by << ")";
  }

  int w_ = 0, h_ = 0;
  std::vector<Pixel> buf_;
  Bytes map_;
};

// A FrameFixture driving decodeFrame8, with a solid sentinel block after the block under test.
class Decoder8Fixture : public FrameFixture<uint8_t> {
protected:
  static constexpr uint8_t kSentinel = 0x77;

  // Decodes `data` as the frame's data stream.
  void Decode(const Bytes &data) {
    data_ = data;
    decodeFrame8(buf_.data(), map_.data(), static_cast<int>(map_.size()), data_.data(), static_cast<int>(data_.size()));
  }

  // Decodes a 16x8 frame of opcode `op` followed by a solid sentinel block.
  void DecodeSingleBlock(uint8_t op, Bytes data) {
    InitFrame(16, 8);
    SetOp(0, 0, op);
    SetOp(1, 0, 0xe);
    data.push_back(kSentinel);
    Decode(data);
  }

  // Expects the sentinel block of DecodeSingleBlock to be intact.
  void ExpectSentinel() { ExpectSolid(1, 0, kSentinel); }

  Bytes data_;
};

// A FrameFixture driving decodeFrame16, with a solid sentinel block after the block under test.
class Decoder16Fixture : public FrameFixture<uint16_t> {
protected:
  static constexpr uint16_t kSentinel = 0x0777;

  // Decodes `main` as the frame's main stream and `motion` as its motion stream.
  void Decode(const Bytes &main, const Bytes &motion = {}) {
    data_ = d3_test::ByteWriter().U16(static_cast<uint16_t>(2 + main.size())).Bytes(main).Bytes(motion).bytes();
    decodeFrame16(reinterpret_cast<unsigned char *>(buf_.data()), map_.data(), static_cast<int>(map_.size()),
                  data_.data(), static_cast<int>(data_.size()));
  }

  // Decodes a 16x8 frame of opcode `op` followed by a solid sentinel block.
  void DecodeSingleBlock(uint8_t op, const Bytes &main) {
    InitFrame(16, 8);
    SetOp(0, 0, op);
    SetOp(1, 0, 0xe);
    Decode(d3_test::ByteWriter().Bytes(main).U16(kSentinel));
  }

  // Expects the sentinel block of DecodeSingleBlock to be intact.
  void ExpectSentinel() { ExpectSolid(1, 0, kSentinel); }

  Bytes data_;
};

} // namespace mvetest

#endif // LIBMVE_TESTS_DECODER_TEST_UTIL_H_
