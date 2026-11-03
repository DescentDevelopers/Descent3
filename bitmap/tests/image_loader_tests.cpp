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

#include <set>
#include <vector>

#include <gtest/gtest.h>

#include "bitmap.h"
#include "cfile.h"
#include "grdefs.h"
#include "iff.h"
#include "support/image_builders.h"
#include "support/image_loading.h"
#include "support/test_support.h"

namespace {

using ImageLoaderTest = BitmapLibTest;

const Bgra kRed{0, 0, 255, 255};
const Bgra kGreen{0, 255, 0, 255};
const Bgra kBlue{255, 0, 0, 255};
const Bgra kGrey{128, 128, 128, 255};
const Bgra kClear{255, 255, 255, 0};

TEST_F(ImageLoaderTest, TgaBothRowOrdersDecodeToTopDownBitmap) {
  const std::vector<std::vector<Bgra>> rows = {{kRed, kGreen, kGrey}, {kBlue, kClear, kRed}};

  for (bool top_down : {false, true}) {
    SCOPED_TRACE(top_down ? "top-down file" : "bottom-up file");
    int handle = Track(LoadFromBytes(temp_dir, RawTga(rows, 32, top_down), LoadTga1555));
    ASSERT_GT(handle, 0);
    ASSERT_EQ(bm_w(handle, 0), 3);
    ASSERT_EQ(bm_h(handle, 0), 2);
    std::vector<uint16_t> px = Pixels(handle);
    // 8-bit channels keep their top 5 bits: 255 -> 31, 128 -> 16.
    EXPECT_EQ(px[0], Rgb1555(31, 0, 0));
    EXPECT_EQ(px[1], Rgb1555(0, 31, 0));
    EXPECT_EQ(px[2], Rgb1555(16, 16, 16));
    EXPECT_EQ(px[3], Rgb1555(0, 0, 31));
    EXPECT_TRUE(IsTransparent1555(px[4]));
    EXPECT_EQ(px[5], Rgb1555(31, 0, 0));
  }
}

TEST_F(ImageLoaderTest, TgaImageIdFieldIsSkipped) {
  const std::vector<std::vector<Bgra>> rows = {{kRed, kGreen}, {kBlue, kGrey}};
  int plain = Track(LoadFromBytes(temp_dir, RawTga(rows, 32, false), LoadTga1555));
  int with_id = Track(LoadFromBytes(temp_dir, RawTga(rows, 32, false, "made by a test"), LoadTga1555));
  ASSERT_GT(plain, 0);
  ASSERT_GT(with_id, 0);
  EXPECT_EQ(Pixels(with_id), Pixels(plain));
}

TEST_F(ImageLoaderTest, RleTgaDecodesSameAsUncompressed32Bit) {
  const std::vector<std::vector<Bgra>> rows = {
      {kRed, kRed, kRed, kRed, kGreen},
      {kBlue, kGrey, kClear, kClear, kClear},
      {kGrey, kGrey, kRed, kGreen, kBlue},
  };
  int raw = Track(LoadFromBytes(temp_dir, RawTga(rows, 32, false), LoadTga1555));
  int rle = Track(LoadFromBytes(temp_dir, RleTga(rows, 32), LoadTga1555));
  ASSERT_GT(raw, 0);
  ASSERT_GT(rle, 0);
  EXPECT_EQ(Pixels(rle), Pixels(raw));
}

TEST_F(ImageLoaderTest, Uncompressed24BitTgaIsOpaqueWithCorrectChannels) {
  const std::vector<std::vector<Bgra>> rows = {{kRed, kGreen, kBlue}};
  int handle = Track(LoadFromBytes(temp_dir, RawTga(rows, 24, false), LoadTga1555));
  ASSERT_GT(handle, 0);
  EXPECT_EQ(Pixels(handle), (std::vector<uint16_t>{Rgb1555(31, 0, 0), Rgb1555(0, 31, 0), Rgb1555(0, 0, 31)}));
}

TEST_F(ImageLoaderTest, TgaLoadedAs4444KeepsTopFourBitsOfEachChannelAndAlpha) {
  const std::vector<std::vector<Bgra>> rows = {{Bgra{0x10, 0x80, 0xFF, 0x80}, Bgra{0, 0, 0, 0}}};
  int handle = Track(LoadFromBytes(temp_dir, RawTga(rows, 32, false), LoadTga4444));
  ASSERT_GT(handle, 0);
  EXPECT_EQ(bm_format(handle), BITMAP_FORMAT_4444);
  EXPECT_EQ(Pixels(handle), (std::vector<uint16_t>{Argb4444(8, 15, 8, 1), Argb4444(0, 0, 0, 0)}));
}

TEST_F(ImageLoaderTest, UnsupportedTgaVariantsAreRejectedWithoutAllocating) {
  const std::set<int> before = UsedSlots();

  // Colour-mapped image
  d3_test::ByteWriter mapped;
  mapped.U8(0).U8(1).U8(1).Fill(15, 0);
  EXPECT_EQ(Track(LoadFromBytes(temp_dir, mapped.bytes(), LoadTga1555)), -1);

  d3_test::ByteWriter sixteen = TgaHeader(kTgaTrueColor, 1, 1, 16, 0);
  sixteen.U16(0xFFFF);
  EXPECT_EQ(Track(LoadFromBytes(temp_dir, sixteen.bytes(), LoadTga1555)), -1);

  EXPECT_EQ(UsedSlots(), before);
}

const std::vector<Rgb> kPalette = {
    {0, 0, 0},       // 0: transparent by convention
    {255, 0, 0},     // 1
    {0, 128, 255},   // 2
    {8, 16, 24},     // 3
};

TEST_F(ImageLoaderTest, Pcx8BitMapsPaletteAndTreatsIndexZeroAsTransparent) {
  // Includes a run, and a literal index >= 0xC0 that has to be escaped.
  std::vector<Rgb> palette = kPalette;
  palette.resize(256, Rgb{0, 0, 0});
  palette[200] = {64, 64, 64};
  const std::vector<std::vector<uint8_t>> rows = {{1, 1, 1, 2}, {200, 0, 3, 1}};

  int handle = Track(LoadFromBytes(temp_dir, Pcx8(rows, palette), bm_pcx_alloc_file));
  ASSERT_GT(handle, 0);
  ASSERT_EQ(bm_w(handle, 0), 4);
  ASSERT_EQ(bm_h(handle, 0), 2);
  std::vector<uint16_t> px = Pixels(handle);
  const uint16_t red = Rgb1555(31, 0, 0);
  EXPECT_EQ(px[0], red);
  EXPECT_EQ(px[1], red);
  EXPECT_EQ(px[2], red);
  EXPECT_EQ(px[3], Rgb1555(0, 16, 31));
  EXPECT_EQ(px[4], Rgb1555(8, 8, 8));
  EXPECT_TRUE(IsTransparent1555(px[5]));
  EXPECT_EQ(px[6], Rgb1555(1, 2, 3));
  EXPECT_EQ(px[7], red);
}

TEST_F(ImageLoaderTest, Pcx24BitDecodesPlanesAndSkipsScanlinePadding) {
  // Width 3 means each plane line carries one padding byte.
  const std::vector<std::vector<Rgb>> rows = {
      {{255, 0, 0}, {0, 255, 0}, {0, 0, 255}},
      {{128, 128, 128}, {255, 255, 255}, {0, 0, 0}},
  };
  int handle = Track(LoadFromBytes(temp_dir, Pcx24(rows), bm_pcx_alloc_file));
  ASSERT_GT(handle, 0);
  ASSERT_EQ(bm_w(handle, 0), 3);
  ASSERT_EQ(bm_h(handle, 0), 2);
  // 24-bit PCX has no transparency: black stays an opaque colour.
  EXPECT_EQ(Pixels(handle), (std::vector<uint16_t>{Rgb1555(31, 0, 0), Rgb1555(0, 31, 0), Rgb1555(0, 0, 31),
                                                   Rgb1555(16, 16, 16), Rgb1555(31, 31, 31), Rgb1555(0, 0, 0)}));
}

TEST_F(ImageLoaderTest, PcxWithUnsupportedPlaneCountIsRejected) {
  d3_test::ByteWriter pcx = PcxHeader(2, 2, 4, 2);
  pcx.Fill(64, 0);
  EXPECT_EQ(Track(LoadFromBytes(temp_dir, pcx.bytes(), bm_pcx_alloc_file)), -1);
}

constexpr uint8_t kMaskNone = 0;
constexpr uint8_t kMaskTransparentColor = 2;

// Expected 1555 colour of each kPalette entry (8-bit channels -> top 5 bits).
const uint16_t kC1 = Rgb1555(31, 0, 0), kC2 = Rgb1555(0, 16, 31), kC3 = Rgb1555(1, 2, 3);

TEST_F(ImageLoaderTest, IffPbmUncompressedAndByteRun1DecodeTheSame) {
  const std::vector<uint8_t> raw_body = {1, 1, 1, 2, 0, 3, 3, 3};
  // ByteRun1: n in [0,127] = n+1 literal bytes, n in [-127,-1] = repeat next byte 1-n times
  const std::vector<uint8_t> packed_body = {0xFE, 1, 0x00, 2, 0x00, 0, 0xFE, 3};

  int raw = Track(LoadFromBytes(temp_dir, IffPbm(4, 2, kMaskNone, 0, kPalette, 0, raw_body), bm_iff_alloc_file));
  int packed = Track(LoadFromBytes(temp_dir, IffPbm(4, 2, kMaskNone, 0, kPalette, 1, packed_body), bm_iff_alloc_file));
  ASSERT_GT(raw, 0);
  ASSERT_GT(packed, 0);
  ASSERT_EQ(bm_w(raw, 0), 4);
  ASSERT_EQ(bm_h(raw, 0), 2);

  std::vector<uint16_t> px = Pixels(raw);
  EXPECT_EQ(px[0], kC1);
  EXPECT_EQ(px[1], kC1);
  EXPECT_EQ(px[2], kC1);
  EXPECT_EQ(px[3], kC2);
  EXPECT_EQ(px[5], kC3);
  EXPECT_EQ(px[6], kC3);
  EXPECT_EQ(px[7], kC3);
  EXPECT_EQ(Pixels(packed), px);
}

TEST_F(ImageLoaderTest, IffPbmTransparentColourIndexBecomesTransparent) {
  const std::vector<uint8_t> body = {0, 1, 2, 3};
  int handle =
      Track(LoadFromBytes(temp_dir, IffPbm(4, 1, kMaskTransparentColor, 2, kPalette, 0, body), bm_iff_alloc_file));
  ASSERT_GT(handle, 0);
  std::vector<uint16_t> px = Pixels(handle);
  EXPECT_EQ(px[1], kC1);
  EXPECT_TRUE(IsTransparent1555(px[2]));
  EXPECT_EQ(px[3], kC3);
}

TEST_F(ImageLoaderTest, IffPbmUncompressedOddWidthSkipsRowPadding) {
  // Uncompressed PBM rows are padded to an even number of bytes.
  const std::vector<uint8_t> body = {1, 2, 3, 0xEE, 3, 2, 1, 0xEE};
  int handle = Track(LoadFromBytes(temp_dir, IffPbm(3, 2, kMaskNone, 0, kPalette, 0, body), bm_iff_alloc_file));
  ASSERT_GT(handle, 0);
  ASSERT_EQ(bm_w(handle, 0), 3);
  ASSERT_EQ(bm_h(handle, 0), 2);
  EXPECT_EQ(Pixels(handle), (std::vector<uint16_t>{kC1, kC2, kC3, kC3, kC2, kC1}));
}

TEST_F(ImageLoaderTest, IffThatIsNotPbmIsRejected) {
  const std::vector<uint8_t> body(8, 1);
  EXPECT_EQ(Track(LoadFromBytes(temp_dir, IffPbm(4, 2, kMaskNone, 0, kPalette, 0, body, "ILBM"), bm_iff_alloc_file)),
            -1);
}

} // namespace
