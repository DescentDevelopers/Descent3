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

#include <string>

#include <gtest/gtest.h>

#include "psglob.h"

#include "support/psglob_match.h"

TEST(PSGlob, LiteralPatternMustMatchWholeText) {
  EXPECT_TRUE(Glob("level1.d3l", "level1.d3l"));
  EXPECT_FALSE(Glob("level1", "level1.d3l"));
  EXPECT_FALSE(Glob("level1.d3l", "level1"));
  EXPECT_TRUE(Glob("", ""));
  EXPECT_FALSE(Glob("", "a"));
  EXPECT_FALSE(Glob("a", ""));
}

TEST(PSGlob, StarMatchesAnySequenceIncludingEmpty) {
  EXPECT_TRUE(Glob("*", ""));
  EXPECT_TRUE(Glob("*", "anything at all"));
  EXPECT_TRUE(Glob("*.ogf", "texture.ogf"));
  EXPECT_TRUE(Glob("*.ogf", ".ogf"));
  EXPECT_TRUE(Glob("*.ogf", "a.b.c.ogf"));
  EXPECT_FALSE(Glob("*.ogf", "texture.ogg"));
  EXPECT_FALSE(Glob("*.ogf", "texture.ogf.bak"));
  EXPECT_TRUE(Glob("tex*", "tex"));
  EXPECT_TRUE(Glob("tex*", "texture.ogf"));
  EXPECT_TRUE(Glob("t*e", "texture"));
  EXPECT_TRUE(Glob("**", "abc"));
}

TEST(PSGlob, MultipleStarsBacktrack) {
  EXPECT_TRUE(Glob("a*b*c", "abc"));
  EXPECT_TRUE(Glob("a*b*c", "aXXbYYc"));
  EXPECT_FALSE(Glob("a*b*c", "aXXcYYb"));
  EXPECT_TRUE(Glob("*ab", "aab"));
  EXPECT_TRUE(Glob("*abc", "ababcabc"));
  EXPECT_FALSE(Glob("*abc", "ababcab"));
}

TEST(PSGlob, QuestionMarkMatchesExactlyOneCharacter) {
  EXPECT_TRUE(Glob("?", "x"));
  EXPECT_FALSE(Glob("?", ""));
  EXPECT_FALSE(Glob("?", "xy"));
  EXPECT_TRUE(Glob("l?vel", "level"));
  EXPECT_FALSE(Glob("l?vel", "lvel"));
  EXPECT_TRUE(Glob("???", "abc"));
  EXPECT_FALSE(Glob("???", "ab"));
}

TEST(PSGlob, QuestionMarkAfterStarRequiresAtLeastOneCharacter) {
  EXPECT_FALSE(Glob("*?", ""));
  EXPECT_TRUE(Glob("*?", "a"));
  EXPECT_TRUE(Glob("*??", "ab"));
  EXPECT_FALSE(Glob("*??", "a"));
  EXPECT_TRUE(Glob("?*", "a"));
  EXPECT_FALSE(Glob("?*", ""));
}

TEST(PSGlob, SetMatchesListedCharacters) {
  EXPECT_TRUE(Glob("[abc]", "a"));
  EXPECT_TRUE(Glob("[abc]", "c"));
  EXPECT_FALSE(Glob("[abc]", "d"));
  EXPECT_FALSE(Glob("[abc]", ""));
  EXPECT_FALSE(Glob("[abc]", "ab"));
  EXPECT_TRUE(Glob("x[abc]z", "xbz"));
  EXPECT_FALSE(Glob("x[abc]z", "xdz"));
}

TEST(PSGlob, SetMatchesRanges) {
  EXPECT_TRUE(Glob("level[0-9]", "level0"));
  EXPECT_TRUE(Glob("level[0-9]", "level5"));
  EXPECT_TRUE(Glob("level[0-9]", "level9"));
  EXPECT_FALSE(Glob("level[0-9]", "levelx"));

  for (const char *ok : {"a", "z", "A", "Z", "0", "9", "_"}) {
    EXPECT_TRUE(Glob("[0-9a-zA-Z_]", ok)) << ok;
  }
  for (const char *bad : {"-", ".", " ", "$", "["}) {
    EXPECT_FALSE(Glob("[0-9a-zA-Z_]", bad)) << bad;
  }
}

TEST(PSGlob, InvertedSetMatchesCharactersOutsideSet) {
  EXPECT_FALSE(Glob("[!abc]", "a"));
  EXPECT_TRUE(Glob("[!abc]", "d"));
  EXPECT_FALSE(Glob("[!0-9]", "5"));
  EXPECT_TRUE(Glob("[!0-9]", "x"));
  EXPECT_FALSE(Glob("[^0-9]", "5"));
  EXPECT_TRUE(Glob("[^0-9]", "x"));
  EXPECT_FALSE(Glob("[!abc]", ""));
}

TEST(PSGlob, StarFollowedBySet) {
  EXPECT_TRUE(Glob("*[0-9]", "level12"));
  EXPECT_FALSE(Glob("*[0-9]", "level"));
  EXPECT_TRUE(Glob("*[0-9].d3l", "level3.d3l"));
  EXPECT_FALSE(Glob("*[0-9].d3l", "levelx.d3l"));
}

TEST(PSGlob, BackslashEscapesSpecialCharacters) {
  EXPECT_TRUE(Glob("\\*", "*"));
  EXPECT_FALSE(Glob("\\*", "a"));
  EXPECT_FALSE(Glob("\\*", ""));
  EXPECT_TRUE(Glob("what\\?", "what?"));
  EXPECT_FALSE(Glob("what\\?", "whatX"));
  EXPECT_TRUE(Glob("\\[x\\]", "[x]"));
  EXPECT_FALSE(Glob("\\[x\\]", "x"));
  EXPECT_TRUE(Glob("a\\\\b", "a\\b"));
}

TEST(PSGlob, EscapedCharacterAfterStar) {
  EXPECT_TRUE(Glob("*\\*", "abc*"));
  EXPECT_FALSE(Glob("*\\*", "abc"));
  EXPECT_TRUE(Glob("*\\?", "really?"));
  EXPECT_FALSE(Glob("*\\?", "really!"));
}

TEST(PSGlob, BackslashEscapesInsideSet) {
  EXPECT_TRUE(Glob("[\\]]", "]"));
  EXPECT_FALSE(Glob("[\\]]", "x"));
  EXPECT_TRUE(Glob("[a\\-z]", "-"));
  EXPECT_TRUE(Glob("[a\\-z]", "a"));
  EXPECT_TRUE(Glob("[a\\-z]", "z"));
  EXPECT_FALSE(Glob("[a\\-z]", "m"));
}

TEST(PSGlob, UnterminatedSetNeverMatches) {
  EXPECT_FALSE(Glob("[abc", "a"));
  EXPECT_FALSE(Glob("[abc", "z"));
  EXPECT_FALSE(Glob("[a-", "a"));
  EXPECT_FALSE(Glob("[", "a"));
}

TEST(PSGlob, DotSpecialProtectsLeadingDot) {
  EXPECT_FALSE(Glob("*", ".hidden", true, true));
  EXPECT_FALSE(Glob("?hidden", ".hidden", true, true));
  EXPECT_TRUE(Glob(".*", ".hidden", true, true));

  EXPECT_TRUE(Glob("*", ".hidden", true, false));
  EXPECT_TRUE(Glob("?hidden", ".hidden", true, false));
}

TEST(PSGlob, DotSpecialOnlyAffectsTheFirstCharacter) {
  EXPECT_TRUE(Glob("*", "file.txt", true, true));
  EXPECT_TRUE(Glob("file?txt", "file.txt", true, true));
  EXPECT_TRUE(Glob("file*", "file.txt", true, true));
  EXPECT_TRUE(Glob("a*", "a.b", true, true));
}

TEST(PSGlob, CaseSensitivityOfLiterals) {
  EXPECT_TRUE(Glob("Level1.D3L", "level1.d3l", false));
  EXPECT_FALSE(Glob("Level1.D3L", "level1.d3l", true));
  EXPECT_TRUE(Glob("level1.d3l", "LEVEL1.D3L", false));
  EXPECT_FALSE(Glob("level1.d3l", "LEVEL1.D3L", true));
  EXPECT_FALSE(Glob("levelA", "levelB", false));
}

TEST(PSGlob, CaseInsensitiveLiteralsAfterStar) {
  EXPECT_TRUE(Glob("*OGF", "texture.ogf", false));
  EXPECT_FALSE(Glob("*OGF", "texture.ogf", true));
  EXPECT_TRUE(Glob("*robot*", "BIGROBOTARM", false));
  EXPECT_FALSE(Glob("*robot*", "BIGROBOTARM", true));
}

TEST(PSGlob, MatchAfterStarMatchesAnySuffix) {
  auto after_star = [](const char *pattern, const char *text) {
    std::string p(pattern);
    std::string t(text);
    return PSGlobMatchAfterStar(p.data(), 1, t.data()) != 0;
  };
  EXPECT_TRUE(after_star("b", "aab"));
  EXPECT_TRUE(after_star("aab", "aab"));
  EXPECT_TRUE(after_star("", "anything"));
  EXPECT_FALSE(after_star("a", "ab"));
  EXPECT_FALSE(after_star("x", ""));
}
