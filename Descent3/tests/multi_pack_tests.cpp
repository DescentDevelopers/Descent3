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

#include <cfloat>
#include <climits>
#include <cstdint>
#include <cstring>
#include <limits>
#include <vector>

#include <gtest/gtest.h>

#include "vecmat_external.h" // multi_external.h uses vector without including it
#include "multi_external.h"

namespace {

TEST(MultiPack, MixedSequenceRoundTripsAndAdvancesCursorConsistently) {
  uint8_t buf[256] = {};
  int wcount = 0;

  MultiAddUbyte(0xFE, buf, &wcount);
  MultiAddSbyte(-5, buf, &wcount);
  MultiAddShort(-12345, buf, &wcount);
  MultiAddUshort(0xBEEF, buf, &wcount);
  MultiAddInt(INT_MIN, buf, &wcount);
  MultiAddUint(0xDEADBEEFu, buf, &wcount);
  MultiAddFloat(-1.5f, buf, &wcount);
  MultiAddString("pilot", buf, &wcount);
  MultiAddVector(vector{1.0f, -2.0f, 3.25f}, buf, &wcount);
  MultiAddByte(0x7F, buf, &wcount);

  int rcount = 0;
  EXPECT_EQ(MultiGetUbyte(buf, &rcount), 0xFE);
  EXPECT_EQ(MultiGetSbyte(buf, &rcount), -5);
  EXPECT_EQ(MultiGetShort(buf, &rcount), -12345);
  EXPECT_EQ(MultiGetUshort(buf, &rcount), 0xBEEF);
  EXPECT_EQ(MultiGetInt(buf, &rcount), INT_MIN);
  EXPECT_EQ(MultiGetUint(buf, &rcount), 0xDEADBEEFu);
  EXPECT_EQ(MultiGetFloat(buf, &rcount), -1.5f);
  char str[32] = {};
  MultiGetString(str, buf, &rcount);
  EXPECT_STREQ(str, "pilot");
  vector v = MultiGetVector(buf, &rcount);
  EXPECT_EQ(v.x(), 1.0f);
  EXPECT_EQ(v.y(), -2.0f);
  EXPECT_EQ(v.z(), 3.25f);
  EXPECT_EQ(MultiGetByte(buf, &rcount), 0x7F);

  EXPECT_EQ(rcount, wcount);
}

TEST(MultiPack, IntegersAreLittleEndianOnTheWire) {
  uint8_t buf[16] = {};
  int count = 0;

  MultiAddShort(0x1234, buf, &count);
  MultiAddInt(0x0A0B0C0D, buf, &count);

  ASSERT_EQ(count, 6);
  const uint8_t expected[] = {0x34, 0x12, 0x0D, 0x0C, 0x0B, 0x0A};
  EXPECT_EQ(memcmp(buf, expected, sizeof(expected)), 0);
}

TEST(MultiPack, FloatsAreLittleEndianIeee754OnTheWire) {
  uint8_t buf[4] = {};
  int count = 0;

  MultiAddFloat(1.0f, buf, &count); // IEEE 754 single 1.0 is 0x3F800000

  ASSERT_EQ(count, 4);
  const uint8_t expected[] = {0x00, 0x00, 0x80, 0x3F};
  EXPECT_EQ(memcmp(buf, expected, sizeof(expected)), 0);
}

TEST(MultiPack, ReadsLittleEndianBytes) {
  uint8_t buf[] = {0xFF, 0xFF, 0x78, 0x56, 0x34, 0x12};
  int count = 0;

  EXPECT_EQ(MultiGetShort(buf, &count), -1);
  EXPECT_EQ(MultiGetUint(buf, &count), 0x12345678u);
  EXPECT_EQ(count, 6);
}

TEST(MultiPack, StringIsLengthPrefixedIncludingTerminator) {
  uint8_t buf[16];
  memset(buf, 0xAA, sizeof(buf));
  int count = 0;

  MultiAddString("abc", buf, &count);

  ASSERT_EQ(count, 5);
  const uint8_t expected[] = {4, 'a', 'b', 'c', 0};
  EXPECT_EQ(memcmp(buf, expected, sizeof(expected)), 0);
}

TEST(MultiPack, EmptyStringRoundTrips) {
  uint8_t buf[8] = {};
  int wcount = 0;
  MultiAddString("", buf, &wcount);
  EXPECT_EQ(wcount, 2);

  char str[8];
  memset(str, 'x', sizeof(str));
  int rcount = 0;
  MultiGetString(str, buf, &rcount);
  EXPECT_STREQ(str, "");
  EXPECT_EQ(rcount, wcount);
}

TEST(MultiPack, LongestStringRoundTrips) {
  // The length prefix is one byte and counts the terminator, so 254 characters is the maximum.
  std::string s(254, 'z');
  std::vector<uint8_t> buf(512);
  int wcount = 0;
  MultiAddString(s.c_str(), buf.data(), &wcount);
  EXPECT_EQ(wcount, 256);

  std::vector<char> out(300, 'x');
  int rcount = 0;
  MultiGetString(out.data(), buf.data(), &rcount);
  EXPECT_EQ(std::string(out.data()), s);
  EXPECT_EQ(rcount, wcount);
}

TEST(MultiPack, ExtremeValuesRoundTrip) {
  uint8_t buf[64] = {};
  int wcount = 0;
  MultiAddShort(INT16_MIN, buf, &wcount);
  MultiAddShort(INT16_MAX, buf, &wcount);
  MultiAddUshort(UINT16_MAX, buf, &wcount);
  MultiAddInt(INT_MAX, buf, &wcount);
  MultiAddUint(UINT32_MAX, buf, &wcount);
  MultiAddFloat(FLT_MAX, buf, &wcount);
  MultiAddFloat(-FLT_MIN, buf, &wcount);
  MultiAddFloat(std::numeric_limits<float>::infinity(), buf, &wcount);

  int rcount = 0;
  EXPECT_EQ(MultiGetShort(buf, &rcount), INT16_MIN);
  EXPECT_EQ(MultiGetShort(buf, &rcount), INT16_MAX);
  EXPECT_EQ(MultiGetUshort(buf, &rcount), UINT16_MAX);
  EXPECT_EQ(MultiGetInt(buf, &rcount), INT_MAX);
  EXPECT_EQ(MultiGetUint(buf, &rcount), UINT32_MAX);
  EXPECT_EQ(MultiGetFloat(buf, &rcount), FLT_MAX);
  EXPECT_EQ(MultiGetFloat(buf, &rcount), -FLT_MIN);
  EXPECT_EQ(MultiGetFloat(buf, &rcount), std::numeric_limits<float>::infinity());
  EXPECT_EQ(rcount, wcount);
}

} // namespace
