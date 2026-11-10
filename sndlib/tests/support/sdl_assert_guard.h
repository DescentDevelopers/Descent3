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
#include <gtest/gtest.h>

// Production ASSERT() / Int3() end up in SDL_assert, whose default handler pops a
// modal dialog (or breaks into the debugger) in debug builds.
class SdlAssertGuard {
public:
  SdlAssertGuard() { SDL_SetAssertionHandler(&Handler, nullptr); }
  ~SdlAssertGuard() { SDL_SetAssertionHandler(nullptr, nullptr); }
  SdlAssertGuard(const SdlAssertGuard &) = delete;
  SdlAssertGuard &operator=(const SdlAssertGuard &) = delete;

private:
  static SDL_AssertState SDLCALL Handler(const SDL_AssertData *data, void *) {
    ADD_FAILURE() << "Production assertion fired: (" << data->condition << ") at " << data->filename << ":"
                  << data->linenum;
    return SDL_ASSERTION_IGNORE;
  }
};
