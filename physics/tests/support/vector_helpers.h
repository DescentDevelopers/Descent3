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

#pragma once

#include <gtest/gtest.h>

#include "vecmat.h"

namespace physics_test {

// Builds a vector from its components.
vector V(float x, float y, float z);

// Returns v scaled to unit length.
vector Normalized(vector v);

} // namespace physics_test

// Expects each component of actual to be within tol of the same component of expected.
#define EXPECT_VEC_NEAR(expected, actual, tol)                                                                         \
  do {                                                                                                                 \
    const vector e_ = (expected);                                                                                      \
    const vector a_ = (actual);                                                                                        \
    EXPECT_NEAR(e_.x(), a_.x(), tol) << "x component";                                                                 \
    EXPECT_NEAR(e_.y(), a_.y(), tol) << "y component";                                                                 \
    EXPECT_NEAR(e_.z(), a_.z(), tol) << "z component";                                                                 \
  } while (0)
