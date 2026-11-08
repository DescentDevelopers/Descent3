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

#include "unzip.h"
#include "d3_test/files.h"
#include "zip_builder.h"

// Writes `ar` to the directory as archive.zip and opens it with `zip`; fails the test if it does not open.
void OpenArchive(ZIP &zip, const d3_test::TempDir &dir, const Archive &ar);

// Reads the next entry of `zip`, expecting it to exist and be named `expected_name`.
zipentry *NextEntry(ZIP &zip, const std::string &expected_name);

// Reads `ent` with ZIP::ReadFile, storing its return code in `*result`.
std::vector<char> ReadIntoMemory(ZIP &zip, zipentry *ent, int *result);
