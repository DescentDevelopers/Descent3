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

#include <array>
#include <cctype>
#include <string>

#include <gtest/gtest.h>

#include "grdefs.h"
#include "grtext.h"
#include "support/grtext_test_support.h"
#include "support/test_fonts.h"

namespace {

class GrtextMeasureTest : public GrtextTest {
protected:
  void SetUp() override {
    GrtextTest::SetUp();
    font_ = LoadFont(MakeMetricsFont());
    grtext_SetFont(font_);
  }
  int font_ = -1;
};

} // namespace

// Width model: glyph widths, separated by a one pixel gap (plus the font's tracking), plus kerning.
// No gap is counted after the last glyph.

TEST_F(GrtextMeasureTest, EmptyStringHasNoWidth) { EXPECT_EQ(grtext_GetTextLineWidth(""), 0); }

TEST_F(GrtextMeasureTest, SingleGlyphIsExactlyItsOwnWidth) {
  EXPECT_EQ(grtext_GetTextLineWidth("A"), 5);
  EXPECT_EQ(grtext_GetTextLineWidth("B"), 6);
}

TEST_F(GrtextMeasureTest, GlyphsAreSeparatedByOnePixelPlusTracking) {
  EXPECT_EQ(grtext_GetTextLineWidth("AB"), 5 + 1 + 6);
  EXPECT_EQ(grtext_GetTextLineWidth("A B"), 5 + 1 + 3 + 1 + 6);

  TestFont tracked = MakeMetricsFont();
  tracked.ffi2 = true;
  tracked.tracking = 2;
  grtext_SetFont(LoadFont(tracked));
  EXPECT_EQ(grtext_GetTextLineWidth("AB"), 5 + 3 + 6);
  EXPECT_EQ(grtext_GetTextLineWidth("BBB"), 6 + 3 + 6 + 3 + 6);
}

TEST_F(GrtextMeasureTest, KerningAdjustsOnlyTheListedOrderedPair) {
  EXPECT_EQ(grtext_GetTextLineWidth("AV"), 5 + 1 + 7 - 2);
  EXPECT_EQ(grtext_GetTextLineWidth("VA"), 7 + 1 + 5);
  EXPECT_EQ(grtext_GetTextLineWidth("AVA"), 5 + 1 + 7 - 2 + 1 + 5);
}

TEST_F(GrtextMeasureTest, LowercaseIsMeasuredWithUppercaseGlyphsOnUppercaseFont) {
  EXPECT_EQ(grtext_GetTextLineWidth("ab va"), grtext_GetTextLineWidth("AB VA"));
}

TEST_F(GrtextMeasureTest, MultilineWidthIsTheWidestLine) {
  int widest = grtext_GetTextLineWidth("BBB");
  EXPECT_EQ(grtext_GetTextLineWidth("A\nBBB\nAB"), widest);
  EXPECT_EQ(grtext_GetTextLineWidth("BBB\nA"), widest);
  EXPECT_EQ(grtext_GetTextLineWidth("A\nBBB"), widest);
}

// Tab stops are every `tabspace` space advances (space width + gap). The fixture uses 4 spaces per tab,
// so with a 3 pixel space the stops are at 16, 32, ...
TEST_F(GrtextMeasureTest, TabAdvancesToTheNextTabStop) {
  const int stop = 4 * (3 + 1);
  EXPECT_EQ(grtext_GetTextLineWidth("\tB"), stop + 6);
  EXPECT_EQ(grtext_GetTextLineWidth("A\tB"), stop + 6);
  // "AAA" is 17 pixels wide, already past the first stop.
  EXPECT_EQ(grtext_GetTextLineWidth("AAA\tB"), 2 * stop + 6);

  grtext_SetParameters(kLeft, kTop, kRight, kBottom, 2);
  EXPECT_EQ(grtext_GetTextLineWidth("A\tB"), 2 * (3 + 1) + 6);
}

TEST_F(GrtextMeasureTest, TextHeightGrowsByOneLineHeightPerLine) {
  int one = grtext_GetTextHeight("A");
  EXPECT_GE(one, 8);
  EXPECT_EQ(grtext_GetTextHeight("A\nB"), 2 * one);
  EXPECT_EQ(grtext_GetTextHeight("A\nB\nAB"), 3 * one);
}

TEST_F(GrtextMeasureTest, FontScaleScalesGlyphSize) {
  int height = grtext_GetTextHeight("A");
  grtext_SetFontScale(2.0f);
  EXPECT_EQ(grtext_GetTextLineWidth("A"), 2 * 5);
  EXPECT_GE(grtext_GetTextHeight("A"), 2 * 8);
  EXPECT_GT(grtext_GetTextHeight("A"), height);
}

TEST_F(GrtextMeasureTest, TemplateMeasurementAgreesWithLoadedFont) {
  TestFont spec = MakeMetricsFont();
  std::filesystem::path path = WriteFont(spec);
  tFontTemplate ft{};
  ASSERT_TRUE(grfont_LoadTemplate(path.u8string().c_str(), &ft));

  for (const char *s : {"A", "AB", "AV", "VA", "ab va", "A B\tV", "A\nBBB\nAB", "HELLO, WORLD"}) {
    SCOPED_TRACE(s);
    EXPECT_EQ(grtext_GetTextLineWidthTemplate(&ft, s), grtext_GetTextLineWidth(s));
    EXPECT_EQ(grtext_GetTextHeightTemplate(&ft, s), grtext_GetTextHeight(s));
  }
  grfont_FreeTemplate(&ft);
}

namespace {

class GrtextRenderTest : public GrtextTest {
protected:
  void SetUp() override {
    GrtextTest::SetUp();
    grtext_SetFont(LoadFont(MakeDecodableFont()));
  }
  void TearDown() override {
    grtext_SetProfanityFilter(false);
    GrtextTest::TearDown();
  }
};

} // namespace

TEST_F(GrtextRenderTest, TextIsDrawnUnchangedWithFilterOff) {
  EXPECT_EQ(Render("Hello there"), "Hellothere");
  EXPECT_EQ(Render("oh shit"), "ohshit");
}

TEST_F(GrtextRenderTest, ProfanityFilterLeavesCleanTextAlone) {
  grtext_SetProfanityFilter(true);
  EXPECT_EQ(Render("Hello there, pilot!"), "Hellothere,pilot!");
}

TEST_F(GrtextRenderTest, ProfanityFilterMasksWordsRegardlessOfCase) {
  grtext_SetProfanityFilter(true);
  std::string drawn = Render("what the FuCk");
  ASSERT_EQ(drawn.size(), 11u);
  EXPECT_EQ(drawn.substr(0, 7), "whatthe");
  EXPECT_TRUE(NoLetters(drawn.substr(7))) << drawn;
}

TEST_F(GrtextRenderTest, ProfanityFilterMasksEveryOccurrence) {
  grtext_SetProfanityFilter(true);
  std::string drawn = Render("shit happens, shit");
  ASSERT_EQ(drawn.size(), 16u);
  EXPECT_TRUE(NoLetters(drawn.substr(0, 4))) << drawn;
  EXPECT_EQ(drawn.substr(4, 8), "happens,");
  EXPECT_TRUE(NoLetters(drawn.substr(12))) << drawn;
}

namespace {

class GrtextClipTest : public GrtextTest {
protected:
  void SetUp() override {
    GrtextTest::SetUp();
    TestFont f; // fixed width 4x8
    grtext_SetFont(LoadFont(f));
    grtext_SetParameters(kClipLeft, kClipTop, kClipRight, kClipBottom);
  }
  static constexpr int kClipLeft = 10, kClipTop = 10, kClipRight = 30, kClipBottom = 30;
};

} // namespace

TEST_F(GrtextClipTest, NothingIsDrawnOutsideTheTextWindow) {
  grtext_Puts(5, 6, "ABCDEFGH");    // crosses the left, top and right edges
  grtext_Puts(12, 26, "AB");        // crosses the bottom edge
  grtext_Puts(-3, 12, "ABCDEFGHIJ"); // wider than the window
  grtext_Flush();

  ASSERT_FALSE(DrawnGlyphs().empty());
  for (const DrawnGlyph &d : DrawnGlyphs()) {
    EXPECT_GE(d.x1, kClipLeft);
    EXPECT_LE(d.x2, kClipRight);
    EXPECT_GE(d.y1, kClipTop);
    EXPECT_LE(d.y2, kClipBottom);
  }
}

TEST_F(GrtextClipTest, TextEntirelyOutsideTheWindowIsNotDrawn) {
  grtext_Puts(100, 15, "AB");
  grtext_Puts(-50, 15, "AB");
  grtext_Puts(15, -40, "AB");
  grtext_Puts(15, 200, "AB");
  grtext_Flush();
  EXPECT_TRUE(DrawnGlyphs().empty());
}

TEST_F(GrtextClipTest, TextInsideTheWindowIsDrawnWhole) {
  grtext_Puts(12, 12, "AB");
  grtext_Flush();
  ASSERT_EQ(DrawnGlyphs().size(), 2u);
  for (const DrawnGlyph &d : DrawnGlyphs()) {
    EXPECT_EQ(d.x2 - d.x1, 4);
    EXPECT_EQ(d.y2 - d.y1, 8);
  }
  EXPECT_LT(DrawnGlyphs()[0].x1, DrawnGlyphs()[1].x1);
}
