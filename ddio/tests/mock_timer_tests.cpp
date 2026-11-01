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

#include <atomic>
#include <chrono>
#include <thread>
#include <gtest/gtest.h>
#include <utility>

#include "chrono_timer.h"
#include "mock_timer_internal.h"

using D3::ChronoTimer;
using std::chrono::steady_clock;

template <bool kSkipRealtime>
struct MockTimerFixture : testing::Test {
protected:
  MockTimerFixture()
    : skip_realtime_{std::exchange(D3::ChronoTimerInternals::skip_realtime_, kSkipRealtime)} {
    ChronoTimer::Initialize();
  }
  ~MockTimerFixture() override {
    D3::ChronoTimerInternals::main_thread_.store({});
    D3::ChronoTimerInternals::skip_realtime_ = skip_realtime_;
  }

private:
  bool const skip_realtime_;
};
using MockTimer = MockTimerFixture<false>;
using MockTimerFast = MockTimerFixture<true>;

TEST(Realtime, DefaultValue) {
  EXPECT_FALSE(D3::ChronoTimerInternals::skip_realtime_);
}

TEST_F(MockTimerFast, DoesNotAdvanceOnItsOwn) {
  ChronoTimer::SleepMS(5);
  for (int i = 0; i < 1000; i++) {
    EXPECT_EQ(ChronoTimer::GetTimeUS(), 5000);
  }
}

TEST_F(MockTimerFast, SleepIsInstant) {
  auto real_start = steady_clock::now();
  ChronoTimer::SleepMS(10000);
  EXPECT_LT(steady_clock::now() - real_start, std::chrono::seconds(1));
  EXPECT_EQ(ChronoTimer::GetTimeMS(), 10000);
}

TEST_F(MockTimerFast, SleepsAccumulate) {
  ChronoTimer::SleepMS(3);
  ChronoTimer::SleepMS(4);
  EXPECT_EQ(ChronoTimer::GetTimeMS(), 7);
}

TEST_F(MockTimerFast, NonPositiveSleepDoesNothing) {
  ChronoTimer::SleepMS(5);
  ChronoTimer::Pump();
  ChronoTimer::SleepMS(0);
  ChronoTimer::SleepMS(-3);
  EXPECT_EQ(ChronoTimer::GetTimeMS(), 5);

  // ...and does not count as advancing the clock, so the next pump still steps
  ChronoTimer::Pump();
  EXPECT_EQ(ChronoTimer::GetTimeMS(), 5 + ChronoTimer::kFixedStepMS);
}

TEST_F(MockTimerFast, PumpStepsWhenIdle) {
  const int64_t step_us = ChronoTimer::kFixedStepMS * 1000;

  ChronoTimer::Pump();
  EXPECT_EQ(ChronoTimer::GetTimeUS(), step_us);

  ChronoTimer::Pump();
  EXPECT_EQ(ChronoTimer::GetTimeUS(), 2 * step_us);
}

TEST_F(MockTimerFast, PumpDoesNotStepAfterSleep) {
  ChronoTimer::SleepMS(5);
  ChronoTimer::Pump();
  EXPECT_EQ(ChronoTimer::GetTimeMS(), 5);

  // ...but the following pump with no sleep in between does
  ChronoTimer::Pump();
  EXPECT_EQ(ChronoTimer::GetTimeMS(), 5 + ChronoTimer::kFixedStepMS);
}

// Simulates the game loop: the framecap sleeps out the rest of the frame, then defer() pumps.
TEST_F(MockTimerFast, FrameCapLoopGivesFixedFrametime) {
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

// These tests measure real time, so their upper bounds are generous guesses at how long the test's own work
// takes. They could fail spuriously on a badly stalled machine.
TEST_F(MockTimer, SleepWaitsForRealTime) {
  auto real_start = steady_clock::now();
  ChronoTimer::SleepMS(50);
  auto real_elapsed = steady_clock::now() - real_start;

  EXPECT_GE(real_elapsed, std::chrono::milliseconds(49));
  EXPECT_LT(real_elapsed, std::chrono::milliseconds(500));
  EXPECT_EQ(ChronoTimer::GetTimeMS(), 50);
}

TEST_F(MockTimer, DropsDebtWhenBehind) {
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

TEST_F(MockTimerFast, BackgroundSleepWaitsForMainThread) {
  std::atomic<int64_t> slept_us{-1};
  std::thread sleeper([&] {
    const int64_t start = ChronoTimer::GetTimeUS();
    ChronoTimer::SleepMS(50);
    slept_us = ChronoTimer::GetTimeUS() - start;
  });

  // While the main thread does not move time, the sleeper neither returns nor moves time itself
  std::this_thread::sleep_for(std::chrono::milliseconds(50));
  EXPECT_EQ(slept_us, -1);
  EXPECT_EQ(ChronoTimer::GetTimeUS(), 0);

  // Move time forward a little at a time until the sleeper wakes
  for (int i = 0; i < 1000 && slept_us < 0; i++) {
    ChronoTimer::SleepMS(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  if (slept_us < 0) {
    ADD_FAILURE() << "the background sleeper did not wake after 1000ms of game time";
    ChronoTimer::SleepMS(1'000'000); // Release it so the test does not hang
  }
  sleeper.join();

  EXPECT_GE(slept_us, 50'000);
}

// Death tests run in a fresh copy of the process. The "threadsafe" style re-runs the test binary, so the clock starts
// out uninitialized there, whatever the parent process has done.
class MockTimerDeathTest : public ::testing::Test {
protected:
  MockTimerDeathTest() { GTEST_FLAG_SET(death_test_style, "threadsafe"); }
};

TEST_F(MockTimerDeathTest, PumpFromBackgroundThreadAborts) {
  EXPECT_DEATH(
      {
        ChronoTimer::Initialize();
        std::thread([] { ChronoTimer::Pump(); }).join();
      },
      "ChronoTimer::Pump was called from a thread other than the main thread");
}

TEST_F(MockTimerDeathTest, SecondInitializeAborts) {
  EXPECT_DEATH(
      {
        ChronoTimer::Initialize();
        ChronoTimer::Initialize();
      },
      "ChronoTimer double-initialized");
}

TEST_F(MockTimerDeathTest, SecondInitializeAbortsBackground) {
  EXPECT_DEATH(
      {
        ChronoTimer::Initialize();
        std::thread([] { ChronoTimer::Initialize(); }).join();
      },
      "ChronoTimer double-initialized");
}

TEST_F(MockTimerDeathTest, PumpBeforeInitializeAborts) {
  EXPECT_DEATH(ChronoTimer::Pump(), "ChronoTimer::Pump was called before ChronoTimer::Initialize");
}

TEST_F(MockTimerDeathTest, SleepBeforeInitializeAborts) {
  EXPECT_DEATH(ChronoTimer::SleepMS(1), "ChronoTimer::SleepMS was called before ChronoTimer::Initialize");
}

TEST_F(MockTimerDeathTest, GetTimeBeforeInitializeAborts) {
  EXPECT_DEATH(ChronoTimer::GetTime(), "ChronoTimer::GetTime was called before ChronoTimer::Initialize");
}

TEST_F(MockTimerDeathTest, GetTimeMSBeforeInitializeAborts) {
  EXPECT_DEATH(ChronoTimer::GetTimeMS(), "ChronoTimer::GetTimeMS was called before ChronoTimer::Initialize");
}

TEST_F(MockTimerDeathTest, GetTimeUSBeforeInitializeAborts) {
  EXPECT_DEATH(ChronoTimer::GetTimeUS(), "ChronoTimer::GetTimeUS was called before ChronoTimer::Initialize");
}
