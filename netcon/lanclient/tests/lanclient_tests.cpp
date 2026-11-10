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

// The DLL talks to the game exclusively through the function/variable pointers
// declared in con_dll.h (normally filled in by CommonDLLInit from the game's API
// table). Those pointers are global definitions inside con_dll.h, which is pulled
// in by lanclient.cpp, so the DLL source is included directly here to give the
// tests access to them.

#include "lanclient.cpp"

#include <cstring>
#include <string>

#include <gtest/gtest.h>

#include "support/fake_host.h"

namespace {

using namespace lanclient_test;

class LanClientTest : public ::testing::Test {
protected:
  FakeHost fake;

  void SetUp() override {
    host = &fake;

    DLLMultiGameStarting = &fake.multi_game_starting;
    DLLGame_is_master_tracker_game = &fake.game_is_master_tracker_game;
    DLLNum_network_games_known = &fake.num_network_games_known;
    DLLMulti_Gamelist_changed = &fake.multi_gamelist_changed;
    DLLNewUIWindow_alpha = &fake.newui_alpha;
    DLLNetwork_games = fake.network_games;
    DLLAuto_login_addr = fake.auto_login_addr;
    DLLAuto_login_port = fake.auto_login_port;
    DLLTCP_active = true;

    DLLTryToJoinServer = FakeTryToJoinServer;
    DLLMultiStartClient = FakeMultiStartClient;
    DLLmsn_CheckGetMission = FakeMsnCheckGetMission;
    DLLDoPlayerMouselookCheck = FakeDoPlayerMouselookCheck;
    DLLSearchForLocalGamesTCP = FakeSearchForLocalGamesTCP;
    DLLUpdateAndPackGameList = FakeUpdateAndPackGameList;
    DLLDoMessageBoxFP = FakeDoMessageBox;
    DLLPollUI = FakePollUI;
    DLLEditGetText = FakeEditGetText;
    DLLEditSetText = FakeEditSetText;
    DLLListGetSelectedIndex = FakeListGetSelectedIndex;
    DLLDatabaseWrite = FakeDatabaseWrite;
    DLLDatabaseRead = FakeDatabaseRead;
    DLLtimer_GetTime = FakeTimerGetTime;
    DLLCreateNewUITextItemFP = FakeCreateNewUITextItem;
    DLLRemoveUITextItem = FakeRemoveUITextItem;
    DLLNewUIWindowCreate = FakeNewUIWindowCreate;
    DLLNewUIWindowOpen = FakeWindowOp;
    DLLNewUIWindowClose = FakeWindowOp;
    DLLNewUIWindowDestroy = FakeWindowOp;
    DLLNewUIWindowLoadBackgroundImage = FakeLoadBackgroundImage;
    DLLTextCreate = FakeTextCreate;
    DLLEditCreate = FakeGadgetCreate;
    DLLListCreate = FakeGadgetCreate;
    DLLHotSpotCreate = FakeHotSpotCreate;
    DLLListRemoveAll = FakeWindowOp;
    DLLListAddItem = FakeListAddItem;
    DLLListSelectItem = FakeListAddItem;
    DLLDeleteUIItem = FakeDeleteUIItem;
    DLLSetScreenMode = FakeSetScreenMode;
  }

  void TearDown() override { host = nullptr; }
};

// The game splits "host:port" into Auto_login_addr / Auto_login_port before
// calling MT_AUTO_LOGIN (see AutoConnectLANIP in Descent3/multi_ui.cpp).
// network_address stores the IP in network byte order and the port in host
// byte order (networking.cpp converts it with htons when sending).

TEST_F(LanClientTest, AutoLoginJoinsServerAtGivenAddressAndPort) {
  SetAutoLogin(fake, "192.168.1.5", "2093");

  DLLMultiCall(MT_AUTO_LOGIN);

  ASSERT_EQ(fake.join_attempts.size(), 1u);
  const network_address &a = fake.join_attempts[0];
  const uint8_t expected_ip[4] = {192, 168, 1, 5};
  EXPECT_EQ(0, memcmp(a.address, expected_ip, 4));
  EXPECT_EQ(a.port, 2093);
  EXPECT_EQ(a.connection_type, NP_TCP);
  EXPECT_EQ(fake.multi_game_starting, 1);
  EXPECT_EQ(fake.game_is_master_tracker_game, 0);
}

TEST_F(LanClientTest, AutoLoginWithoutPortUsesDefaultGamePort) {
  SetAutoLogin(fake, "10.0.0.1", "");

  DLLMultiCall(MT_AUTO_LOGIN);

  ASSERT_EQ(fake.join_attempts.size(), 1u);
  EXPECT_EQ(fake.join_attempts[0].port, DEFAULT_GAME_PORT);
}

TEST_F(LanClientTest, AutoLoginFailedJoinReportsGameNotStarting) {
  SetAutoLogin(fake, "10.0.0.1", "2093");
  fake.join_result = 0;
  fake.multi_game_starting = 1;

  DLLMultiCall(MT_AUTO_LOGIN);

  EXPECT_EQ(fake.join_attempts.size(), 1u);
  EXPECT_EQ(fake.multi_game_starting, 0);
}

TEST_F(LanClientTest, AutoLoginWithoutAddressDoesNotTryToJoin) {
  SetAutoLogin(fake, "", "2093");
  fake.multi_game_starting = 1;

  DLLMultiCall(MT_AUTO_LOGIN);

  EXPECT_TRUE(fake.join_attempts.empty());
  EXPECT_EQ(fake.multi_game_starting, 0);
}

class LanClientNoTcpTest : public LanClientTest, public ::testing::WithParamInterface<int> {};

TEST_P(LanClientNoTcpTest, RefusesWithErrorAndDoesNotTouchNetwork) {
  DLLTCP_active = false;
  SetAutoLogin(fake, "10.0.0.1", "2093");
  fake.multi_game_starting = 1;

  DLLMultiCall(GetParam());

  EXPECT_EQ(fake.message_boxes, 1);
  EXPECT_EQ(fake.multi_game_starting, 0);
  EXPECT_TRUE(fake.join_attempts.empty());
  EXPECT_TRUE(fake.searches.empty());
}

INSTANTIATE_TEST_SUITE_P(EntryEvents, LanClientNoTcpTest,
                         ::testing::Values(MT_EVT_LOGIN, MT_RETURN_TO_GAME_LIST, MT_AUTO_LOGIN));

TEST_F(LanClientTest, DirectAddressSearchUsesTypedHostAndPort) {
  fake.edit_box_text = "10.1.2.3:3000";
  fake.poll_results = {9}; // "search address" hotspot, then the menu is cancelled

  DLLMultiCall(MT_EVT_LOGIN);

  auto searches = DirectedSearches(fake);
  ASSERT_EQ(searches.size(), 1u);
  EXPECT_EQ(searches[0].addr, inet_addr("10.1.2.3"));
  // SearchForLocalGamesTCP takes the port in network byte order.
  EXPECT_EQ(ntohs(searches[0].port), 3000);
}

TEST_F(LanClientTest, DirectAddressSearchWithoutPortUsesDefaultGamePort) {
  fake.edit_box_text = "10.1.2.3";
  fake.poll_results = {9};

  DLLMultiCall(MT_EVT_LOGIN);

  auto searches = DirectedSearches(fake);
  ASSERT_EQ(searches.size(), 1u);
  EXPECT_EQ(searches[0].addr, inet_addr("10.1.2.3"));
  EXPECT_EQ(ntohs(searches[0].port), DEFAULT_GAME_PORT);
}

TEST_F(LanClientTest, DirectAddressSearchIgnoresInvalidAddress) {
  fake.edit_box_text = "not an address";
  fake.poll_results = {9};

  DLLMultiCall(MT_EVT_LOGIN);

  EXPECT_TRUE(DirectedSearches(fake).empty());
}

TEST_F(LanClientTest, SearchedAddressIsRememberedForNextVisit) {
  fake.edit_box_text = "10.1.2.3:3000";
  fake.poll_results = {9};
  DLLMultiCall(MT_EVT_LOGIN);

  fake.edit_box_set_texts.clear();
  fake.polls_after_script = 0;
  DLLMultiCall(MT_EVT_LOGIN);

  ASSERT_FALSE(fake.edit_box_set_texts.empty());
  EXPECT_EQ(fake.edit_box_set_texts.back(), "10.1.2.3:3000");
}

TEST_F(LanClientTest, JoinSelectedGameConnectsToThatGameOverTcp) {
  AddNetworkGame(fake, "first", "10.0.0.1", 1111);
  AddNetworkGame(fake, "second", "10.0.0.2", 2222);
  AddNetworkGame(fake, "third", "10.0.0.3", 3333);
  fake.selected_index = 1;
  fake.poll_results = {-1, UID_OK}; // let the list populate, then "join selected"

  DLLMultiCall(MT_EVT_LOGIN);

  ASSERT_EQ(fake.join_attempts.size(), 1u);
  EXPECT_EQ(AddressOf(fake.join_attempts[0]), inet_addr("10.0.0.2"));
  EXPECT_EQ(fake.join_attempts[0].port, 2222);
  EXPECT_EQ(fake.join_attempts[0].connection_type, NP_TCP);
  EXPECT_EQ(fake.multi_game_starting, 1);
  EXPECT_EQ(fake.game_is_master_tracker_game, 0);
}

TEST_F(LanClientTest, FailedJoinReportsGameNotStarting) {
  AddNetworkGame(fake, "only", "10.0.0.1", 1111);
  fake.selected_index = 0;
  fake.join_result = 0;
  fake.poll_results = {-1, UID_OK};

  DLLMultiCall(MT_EVT_LOGIN);

  EXPECT_EQ(fake.join_attempts.size(), 1u);
  EXPECT_EQ(fake.multi_game_starting, 0);
}

TEST_F(LanClientTest, JoinIsNotAttemptedWhenMissionCannotBeObtained) {
  AddNetworkGame(fake, "only", "10.0.0.1", 1111);
  fake.selected_index = 0;
  fake.mission_check_result = 0;
  fake.poll_results = {-1, UID_OK};

  DLLMultiCall(MT_EVT_LOGIN);

  EXPECT_TRUE(fake.join_attempts.empty());
  EXPECT_EQ(fake.multi_game_starting, 0);
}

TEST_F(LanClientTest, JoinWithNoGamesListedShowsErrorInsteadOfJoining) {
  fake.poll_results = {UID_OK};

  DLLMultiCall(MT_EVT_LOGIN);

  EXPECT_TRUE(fake.join_attempts.empty());
  EXPECT_EQ(fake.message_boxes, 1);
  EXPECT_EQ(fake.multi_game_starting, 0);
}

class ConDllGetStringTest : public ::testing::Test {
protected:
  char s0[5] = "zero";
  char s1[4] = "one";
  char past_end[9] = "past end";
  char *table[3] = {s0, s1, past_end};
  char **saved_table = nullptr;
  int saved_size = 0;

  void SetUp() override {
    saved_table = StringTable;
    saved_size = StringTableSize;
    StringTable = table;
    StringTableSize = 2;
  }

  void TearDown() override {
    StringTable = saved_table;
    StringTableSize = saved_size;
  }
};

TEST_F(ConDllGetStringTest, OutOfRangeIndicesNeverReturnTableEntriesOrNull) {
  EXPECT_STREQ(GetString(0), "zero");
  EXPECT_STREQ(GetString(1), "one");
  for (int bad : {-1, 2, 1000}) {
    const char *s = GetString(bad);
    ASSERT_NE(s, nullptr) << bad;
    EXPECT_NE(s, s0) << bad;
    EXPECT_NE(s, s1) << bad;
    EXPECT_NE(s, past_end) << bad;
  }
}

class ConDllMissionListTest : public ::testing::Test {
protected:
  void SetUp() override {
    DLLmem_free = CountingFree;
    DLLRemoveUITextItem = FakeRemoveUITextItem;
    msn_frees = 0;
    FirstMsn = nullptr;
  }
  void TearDown() override { RemoveAllMsnItems(); }
};

TEST_F(ConDllMissionListTest, FindLocatesEveryItemByName) {
  EXPECT_EQ(FindMsnItem("anything"), nullptr);

  msn_list *a = AddMission("Alpha");
  msn_list *b = AddMission("Bravo");
  msn_list *c = AddMission("Charlie");

  EXPECT_EQ(FindMsnItem("Alpha"), a);
  EXPECT_EQ(FindMsnItem("Bravo"), b);
  EXPECT_EQ(FindMsnItem("Charlie"), c);
  EXPECT_EQ(FindMsnItem("Delta"), nullptr);
}

TEST_F(ConDllMissionListTest, RemoveAllFreesEachItemOnceAndEmptiesList) {
  AddMission("Alpha");
  AddMission("Bravo");
  AddMission("Charlie");

  RemoveAllMsnItems();

  EXPECT_EQ(msn_frees, 3);
  EXPECT_EQ(FindMsnItem("Alpha"), nullptr);

  msn_list *d = AddMission("Delta");
  EXPECT_EQ(FindMsnItem("Delta"), d);
  EXPECT_EQ(FindMsnItem("Charlie"), nullptr);
}

} // namespace
