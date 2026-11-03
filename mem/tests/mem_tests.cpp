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
#include <cstring>

#include <gtest/gtest.h>

#include "mem.h"

// These tests must hold whether mem is built with ENABLE_MEM_RTL (thin CRT wrappers, the default) or
// without it (the library's own heap).

TEST(MemStrdupUnique, NullSourceYieldsNull) { EXPECT_EQ(mem_strdup_unique(nullptr), nullptr); }

TEST(MemStrdupUnique, CopyIsIndependentOfSource) {
  char src[] = "Pyro-GL";
  auto copy = mem_strdup_unique(src);

  ASSERT_NE(copy, nullptr);
  EXPECT_NE(copy.get(), src);
  EXPECT_STREQ(copy.get(), "Pyro-GL");

  src[0] = 'X';
  EXPECT_STREQ(copy.get(), "Pyro-GL");
}

TEST(MemStrdupUnique, EmptyStringYieldsEmptyNonNullBuffer) {
  auto copy = mem_strdup_unique("");

  ASSERT_NE(copy, nullptr);
  EXPECT_EQ(copy[0], '\0');
}

class MemAlloc : public ::testing::Test {
protected:
  // Without mem_Init() the non-RTL build has no heap to allocate from. The library's own static destructor
  // calls mem_shutdown() at exit.
  static void SetUpTestSuite() { mem_Init(); }
};

TEST_F(MemAlloc, StrdupCopiesStringIntoIndependentBlock) {
  char src[] = "Mercenary";
  char *copy = mem_strdup(src);

  ASSERT_NE(copy, nullptr);
  EXPECT_NE(copy, src);
  EXPECT_STREQ(copy, "Mercenary");

  src[0] = 'X';
  EXPECT_STREQ(copy, "Mercenary");

  mem_free(copy);
}

TEST_F(MemAlloc, TypedArrayAllocationCoversAllElements) {
  struct Elem {
    int32_t a;
    int32_t b;
    int32_t c;
  };
  constexpr int kCount = 37;

  Elem *arr = mem_rmalloc<Elem>(kCount);
  ASSERT_NE(arr, nullptr);
  EXPECT_GE(mem_size(arr), static_cast<int>(kCount * sizeof(Elem)));

  for (int i = 0; i < kCount; i++) {
    arr[i] = {i, -i, i * 3};
  }
  for (int i = 0; i < kCount; i++) {
    EXPECT_EQ(arr[i].a, i);
    EXPECT_EQ(arr[i].b, -i);
    EXPECT_EQ(arr[i].c, i * 3);
  }

  mem_free(arr);
}

TEST_F(MemAlloc, ReallocGrowPreservesExistingContents) {
  constexpr int kOld = 16;
  constexpr int kNew = 4096;

  auto *p = static_cast<uint8_t *>(mem_malloc(kOld));
  ASSERT_NE(p, nullptr);
  for (int i = 0; i < kOld; i++) {
    p[i] = static_cast<uint8_t>(0xA0 + i);
  }

  auto *q = static_cast<uint8_t *>(mem_realloc(p, kNew));
  ASSERT_NE(q, nullptr);
  EXPECT_GE(mem_size(q), kNew);
  for (int i = 0; i < kOld; i++) {
    EXPECT_EQ(q[i], static_cast<uint8_t>(0xA0 + i)) << "byte " << i;
  }

  std::memset(q + kOld, 0x5A, kNew - kOld);
  EXPECT_EQ(q[kNew - 1], 0x5A);

  mem_free(q);
}

TEST_F(MemAlloc, ReallocShrinkPreservesPrefix) {
  constexpr int kOld = 256;
  constexpr int kNew = 10;

  auto *p = static_cast<uint8_t *>(mem_malloc(kOld));
  ASSERT_NE(p, nullptr);
  for (int i = 0; i < kOld; i++) {
    p[i] = static_cast<uint8_t>(i);
  }

  auto *q = static_cast<uint8_t *>(mem_realloc(p, kNew));
  ASSERT_NE(q, nullptr);
  EXPECT_GE(mem_size(q), kNew);
  for (int i = 0; i < kNew; i++) {
    EXPECT_EQ(q[i], static_cast<uint8_t>(i)) << "byte " << i;
  }

  mem_free(q);
}
