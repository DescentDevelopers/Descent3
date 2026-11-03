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

#include "fvi_checks.h"

#include <cmath>

#include <gtest/gtest.h>

#include "findintersection.h"

#include "vector_helpers.h"

namespace physics_test {

bool RayHitsBox(const vector &mn, const vector &mx, const vector &origin, const vector &dir) {
  return FastVectorBBox(mn.data(), mx.data(), origin.data(), dir.data());
}

int SweepSphereToFace(Polygon &face, const vector &p0, const vector &p1, float rad, FaceContact &contact) {
  return check_line_to_face(&contact.newp, &contact.colp, &contact.dist, &contact.wall_norm, &p0, &p1, &face.normal,
                            face.list(), face.nv(), rad);
}

void ExpectDropNearVertexTouchesVertex(Polygon &face, const vector &vertex, vector offset_dir, int expected_only_edge,
                                       float tol) {
  vm_NormalizeVector(&offset_dir);
  const vector target = vertex + offset_dir * 0.5f;

  vector t = target;
  ASSERT_EQ(1u << expected_only_edge, check_point_to_face(&t, &face.normal, face.nv(), face.list()));

  const vector p0 = target + face.normal * 5.0f;
  const vector p1 = target - face.normal * 5.0f;
  vector newp, colp, wall_norm;
  float dist;
  ASSERT_NE(0,
            check_line_to_face(&newp, &colp, &dist, &wall_norm, &p0, &p1, &face.normal, face.list(), face.nv(), 1.0f));

  // Centre touches the vertex when it is sqrt(1 - 0.5^2) above the plane.
  const float h = std::sqrt(1.0f - 0.25f);
  const vector expected_centre = target + face.normal * h;
  EXPECT_VEC_NEAR(expected_centre, newp, tol);
  EXPECT_NEAR(5.0f - h, dist, tol);
  EXPECT_VEC_NEAR(vertex, colp, tol);
  EXPECT_VEC_NEAR(Normalized(expected_centre - vertex), wall_norm, tol);
}

} // namespace physics_test
