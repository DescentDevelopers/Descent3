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

// Anarchy-style scoring: a player's score is kills minus suicides (see DetermineScore).

#pragma once

static void ResetModeSlot(player_record *, int) {}
static void SetModeLevelScore(player_record *pr, int score) {
  pr->dstats.kills[DSTAT_LEVEL] = score;
  pr->dstats.suicides[DSTAT_LEVEL] = 0;
}
