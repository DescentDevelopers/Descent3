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

// Most tests build points directly in rotated (view) space, where the view
// pyramid is |x| <= z, |y| <= z, z >= 0.

#include <cmath>

#include <gtest/gtest.h>

#include "3d.h"
#include "HardwareInternal.h"
#include "support/renderer_checks.h"
#include "support/renderer_helpers.h"

namespace {

constexpr float kTol = 1e-4f;

class ClipperTest : public testing::Test {
protected:
  void SetUp() override {
    matrix ident;
    vm_MakeIdentity(&ident);
    StartTestFrame(640, 480, vector{0.0f, 0.0f, 0.0f}, ident, 1.0f);
  }
};

TEST_F(ClipperTest, PolygonClippedByOneSideKeepsWindingAndInterpolatesUV) {
  g3Point a = MakeUVPoint(0.0f, 0.0f, 2.0f, 0.0f, 0.0f);
  g3Point b = MakeUVPoint(4.0f, 0.0f, 2.0f, 1.0f, 0.0f);
  g3Point c = MakeUVPoint(0.0f, 1.0f, 2.0f, 0.0f, 1.0f);
  ASSERT_EQ(b.p3_codes, CC_OFF_RIGHT);

  g3Point *in[] = {&a, &b, &c};
  int nv = 3;
  g3Codes cc = CodesOf(in, nv);
  g3Point **out = g3_ClipPolygon(in, &nv, &cc);

  ASSERT_EQ(nv, 4);
  EXPECT_EQ(cc.cc_and, 0);
  EXPECT_EQ(cc.cc_or, 0);

  int ia = IndexOf(out, nv, &a);
  ASSERT_NE(ia, -1);
  const g3Point *ab = out[(ia + 1) % 4];
  const g3Point *bc = out[(ia + 2) % 4];
  EXPECT_EQ(out[(ia + 3) % 4], &c);

  // a-b crosses x = 2 halfway: (2, 0, 2), uv (0.5, 0).
  ExpectPos(ab, 2.0f, 0.0f, 2.0f);
  ExpectUV(ab, 0.5f, 0.0f);
  // b-c crosses x = 2 halfway: (2, 0.5, 2), uv (0.5, 0.5).
  ExpectPos(bc, 2.0f, 0.5f, 2.0f);
  ExpectUV(bc, 0.5f, 0.5f);

  for (int i = 0; i < nv; ++i)
    EXPECT_EQ(out[i]->p3_codes, 0) << "vertex " << i;

  g3_FreeTempPoints(out, nv);
}

TEST_F(ClipperTest, PolygonLargerThanViewIsClippedToViewSquare) {
  // At z = 1 the view is the square |x| <= 1, |y| <= 1.
  g3Point p0 = MakePoint(-3.0f, -3.0f, 1.0f);
  g3Point p1 = MakePoint(3.0f, -3.0f, 1.0f);
  g3Point p2 = MakePoint(3.0f, 3.0f, 1.0f);
  g3Point p3 = MakePoint(-3.0f, 3.0f, 1.0f);

  g3Point *in[] = {&p0, &p1, &p2, &p3};
  int nv = 4;
  g3Codes cc = CodesOf(in, nv);
  ASSERT_EQ(cc.cc_and, 0);
  g3Point **out = g3_ClipPolygon(in, &nv, &cc);

  ASSERT_EQ(nv, 4);
  EXPECT_EQ(cc.cc_and, 0);
  bool seen[2][2] = {};
  for (int i = 0; i < nv; ++i) {
    SCOPED_TRACE(testing::Message() << "vertex " << i);
    EXPECT_NEAR(std::abs(out[i]->p3_x), 1.0f, kTol);
    EXPECT_NEAR(std::abs(out[i]->p3_y), 1.0f, kTol);
    EXPECT_NEAR(out[i]->p3_z, 1.0f, kTol);
    seen[out[i]->p3_x > 0][out[i]->p3_y > 0] = true;
  }
  EXPECT_TRUE(seen[0][0] && seen[0][1] && seen[1][0] && seen[1][1]);

  g3_FreeTempPoints(out, nv);
}

TEST_F(ClipperTest, PolygonOutsideCornerOfViewIsReportedClippedAway) {
  // At z = 1 the view is |x|,|y| <= 1. The triangle's nearest edge is the
  // line x + y = 3.5, well clear of the view's corner (1, 1).
  g3Point a = MakePoint(3.0f, 0.5f, 1.0f);
  g3Point b = MakePoint(3.0f, 3.0f, 1.0f);
  g3Point c = MakePoint(0.5f, 3.0f, 1.0f);

  g3Point *in[] = {&a, &b, &c};
  int nv = 3;
  g3Codes cc = CodesOf(in, nv);
  ASSERT_EQ(cc.cc_and, 0);
  g3Point **out = g3_ClipPolygon(in, &nv, &cc);

  EXPECT_TRUE(nv == 0 || cc.cc_and != 0);

  g3_FreeTempPoints(out, nv);
}

TEST_F(ClipperTest, PolygonIsClippedAtFarPlane) {
  g3_SetFarClipZ(8.0f);

  g3Point a = MakeUVPoint(-1.0f, -1.0f, 5.0f, 0.0f, 0.0f);
  g3Point b = MakeUVPoint(1.0f, -1.0f, 5.0f, 1.0f, 0.0f);
  g3Point c = MakeUVPoint(1.0f, 1.0f, 15.0f, 1.0f, 1.0f);
  g3Point d = MakeUVPoint(-1.0f, 1.0f, 15.0f, 0.0f, 1.0f);
  ASSERT_EQ(c.p3_codes, CC_OFF_FAR);

  g3Point *in[] = {&a, &b, &c, &d};
  int nv = 4;
  g3Codes cc = CodesOf(in, nv);
  g3Point **out = g3_ClipPolygon(in, &nv, &cc);

  ASSERT_EQ(nv, 4);
  int ia = IndexOf(out, nv, &a);
  ASSERT_NE(ia, -1);
  EXPECT_EQ(out[(ia + 1) % 4], &b);
  // b-c and a-d reach z = 8 three tenths of the way from the near end:
  // y = -1 + 0.3 * 2 = -0.4, v = 0.3.
  const g3Point *bc = out[(ia + 2) % 4];
  const g3Point *da = out[(ia + 3) % 4];
  ExpectPos(bc, 1.0f, -0.4f, 8.0f);
  ExpectUV(bc, 1.0f, 0.3f);
  ExpectPos(da, -1.0f, -0.4f, 8.0f);
  ExpectUV(da, 0.0f, 0.3f);

  g3_FreeTempPoints(out, nv);
}

// Intensity lighting, p3_l, shares storage with p3_r, so it is covered by the
// red channel.
TEST_F(ClipperTest, LineClipInterpolatesAllVertexAttributes) {
  // Left plane is x = -z. From (0,0,1) to (-3,0,1) the line meets x = -1
  // a third of the way along.
  g3Point inside = MakePoint(0.0f, 0.0f, 1.0f);
  g3Point outside = MakePoint(-3.0f, 0.0f, 1.0f);
  ASSERT_EQ(outside.p3_codes, CC_OFF_LEFT);

  inside.p3_flags |= PF_UV | PF_UV2 | PF_RGBA;
  inside.p3_uvl = {};
  outside.p3_flags |= PF_UV | PF_UV2 | PF_RGBA;
  outside.p3_u = 0.3f;
  outside.p3_v = 0.6f;
  outside.p3_u2 = 0.9f;
  outside.p3_v2 = -0.3f;
  outside.p3_r = 0.3f;
  outside.p3_g = 0.6f;
  outside.p3_b = 0.9f;
  outside.p3_a = 1.2f;

  // Pass the outside point first; ClipLine may reorder the pair.
  g3Point *p0 = &outside;
  g3Point *p1 = &inside;
  ClipLine(&p0, &p1, inside.p3_codes | outside.p3_codes);

  g3Point *ends[] = {p0, p1};
  ASSERT_NE(IndexOf(ends, 2, &inside), -1);
  ASSERT_EQ(IndexOf(ends, 2, &outside), -1);
  const g3Point *clipped = (p0 == &inside) ? p1 : p0;

  ExpectPos(clipped, -1.0f, 0.0f, 1.0f);
  EXPECT_EQ(clipped->p3_codes, 0);
  EXPECT_NEAR(clipped->p3_u, 0.1f, kTol);
  EXPECT_NEAR(clipped->p3_v, 0.2f, kTol);
  EXPECT_NEAR(clipped->p3_u2, 0.3f, kTol);
  EXPECT_NEAR(clipped->p3_v2, -0.1f, kTol);
  EXPECT_NEAR(clipped->p3_r, 0.1f, kTol);
  EXPECT_NEAR(clipped->p3_g, 0.2f, kTol);
  EXPECT_NEAR(clipped->p3_b, 0.3f, kTol);
  EXPECT_NEAR(clipped->p3_a, 0.4f, kTol);
  EXPECT_TRUE(clipped->p3_flags & PF_UV);
  EXPECT_TRUE(clipped->p3_flags & PF_UV2);
  EXPECT_TRUE(clipped->p3_flags & PF_RGBA);

  g3_FreeTempPoints(ends, 2);
}

TEST_F(ClipperTest, LineToPointBehindViewerEndsWhereItLeavesView) {
  // Line from (3, 0, 5) to (1, 0, -3): x = 3 - 2t, z = 5 - 8t. The behind
  // point is outside every side plane as well. The line leaves the view
  // through the right plane x = z at t = 1/3, i.e. at (7/3, 0, 7/3).
  g3Point inside = MakePoint(3.0f, 0.0f, 5.0f);
  g3Point behind = MakePoint(1.0f, 0.0f, -3.0f);
  ASSERT_EQ(inside.p3_codes, 0);
  ASSERT_TRUE(behind.p3_codes & CC_BEHIND);

  g3Point *p0 = &inside;
  g3Point *p1 = &behind;
  ClipLine(&p0, &p1, inside.p3_codes | behind.p3_codes);

  g3Point *ends[] = {p0, p1};
  ASSERT_NE(IndexOf(ends, 2, &inside), -1);
  const g3Point *clipped = (p0 == &inside) ? p1 : p0;

  ExpectPos(clipped, 7.0f / 3.0f, 0.0f, 7.0f / 3.0f);
  EXPECT_EQ(clipped->p3_codes, 0);

  g3_FreeTempPoints(ends, 2);
}

// The custom clip plane is given in world space, while rotated points live in
// View_matrix space, which is scaled differently along camera x and y (by
// Matrix_scale; at 640x480 the two scales differ). These tests use a tilted,
// offset camera and a plane whose normal mixes camera x and y, so that any
// mismatch between the two spaces moves the plane.
class CustomClipPlaneTest : public ClipperTest {
protected:
  void SetUp() override {
    ClipperTest::SetUp();
    StartTestFrame(640, 480, view_pos_, orient_, 0.726f);

    // In camera space, keep the half-space x + y >= 1. The normal points to
    // the kept side and need not be normalized.
    vector plane_pt = CameraToWorld(view_pos_, orient_, vector{1.0f, 0.0f, 0.0f});
    vector plane_normal = orient_.rvec + orient_.uvec;
    g3_SetCustomClipPlane(1, &plane_pt, &plane_normal);
  }
  void TearDown() override { g3_SetCustomClipPlane(0, nullptr, nullptr); }

  const vector view_pos_{5.0f, 2.0f, -3.0f};
  const matrix orient_ = TiltedOrientation();
};

TEST_F(CustomClipPlaneTest, PointsAreCodedByTheirSideOfThePlane) {
  struct Case {
    float x, y;
    uint8_t expected;
  } cases[] = {
      {3.0f, -1.8f, 0},              // x + y = 1.2
      {-1.0f, 1.8f, CC_OFF_CUSTOM},  // x + y = 0.8
      {-1.8f, 3.0f, 0},              // x + y = 1.2
      {1.8f, -1.0f, CC_OFF_CUSTOM},  // x + y = 0.8
      {0.7f, 0.5f, 0},               // x + y = 1.2
      {0.4f, 0.4f, CC_OFF_CUSTOM},   // x + y = 0.8
  };
  for (const Case &c : cases) {
    SCOPED_TRACE(testing::Message() << "camera-space x=" << c.x << " y=" << c.y);
    g3Point p = RotatedUVPoint(CameraToWorld(view_pos_, orient_, vector{c.x, c.y, 10.0f}));
    EXPECT_EQ(p.p3_codes, c.expected);
  }
}

TEST_F(CustomClipPlaneTest, PolygonIsClippedAtCustomPlane) {
  g3Point a = RotatedUVPoint(CameraToWorld(view_pos_, orient_, vector{3.0f, 0.0f, 10.0f}), 0.0f, 0.0f);
  g3Point b = RotatedUVPoint(CameraToWorld(view_pos_, orient_, vector{0.0f, 0.0f, 10.0f}), 1.0f, 0.0f);
  g3Point c = RotatedUVPoint(CameraToWorld(view_pos_, orient_, vector{2.0f, 1.0f, 10.0f}), 0.0f, 1.0f);
  ASSERT_EQ(a.p3_codes, 0);
  ASSERT_EQ(b.p3_codes, CC_OFF_CUSTOM);
  ASSERT_EQ(c.p3_codes, 0);

  g3Point *in[] = {&a, &b, &c};
  int nv = 3;
  g3Codes cc = CodesOf(in, nv);
  g3Point **out = g3_ClipPolygon(in, &nv, &cc);

  ASSERT_EQ(nv, 4);
  int ia = IndexOf(out, nv, &a);
  ASSERT_NE(ia, -1);
  const g3Point *ab = out[(ia + 1) % 4];
  const g3Point *bc = out[(ia + 2) % 4];
  EXPECT_EQ(out[(ia + 3) % 4], &c);

  // a-b, (3,0) -> (0,0), meets x + y = 1 at t = 2/3: camera (1, 0, 10).
  // c-b, (2,1) -> (0,0), meets it at t = 2/3: camera (2/3, 1/3, 10).
  g3Point expect_ab = RotatedUVPoint(CameraToWorld(view_pos_, orient_, vector{1.0f, 0.0f, 10.0f}));
  g3Point expect_bc = RotatedUVPoint(CameraToWorld(view_pos_, orient_, vector{2.0f / 3.0f, 1.0f / 3.0f, 10.0f}));
  ExpectPos(ab, expect_ab.p3_x, expect_ab.p3_y, expect_ab.p3_z);
  ExpectUV(ab, 2.0f / 3.0f, 0.0f);
  ExpectPos(bc, expect_bc.p3_x, expect_bc.p3_y, expect_bc.p3_z);
  ExpectUV(bc, 2.0f / 3.0f, 1.0f / 3.0f);
  EXPECT_FALSE(ab->p3_codes & CC_OFF_CUSTOM);
  EXPECT_FALSE(bc->p3_codes & CC_OFF_CUSTOM);

  g3_FreeTempPoints(out, nv);
}

} // namespace
