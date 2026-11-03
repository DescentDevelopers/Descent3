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

// No sockets are opened: outgoing sends go to INVALID_SOCKET and simply fail.

#include <cstring>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "networking.h"
#include "chat_api.h"
#include "support/chat_api_internals.h"
#include "support/chat_test_helpers.h"
#include "support/chat_module_fakes.h"

using ::testing::Contains;
using ::testing::ElementsAre;
using ::testing::EndsWith;
using ::testing::IsEmpty;
using ::testing::Not;
using ::testing::Pair;
using ::testing::StartsWith;
using ::testing::UnorderedElementsAre;

namespace {

using namespace chat_test;

class ChatApiTest : public ::testing::Test {
protected:
  void SetUp() override { ResetChat(); }
  void TearDown() override { ResetChat(); }
};

TEST(ChatGetWordNum, SplitsOnAnyRunOfWhitespace) {
  const char *line = "  alpha\tbeta \r\n gamma";
  EXPECT_STREQ(GetWordNum(0, line), "alpha");
  EXPECT_STREQ(GetWordNum(1, line), "beta");
  EXPECT_STREQ(GetWordNum(2, line), "gamma");
}

TEST(ChatGetWordNum, PastTheLastWordIsEmpty) {
  EXPECT_STREQ(GetWordNum(3, "a b c"), "");
  EXPECT_STREQ(GetWordNum(0, ""), "");
  EXPECT_STREQ(GetWordNum(0, "   "), "");
}

TEST(ChatGetWordNum, ColonPrefixedWordReturnsRestOfLine) {
  EXPECT_STREQ(GetWordNum(2, "Pilot Bob :hello there  world"), "hello there  world");
  EXPECT_STREQ(GetWordNum(0, ":all of it"), "all of it");
  EXPECT_STREQ(GetWordNum(1, "PRIVMSG #lobby :hello there"), "#lobby");
}

TEST_F(ChatApiTest, CommandQueueIsFifoAndCopiesPayload) {
  char nick[] = "first";
  AddChatCommandToQueue(CC_USER_JOINING, nick, sizeof(nick));
  std::strcpy(nick, "XXXXX");
  AddChatCommandToQueue(CC_USER_LEAVING, "second", 7);
  AddChatCommandToQueue(CC_DISCONNECTED, nullptr, 0);

  EXPECT_THAT(DrainCommands(), ElementsAre(Pair(CC_USER_JOINING, "first"), Pair(CC_USER_LEAVING, "second"),
                                           Pair(CC_DISCONNECTED, "")));
  EXPECT_EQ(GetChatCommandFromQueue(), nullptr);
}

TEST_F(ChatApiTest, AddChatUserIgnoresCaseInsensitiveDuplicates) {
  EXPECT_EQ(AddChatUser("Bob"), 1);
  EXPECT_EQ(AddChatUser("bob"), 0);
  EXPECT_THAT(Users(), ElementsAre("Bob"));
  EXPECT_THAT(DrainCommands(), ElementsAre(Pair(CC_USER_JOINING, "Bob")));
}

TEST_F(ChatApiTest, RemoveChatUserMatchesCaseInsensitively) {
  AddChatUser("Bob");
  AddChatUser("Alice");
  FlushChatCommandQueue();

  char alice[] = "ALICE";
  EXPECT_EQ(RemoveChatUser(alice), 1);
  EXPECT_THAT(DrainCommands(), ElementsAre(Pair(CC_USER_LEAVING, "Alice")));
  EXPECT_THAT(Users(), ElementsAre("Bob"));

  char carol[] = "Carol";
  EXPECT_EQ(RemoveChatUser(carol), 0);
  EXPECT_THAT(DrainCommands(), IsEmpty());
}

TEST_F(ChatApiTest, NamesReplyAddsEveryUserWithModePrefixStripped) {
  Feed(":irc.test 353 Pilot = #descent :@Op +Voice Plain");
  EXPECT_THAT(Users(), UnorderedElementsAre("Op", "Voice", "Plain"));
}

TEST_F(ChatApiTest, OtherUserJoiningIsAddedOnce) {
  Welcome("Pilot");
  Feed(":Bob!bob@host.example JOIN #descent");
  EXPECT_THAT(Users(), ElementsAre("Bob"));
  EXPECT_THAT(DrainCommands(), ElementsAre(Pair(CC_USER_JOINING, "Bob")));
}

TEST_F(ChatApiTest, OtherUserPartingOrQuittingIsRemoved) {
  Welcome("Pilot");
  Names("Bob Alice Carol");
  Feed(":Bob!bob@host.example PART #descent");
  Feed(":Alice!alice@host.example QUIT :Quit: bye");
  EXPECT_THAT(Users(), ElementsAre("Carol"));
}

TEST_F(ChatApiTest, OurOwnPartClearsTheUserList) {
  Welcome("Pilot");
  Names("Bob Pilot Carol");
  Feed(":Pilot!pilot@host.example PART #descent");
  EXPECT_THAT(Users(), IsEmpty());
}

TEST_F(ChatApiTest, BeingKickedLeavesTheChannel) {
  Welcome("Pilot");
  ConnectedScope connected;
  Feed(":Pilot!pilot@host.example JOIN #descent");
  Names("@Op Pilot Other");
  ASSERT_NE(SendChatString("still here"), nullptr) << "precondition: chat is echoed while in a channel";

  Feed(":Op!op@host.example KICK #descent Pilot :behave");

  EXPECT_THAT(DrainCommands(), Contains(Pair(CC_KICKED, "")));
  EXPECT_THAT(Users(), IsEmpty());
  EXPECT_EQ(SendChatString("anyone?"), nullptr);
}

TEST_F(ChatApiTest, OtherUserNickChangeRenamesThemInTheList) {
  Welcome("Pilot");
  Names("Bob Pilot");
  Feed(":Bob!bob@host.example NICK :Robert");
  EXPECT_THAT(DrainCommands(), Contains(Pair(CC_NICKCHANGED, "Bob Robert")));
  EXPECT_THAT(Users(), UnorderedElementsAre("Robert", "Pilot"));
}

TEST_F(ChatApiTest, EndOfMotdMarksConnectedAndAdoptsServerNick) {
  Feed(":irc.test 376 Pilot_ :End of /MOTD command.");
  EXPECT_EQ(Chat_server_connected, 1);
  EXPECT_STREQ(Nick_name, "Pilot_");
}

TEST_F(ChatApiTest, OurOwnNickChangeIsAdopted) {
  Welcome("Pilot");
  Feed(":Pilot!pilot@host.example NICK :Ace");
  EXPECT_STREQ(Nick_name, "Ace");
  Feed(":Ace!pilot@host.example JOIN #descent");
  EXPECT_THAT(DrainCommands(), Contains(Pair(CC_YOURCHANNEL, "#descent")));
}

TEST_F(ChatApiTest, NickInUseRetriesWithDistinctVariantsOfTheOriginal) {
  char nick[] = "Pilot";
  char tid[] = "1234";
  // Port 0 is rejected before any socket work, but the nickname is recorded first.
  ASSERT_EQ(ConnectToChatServer("irc.invalid", 0, nick, tid), -1);

  Feed(":irc.test 433 * Pilot :Nickname is already in use");
  std::string first = Nick_name;
  Feed(":irc.test 433 * " + first + " :Nickname is already in use");
  std::string second = Nick_name;

  EXPECT_THAT(first, StartsWith("Pilot"));
  EXPECT_THAT(second, StartsWith("Pilot"));
  EXPECT_NE(first, "Pilot");
  EXPECT_NE(second, first);
}

TEST_F(ChatApiTest, BadNickDisconnects) {
  EXPECT_EQ(Str(Feed(":irc.test 432 * b@d :Erroneous nickname")), TEST_BAD_NICK);
  EXPECT_THAT(DrainCommands(), Contains(Pair(CC_DISCONNECTED, "")));
}

// SetNewChatChannel returns 0 = joining, 1 = joined, -1 = failed.

TEST_F(ChatApiTest, ServerConfirmingOurJoinCompletesSetNewChatChannel) {
  Welcome("Pilot");
  ConnectedScope connected;
  EXPECT_EQ(SetNewChatChannel("#descent"), 0);
  EXPECT_EQ(SetNewChatChannel("#descent"), 0) << "still waiting for the server";

  Feed(":Pilot!pilot@host.example JOIN :#descent");

  EXPECT_EQ(SetNewChatChannel("#descent"), 1);
  EXPECT_THAT(DrainCommands(),
              UnorderedElementsAre(Pair(CC_YOURCHANNEL, "#descent"), Pair(CC_USER_JOINING, "Pilot")));
}

TEST_F(ChatApiTest, AutoselectReportsTheChannelTheServerPutUsIn) {
  Welcome("Pilot");
  ConnectedScope connected;
  EXPECT_EQ(SetNewChatChannel("#autoselect"), 0);

  Feed(":Pilot!pilot@host.example JOIN #descent_2");

  EXPECT_EQ(SetNewChatChannel("#autoselect"), 1);
  EXPECT_THAT(DrainCommands(), Contains(Pair(CC_YOURCHANNEL, "#descent_2")));
}

TEST_F(ChatApiTest, ChannelMessageShowsSenderAndText) {
  Welcome("Pilot");
  EXPECT_THAT(Str(Feed(":Bob!bob@host.example PRIVMSG #descent :hello there")), EndsWith("[Bob] hello there"));
}

TEST_F(ChatApiTest, PrivateMessageUsesTheFromFormat) {
  Welcome("Pilot");
  EXPECT_EQ(Str(Feed(":Bob!bob@host.example PRIVMSG Pilot :psst")), "from Bob: psst");
}

TEST_F(ChatApiTest, CtcpActionShowsAsEmote) {
  Welcome("Pilot");
  std::string emote = Str(Feed(":Bob!bob@host.example PRIVMSG #descent :\001ACTION waves\001"));
  EXPECT_THAT(emote, EndsWith("* Bob waves"));
  // The text starts with a colour code, which begins with the same byte as a CTCP delimiter.
  EXPECT_EQ(emote.find('\001', 1), std::string::npos) << "CTCP delimiters must not leak into the text";
}

TEST_F(ChatApiTest, MotdTextIsShown) {
  EXPECT_THAT(Str(Feed(":irc.test 372 Pilot :- Welcome to the server")), EndsWith("- Welcome to the server"));
}

TEST_F(ChatApiTest, MalformedOrOverlongLinesAreIgnored) {
  Welcome("Pilot");
  Names("Bob");
  for (const char *line : {"", "   ", ":", ":irc.test"}) {
    SCOPED_TRACE(std::string("line: \"") + line + "\"");
    EXPECT_EQ(Feed(line), nullptr);
  }
  std::string overlong = ":Bob!bob@host.example PRIVMSG #descent :" + std::string(700, 'x');
  EXPECT_EQ(Feed(overlong), nullptr);

  EXPECT_THAT(DrainCommands(), IsEmpty());
  EXPECT_THAT(Users(), ElementsAre("Bob"));
}

TEST_F(ChatApiTest, LocalEchoOfPrivateMessage) {
  Welcome("Pilot");
  ConnectedScope connected;
  EXPECT_EQ(Str(SendChatString("/msg Bob see you there")), "to Bob: see you there");
}

TEST_F(ChatApiTest, LocalEchoOfChannelChatAndEmote) {
  Welcome("Pilot");
  ConnectedScope connected;
  Feed(":Pilot!pilot@host.example JOIN #descent");

  EXPECT_THAT(Str(SendChatString("hello all")), EndsWith("[Pilot] hello all"));
  EXPECT_THAT(Str(SendChatString("/me waves")), EndsWith("* Pilot waves"));
}

TEST_F(ChatApiTest, ChannelListIsAvailableOnceTheListReplyIsComplete) {
  ConnectedScope connected;
  EXPECT_EQ(GetChannelList(), nullptr);

  Feed(":irc.test 321 Pilot Channel :Users  Name");
  Feed(":irc.test 322 Pilot #descent 12 :Main lobby");
  Feed(":irc.test 322 Pilot #d3_ctf 3 :Capture the flag");
  EXPECT_EQ(GetChannelList(), nullptr) << "list not finished yet";
  Feed(":irc.test 323 Pilot :End of /LIST");

  const char *list = GetChannelList();
  ASSERT_NE(list, nullptr);
  EXPECT_THAT(ParseChannelList(list), UnorderedElementsAre(ChannelEntry{"#descent", 12, "Main lobby"},
                                                           ChannelEntry{"#d3_ctf", 3, "Capture the flag"}));
  EXPECT_EQ(GetChannelList(), nullptr);
}

TEST_F(ChatApiTest, ChannelEntriesOutsideAListReplyAreIgnored) {
  ConnectedScope connected;
  Feed(":irc.test 322 Pilot #stray 1 :Not part of a list");
  Feed(":irc.test 321 Pilot Channel :Users  Name");
  Feed(":irc.test 323 Pilot :End of /LIST");

  const char *list = GetChannelList();
  ASSERT_NE(list, nullptr);
  EXPECT_THAT(ParseChannelList(list), IsEmpty());
}

TEST_F(ChatApiTest, NewListReplyDiscardsTheEntriesOfAnUnfinishedOne) {
  ConnectedScope connected;
  Feed(":irc.test 321 Pilot Channel :Users  Name");
  Feed(":irc.test 322 Pilot #old 1 :Stale");
  Feed(":irc.test 321 Pilot Channel :Users  Name");
  Feed(":irc.test 322 Pilot #new 2 :Fresh");
  Feed(":irc.test 323 Pilot :End of /LIST");

  const char *list = GetChannelList();
  ASSERT_NE(list, nullptr);
  EXPECT_THAT(ParseChannelList(list), ElementsAre(ChannelEntry{"#new", 2, "Fresh"}));
}

// GetChannelByUser / GetTrackerIdByUser: nullptr = pending, (char *)-1 = failed.
const char *const kFailed = reinterpret_cast<const char *>(-1);

TEST_F(ChatApiTest, ChannelLookupReturnsTheChannelFromWhois) {
  Welcome("Pilot");
  EXPECT_EQ(GetChannelByUser("Bob"), nullptr);
  EXPECT_EQ(GetChannelByUser("Bob"), nullptr) << "pending until the server answers";

  Feed(":irc.test 319 Pilot Bob :#descent");
  Feed(":irc.test 318 Pilot Bob :End of /WHOIS list.");

  EXPECT_EQ(Str(GetChannelByUser("Bob")), "#descent");
}

TEST_F(ChatApiTest, ChannelLookupCompletesForUserInNoChannel) {
  Welcome("Pilot");
  EXPECT_EQ(GetChannelByUser("Bob"), nullptr);

  Feed(":irc.test 318 Pilot Bob :End of /WHOIS list.");

  const char *result = GetChannelByUser("Bob");
  EXPECT_NE(result, nullptr) << "lookup must not stay pending forever";
  EXPECT_NE(result, kFailed) << "the user is online";
}

TEST_F(ChatApiTest, ChannelLookupFailsForUnknownUser) {
  Welcome("Pilot");
  EXPECT_EQ(GetChannelByUser("Ghost"), nullptr);

  EXPECT_EQ(Str(Feed(":irc.test 401 Pilot Ghost :No such nick/channel")), "not-online Ghost");

  EXPECT_EQ(GetChannelByUser("Ghost"), kFailed);
  // The failure is consumed: the next call starts a new lookup.
  EXPECT_EQ(GetChannelByUser("Bob"), nullptr);
}

TEST_F(ChatApiTest, CancellingClearsAStaleNoSuchNickError) {
  Welcome("Pilot");
  Feed(":irc.test 401 Pilot Ghost :No such nick/channel");

  EXPECT_EQ(GetChannelByUser(kFailed), nullptr);
  EXPECT_EQ(GetChannelByUser("Bob"), nullptr);
  Feed(":irc.test 319 Pilot Bob :#descent");

  EXPECT_EQ(Str(GetChannelByUser("Bob")), "#descent");
}

TEST_F(ChatApiTest, TrackerIdLookupReturnsTheWhoisRealName) {
  // The tracker id is sent as the IRC "real name" in ConnectToChatServer's USER line.
  Welcome("Pilot");
  EXPECT_EQ(GetTrackerIdByUser("Bob"), nullptr);

  Feed(":irc.test 311 Pilot Bob bob host.example * :1234567");

  EXPECT_EQ(Str(GetTrackerIdByUser("Bob")), "1234567");
}

TEST_F(ChatApiTest, TrackerIdLookupFailsForUnknownUser) {
  Welcome("Pilot");
  EXPECT_EQ(GetTrackerIdByUser("Ghost"), nullptr);
  Feed(":irc.test 401 Pilot Ghost :No such nick/channel");
  EXPECT_EQ(GetTrackerIdByUser("Ghost"), kFailed);
}

} // namespace
