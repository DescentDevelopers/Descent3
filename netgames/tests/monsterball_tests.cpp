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

#include "../monsterball/monsterball.cpp"

#include "support/mode_score_hooks.h"
#include "support/ranking_contract.h"

const RankingMode kMonsterballRanking{compare_slots, ResetModeSlot, SetModeLevelScore};

INSTANTIATE_TEST_SUITE_P(Monsterball, ScoreboardRanking, testing::Values(kMonsterballRanking));
INSTANTIATE_TEST_SUITE_P(Monsterball, ScoreboardRankingByModeScore, testing::Values(kMonsterballRanking));

TEST_P(ScoreboardRanking, SortPlayerSlotsProducesFullLeaderboard) { ExpectFullLeaderboardOrder(SortPlayerSlots); }

TEST(MonsterballTeamStandings, SortTeamsListsTeamsByDescendingScore) {
  TeamScores[0] = 10;
  TeamScores[1] = 40;
  TeamScores[2] = -5;
  TeamScores[3] = 25;
  SortTeams();
  EXPECT_EQ(SortedTeams[0], 1);
  EXPECT_EQ(SortedTeams[1], 3);
  EXPECT_EQ(SortedTeams[2], 0);
  EXPECT_EQ(SortedTeams[3], 2);
}

TEST(MonsterballPlayerStatWireFormat, UnpackRestoresEveryPackedField) {
  tPlayerStat in{};
  in.Score[0] = 4;
  in.Score[1] = 19;
  in.BadScore[0] = -2;
  in.BadScore[1] = 65536;

  uint8_t buffer[64];
  memset(buffer, 0xCD, sizeof(buffer));
  int written = pack_pstat(&in, buffer);

  tPlayerStat out;
  memset(&out, 0xAB, sizeof(out));
  int read = unpack_pstat(&out, buffer);

  EXPECT_EQ(read, written);
  EXPECT_EQ(out.Score[0], 4);
  EXPECT_EQ(out.Score[1], 19);
  EXPECT_EQ(out.BadScore[0], -2);
  EXPECT_EQ(out.BadScore[1], 65536);
  // DMFC budgets packet space with the size passed to SetupPlayerRecord, sizeof(tPlayerStat).
  EXPECT_LE(written, static_cast<int>(sizeof(tPlayerStat)));
}
