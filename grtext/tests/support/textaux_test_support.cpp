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

#include "textaux_test_support.h"

#include <cstring>
#include <sstream>

#include "textaux.h"

std::string Wrap(const char *src, int width, int font) {
  std::vector<char> dest(strlen(src) + 64, '?');
  textaux_WordWrap(src, dest.data(), width, font);
  return dest.data();
}

std::vector<std::string> Lines(const std::string &s) {
  std::vector<std::string> lines;
  std::stringstream ss(s);
  for (std::string line; std::getline(ss, line, '\n');)
    lines.push_back(line);
  return lines;
}
