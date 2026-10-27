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

#include <gtest/gtest.h>

#include "chrono_timer.h"

using D3::ChronoTimer;

// Watches the clock tick forward for 10ms, so it sees many ticks however fast the machine is. Back-to-back reads
// often land in the same microsecond, so each read only has to be no earlier than the last.
TEST(ChronoTimer, IsMonotonic) {
  ChronoTimer::Initialize();
  const int64_t start = ChronoTimer::GetTimeUS();
  int64_t prev = start;
  int ticks = 0;
  while (prev - start < 10000) {
    int64_t now = ChronoTimer::GetTimeUS();
    ASSERT_GE(now, prev);
    if (now > prev) {
      ticks++;
    }
    prev = now;
  }
  EXPECT_GT(ticks, 1);
}

TEST(ChronoTimer, SleepAdvancesTime) {
  ChronoTimer::Initialize();
  int64_t start = ChronoTimer::GetTimeMS();
  ChronoTimer::SleepMS(20);
  EXPECT_GE(ChronoTimer::GetTimeMS() - start, 20);
}
