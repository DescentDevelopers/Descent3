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

// Every test seeds explicitly because the generator state is global.

#include <cstdint>
#include <vector>

#include <gtest/gtest.h>

#include "psrand.h"

#include "support/psrand_draw.h"

// The generator is "adapted from the C runtime lib" (psrand.cpp), i.e. the Microsoft CRT rand()
// the original Windows release used. These are the well-known first outputs of MSVC's
// srand(1); rand()... sequence.
TEST(PSRand, MatchesMicrosoftCrtSequence) {
  const std::vector<int32_t> expected = {41, 18467, 6334, 26500, 19169, 15724, 11478, 29358, 26962, 24464};
  EXPECT_EQ(Draw(1, static_cast<int>(expected.size())), expected);
}

TEST(PSRand, ReseedResetsStateRegardlessOfHistory) {
  auto fresh = Draw(4242, 50);

  ps_srand(7);
  for (int i = 0; i < 12345; ++i)
    ps_rand();

  EXPECT_EQ(Draw(4242, 50), fresh);
}

// Includes seeds that make the internal state negative.
TEST(PSRand, ValuesStayWithinDocumentedRange) {
  for (uint32_t seed : {1u, 0x7FFFFFFFu, 0x80000000u, 0xFFFFFFFFu}) {
    ps_srand(seed);
    for (int i = 0; i < 100000; ++i) {
      int32_t v = ps_rand();
      ASSERT_GE(v, 0) << "seed " << seed << " draw " << i;
      ASSERT_LE(v, D3_RAND_MAX) << "seed " << seed << " draw " << i;
    }
  }
}
