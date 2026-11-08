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

#include "osiris_vector.h"

namespace dallas_fakes {

// Default tolerance for the near-equality helpers.
constexpr float kEps = 1e-5f;

// Signed difference a - b of two 16-bit angles, wrapped to [-0x8000, 0x7FFF].
int AngleDiff(angle a, angle b);

// Builds a vector from its components.
vector V(float x, float y, float z);

// Reference dot product, independent of the code under test.
float Dot(const vector &a, const vector &b);
// Reference vector length, independent of the code under test.
float Len(const vector &a);
// Reference cross product, independent of the code under test.
vector Cross(const vector &a, const vector &b);
// Reference determinant (triple product of the rows), independent of the code under test.
float Det(const matrix &m);

// Expects every component of a and b to be within eps.
void ExpectVecNear(const vector &a, const vector &b, float eps = kEps);
// Expects every element of a and b to be within eps.
void ExpectMatNear(const matrix &a, const matrix &b, float eps = kEps);
// Expects m to be a proper rotation: orthonormal rows with determinant +1.
void ExpectRotation(const matrix &m, float eps = 1e-4f);

// An exact rotation built from libm sin/cos (vm_AnglesToMatrix goes through the low-precision trig
// tables in fix.cpp, whose ~3e-4 error would swamp the checks of the matrix-multiply functions).
matrix SomeRotation();

// True when every component of v is finite.
bool IsFinite(const vector &v);

} // namespace dallas_fakes
