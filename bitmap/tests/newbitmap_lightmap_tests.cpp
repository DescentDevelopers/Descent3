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

#include <algorithm>
#include <iterator>
#include <string>
#include <tuple>
#include <vector>

#include <gtest/gtest.h>

#include "NewBitmap.h"
#include "d3_test/files.h"
#include "lightmap.h"
#include "support/test_support.h"

namespace {

using NewBitmapTest = BitmapLibTest;

TEST_F(NewBitmapTest, SaveAsPngWritesPngWithBitmapDimensions) {
  NewBitmap bitmap(7, 3, PixelDataFormat::RGBA32);
  ASSERT_NE(bitmap.getData(), nullptr);
  for (int i = 0; i < 7 * 3 * 4; i++)
    bitmap.getData()[i] = static_cast<uint8_t>(i);

  std::filesystem::path path = temp_dir / "shot.png";
  bitmap.saveAsPNG(path.string().c_str());

  // PNG signature, then the IHDR chunk carrying big-endian width and height.
  std::vector<uint8_t> png = d3_test::ReadFileBytes(path);
  ASSERT_GE(png.size(), 24u);
  const uint8_t signature[8] = {0x89, 'P', 'N', 'G', '\r', '\n', 0x1A, '\n'};
  EXPECT_TRUE(std::equal(std::begin(signature), std::end(signature), png.begin()));
  EXPECT_EQ(std::string(png.begin() + 12, png.begin() + 16), "IHDR");
  EXPECT_EQ(ReadBe32(png, 16), 7u);
  EXPECT_EQ(ReadBe32(png, 20), 3u);
}

class LightmapSquareResTest : public ::testing::TestWithParam<std::tuple<int, int, int>> {
protected:
  void TearDown() override {
    if (handle_ >= 0)
      lm_FreeLightmap(handle_);
  }
  int handle_ = -1;
};

// square_res is the side of the square texture the renderer uploads the
// lightmap into (render.cpp scales texture coordinates by width / square_res),
// so it has to be a power of two that is at least as large as both sides, and
// the smallest such one so no texture memory is wasted.
TEST_P(LightmapSquareResTest, IsSmallestPowerOfTwoHoldingBothSides) {
  const auto [w, h, expected] = GetParam();
  handle_ = lm_AllocLightmap(w, h);
  ASSERT_GE(handle_, 0);
  EXPECT_EQ(GameLightmaps[handle_].square_res, expected);
}

INSTANTIATE_TEST_SUITE_P(Sizes, LightmapSquareResTest,
                         ::testing::Values(std::make_tuple(1, 1, 2), std::make_tuple(2, 2, 2),
                                           std::make_tuple(3, 1, 4), std::make_tuple(1, 5, 8),
                                           std::make_tuple(8, 8, 8), std::make_tuple(9, 3, 16),
                                           std::make_tuple(33, 64, 64), std::make_tuple(65, 2, 128),
                                           std::make_tuple(128, 128, 128)),
                         [](const auto &info) {
                           return std::to_string(std::get<0>(info.param)) + "x" +
                                  std::to_string(std::get<1>(info.param));
                         });

} // namespace
