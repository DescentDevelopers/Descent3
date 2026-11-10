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

#include "d3_test/files.h"

namespace aiscripts_test {

// Writes `contents` to a message file in `dir` and loads it with LoadMessageFile.
bool LoadMessages(const d3_test::TempDir &dir, const std::string &contents);

// What LookupMessage returns for a name that isn't in the loaded message file.
std::string NotFoundMessage();

} // namespace aiscripts_test
