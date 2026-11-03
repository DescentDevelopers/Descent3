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

#include "gr.h"
#include "surface.h"

namespace twodlib_test {

// Draws text with vp.puts inside a lock/unlock pair; returns what puts returns.
int Puts(grViewport &vp, int x, int y, const char *text, grTextAlign align = grTextUnjustified);

// vp.get_text_line_width on a writable copy of text.
int LineWidth(grViewport &vp, const char *text);

// Expects the w x h part of test-font glyph ch starting at (col0, row0) to be drawn at (x, y) on s; w and h
// default to the rest of the glyph.
void ExpectGlyph(const OwnedSurface &s, int ch, int x, int y, int col0 = 0, int row0 = 0, int w = -1, int h = -1);

} // namespace twodlib_test
