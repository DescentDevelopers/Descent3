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

// The file format is documented at the top of scripts/data/fullhog/D3.STR: every non-blank line starts with a
// token, "!/!" for a comment or "!=!", "!G!", "!S!", "!I!", "!F!" for the English, German, Spanish, Italian or
// French version of a string. Lines without a token continue the previous string. Strings may contain the escapes
// \t, \n and \0 - \255 (a decimal character code).

#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "d3_test/files.h"
#include "localization.h"
#include "support/string_table.h"

namespace {

class StringTableTest : public ::testing::Test {
protected:
  void SetUp() override { Localization_SetLanguage(LANGUAGE_ENGLISH); }

  StringTableLoader table;
};

using Strings = std::vector<std::string>;

TEST_F(StringTableTest, LoadsEnglishStringsInFileOrder) {
  ASSERT_TRUE(table.Load("!=!New\n!=!Delete\n!=!Config\n"));

  EXPECT_EQ(table.strings, (Strings{"New", "Delete", "Config"}));
}

TEST_F(StringTableTest, SelectsStringsForTheCurrentLanguage) {
  const char *file = "!/!0: new pilot\n"
                     "!=!New\n"
                     "!G!Neu\n"
                     "!F!Nouveau\n"
                     "!/!1: delete pilot\n"
                     "!=!Delete\n"
                     "!G!Loeschen\n"
                     "!F!Supprimer\n";

  Localization_SetLanguage(LANGUAGE_GERMAN);
  ASSERT_TRUE(table.Load(file));
  EXPECT_EQ(table.strings, (Strings{"Neu", "Loeschen"}));

  Localization_SetLanguage(LANGUAGE_FRENCH);
  ASSERT_TRUE(table.Load(file));
  EXPECT_EQ(table.strings, (Strings{"Nouveau", "Supprimer"}));

  Localization_SetLanguage(LANGUAGE_ENGLISH);
  ASSERT_TRUE(table.Load(file));
  EXPECT_EQ(table.strings, (Strings{"New", "Delete"}));
}

TEST_F(StringTableTest, UntaggedLinesContinueThePreviousStringOnANewLine) {
  ASSERT_TRUE(table.Load("!=!first line\n"
                         "second line\n"
                         "third line\n"
                         "!=!next string\n"));

  EXPECT_EQ(table.strings, (Strings{"first line\nsecond line\nthird line", "next string"}));
}

TEST_F(StringTableTest, ContinuationLinesOfOtherLanguagesAreIgnored) {
  Localization_SetLanguage(LANGUAGE_GERMAN);
  ASSERT_TRUE(table.Load("!=!English text\n"
                         "more English\n"
                         "!G!Deutscher Text\n"
                         "mehr Deutsch\n"
                         "!F!Texte\n"
                         "plus\n"));

  EXPECT_EQ(table.strings, (Strings{"Deutscher Text\nmehr Deutsch"}));
}

TEST_F(StringTableTest, CommentEndsTheCurrentString) {
  ASSERT_TRUE(table.Load("!=!A\n"
                         "!/! a comment\n"
                         "this line follows a comment, not a string\n"
                         "!=!B\n"));

  EXPECT_EQ(table.strings, (Strings{"A", "B"}));
}

TEST_F(StringTableTest, BlankLinesBetweenStringsDoNotCreateStrings) {
  ASSERT_TRUE(table.Load("\n!=!A\n\n\n!=!B\n\n"));

  EXPECT_EQ(table.strings, (Strings{"A", "B"}));
}

TEST_F(StringTableTest, LastStringIsKeptWithoutTrailingNewline) {
  ASSERT_TRUE(table.Load("!=!A\n!=!B"));

  EXPECT_EQ(table.strings, (Strings{"A", "B"}));
}

TEST_F(StringTableTest, CrLfLineEndingsAreNotPartOfStrings) {
  ASSERT_TRUE(table.Load("!=!A\r\ncontinued\r\n!=!B\r\n"));

  EXPECT_EQ(table.strings, (Strings{"A\ncontinued", "B"}));
}

TEST_F(StringTableTest, TranslatesDocumentedEscapeSequences) {
  ASSERT_TRUE(table.Load("!=!col1\\tcol2\n"
                         "!=!line1\\nline2\n"
                         "!=!back\\\\slash \\\"quoted\\\"\n"));

  EXPECT_EQ(table.strings, (Strings{"col1\tcol2", "line1\nline2", "back\\slash \"quoted\""}));
}

TEST_F(StringTableTest, TranslatesDecimalCharacterCodes) {
  ASSERT_TRUE(table.Load("!=!\\65\\066C\n"
                         "!=!x\\1y\n"
                         "!=!\\255\n"));

  ASSERT_EQ(table.strings.size(), 3u);
  EXPECT_EQ(table.strings[0], "ABC");
  EXPECT_EQ(table.strings[1], std::string("x\x01y"));
  EXPECT_EQ(table.strings[2], std::string("\xff"));
}

TEST_F(StringTableTest, EscapesInContinuationLinesAreTranslated) {
  ASSERT_TRUE(table.Load("!=!a\\tb\n"
                         "c\\td\n"));

  EXPECT_EQ(table.strings, (Strings{"a\tb\nc\td"}));
}

TEST_F(StringTableTest, FallsBackToEnglishWhenFileHasNoStringsForCurrentLanguage) {
  Localization_SetLanguage(LANGUAGE_SPANISH);

  ASSERT_TRUE(table.Load("!=!One\n!G!Eins\n!=!Two\n!G!Zwei\n"));

  EXPECT_EQ(table.strings, (Strings{"One", "Two"}));
  EXPECT_EQ(Localization_GetLanguage(), LANGUAGE_SPANISH);
}

TEST_F(StringTableTest, FileWithoutStringsLoadsAsEmptyTable) {
  EXPECT_TRUE(table.Load("!/! only a comment\n\n"));

  EXPECT_EQ(table.size, 0);
  EXPECT_TRUE(table.table_is_null);
}

TEST_F(StringTableTest, MissingFileFails) {
  d3_test::TempDir dir;
  auto missing = dir.path() / "missing.str";
  char **table = reinterpret_cast<char **>(&table); // any non-null value
  int size = -1;

  EXPECT_FALSE(CreateStringTable(missing.string().c_str(), &table, &size));
  EXPECT_EQ(table, nullptr);
  EXPECT_EQ(size, 0);
}

TEST_F(StringTableTest, LongLinesAreLoadedInFull) {
  // D3.STR allows lines of up to 1024 characters.
  std::string long_text(1000, 'x');
  ASSERT_TRUE(table.Load("!=!" + long_text + "\n!=!short\n"));

  EXPECT_EQ(table.strings, (Strings{long_text, "short"}));
}

} // namespace
