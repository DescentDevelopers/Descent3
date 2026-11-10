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

#include <cmath>
#include <cstdlib>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

#include "cfile.h"
#include "grdefs.h"
#include "grtext.h"
#include "d3_test/byte_writer.h"
#include "d3_test/files.h"
#include "support/grtext_test_support.h"
#include "support/test_fonts.h"

using GrfontTest = GrtextTest;

TEST_F(GrfontTest, ProportionalWidthsAndHeightComeFromTheFile) {
  TestFont spec = MakeProportionalFont();
  int font = LoadFont(spec);

  EXPECT_EQ(grfont_GetHeight(font), 9);
  EXPECT_EQ(grfont_GetCharWidth(font, ' '), 3);
  EXPECT_EQ(grfont_GetCharWidth(font, 'A'), 5);
  EXPECT_EQ(grfont_GetCharWidth(font, 'B'), 6);
  EXPECT_EQ(grfont_GetCharWidth(font, 'C'), 4);
  EXPECT_EQ(grfont_GetCharWidth(font, 'V'), 7);
}

TEST_F(GrfontTest, FixedWidthFontUsesTheSameWidthForEveryGlyph) {
  TestFont spec;
  spec.width = 6;
  int font = LoadFont(spec);

  EXPECT_EQ(grfont_GetCharWidth(font, ' '), 6);
  EXPECT_EQ(grfont_GetCharWidth(font, 'A'), 6);
  EXPECT_EQ(grfont_GetCharWidth(font, 'Z'), 6);
}

TEST_F(GrfontTest, LowercaseUsesUppercaseGlyphsOnlyWhenFontHasNoLowercase) {
  int upper_only = LoadFont(MakeProportionalFont());
  EXPECT_EQ(grfont_GetCharWidth(upper_only, 'a'), grfont_GetCharWidth(upper_only, 'A'));
  EXPECT_EQ(grfont_GetCharWidth(upper_only, 'v'), grfont_GetCharWidth(upper_only, 'V'));

  TestFont with_lower;
  with_lower.max_ascii = 'z';
  with_lower.widths.assign(with_lower.NumGlyphs(), 4);
  with_lower.widths['a' - with_lower.min_ascii] = 2;
  int mixed = LoadFont(with_lower);
  EXPECT_EQ(grfont_GetCharWidth(mixed, 'a'), 2);
  EXPECT_EQ(grfont_GetCharWidth(mixed, 'A'), 4);
}

TEST_F(GrfontTest, CharactersMissingFromTheFontHaveNoWidth) {
  int font = LoadFont(MakeProportionalFont());

  EXPECT_EQ(grfont_GetCharWidth(font, '\t'), 0);        // below min_ascii
  EXPECT_EQ(grfont_GetCharWidth(font, '{'), 0);         // above max_ascii, not a lowercase letter
  EXPECT_EQ(grfont_GetCharWidth(font, (char)0xE9), 0); // high-bit char passed as a (signed) char
}

TEST_F(GrfontTest, KerningAppliesToOrderedPairsWithSignedAdjustment) {
  TestFont spec = MakeProportionalFont();
  spec.kerning = {{'A', 'V', -2}, {'L', 'T', 3}};
  int font = LoadFont(spec);

  EXPECT_EQ(grfont_GetKernedSpacing(font, 'A', 'V'), -2);
  EXPECT_EQ(grfont_GetKernedSpacing(font, 'L', 'T'), 3);
  EXPECT_EQ(grfont_GetKernedSpacing(font, 'V', 'A'), 0);
  EXPECT_EQ(grfont_GetKernedSpacing(font, 'A', 'A'), 0);

  int unkerned = LoadFont(MakeProportionalFont());
  EXPECT_EQ(grfont_GetKernedSpacing(unkerned, 'A', 'V'), 0);
}

TEST_F(GrfontTest, TrackingIsReadFromNewStyleFontsAndZeroForOldOnes) {
  TestFont new_style = MakeProportionalFont();
  new_style.ffi2 = true;
  new_style.tracking = 3;
  int tracked = LoadFont(new_style);
  int old_style = LoadFont(MakeProportionalFont());

  EXPECT_EQ(grfont_GetTracking(tracked), 3);
  EXPECT_EQ(grfont_GetTracking(old_style), 0);
  // The extra header block must not throw off the rest of the file.
  EXPECT_EQ(grfont_GetCharWidth(tracked, 'V'), 7);
}

TEST_F(GrfontTest, FontsLoadedTogetherKeepSeparateMetrics) {
  TestFont narrow;
  narrow.width = 3;
  narrow.height = 5;
  TestFont wide;
  wide.width = 11;
  wide.height = 13;

  int a = LoadFont(narrow);
  int b = LoadFont(wide);
  ASSERT_NE(a, b);
  EXPECT_EQ(grfont_GetCharWidth(a, 'M'), 3);
  EXPECT_EQ(grfont_GetHeight(a), 5);
  EXPECT_EQ(grfont_GetCharWidth(b, 'M'), 11);
  EXPECT_EQ(grfont_GetHeight(b), 13);
}

// Glyph widths cross byte boundaries in the 1-bpp source data, and the glyphs do not all fit on one 128x128 font
// surface.
TEST_F(GrfontTest, MonochromeGlyphPixelsReachTheFontSurface) {
  TestFont spec;
  spec.min_ascii = 'A';
  spec.max_ascii = 'C';
  spec.height = 70;
  spec.widths = {9, 60, 70};
  spec.width = 70;
  auto bit_set = [](int glyph, int row, int col) { return (row * 7 + col * 3 + glyph) % 5 == 0; };
  for (int g = 0; g < spec.NumGlyphs(); g++) {
    int w = spec.widths[g];
    int bytes_per_row = (w + 7) / 8;
    std::vector<uint8_t> data(bytes_per_row * spec.height, 0);
    for (int r = 0; r < spec.height; r++)
      for (int c = 0; c < w; c++)
        if (bit_set(g, r, c))
          data[r * bytes_per_row + c / 8] |= 0x80 >> (c % 8);
    spec.glyph_data.push_back(data);
  }
  int font = LoadFont(spec);
  grtext_SetFont(font);

  for (int g = 0; g < spec.NumGlyphs(); g++) {
    SCOPED_TRACE(testing::Message() << "glyph " << char('A' + g));
    d3_test::ResetRenderer();
    char str[2] = {char('A' + g), 0};
    grtext_Puts(0, 0, str);
    grtext_Flush();
    ASSERT_EQ(DrawnGlyphs().size(), 1u);
    const DrawnGlyph &d = DrawnGlyphs()[0];
    int u0 = (int)std::lround(d.u * 128), v0 = (int)std::lround(d.v * 128);
    int mismatches = 0;
    for (int r = 0; r < spec.height; r++) {
      for (int c = 0; c < spec.widths[g]; c++) {
        uint16_t px = BitmapPixel(d.bm_handle, u0 + c, v0 + r);
        bool opaque = (px & OPAQUE_FLAG) != 0;
        if (opaque != bit_set(g, r, c) || (!opaque && px != NEW_TRANSPARENT_COLOR))
          mismatches++;
      }
    }
    EXPECT_EQ(mismatches, 0);
  }
}

// Old-style color fonts store 565 pixels with pure green (0x07e0) as the transparent key; the font surfaces are
// 1555.
TEST_F(GrfontTest, ColorGlyphPixelsAreConvertedFrom565) {
  TestFont spec;
  spec.min_ascii = 'A';
  spec.max_ascii = 'A';
  spec.color = true;
  spec.width = 5;
  spec.height = 1;
  const uint16_t src[5] = {0xF800 /*red*/, 0x07C0 /*green*/, 0x001F /*blue*/, 0xFFFF /*white*/, 0x07E0 /*key*/};
  d3_test::ByteWriter data;
  for (uint16_t p : src)
    data.U16(p);
  spec.glyph_data.push_back(data);
  int font = LoadFont(spec);
  grtext_SetFont(font);

  grtext_Puts(0, 0, "A");
  grtext_Flush();
  ASSERT_EQ(DrawnGlyphs().size(), 1u);
  const DrawnGlyph &d = DrawnGlyphs()[0];
  int u0 = (int)std::lround(d.u * 128), v0 = (int)std::lround(d.v * 128);

  EXPECT_EQ(BitmapPixel(d.bm_handle, u0 + 0, v0), OPAQUE_FLAG | 0x7C00);
  EXPECT_EQ(BitmapPixel(d.bm_handle, u0 + 1, v0), OPAQUE_FLAG | 0x03E0);
  EXPECT_EQ(BitmapPixel(d.bm_handle, u0 + 2, v0), OPAQUE_FLAG | 0x001F);
  EXPECT_EQ(BitmapPixel(d.bm_handle, u0 + 3, v0), OPAQUE_FLAG | 0x7FFF);
  EXPECT_EQ(BitmapPixel(d.bm_handle, u0 + 4, v0), NEW_TRANSPARENT_COLOR);
}

TEST_F(GrfontTest, LoadTemplateReadsFontMetrics) {
  TestFont spec = MakeProportionalFont();
  spec.kerning = {{'A', 'V', -2}};
  spec.ffi2 = true;
  spec.tracking = -1;
  std::filesystem::path path = WriteFont(spec);

  tFontTemplate ft{};
  ASSERT_TRUE(grfont_LoadTemplate(path.u8string().c_str(), &ft));
  EXPECT_EQ(ft.min_ascii, ' ');
  EXPECT_EQ(ft.max_ascii, 'Z');
  EXPECT_EQ(ft.ch_height, 9);
  EXPECT_EQ(ft.ch_maxwidth, 7);
  EXPECT_TRUE(ft.proportional);
  EXPECT_TRUE(ft.uppercase);
  EXPECT_TRUE(ft.monochromatic);
  EXPECT_TRUE(ft.ffi2);
  EXPECT_EQ(ft.ch_tracking, -1);
  ASSERT_NE(ft.ch_widths, nullptr);
  EXPECT_EQ(ft.ch_widths['A' - ' '], 5);
  EXPECT_EQ(ft.ch_widths['V' - ' '], 7);
  EXPECT_EQ(grfont_GetKernedSpacingTemp(&ft, 'A', 'V'), -2);
  EXPECT_EQ(grfont_GetKernedSpacingTemp(&ft, 'V', 'A'), 0);
  grfont_FreeTemplate(&ft);
  EXPECT_EQ(ft.ch_widths, nullptr);
  EXPECT_EQ(ft.kern_data, nullptr);

  TestFont fixed_color;
  fixed_color.max_ascii = 'z';
  fixed_color.color = true;
  std::filesystem::path path2 = WriteFont(fixed_color);
  tFontTemplate ft2{};
  ASSERT_TRUE(grfont_LoadTemplate(path2.u8string().c_str(), &ft2));
  EXPECT_FALSE(ft2.proportional);
  EXPECT_FALSE(ft2.uppercase);
  EXPECT_FALSE(ft2.monochromatic);
  EXPECT_FALSE(ft2.ffi2);
  EXPECT_EQ(ft2.ch_tracking, 0);
  EXPECT_EQ(ft2.ch_widths, nullptr);
  EXPECT_EQ(ft2.kern_data, nullptr);
  grfont_FreeTemplate(&ft2);
}

TEST_F(GrfontTest, SetTemplateRoundTripsThroughLoadTemplate) {
  TestFont spec = MakeProportionalFont();
  spec.kerning = {{'A', 'V', -2}};
  std::filesystem::path path = WriteFont(spec);
  std::string name = path.u8string();

  tFontTemplate ft{};
  ASSERT_TRUE(grfont_LoadTemplate(name.c_str(), &ft));
  ft.ch_widths['A' - ' '] = 2;
  ft.ch_widths['W' - ' '] = 8;
  uint8_t kerning[] = {'A', 'W', (uint8_t)-1, 'T', 'O', 2, 255, 255, 0};
  uint8_t *loaded_kerning = ft.kern_data;
  ft.kern_data = kerning;
  ft.ffi2 = true;
  ft.ch_tracking = 2;
  EXPECT_TRUE(grfont_SetTemplate(name.c_str(), &ft));
  ft.kern_data = loaded_kerning;
  grfont_FreeTemplate(&ft);

  tFontTemplate back{};
  ASSERT_TRUE(grfont_LoadTemplate(name.c_str(), &back));
  EXPECT_EQ(back.min_ascii, ' ');
  EXPECT_EQ(back.max_ascii, 'Z');
  EXPECT_EQ(back.ch_height, 9);
  EXPECT_TRUE(back.proportional);
  EXPECT_TRUE(back.ffi2);
  EXPECT_EQ(back.ch_tracking, 2);
  ASSERT_NE(back.ch_widths, nullptr);
  EXPECT_EQ(back.ch_widths['A' - ' '], 2);
  EXPECT_EQ(back.ch_widths['B' - ' '], 6);
  EXPECT_EQ(back.ch_widths['W' - ' '], 8);
  EXPECT_EQ(grfont_GetKernedSpacingTemp(&back, 'A', 'W'), -1);
  EXPECT_EQ(grfont_GetKernedSpacingTemp(&back, 'T', 'O'), 2);
  EXPECT_EQ(grfont_GetKernedSpacingTemp(&back, 'A', 'V'), 0);
  grfont_FreeTemplate(&back);
}
