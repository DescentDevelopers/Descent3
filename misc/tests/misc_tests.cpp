/*
 * Descent 3
 * Copyright (C) 2024 Descent Developers
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

#include <utility>

#include <gtest/gtest.h>

#include "pstring.h"

TEST(D3, CleanupStr) {
  std::vector<std::pair<const char*, const char*>> test_data = {
      {"String with tails \t \n   ", "String with tails"},
      {" \t \n   String with heads", "String with heads"},
      {" \t \n String with heads and tails \t  \n ", "String with heads and tails"},
      {"A normal string", "A normal string"},
      {"", ""},
      {"       ", ""},
      {" \n One \n \n Two \n \n ", "One \n \n Two"},
      {"A very large string which excess size of result", "A very large string which exces"},

  };
  char result[32];

  for (auto i : test_data) {
    CleanupStr(result, i.first, 32);
    ASSERT_STREQ(result, i.second);
  }
}

TEST(D3, StringJoinSplit) {
  std::string test = "Joined text;Another text;And more;";
  std::vector<std::string> parts = {"Joined text", "Another text", "And more", ""};

  std::string empty = "";
  std::vector<std::string> empty_parts = {""};

  ASSERT_EQ(test, StringJoin(parts, ";"));
  ASSERT_EQ(StringSplit(test, ";"), parts);

  ASSERT_EQ(empty, StringJoin(empty_parts, ";"));
  ASSERT_EQ(StringSplit(empty, ";"), empty_parts);
}
