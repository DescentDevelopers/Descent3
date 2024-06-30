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

#include <gtest/gtest.h>

#include "byteswap.h"

// This code taken from original byteswap.h for testing float conversion
// It cannot convert negative float numbers in 64-bit systems, so testing only non-negative numbers

#define SWAPINT(x) (int)(((x) << 24) | (((uint32_t)(x)) >> 24) | (((x) & 0x0000ff00) << 8) | (((x) & 0x00ff0000) >> 8))

// Stupid function to trick the compiler into letting me byteswap a float
inline float SWAPFLOAT(float x) {
  int i = SWAPINT(*((int *)&(x)));
  return *((float *)&(i));
}

// Taken from CFILE.cpp
inline double SWAPDOUBLE(double x) {
  double t;
  int *sp = (int *)&x;
  int *dp = (int *)&t;
  dp[0] = SWAPINT(sp[1]);
  dp[1] = SWAPINT(sp[0]);
  x = t;

  return x;
}

TEST(D3, ByteSwap) {
  EXPECT_EQ(D3::byteswap((uint8_t)0x01), (uint8_t)0x01);
  EXPECT_EQ(D3::byteswap((int8_t)0x01), (int16_t)0x01);
  EXPECT_EQ(D3::byteswap((uint16_t)0x0123), (uint16_t)0x2301);
  EXPECT_EQ(D3::byteswap((int16_t)0x0123), (int16_t)0x2301);
  EXPECT_EQ(D3::byteswap((int32_t)0x01234567), (int32_t)0x67452301);
  EXPECT_EQ(D3::byteswap((int64_t)0x0123456789ABCDEF), (int64_t)0xEFCDAB8967452301);

  EXPECT_EQ(D3::byteswap((int32_t)(0xFF000000)), (int32_t)0xFF);
}

TEST(D3, Converting) {
#ifndef OUTRAGE_BIG_ENDIAN
  EXPECT_EQ(D3::convert_le((int16_t)0x10), 0x10);
  EXPECT_EQ(D3::convert_le((int32_t)0x10), 0x10);
  EXPECT_EQ(D3::convert_le((int64_t)0x10), 0x10);

  EXPECT_EQ(D3::convert_be((int16_t)0x10), 0x1000);
  EXPECT_EQ(D3::convert_be((int32_t)0x10), 0x10000000);
  EXPECT_EQ(D3::convert_be((int64_t)0x10), 0x1000000000000000);
#else
  EXPECT_EQ(D3::convert_be((int16_t)0x10), 0x10);
  EXPECT_EQ(D3::convert_be((int32_t)0x10), 0x10);
  EXPECT_EQ(D3::convert_be((int64_t)0x10), 0x10);

  EXPECT_EQ(D3::convert_le((int16_t)0x10), 0x1000);
  EXPECT_EQ(D3::convert_le((int16_t)0x10), 0x10000000);
  EXPECT_EQ(D3::convert_le((int16_t)0x10), 0x1000000000000000);
#endif
}

TEST(D3, FloatDoubleOperations) {
  // float/double sanity checks
  EXPECT_EQ(sizeof(float), 4);
  EXPECT_EQ(sizeof(double), 8);

  EXPECT_EQ(D3::byteswap(0.0f), SWAPFLOAT(0.0f));
  EXPECT_EQ(D3::byteswap(10.0f), SWAPFLOAT(10.0f));
  EXPECT_EQ(D3::byteswap(10000.0f), SWAPFLOAT(10000.0f));

  EXPECT_EQ(D3::byteswap(0.0), SWAPDOUBLE(0.0));
  EXPECT_EQ(D3::byteswap(10.0), SWAPDOUBLE(10.0));
  EXPECT_EQ(D3::byteswap(10000.0), SWAPDOUBLE(10000.0));
}
