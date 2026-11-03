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

#include "../hoard/hoard.cpp"

#include "support/mode_score_hooks.h"
#include "support/ranking_contract.h"

const RankingMode kHoardRanking{compare_slots, ResetModeSlot, SetModeLevelScore};

INSTANTIATE_TEST_SUITE_P(Hoard, ScoreboardRanking, testing::Values(kHoardRanking));
INSTANTIATE_TEST_SUITE_P(Hoard, ScoreboardRankingByModeScore, testing::Values(kHoardRanking));

TEST_P(ScoreboardRanking, SortPlayerScoresProducesFullLeaderboard) { ExpectFullLeaderboardOrder(SortPlayerScores); }

TEST(HoardPlayerStatWireFormat, UnpackRestoresEveryPackedField) {
  tPlayerStat in{};
  in.Score[0] = 7;
  in.Score[1] = -3;
  in.HighestScore[0] = 12;
  in.HighestScore[1] = 100000;

  uint8_t buffer[64];
  memset(buffer, 0xCD, sizeof(buffer));
  int written = pack_pstat(&in, buffer);

  tPlayerStat out;
  memset(&out, 0xAB, sizeof(out));
  int read = unpack_pstat(&out, buffer);

  EXPECT_EQ(read, written);
  EXPECT_EQ(out.Score[0], 7);
  EXPECT_EQ(out.Score[1], -3);
  EXPECT_EQ(out.HighestScore[0], 12);
  EXPECT_EQ(out.HighestScore[1], 100000);
  // DMFC budgets packet space with the size passed to SetupPlayerRecord, sizeof(tPlayerStat).
  EXPECT_LE(written, static_cast<int>(sizeof(tPlayerStat)));
}
