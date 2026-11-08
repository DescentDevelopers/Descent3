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

#include "ranking_contract.h"

#include <cstring>
#include <set>

void ScoreboardRanking::SetUp() {
  // The real DMFC implementation owns the player records; it can only be created once per process.
  if (!DMFCBase)
    DMFCBase = CreateDMFC();
  ASSERT_NE(DMFCBase, nullptr);
  for (int i = 0; i < MAX_PLAYER_RECORDS; i++) {
    player_record *pr = DMFCBase->GetPlayerRecord(i);
    ASSERT_NE(pr, nullptr);
    memset(pr, 0, sizeof(*pr));
    pr->state = STATE_EMPTY;
    GetParam().reset_slot(pr, i);
  }
}

void ScoreboardRanking::Occupy(int slot, slot_state state, int score) {
  player_record *pr = DMFCBase->GetPlayerRecord(slot);
  pr->state = state;
  GetParam().set_level_score(pr, score);
}

void ScoreboardRanking::ExpectFullLeaderboardOrder(const std::function<void(int *sortedindex, int size)> &sort) {
  InGame(5, 3);
  Disconnected(9, 50);
  InGame(2, 10);
  InGame(7, -1);
  Disconnected(12, 4);

  int sorted[MAX_PLAYER_RECORDS];
  for (int &s : sorted)
    s = -1;
  sort(sorted, MAX_PLAYER_RECORDS);

  // In-game players by score, then disconnected players by score.
  EXPECT_EQ(sorted[0], 2);
  EXPECT_EQ(sorted[1], 5);
  EXPECT_EQ(sorted[2], 7);
  EXPECT_EQ(sorted[3], 9);
  EXPECT_EQ(sorted[4], 12);

  std::set<int> seen;
  for (int i = 0; i < MAX_PLAYER_RECORDS; i++) {
    ASSERT_GE(sorted[i], 0);
    ASSERT_LT(sorted[i], MAX_PLAYER_RECORDS);
    EXPECT_TRUE(seen.insert(sorted[i]).second) << "slot " << sorted[i] << " listed twice";
    if (i >= 5) {
      EXPECT_EQ(DMFCBase->GetPlayerRecord(sorted[i])->state, STATE_EMPTY) << "position " << i;
    }
  }
}

TEST_P(ScoreboardRanking, HigherLevelScoreIsListedFirst) {
  InGame(0, 3);
  InGame(1, 7);
  EXPECT_TRUE(ListedBelow(0, 1));
  EXPECT_FALSE(ListedBelow(1, 0));
}

TEST_P(ScoreboardRanking, NegativeScoresRankBelowZeroAndPositive) {
  InGame(0, -4);
  InGame(1, 0);
  InGame(2, 2);
  EXPECT_TRUE(ListedBelow(0, 1));
  EXPECT_TRUE(ListedBelow(0, 2));
  EXPECT_FALSE(ListedBelow(1, 0));
  EXPECT_FALSE(ListedBelow(2, 0));
}

TEST_P(ScoreboardRanking, PlayerInGameIsListedAboveDisconnectedPlayerWithHigherScore) {
  InGame(0, 1);
  Disconnected(1, 50);
  EXPECT_TRUE(ListedBelow(1, 0));
  EXPECT_FALSE(ListedBelow(0, 1));
}

TEST_P(ScoreboardRanking, DisconnectedPlayersAreListedByScore) {
  Disconnected(0, 2);
  Disconnected(1, 9);
  EXPECT_TRUE(ListedBelow(0, 1));
  EXPECT_FALSE(ListedBelow(1, 0));
}

TEST_P(ScoreboardRanking, EmptySlotIsListedBelowEveryOccupiedSlot) {
  // slot 0 stays empty
  InGame(1, -10);
  Disconnected(2, -10);
  EXPECT_TRUE(ListedBelow(0, 1));
  EXPECT_TRUE(ListedBelow(0, 2));
  EXPECT_FALSE(ListedBelow(1, 0));
  EXPECT_FALSE(ListedBelow(2, 0));
}

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(ScoreboardRankingByModeScore);

TEST_P(ScoreboardRankingByModeScore, ModeScoreNotKillCountDecidesRank) {
  InGame(0, 1);
  InGame(1, 2);
  DMFCBase->GetPlayerRecord(0)->dstats.kills[DSTAT_LEVEL] = 50;
  DMFCBase->GetPlayerRecord(1)->dstats.kills[DSTAT_LEVEL] = 0;
  EXPECT_TRUE(ListedBelow(0, 1));
  EXPECT_FALSE(ListedBelow(1, 0));
}

GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(ScoreboardRankingByKillsMinusSuicides);

TEST_P(ScoreboardRankingByKillsMinusSuicides, SuicidesCountAgainstScore) {
  InGame(0, 0);
  InGame(1, 0);
  player_record *a = DMFCBase->GetPlayerRecord(0);
  player_record *b = DMFCBase->GetPlayerRecord(1);
  a->dstats.kills[DSTAT_LEVEL] = 10;
  a->dstats.suicides[DSTAT_LEVEL] = 8; // net 2
  b->dstats.kills[DSTAT_LEVEL] = 5;
  b->dstats.suicides[DSTAT_LEVEL] = 0; // net 5
  EXPECT_TRUE(ListedBelow(0, 1));
  EXPECT_FALSE(ListedBelow(1, 0));
}

TEST_P(ScoreboardRankingByKillsMinusSuicides, BeingKilledByOthersDoesNotCostScore) {
  InGame(0, 5);
  InGame(1, 4);
  DMFCBase->GetPlayerRecord(0)->dstats.deaths[DSTAT_LEVEL] = 20;
  EXPECT_TRUE(ListedBelow(1, 0));
  EXPECT_FALSE(ListedBelow(0, 1));
}
