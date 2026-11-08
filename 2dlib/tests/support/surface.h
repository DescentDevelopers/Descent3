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

#include <cstdint>
#include <vector>

#include "gr.h"

namespace twodlib_test {

// Fill value for pixels a test expects to stay untouched.
constexpr uint16_t kSentinel = 0x5A5A;

// A 16-bit memory surface over owned pixels, with guard_rows extra rows of storage below it.
struct OwnedSurface {
  int w, h, pitch; // pitch in pixels
  std::vector<uint16_t> pixels;
  grMemorySurface surf;

  OwnedSurface(int w, int h, int pitch, unsigned flags = 0, int guard_rows = 0, uint16_t fill = kSentinel);

  uint16_t &at(int x, int y) { return pixels[(size_t)y * pitch + x]; }
  uint16_t at(int x, int y) const { return pixels[(size_t)y * pitch + x]; }
};

// Reads one pixel through the surface's lock.
uint16_t ReadPixel(grSurface &sf, int x, int y);

// The pixel value clear() writes for col.
uint16_t ClearValue(ddgr_color col);

// Number of pixels that are not zero.
int CountDrawnPixels(const OwnedSurface &s);

// Expects dst to hold the w x h block of src at (sx0, sy0) placed at (dx0, dy0), and kSentinel everywhere else.
void ExpectBlock(const OwnedSurface &dst, const OwnedSurface &src, int dx0, int dy0, int sx0, int sy0, int w, int h);

} // namespace twodlib_test
