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

// Pattern bits are consumed least-significant bit first, left to right then top to bottom. Where several opcodes are
// checked, the block after the one under test is a solid block (opcode 0xe) with a sentinel color: it only comes out
// right if the block under test consumed exactly its share of the data stream.

#include <vector>

#include <gtest/gtest.h>

#include "support/decoder_test_util.h"

using namespace mvetest;

namespace {

using Decoder8Test = Decoder8Fixture;

TEST_F(Decoder8Test, LowNibbleOfMapByteIsTheLeftBlock) {
  InitFrame(16, 8);
  SetOp(0, 0, 0xe); // solid
  SetOp(1, 0, 0xb); // raw
  Bytes data = {0x11};
  for (int i = 0; i < 64; i++)
    data.push_back(static_cast<uint8_t>(0x80 + i));
  Decode(data);

  ExpectSolid(0, 0, 0x11);
  for (int i = 0; i < 64; i++)
    EXPECT_EQ(Cur(8 + i % 8, i / 8), 0x80 + i) << "raw pixel " << i;
}

TEST_F(Decoder8Test, BlocksAreDecodedInRasterOrder) {
  InitFrame(32, 16);
  for (int by = 0; by < 2; by++)
    for (int bx = 0; bx < 4; bx++)
      SetOp(bx, by, 0xe);
  Decode({1, 2, 3, 4, 5, 6, 7, 8});

  for (int by = 0; by < 2; by++)
    for (int bx = 0; bx < 4; bx++)
      ExpectSolid(bx, by, static_cast<uint8_t>(1 + by * 4 + bx));
}

TEST_F(Decoder8Test, Op0bRawPixelsAreRowMajor) {
  Bytes data;
  for (int i = 0; i < 64; i++)
    data.push_back(static_cast<uint8_t>(i));
  DecodeSingleBlock(0xb, data);

  for (int y = 0; y < 8; y++)
    for (int x = 0; x < 8; x++)
      EXPECT_EQ(Cur(x, y), y * 8 + x) << "(" << x << "," << y << ")";
  ExpectSentinel();
}

TEST_F(Decoder8Test, Op0cEachByteFillsA2x2Block) {
  Bytes data;
  for (int i = 0; i < 16; i++)
    data.push_back(static_cast<uint8_t>(0x10 + i));
  DecodeSingleBlock(0xc, data);

  const char *expected = R"(
10 10 11 11 12 12 13 13
10 10 11 11 12 12 13 13
14 14 15 15 16 16 17 17
14 14 15 15 16 16 17 17
18 18 19 19 1a 1a 1b 1b
18 18 19 19 1a 1a 1b 1b
1c 1c 1d 1d 1e 1e 1f 1f
1c 1c 1d 1d 1e 1e 1f 1f
)";
  EXPECT_EQ(BlockText(0, 0), expected);
  ExpectSentinel();
}

TEST_F(Decoder8Test, Op0dEachByteFillsA4x4Quadrant) {
  DecodeSingleBlock(0xd, {0xa1, 0xb2, 0xc3, 0xd4});

  const char *expected = R"(
a1 a1 a1 a1 b2 b2 b2 b2
a1 a1 a1 a1 b2 b2 b2 b2
a1 a1 a1 a1 b2 b2 b2 b2
a1 a1 a1 a1 b2 b2 b2 b2
c3 c3 c3 c3 d4 d4 d4 d4
c3 c3 c3 c3 d4 d4 d4 d4
c3 c3 c3 c3 d4 d4 d4 d4
c3 c3 c3 c3 d4 d4 d4 d4
)";
  EXPECT_EQ(BlockText(0, 0), expected);
  ExpectSentinel();
}

TEST_F(Decoder8Test, Op0fIsACheckerboardStartingWithTheFirstColor) {
  DecodeSingleBlock(0xf, {0x01, 0x02});

  const char *expected = R"(
01 02 01 02 01 02 01 02
02 01 02 01 02 01 02 01
01 02 01 02 01 02 01 02
02 01 02 01 02 01 02 01
01 02 01 02 01 02 01 02
02 01 02 01 02 01 02 01
01 02 01 02 01 02 01 02
02 01 02 01 02 01 02 01
)";
  EXPECT_EQ(BlockText(0, 0), expected);
  ExpectSentinel();
}

TEST_F(Decoder8Test, Op07OneBitPerPixelMatchesTheDocumentedExample) {
  DecodeSingleBlock(0x7, {0x11, 0x22, 0xfe, 0x83, 0x83, 0x83, 0x83, 0x83, 0x83, 0xfe});

  const char *expected = R"(
11 22 22 22 22 22 22 22
22 22 11 11 11 11 11 22
22 22 11 11 11 11 11 22
22 22 11 11 11 11 11 22
22 22 11 11 11 11 11 22
22 22 11 11 11 11 11 22
22 22 11 11 11 11 11 22
11 22 22 22 22 22 22 22
)";
  EXPECT_EQ(BlockText(0, 0), expected);
  ExpectSentinel();
}

TEST_F(Decoder8Test, Op07WhenP0GreaterThanP1EachBitIsA2x2Square) {
  // P0 > P1 selects 2x2 mode: two bytes, the low nibble of each covering the upper pair of rows. The four row pairs
  // come from nibbles 0x1, 0x8, 0xf and 0x6.
  DecodeSingleBlock(0x7, {0x22, 0x11, 0x81, 0x6f});

  const char *expected = R"(
11 11 22 22 22 22 22 22
11 11 22 22 22 22 22 22
22 22 22 22 22 22 11 11
22 22 22 22 22 22 11 11
11 11 11 11 11 11 11 11
11 11 11 11 11 11 11 11
22 22 11 11 11 11 22 22
22 22 11 11 11 11 22 22
)";
  EXPECT_EQ(BlockText(0, 0), expected);
  ExpectSentinel();
}

TEST_F(Decoder8Test, Op08FourQuadrantsWithTheirOwnColors) {
  // P0 <= P1: four (P0 P1 B0 B1) groups for the top-left, bottom-left, top-right and bottom-right quadrants.
  DecodeSingleBlock(0x8, {
                             0x01, 0x02, 0x01, 0x80, // TL: first and last pixel set
                             0x03, 0x04, 0xff, 0x00, // BL: top two rows set
                             0x05, 0x06, 0x0f, 0xf0, // TR: rows 0 and 3 set
                             0x07, 0x08, 0x55, 0x55, // BR: every other pixel set
                         });

  const char *expected = R"(
02 01 01 01 06 06 06 06
01 01 01 01 05 05 05 05
01 01 01 01 05 05 05 05
01 01 01 02 06 06 06 06
04 04 04 04 08 07 08 07
04 04 04 04 08 07 08 07
03 03 03 03 08 07 08 07
03 03 03 03 08 07 08 07
)";
  EXPECT_EQ(BlockText(0, 0), expected);
  ExpectSentinel();
}

TEST_F(Decoder8Test, Op08LeftAndRightHalves) {
  // P0 > P1 and P2 <= P3: (P0 P1 B0-B3) is the left half, (P2 P3 B4-B7) the right half.
  DecodeSingleBlock(0x8, {
                             0x22, 0x11, 0x01, 0x00, 0x00, 0x80, // left: top-left and bottom-right pixels set
                             0x33, 0x44, 0xff, 0xff, 0x00, 0x00, // right: top four rows set
                         });

  const char *expected = R"(
11 22 22 22 44 44 44 44
22 22 22 22 44 44 44 44
22 22 22 22 44 44 44 44
22 22 22 22 44 44 44 44
22 22 22 22 33 33 33 33
22 22 22 22 33 33 33 33
22 22 22 22 33 33 33 33
22 22 22 11 33 33 33 33
)";
  EXPECT_EQ(BlockText(0, 0), expected);
  ExpectSentinel();
}

TEST_F(Decoder8Test, Op08TopAndBottomHalves) {
  // P0 > P1 and P2 > P3: (P0 P1 B0-B3) is the top half, (P2 P3 B4-B7) the bottom half, one byte per row.
  DecodeSingleBlock(0x8, {
                             0x22, 0x11, 0x01, 0x80, 0xff, 0x00, //
                             0x44, 0x33, 0x0f, 0xf0, 0xaa, 0x00, //
                         });

  const char *expected = R"(
11 22 22 22 22 22 22 22
22 22 22 22 22 22 22 11
11 11 11 11 11 11 11 11
22 22 22 22 22 22 22 22
33 33 33 33 44 44 44 44
44 44 44 44 33 33 33 33
44 33 44 33 44 33 44 33
44 44 44 44 44 44 44 44
)";
  EXPECT_EQ(BlockText(0, 0), expected);
  ExpectSentinel();
}

// 2 bits per pixel: 00=P0, 01=P1, 10=P2, 11=P3.
// 0xe4 = 11 10 01 00 -> P0 P1 P2 P3 (least-significant pair first); 0x1b -> P3 P2 P1 P0.

TEST_F(Decoder8Test, Op09OnePixelPerTwoBits) {
  // P0 <= P1 and P2 <= P3: 16 bytes, two per row.
  DecodeSingleBlock(0x9, {0x10, 0x20, 0x30, 0x40,                           //
                          0xe4, 0xe4, 0x1b, 0x1b, 0x00, 0x00, 0x00, 0x00,   //
                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff}); //

  const char *expected = R"(
10 20 30 40 10 20 30 40
40 30 20 10 40 30 20 10
10 10 10 10 10 10 10 10
10 10 10 10 10 10 10 10
10 10 10 10 10 10 10 10
10 10 10 10 10 10 10 10
10 10 10 10 10 10 10 10
40 40 40 40 40 40 40 40
)";
  EXPECT_EQ(BlockText(0, 0), expected);
  ExpectSentinel();
}

TEST_F(Decoder8Test, Op09TwoBitsPer2x2Square) {
  // P0 <= P1 and P2 > P3: 4 bytes, one per pair of rows.
  DecodeSingleBlock(0x9, {0x10, 0x20, 0x40, 0x30, 0xe4, 0x00, 0xff, 0x1b});

  const char *expected = R"(
10 10 20 20 40 40 30 30
10 10 20 20 40 40 30 30
10 10 10 10 10 10 10 10
10 10 10 10 10 10 10 10
30 30 30 30 30 30 30 30
30 30 30 30 30 30 30 30
30 30 40 40 20 20 10 10
30 30 40 40 20 20 10 10
)";
  EXPECT_EQ(BlockText(0, 0), expected);
  ExpectSentinel();
}

TEST_F(Decoder8Test, Op09TwoBitsPer2x1Strip) {
  // P0 > P1 and P2 <= P3: 8 bytes, one per row, each pair of bits covering two horizontal pixels.
  DecodeSingleBlock(0x9, {0x20, 0x10, 0x30, 0x40, 0xe4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff});

  const char *expected = R"(
20 20 10 10 30 30 40 40
20 20 20 20 20 20 20 20
20 20 20 20 20 20 20 20
20 20 20 20 20 20 20 20
20 20 20 20 20 20 20 20
20 20 20 20 20 20 20 20
20 20 20 20 20 20 20 20
40 40 40 40 40 40 40 40
)";
  EXPECT_EQ(BlockText(0, 0), expected);
  ExpectSentinel();
}

TEST_F(Decoder8Test, Op09TwoBitsPer1x2Strip) {
  // P0 > P1 and P2 > P3: 8 bytes, two per pair of rows, each pair of bits covering two vertical pixels.
  DecodeSingleBlock(0x9, {0x20, 0x10, 0x40, 0x30, 0xe4, 0xe4, 0x00, 0x00, 0xff, 0xff, 0x1b, 0x1b});

  const char *expected = R"(
20 10 40 30 20 10 40 30
20 10 40 30 20 10 40 30
20 20 20 20 20 20 20 20
20 20 20 20 20 20 20 20
30 30 30 30 30 30 30 30
30 30 30 30 30 30 30 30
30 40 10 20 30 40 10 20
30 40 10 20 30 40 10 20
)";
  EXPECT_EQ(BlockText(0, 0), expected);
  ExpectSentinel();
}

TEST_F(Decoder8Test, Op0aFourQuadrantsWithTheirOwnColors) {
  // P0 <= P1: four (P0-P3 B0-B3) groups for the top-left, bottom-left, top-right and bottom-right quadrants.
  DecodeSingleBlock(0xa, {
                             0x01, 0x02, 0x03, 0x04, 0xe4, 0x00, 0xff, 0x1b, // TL
                             0x11, 0x12, 0x13, 0x14, 0xe4, 0xe4, 0xe4, 0xe4, // BL
                             0x21, 0x22, 0x23, 0x24, 0xe4, 0xe4, 0xe4, 0xe4, // TR
                             0x31, 0x32, 0x33, 0x34, 0xe4, 0xe4, 0xe4, 0xe4, // BR
                         });

  const char *expected = R"(
01 02 03 04 21 22 23 24
01 01 01 01 21 22 23 24
04 04 04 04 21 22 23 24
04 03 02 01 21 22 23 24
11 12 13 14 31 32 33 34
11 12 13 14 31 32 33 34
11 12 13 14 31 32 33 34
11 12 13 14 31 32 33 34
)";
  EXPECT_EQ(BlockText(0, 0), expected);
  ExpectSentinel();
}

TEST_F(Decoder8Test, Op0aLeftAndRightHalves) {
  // P0 > P1 and P4 <= P5: (P0-P3 B0-B7) is the left half, (P4-P7 B8-B15) the right half.
  DecodeSingleBlock(0xa, {
                             0x02, 0x01, 0x03, 0x04, 0xe4, 0xe4, 0xe4, 0xe4, 0x00, 0x00, 0x00, 0x00, // left
                             0x05, 0x06, 0x07, 0x08, 0xff, 0xff, 0xff, 0xff, 0x1b, 0x1b, 0x1b, 0x1b, // right
                         });

  const char *expected = R"(
02 01 03 04 08 08 08 08
02 01 03 04 08 08 08 08
02 01 03 04 08 08 08 08
02 01 03 04 08 08 08 08
02 02 02 02 08 07 06 05
02 02 02 02 08 07 06 05
02 02 02 02 08 07 06 05
02 02 02 02 08 07 06 05
)";
  EXPECT_EQ(BlockText(0, 0), expected);
  ExpectSentinel();
}

TEST_F(Decoder8Test, Op0aTopAndBottomHalves) {
  // P0 > P1 and P4 > P5: (P0-P3 B0-B7) is the top half, (P4-P7 B8-B15) the bottom half, two bytes per row.
  DecodeSingleBlock(0xa, {
                             0x02, 0x01, 0x03, 0x04, 0xe4, 0xe4, 0x00, 0x00, 0xff, 0xff, 0x1b, 0x1b, // top
                             0x06, 0x05, 0x07, 0x08, 0xe4, 0xe4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // bottom
                         });

  const char *expected = R"(
02 01 03 04 02 01 03 04
02 02 02 02 02 02 02 02
04 04 04 04 04 04 04 04
04 03 01 02 04 03 01 02
06 05 07 08 06 05 07 08
06 06 06 06 06 06 06 06
06 06 06 06 06 06 06 06
06 06 06 06 06 06 06 06
)";
  EXPECT_EQ(BlockText(0, 0), expected);
  ExpectSentinel();
}

TEST_F(Decoder8Test, Op00CopiesTheSameBlockFromThePreviousFrameAndOp01LeavesItAlone) {
  InitFrame(16, 8);
  FillFrames();
  auto cur = SnapshotCur();
  auto prev = SnapshotPrev();
  SetOp(0, 0, 0x0);
  SetOp(1, 0, 0x1);
  Decode({});

  EXPECT_EQ(CurBlock(0, 0), BlockAt(prev, 0, 0));
  EXPECT_EQ(CurBlock(1, 0), BlockAt(cur, 8, 0));
}

TEST_F(Decoder8Test, Op02CopiesFromBelowOrRightInTheCurrentFrame) {
  // B < 56: dx = 8 + B % 7, dy = B / 7. B >= 56: dx = -14 + (B - 56) % 29, dy = 8 + (B - 56) / 29.
  InitFrame(32, 24);
  FillFrames();
  auto cur = SnapshotCur();
  SetOp(0, 0, 0x2); // B = 9  -> (+10, +1)
  SetOp(2, 0, 0x2); // B = 88 -> (-11, +9)
  SetOp(3, 0, 0xe);
  Decode({9, 88, kSentinel});

  EXPECT_EQ(CurBlock(0, 0), BlockAt(cur, 0 + 10, 0 + 1));
  EXPECT_EQ(CurBlock(2, 0), BlockAt(cur, 16 - 11, 0 + 9));
  ExpectSolid(3, 0, kSentinel);
}

TEST_F(Decoder8Test, Op03CopiesFromAboveOrLeftInTheCurrentFrame) {
  // Opcode 0x2's offsets, negated.
  InitFrame(48, 24);
  FillFrames();
  auto cur = SnapshotCur();
  SetOp(0, 2, 0x3); // B = 88 -> (+11, -9)
  SetOp(2, 2, 0xe);
  SetOp(5, 2, 0x3); // B = 9  -> (-10, -1)
  Decode({88, kSentinel, 9});

  EXPECT_EQ(CurBlock(0, 2), BlockAt(cur, 0 + 11, 16 - 9));
  ExpectSolid(2, 2, kSentinel);
  EXPECT_EQ(CurBlock(5, 2), BlockAt(cur, 40 - 10, 16 - 1));
}

TEST_F(Decoder8Test, Op04CopiesANearbyBlockFromThePreviousFrame) {
  // dx = (B & 0xf) - 8, dy = (B >> 4) - 8.
  InitFrame(32, 24);
  FillFrames();
  auto prev = SnapshotPrev();
  SetOp(1, 1, 0x4); // B = 0x9a -> (+2, +1)
  SetOp(2, 1, 0x4); // B = 0x00 -> (-8, -8)
  SetOp(3, 1, 0xe);
  Decode({0x9a, 0x00, kSentinel});

  EXPECT_EQ(CurBlock(1, 1), BlockAt(prev, 8 + 2, 8 + 1));
  EXPECT_EQ(CurBlock(2, 1), BlockAt(prev, 16 - 8, 8 - 8));
  ExpectSolid(3, 1, kSentinel);
}

TEST_F(Decoder8Test, Op05CopiesFromThePreviousFrameAtASignedByteOffset) {
  InitFrame(32, 24);
  FillFrames();
  auto prev = SnapshotPrev();
  SetOp(1, 1, 0x5);
  SetOp(2, 1, 0xe);
  Decode({0xfd, 0x05, kSentinel}); // (-3, +5)

  EXPECT_EQ(CurBlock(1, 1), BlockAt(prev, 8 - 3, 8 + 5));
  ExpectSolid(2, 1, kSentinel);
}

} // namespace
