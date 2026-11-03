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
#include <filesystem>
#include <vector>

// Converts an unsigned 8-bit PCM sample to the equivalent signed 16-bit sample.
int16_t To16(uint8_t u);

// Loads a wave file into SoundFiles[slot] through SoundLoadWaveFile.
bool Load(const std::filesystem::path &path, int slot, bool high_quality, float volume = 1.0f, bool load_data = true);

// The loaded (padded) 16-bit or 8-bit samples of SoundFiles[slot].
std::vector<int> Samples16(int slot);
std::vector<int> Samples8(int slot);
