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
#include <map>
#include <vector>

#include <gtest/gtest.h>

#include "gamedll_header.h"
#include "DMFC.h"
#include "dmfcinternal.h"

#include "support/pinfo_util.h"

// PInfo tracks, for one player record, how many times each other player record killed it ("killer list").

using namespace dmfc_test;

TEST(PInfo, StartsWithOnlyASuicideEntryForItsOwnSlot) {
  PInfo info(4);

  EXPECT_EQ(Killers(info), (std::map<int, int>{{4, 0}}));
  EXPECT_EQ(info.GetKillerInfo(5), nullptr);
}

TEST(PInfo, UpdateAccumulatesKillsPerSlot) {
  PInfo info(0);

  AddKills(info, 5, 2);
  AddKills(info, 9, 1);
  AddKills(info, 5, 3);
  AddKills(info, 0, 1);

  EXPECT_EQ(Killers(info), (std::map<int, int>{{0, 1}, {5, 5}, {9, 1}}));
  ASSERT_NE(info.GetKillerInfo(5), nullptr);
  EXPECT_EQ(info.GetKillerInfo(5)->kills, 5);
}

TEST(PInfo, RemoveKillerRemovesOnlyThatSlot) {
  PInfo info(0);
  AddKills(info, 3, 1);
  AddKills(info, 7, 2);
  AddKills(info, 11, 3);

  info.RemoveKiller(7);
  EXPECT_EQ(Killers(info), (std::map<int, int>{{0, 0}, {3, 1}, {11, 3}}));

  // removing the head of the list (the player's own entry) must keep the rest reachable
  info.RemoveKiller(0);
  EXPECT_EQ(Killers(info), (std::map<int, int>{{3, 1}, {11, 3}}));

  info.RemoveKiller(42);
  EXPECT_EQ(Killers(info), (std::map<int, int>{{3, 1}, {11, 3}}));
}

TEST(PInfo, PackUnpackRoundTrip) {
  PInfo source(2);
  AddKills(source, 2, 1);
  AddKills(source, 6, 4);
  AddKills(source, 63, 100000);

  tPExtraInfo *src_extra = source.GetExtraInfo();
  src_extra->last_kill_time = 12.5f;
  src_extra->last_death_time = 3.25f;
  src_extra->kill_time = 40.75f;
  src_extra->death_time = 33.0f;
  src_extra->observer_time = 7.5f;
  src_extra->kills_in_a_row = 3;
  src_extra->deaths_in_a_row = 65000;
  src_extra->last_kill_num = 6;
  src_extra->last_death_num = -1;
  src_extra->got_revenge = 1;

  const int size = source.GetSizeOfData();
  const uint8_t sentinel = 0xCD;
  std::vector<uint8_t> buffer(size + 16, sentinel);
  source.PackData(buffer.data());
  for (size_t i = size; i < buffer.size(); i++)
    ASSERT_EQ(buffer[i], sentinel) << "PackData wrote past GetSizeOfData() at byte " << i;

  // unpack into a PInfo that already has unrelated state; it must be replaced, not merged
  PInfo dest(9);
  AddKills(dest, 17, 5);
  ASSERT_TRUE(dest.UnpackData(buffer.data(), size));

  EXPECT_EQ(Killers(dest), Killers(source));

  const tPExtraInfo *dst_extra = dest.GetExtraInfo();
  EXPECT_FLOAT_EQ(dst_extra->last_kill_time, 12.5f);
  EXPECT_FLOAT_EQ(dst_extra->last_death_time, 3.25f);
  EXPECT_FLOAT_EQ(dst_extra->kill_time, 40.75f);
  EXPECT_FLOAT_EQ(dst_extra->death_time, 33.0f);
  EXPECT_FLOAT_EQ(dst_extra->observer_time, 7.5f);
  EXPECT_EQ(dst_extra->kills_in_a_row, 3);
  EXPECT_EQ(dst_extra->deaths_in_a_row, 65000);
  EXPECT_EQ(dst_extra->last_kill_num, 6);
  EXPECT_EQ(dst_extra->last_death_num, -1);
  EXPECT_EQ(dst_extra->got_revenge, 1);
}

TEST(PInfo, PackUnpackRoundTripWithEmptyKillerList) {
  PInfo source(1);
  source.ResetAll();
  source.GetExtraInfo()->kills_in_a_row = 2;
  ASSERT_TRUE(Killers(source).empty());

  const int size = source.GetSizeOfData();
  std::vector<uint8_t> buffer(size + 16, 0xCD);
  source.PackData(buffer.data());

  PInfo dest(5);
  AddKills(dest, 8, 1);
  ASSERT_TRUE(dest.UnpackData(buffer.data(), size));

  EXPECT_TRUE(Killers(dest).empty());
  EXPECT_EQ(dest.GetExtraInfo()->kills_in_a_row, 2);
}
