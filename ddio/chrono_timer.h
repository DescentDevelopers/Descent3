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

#pragma once

#include <chrono>

namespace D3 {

static std::chrono::time_point<std::chrono::steady_clock> m_start_tstamp;

/**
 * Static class for handling timers and time durations
 */
class ChronoTimer {
public:

  /// Initialize internal timestamp
  static void Initialize();

  /// Get time in seconds after class initialization (i.e. application start)
  static float GetTime();

  /// Get time in milliseconds after class initialization (i.e. application start)
  static int64_t GetTimeMS();

  /// Get time in microseconds after class initialization (i.e. application start)
  static int64_t GetTimeUS();

  /// Sleep for delay milliseconds
  static void SleepMS(int delay);

  /// Let time pass while waiting on the timer without sleeping. Call once per wait-loop iteration.
  /// Does nothing on the real clock.
  static void Pump();

#ifdef MOCK_CLOCK
  /// Length of every game frame on the mock clock. Matches the default 60fps framecap, which truncates to 16ms.
  static constexpr int kFixedStepMS = 16;

  /// Whether the mock clock waits for real time to catch up when it advances (default true)
  static void SetPaced(bool paced);
#endif
};

}
