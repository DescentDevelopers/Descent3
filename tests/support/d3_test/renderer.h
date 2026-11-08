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

#include <vector>

// The renderer the tests run against (tests/support/renderer, d3_test_renderer): the real device-independent 3D
// pipeline (renderer/Hardware*.cpp) on top of a fake of the OpenGL backend. The fake draws nothing; what the tests
// need to see of it is here.

namespace d3_test {

// The viewport the fake reports through rend_GetProjectionParameters() and rend_GetProjectionScreenParameters().
struct FakeViewport {
  int x = 0;
  int y = 0;
  int width = 640;
  int height = 480;
};
FakeViewport &RendererViewport();

// One rend_DrawFontCharacter() call.
struct DrawnFontCharacter {
  int bm_handle;
  int x1, y1, x2, y2;
  float u, v, w, h;
};
// Every rend_DrawFontCharacter() call since the last ResetRenderer().
const std::vector<DrawnFontCharacter> &DrawnFontCharacters();

// Forgets what was drawn and puts the viewport back to 640x480.
void ResetRenderer();

} // namespace d3_test
