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

// Ranking hooks for modes that keep score in their own per-player tPlayerStat::Score[] (attached to
// each player record as user_info) instead of DMFC's kill count. Include after the mode's source.

#pragma once

// Normally allocated by DMFC (SetupPlayerRecord); the tests own them instead.
static tPlayerStat g_mode_stats[MAX_PLAYER_RECORDS];

static void ResetModeSlot(player_record *pr, int slot) {
  g_mode_stats[slot] = tPlayerStat{};
  pr->user_info = &g_mode_stats[slot];
}

static void SetModeLevelScore(player_record *pr, int score) {
  static_cast<tPlayerStat *>(pr->user_info)->Score[DSTAT_LEVEL] = score;
}
