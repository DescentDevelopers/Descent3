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

#include "test_font.h"

#include "d3_test/byte_writer.h"
#include "grdefs.h"

namespace twodlib_test {

namespace {

// Font file flags (font.cpp)
constexpr int16_t FT_COLOR = 1;
constexpr int16_t FT_PROPORTIONAL = 2;

} // namespace

int GlyphWidth(int ch) {
  switch (ch) {
  case ' ':
    return 3;
  case 'A':
    return 4;
  case 'B':
    return 5;
  case 'C':
    return 6;
  default:
    return 2;
  }
}

uint16_t GlyphPixel(int ch, int col, int row) { return (uint16_t)(OPAQUE_FLAG16 | (ch << 8) | (col << 4) | row); }

std::vector<uint8_t> BuildFontFile() {
  d3_test::ByteWriter out;
  out.U32(0xfeedbaba);
  out.I16(6); // widest char
  out.I16(kGlyphHeight);
  out.I16(FT_COLOR | FT_PROPORTIONAL);
  out.I16(0); // baseline
  out.U8(kMinAscii);
  out.U8(kMaxAscii);
  out.FixedText(kFontName, 32);

  for (int ch = kMinAscii; ch <= kMaxAscii; ch++)
    out.I16((int16_t)GlyphWidth(ch));

  d3_test::ByteWriter raw;
  for (int ch = kMinAscii; ch <= kMaxAscii; ch++)
    for (int row = 0; row < kGlyphHeight; row++)
      for (int col = 0; col < GlyphWidth(ch); col++)
        raw.U16(GlyphPixel(ch, col, row));
  out.U32((uint32_t)raw.size());
  out.Bytes(raw.bytes());
  return out.bytes();
}

} // namespace twodlib_test
