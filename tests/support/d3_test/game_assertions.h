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

#include <SDL3/SDL_assert.h>

namespace d3_test {

// The shared main() (test_main.cpp) fails the current test when a game ASSERT() or Int3() fires. A test that triggers
// them on purpose declares an ExpectGameAssertions while it does: assertions fired in its lifetime are counted
// instead, and the previous handler comes back when it goes out of scope.
class ExpectGameAssertions {
public:
  ExpectGameAssertions();
  ~ExpectGameAssertions();
  ExpectGameAssertions(const ExpectGameAssertions &) = delete;
  ExpectGameAssertions &operator=(const ExpectGameAssertions &) = delete;

  // How many assertions have fired since construction.
  int count() const { return count_; }

private:
  static SDL_AssertState SDLCALL Count(const SDL_AssertData *data, void *userdata);

  int count_ = 0;
  SDL_AssertionHandler previous_handler_;
  void *previous_userdata_;
};

} // namespace d3_test
