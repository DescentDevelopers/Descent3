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

#include <array>
#include <cstdint>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "networking.h"
#include "dip_gametrack.h"
#include "support/dip_gametrack_internals.h"
#include "support/dip_test_helpers.h"

using ::testing::ElementsAre;
using ::testing::IsEmpty;

namespace {

using namespace dip_test;

// D3_DEFAULT_PORT (Descent3/descent.h).
constexpr uint16_t kDefaultGamePort = 2092;

class DipGameTrackTest : public ::testing::Test {
protected:
  void SetUp() override { Decode(""); }
  void TearDown() override { Decode(""); }
};

TEST_F(DipGameTrackTest, ParsesAddressesWithAndWithoutPort) {
  EXPECT_THAT(Decode("192.168.1.20:2093\n10.0.0.1\n"),
              ElementsAre(H(192, 168, 1, 20, 2093), H(10, 0, 0, 1, kDefaultGamePort)));
}

TEST_F(DipGameTrackTest, LastLineNeedsNoTrailingNewline) {
  EXPECT_THAT(Decode("1.2.3.4:5000"), ElementsAre(H(1, 2, 3, 4, 5000)));
}

TEST_F(DipGameTrackTest, SkipsBlankAndInvalidLines) {
  EXPECT_THAT(Decode("\nnot an address\n1.2.3.4:2092\n256.1.1.1\n\n5.6.7.8\n"),
              ElementsAre(H(1, 2, 3, 4, 2092), H(5, 6, 7, 8, kDefaultGamePort)));
}

TEST_F(DipGameTrackTest, OverlongAddressWithPortIsRejected) {
  EXPECT_THAT(Decode("123.123.123.1234:2092\n"), IsEmpty());
}

TEST_F(DipGameTrackTest, OverlongAddressWithoutPortIsRejected) {
  EXPECT_THAT(Decode("123.123.123.1234\n"), IsEmpty());
}

TEST_F(DipGameTrackTest, CrlfLinesAreParsedLikeLfLines) {
  EXPECT_THAT(Decode("123.123.123.123\r\n10.0.0.1\r\n"),
              ElementsAre(H(123, 123, 123, 123, kDefaultGamePort), H(10, 0, 0, 1, kDefaultGamePort)));
}

TEST_F(DipGameTrackTest, NewAnswerReplacesThePreviousList) {
  std::stringstream first("1.1.1.1\n2.2.2.2\n");
  DecodeApiAnswer(first);
  EXPECT_THAT(Decode("3.3.3.3\n"), ElementsAre(H(3, 3, 3, 3, kDefaultGamePort)));
}

TEST_F(DipGameTrackTest, GetDIpGameListHandsOutEachListOnce) {
  std::stringstream in("1.2.3.4\n");
  DecodeApiAnswer(in);
  EXPECT_EQ(GetDIpGameList().size(), 1u);
  EXPECT_TRUE(GetDIpGameList().empty());
}

} // namespace
