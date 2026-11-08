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

#include <cstdint>
#include <vector>

#include <gtest/gtest.h>

#include "bitmap.h"
#include "gr.h"
#include "support/surface.h"

using namespace twodlib_test;

TEST(GrMemorySurface, ClearRectFillsExactlyThatRect) {
  grMemorySurface sf(8, 6, BPP_16);
  const uint16_t blue = ClearValue(GR_BLUE), red = ClearValue(GR_RED);
  ASSERT_NE(blue, red);

  sf.clear(GR_BLUE);
  sf.clear(2, 1, 3, 2, GR_RED); // x 2..4, y 1..2

  for (int y = 0; y < 6; y++) {
    for (int x = 0; x < 8; x++) {
      const bool inside = x >= 2 && x < 5 && y >= 1 && y < 3;
      EXPECT_EQ(ReadPixel(sf, x, y), inside ? red : blue) << "at (" << x << "," << y << ")";
    }
  }
}

TEST(GrMemorySurface, ClearRespectsRowPitchOfWrappedMemory) {
  // 3 visible pixels per row, 5 pixels of storage per row.
  OwnedSurface s(3, 2, 5, 0, 1);
  s.surf.clear(GR_GREEN);
  const uint16_t green = ClearValue(GR_GREEN);

  for (int y = 0; y < 3; y++) {
    for (int x = 0; x < 5; x++) {
      const bool visible = x < 3 && y < 2;
      EXPECT_EQ(s.at(x, y), visible ? green : kSentinel) << "at (" << x << "," << y << ")";
    }
  }
}

TEST(GrMemorySurface, LockAtOffsetReturnsPointerToThatPixel) {
  grMemorySurface sf(8, 6, BPP_16);
  sf.clear(GR_BLACK);

  int full_rowsize, offset_rowsize;
  sf.lock(&full_rowsize);
  sf.unlock();

  uint16_t *p = reinterpret_cast<uint16_t *>(sf.lock(3, 2, &offset_rowsize));
  *p = 0x1234;
  sf.unlock();

  EXPECT_EQ(offset_rowsize, full_rowsize);
  EXPECT_EQ(ReadPixel(sf, 3, 2), 0x1234);
  EXPECT_EQ(ReadPixel(sf, 2, 2), ClearValue(GR_BLACK));
  EXPECT_EQ(ReadPixel(sf, 3, 1), ClearValue(GR_BLACK));
}

TEST(GrMemorySurface, BltCopiesSourceRectToDestinationPosition) {
  OwnedSurface src(4, 4, 6);
  for (int y = 0; y < 4; y++)
    for (int x = 0; x < 4; x++)
      src.at(x, y) = (uint16_t)(0x100 | (y << 4) | x); // no opaque bit: plain blt copies regardless

  OwnedSurface dst(8, 8, 9);
  dst.surf.blt(3, 2, &src.surf, 1, 1, 2, 3);

  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 9; x++) {
      const bool inside = x >= 3 && x < 5 && y >= 2 && y < 5;
      const uint16_t expected = inside ? src.at(x - 3 + 1, y - 2 + 1) : kSentinel;
      EXPECT_EQ(dst.at(x, y), expected) << "at (" << x << "," << y << ")";
    }
  }
}

TEST(GrMemorySurface, ColorKeyedBltSkipsPixelsWithoutOpaqueFlag) {
  OwnedSurface src(4, 1, 4, SURFFLAG_COLORKEY);
  src.at(0, 0) = OPAQUE_FLAG16 | 0x001F;
  src.at(1, 0) = 0x001F;
  src.at(2, 0) = OPAQUE_FLAG16; // opaque black is still drawn
  src.at(3, 0) = 0x0000;

  OwnedSurface dst(4, 1, 4);
  dst.surf.blt(0, 0, &src.surf);

  EXPECT_EQ(dst.at(0, 0), OPAQUE_FLAG16 | 0x001F);
  EXPECT_EQ(dst.at(1, 0), kSentinel);
  EXPECT_EQ(dst.at(2, 0), OPAQUE_FLAG16);
  EXPECT_EQ(dst.at(3, 0), kSentinel);
}

TEST(GrMemorySurface, ReplaceColorReplacesOnlyMatchingPixels) {
  grMemorySurface sf(6, 4, BPP_16);
  sf.clear(GR_BLUE);
  sf.clear(1, 1, 2, 2, GR_RED);

  sf.replace_color(GR_RED, GR_GREEN);

  const uint16_t blue = ClearValue(GR_BLUE), green = ClearValue(GR_GREEN);
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 6; x++) {
      const bool inside = x >= 1 && x < 3 && y >= 1 && y < 3;
      EXPECT_EQ(ReadPixel(sf, x, y), inside ? green : blue) << "at (" << x << "," << y << ")";
    }
  }
}

TEST(GrMemorySurface, Load1555SameSizeCopiesPixels) {
  const uint16_t data[2][3] = {{0x8001, 0x7FFF, 0x0000}, {0x1234, 0xFFFF, 0x8000}};
  OwnedSurface s(3, 2, 5);

  s.surf.load(const_cast<char *>(reinterpret_cast<const char *>(data)), 3, 2, BPP_16, BITMAP_FORMAT_1555);

  for (int y = 0; y < 2; y++) {
    for (int x = 0; x < 5; x++)
      EXPECT_EQ(s.at(x, y), x < 3 ? data[y][x] : kSentinel) << "at (" << x << "," << y << ")";
  }
}

TEST(GrMemorySurface, Load1555IntoLargerSurfaceScalesByPixelReplication) {
  const uint16_t data[2][2] = {{0x8001, 0x8002}, {0x8003, 0x8004}};
  OwnedSurface s(4, 4, 4);

  s.surf.load(const_cast<char *>(reinterpret_cast<const char *>(data)), 2, 2, BPP_16, BITMAP_FORMAT_1555);

  for (int y = 0; y < 4; y++)
    for (int x = 0; x < 4; x++)
      EXPECT_EQ(s.at(x, y), data[y / 2][x / 2]) << "at (" << x << "," << y << ")";
}

TEST(GrMemorySurface, Load4444ProducesOpaquePixelsWithSameColor) {
  // ARGB4444, alpha fully opaque.
  const uint16_t data[4] = {0xFF00, 0xF0F0, 0xF00F, 0xF840};
  const int expected_rgb[4][3] = {{0xFF, 0, 0}, {0, 0xFF, 0}, {0, 0, 0xFF}, {0x88, 0x44, 0}};
  OwnedSurface s(4, 1, 4);

  s.surf.load(const_cast<char *>(reinterpret_cast<const char *>(data)), 4, 1, BPP_16, BITMAP_FORMAT_4444);

  for (int i = 0; i < 4; i++) {
    SCOPED_TRACE(i);
    EXPECT_TRUE(s.at(i, 0) & OPAQUE_FLAG16);
    // 4 bits per channel -> 5 bits per channel loses precision, so allow one 4-bit step.
    const ddgr_color c = GR_16_TO_COLOR(s.at(i, 0));
    EXPECT_NEAR(GR_COLOR_RED(c), expected_rgb[i][0], 17);
    EXPECT_NEAR(GR_COLOR_GREEN(c), expected_rgb[i][1], 17);
    EXPECT_NEAR(GR_COLOR_BLUE(c), expected_rgb[i][2], 17);
  }
}

TEST(GrMemorySurface, Load24BitPlacesEachRowInTheMatchingSurfaceRow) {
  // All channel values below 0x80 to stay clear of sign-extension issues.
  const uint8_t image[2][2][3] = {{{0x10, 0x20, 0x30}, {0x18, 0x28, 0x38}},
                                  {{0x40, 0x08, 0x18}, {0x48, 0x10, 0x20}}};

  OwnedSurface row1_alone(2, 1, 2);
  row1_alone.surf.load(const_cast<char *>(reinterpret_cast<const char *>(image[1])), 2, 1, BPP_24);

  OwnedSurface s(2, 2, 2, 0, 4);
  s.surf.load(const_cast<char *>(reinterpret_cast<const char *>(image)), 2, 2, BPP_24);

  EXPECT_EQ(s.at(0, 1), row1_alone.at(0, 0));
  EXPECT_EQ(s.at(1, 1), row1_alone.at(1, 0));
  for (int y = 2; y < 6; y++)
    for (int x = 0; x < 2; x++)
      EXPECT_EQ(s.at(x, y), kSentinel) << "wrote outside surface at (" << x << "," << y << ")";
}

TEST(GrMemorySurface, Load24BitMatchesClearForSameColor) {
  const uint8_t rgb[4][3] = {{0x10, 0x20, 0x30}, {0x40, 0x60, 0x08}, {0xF8, 0x80, 0x08}, {0xFF, 0xFF, 0xFF}};
  OwnedSurface s(4, 1, 4);

  s.surf.load(const_cast<char *>(reinterpret_cast<const char *>(rgb)), 4, 1, BPP_24);

  for (int i = 0; i < 4; i++) {
    SCOPED_TRACE(i);
    EXPECT_EQ(s.at(i, 0) & 0x7FFF, ClearValue(GR_RGB(rgb[i][0], rgb[i][1], rgb[i][2])) & 0x7FFF);
  }
}

TEST(GrMemorySurface, Load32BitMatchesClearForSameColor) {
  const uint32_t argb[4] = {0x00102030, 0x00406008, 0x00F88008, 0x00FFFFFF};
  OwnedSurface s(4, 1, 4); // xlat32_16 requires a width that is a multiple of 4

  s.surf.load(const_cast<char *>(reinterpret_cast<const char *>(argb)), 4, 1, BPP_32);

  for (int i = 0; i < 4; i++) {
    SCOPED_TRACE(i);
    EXPECT_EQ(s.at(i, 0) & 0x7FFF, ClearValue(argb[i]) & 0x7FFF);
  }
}

namespace {
struct GrViewportBltTest : ::testing::Test {
  OwnedSurface src{4, 3, 4};
  OwnedSurface dst{6, 5, 7}; // one column of row padding to catch writes past the right edge

  void SetUp() override {
    for (int y = 0; y < 3; y++)
      for (int x = 0; x < 4; x++)
        src.at(x, y) = (uint16_t)(0x100 | (y << 4) | x);
  }
};
} // namespace

TEST_F(GrViewportBltTest, WholeSurfaceBltClipsAtTopLeft) {
  grViewport vp(&dst.surf);
  vp.blt(-2, -1, &src.surf);
  ExpectBlock(dst, src, 0, 0, 2, 1, 2, 2);
}

TEST_F(GrViewportBltTest, WholeSurfaceBltClipsAtBottomRight) {
  grViewport vp(&dst.surf);
  vp.blt(4, 3, &src.surf);
  ExpectBlock(dst, src, 4, 3, 0, 0, 2, 2);
}

TEST_F(GrViewportBltTest, BltEntirelyOutsideViewportDrawsNothing) {
  grViewport vp(&dst.surf);
  vp.blt(6, 0, &src.surf);
  vp.blt(-4, 0, &src.surf);
  vp.blt(0, 5, &src.surf);
  vp.blt(0, -3, &src.surf);
  ExpectBlock(dst, src, 0, 0, 0, 0, 0, 0);
}

TEST_F(GrViewportBltTest, SubRectBltClipsAtBottomRight) {
  grViewport vp(&dst.surf);
  vp.blt(4, 4, &src.surf, 1, 0, 3, 2);
  ExpectBlock(dst, src, 4, 4, 1, 0, 2, 1);
}

TEST_F(GrViewportBltTest, SubRectBltClippedAtTopLeftDrawsVisiblePartOfSource) {
  grViewport vp(&dst.surf);
  vp.blt(-1, -1, &src.surf, 0, 0, 3, 3);
  ExpectBlock(dst, src, 0, 0, 1, 1, 2, 2);
}
