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

#include "dallas_vector_helpers.h"

#include <cmath>
#include <cstdint>

#include <gtest/gtest.h>

namespace dallas_fakes {

int AngleDiff(angle a, angle b) { return static_cast<int16_t>(static_cast<uint16_t>(a - b)); }

vector V(float x, float y, float z) { return vector{x, y, z}; }

float Dot(const vector &a, const vector &b) { return a.x() * b.x() + a.y() * b.y() + a.z() * b.z(); }
float Len(const vector &a) { return std::sqrt(Dot(a, a)); }
vector Cross(const vector &a, const vector &b) {
  return V(a.y() * b.z() - a.z() * b.y(), a.z() * b.x() - a.x() * b.z(), a.x() * b.y() - a.y() * b.x());
}
float Det(const matrix &m) { return Dot(m.rvec, Cross(m.uvec, m.fvec)); }

void ExpectVecNear(const vector &a, const vector &b, float eps) {
  EXPECT_NEAR(a.x(), b.x(), eps);
  EXPECT_NEAR(a.y(), b.y(), eps);
  EXPECT_NEAR(a.z(), b.z(), eps);
}
void ExpectMatNear(const matrix &a, const matrix &b, float eps) {
  for (int i = 0; i < 9; i++)
    EXPECT_NEAR(a.a1d[i], b.a1d[i], eps) << "element " << i;
}

void ExpectRotation(const matrix &m, float eps) {
  EXPECT_NEAR(Len(m.rvec), 1.0f, eps);
  EXPECT_NEAR(Len(m.uvec), 1.0f, eps);
  EXPECT_NEAR(Len(m.fvec), 1.0f, eps);
  EXPECT_NEAR(Dot(m.rvec, m.uvec), 0.0f, eps);
  EXPECT_NEAR(Dot(m.rvec, m.fvec), 0.0f, eps);
  EXPECT_NEAR(Dot(m.uvec, m.fvec), 0.0f, eps);
  EXPECT_NEAR(Det(m), 1.0f, eps);
}

matrix SomeRotation() {
  const double p = 0.3, b = 1.1, h = 2.0;
  matrix m;
  vm_SinCosToMatrix(&m, std::sin(p), std::cos(p), std::sin(b), std::cos(b), std::sin(h), std::cos(h));
  return m;
}

bool IsFinite(const vector &v) { return std::isfinite(v.x()) && std::isfinite(v.y()) && std::isfinite(v.z()); }

} // namespace dallas_fakes
