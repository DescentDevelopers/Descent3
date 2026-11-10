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

#include <set>
#include <string>

#include <gtest/gtest.h>

#include "support/hosts_file.h"

using namespace dmfc_test;

namespace {

class HostsFileTest : public ::testing::Test {
protected:
  HostsFileSandbox hosts_;
};

const uint32_t kAll = Octets(255, 255, 255, 255);

} // namespace

TEST_F(HostsFileTest, ExactAddressGetsFullMask) {
  EXPECT_EQ(hosts_.Parse("192.168.1.10\n"), (std::multiset<IpMask>{{Octets(192, 168, 1, 10), kAll}}));
}

TEST_F(HostsFileTest, WildcardOctetsAreMaskedOut) {
  EXPECT_EQ(hosts_.Parse("10.*.*.*\n172.16.*.5\n*.*.*.*\n"),
            (std::multiset<IpMask>{{Octets(10, 0, 0, 0), Octets(255, 0, 0, 0)},
                                   {Octets(172, 16, 0, 5), Octets(255, 255, 0, 255)},
                                   {Octets(0, 0, 0, 0), Octets(0, 0, 0, 0)}}));
}

TEST_F(HostsFileTest, LeadingWhitespaceAndMissingFinalNewlineAreAccepted) {
  EXPECT_EQ(hosts_.Parse("  \t1.2.3.4\n255.255.255.255"),
            (std::multiset<IpMask>{{Octets(1, 2, 3, 4), kAll}, {Octets(255, 255, 255, 255), kAll}}));
}

TEST_F(HostsFileTest, MalformedLinesAreSkippedWithoutStoppingTheParse) {
  const std::string contents = "# comment\n"
                               "hostname.example.com\n"
                               "1.2.3\n"
                               "1.2.3.4.5\n"
                               "256.1.1.1\n"
                               "1.2.3.\n"
                               "1..3.4\n"
                               "1234.1.1.1\n"
                               "12a.1.1.1\n"
                               "\n"
                               "8.8.4.4\n";
  EXPECT_EQ(hosts_.Parse(contents), (std::multiset<IpMask>{{Octets(8, 8, 4, 4), kAll}}));
}

TEST_F(HostsFileTest, MissingFileYieldsNoEntries) {
  EXPECT_TRUE(ParseHostsPath(hosts_.dir() / "does_not_exist.deny").empty());
}
