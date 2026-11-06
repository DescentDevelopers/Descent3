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
#include <set>
#include <string>

#include <gtest/gtest.h>

#include "support/gametrack_packets.h"
#include "support/test_support.h"
#include "mt_net.h"
#include "mtgametrack.h"

namespace {

using namespace mt_test;

class GameTracker : public ::testing::Test {
protected:
  void SetUp() override {
    mt_test::Reset();
    InitGameTrackerClient(GT_D3TNG);
    // Nothing else resets the "game is running" flag; ending a game does.
    SendGameOver();
    InitGameTrackerClient(GT_D3TNG);
    while (GetGameList()) {
    }
    mt_test::sent.clear();
  }
};

TEST_F(GameTracker, ReceivedGameListsAreHandedOutOnceEach) {
  game_list a = MakeGameList(0x01020304, 2092);
  game_list b = MakeGameList(0x05060708, 2093);
  Deliver(MakePacket(GNT_GAMELIST_LITE_DATA, &a, sizeof(a)));
  Deliver(MakePacket(GNT_GAMELIST_LITE_DATA, &b, sizeof(b)));

  std::set<std::pair<uint32_t, uint16_t>> got;
  while (game_list *gl = GetGameList())
    got.insert({gl->game_server[0], gl->game_port[0]});

  std::set<std::pair<uint32_t, uint16_t>> expected = {{0x01020304u, 2092}, {0x05060708u, 2093}};
  EXPECT_EQ(got, expected);
  EXPECT_EQ(GetGameList(), nullptr);
}

TEST_F(GameTracker, RequestGameListDiscardsQueuedListsAndAsksForOurLobby) {
  game_list a = MakeGameList(0x01020304, 2092);
  Deliver(MakePacket(GNT_GAMELIST_LITE_DATA, &a, sizeof(a)));

  RequestGameList();

  EXPECT_EQ(GetGameList(), nullptr) << "lists from before the request are stale";
  auto reqs = SentToTracker(GNT_GAMELIST_LITE_FILTER_REQ);
  ASSERT_EQ(reqs.size(), 1u);
  const char *lobby = reqs[0].header.data;
  EXPECT_STREQ(lobby, "#test_lobby"); // DLLPXO_hosted_lobby_name in stubs.cpp
  EXPECT_GE(reqs[0].len, GAME_HEADER_ONLY_SIZE + strlen("#test_lobby") + 1)
      << "lobby name (with its NUL) must be inside the sent length";
}

TEST_F(GameTracker, GameOverIsConfirmedByServerAck) {
  EXPECT_EQ(SendGameOver(), 0);
  EXPECT_EQ(SentToTracker(GNT_GAMEOVER).size(), 1u);
  EXPECT_EQ(SendGameOver(), 0) << "waiting for the tracker";

  Deliver(MakePacket(GNT_SERVER_ACK));
  EXPECT_EQ(SendGameOver(), 1);

  mt_test::sent.clear();
  EXPECT_EQ(SendGameOver(), 0) << "a confirmed game over leaves us ready for the next one";
  EXPECT_EQ(SentToTracker(GNT_GAMEOVER).size(), 1u);
}

TEST_F(GameTracker, GameOverIsResentUntilItTimesOut) {
  SendGameOver();
  int result = 0;
  AdvanceAndPoll(TRACKER_TIMEOUT * 3, [&] {
    if (result == 0)
      result = SendGameOver();
  });
  EXPECT_EQ(result, 2);
  EXPECT_GE(SentToTracker(GNT_GAMEOVER).size(), 3u) << "unacknowledged game over should be retransmitted";
}

TEST_F(GameTracker, GameOverStopsBeingResentAfterGivingUp) {
  SendGameOver();
  int result = 0;
  AdvanceAndPoll(TRACKER_TIMEOUT * 3, [&] {
    if (result == 0)
      result = SendGameOver();
  });
  ASSERT_EQ(result, 2);

  mt_test::sent.clear();
  AdvanceAndPoll(TRACKER_TIMEOUT, [] { IdleGameTracker(); });
  EXPECT_EQ(SentToTracker(GNT_GAMEOVER).size(), 0u);
}

TEST_F(GameTracker, NewGameIsAnnouncedAndResentUntilAcked) {
  d3_net_game_data_tiny g = MakeTinyGame("#my_lobby");
  StartTrackerGame(&g);

  auto updates = SentToTracker(GNT_GAMEUPDATE_TINY);
  ASSERT_EQ(updates.size(), 1u);
  d3_net_game_data_tiny sent_game;
  memcpy(&sent_game, updates[0].header.data, sizeof(sent_game));
  EXPECT_STREQ(sent_game.lobby, "#my_lobby");
  EXPECT_GE(updates[0].len, GAME_HEADER_ONLY_SIZE + sizeof(d3_net_game_data_tiny));

  mt_test::sent.clear();
  AdvanceAndPoll(TRACKER_RESEND_TIME * 3, [] { IdleGameTracker(); });
  EXPECT_GE(SentToTracker(GNT_GAMEUPDATE_TINY).size(), 2u);
}

TEST_F(GameTracker, AckedGameIsOnlyRefreshedPeriodically) {
  d3_net_game_data_tiny g = MakeTinyGame("#my_lobby");
  StartTrackerGame(&g);
  Deliver(MakePacket(GNT_SERVER_ACK));
  mt_test::sent.clear();

  AdvanceAndPoll(TRACKER_UPDATE_INTERVAL / 2, [] { IdleGameTracker(); });
  EXPECT_EQ(SentToTracker(GNT_GAMEUPDATE_TINY).size(), 0u) << "acknowledged update must not be resent";

  AdvanceAndPoll(TRACKER_UPDATE_INTERVAL, [] { IdleGameTracker(); });
  EXPECT_GE(SentToTracker(GNT_GAMEUPDATE_TINY).size(), 1u) << "running game must be refreshed";
}

TEST_F(GameTracker, EndedGameIsNoLongerRefreshed) {
  d3_net_game_data_tiny g = MakeTinyGame("#my_lobby");
  StartTrackerGame(&g);
  SendGameOver();
  Deliver(MakePacket(GNT_SERVER_ACK));
  SendGameOver();
  mt_test::sent.clear();

  AdvanceAndPoll(TRACKER_UPDATE_INTERVAL * 2, [] { IdleGameTracker(); });
  EXPECT_EQ(SentToTracker(GNT_GAMEUPDATE_TINY).size(), 0u);
}

TEST_F(GameTracker, EveryTrackerPacketIsAcknowledgedWithItsSignature) {
  game_packet_header p = MakePacket(GNT_SERVER_ACK);
  p.sig = 0xFEEDF00D;
  Deliver(p);

  auto acks = SentToTracker(GNT_CLIENT_ACK);
  ASSERT_EQ(acks.size(), 1u);
  EXPECT_EQ(mt_test::Unpacked(acks[0].header.sig), 0xFEEDF00Du);
}

TEST_F(GameTracker, HolePunchRequestIsAnsweredAtTheRequestedAddress) {
  hole_punch_addr target;
  const uint8_t ip[4] = {10, 1, 2, 3};
  memcpy(&target.addr, ip, sizeof(ip));
  target.port = htons(4567); // network byte order, as in a sockaddr
  Deliver(MakePacket(GNT_NAT_HOLE_PUNCH_REQ, &target, sizeof(target)));

  auto punches = SentToTracker(GNT_NAT_HOLE_PUNCH_ACK);
  ASSERT_EQ(punches.size(), 1u);
  EXPECT_EQ(memcmp(punches[0].to.address, ip, sizeof(ip)), 0);
  EXPECT_EQ(punches[0].to.port, 4567);
}

TEST_F(GameTracker, Ipv6HolePunchRequestIsNotMisreadAsIpv4) {
  hole_punch_addr_ip6 target;
  memset(&target, 0, sizeof(target));
  target.addr[0] = 0x20;
  target.addr[1] = 0x01;
  target.port = htons(4567);
  Deliver(MakePacket(GNT_NAT_HOLE_PUNCH_REQ, &target, sizeof(target)));

  EXPECT_TRUE(SentToTracker(GNT_NAT_HOLE_PUNCH_ACK).empty());
}

} // namespace
