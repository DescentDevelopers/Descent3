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

#include <algorithm>
#include <cmath>
#include <random>
#include <vector>

#include <gtest/gtest.h>

#include "findintersection.h"
#include "vecmat.h"

#include "support/fvi_checks.h"
#include "support/fvi_polygon.h"
#include "support/vector_helpers.h"

namespace {

using physics_test::ExpectDropNearVertexTouchesVertex;
using physics_test::FaceContact;
using physics_test::Normalized;
using physics_test::Polygon;
using physics_test::RayHitsBox;
using physics_test::SweepSphereToFace;
using physics_test::V;

constexpr float kTol = 1e-3f;

TEST(FastVectorBBox, OriginInsideBoxHitsRegardlessOfDirection) {
  const vector mn = V(-1, -1, -1), mx = V(1, 1, 1);
  EXPECT_TRUE(RayHitsBox(mn, mx, V(0.5f, -0.5f, 0.9f), V(0, 0, 0)));
  EXPECT_TRUE(RayHitsBox(mn, mx, V(0, 0, 0), V(1, 0, 0)));
  EXPECT_TRUE(RayHitsBox(mn, mx, V(0, 0, 0), V(-3, 2, -1)));
}

TEST(FastVectorBBox, RayAimedAtBoxFromEachSideHits) {
  const vector mn = V(-1, -1, -1), mx = V(1, 1, 1);
  for (int axis = 0; axis < 3; axis++) {
    for (float sign : {-1.0f, 1.0f}) {
      vector origin = V(0.2f, -0.3f, 0.1f);
      origin[axis] = sign * 10.0f;
      vector dir = V(0, 0, 0);
      dir[axis] = -sign * 5.0f;
      EXPECT_TRUE(RayHitsBox(mn, mx, origin, dir)) << "axis " << axis << " sign " << sign;
    }
  }
}

TEST(FastVectorBBox, RayPointingAwayFromBoxMisses) {
  const vector mn = V(-1, -1, -1), mx = V(1, 1, 1);
  EXPECT_FALSE(RayHitsBox(mn, mx, V(5, 0, 0), V(1, 0, 0)));
  EXPECT_FALSE(RayHitsBox(mn, mx, V(0, -5, 0), V(0.1f, -1, 0.1f)));
}

TEST(FastVectorBBox, RayThatOvershootsTheBoxMisses) {
  const vector mn = V(-1, -1, -1), mx = V(1, 1, 1);
  // Reaches the z = -1 face plane only at x = 35, far past the box.
  EXPECT_FALSE(RayHitsBox(mn, mx, V(-5, 0, -5), V(1, 0, 0.1f)));
}

TEST(FastVectorBBox, RayAimedAtAnEdgeSharedByTwoFacesHitsOneOfThem) {
  struct EdgeRay {
    float x0, y0, y1, z0, w;
    vector origin, dir;
  };
  const EdgeRay rays[] = {
      {15.2155762f, -586.900513f, -574.689331f, 11.1446533f, 34.4990692f, V(43.0987511f, -585.120483f, 23.583374f),
       V(-25.490881f, -0.199814513f, -11.3715153f)},
      {15.2155762f, -586.900513f, -574.689331f, 11.1446533f, 34.4990692f, V(-12.6675987f, -585.120483f, 23.583374f),
       V(25.490881f, -0.199814513f, -11.3715153f)},
      {40.5061035f, -1111.7417f, -1102.65808f, -33.9718018f, 43.8956566f, V(0.559703827f, -1111.37952f, 0.401409149f),
       V(76.1575241f, 8.76260567f, -65.53228f)},
  };

  for (const EdgeRay &r : rays) {
    SCOPED_TRACE(testing::Message() << "edge x=" << r.x0 << " z=" << r.z0);
    const vector left_floor_mn = V(r.x0 - r.w, r.y0, r.z0), left_floor_mx = V(r.x0, r.y1, r.z0);
    const vector right_floor_mn = V(r.x0, r.y0, r.z0), right_floor_mx = V(r.x0 + r.w, r.y1, r.z0);
    const vector wall_mn = V(r.x0, r.y0, r.z0), wall_mx = V(r.x0, r.y1, r.z0 + r.w);
    const bool left_floor = RayHitsBox(left_floor_mn, left_floor_mx, r.origin, r.dir);
    EXPECT_TRUE(left_floor || RayHitsBox(right_floor_mn, right_floor_mx, r.origin, r.dir));
    if (r.origin.x() < r.x0)
      EXPECT_TRUE(left_floor || RayHitsBox(wall_mn, wall_mx, r.origin, r.dir));
  }
}

TEST(FastVectorBBox, NeverRejectsASegmentThatEntersTheBox) {
  const vector mn = V(-2, -1, -3), mx = V(4, 1, 0.5f);
  std::mt19937 rng(12345);
  std::uniform_real_distribution<float> coord(-10.0f, 10.0f);

  auto inside = [&](const vector &p) {
    return p.x() >= mn.x() && p.x() <= mx.x() && p.y() >= mn.y() && p.y() <= mx.y() && p.z() >= mn.z() &&
           p.z() <= mx.z();
  };

  int entering = 0;
  for (int iter = 0; iter < 2000; iter++) {
    const vector p0 = V(coord(rng), coord(rng), coord(rng));
    const vector p1 = V(coord(rng), coord(rng), coord(rng));
    const vector delta = p1 - p0;
    bool enters = false;
    for (int s = 0; s <= 200 && !enters; s++)
      enters = inside(p0 + delta * (s / 200.0f));
    if (enters) {
      entering++;
      EXPECT_TRUE(RayHitsBox(mn, mx, p0, delta)) << "p0=(" << p0.x() << "," << p0.y() << "," << p0.z() << ") delta=("
                                                 << delta.x() << "," << delta.y() << "," << delta.z() << ")";
    }
  }
  EXPECT_GT(entering, 100);
}

// check_point_to_face returns a bitmask with bit k set when the point (projected onto the
// face's plane) lies outside edge k (vertex k -> vertex k+1). Zero means inside.

TEST(CheckPointToFace, ClassifiesPointsForSquaresFacingEveryAxisDirection) {
  for (int axis = 0; axis < 3; axis++) {
    const int a = (axis + 1) % 3, b = (axis + 2) % 3;
    for (float facing : {1.0f, -1.0f}) {
      std::vector<vector> corners;
      const float ca[4] = {-1, 1, 1, -1};
      const float cb[4] = {-1, -1, 1, 1};
      for (int k = 0; k < 4; k++) {
        vector v = V(0, 0, 0);
        v[axis] = 3.0f;
        v[a] = ca[k];
        v[b] = cb[k];
        corners.push_back(v);
      }
      if (facing < 0)
        std::reverse(corners.begin(), corners.end());
      Polygon sq(corners);
      SCOPED_TRACE(testing::Message() << "axis " << axis << " normal " << sq.normal.x() << "," << sq.normal.y()
                                      << "," << sq.normal.z());
      ASSERT_NEAR(std::fabs(sq.normal[axis]), 1.0f, kTol);

      vector centre = V(0, 0, 0);
      centre[axis] = 3.0f;
      EXPECT_EQ(0u, check_point_to_face(&centre, &sq.normal, sq.nv(), sq.list()));

      for (int k = 0; k < 4; k++) {
        const vector mid = (sq.verts[k] + sq.verts[(k + 1) % 4]) * 0.5f;
        vector beyond = centre + (mid - centre) * 1.2f;
        EXPECT_EQ(1u << k, check_point_to_face(&beyond, &sq.normal, sq.nv(), sq.list())) << "edge " << k;
      }
    }
  }
}

TEST(CheckPointToFace, CornerRegionIsOutsideBothAdjacentEdges) {
  Polygon sq({V(0, 0, 0), V(0, 0, 4), V(4, 0, 4), V(4, 0, 0)});
  // Beyond vertex 2, i.e. outside edge 1 (v1->v2) and edge 2 (v2->v3).
  vector p = V(5, 0, 5);
  EXPECT_EQ((1u << 1) | (1u << 2), check_point_to_face(&p, &sq.normal, sq.nv(), sq.list()));
}

TEST(CheckPointToFace, TiltedTriangleClassifiesCentroidAndOutsidePoints) {
  Polygon tri({V(1, 0, 0), V(0, 2, 0), V(0, 0, 3)});
  const vector centroid = (tri.verts[0] + tri.verts[1] + tri.verts[2]) / 3.0f;
  vector c = centroid;
  EXPECT_EQ(0u, check_point_to_face(&c, &tri.normal, tri.nv(), tri.list()));

  for (int k = 0; k < 3; k++) {
    // Reflect the centroid across edge k's midpoint; that lands outside edge k only.
    const vector mid = (tri.verts[k] + tri.verts[(k + 1) % 3]) * 0.5f;
    vector out = mid + (mid - centroid);
    EXPECT_EQ(1u << k, check_point_to_face(&out, &tri.normal, tri.nv(), tri.list())) << "edge " << k;
  }
}

TEST(CheckVectorToSphere, HeadOnHitStopsAtSphereSurface) {
  const vector p0 = V(0, 0, -10), p1 = V(0, 0, 10);
  vector centre = V(0, 0, 0);
  vector intp;
  float dist = -1;
  ASSERT_EQ(1, check_vector_to_sphere_1(&intp, &dist, &p0, &p1, &centre, 2.0f, false, false));
  EXPECT_NEAR(8.0f, dist, kTol);
  EXPECT_VEC_NEAR(V(0, 0, -2), intp, kTol);
}

TEST(CheckVectorToSphere, OffCentreHitFollowsPythagoras) {
  // Line passes 1.2 from the centre of a radius-2 sphere: it enters sqrt(4 - 1.44) = 1.6
  // before the point of closest approach (z = 0).
  const vector p0 = V(1.2f, 0, -10), p1 = V(1.2f, 0, 10);
  vector centre = V(0, 0, 0);
  vector intp;
  float dist = -1;
  ASSERT_EQ(1, check_vector_to_sphere_1(&intp, &dist, &p0, &p1, &centre, 2.0f, false, false));
  EXPECT_NEAR(10.0f - 1.6f, dist, kTol);
  EXPECT_VEC_NEAR(V(1.2f, 0, -1.6f), intp, kTol);
  EXPECT_NEAR(2.0f, vm_VectorDistance(&intp, &centre), kTol);
}

TEST(CheckVectorToSphere, MissesWhenPassingOutsideRadius) {
  const vector p0 = V(2.5f, 0, -10), p1 = V(2.5f, 0, 10);
  vector centre = V(0, 0, 0);
  vector intp;
  float dist;
  EXPECT_EQ(0, check_vector_to_sphere_1(&intp, &dist, &p0, &p1, &centre, 2.0f, false, false));
}

TEST(CheckVectorToSphere, MissesWhenSegmentEndsBeforeReachingSphere) {
  const vector p0 = V(0, 0, -10), p1 = V(0, 0, -2.5f);
  vector centre = V(0, 0, 0);
  vector intp;
  float dist;
  EXPECT_EQ(0, check_vector_to_sphere_1(&intp, &dist, &p0, &p1, &centre, 2.0f, false, false));
}

TEST(CheckVectorToSphere, MissesWhenMovingAway) {
  const vector p0 = V(0, 0, -3), p1 = V(0, 0, -10);
  vector centre = V(0, 0, 0);
  vector intp;
  float dist;
  EXPECT_EQ(0, check_vector_to_sphere_1(&intp, &dist, &p0, &p1, &centre, 2.0f, false, false));
}

TEST(CheckVectorToSphere, StartInsideSphereIsOnlyReportedWhenInitialCollisionsRequested) {
  const vector p0 = V(0.5f, 0, -1), p1 = V(0.5f, 0, 10);
  vector centre = V(0, 0, 0);
  vector intp;
  float dist = -1;

  EXPECT_EQ(0, check_vector_to_sphere_1(&intp, &dist, &p0, &p1, &centre, 2.0f, false, false));

  ASSERT_EQ(1, check_vector_to_sphere_1(&intp, &dist, &p0, &p1, &centre, 2.0f, false, true));
  EXPECT_FLOAT_EQ(0.0f, dist);
  EXPECT_VEC_NEAR(p0, intp, kTol);
}

// check_line_to_face: moving sphere (radius may be 0) vs. a one-sided polygon.
//   newp     - sphere centre at first contact
//   colp     - contact point on the polygon
//   col_dist - distance travelled from p0 to newp
//   wall_norm- contact normal (points from the contact point towards the sphere centre)

class CheckLineToFace : public testing::Test {
protected:
  // The engine normal is +y or -y depending on winding; `up` is that normal so approach
  // directions follow it.
  Polygon square{{V(-5, 0, -5), V(-5, 0, 5), V(5, 0, 5), V(5, 0, -5)}};
  vector up;

  void SetUp() override {
    up = square.normal;
    ASSERT_NEAR(1.0f, std::fabs(up.y()), kTol);
  }

  FaceContact hit;
};

TEST_F(CheckLineToFace, PointThroughInteriorHitsAtPlane) {
  const vector p0 = V(1, 0, 2) + up * 8.0f;
  const vector p1 = V(1, 0, 2) - up * 8.0f;
  ASSERT_NE(0, SweepSphereToFace(square, p0, p1, 0.0f, hit));
  EXPECT_VEC_NEAR(V(1, 0, 2), hit.newp, kTol);
  EXPECT_VEC_NEAR(V(1, 0, 2), hit.colp, kTol);
  EXPECT_NEAR(8.0f, hit.dist, kTol);
  EXPECT_VEC_NEAR(up, hit.wall_norm, kTol);
}

TEST_F(CheckLineToFace, ObliquePointHitsWherePathCrossesPlane) {
  // From (-3, 6) to (3, -6) in (x, height) along the path the plane is crossed half way, at x = 0.
  const vector p0 = V(-3, 0, 1) + up * 6.0f;
  const vector p1 = V(3, 0, 1) - up * 6.0f;
  ASSERT_NE(0, SweepSphereToFace(square, p0, p1, 0.0f, hit));
  EXPECT_VEC_NEAR(V(0, 0, 1), hit.newp, kTol);
  EXPECT_NEAR(0.5f * vm_VectorDistance(&p0, &p1), hit.dist, kTol);
}

TEST_F(CheckLineToFace, PointCrossingPlaneOutsidePolygonMisses) {
  const vector p0 = V(7, 0, 0) + up * 8.0f;
  const vector p1 = V(7, 0, 0) - up * 8.0f;
  EXPECT_EQ(0, SweepSphereToFace(square, p0, p1, 0.0f, hit));
}

TEST_F(CheckLineToFace, FacesAreOneSided) {
  const vector p0 = V(1, 0, 2) - up * 8.0f;
  const vector p1 = V(1, 0, 2) + up * 8.0f;
  EXPECT_EQ(0, SweepSphereToFace(square, p0, p1, 0.0f, hit));
  EXPECT_EQ(0, SweepSphereToFace(square, p0, p1, 1.0f, hit));
}

TEST_F(CheckLineToFace, SegmentEndingInFrontOfFaceMisses) {
  const vector p0 = V(0, 0, 0) + up * 8.0f;
  const vector p1 = V(0, 0, 0) + up * 1.5f;
  EXPECT_EQ(0, SweepSphereToFace(square, p0, p1, 1.0f, hit));
}

TEST_F(CheckLineToFace, SphereStopsOneRadiusInFrontOfFace) {
  const vector p0 = V(2, 0, -1) + up * 10.0f;
  const vector p1 = V(2, 0, -1) - up * 10.0f;
  ASSERT_NE(0, SweepSphereToFace(square, p0, p1, 1.5f, hit));
  EXPECT_VEC_NEAR(V(2, 0, -1) + up * 1.5f, hit.newp, kTol);
  EXPECT_VEC_NEAR(V(2, 0, -1), hit.colp, kTol);
  EXPECT_NEAR(10.0f - 1.5f, hit.dist, kTol);
  EXPECT_VEC_NEAR(up, hit.wall_norm, kTol);
}

TEST_F(CheckLineToFace, SphereAlreadyTouchingPlaneCollidesImmediately) {
  const vector p0 = V(0, 0, 0) + up * 0.5f;
  const vector p1 = V(0, 0, 0) - up * 5.0f;
  ASSERT_NE(0, SweepSphereToFace(square, p0, p1, 1.0f, hit));
  EXPECT_VEC_NEAR(p0, hit.newp, kTol);
}

TEST_F(CheckLineToFace, SphereGrazingAnEdgeTouchesTheEdge) {
  // The centre descends 0.6 outside the x = 5 edge. A radius-1 sphere first touches the
  // edge when the centre is sqrt(1 - 0.6^2) = 0.8 above the plane.
  const vector start = V(5.6f, 0, 1) + up * 10.0f;
  const vector end = V(5.6f, 0, 1) - up * 10.0f;
  ASSERT_NE(0, SweepSphereToFace(square, start, end, 1.0f, hit));
  EXPECT_VEC_NEAR(V(5.6f, 0, 1) + up * 0.8f, hit.newp, kTol);
  EXPECT_NEAR(10.0f - 0.8f, hit.dist, kTol);
  EXPECT_VEC_NEAR(V(5, 0, 1), hit.colp, kTol);
  EXPECT_VEC_NEAR(Normalized(V(0.6f, 0, 0) + up * 0.8f), hit.wall_norm, kTol);
}

TEST_F(CheckLineToFace, SphereFarOutsideEdgeMisses) {
  const vector start = V(6.2f, 0, 1) + up * 10.0f;
  const vector end = V(6.2f, 0, 1) - up * 10.0f;
  EXPECT_EQ(0, SweepSphereToFace(square, start, end, 1.0f, hit));
}

// An acute triangle has, next to each vertex, regions that are outside exactly one edge
// yet closer to the vertex than to any edge interior. A sphere dropped straight onto such
// a region touches only the vertex.
class CheckLineToFaceVertex : public testing::Test {
protected:
  // Edges: 0 = A->B, 1 = B->C, 2 = C->A.
  vector A = V(0, 0, 0), B = V(5, 0, 8.66f), C = V(10, 0, 0);
  Polygon tri{{A, B, C}};

  void SetUp() override { ASSERT_NEAR(1.0f, tri.normal.y(), kTol); }
};

TEST_F(CheckLineToFaceVertex, SphereHittingVertexAtEndOfCrossedEdgeTouchesThatVertex) {
  ExpectDropNearVertexTouchesVertex(tri, A, V(-0.2f, 0, -1), 2, kTol);
}

} // namespace
