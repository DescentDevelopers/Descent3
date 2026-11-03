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

namespace twodlib_test {

constexpr uint8_t kMinAscii = ' ';
constexpr uint8_t kMaxAscii = 'Z';
constexpr int kGlyphHeight = 6;
const char kFontName[] = "testfont";
const char kFontFile[] = "2dlib_test.fnt";

int GlyphWidth(int ch);

uint16_t GlyphPixel(int ch, int col, int row);

std::vector<uint8_t> BuildFontFile();

} // namespace twodlib_test
