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

#include <cstring>
#include <string>
#include <utility>
#include <vector>

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

TEST(D3, CleanupStrNeverWritesPastDestlen) {
  char buf[16];
  std::memset(buf, '#', sizeof(buf));

  CleanupStr(buf, "   0123456789abcdef   ", 8);

  EXPECT_STREQ(buf, "0123456");
  for (std::size_t i = 8; i < sizeof(buf); ++i)
    EXPECT_EQ(buf[i], '#') << "byte " << i << " written past destlen";
}

TEST(D3, CleanupStrZeroLengthBufferIsNotWritten) {
  char buf[4] = {'#', '#', '#', '#'};
  CleanupStr(buf, "abc", 0);
  for (char c : buf)
    EXPECT_EQ(c, '#');
}

TEST(D3, CleanupStrOneByteBufferHoldsOnlyTerminator) {
  char buf[4] = {'#', '#', '#', '#'};
  CleanupStr(buf, "  abc  ", 1);
  EXPECT_EQ(buf[0], '\0');
  EXPECT_EQ(buf[1], '#');
}

TEST(D3, StringSplitKeepsEmptyFields) {
  using V = std::vector<std::string>;
  EXPECT_EQ(StringSplit("a;b;c", ";"), (V{"a", "b", "c"}));
  EXPECT_EQ(StringSplit(";a", ";"), (V{"", "a"}));
  EXPECT_EQ(StringSplit("a;;b", ";"), (V{"a", "", "b"}));
  EXPECT_EQ(StringSplit(";", ";"), (V{"", ""}));
  EXPECT_EQ(StringSplit("no delimiter here", ";"), (V{"no delimiter here"}));
}

TEST(D3, StringSplitMultiCharacterDelimiter) {
  using V = std::vector<std::string>;
  EXPECT_EQ(StringSplit("a::b::c", "::"), (V{"a", "b", "c"}));
  EXPECT_EQ(StringSplit("a:b::c", "::"), (V{"a:b", "c"}));
}

// mission_download.cpp splits a URL on '/' and rejoins parts [0,3) as the server and [3,end) as
// the path, which relies on "scheme://" producing an empty field between the two slashes.
TEST(D3, StringSplitJoinUrlAsUsedByMissionDownload) {
  using V = std::vector<std::string>;
  auto parts = StringSplit("https://example.com/missions/pack/level.mn3", "/");
  ASSERT_EQ(parts, (V{"https:", "", "example.com", "missions", "pack", "level.mn3"}));
  EXPECT_EQ(StringJoin(V(parts.begin(), parts.begin() + 3), "/"), "https://example.com");
  EXPECT_EQ("/" + StringJoin(V(parts.begin() + 3, parts.end()), "/"), "/missions/pack/level.mn3");
}

TEST(D3, StringJoinEdgeCases) {
  using V = std::vector<std::string>;
  EXPECT_EQ(StringJoin(V{}, ";"), "");
  EXPECT_EQ(StringJoin(V{"only"}, ";"), "only");
  EXPECT_EQ(StringJoin(V{"a", "b", "c"}, ", "), "a, b, c");
  EXPECT_EQ(StringJoin(V{"", ""}, ";"), ";");
  EXPECT_EQ(StringJoin(V{"a", "b"}, ""), "ab");
}

TEST(D3, StringJoinSplitRoundTrip) {
  using V = std::vector<std::string>;
  const std::vector<V> cases = {
      {"*.ogf", "*.tga", "*.pcx"},
      {"", "middle", ""},
      {"x"},
      {"", "", ""},
      {"with space", "and\ttab"},
  };
  for (const std::string delim : {";", "::", "<->"}) {
    for (const auto &parts : cases) {
      EXPECT_EQ(StringSplit(StringJoin(parts, delim), delim), parts) << "delim " << delim;
    }
  }
}
