/*
 * Descent 3
 * Copyright (C) 2024 Descent Developers
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

#include <chrono>
#include <thread>
#include <gtest/gtest.h>

#include "chrono_timer.h"

using D3::ChronoTimer;
using std::chrono::steady_clock;

class MockTimer : public ::testing::Test {
protected:
  MockTimer() { ChronoTimer::Initialize(); }
};

// Runs the clock as fast as possible instead of keeping pace with the wall clock
class MockTimerUnpaced : public MockTimer {
protected:
  MockTimerUnpaced() { ChronoTimer::SetPaced(false); }
  ~MockTimerUnpaced() override { ChronoTimer::SetPaced(true); }
};

TEST_F(MockTimerUnpaced, DoesNotAdvanceOnItsOwn) {
  ChronoTimer::SleepMS(5);
  for (int i = 0; i < 1000; i++) {
    EXPECT_EQ(ChronoTimer::GetTimeUS(), 5000);
  }
}

TEST_F(MockTimerUnpaced, InitializeResets) {
  ChronoTimer::SleepMS(5);
  ChronoTimer::Initialize();
  EXPECT_EQ(ChronoTimer::GetTimeUS(), 0);

  // The sleep before Initialize must not stop the next pump from stepping
  ChronoTimer::Pump();
  EXPECT_EQ(ChronoTimer::GetTimeMS(), ChronoTimer::kFixedStepMS);
}

TEST_F(MockTimerUnpaced, SleepIsInstant) {
  auto real_start = steady_clock::now();
  ChronoTimer::SleepMS(10000);
  EXPECT_LT(steady_clock::now() - real_start, std::chrono::seconds(1));
  EXPECT_EQ(ChronoTimer::GetTimeMS(), 10000);
}

TEST_F(MockTimerUnpaced, SleepsAccumulate) {
  ChronoTimer::SleepMS(3);
  ChronoTimer::SleepMS(4);
  EXPECT_EQ(ChronoTimer::GetTimeMS(), 7);
}

TEST_F(MockTimerUnpaced, NonPositiveSleepDoesNothing) {
  ChronoTimer::SleepMS(5);
  ChronoTimer::Pump();
  ChronoTimer::SleepMS(0);
  ChronoTimer::SleepMS(-3);
  EXPECT_EQ(ChronoTimer::GetTimeMS(), 5);

  // ...and does not count as advancing the clock, so the next pump still steps
  ChronoTimer::Pump();
  EXPECT_EQ(ChronoTimer::GetTimeMS(), 5 + ChronoTimer::kFixedStepMS);
}

TEST_F(MockTimerUnpaced, PumpStepsWhenIdle) {
  const int64_t step_us = ChronoTimer::kFixedStepMS * 1000;

  ChronoTimer::Pump();
  EXPECT_EQ(ChronoTimer::GetTimeUS(), step_us);

  ChronoTimer::Pump();
  EXPECT_EQ(ChronoTimer::GetTimeUS(), 2 * step_us);
}

TEST_F(MockTimerUnpaced, PumpDoesNotStepAfterSleep) {
  ChronoTimer::SleepMS(5);
  ChronoTimer::Pump();
  EXPECT_EQ(ChronoTimer::GetTimeMS(), 5);

  // ...but the following pump with no sleep in between does
  ChronoTimer::Pump();
  EXPECT_EQ(ChronoTimer::GetTimeMS(), 5 + ChronoTimer::kFixedStepMS);
}

// Simulates the game loop: the framecap sleeps out the rest of the frame, then defer() pumps.
TEST_F(MockTimerUnpaced, FrameCapLoopGivesFixedFrametime) {
  const int step = ChronoTimer::kFixedStepMS;
  int64_t last = ChronoTimer::GetTimeMS();
  for (int frame = 0; frame < 100; frame++) {
    // Some frames sleep for part of the frame on their own
    if (frame % 3 == 0) {
      ChronoTimer::SleepMS(3);
    }

    int64_t elapsed = ChronoTimer::GetTimeMS() - last;
    if (elapsed < step) {
      ChronoTimer::SleepMS(step - static_cast<int>(elapsed));
    }

    int64_t now = ChronoTimer::GetTimeMS();
    EXPECT_EQ(now - last, step);
    last = now;

    ChronoTimer::Pump();
  }
}

// The pacing tests measure real time, so their upper bounds are generous guesses at how long the test's own work
// takes. They could fail spuriously on a badly stalled machine.
TEST_F(MockTimer, PacedSleepWaitsForRealTime) {
  auto real_start = steady_clock::now();
  ChronoTimer::SleepMS(50);
  auto real_elapsed = steady_clock::now() - real_start;

  EXPECT_GE(real_elapsed, std::chrono::milliseconds(49));
  EXPECT_LT(real_elapsed, std::chrono::milliseconds(500));
  EXPECT_EQ(ChronoTimer::GetTimeMS(), 50);
}

TEST_F(MockTimer, PacedClockDropsDebtWhenBehind) {
  // Fall 100ms behind the wall clock without advancing mock time
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // Already behind, so this returns at once...
  auto real_start = steady_clock::now();
  ChronoTimer::SleepMS(20);
  EXPECT_LT(steady_clock::now() - real_start, std::chrono::milliseconds(15));

  // ...and the debt is forgotten, so the next sleep waits its full length instead of rushing to catch up
  real_start = steady_clock::now();
  ChronoTimer::SleepMS(20);
  EXPECT_GE(steady_clock::now() - real_start, std::chrono::milliseconds(19));
}
