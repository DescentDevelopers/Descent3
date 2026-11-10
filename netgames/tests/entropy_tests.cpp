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

#include "../entropy/EntropyBase.cpp"

#include "support/mode_score_hooks.h"
#include "support/ranking_contract.h"
#include "support/vector_helpers.h"

const RankingMode kEntropyRanking{compare_slots, ResetModeSlot, SetModeLevelScore};

INSTANTIATE_TEST_SUITE_P(Entropy, ScoreboardRanking, testing::Values(kEntropyRanking));
INSTANTIATE_TEST_SUITE_P(Entropy, ScoreboardRankingByModeScore, testing::Values(kEntropyRanking));

TEST(EntropyTeamStandings, SortTeamScoresListsLeaderFirst) {
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

// The distance CompareDistanceTravel measures is an approximation, so these tests stay well clear
// of its threshold of a few units.
TEST(EntropyTakeoverStillness, NotMovingCountsAsStandingStill) {
  vector anchor = Vec(100, -20, 7);
  vector curr = anchor;
  EXPECT_TRUE(CompareDistanceTravel(&curr, &anchor));
  ExpectVecEq(anchor, Vec(100, -20, 7));
}

TEST(EntropyTakeoverStillness, SmallDriftCountsAsStandingStillAndKeepsAnchor) {
  vector anchor = Vec(0, 0, 0);
  vector curr = Vec(1, -1, 1);
  EXPECT_TRUE(CompareDistanceTravel(&curr, &anchor));
  ExpectVecEq(anchor, Vec(0, 0, 0));
}

TEST(EntropyTakeoverStillness, LargeMoveAlongAnyAxisOrDirectionResetsAnchor) {
  const vector moves[] = {Vec(10, 0, 0),  Vec(0, 10, 0),  Vec(0, 0, 10), Vec(-10, 0, 0),
                          Vec(0, -10, 0), Vec(0, 0, -10), Vec(8, -8, 8)};
  for (const vector &delta : moves) {
    vector anchor = Vec(50, 50, 50);
    vector curr = Vec(50 + delta.x(), 50 + delta.y(), 50 + delta.z());
    EXPECT_FALSE(CompareDistanceTravel(&curr, &anchor))
        << "delta " << delta.x() << "," << delta.y() << "," << delta.z();
    ExpectVecEq(anchor, curr);
  }
}

TEST(EntropyTakeoverStillness, CreepingAwayInSmallStepsIsMeasuredFromTheAnchor) {
  vector anchor = Vec(0, 0, 0);
  for (int x = 1; x <= 3; x++) {
    vector curr = Vec(static_cast<float>(x), 0, 0);
    EXPECT_TRUE(CompareDistanceTravel(&curr, &anchor)) << "x=" << x;
  }
  vector far_away = Vec(12, 0, 0);
  EXPECT_FALSE(CompareDistanceTravel(&far_away, &anchor));
  ExpectVecEq(anchor, far_away);

  vector near_new_anchor = Vec(13, 0, 0);
  EXPECT_TRUE(CompareDistanceTravel(&near_new_anchor, &anchor));
}
