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

#include "vecmat_test_helpers.h"

#include <cmath>
#include <cstdint>

namespace vecmat_test {

int AngleDiff(angle a, angle b) { return static_cast<int16_t>(static_cast<uint16_t>(a - b)); }

::testing::AssertionResult VecNear(const vector &actual, const vector &expected, float tol) {
  for (int i = 0; i < 3; i++) {
    if (!(std::fabs(actual[i] - expected[i]) <= tol)) {
      return ::testing::AssertionFailure()
             << "(" << actual.x() << ", " << actual.y() << ", " << actual.z() << ") != expected (" << expected.x()
             << ", " << expected.y() << ", " << expected.z() << ") within " << tol;
    }
  }
  return ::testing::AssertionSuccess();
}

::testing::AssertionResult MatNear(const matrix &actual, const matrix &expected, float tol) {
  const char *names[3] = {"rvec", "uvec", "fvec"};
  const vector *a[3] = {&actual.rvec, &actual.uvec, &actual.fvec};
  const vector *e[3] = {&expected.rvec, &expected.uvec, &expected.fvec};
  for (int i = 0; i < 3; i++) {
    auto r = VecNear(*a[i], *e[i], tol);
    if (!r)
      return ::testing::AssertionFailure() << names[i] << ": " << r.message();
  }
  return ::testing::AssertionSuccess();
}

float Dot(const vector &a, const vector &b) { return a.x() * b.x() + a.y() * b.y() + a.z() * b.z(); }
float Len(const vector &a) { return std::sqrt(Dot(a, a)); }
vector Unit(const vector &a) { return a / Len(a); }

float Det(const matrix &m) {
  const vector &r = m.rvec, &u = m.uvec, &f = m.fvec;
  return r.x() * (u.y() * f.z() - u.z() * f.y()) - r.y() * (u.x() * f.z() - u.z() * f.x()) +
         r.z() * (u.x() * f.y() - u.y() * f.x());
}

::testing::AssertionResult IsOrientation(const matrix &m, float tol) {
  const vector *rows[3] = {&m.rvec, &m.uvec, &m.fvec};
  for (int i = 0; i < 3; i++) {
    if (!(std::fabs(Len(*rows[i]) - 1.0f) <= tol))
      return ::testing::AssertionFailure() << "row " << i << " has length " << Len(*rows[i]);
    for (int j = i + 1; j < 3; j++) {
      if (!(std::fabs(Dot(*rows[i], *rows[j])) <= tol))
        return ::testing::AssertionFailure() << "rows " << i << " and " << j << " have dot " << Dot(*rows[i], *rows[j]);
    }
  }
  if (!(std::fabs(Det(m) - 1.0f) <= tol))
    return ::testing::AssertionFailure() << "determinant is " << Det(m) << " (expected +1, right-handed)";
  return ::testing::AssertionSuccess();
}

matrix GeneralMatrix() { return matrix{vector{1, 2, 3}, vector{0, 1, 4}, vector{5, 6, 0}}; }

matrix OtherMatrix() { return matrix{vector{2, -1, 0}, vector{1, 3, 2}, vector{0, 1, 1}}; }

vector Tilt(const vector &p) { return vector{p.x() + 10, 0.6f * p.y() - 3, 0.8f * p.y() + 7}; }

} // namespace vecmat_test
