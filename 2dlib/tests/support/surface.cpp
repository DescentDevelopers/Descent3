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

#include "surface.h"

#include <gtest/gtest.h>

namespace twodlib_test {

OwnedSurface::OwnedSurface(int w, int h, int pitch, unsigned flags, int guard_rows, uint16_t fill)
    : w(w), h(h), pitch(pitch), pixels((size_t)pitch * (h + guard_rows), fill) {
  surf.init(w, h, BPP_16, reinterpret_cast<char *>(pixels.data()), pitch * 2, flags);
}

uint16_t ReadPixel(grSurface &sf, int x, int y) {
  int rowsize;
  char *base = sf.lock(&rowsize);
  uint16_t value = *reinterpret_cast<uint16_t *>(base + y * rowsize + x * 2);
  sf.unlock();
  return value;
}

uint16_t ClearValue(ddgr_color col) {
  grMemorySurface ref(1, 1, BPP_16);
  ref.clear(col);
  return ReadPixel(ref, 0, 0);
}

int CountDrawnPixels(const OwnedSurface &s) {
  int n = 0;
  for (uint16_t p : s.pixels)
    n += (p != 0);
  return n;
}

void ExpectBlock(const OwnedSurface &dst, const OwnedSurface &src, int dx0, int dy0, int sx0, int sy0, int w, int h) {
  int mismatches = 0;
  for (int y = 0; y < dst.h; y++) {
    for (int x = 0; x < dst.pitch; x++) {
      const bool inside = x >= dx0 && x < dx0 + w && y >= dy0 && y < dy0 + h;
      const uint16_t expected = inside ? src.at(sx0 + x - dx0, sy0 + y - dy0) : kSentinel;
      if (dst.at(x, y) != expected && ++mismatches <= 4)
        ADD_FAILURE() << "at (" << x << "," << y << "): got 0x" << std::hex << dst.at(x, y) << ", expected 0x"
                      << expected << (inside ? "" : " (untouched)");
    }
  }
  EXPECT_EQ(mismatches, 0) << "pixels differing from expected blt result";
}

} // namespace twodlib_test
