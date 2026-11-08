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

// Ranking tests shared by every mode, instantiated once per mode with that mode's RankingMode.
// ScoreboardRanking holds the tests every mode passes; ScoreboardRankingByModeScore holds the ones
// that only apply to modes ranking by their own score (flags captured, orbs scored, ...) rather
// than by DMFC's kill count; ScoreboardRankingByKillsMinusSuicides holds the ones that only apply
// to modes ranking by DMFC's kills minus suicides.

#pragma once

#include <functional>

#include <gtest/gtest.h>

#include "idmfc.h"

extern IDMFC *DMFCBase;

struct RankingMode {
  // The mode's compare_slots(a, b): true when slot `a` must be listed *below* slot `b`.
  std::function<bool(int a, int b)> compare_slots;
  // Prepare a freshly zeroed record in `slot` (e.g. attach the mode's per-player stats struct).
  std::function<void(player_record *pr, int slot)> reset_slot;
  // Give the player a level score of `score` the way the mode keeps score.
  std::function<void(player_record *pr, int score)> set_level_score;
};

class ScoreboardRanking : public testing::TestWithParam<RankingMode> {
protected:
  void SetUp() override;

  void Occupy(int slot, slot_state state, int score);
  void InGame(int slot, int score) { Occupy(slot, STATE_INGAME, score); }
  void Disconnected(int slot, int score) { Occupy(slot, STATE_DISCONNECTED, score); }

  bool ListedBelow(int a, int b) const { return GetParam().compare_slots(a, b); }

  void ExpectFullLeaderboardOrder(const std::function<void(int *sortedindex, int size)> &sort);
};

class ScoreboardRankingByModeScore : public ScoreboardRanking {};

class ScoreboardRankingByKillsMinusSuicides : public ScoreboardRanking {};
