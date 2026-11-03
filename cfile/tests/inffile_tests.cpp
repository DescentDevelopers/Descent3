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
#include <vector>

#include <gtest/gtest.h>

#include "d3_test/files.h"
#include "support/cfile_test_utils.h"
#include "support/inffile_test_utils.h"
#include "inffile.h"

using cfile_test::CFileFixture;
using cfile_test::CMD_IFI;
using cfile_test::CMD_LLPT;
using cfile_test::CMD_LPLC;
using cfile_test::CMD_NAME;
using cfile_test::CMD_PLAY;
using cfile_test::CMD_SETI;
using cfile_test::CMD_STREAM;
using cfile_test::Parsed;
using cfile_test::ParseNextLine;
using cfile_test::TestLex;

class InfFileTest : public CFileFixture {};

TEST_F(InfFileTest, OpenRequiresTheTagOnTheFirstLine) {
  dir_.Write("theme.omf", "[theme file]\r\nplay\r\n");

  InfFile wrong;
  EXPECT_FALSE(wrong.Open(dir_ / "theme.omf", "[hud file]", TestLex));

  InfFile right;
  EXPECT_TRUE(right.Open(dir_ / "theme.omf", "[theme file]", TestLex));

  InfFile missing;
  EXPECT_FALSE(missing.Open(dir_ / "nope.omf", "[theme file]", TestLex));
}

TEST_F(InfFileTest, ParsesCommandsAndOperandsLikeThemeFiles) {
  dir_.Write("theme.omf", "[theme file]\r\n"
                          "\r\n"
                          "stream=MenuMain.osf$MenuMain\r\n"
                          "@ idle Theme\r\n"
                          "ifi 0;llpt MenuMain;lplc 0\r\n"
                          "ifi 1; seti 0\r\n"
                          "play\r\n"
                          "bogus 5\r\n");

  InfFile inf;
  ASSERT_TRUE(inf.Open(dir_ / "theme.omf", "[theme file]", TestLex));

  EXPECT_EQ(ParseNextLine(inf), (std::vector<Parsed>{{INFFILE_EOL, ""}}));
  EXPECT_EQ(ParseNextLine(inf), (std::vector<Parsed>{{CMD_STREAM, "MenuMain.osf$MenuMain"}, {INFFILE_EOL, ""}}));
  EXPECT_EQ(ParseNextLine(inf), (std::vector<Parsed>{{INFFILE_COMMENT, ""}}));
  EXPECT_EQ(ParseNextLine(inf),
            (std::vector<Parsed>{{CMD_IFI, "0"}, {CMD_LLPT, "MenuMain"}, {CMD_LPLC, "0"}, {INFFILE_EOL, ""}}));
  EXPECT_EQ(ParseNextLine(inf), (std::vector<Parsed>{{CMD_IFI, "1"}, {CMD_SETI, "0"}, {INFFILE_EOL, ""}}));
  EXPECT_EQ(ParseNextLine(inf), (std::vector<Parsed>{{CMD_PLAY, ""}, {INFFILE_EOL, ""}}));
  // Unknown commands come back as whatever the lexer returns for them.
  EXPECT_EQ(ParseNextLine(inf), (std::vector<Parsed>{{INFFILE_ERROR, ""}}));

  EXPECT_FALSE(inf.ReadLine());
}

TEST_F(InfFileTest, OperandIsTrimmedButKeepsInternalSpaces) {
  dir_.Write("pad.ctl", "[controller settings]\n"
                        "name=  Logitech Extreme 3D  \n"
                        "name\tGamepad Two\n");

  InfFile inf;
  ASSERT_TRUE(inf.Open(dir_ / "pad.ctl", "[controller settings]", TestLex));
  EXPECT_EQ(ParseNextLine(inf), (std::vector<Parsed>{{CMD_NAME, "Logitech Extreme 3D"}, {INFFILE_EOL, ""}}));
  EXPECT_EQ(ParseNextLine(inf), (std::vector<Parsed>{{CMD_NAME, "Gamepad Two"}, {INFFILE_EOL, ""}}));
  EXPECT_FALSE(inf.ReadLine());
}

TEST_F(InfFileTest, SymbolsAreDefinedAndSubstituted) {
  dir_.Write("sym.omf", "[theme file]\n"
                        "#track=MenuMain.osf\n"
                        "stream=#track\n"
                        "stream=#undefined\n");

  InfFile inf;
  ASSERT_TRUE(inf.Open(dir_ / "sym.omf", "[theme file]", TestLex));

  char operand[INFFILE_LINELEN];
  ASSERT_TRUE(inf.ReadLine());
  EXPECT_EQ(inf.ParseLine(operand, INFFILE_LINELEN), INFFILE_SYMBOL);

  EXPECT_EQ(ParseNextLine(inf), (std::vector<Parsed>{{CMD_STREAM, "MenuMain.osf"}, {INFFILE_EOL, ""}}));
  EXPECT_EQ(ParseNextLine(inf), (std::vector<Parsed>{{INFFILE_ERROR, ""}}));
}

TEST_F(InfFileTest, CanBeReopenedAfterClose) {
  dir_.Write("theme.omf", "[theme file]\nseti 7\n");

  InfFile inf;
  for (int pass = 0; pass < 2; pass++) {
    SCOPED_TRACE(pass);
    ASSERT_TRUE(inf.Open(dir_ / "theme.omf", "[theme file]", TestLex));
    EXPECT_EQ(ParseNextLine(inf), (std::vector<Parsed>{{CMD_SETI, "7"}, {INFFILE_EOL, ""}}));
    EXPECT_FALSE(inf.ReadLine());
    inf.Close();
  }
}
