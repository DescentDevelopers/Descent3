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

// Mock ChronoTimer for reproducible test runs (ENABLE_MOCK_CLOCK). Time starts at zero and only moves when
// the game sleeps or pumps, so every game frame is exactly kFixedStepMS long no matter how long it took to compute.
// When paced, advancing also waits for the wall clock to catch up, so the game still plays at normal speed.

#include <atomic>
#include <chrono>
#include <thread>

#include "chrono_timer.h"

namespace D3 {

static std::atomic<int64_t> s_time_us{0};
static std::atomic<bool> s_advanced{false};
static bool s_paced = true;
static std::chrono::steady_clock::time_point s_wall_anchor = std::chrono::steady_clock::now();

// Re-anchor so that the current mock time corresponds to now on the wall clock
static void Reanchor() { s_wall_anchor = std::chrono::steady_clock::now() - std::chrono::microseconds(s_time_us); }

static void Advance(int64_t us) {
  s_time_us += us;
  if (!s_paced) {
    return;
  }

  auto target = s_wall_anchor + std::chrono::microseconds(s_time_us);
  if (std::chrono::steady_clock::now() < target) {
    std::this_thread::sleep_until(target);
  } else {
    Reanchor(); // Running behind; drop the debt rather than rushing later frames to catch up
  }
}

// Like the real clock, restarts time from zero
void ChronoTimer::Initialize() {
  s_time_us = 0;
  s_advanced = false;
  Reanchor();
}

float ChronoTimer::GetTime() { return s_time_us / 1'000'000.0f; }

int64_t ChronoTimer::GetTimeMS() { return s_time_us / 1000; }

int64_t ChronoTimer::GetTimeUS() { return s_time_us; }

void ChronoTimer::SleepMS(int delay) {
  // Like the real clock, a sleep of zero or less returns at once, rather than moving time backwards
  if (delay <= 0) {
    return;
  }

  s_advanced = true;
  Advance(static_cast<int64_t>(delay) * 1000);
}

void ChronoTimer::Pump() {
  // Only step if nothing else moved the clock since the previous pump, so a game frame (which sleeps out its
  // framecap) is not lengthened, while loops that wait on the timer without sleeping still make progress.
  if (!s_advanced.exchange(false)) {
    Advance(static_cast<int64_t>(kFixedStepMS) * 1000);
  }
}

void ChronoTimer::SetPaced(bool paced) {
  s_paced = paced;
  if (paced) {
    Reanchor();
  }
}

} // namespace D3
