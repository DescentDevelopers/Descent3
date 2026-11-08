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

#include "briefing_parse.h"

namespace briefing_test {

int Parse(CBriefParse &parser, const d3_test::TempDir &dir, std::string_view contents) {
  auto file = dir.Write("test.brf", contents);
  return parser.ParseBriefing(file.string().c_str());
}

int ParseInScreen(CBriefParse &parser, const d3_test::TempDir &dir, const std::string &body) {
  return Parse(parser, dir, "$screen 0   desc \"s\" start\n" + body + "$endscreen\n");
}

} // namespace briefing_test
