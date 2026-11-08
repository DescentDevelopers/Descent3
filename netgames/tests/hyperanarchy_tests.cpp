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

#include "../hyperanarchy/hyperanarchy.cpp"

#include "support/mode_score_hooks.h"
#include "support/ranking_contract.h"

const RankingMode kHyperAnarchyRanking{compare_slots, ResetModeSlot, SetModeLevelScore};

INSTANTIATE_TEST_SUITE_P(HyperAnarchy, ScoreboardRanking, testing::Values(kHyperAnarchyRanking));
INSTANTIATE_TEST_SUITE_P(HyperAnarchy, ScoreboardRankingByModeScore, testing::Values(kHyperAnarchyRanking));

TEST_P(ScoreboardRanking, SortPlayerScoresProducesFullLeaderboard) { ExpectFullLeaderboardOrder(SortPlayerScores); }
