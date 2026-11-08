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

// The File_* engine imports are bound to the real cfile library, as in the game, so the
// line handling is the real one: cfile opens the file in text mode and normalises CR/LF
// line endings.
//
// This file is compiled into one test executable per module; the module under test is
// whichever aiscripts_<module>_module.cpp it is linked with.

#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "support/aiscripts_message_files.h"
#include "support/aiscripts_test_support.h"

namespace {

using aiscripts_test::LoadMessageFile;
using aiscripts_test::LoadMessages;
using aiscripts_test::LookupMessage;
using aiscripts_test::NotFoundMessage;

class MessageFileTest : public ::testing::Test {
protected:
  void SetUp() override {
    aiscripts_test::InstallEngineImports();
    aiscripts_test::ResetMessages();
  }

  void TearDown() override { aiscripts_test::ResetMessages(); }

  d3_test::TempDir files_;
};

TEST_F(MessageFileTest, LoadsNameValuePairs) {
  ASSERT_TRUE(LoadMessages(files_, "Alpha=First message\n"
                                   "Beta=Second message\n"));

  EXPECT_STREQ(LookupMessage("Alpha"), "First message");
  EXPECT_STREQ(LookupMessage("Beta"), "Second message");
}

TEST_F(MessageFileTest, SkipsCommentsBlankLinesAndTheHeaderLine) {
  // Layout written by DALLAS (compare scripts/level1.msg).
  ASSERT_TRUE(LoadMessages(files_, "//////////////////////////////////////////////\n"
                                   "// D.A.L.L.A.S. Generated Message Table File\n"
                                   "//////////////////////////////////////////////\n"
                                   "\n"
                                   "NEXT_MESSAGE_ID_NUMBER     3\n"
                                   "\n"
                                   "// Message List\n"
                                   "//Commented=not a message\n"
                                   "IntroMessage=Deimos - PTMC Data Retention Center\n"
                                   "\n"
                                   "Exit=Exit found!\n"));

  EXPECT_STREQ(LookupMessage("IntroMessage"), "Deimos - PTMC Data Retention Center");
  EXPECT_STREQ(LookupMessage("Exit"), "Exit found!");
  EXPECT_EQ(LookupMessage("NEXT_MESSAGE_ID_NUMBER"), NotFoundMessage());
  EXPECT_EQ(LookupMessage("//Commented"), NotFoundMessage());
  EXPECT_EQ(LookupMessage("Commented"), NotFoundMessage());
}

TEST_F(MessageFileTest, StripsLineIndentationTrailingSpaceAndCarriageReturns) {
  ASSERT_TRUE(LoadMessages(files_, "  Indented=Hello world   \r\n"
                                   "\tTabbed=Tab\t\r\n"
                                   "Crlf=Windows line ending\r\n"));

  EXPECT_STREQ(LookupMessage("Indented"), "Hello world");
  EXPECT_STREQ(LookupMessage("Tabbed"), "Tab");
  EXPECT_STREQ(LookupMessage("Crlf"), "Windows line ending");
}

TEST_F(MessageFileTest, MessageTextKeepsEverythingAfterTheFirstEqualsSign) {
  ASSERT_TRUE(LoadMessages(files_, "Formula=a = b + c\n"
                                   "Url=see http://example.com//path\n"));

  EXPECT_STREQ(LookupMessage("Formula"), "a = b + c");
  EXPECT_STREQ(LookupMessage("Url"), "see http://example.com//path");
}

TEST_F(MessageFileTest, EmptyMessageIsFoundAndDistinctFromMissing) {
  // DALLAS writes unused/blank messages as "Name=" (e.g. "Blank=" in level1.msg).
  ASSERT_TRUE(LoadMessages(files_, "Blank=\n"
                                   "After=still parsed\n"));

  EXPECT_STREQ(LookupMessage("Blank"), "");
  EXPECT_STREQ(LookupMessage("After"), "still parsed");
}

TEST_F(MessageFileTest, LastLineWithoutNewlineIsLoaded) {
  ASSERT_TRUE(LoadMessages(files_, "First=one\n"
                                   "Last=no newline at end of file"));

  EXPECT_STREQ(LookupMessage("First"), "one");
  EXPECT_STREQ(LookupMessage("Last"), "no newline at end of file");
}

TEST_F(MessageFileTest, LookupOfMissingOrNullNameReturnsAPlaceholderString) {
  ASSERT_TRUE(LoadMessages(files_, "Alpha=First message\n"));

  const char *missing = LookupMessage("Beta");
  ASSERT_NE(missing, nullptr);
  EXPECT_STRNE(missing, "First message");

  const char *null_name = LookupMessage(nullptr);
  ASSERT_NE(null_name, nullptr);
  EXPECT_STRNE(null_name, "First message");
}

TEST_F(MessageFileTest, LoadingAnotherFileReplacesThePreviousMessages) {
  ASSERT_TRUE(LoadMessageFile(files_.Write("first.msg", "Old=from the first file\n").string().c_str()));
  ASSERT_TRUE(LoadMessageFile(files_.Write("second.msg", "New=from the second file\n").string().c_str()));

  EXPECT_STREQ(LookupMessage("New"), "from the second file");
  EXPECT_EQ(LookupMessage("Old"), NotFoundMessage());
}

TEST_F(MessageFileTest, MissingFileReportsFailure) {
  EXPECT_FALSE(LoadMessageFile((files_.path() / "does_not_exist.msg").string().c_str()));
}

TEST_F(MessageFileTest, OverlongLineIsTruncatedWithoutDisturbingLaterLines) {
  const std::string long_text(5000, 'x');
  ASSERT_TRUE(LoadMessages(files_, "Long=" + long_text + "\n" + "After=ok\n"));

  const std::string loaded = LookupMessage("Long");
  EXPECT_FALSE(loaded.empty());
  EXPECT_LT(loaded.size(), long_text.size());
  EXPECT_EQ(loaded, std::string(loaded.size(), 'x'));
  EXPECT_STREQ(LookupMessage("After"), "ok");
}

TEST_F(MessageFileTest, FileWithMoreMessagesThanTheTableHoldsLoadsSafely) {
  std::string contents;
  for (int i = 0; i < 1000; i++)
    contents += "Msg" + std::to_string(i) + "=text " + std::to_string(i) + "\n";
  ASSERT_TRUE(LoadMessages(files_, contents));

  EXPECT_STREQ(LookupMessage("Msg0"), "text 0");
  EXPECT_STREQ(LookupMessage("Msg1"), "text 1");
}

} // namespace
