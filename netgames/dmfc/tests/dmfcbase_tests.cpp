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

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "support/dmfc_fakes.h"
#include "support/dmfc_internals.h"
#include "support/dmfc_test_support.h"

using DMFCBaseTest = DMFCFixture;

TEST_F(DMFCBaseTest, CompareNetworkAddressPortSensitivity) {
  network_address a{};
  a.address[0] = 192;
  a.address[1] = 168;
  a.address[2] = 0;
  a.address[3] = 1;
  a.port = 2092;
  a.connection_type = NP_TCP;

  network_address same = a;
  network_address other_port = a;
  other_port.port = 2093;
  network_address other_host = a;
  other_host.address[3] = 2;
  network_address other_protocol = a;
  other_protocol.connection_type = NP_IPX;

  EXPECT_TRUE(dmfc->CompareNetworkAddress(&a, &same, true));
  EXPECT_TRUE(dmfc->CompareNetworkAddress(&a, &same, false));

  EXPECT_FALSE(dmfc->CompareNetworkAddress(&a, &other_port, true));
  EXPECT_TRUE(dmfc->CompareNetworkAddress(&a, &other_port, false));

  EXPECT_FALSE(dmfc->CompareNetworkAddress(&a, &other_host, false));
  EXPECT_FALSE(dmfc->CompareNetworkAddress(&a, &other_protocol, false));
}

TEST(RemotePadBuffer, PadsToMultipleOfEightWithZeros) {
  struct Case {
    int in, out;
  };
  for (Case c : {Case{1, 8}, Case{7, 8}, Case{8, 8}, Case{9, 16}, Case{16, 16}, Case{17, 24}}) {
    std::vector<uint8_t> input(c.in);
    for (int i = 0; i < c.in; i++)
      input[i] = static_cast<uint8_t>(0xA0 + i);

    int new_size = -1;
    uint8_t *padded = nullptr;
    Remote_pad_buffer(c.in, input.data(), &new_size, &padded);

    ASSERT_NE(padded, nullptr);
    EXPECT_EQ(new_size, c.out) << "input size " << c.in;
    EXPECT_EQ(memcmp(padded, input.data(), c.in), 0) << "input size " << c.in;
    for (int i = c.in; i < new_size; i++)
      EXPECT_EQ(padded[i], 0) << "input size " << c.in << " byte " << i;
    free(padded);
  }
}

namespace {

class ClipStringTest : public DMFCFixture {
protected:
  void SetUp() override {
    DMFCFixture::SetUp();
    saved_width_ = DLLgrtext_GetTextLineWidth;
    DLLgrtext_GetTextLineWidth = dmfc_test::FixedWidthTextLineWidth;
  }
  void TearDown() override {
    DLLgrtext_GetTextLineWidth = saved_width_;
    DMFCFixture::TearDown();
  }

  grtext_GetTextLineWidth_fp saved_width_ = nullptr;
};

const std::string kArrow(1, CHAR_RIGHT_ARROW);

TEST_F(ClipStringTest, LeavesStringThatFitsUnchanged) {
  char s[] = "ABC";
  dmfc->ClipString(40, s, false);
  EXPECT_STREQ(s, "ABC");

  char t[] = "ABC";
  dmfc->ClipString(40, t, true);
  EXPECT_STREQ(t, "ABC");
}

TEST_F(ClipStringTest, LeavesStringThatExactlyFitsUnchanged) {
  char s[] = "ABCD";
  const int width = DLLgrtext_GetTextLineWidth(s);
  ASSERT_EQ(width, 19);
  dmfc->ClipString(width, s, false);
  EXPECT_STREQ(s, "ABCD");

  char t[] = "ABCD";
  dmfc->ClipString(width, t, true);
  EXPECT_STREQ(t, "ABCD");
}

TEST_F(ClipStringTest, WithArrowKeepsLongestPrefixThatFitsWithTheArrow) {
  char s[] = "ABCDEFGH";
  dmfc->ClipString(32, s, true);
  EXPECT_EQ(std::string(s), "ABCDE" + kArrow);
  EXPECT_LE(DLLgrtext_GetTextLineWidth(s), 32);
}

} // namespace
