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

#include <array>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "d3_test/files.h"
#include "d3_test/renderer.h"

#include "d3_test/renderer.h"

struct TestFont {
  uint8_t min_ascii = ' ';
  uint8_t max_ascii = 'Z';
  int16_t height = 8;
  int16_t width = 4; // glyph width of a fixed-width font, or the widest glyph of a proportional one
  bool color = false;

  std::vector<uint8_t> widths;

  struct Kern {
    uint8_t first, second;
    int8_t delta;
  };
  std::vector<Kern> kerning;

  // Font file info 2 block (tracking). Fonts without it are "old" D3 fonts.
  bool ffi2 = false;
  int16_t tracking = 0;

  // Glyph bitmaps, one entry per glyph, min_ascii..max_ascii. A missing entry is written as all zeroes.
  // Mono glyphs: rows of ceil(w/8) bytes, MSB is the leftmost pixel. Color glyphs: w*h 16-bit 565 pixels.
  std::vector<std::vector<uint8_t>> glyph_data;

  int GlyphWidth(int ch) const { return widths.empty() ? width : widths[ch - min_ascii]; }
  int NumGlyphs() const { return max_ascii - min_ascii + 1; }
};

// The contents of a font file holding `font`.
std::vector<uint8_t> FontFileBytes(const TestFont &font);

// The glyphs grtext drew, through rend_DrawFontCharacter().
using DrawnGlyph = d3_test::DrawnFontCharacter;
inline const std::vector<DrawnGlyph> &DrawnGlyphs() { return d3_test::DrawnFontCharacters(); }

uint16_t BitmapPixel(int bm_handle, int x, int y);

class GrtextTest : public ::testing::Test {
protected:
  void SetUp() override;
  void TearDown() override;

  int LoadFont(const TestFont &font);
  std::filesystem::path FontPath(const std::string &name) const { return dir_ / name; }
  std::filesystem::path WriteFont(const TestFont &font);

  // Text area used for the clip rectangle, large enough that nothing in the tests is clipped by default.
  static constexpr int kLeft = 0, kTop = 0, kRight = 4000, kBottom = 1000, kTabSpaces = 4;

private:
  d3_test::TempDir dir_;
  int font_counter_ = 0;
};

// grtext_Init decodes the profanity list in place and therefore must run exactly once per process.
void EnsureGrtextInitialized();

// Defined in grtext.cpp; only declared by its user in Descent3.
void grtext_SetProfanityFilter(bool enabled);
