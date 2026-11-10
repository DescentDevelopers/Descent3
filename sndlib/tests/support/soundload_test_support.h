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

#include <string>
#include <vector>

#include "sounds.h"

// Defined in soundload.cpp but not declared in any header.
extern const char *Static_sound_names[NUM_STATIC_SOUNDS];
void ChangeSoundFileName(const char *src, char *dest);

// Allocates a sound named `name` and returns its index, failing the test if no slot is free.
int AddSound(const std::string &name);

// Points GameTextures[texture_index] at a sound and records the texture in `touched` for later reset.
void SetTextureSound(std::vector<int> &touched, int texture_index, int sound_index);
