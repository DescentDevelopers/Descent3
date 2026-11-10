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

#include "test_stubs.h"

#include <cstring>

#include "d3_test/game.h"
#include "gametexture.h"

// manage.cpp; DataError() counts into Data_error_count only while add-on data is loading.
extern bool Loading_addon;
extern int Data_error_count;

namespace model_test {

void ResetStubs() {
  d3_test::ClearTextureNames();
  for (auto &t : GameTextures) {
    std::memset(&t, 0, sizeof(t));
    t.alpha = 1.0f;
  }
  Loading_addon = true;
  Data_error_count = 0;
}

int DataErrorCount() { return Data_error_count; }

} // namespace model_test
