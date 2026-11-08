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

#include <cstdio>
#include <cstdlib>
#include <optional>

#include <gtest/gtest.h>

#include "cfile.h"
#include "d3_test/files.h"
#include "d3_test/game.h"
#include "gr.h"
#include "support/surface.h"
#include "support/test_font.h"
#include "support/text_draw.h"

using namespace twodlib_test;

namespace {

class GrViewportTextTest : public ::testing::Test {
protected:
  static void SetUpTestSuite() {
    // grFont keeps its glyphs in bitmaps.
    d3_test::EnsureBitmaps();
    // The font registry is process-global and has no unregister, so set it up once.
    static bool registered = false;
    if (registered)
      return;

    s_dir.emplace();
    s_dir->Write(kFontFile, BuildFontFile());
    ASSERT_TRUE(cf_SetSearchPath(s_dir->path()));

    grFont::init_system();
    char name[32], file[32];
    std::snprintf(name, sizeof(name), "%s", kFontName);
    std::snprintf(file, sizeof(file), "%s", kFontFile);
    ASSERT_NE(grFont::register_font(name, file), -1);
    registered = true;
  }

  static void TearDownTestSuite() {
    // The font data is fully loaded into memory by register_font; the file is not needed.
    s_dir.reset();
  }

  static constexpr int kW = 32, kH = 16;

  OwnedSurface screen{kW, kH, kW, 0, 0, 0};

  static std::optional<d3_test::TempDir> s_dir;
};

std::optional<d3_test::TempDir> GrViewportTextTest::s_dir;

} // namespace

TEST_F(GrViewportTextTest, LineWidthIsGlyphWidthsPlusSpacingBetweenThem) {
  grViewport vp(&screen.surf);
  vp.set_font(kFontName);

  vp.set_text_spacing(1);
  EXPECT_EQ(LineWidth(vp, "ABC"), 4 + 5 + 6 + 2 * 1);
  vp.set_text_spacing(3);
  EXPECT_EQ(LineWidth(vp, "ABC"), 4 + 5 + 6 + 2 * 3);
  EXPECT_EQ(LineWidth(vp, "A"), 4);
}

TEST_F(GrViewportTextTest, PutsDrawsGlyphsLeftToRightSeparatedBySpacing) {
  grViewport vp(&screen.surf);
  vp.set_font(kFontName);
  vp.set_text_spacing(1);

  Puts(vp, 1, 2, "AB");

  ExpectGlyph(screen, 'A', 1, 2);
  ExpectGlyph(screen, 'B', 1 + 4 + 1, 2);
  EXPECT_EQ(CountDrawnPixels(screen), (4 + 5) * kGlyphHeight) << "pixels drawn outside the two glyphs";
}

TEST_F(GrViewportTextTest, PutsStartsEachLineBelowThePreviousByHeightPlusLineSpacing) {
  grViewport vp(&screen.surf);
  vp.set_font(kFontName);
  vp.set_text_line_spacing(2);

  Puts(vp, 3, 0, "A\nB");

  ExpectGlyph(screen, 'A', 3, 0);
  ExpectGlyph(screen, 'B', 3, kGlyphHeight + 2);
  EXPECT_EQ(CountDrawnPixels(screen), (4 + 5) * kGlyphHeight);
}

TEST_F(GrViewportTextTest, PutsCenteredLeavesEqualMarginsOnBothSides) {
  grViewport vp(&screen.surf);
  vp.set_font(kFontName);
  vp.set_text_spacing(1);

  Puts(vp, 0, 0, "AB", grTextCentered);

  const int line_width = 4 + 1 + 5;
  int left = 0;
  while (left < kW && screen.at(left, 0) == 0)
    left++;
  const int right = kW - left - line_width;
  EXPECT_LE(std::abs(left - right), 1) << "left margin " << left << ", right margin " << right;
  ExpectGlyph(screen, 'A', left, 0);
  ExpectGlyph(screen, 'B', left + 5, 0);
}

TEST_F(GrViewportTextTest, PutsClipsGlyphAtLeftEdgeAndKeepsFollowingGlyphsInPlace) {
  grViewport vp(&screen.surf);
  vp.set_font(kFontName);
  vp.set_text_spacing(1);

  Puts(vp, -2, 0, "AB");

  ExpectGlyph(screen, 'A', 0, 0, /*col0=*/2); // columns 2..3 of 'A' visible
  ExpectGlyph(screen, 'B', -2 + 4 + 1, 0);
  EXPECT_EQ(CountDrawnPixels(screen), (2 + 5) * kGlyphHeight);
}

TEST_F(GrViewportTextTest, PutsClipsGlyphAtTopEdge) {
  grViewport vp(&screen.surf);
  vp.set_font(kFontName);

  Puts(vp, 0, -2, "A");

  ExpectGlyph(screen, 'A', 0, 0, 0, /*row0=*/2); // rows 2..5 visible
  EXPECT_EQ(CountDrawnPixels(screen), 4 * (kGlyphHeight - 2));
}

