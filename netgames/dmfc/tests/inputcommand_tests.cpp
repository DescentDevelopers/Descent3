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

#include "support/dmfc_fakes.h"
#include "support/dmfc_internals.h"
#include "support/dmfc_test_support.h"

using namespace dmfc_test;

TEST(StringParseWord, ReturnsSpaceSeparatedWordsInOrder) {
  const char *input = "  $kick   Bob  now";
  char word[32];
  const char *pos = input;
  std::vector<std::string> words;

  while (StringParseWord(pos, word, sizeof(word), &pos))
    words.emplace_back(word);

  EXPECT_EQ(words, (std::vector<std::string>{"$kick", "Bob", "now"}));
  EXPECT_STREQ(word, "");
}

TEST(StringParseWord, ReturnsFalseForEmptyOrBlankInput) {
  char word[8] = "junk";
  const char *pos = nullptr;

  EXPECT_FALSE(StringParseWord("", word, sizeof(word), &pos));
  EXPECT_STREQ(word, "");

  EXPECT_FALSE(StringParseWord("     ", word, sizeof(word), &pos));
  EXPECT_STREQ(word, "");
}

TEST(StringParseWord, TruncatesLongWordButSkipsAllOfIt) {
  char word[5];
  const char *pos = nullptr;

  ASSERT_TRUE(StringParseWord("abcdefgh next", word, sizeof(word), &pos));
  EXPECT_STREQ(word, "abcd");

  ASSERT_TRUE(StringParseWord(pos, word, sizeof(word), &pos));
  EXPECT_STREQ(word, "next");
}

TEST(StringParseNumber, ParsesSuccessiveIntegers) {
  const char *pos = nullptr;
  int value = -1;

  ASSERT_TRUE(StringParseNumber(" 42 -7", &value, &pos));
  EXPECT_EQ(value, 42);
  ASSERT_TRUE(StringParseNumber(pos, &value, &pos));
  EXPECT_EQ(value, -7);

  value = 99;
  EXPECT_FALSE(StringParseNumber(pos, &value, &pos));
  EXPECT_EQ(value, 0);
}

namespace {

class InputCommandTest : public DMFCFixture {
protected:
  void SetUp() override {
    DMFCFixture::SetUp();
    g_kick_calls.clear();
    g_other_calls.clear();
  }
};

} // namespace

TEST_F(InputCommandTest, DispatchesToRegisteredHandlerWithFullCommandString) {
  ASSERT_EQ(dmfc->AddInputCommand("kick", "kick a player", KickHandler), 1);
  ASSERT_EQ(dmfc->AddInputCommand("ban", "ban a player", OtherHandler), 1);

  EXPECT_TRUE(HandleCommand(*dmfc, "$kick 3"));
  EXPECT_EQ(g_kick_calls, (std::vector<std::string>{"$kick 3"}));
  EXPECT_TRUE(g_other_calls.empty());

  EXPECT_TRUE(HandleCommand(*dmfc, "$ban"));
  EXPECT_EQ(g_other_calls, (std::vector<std::string>{"$ban"}));
}

TEST_F(InputCommandTest, CommandNameIsCaseInsensitiveButArgumentsKeepTheirCase) {
  ASSERT_EQ(dmfc->AddInputCommand("RemoteAdminPass", "", KickHandler), 1);

  EXPECT_TRUE(HandleCommand(*dmfc, "$REMOTEADMINPASS SeCrEt"));
  ASSERT_EQ(g_kick_calls.size(), 1u);
  EXPECT_EQ(g_kick_calls[0].substr(g_kick_calls[0].find(' ')), " SeCrEt");
}

TEST_F(InputCommandTest, RejectsUnknownPrefixedAndUnmarkedCommands) {
  ASSERT_EQ(dmfc->AddInputCommand("kick", "", KickHandler), 1);

  EXPECT_FALSE(HandleCommand(*dmfc, "kick 3"));
  EXPECT_FALSE(HandleCommand(*dmfc, "$kic 3"));
  EXPECT_FALSE(HandleCommand(*dmfc, "$kicker 3"));
  EXPECT_FALSE(HandleCommand(*dmfc, "$ban 3"));
  EXPECT_TRUE(g_kick_calls.empty());
}

TEST_F(InputCommandTest, DuplicateRegistrationIsRejectedCaseInsensitively) {
  ASSERT_EQ(dmfc->AddInputCommand("kick", "", KickHandler), 1);
  ASSERT_EQ(dmfc->AddInputCommand("ban", "", OtherHandler), 1);

  EXPECT_EQ(dmfc->AddInputCommand("KICK", "", OtherHandler), 0);
  EXPECT_EQ(dmfc->AddInputCommand("ban", "", KickHandler), 0);

  EXPECT_TRUE(HandleCommand(*dmfc, "$kick"));
  EXPECT_TRUE(HandleCommand(*dmfc, "$ban"));
  EXPECT_EQ(g_kick_calls, (std::vector<std::string>{"$kick"}));
  EXPECT_EQ(g_other_calls, (std::vector<std::string>{"$ban"}));
}

TEST_F(InputCommandTest, ReportsWhetherCommandMayBeUsedRemotely) {
  ASSERT_EQ(dmfc->AddInputCommand("kick", "", KickHandler, true), 1);
  ASSERT_EQ(dmfc->AddInputCommand("observer", "", OtherHandler, false), 1);

  char kick[] = "Kick";
  char observer[] = "observer";
  char missing[] = "warp";
  EXPECT_EQ(dmfc->CanInputCommandBeUsedRemotely(kick), 1);
  EXPECT_EQ(dmfc->CanInputCommandBeUsedRemotely(observer), 0);
  EXPECT_EQ(dmfc->CanInputCommandBeUsedRemotely(missing), -1);
}
