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

#include <cstdint>
#include <cstring>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "support/dmfc_builders.h"
#include "support/dmfc_fakes.h"
#include "support/dmfc_internals.h"
#include "support/dmfc_test_support.h"

using namespace dmfc_test;

namespace {

class PlayerRecordTest : public DMFCFixture {
protected:
  void SetUp() override {
    DMFCFixture::SetUp();
    PRec_Init();
  }
  void TearDown() override {
    PRec_Close();
    DMFCFixture::TearDown();
  }
};

} // namespace

TEST_F(PlayerRecordTest, FindPlayerOnlyMatchesDisconnectedRecordsWithSameCallsign) {
  OccupyRecord(3, STATE_INGAME, "Alice");
  OccupyRecord(5, STATE_DISCONNECTED, "Bob");
  EXPECT_EQ(PRec_FindPlayer("Alice", nullptr, nullptr), -1);

  OccupyRecord(7, STATE_DISCONNECTED, "Alice");
  EXPECT_EQ(PRec_FindPlayer("Alice", nullptr, nullptr), 7);
  EXPECT_EQ(PRec_FindPlayer("Bob", nullptr, nullptr), 5);
  EXPECT_EQ(PRec_FindPlayer("Carol", nullptr, nullptr), -1);
  EXPECT_EQ(PRec_FindPlayer(nullptr, nullptr, nullptr), -1);
}

TEST_F(PlayerRecordTest, FindPlayerComparesAddressIgnoringPort) {
  player_record *pr = OccupyRecord(4, STATE_DISCONNECTED, "Alice");
  pr->net_addr = MakeAddress(10, 0, 0, 1, 2092);

  network_address same_host_new_port = MakeAddress(10, 0, 0, 1, 5555);
  network_address other_host = MakeAddress(10, 0, 0, 2, 2092);

  EXPECT_EQ(PRec_FindPlayer("Alice", &same_host_new_port, nullptr), 4);
  EXPECT_EQ(PRec_FindPlayer("Alice", &other_host, nullptr), -1);
}

TEST_F(PlayerRecordTest, FindPlayerRequiresMatchingTrackerIdWhenOneIsGiven) {
  player_record *with_id = OccupyRecord(2, STATE_DISCONNECTED, "Alice");
  with_id->tracker_id = HeapString("1234");
  OccupyRecord(6, STATE_DISCONNECTED, "Bob");

  EXPECT_EQ(PRec_FindPlayer("Alice", nullptr, "1234"), 2);
  EXPECT_EQ(PRec_FindPlayer("Alice", nullptr, "9999"), -1);
  EXPECT_EQ(PRec_FindPlayer("Bob", nullptr, "1234"), -1);
}

TEST_F(PlayerRecordTest, GetFreeSlotPrefersAnEmptySlot) {
  OccupyRecord(0, STATE_INGAME, "A");
  OccupyRecord(1, STATE_DISCONNECTED, "B")->disconnect_time = 1.0f;

  int slot = PRec_GetFreeSlot();
  ASSERT_GE(slot, 0);
  ASSERT_LT(slot, MAX_PLAYER_RECORDS);
  EXPECT_NE(slot, 0);
  EXPECT_NE(slot, 1);
  EXPECT_EQ(PRec_GetPRecord(slot)->state, STATE_EMPTY);
  EXPECT_STREQ(PRec_GetPRecord(1)->callsign, "B");
}

TEST_F(PlayerRecordTest, GetFreeSlotRecyclesLongestDisconnectedRecordWhenFull) {
  ASSERT_EQ(PRec_SetupUserPRec(16, PackDummy, PackDummy), 0);

  for (int i = 0; i < MAX_PLAYER_RECORDS; i++)
    OccupyRecord(i, STATE_INGAME, "InGame");
  OccupyRecord(10, STATE_DISCONNECTED, "Recent")->disconnect_time = 50.0f;
  OccupyRecord(20, STATE_DISCONNECTED, "Oldest")->disconnect_time = 5.0f;
  OccupyRecord(30, STATE_DISCONNECTED, "Middle")->disconnect_time = 20.0f;

  player_record *observer = PRec_GetPRecord(1);
  observer->pinfo = new PInfo(1);
  tPKillerInfo amount{};
  amount.kills = 2;
  observer->pinfo->Update(20, &amount);
  observer->pinfo->Update(10, &amount);
  PRec_GetPRecord(20)->pinfo = new PInfo(20);
  PRec_GetPRecord(20)->tracker_id = HeapString("555");

  void *user_stats_before = PRec_GetUserStats(20);
  ASSERT_NE(user_stats_before, nullptr);

  EXPECT_EQ(PRec_GetFreeSlot(), 20);

  player_record *recycled = PRec_GetPRecord(20);
  EXPECT_EQ(recycled->state, STATE_EMPTY);
  EXPECT_STREQ(recycled->callsign, "");
  EXPECT_EQ(recycled->pinfo, nullptr);
  EXPECT_EQ(recycled->tracker_id, nullptr);
  // the mod's user-stats storage for the slot stays allocated for the next occupant
  EXPECT_EQ(PRec_GetUserStats(20), user_stats_before);
  EXPECT_EQ(recycled->user_info_size, 16);

  EXPECT_EQ(observer->pinfo->GetKillerInfo(20), nullptr);
  EXPECT_NE(observer->pinfo->GetKillerInfo(10), nullptr);

  EXPECT_EQ(PRec_GetPRecord(10)->state, STATE_DISCONNECTED);
  EXPECT_EQ(PRec_GetPRecord(30)->state, STATE_DISCONNECTED);
}

TEST(PlayerRecordPacking, NetworkAddressRoundTrip) {
  network_address first = MakeAddress(192, 168, 1, 77, 2092);
  first.address[4] = 0xAB;
  first.address[5] = 0xCD;
  first.net_id[0] = 1;
  first.net_id[3] = 0xFE;
  network_address second = MakeAddress(10, 20, 30, 40, 65535);
  second.connection_type = NP_IPX;

  uint8_t buffer[64] = {};
  int write_count = 0;
  MultiPackNetworkAddress(&first, buffer, &write_count);
  MultiPackNetworkAddress(&second, buffer, &write_count);

  network_address out1{}, out2{};
  int read_count = 0;
  MultiUnpackNetworkAddress(&out1, buffer, &read_count);
  MultiUnpackNetworkAddress(&out2, buffer, &read_count);

  EXPECT_EQ(read_count, write_count);
  for (auto [in, out] : {std::pair{&first, &out1}, std::pair{&second, &out2}}) {
    EXPECT_EQ(memcmp(in->address, out->address, sizeof(in->address)), 0);
    EXPECT_EQ(memcmp(in->net_id, out->net_id, sizeof(in->net_id)), 0);
    EXPECT_EQ(in->port, out->port);
    EXPECT_EQ(in->connection_type, out->connection_type);
  }
}

TEST(PlayerRecordPacking, DStatsRoundTrip) {
  t_dstat first{{3, 120}, {4, 98}, {0, 7}};
  t_dstat second{{-1, 2147483647}, {5, 6}, {7, 8}};

  uint8_t buffer[128] = {};
  int write_count = 0;
  MultiPackDStats(&first, buffer, &write_count);
  MultiPackDStats(&second, buffer, &write_count);

  t_dstat out1{}, out2{};
  int read_count = 0;
  MultiUnpackDStats(&out1, buffer, &read_count);
  MultiUnpackDStats(&out2, buffer, &read_count);

  EXPECT_EQ(read_count, write_count);
  for (auto [in, out] : {std::pair{&first, &out1}, std::pair{&second, &out2}}) {
    for (int i = 0; i < 2; i++) {
      EXPECT_EQ(in->kills[i], out->kills[i]);
      EXPECT_EQ(in->deaths[i], out->deaths[i]);
      EXPECT_EQ(in->suicides[i], out->suicides[i]);
    }
  }
}
