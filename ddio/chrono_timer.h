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

/**
 * Static class for handling timers and time durations
 *
 * Threading rules:
 * - The thread that calls Initialize is the main thread. Only the main thread may call Pump.
 * - Only the main thread's SleepMS moves time on the mock clock. On any other thread, SleepMS waits until the main
 *   thread has moved time forward by the delay. On the real clock, SleepMS sleeps for real time on every thread.
 * - Any thread may call GetTime, GetTimeMS and GetTimeUS.
 * - It is an error to:
 *   - Call Initialize more than once
 *   - Call Pump, SleepMS, or any GetTime before Initialize
 *   - Call Pump from a thread other than the main thread
 * - The main thread must never wait for a background thread, whether by joining it or by waiting for a flag it sets.
 *   It may take a lock that a background thread also takes, but no background thread may hold such a lock while it
 *   sleeps or waits. Either mistake stalls frames, and on the mock clock it can deadlock.
 */
class ChronoTimer {
public:

  /// Start the clock from zero. May only be called once. Sets internal notion of "main thread".
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
#endif
};

}
