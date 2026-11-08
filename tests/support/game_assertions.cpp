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


#include "d3_test/game_assertions.h"

namespace d3_test {

ExpectGameAssertions::ExpectGameAssertions() {
  previous_handler_ = SDL_GetAssertionHandler(&previous_userdata_);
  SDL_SetAssertionHandler(Count, this);
}

ExpectGameAssertions::~ExpectGameAssertions() { SDL_SetAssertionHandler(previous_handler_, previous_userdata_); }

SDL_AssertState SDLCALL ExpectGameAssertions::Count(const SDL_AssertData *, void *userdata) {
  static_cast<ExpectGameAssertions *>(userdata)->count_++;
  return SDL_ASSERTION_IGNORE;
}

} // namespace d3_test
