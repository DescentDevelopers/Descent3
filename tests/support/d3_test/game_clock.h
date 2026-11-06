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


#pragma once

#include "chrono_timer.h"

namespace d3_test {

// Starts the game clock the first time it is called in a process, and does nothing after that (the clock aborts if
// it is started twice). Tests that read the clock call it from their fixture's SetUp() or SetUpTestSuite(), rather
// than from a global test environment, so that the clock's own "used before Initialize" tests can share an
// executable with them.
inline void EnsureChronoTimer() {
  static const bool started = (D3::ChronoTimer::Initialize(), true);
  (void)started;
}

} // namespace d3_test
