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

#include "test_fonts.h"

#include <cctype>

#include "grtext.h"

TestFont MakeProportionalFont() {
  TestFont f;
  f.min_ascii = ' ';
  f.max_ascii = 'Z';
  f.height = 9;
  f.width = 7;
  f.widths.assign(f.NumGlyphs(), 4);
  f.widths[' ' - f.min_ascii] = 3;
  f.widths['A' - f.min_ascii] = 5;
  f.widths['B' - f.min_ascii] = 6;
  f.widths['V' - f.min_ascii] = 7;
  return f;
}

TestFont MakeMetricsFont() {
  TestFont f;
  f.min_ascii = ' ';
  f.max_ascii = 'Z';
  f.height = 8;
  f.width = 7;
  f.widths.assign(f.NumGlyphs(), 4);
  f.widths[' ' - f.min_ascii] = 3;
  f.widths['A' - f.min_ascii] = 5;
  f.widths['B' - f.min_ascii] = 6;
  f.widths['V' - f.min_ascii] = 7;
  f.kerning = {{'A', 'V', -2}};
  return f;
}

TestFont MakeDecodableFont() {
  TestFont f;
  f.min_ascii = ' ';
  f.max_ascii = 'z';
  f.height = 2;
  f.width = f.NumGlyphs();
  for (int i = 0; i < f.NumGlyphs(); i++)
    f.widths.push_back(static_cast<uint8_t>(i + 1));
  return f;
}

std::string DecodeDrawnText() {
  std::string out;
  for (const DrawnGlyph &d : DrawnGlyphs())
    out += static_cast<char>(' ' + (d.x2 - d.x1) - 1);
  return out;
}

std::string Render(const char *str) {
  d3_test::ResetRenderer();
  grtext_Puts(10, 10, str);
  grtext_Flush();
  return DecodeDrawnText();
}

bool NoLetters(const std::string &s) {
  for (char c : s)
    if (std::isalpha(static_cast<unsigned char>(c)))
      return false;
  return true;
}
