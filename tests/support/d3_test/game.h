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

// Controls for the stand-ins of the Descent3 executable in tests/support/game (d3_test_game).

namespace d3_test {

// The stand-ins for FindTextureName() and FindTextureBitmapName() find the names set here (case-insensitively), and
// nothing else.
void SetTextureName(const char *name, int texture);
void SetTextureBitmapName(const char *name, int texture);
void ClearTextureNames();

// Start a library the first time they are called in a process, as the game does at startup: the bitmap library
// (bm_InitBitmaps) and the memory library (mem_Init, without which Windows builds cannot allocate through it).
void EnsureBitmaps();
void EnsureMem();

} // namespace d3_test
