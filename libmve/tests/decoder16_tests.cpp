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

// Unlike the 8-bit decoder, pixels are little-endian 16-bit values; the data starts with a LE16 offset to a second
// stream that holds the motion bytes of opcodes 0x2-0x4; and pattern modes are selected by the high bit of P0 (and P2)
// rather than by comparing P0 with P1. As in the 8-bit tests, a solid sentinel block after the block under test checks
// how much of the main stream was consumed.

#include <map>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "d3_test/byte_writer.h"
#include "support/decoder_test_util.h"

using namespace mvetest;

namespace {

using Decoder16Test = Decoder16Fixture;

TEST_F(Decoder16Test, PixelsAreLittleEndian) {
  InitFrame(16, 8);
  SetOp(0, 0, 0xe); // solid
  SetOp(1, 0, 0xb); // raw
  d3_test::ByteWriter main;
  main.Bytes({0x34, 0x12});
  for (int i = 0; i < 64; i++)
    main.U16(static_cast<uint16_t>(0x0100 + i * 3));
  Decode(main);

  ExpectSolid(0, 0, 0x1234);
  for (int i = 0; i < 64; i++)
    EXPECT_EQ(Cur(8 + i % 8, i / 8), 0x0100 + i * 3) << "raw pixel " << i;
}

TEST_F(Decoder16Test, Op07HighBitClearMeansOneBitPerPixel) {
  d3_test::ByteWriter main;
  main.U16(0x0123).U16(0x7abc).Bytes({0x01, 0x80, 0xff, 0x00, 0x00, 0x00, 0x00, 0x0f});
  DecodeSingleBlock(0x7, main);

  const char *expected = R"(
BAAAAAAA
AAAAAAAB
BBBBBBBB
AAAAAAAA
AAAAAAAA
AAAAAAAA
AAAAAAAA
BBBBAAAA
)";
  EXPECT_EQ(BlockText(0, 0), Expand(expected, {{'A', 0x0123}, {'B', 0x7abc}}));
  ExpectSentinel();
}

TEST_F(Decoder16Test, Op07HighBitSetMeansOneBitPer2x2Square) {
  d3_test::ByteWriter main;
  main.U16(0x8123).U16(0x0abc).Bytes({0x81, 0x6f});
  DecodeSingleBlock(0x7, main);

  const char *expected = R"(
BBAAAAAA
BBAAAAAA
AAAAAABB
AAAAAABB
BBBBBBBB
BBBBBBBB
AABBBBAA
AABBBBAA
)";
  EXPECT_EQ(BlockText(0, 0), Expand(expected, {{'A', 0x8123}, {'B', 0x0abc}}));
  ExpectSentinel();
}

TEST_F(Decoder16Test, Op09HighBitOfP2SelectsTwoBitsPer2x2Square) {
  // P0 high bit clear, P2 high bit set: 4 pattern bytes, one per pair of rows (0xe4 -> P0 P1 P2 P3).
  d3_test::ByteWriter main;
  main.U16(0x0010).U16(0x0020).U16(0x8040).U16(0x0030).Bytes({0xe4, 0x00, 0xff, 0x1b});
  DecodeSingleBlock(0x9, main);

  const char *expected = R"(
AABBCCDD
AABBCCDD
AAAAAAAA
AAAAAAAA
DDDDDDDD
DDDDDDDD
DDCCBBAA
DDCCBBAA
)";
  EXPECT_EQ(BlockText(0, 0), Expand(expected, {{'A', 0x0010}, {'B', 0x0020}, {'C', 0x8040}, {'D', 0x0030}}));
  ExpectSentinel();
}

TEST_F(Decoder16Test, Op0dEachPixelFillsA4x4Quadrant) {
  d3_test::ByteWriter main;
  main.U16(0x0a01).U16(0x0b02).U16(0x0c03).U16(0x0d04);
  DecodeSingleBlock(0xd, main);

  const char *expected = R"(
AAAABBBB
AAAABBBB
AAAABBBB
AAAABBBB
CCCCDDDD
CCCCDDDD
CCCCDDDD
CCCCDDDD
)";
  EXPECT_EQ(BlockText(0, 0), Expand(expected, {{'A', 0x0a01}, {'B', 0x0b02}, {'C', 0x0c03}, {'D', 0x0d04}}));
  ExpectSentinel();
}

TEST_F(Decoder16Test, MotionOpcodesTakeTheirOffsetFromTheMotionStream) {
  // Opcode 0x5 reads its two bytes from the main stream. The offsets themselves mean the same as in the 8-bit decoder.
  InitFrame(32, 24);
  FillFrames();
  auto cur = SnapshotCur();
  auto prev = SnapshotPrev();
  SetOp(0, 0, 0x2); // motion B = 9    -> (+10, +1) in the current frame
  SetOp(1, 1, 0x4); // motion B = 0x9a -> (+2, +1) in the previous frame
  SetOp(2, 1, 0x5); // main (-3, +5) in the previous frame
  SetOp(3, 1, 0xe); // main sentinel
  SetOp(0, 2, 0x0); // same block of the previous frame
  d3_test::ByteWriter main;
  main.Bytes({0xfd, 0x05}).U16(kSentinel);
  Decode(main, {9, 0x9a});

  EXPECT_EQ(CurBlock(0, 0), BlockAt(cur, 0 + 10, 0 + 1));
  EXPECT_EQ(CurBlock(1, 1), BlockAt(prev, 8 + 2, 8 + 1));
  EXPECT_EQ(CurBlock(2, 1), BlockAt(prev, 16 - 3, 8 + 5));
  ExpectSolid(3, 1, kSentinel);
  EXPECT_EQ(CurBlock(0, 2), BlockAt(prev, 0, 16));
}

TEST_F(Decoder16Test, Op03TakesItsOffsetFromTheMotionStream) {
  InitFrame(32, 24);
  FillFrames();
  auto cur = SnapshotCur();
  SetOp(0, 2, 0x3); // motion B = 88 -> (+11, -9)
  SetOp(1, 2, 0xe);
  d3_test::ByteWriter main;
  main.U16(kSentinel);
  Decode(main, {88});

  EXPECT_EQ(CurBlock(0, 2), BlockAt(cur, 0 + 11, 16 - 9));
  ExpectSolid(1, 2, kSentinel);
}

} // namespace
