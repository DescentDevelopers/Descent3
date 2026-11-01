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

// Mock ChronoTimer for reproducible test runs (ENABLE_MOCK_CLOCK). Time starts at zero and only moves when
// the game sleeps or pumps, so every game frame is exactly kFixedStepMS long no matter how long it took to compute.
// Advancing also waits for the wall clock to catch up, so the game still plays at normal speed. Only the unit tests
// turn that off (skip_realtime_), to run the clock as fast as possible.
//
// Only the main thread moves time. Background threads that sleep wait for the main thread to move time past their
// wake-up point. See the threading rules in chrono_timer.h.

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <thread>

#include "chrono_timer.h"
#include "mock_timer_internal.h"

namespace D3 {

namespace ChronoTimerInternals {
  std::atomic<std::thread::id> main_thread_;
  bool skip_realtime_;
}
using namespace ChronoTimerInternals;

namespace {
  std::atomic<int64_t> time_us_;
  
  bool slept_since_pump_;
  std::chrono::steady_clock::time_point wall_anchor_;

  std::mutex sleepers_mutex_;
  std::condition_variable sleepers_cv_;

  bool OnMainThread(const char *function) {
    std::thread::id const main_thread = main_thread_;
    if (main_thread == std::thread::id{}) {
      std::fprintf(stderr, "ChronoTimer::%s was called before ChronoTimer::Initialize\n", function);
      std::abort();
    }
    return main_thread == std::this_thread::get_id();
  }

  void Reanchor() {
    wall_anchor_ = std::chrono::steady_clock::now() - std::chrono::microseconds(time_us_);
  }

  void Advance(int64_t us) {
    auto new_time = [us] {
      std::lock_guard<std::mutex> lock{sleepers_mutex_};
      return std::chrono::microseconds{time_us_ += us};
    }();
    sleepers_cv_.notify_all();

    if (skip_realtime_) {
      return;
    }

    auto const target = wall_anchor_ + new_time;
    if (std::chrono::steady_clock::now() < target) {
      std::this_thread::sleep_until(target);
    } else {
      Reanchor(); // Running behind; drop the debt rather than rushing later frames to catch up
    }
  }
} // namespace (anonymous)

void ChronoTimer::Initialize() {
  std::thread::id expected{};
  if (!main_thread_.compare_exchange_strong(expected, std::this_thread::get_id())) {
    std::fprintf(stderr, "ChronoTimer double-initialized\n");
    std::abort();
  }

  time_us_ = 0;
  slept_since_pump_ = false;
  Reanchor();
}

float ChronoTimer::GetTime() {
  OnMainThread("GetTime");
  return time_us_ / 1'000'000.0f;
}

int64_t ChronoTimer::GetTimeMS() {
  OnMainThread("GetTimeMS");
  return time_us_ / 1000;
}

int64_t ChronoTimer::GetTimeUS() {
  OnMainThread("GetTimeUS");
  return time_us_;
}

void ChronoTimer::SleepMS(int delay) {
  // Like the real clock, a sleep of zero or less returns at once, rather than moving time backwards
  if (delay <= 0) {
    return;
  }

  const int64_t delay_us = static_cast<int64_t>(delay) * 1000;

  if (OnMainThread("SleepMS")) {
    Advance(delay_us);
    slept_since_pump_ = true;
  } else {
    // wait for the main thread to move time forward
    std::unique_lock<std::mutex> lock(sleepers_mutex_);
    sleepers_cv_.wait(lock, [wake_us = time_us_ + delay_us] { return time_us_ >= wake_us; });
  }
}

void ChronoTimer::Pump() {
  if (!OnMainThread("Pump")) {
    std::fprintf(stderr, "ChronoTimer::Pump was called from a thread other than the main thread\n");
    std::abort();
  }

  // Only step if nothing else slept since the previous pump, so a game frame (which sleeps out its
  // framecap) is not lengthened, while loops that wait on the timer without sleeping still make progress.
  if (!slept_since_pump_) {
    Advance(static_cast<int64_t>(kFixedStepMS) * 1000);
  }
  slept_since_pump_ = false;
}

} // namespace D3
