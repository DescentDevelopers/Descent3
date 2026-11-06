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

#include <cstdint>
#include <string>

#include <gtest/gtest.h>

#include "ddgr.h"
#include "ddgrWin32DX.h"

#include "support/dx_surface.h"

namespace {

TEST(DdgrDxTest, ClearFillsExactlyTheRequestedRect) {
  const int W = 8, H = 6;
  const int L = 1, T = 1, CW = 5, CH = 4;
  const uint32_t sentinel = 0x00a5a5a5;
  const ddgr_color color = 0x00404040;

  DxSurface sf(W, H);
  ASSERT_TRUE(sf.valid());
  sf.fill(sentinel);

  ddgr_dx_surf_Clear(sf.get(), color, L, T, CW, CH);

  for (int y = 0; y < H; y++) {
    for (int x = 0; x < W; x++) {
      const bool inside = x >= L && x < L + CW && y >= T && y < T + CH;
      EXPECT_EQ(sf.get(x, y), inside ? color : sentinel) << "at (" << x << "," << y << ")";
    }
  }
}

} // namespace
