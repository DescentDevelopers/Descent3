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

#include "text_draw.h"

#include <string>

#include <gtest/gtest.h>

#include "test_font.h"

namespace twodlib_test {

int Puts(grViewport &vp, int x, int y, const char *text, grTextAlign align) {
  std::string buf(text);
  vp.lock();
  int r = vp.puts(align, x, y, buf.data());
  vp.unlock();
  return r;
}

int LineWidth(grViewport &vp, const char *text) {
  std::string buf(text);
  return vp.get_text_line_width(buf.data());
}

void ExpectGlyph(const OwnedSurface &s, int ch, int x, int y, int col0, int row0, int w, int h) {
  if (w < 0)
    w = GlyphWidth(ch) - col0;
  if (h < 0)
    h = kGlyphHeight - row0;
  int mismatches = 0;
  for (int r = 0; r < h; r++) {
    for (int c = 0; c < w; c++) {
      const uint16_t got = s.at(x + c, y + r), expected = GlyphPixel(ch, col0 + c, row0 + r);
      if (got != expected && ++mismatches <= 4)
        ADD_FAILURE() << "glyph '" << (char)ch << "' col " << col0 + c << " row " << row0 + r << " expected at ("
                      << x + c << "," << y + r << "), found 0x" << std::hex << got;
    }
  }
  EXPECT_EQ(mismatches, 0) << "pixels of glyph '" << (char)ch << "' missing or misplaced";
}

} // namespace twodlib_test
