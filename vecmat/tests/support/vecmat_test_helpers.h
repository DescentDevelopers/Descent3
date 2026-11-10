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

namespace vecmat_test {

// Signed difference a - b of two 16-bit angles, wrapped to [-0x8000, 0x7FFF].
int AngleDiff(angle a, angle b);

// Succeeds when every component of actual is within tol of expected.
::testing::AssertionResult VecNear(const vector &actual, const vector &expected, float tol);
// Succeeds when every row of actual is within tol of expected, componentwise.
::testing::AssertionResult MatNear(const matrix &actual, const matrix &expected, float tol);

// Reference dot product, independent of the code under test.
float Dot(const vector &a, const vector &b);
// Reference vector length, independent of the code under test.
float Len(const vector &a);
// a scaled to unit length, independent of the code under test.
vector Unit(const vector &a);
// Reference determinant by cofactor expansion, independent of the code under test.
float Det(const matrix &m);

// Succeeds when m has unit-length, mutually orthogonal rows and determinant +1 (right-handed).
::testing::AssertionResult IsOrientation(const matrix &m, float tol);

// Non-orthogonal and non-symmetric, with det = 1.
matrix GeneralMatrix();
// det = 2*(3-2) - (-1)*(1-0) + 0 = 3.
matrix OtherMatrix();

// A rigid motion into a tilted plane, so distances and areas are preserved.
vector Tilt(const vector &p);

} // namespace vecmat_test
