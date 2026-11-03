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
#include <string_view>
#include <vector>

#include "d3_test/files.h"

// Loads string table files written to its own temp directory and keeps what the last load produced.
class StringTableLoader {
public:
  // Writes contents to a file, loads it with CreateStringTable and copies its strings ("<null>" for null entries).
  bool Load(std::string_view contents);

  std::vector<std::string> strings;
  int size = -1;
  bool table_is_null = false;

private:
  d3_test::TempDir dir_;
};
