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

#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <thread>

#include "chrono_timer.h"
#include "chrono_timer_internal.h"

namespace D3 {

namespace ChronoTimerInternals {
  std::chrono::time_point<std::chrono::steady_clock> start_;
}
using namespace ChronoTimerInternals;

void ChronoTimer::Initialize() {
  if (start_ != std::chrono::time_point<std::chrono::steady_clock>{}) {
    std::fprintf(stderr, "ChronoTimer double-initialized\n");
    std::abort();
  }
  start_ = std::chrono::steady_clock::now();
}

float ChronoTimer::GetTime() {
  return std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now() - start_)
      .count();
}

int64_t ChronoTimer::GetTimeMS() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_)
      .count();
}

int64_t ChronoTimer::GetTimeUS() {
  return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start_)
      .count();
}

void ChronoTimer::SleepMS(int delay) {
  std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

void ChronoTimer::Pump() {}

} // namespace D3
