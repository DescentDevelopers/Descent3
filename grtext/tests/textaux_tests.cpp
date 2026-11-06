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
#include <sstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "grtext.h"
#include "support/grtext_test_support.h"
#include "support/textaux_test_support.h"
#include "textaux.h"

namespace {

// Fixed-width font: every glyph is 4 pixels wide and glyphs are 1 pixel apart, so a run of n characters
// advances 5n pixels and is 5n - 1 pixels wide.
class TextauxTest : public GrtextTest {
protected:
  void SetUp() override {
    GrtextTest::SetUp();
    TestFont f;
    f.max_ascii = 'z';
    f.width = 4;
    font_ = LoadFont(f);
    grtext_SetFont(font_);
  }

  int font_ = -1;
};

} // namespace

TEST_F(TextauxTest, WordWrapBreaksAtSpacesGreedily) {
  // 52 pixels holds at most 10 characters (10 * 5 = 50 advance).
  EXPECT_EQ(Wrap("the quick brown fox jumps over the lazy dog", 52, font_),
            "the quick\nbrown fox\njumps over\nthe lazy\ndog");
}

TEST_F(TextauxTest, WordWrapLinesFitTheRequestedWidth) {
  const char *text = "Pilot, the reactor core has gone critical. Proceed to the exit before the mine collapses.";
  for (int width : {52, 77, 123, 200}) {
    SCOPED_TRACE(width);
    std::string wrapped = Wrap(text, width, font_);
    for (const std::string &line : Lines(wrapped))
      EXPECT_LE(grtext_GetTextLineWidth(line.c_str()), width) << '"' << line << '"';
    std::string unwrapped = wrapped;
    for (char &c : unwrapped)
      if (c == '\n')
        c = ' ';
    EXPECT_EQ(unwrapped, text);
  }
}

TEST_F(TextauxTest, WordWrapKeepsExistingLineBreaks) {
  EXPECT_EQ(Wrap("one\ntwo three", 52, font_), "one\ntwo three");
  EXPECT_EQ(Wrap("first line is long\nshort", 52, font_), "first line\nis long\nshort");
}

TEST_F(TextauxTest, WordWrapLeavesTextThatFitsAlone) {
  EXPECT_EQ(Wrap("fits fine", 200, font_), "fits fine");
  EXPECT_EQ(Wrap("x", 1, font_), "x");
}

TEST_F(TextauxTest, WordWrapPutsOverlongWordOnItsOwnLineUnbroken) {
  EXPECT_EQ(Wrap("a supercalifragilistic b", 52, font_), "a\nsupercalifragilistic\nb");
  EXPECT_EQ(Wrap("supercalifragilistic", 52, font_), "supercalifragilistic");
}

TEST_F(TextauxTest, WordWrapOfEmptyTextIsEmpty) {
  char dest[16] = "stale contents";
  textaux_WordWrap("", dest, 100, font_);
  EXPECT_STREQ(dest, "");
}

TEST_F(TextauxTest, CopyTextLineWalksLinesIncludingEmptyOnes) {
  const char *text = "alpha\nbeta\n\ngamma";
  std::vector<std::string> lines;
  char line[32];
  const char *next = text;
  do {
    next = textaux_CopyTextLine(next, line);
    lines.push_back(line);
  } while (next && lines.size() < 10);

  EXPECT_EQ(lines, (std::vector<std::string>{"alpha", "beta", "", "gamma"}));
}

TEST_F(TextauxTest, CopyTextLineOfNullSourceGivesEmptyLine) {
  char line[8] = "junk";
  EXPECT_EQ(textaux_CopyTextLine(nullptr, line), nullptr);
  EXPECT_STREQ(line, "");
}

TEST_F(TextauxTest, ClipStringLeavesStringThatFitsUnchanged) {
  char s[] = "ABC"; // 14 pixels
  textaux_ClipString(40, s);
  EXPECT_STREQ(s, "ABC");

  char t[] = "ABC";
  textaux_ClipString(40, t, 1.0f, '>');
  EXPECT_STREQ(t, "ABC");
}

TEST_F(TextauxTest, ClipStringLeavesStringThatExactlyFitsUnchanged) {
  char s[] = "ABCD";
  const int width = grtext_GetTextLineWidth(s);
  ASSERT_EQ(width, 19);
  textaux_ClipString(width, s);
  EXPECT_STREQ(s, "ABCD");

  char t[] = "ABCD";
  textaux_ClipString(width, t, 1.0f, '>');
  EXPECT_STREQ(t, "ABCD");
}

TEST_F(TextauxTest, ClipStringWithEndCharNeverExceedsTheWidth) {
  for (int width = 4; width < 44; width++) {
    char s[] = "ABCDEFGH";
    textaux_ClipString(width, s, 1.0f, '>');
    EXPECT_LE(grtext_GetTextLineWidth(s), width) << "width " << width << ": \"" << s << '"';
  }

  char exact[] = "ABCDEFGH";
  textaux_ClipString(29, exact, 1.0f, '>');
  EXPECT_STREQ(exact, "ABCDE>");
}

TEST_F(TextauxTest, ClipStringWithEndCharKeepsLongestPrefixThatFitsWithMarker) {
  char s[] = "ABCDEFGH";
  textaux_ClipString(32, s, 1.0f, '>');
  EXPECT_STREQ(s, "ABCDE>"); // 6 glyphs = 29 pixels; "ABCDEF>" would be 34
  EXPECT_LE(grtext_GetTextLineWidth(s), 32);
}

TEST_F(TextauxTest, ClipStringWithoutEndCharFitsWithinWidth) {
  char s[] = "ABCDEFGH";
  textaux_ClipString(22, s); // "ABCD" is 19 pixels, "ABCDE" is 24
  EXPECT_LE(grtext_GetTextLineWidth(s), 22) << '"' << s << '"';
  EXPECT_STREQ(s, "ABCD");
}

TEST_F(TextauxTest, ClipStringNeverWritesBeforeTheString) {
  char buf[] = "#ABCDEF";
  char *s = buf + 1;
  textaux_ClipString(grtext_GetTextLineWidth(">"), s, 1.0f, '>');
  EXPECT_EQ(buf[0], '#');
  EXPECT_LE(grtext_GetTextLineWidth(s), grtext_GetTextLineWidth(">"));
}
