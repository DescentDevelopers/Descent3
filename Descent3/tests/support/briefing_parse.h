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

#include "BriefingParse.h"
#include "d3_test/files.h"

namespace briefing_test {

// Writes contents to test.brf in dir and parses it.
int Parse(CBriefParse &parser, const d3_test::TempDir &dir, std::string_view contents);

// Parses body wrapped in a minimal screen 0 block.
int ParseInScreen(CBriefParse &parser, const d3_test::TempDir &dir, const std::string &body);

} // namespace briefing_test
