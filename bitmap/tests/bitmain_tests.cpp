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

#include <cstring>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "bitmap.h"
#include "cfile.h"
#include "grdefs.h"
#include "support/test_support.h"

namespace {

using OgfRoundTripTest = BitmapLibTest;

TEST_F(OgfRoundTripTest, PreservesNameSizeAndPixels) {
  // Odd dimensions plus a mix of content that exercises every RLE case: a run
  // longer than the 250-pixel maximum run length, isolated pixels, short runs
  // and transparent pixels.
  const int w = 37, h = 13;
  std::vector<uint16_t> pixels(w * h);
  for (int i = 0; i < w * h; i++) {
    if (i < 300)
      pixels[i] = Rgb1555(3, 7, 11);
    else if (i % 5 == 0)
      pixels[i] = NEW_TRANSPARENT_COLOR;
    else
      pixels[i] = Rgb1555(i % 32, (i / 2) % 32, (i / 3) % 32);
  }
  int src = MakeBitmap(w, h, pixels);
  strcpy(GameBitmaps[src].name, "roundtrip.ogf");

  int loaded = Track(SaveAndReload(temp_dir, src, 0));
  ASSERT_GT(loaded, 0);
  EXPECT_STREQ(GameBitmaps[loaded].name, "roundtrip.ogf");
  EXPECT_EQ(bm_w(loaded, 0), w);
  EXPECT_EQ(bm_h(loaded, 0), h);
  EXPECT_EQ(bm_format(loaded), BITMAP_FORMAT_1555);
  EXPECT_EQ(Pixels(loaded), pixels);
}

TEST_F(OgfRoundTripTest, Preserves4444Format) {
  const int w = 4, h = 3;
  std::vector<uint16_t> pixels;
  for (int i = 0; i < w * h; i++)
    pixels.push_back(Argb4444(i % 16, 15 - i, (i * 3) % 16, 7));
  int src = MakeBitmap(w, h, pixels, BITMAP_FORMAT_4444);

  int loaded = Track(SaveAndReload(temp_dir, src, 0));
  ASSERT_GT(loaded, 0);
  EXPECT_EQ(bm_format(loaded), BITMAP_FORMAT_4444);
  EXPECT_EQ(Pixels(loaded), pixels);
}

TEST_F(OgfRoundTripTest, PreservesAllMipLevels) {
  const int w = 16, h = 8;
  std::vector<uint16_t> pixels(w * h);
  for (int i = 0; i < w * h; i++)
    pixels[i] = Rgb1555(i % 32, (i * 7) % 32, (i * 13) % 32);
  int src = MakeBitmap(w, h, pixels, BITMAP_FORMAT_1555, true);
  bm_GenerateMipMaps(src);
  ASSERT_EQ(bm_mipped(src), 1);

  int loaded = Track(SaveAndReload(temp_dir, src, 1));
  ASSERT_GT(loaded, 0);
  ASSERT_EQ(bm_mipped(loaded), 1);
  for (int m = 0; m < bm_miplevels(src); m++) {
    if (bm_w(src, m) == 0 || bm_h(src, m) == 0)
      break;
    SCOPED_TRACE("mip level " + std::to_string(m));
    EXPECT_EQ(bm_w(loaded, m), bm_w(src, m));
    EXPECT_EQ(bm_h(loaded, m), bm_h(src, m));
    EXPECT_EQ(Pixels(loaded, m), Pixels(src, m));
  }
}

TEST_F(OgfRoundTripTest, LoadingMippedFileWithoutMipsKeepsTopLevel) {
  const int w = 8, h = 8;
  std::vector<uint16_t> pixels(w * h);
  for (int i = 0; i < w * h; i++)
    pixels[i] = Rgb1555(i % 32, 31 - (i % 32), 5);
  int src = MakeBitmap(w, h, pixels, BITMAP_FORMAT_1555, true);
  bm_GenerateMipMaps(src);

  int loaded = Track(SaveAndReload(temp_dir, src, 0));
  ASSERT_GT(loaded, 0);
  EXPECT_EQ(bm_mipped(loaded), 0);
  EXPECT_EQ(bm_w(loaded, 0), w);
  EXPECT_EQ(bm_h(loaded, 0), h);
  EXPECT_EQ(Pixels(loaded), pixels);
}

using BitmapNameTest = BitmapLibTest;

TEST_F(BitmapNameTest, LoadedBitmapIsFoundByNameCaseInsensitively) {
  std::filesystem::path path = WriteOgf(temp_dir, "wallpanel.ogf", Rgb1555(9, 9, 9));
  int handle = Track(bm_AllocLoadFileBitmap(path.string().c_str(), 0));
  ASSERT_GT(handle, 0);
  EXPECT_EQ(Pixels(handle), std::vector<uint16_t>(4, Rgb1555(9, 9, 9)));

  EXPECT_EQ(bm_FindBitmapName("wallpanel.ogf"), handle);
  EXPECT_EQ(bm_FindBitmapName("WallPanel.OGF"), handle);
  EXPECT_EQ(bm_FindBitmapName("wallpanel2.ogf"), -1);
}

TEST_F(BitmapNameTest, LoadingSameFileTwiceSharesOneRefcountedHandle) {
  std::filesystem::path path = WriteOgf(temp_dir, "shared.ogf");
  int first = Track(bm_AllocLoadFileBitmap(path.string().c_str(), 0));
  int second = Track(bm_AllocLoadFileBitmap(path.string().c_str(), 0));
  ASSERT_GT(first, 0);
  EXPECT_EQ(second, first);

  bm_FreeBitmap(first);
  EXPECT_GT(GameBitmaps[first].used, 0);
  EXPECT_EQ(bm_FindBitmapName("shared.ogf"), first);
  EXPECT_EQ(Pixels(first), std::vector<uint16_t>(4, Rgb1555(1, 2, 3)));

  bm_FreeBitmap(first);
  EXPECT_EQ(GameBitmaps[first].used, 0);
  EXPECT_EQ(bm_FindBitmapName("shared.ogf"), -1);
}

TEST_F(BitmapNameTest, ChangeEndNameStripsDirectoryAndExtension) {
  std::string src = (temp_dir / "Wall.tga").string();
  char dest[BITMAP_NAME_LEN + 16];
  bm_ChangeEndName(src.c_str(), dest);
  EXPECT_STREQ(dest, "Wall.ogf");
}

TEST_F(BitmapNameTest, ChangeEndNameAvoidsNamesAlreadyInUse) {
  std::filesystem::path path = WriteOgf(temp_dir, "taken.ogf");
  int handle = Track(bm_AllocLoadFileBitmap(path.string().c_str(), 0));
  ASSERT_GT(handle, 0);
  ASSERT_EQ(bm_FindBitmapName("taken.ogf"), handle);

  std::string src = (temp_dir / "taken.tga").string();
  char dest[BITMAP_NAME_LEN + 16];
  bm_ChangeEndName(src.c_str(), dest);
  EXPECT_STRCASENE(dest, "taken.ogf");
  EXPECT_EQ(bm_FindBitmapName(dest), -1);
  const size_t len = strlen(dest);
  ASSERT_GT(len, 4u);
  EXPECT_STREQ(dest + len - 4, ".ogf");
}

TEST_F(BitmapNameTest, ChangeEndNameShortensLongNamesToFitBitmapName) {
  std::string long_name(60, 'x');
  std::string src = (temp_dir / (long_name + ".tga")).string();
  char dest[BITMAP_NAME_LEN + 16];
  bm_ChangeEndName(src.c_str(), dest);
  // bm_AllocLoadFileBitmap refuses names longer than 33 characters, and the
  // name has to fit in bms_bitmap::name with its terminator.
  EXPECT_LE(strlen(dest), 33u);
  EXPECT_LT(strlen(dest), static_cast<size_t>(BITMAP_NAME_LEN));
  EXPECT_EQ(std::string(dest).find("xxxx"), 0u);
}

TEST_F(BitmapLibTest, GenerateMipMapsAveragesEachTwoByTwoBlock1555) {
  // clang-format off
  const std::vector<uint16_t> src = {
    Rgb1555(2, 12, 1),  Rgb1555(4, 12, 1),  Rgb1555(31, 0, 0),  Rgb1555(31, 0, 0),
    Rgb1555(6, 12, 1),  Rgb1555(8, 12, 1),  Rgb1555(31, 0, 0),  Rgb1555(31, 0, 0),
    Rgb1555(0, 0, 7),   Rgb1555(0, 8, 7),   Rgb1555(20, 20, 1), Rgb1555(20, 20, 3),
    Rgb1555(0, 16, 7),  Rgb1555(0, 24, 7),  Rgb1555(20, 20, 5), Rgb1555(20, 20, 7),
  };
  // clang-format on
  int handle = MakeBitmap(4, 4, src, BITMAP_FORMAT_1555, true);
  bm_GenerateMipMaps(handle);

  ASSERT_EQ(bm_mipped(handle), 1);
  ASSERT_EQ(bm_w(handle, 1), 2);
  ASSERT_EQ(bm_h(handle, 1), 2);
  EXPECT_EQ(Pixels(handle, 1),
            (std::vector<uint16_t>{Rgb1555(5, 12, 1), Rgb1555(31, 0, 0), Rgb1555(0, 12, 7), Rgb1555(20, 20, 4)}));
  ASSERT_EQ(bm_w(handle, 2), 1);
  ASSERT_EQ(bm_h(handle, 2), 1);
  EXPECT_EQ(Pixels(handle, 2), std::vector<uint16_t>{Rgb1555(14, 11, 3)});
  EXPECT_EQ(Pixels(handle, 0), src);
}

TEST_F(BitmapLibTest, GenerateMipMapsTreatsMostlyTransparentBlocksAsTransparent) {
  const uint16_t T = NEW_TRANSPARENT_COLOR;
  const uint16_t O = Rgb1555(10, 10, 10);
  // clang-format off
  const std::vector<uint16_t> src = {
    T, T, T, T,
    T, T, T, O,
    O, O, O, O,
    O, O, O, O,
  };
  // clang-format on
  int handle = MakeBitmap(4, 4, src, BITMAP_FORMAT_1555, true);
  bm_GenerateMipMaps(handle);

  std::vector<uint16_t> mip = Pixels(handle, 1);
  ASSERT_EQ(mip.size(), 4u);
  EXPECT_TRUE(IsTransparent1555(mip[0]));
  EXPECT_TRUE(IsTransparent1555(mip[1]));
  EXPECT_EQ(mip[2], O);
  EXPECT_EQ(mip[3], O);
}

TEST_F(BitmapLibTest, GenerateMipMapsAveragesAllChannelsIncludingAlpha4444) {
  const std::vector<uint16_t> src = {Argb4444(15, 4, 1, 0), Argb4444(15, 8, 1, 4), Argb4444(0, 12, 1, 0),
                                     Argb4444(2, 0, 1, 0)};
  int handle = MakeBitmap(2, 2, src, BITMAP_FORMAT_4444, true);
  bm_GenerateMipMaps(handle);

  EXPECT_EQ(Pixels(handle, 1), std::vector<uint16_t>{Argb4444(8, 6, 1, 1)});
}

TEST_F(BitmapLibTest, ScaleBitmapToBitmapUpscalesByReplicatingPixels) {
  const uint16_t a = Rgb1555(1, 0, 0), b = Rgb1555(0, 2, 0), c = Rgb1555(0, 0, 3), d = Rgb1555(4, 4, 4);
  int src = MakeBitmap(2, 2, {a, b, c, d});
  int dest = MakeBitmap(4, 4, std::vector<uint16_t>(16, 0));

  bm_ScaleBitmapToBitmap(dest, src);

  // clang-format off
  EXPECT_EQ(Pixels(dest), (std::vector<uint16_t>{
    a, a, b, b,
    a, a, b, b,
    c, c, d, d,
    c, c, d, d,
  }));
  // clang-format on
}

TEST_F(BitmapLibTest, ChangeSizeResizesInPlaceKeepingHandleAndFormat) {
  const uint16_t a = Argb4444(15, 1, 0, 0), b = Argb4444(8, 0, 2, 0), c = Argb4444(4, 0, 0, 3),
                 d = Argb4444(0, 4, 4, 4);
  int handle = MakeBitmap(2, 2, {a, b, c, d}, BITMAP_FORMAT_4444);

  bm_ChangeSize(handle, 4, 4);

  EXPECT_GT(GameBitmaps[handle].used, 0);
  EXPECT_EQ(bm_w(handle, 0), 4);
  EXPECT_EQ(bm_h(handle, 0), 4);
  EXPECT_EQ(bm_format(handle), BITMAP_FORMAT_4444);
  // clang-format off
  EXPECT_EQ(Pixels(handle), (std::vector<uint16_t>{
    a, a, b, b,
    a, a, b, b,
    c, c, d, d,
    c, c, d, d,
  }));
  // clang-format on
}

class ChunkedBitmapTest : public BitmapLibTest, public ::testing::WithParamInterface<std::pair<int, int>> {};

TEST_P(ChunkedBitmapTest, ChunksTileTheSourceAndPadWithTransparency) {
  const int w = GetParam().first, h = GetParam().second;
  std::vector<uint16_t> pixels(w * h);
  for (int y = 0; y < h; y++)
    for (int x = 0; x < w; x++)
      pixels[y * w + x] = ChunkPattern(x, y);
  int src = MakeBitmap(w, h, pixels);

  chunked_bitmap chunk{};
  ASSERT_TRUE(bm_CreateChunkedBitmap(src, &chunk));
  ASSERT_NE(chunk.bm_array, nullptr);
  EXPECT_EQ(chunk.pw, w);
  EXPECT_EQ(chunk.ph, h);
  ASSERT_GT(chunk.w, 0);
  ASSERT_GT(chunk.h, 0);

  const int size = bm_w(chunk.bm_array[0], 0);
  ASSERT_GT(size, 0);
  EXPECT_GE(chunk.w * size, w);
  EXPECT_LT((chunk.w - 1) * size, w);
  EXPECT_GE(chunk.h * size, h);
  EXPECT_LT((chunk.h - 1) * size, h);

  int mismatches = 0;
  std::string first_mismatch;
  for (int cy = 0; cy < chunk.h; cy++) {
    for (int cx = 0; cx < chunk.w; cx++) {
      int piece = chunk.bm_array[cy * chunk.w + cx];
      ASSERT_EQ(bm_w(piece, 0), size);
      ASSERT_EQ(bm_h(piece, 0), size);
      const uint16_t *data = bm_data(piece, 0);
      for (int ly = 0; ly < size; ly++) {
        for (int lx = 0; lx < size; lx++) {
          const int gx = cx * size + lx, gy = cy * size + ly;
          const uint16_t got = data[ly * size + lx];
          const bool inside = gx < w && gy < h;
          const bool ok = inside ? got == ChunkPattern(gx, gy) : IsTransparent1555(got);
          if (!ok && mismatches++ == 0)
            first_mismatch = "at source (" + std::to_string(gx) + "," + std::to_string(gy) + ")";
        }
      }
    }
  }
  EXPECT_EQ(mismatches, 0) << "first mismatch " << first_mismatch;

  std::vector<int> pieces(chunk.bm_array, chunk.bm_array + chunk.w * chunk.h);
  bm_DestroyChunkedBitmap(&chunk);
  EXPECT_EQ(chunk.bm_array, nullptr);
  for (int piece : pieces)
    EXPECT_EQ(GameBitmaps[piece].used, 0) << "chunk bitmap " << piece << " not freed";
}

INSTANTIATE_TEST_SUITE_P(Sizes, ChunkedBitmapTest,
                         ::testing::Values(std::make_pair(20, 10), std::make_pair(64, 64), std::make_pair(100, 40),
                                           std::make_pair(300, 200)),
                         [](const auto &info) {
                           return std::to_string(info.param.first) + "x" + std::to_string(info.param.second);
                         });

TEST_F(BitmapLibTest, TransparencyIsTheMissingOpaqueBitIn1555) {
  int handle = MakeBitmap(3, 1, {Rgb1555(1, 2, 3), static_cast<uint16_t>(0x7FFF), Rgb1555(0, 0, 0)});
  EXPECT_FALSE(bm_pixel_transparent(handle, 0, 0));
  EXPECT_TRUE(bm_pixel_transparent(handle, 1, 0));
  EXPECT_FALSE(bm_pixel_transparent(handle, 2, 0));

  EXPECT_EQ(bm_SetBitmapIfTransparent(handle), 1);
  EXPECT_TRUE(GameBitmaps[handle].flags & BF_TRANSPARENT);
}

TEST_F(BitmapLibTest, TransparencyIsZeroAlphaIn4444) {
  int handle = MakeBitmap(2, 2, {Argb4444(1, 0, 0, 0), Argb4444(0, 15, 15, 15), Argb4444(15, 0, 0, 0),
                                 Argb4444(8, 1, 1, 1)},
                          BITMAP_FORMAT_4444);
  EXPECT_FALSE(bm_pixel_transparent(handle, 0, 0));
  EXPECT_TRUE(bm_pixel_transparent(handle, 1, 0));
  EXPECT_FALSE(bm_pixel_transparent(handle, 0, 1));
  EXPECT_FALSE(bm_pixel_transparent(handle, 1, 1));
  EXPECT_EQ(bm_SetBitmapIfTransparent(handle), 1);
  EXPECT_TRUE(GameBitmaps[handle].flags & BF_TRANSPARENT);
}

TEST_F(BitmapLibTest, FullyOpaqueBitmapIsNotFlaggedTransparent) {
  int opaque1555 = MakeBitmap(2, 1, {Rgb1555(0, 0, 0), Rgb1555(31, 31, 31)});
  EXPECT_EQ(bm_SetBitmapIfTransparent(opaque1555), 0);
  EXPECT_FALSE(GameBitmaps[opaque1555].flags & BF_TRANSPARENT);

  int opaque4444 = MakeBitmap(2, 1, {Argb4444(1, 0, 0, 0), Argb4444(15, 0, 0, 0)}, BITMAP_FORMAT_4444);
  EXPECT_EQ(bm_SetBitmapIfTransparent(opaque4444), 0);
  EXPECT_FALSE(GameBitmaps[opaque4444].flags & BF_TRANSPARENT);
}

TEST_F(BitmapLibTest, ClearBitmapMakesEveryPixelTransparent) {
  const int w = 5, h = 3;
  int handle = MakeBitmap(w, h, std::vector<uint16_t>(w * h, Rgb1555(31, 31, 31)));
  bm_ClearBitmap(handle);
  for (int y = 0; y < h; y++)
    for (int x = 0; x < w; x++)
      EXPECT_TRUE(bm_pixel_transparent(handle, x, y)) << "at (" << x << "," << y << ")";
}

class PageableBitmapTest : public BitmapNameTest {
protected:
  void SetUp() override {
    BitmapNameTest::SetUp();
    // Paging re-opens the file by its bare bitmap name, so it has to be on
    // the cfile search path.
    ASSERT_TRUE(cf_SetSearchPath(temp_dir.path()));
  }
};

TEST_F(PageableBitmapTest, TexelDataIsLoadedOnFirstAccess) {
  const int w = 8, h = 4;
  std::vector<uint16_t> pixels(w * h);
  for (int i = 0; i < w * h; i++)
    pixels[i] = Rgb1555(i % 32, 3, 31 - i % 32);
  int src = MakeBitmap(w, h, pixels);
  strcpy(GameBitmaps[src].name, "pagein_a.ogf");
  std::filesystem::path path = temp_dir / "pagein_a.ogf";
  ASSERT_EQ(bm_SaveFileBitmap(path, src), 1);
  bm_FreeBitmap(src);

  int handle = Track(bm_AllocLoadFileNoMemBitmap(path.string().c_str(), 0));
  ASSERT_GT(handle, 0);
  EXPECT_TRUE(GameBitmaps[handle].flags & BF_NOT_RESIDENT);

  EXPECT_EQ(bm_w(handle, 0), w);
  EXPECT_EQ(bm_h(handle, 0), h);
  EXPECT_FALSE(GameBitmaps[handle].flags & BF_NOT_RESIDENT);
  EXPECT_EQ(Pixels(handle), pixels);
}

TEST_F(PageableBitmapTest, MipmapsAreGeneratedOnPageInWhenRequested) {
  const uint16_t colour = Rgb1555(12, 20, 28);
  int src = MakeBitmap(8, 8, std::vector<uint16_t>(64, colour));
  strcpy(GameBitmaps[src].name, "pagein_b.ogf");
  std::filesystem::path path = temp_dir / "pagein_b.ogf";
  ASSERT_EQ(bm_SaveFileBitmap(path, src), 1);
  bm_FreeBitmap(src);

  int handle = Track(bm_AllocLoadFileNoMemBitmap(path.string().c_str(), 1));
  ASSERT_GT(handle, 0);
  ASSERT_EQ(bm_mipped(handle), 1);
  ASSERT_EQ(bm_w(handle, 1), 4);
  ASSERT_EQ(bm_h(handle, 1), 4);
  EXPECT_EQ(Pixels(handle, 1), std::vector<uint16_t>(16, colour));
}

TEST_F(PageableBitmapTest, FreeingAPagedInBitmapRemovesItsName) {
  std::filesystem::path path = WriteOgfStoringName(temp_dir, "pagein_f.ogf", "stock_f.ogf");

  int handle = Track(bm_AllocLoadFileNoMemBitmap(path.string().c_str(), 0));
  ASSERT_GT(handle, 0);
  ASSERT_EQ(bm_w(handle, 0), 2);
  bm_FreeBitmap(handle);

  EXPECT_EQ(bm_FindBitmapName("pagein_f.ogf"), -1);
  EXPECT_EQ(bm_FindBitmapName("stock_f.ogf"), -1);
}

} // namespace
