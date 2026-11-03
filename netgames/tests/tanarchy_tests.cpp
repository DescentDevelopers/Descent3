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

#include "../tanarchy/tanarchy.cpp"

#include "support/anarchy_score_hooks.h"
#include "support/ranking_contract.h"

const RankingMode kTeamAnarchyRanking{compare_slots, ResetModeSlot, SetModeLevelScore};

INSTANTIATE_TEST_SUITE_P(TeamAnarchy, ScoreboardRanking, testing::Values(kTeamAnarchyRanking));
INSTANTIATE_TEST_SUITE_P(TeamAnarchy, ScoreboardRankingByKillsMinusSuicides, testing::Values(kTeamAnarchyRanking));

TEST(TeamAnarchyStandings, SortTeamScoresListsLeaderFirst) {
  int sorted[NUM_TEAMS];

  int blue_leads[NUM_TEAMS] = {3, 9};
  SortTeamScores(sorted, blue_leads);
  EXPECT_EQ(sorted[0], BLUE_TEAM);
  EXPECT_EQ(sorted[1], RED_TEAM);

  int red_leads[NUM_TEAMS] = {9, 3};
  SortTeamScores(sorted, red_leads);
  EXPECT_EQ(sorted[0], RED_TEAM);
  EXPECT_EQ(sorted[1], BLUE_TEAM);
}
