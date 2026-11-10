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

#include "renderer_checks.h"

#include <gtest/gtest.h>

namespace {

constexpr float kPosTol = 1e-4f;
constexpr float kRotationTol = 1e-3f;

} // namespace

g3Codes CodesOf(g3Point **pts, int n) {
  g3Codes cc;
  cc.cc_or = 0;
  cc.cc_and = 0xff;
  for (int i = 0; i < n; ++i) {
    cc.cc_or |= pts[i]->p3_codes;
    cc.cc_and &= pts[i]->p3_codes;
  }
  return cc;
}

int IndexOf(g3Point **list, int n, const g3Point *target) {
  for (int i = 0; i < n; ++i) {
    if (list[i] == target)
      return i;
  }
  return -1;
}

void ExpectPos(const g3Point *p, float x, float y, float z) {
  EXPECT_NEAR(p->p3_x, x, kPosTol);
  EXPECT_NEAR(p->p3_y, y, kPosTol);
  EXPECT_NEAR(p->p3_z, z, kPosTol);
}

void ExpectUV(const g3Point *p, float u, float v) {
  ASSERT_TRUE(p->p3_flags & PF_UV);
  EXPECT_NEAR(p->p3_u, u, kPosTol);
  EXPECT_NEAR(p->p3_v, v, kPosTol);
}

void ExpectSameRotation(const g3Point &actual, const g3Point &expected) {
  EXPECT_NEAR(actual.p3_x, expected.p3_x, kRotationTol);
  EXPECT_NEAR(actual.p3_y, expected.p3_y, kRotationTol);
  EXPECT_NEAR(actual.p3_z, expected.p3_z, kRotationTol);
  EXPECT_EQ(actual.p3_codes, expected.p3_codes);
}
