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
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "erooms.h"
#include "fix.h"
#include "gametexture.h"
#include "lightmap_info.h"
#include "mem.h"
#include "room.h"
#include "RoomUVs.h"
#include "support/erooms_internals.h"
#include "support/room_test_expect.h"
#include "support/room_test_geometry.h"
#include "support/room_test_rooms.h"
#include "d3_test/files.h"
#include "vecmat.h"

namespace {

using namespace room_test;

class EroomsTest : public ::testing::Test {
protected:
  // The editor initialises the sine/cosine tables used by the angle-based matrix code at startup.
  static void SetUpTestSuite() { InitMathTables(); }

  void TearDown() override {
    for (int i = 0; i < MAX_ROOMS + MAX_PALETTE_ROOMS; i++)
      if (Rooms[i].used)
        FreeRoom(&Rooms[i]);
    Highest_room_index = -1;
  }
};

TEST_F(EroomsTest, CheckFaceConcavityAcceptsConvexFaces) {
  std::vector<vector> square = {V(0, 0, 0), V(0, 10, 0), V(10, 10, 0), V(10, 0, 0)};
  std::vector<int16_t> quad = {0, 1, 2, 3};
  vector n = NormalOf(square);
  EXPECT_EQ(CheckFaceConcavity(4, quad.data(), &n, square.data()), -1);

  std::vector<int16_t> reversed = {3, 2, 1, 0};
  vector rn = -n;
  EXPECT_EQ(CheckFaceConcavity(4, reversed.data(), &rn, square.data()), -1);

  std::vector<vector> hex;
  const vector a = V(1, 1, 0) / sqrtf(2.0f), b = V(0, 0, 1);
  for (int i = 0; i < 6; i++) {
    float t = (float)i * 3.14159265f / 3.0f;
    hex.push_back(a * (5 * cosf(t)) + b * (5 * sinf(t)) + V(3, -2, 7));
  }
  std::vector<int16_t> hexface = {0, 1, 2, 3, 4, 5};
  vector hn = NormalOf(hex);
  EXPECT_EQ(CheckFaceConcavity(6, hexface.data(), &hn, hex.data()), -1);
}

TEST_F(EroomsTest, CheckFaceConcavityReturnsFacePositionOfReflexCorner) {
  // The notch at (5,4) is the only reflex corner.
  std::vector<vector> pts = {V(0, 0, 0), V(10, 0, 0), V(10, 10, 0), V(5, 4, 0), V(0, 10, 0)};
  vector n = NormalOf({pts[0], pts[1], pts[2]}); // normal of a convex part of the face

  std::vector<int16_t> face = {0, 1, 2, 3, 4};
  EXPECT_EQ(CheckFaceConcavity(5, face.data(), &n, pts.data()), 3);

  // The result is an index into the face's vertex list (callers do face_verts[result]).
  std::vector<int16_t> rotated = {2, 3, 4, 0, 1};
  EXPECT_EQ(CheckFaceConcavity(5, rotated.data(), &n, pts.data()), 1);
}

TEST_F(EroomsTest, FaceIsPlanarToleratesSmallDeviationOnly) {
  const vector a = V(1, -1, 0) / sqrtf(2.0f), b = V(0, 0, 1);
  std::vector<vector> pts = {a * 0 + b * 0, a * 0 + b * 10, a * 10 + b * 10, a * 10 + b * 0};
  std::vector<int16_t> quad = {0, 1, 2, 3};
  vector n = NormalOf({pts[0], pts[1], pts[2]});

  EXPECT_TRUE(FaceIsPlanar(4, quad.data(), &n, pts.data()));

  std::vector<vector> slightly_off = pts;
  slightly_off[3] += n * 0.01f;
  EXPECT_TRUE(FaceIsPlanar(4, quad.data(), &n, slightly_off.data()));

  std::vector<vector> bent = pts;
  bent[3] += n * 1.0f;
  EXPECT_FALSE(FaceIsPlanar(4, quad.data(), &n, bent.data()));

  // Three points always define a plane, whatever normal is passed.
  vector wrong_normal = V(1, 0, 0);
  std::vector<int16_t> tri = {0, 1, 3};
  EXPECT_TRUE(FaceIsPlanar(3, tri.data(), &wrong_normal, bent.data()));
}

TEST_F(EroomsTest, CheckPointToPlaneClassifiesSides) {
  vector plane_point = V(3, 5, -2);
  vector normal = V(0, 0.6f, 0.8f);

  vector in_front = plane_point + normal * 2.0f;
  vector behind = plane_point - normal * 2.0f;
  vector on_plane = plane_point + V(4, 0.8f, -0.6f) * 3.0f; // direction perpendicular to normal
  vector nearly_on = on_plane + normal * 0.01f;

  EXPECT_EQ(CheckPointToPlane(&in_front, &plane_point, &normal), 1);
  EXPECT_EQ(CheckPointToPlane(&behind, &plane_point, &normal), -1);
  EXPECT_EQ(CheckPointToPlane(&on_plane, &plane_point, &normal), 0);
  EXPECT_EQ(CheckPointToPlane(&nearly_on, &plane_point, &normal), 0);
}

TEST_F(EroomsTest, CheckFaceToPlaneReportsFirstVertexBehind) {
  room *rp = MakeRoom(0, {V(0, 0, 5), V(0, 10, -1), V(10, 10, 2), V(10, 0, -3)}, {{0, 1, 2, 3}});
  vector origin = V(0, 0, 0);
  vector up_z = V(0, 0, 1);
  vector down_z = V(0, 0, -1);

  EXPECT_EQ(CheckFaceToPlane(rp, 0, &origin, &up_z), 1); // vertex 1 (z=-1) is the first behind z=0

  vector below_all = V(0, 0, -10);
  EXPECT_EQ(CheckFaceToPlane(rp, 0, &below_all, &up_z), -1);

  EXPECT_EQ(CheckFaceToPlane(rp, 0, &origin, &down_z), 0); // vertex 0 (z=5) is behind the flipped plane
}

TEST_F(EroomsTest, CheckPointAgainstEdgeIsRelativeToPolygonInterior) {
  std::vector<vector> sq = {V(0, 0, 0), V(0, 10, 0), V(10, 10, 0), V(10, 0, 0)};

  for (bool reverse : {false, true}) {
    SCOPED_TRACE(reverse ? "reversed winding" : "original winding");
    std::vector<vector> poly = sq;
    if (reverse)
      std::reverse(poly.begin(), poly.end());
    vector n = NormalOf(poly);
    vector center = V(5, 5, 0);

    for (size_t e = 0; e < poly.size(); e++) {
      vector *v0 = &poly[e], *v1 = &poly[(e + 1) % poly.size()];
      vector mid = (*v0 + *v1) / 2.0f;
      vector outside = mid + (mid - center);

      EXPECT_EQ(CheckPointAgainstEdge(&center, v0, v1, &n), 1) << "edge " << e;
      EXPECT_EQ(CheckPointAgainstEdge(&mid, v0, v1, &n), 0) << "edge " << e;
      EXPECT_EQ(CheckPointAgainstEdge(&outside, v0, v1, &n), -1) << "edge " << e;
    }
  }
}

TEST_F(EroomsTest, ClipEdgeInterpolatesPositionAndUVL) {
  vector normal = V(0, 0, 1);

  vertex v0{}, v1{}, result{};
  v0.vec = V(0, 0, 0);
  v0.uvl.u = 0;
  v0.uvl.v = 1;
  v0.uvl.alpha = 0;
  v1.vec = V(8, 8, 0);
  v1.uvl.u = 2;
  v1.uvl.v = -1;
  v1.uvl.alpha = 200;

  // The vertical line x=2 crosses the diagonal edge a quarter of the way along.
  vector c0 = V(2, -10, 0), c1 = V(2, 10, 0);
  ClipEdge(&normal, &v0, &v1, &c0, &c1, &result);

  EXPECT_NEAR(result.vec.x(), 2, kTol);
  EXPECT_NEAR(result.vec.y(), 2, kTol);
  EXPECT_NEAR(result.vec.z(), 0, kTol);
  EXPECT_NEAR(result.uvl.u, 0.5f, kTol);
  EXPECT_NEAR(result.uvl.v, 0.5f, kTol);
  EXPECT_NEAR(result.uvl.alpha, 50, 1);
}

TEST_F(EroomsTest, FindSharedEdgeFindsOppositelyWoundCommonEdge) {
  room *rp = MakeRoom(0, {V(0, 0, 0), V(10, 0, 0), V(10, 10, 0), V(0, 10, 0), V(20, 0, 0), V(20, 10, 0)},
                      {{0, 1, 2, 3}, {1, 4, 5, 2}, {3, 2, 1, 0}});
  int vn0 = -1, vn1 = -1;

  ASSERT_TRUE(FindSharedEdge(&rp->faces[0], &rp->faces[1], &vn0, &vn1));
  // Documented contract: the edge is <vn0,vn0+1> on face 0 and <vn1+1,vn1> on face 1.
  const face &f0 = rp->faces[0], &f1 = rp->faces[1];
  EXPECT_EQ(f0.face_verts[vn0], f1.face_verts[(vn1 + 1) % f1.num_verts]);
  EXPECT_EQ(f0.face_verts[(vn0 + 1) % f0.num_verts], f1.face_verts[vn1]);
  EXPECT_EQ((std::set<int>{f0.face_verts[vn0], f0.face_verts[(vn0 + 1) % f0.num_verts]}), (std::set<int>{1, 2}));

  // Face 2 is face 1's neighbour mirrored: it shares edge 1-2 but in the same direction,
  // so the faces do not form a consistently oriented surface there.
  EXPECT_FALSE(FindSharedEdge(&rp->faces[1], &rp->faces[2], &vn0, &vn1));
}

TEST_F(EroomsTest, FindSharedEdgeAcrossRoomsMatchesByPosition) {
  // Room 0's face and room 1's face meet along x=10, with independent vertex numbering and a
  // tiny positional mismatch that is within tolerance.
  MakeRoom(0, {V(0, 0, 0), V(10, 0, 0), V(10, 10, 0), V(0, 10, 0)}, {{0, 1, 2, 3}});
  MakeRoom(1, {V(10.02f, 10, 0), V(10, 0.03f, 0), V(20, 0, 0), V(20, 10, 0)}, {{1, 2, 3, 0}});
  MakeRoom(2, {V(11, 10, 0), V(11, 0, 0), V(21, 0, 0), V(21, 10, 0)}, {{1, 2, 3, 0}});

  int vn0 = -1, vn1 = -1;
  ASSERT_TRUE(FindSharedEdgeAcrossRooms(&Rooms[0], 0, &Rooms[1], 0, &vn0, &vn1));
  vector a0 = Rooms[0].verts[Rooms[0].faces[0].face_verts[vn0]];
  vector b0 = Rooms[0].verts[Rooms[0].faces[0].face_verts[(vn0 + 1) % 4]];
  vector a1 = Rooms[1].verts[Rooms[1].faces[0].face_verts[vn1]];
  vector b1 = Rooms[1].verts[Rooms[1].faces[0].face_verts[(vn1 + 1) % 4]];
  EXPECT_TRUE(PointsAreSame(&a0, &b1));
  EXPECT_TRUE(PointsAreSame(&b0, &a1));
  EXPECT_NEAR(a0.x(), 10, kTol);
  EXPECT_NEAR(b0.x(), 10, kTol);

  EXPECT_FALSE(FindSharedEdgeAcrossRooms(&Rooms[0], 0, &Rooms[2], 0, &vn0, &vn1));
}

TEST_F(EroomsTest, CheckForDuplicateFaceIgnoresStartingVertexButNotFacing) {
  room *rp = MakeRoom(0, {V(0, 0, 0), V(10, 0, 0), V(10, 10, 0), V(0, 10, 0), V(0, 0, 10)},
                      {{0, 1, 2, 3},   // 0
                       {2, 3, 0, 1},   // 1: same face as 0, listed from a different vertex
                       {3, 2, 1, 0},   // 2: same corners as 0 but facing the other way
                       {0, 1, 4}});    // 3: unrelated
  EXPECT_EQ(CheckForDuplicateFace(rp, 0), -1);
  EXPECT_EQ(CheckForDuplicateFace(rp, 1), 0);
  EXPECT_EQ(CheckForDuplicateFace(rp, 2), -1);
  EXPECT_EQ(CheckForDuplicateFace(rp, 3), -1);
}

TEST_F(EroomsTest, DeleteUnusedRoomVertsKeepsFaceGeometry) {
  room *rp = MakeRoom(0, {V(99, 99, 99), V(0, 0, 0), V(-5, -5, -5), V(10, 0, 0), V(10, 10, 0), V(0, 10, 0)},
                      {{1, 5, 4, 3}});
  auto before = FacePositions(rp, 0);

  EXPECT_EQ(DeleteUnusedRoomVerts(rp), 2);
  EXPECT_EQ(rp->num_verts, 4);
  ExpectSamePositions(FacePositions(rp, 0), before);
  for (int v = 0; v < 4; v++)
    EXPECT_LT(rp->faces[0].face_verts[v], rp->num_verts);

  EXPECT_EQ(DeleteUnusedRoomVerts(rp), 0);
}

TEST_F(EroomsTest, RemoveDuplicatePointsWeldsCoincidentVertices) {
  // Vertex 3 coincides with vertex 0 and vertex 4 is within tolerance of vertex 2.
  room *rp = MakeRoom(0, {V(0, 0, 0), V(10, 0, 0), V(10, 10, 0), V(0, 0, 0), V(10.05f, 10, 0), V(0, 10, 0)},
                      {{0, 1, 2}, {3, 4, 5}});
  auto before0 = FacePositions(rp, 0);
  auto before1 = FacePositions(rp, 1);
  int vn0, vn1;
  ASSERT_FALSE(FindSharedEdge(&rp->faces[0], &rp->faces[1], &vn0, &vn1));

  EXPECT_EQ(RemoveDuplicatePoints(rp), 2);
  EXPECT_EQ(rp->num_verts, 4);

  ExpectSamePositions(FacePositions(rp, 0), before0);
  auto after1 = FacePositions(rp, 1);
  ASSERT_EQ(after1.size(), 3u);
  EXPECT_TRUE(PointsAreSame(&after1[1], &before1[1]));
  EXPECT_TRUE(FindSharedEdge(&rp->faces[0], &rp->faces[1], &vn0, &vn1));
}

TEST_F(EroomsTest, RemoveDuplicateFacePointsDropsRepeatedCornersAndKeepsUVs) {
  room *rp = MakeRoom(0, {V(0, 0, 0), V(10, 0, 0), V(10, 10, 0), V(0, 10, 0)},
                      {{0, 1, 1, 2, 3}, {0, 1, 2, 3, 0}, {0, 1, 2, 3}});
  for (int f = 0; f < rp->num_faces; f++)
    for (int v = 0; v < rp->faces[f].num_verts; v++)
      rp->faces[f].face_uvls[v].u = (float)rp->faces[f].face_verts[v];

  EXPECT_EQ(RemoveDuplicateFacePoints(rp), 2);

  for (int f = 0; f < rp->num_faces; f++) {
    SCOPED_TRACE(f);
    EXPECT_EQ(FaceVerts(rp->faces[f]), (std::vector<int16_t>{0, 1, 2, 3}));
    for (int v = 0; v < rp->faces[f].num_verts; v++)
      EXPECT_EQ(rp->faces[f].face_uvls[v].u, (float)rp->faces[f].face_verts[v]);
  }
}

TEST_F(EroomsTest, AddVertToFaceInsertsAfterGivenCornerAndInterpolatesUV) {
  room *rp = MakeRoom(0, {V(0, 0, 0), V(0, 20, 0), V(20, 20, 0), V(20, 0, 0)}, {{0, 1, 2, 3}});
  const float us[] = {0, 0, 1, 1}, vs[] = {1, 0, 0, 1};
  for (int v = 0; v < 4; v++) {
    rp->faces[0].face_uvls[v].u = us[v];
    rp->faces[0].face_uvls[v].v = vs[v];
  }

  // New vertex a quarter of the way along the edge from corner 1 to corner 2.
  int nv = RoomAddVertices(rp, 1);
  rp->verts[nv] = V(5, 20, 0);
  AddVertToFace(rp, 0, nv, 1);

  EXPECT_EQ(FaceVerts(rp->faces[0]), (std::vector<int16_t>{0, 1, (int16_t)nv, 2, 3}));
  const roomUVL *uv = rp->faces[0].face_uvls;
  EXPECT_NEAR(uv[2].u, 0.25f, kTol);
  EXPECT_NEAR(uv[2].v, 0.0f, kTol);
  const int old_pos[] = {0, 1, 3, 4};
  for (int c = 0; c < 4; c++) {
    EXPECT_EQ(uv[old_pos[c]].u, us[c]) << "corner " << c;
    EXPECT_EQ(uv[old_pos[c]].v, vs[c]) << "corner " << c;
  }
}

TEST_F(EroomsTest, RoomAddVerticesAppendsWithoutRenumbering) {
  room *rp = MakeRoom(0, {V(1, 2, 3), V(4, 5, 6), V(7, 8, -9)}, {{0, 1, 2}});

  EXPECT_EQ(RoomAddVertices(rp, 2), 3);
  EXPECT_EQ(rp->num_verts, 5);
  ExpectSamePositions({rp->verts[0], rp->verts[1], rp->verts[2]}, {V(1, 2, 3), V(4, 5, 6), V(7, 8, -9)});
}

TEST_F(EroomsTest, DeleteRoomFaceRenumbersPortalsAndDropsOrphanedVerts) {
  // Face 0 uses vertex 4, which nothing else uses.
  room *rp = MakeRoom(0, {V(0, 0, 0), V(10, 0, 0), V(10, 10, 0), V(0, 10, 0), V(5, -5, 0)},
                      {{0, 4, 1}, {0, 1, 2, 3}, {3, 2, 1, 0}}, 1);
  rp->portals[0].portal_face = 2;
  rp->faces[2].portal_num = 0;
  auto kept_face = FacePositions(rp, 1);

  DeleteRoomFace(rp, 0);

  EXPECT_EQ(rp->num_faces, 2);
  EXPECT_EQ(rp->portals[0].portal_face, 1);
  EXPECT_EQ(rp->faces[1].portal_num, 0);
  EXPECT_EQ(rp->num_verts, 4);
  ExpectSamePositions(FacePositions(rp, 0), kept_face);
}

TEST_F(EroomsTest, DeleteRoomFaceCanKeepUnusedVerts) {
  room *rp = MakeRoom(0, {V(0, 0, 0), V(10, 0, 0), V(10, 10, 0), V(5, -5, 0)}, {{0, 3, 1}, {0, 1, 2}});
  DeleteRoomFace(rp, 0, false);
  EXPECT_EQ(rp->num_faces, 1);
  EXPECT_EQ(rp->num_verts, 4);
}

TEST_F(EroomsTest, LinkRoomsCreatesReciprocalPortals) {
  MakeRoom(0, kCubeVerts, kCubeFaces);
  MakeRoom(1, kCubeVerts, kCubeFaces);

  LinkRooms(Rooms, 0, 5, 1, 4);

  ASSERT_EQ(Rooms[0].num_portals, 1);
  ASSERT_EQ(Rooms[1].num_portals, 1);
  const portal &p0 = Rooms[0].portals[0], &p1 = Rooms[1].portals[0];
  EXPECT_EQ(p0.croom, 1);
  EXPECT_EQ(p0.cportal, 0);
  EXPECT_EQ(p0.portal_face, 5);
  EXPECT_EQ(p1.croom, 0);
  EXPECT_EQ(p1.cportal, 0);
  EXPECT_EQ(p1.portal_face, 4);
  EXPECT_EQ(Rooms[0].faces[5].portal_num, 0);
  EXPECT_EQ(Rooms[1].faces[4].portal_num, 0);
}

TEST_F(EroomsTest, DeleteRoomPortalRenumbersLaterPortalsOnBothSides) {
  MakeRoom(0, kCubeVerts, kCubeFaces);
  MakeRoom(1, kCubeVerts, kCubeFaces);
  MakeRoom(2, kCubeVerts, kCubeFaces);
  LinkRooms(Rooms, 0, 5, 1, 4);
  LinkRooms(Rooms, 0, 4, 2, 5);
  ASSERT_EQ(Rooms[0].portals[1].croom, 2);

  DeleteRoomPortal(&Rooms[0], 0);

  ASSERT_EQ(Rooms[0].num_portals, 1);
  EXPECT_EQ(Rooms[0].faces[5].portal_num, -1);
  EXPECT_EQ(Rooms[0].portals[0].croom, 2);
  EXPECT_EQ(Rooms[0].portals[0].portal_face, 4);
  EXPECT_EQ(Rooms[0].faces[4].portal_num, 0);
  EXPECT_EQ(Rooms[2].portals[0].cportal, 0);
}

TEST_F(EroomsTest, BuildListOfPortalVertsListsEachVertexOnce) {
  room *rp = MakeRoom(0, {V(0, 0, 0), V(10, 0, 0), V(10, 10, 0), V(0, 10, 0), V(5, 5, 9), V(20, 0, 0), V(20, 10, 0)},
                      {{0, 1, 2, 3}, {1, 5, 6, 2}, {0, 4, 1}}, 2);
  rp->portals[0].portal_face = 0;
  rp->portals[1].portal_face = 1;

  int list[MAX_VERTS_PER_ROOM];
  int n = BuildListOfPortalVerts(rp, list);

  EXPECT_EQ(std::set<int>(list, list + n), (std::set<int>{0, 1, 2, 3, 5, 6}));
  EXPECT_EQ(n, 6);
}

TEST_F(EroomsTest, CopyFaceDropsPortalAndPerFaceLightingState) {
  room *rp = MakeRoom(0, {V(0, 0, 0), V(10, 0, 0), V(10, 10, 0)}, {{0, 1, 2}});
  face &src = rp->faces[0];
  src.flags = FF_LIGHTMAP | FF_HAS_TRIGGER | FF_GOALFACE | FF_VERTEX_ALPHA;
  src.portal_num = 3;
  src.tmap = 17;
  src.light_multiple = 9;
  src.face_uvls[1].u = 0.75f;

  face dst{};
  CopyFace(&dst, &src);

  EXPECT_EQ(dst.portal_num, -1);
  EXPECT_EQ(dst.flags, FF_GOALFACE | FF_VERTEX_ALPHA);
  EXPECT_EQ(dst.tmap, 17);
  EXPECT_EQ(dst.light_multiple, 9);
  EXPECT_EQ(FaceVerts(dst), FaceVerts(src));
  EXPECT_EQ(dst.face_uvls[1].u, 0.75f);
  EXPECT_NE(dst.face_verts, src.face_verts);
  FreeRoomFace(&dst);
}

TEST_F(EroomsTest, CopyRoomCopiesGeometryButNotPortals) {
  room *src = MakeRoom(0, kCubeVerts, kCubeFaces);
  MakeRoom(1, kCubeVerts, kCubeFaces);
  LinkRooms(Rooms, 0, 5, 1, 4);
  src->faces[2].tmap = 42;
  src->flags = RF_FUELCEN;

  room *dst = &Rooms[2];
  CopyRoom(dst, src);
  Highest_room_index = 2;

  EXPECT_EQ(dst->num_portals, 0);
  EXPECT_EQ(dst->faces[5].portal_num, -1);
  EXPECT_EQ(dst->flags, RF_FUELCEN);
  EXPECT_EQ(dst->faces[2].tmap, 42);
  ASSERT_EQ(dst->num_faces, src->num_faces);
  ASSERT_EQ(dst->num_verts, src->num_verts);
  for (int f = 0; f < dst->num_faces; f++)
    EXPECT_EQ(FaceVerts(dst->faces[f]), FaceVerts(src->faces[f]));
  ExpectSamePositions(std::vector<vector>(dst->verts, dst->verts + dst->num_verts),
                      std::vector<vector>(src->verts, src->verts + src->num_verts));
}

TEST_F(EroomsTest, ComputeRoomShellMarksFacesUnreachableFromPortals) {
  std::vector<vector> verts = kCubeVerts;
  for (vector p : {V(2, 2, 5), V(2, 8, 5), V(8, 8, 5), V(8, 2, 5)})
    verts.push_back(p);
  FaceList faces = kCubeFaces;
  faces.push_back({8, 9, 10, 11});

  room *rp = MakeRoom(0, verts, faces, 1);
  rp->portals[0].portal_face = 0;
  rp->faces[0].portal_num = 0;

  EXPECT_EQ(ComputeRoomShell(rp), 0);
  for (int f = 0; f < 6; f++)
    EXPECT_FALSE(rp->faces[f].flags & FF_NOT_SHELL) << "cube face " << f;
  EXPECT_TRUE(rp->faces[6].flags & FF_NOT_SHELL);
}

TEST_F(EroomsTest, ComputeRoomShellCountsOpenEdgesAndClearsFlagsOnError) {
  // The cube without its top face (index 1): each of the four side faces has one open edge.
  FaceList faces = kCubeFaces;
  faces.erase(faces.begin() + 1);
  room *rp = MakeRoom(0, kCubeVerts, faces, 1);
  rp->portals[0].portal_face = 0;
  rp->faces[0].portal_num = 0;
  for (int f = 0; f < rp->num_faces; f++)
    rp->faces[f].flags |= FF_NOT_SHELL;

  EXPECT_EQ(ComputeRoomShell(rp), 4);
  for (int f = 0; f < rp->num_faces; f++)
    EXPECT_FALSE(rp->faces[f].flags & FF_NOT_SHELL) << "face " << f;
}

TEST_F(EroomsTest, DefaultUVsTileOncePerTwentyUnits) {
  room *rp = MakeRoom(0, {V(0, 0, 0), V(0, 20, 0), V(40, 20, 0), V(40, 0, 0)}, {{0, 1, 2, 3}});
  AssignDefaultUVsToRoomFace(rp, 0);

  float umin = 1e9f, umax = -1e9f, vmin = 1e9f, vmax = -1e9f;
  for (int v = 0; v < 4; v++) {
    const roomUVL &uv = rp->faces[0].face_uvls[v];
    umin = std::min(umin, uv.u);
    umax = std::max(umax, uv.u);
    vmin = std::min(vmin, uv.v);
    vmax = std::max(vmax, uv.v);
    EXPECT_EQ(uv.alpha, 255);
  }
  EXPECT_NEAR(umin, 0, kTol);
  EXPECT_NEAR(umax, 2, kTol);
  EXPECT_NEAR(vmin, 0, kTol);
  EXPECT_NEAR(vmax, 1, kTol);

  const roomUVL *uv = rp->faces[0].face_uvls;
  EXPECT_NEAR(std::abs(uv[3].u - uv[0].u), 2, kTol);
  EXPECT_NEAR(uv[3].v, uv[0].v, kTol);
  EXPECT_NEAR(std::abs(uv[1].v - uv[0].v), 1, kTol);
  EXPECT_NEAR(uv[1].u, uv[0].u, kTol);
}

TEST_F(EroomsTest, FixConcaveFacesTriangulatesNonPlanarFace) {
  room *rp = MakeRoom(0, kBentQuad, {{0, 1, 2, 3}});
  rp->faces[0].tmap = 7;
  int list[] = {0};

  FixConcaveFaces(rp, list, 1);

  ASSERT_EQ(rp->num_faces, 2);
  ExpectTriangulates(rp, {0, 1}, {0, 1, 2, 3});
  EXPECT_EQ(rp->faces[0].tmap, 7);
  EXPECT_EQ(rp->faces[1].tmap, 7);
}

TEST_F(EroomsTest, FixConcaveFacesLeavesPlanarFacesAlone) {
  room *rp = MakeRoom(0, {V(0, 0, 0), V(0, 10, 0), V(10, 10, 0), V(10, 0, 0)}, {{0, 1, 2, 3}});
  int list[] = {0};

  FixConcaveFaces(rp, list, 1);

  ASSERT_EQ(rp->num_faces, 1);
  EXPECT_EQ(FaceVerts(rp->faces[0]), (std::vector<int16_t>{0, 1, 2, 3}));
}

using RoomUVsTest = EroomsTest;

TEST_F(RoomUVsTest, AssignUVsToFaceExtendsEdgeMappingWithoutDistortion) {
  // The last edge wraps around from the last vertex to the first.
  for (auto [va, vb] : {std::pair{1, 2}, std::pair{2, 1}, std::pair{4, 0}}) {
    SCOPED_TRACE(testing::Message() << "edge " << va << "," << vb);
    room *rp = MakeRoom(0, TiltedPentagon(), {{0, 1, 2, 3, 4}});

    roomUVL uva{}, uvb{};
    uva.u = 0.3f;
    uva.v = 0.1f;
    uvb.u = 0.3f + 0.9f;
    uvb.v = 0.1f + 1.2f;
    AssignUVsToFace(rp, 0, &uva, &uvb, va, vb);

    const roomUVL *uv = rp->faces[0].face_uvls;
    EXPECT_FLOAT_EQ(uv[va].u, uva.u);
    EXPECT_FLOAT_EQ(uv[va].v, uva.v);
    EXPECT_FLOAT_EQ(uv[vb].u, uvb.u);
    EXPECT_FLOAT_EQ(uv[vb].v, uvb.v);
    ExpectUVsUndistorted(rp, 0);

    FreeRoom(rp);
  }
}

TEST_F(RoomUVsTest, StretchRoomUVsRealignsFromEdgeAndKeepsLightmapUVs) {
  room *rp = MakeRoom(0, {V(0, 0, 0), V(0, 30, 0), V(20, 30, 0), V(20, 0, 0)}, {{0, 1, 2, 3}});
  const float u[] = {0.1f, 0.4f, 2.5f, 0.9f}, v[] = {0.2f, -1.0f, 0.3f, 0.7f};
  for (int i = 0; i < 4; i++) {
    rp->faces[0].face_uvls[i].u = u[i];
    rp->faces[0].face_uvls[i].v = v[i];
    rp->faces[0].face_uvls[i].u2 = 0.1f * i;
    rp->faces[0].face_uvls[i].v2 = 0.2f * i;
  }

  StretchRoomUVs(rp, 0, 3); // the edge from corner 3 to corner 0

  const roomUVL *uv = rp->faces[0].face_uvls;
  EXPECT_FLOAT_EQ(uv[3].u, u[3]);
  EXPECT_FLOAT_EQ(uv[3].v, v[3]);
  EXPECT_FLOAT_EQ(uv[0].u, u[0]);
  EXPECT_FLOAT_EQ(uv[0].v, v[0]);
  ExpectUVsUndistorted(rp, 0);
  for (int i = 0; i < 4; i++) {
    EXPECT_FLOAT_EQ(uv[i].u2, 0.1f * i) << "corner " << i;
    EXPECT_FLOAT_EQ(uv[i].v2, 0.2f * i) << "corner " << i;
  }
}

class RoomFileTest : public EroomsTest {
protected:
  void SetUp() override {
    SetTexture(5, "Wall");
    SetTexture(9, "Floor");
  }
  void TearDown() override {
    for (int t : {5, 9})
      GameTextures[t].used = 0;
    EroomsTest::TearDown();
  }

  d3_test::TempDir dir_;
};

TEST_F(RoomFileTest, SaveThenLoadRoundTripsGeometryTexturesAndUVs) {
  room *src = MakeDecoratedCube();
  std::string path = (dir_ / "cube.orf").string();
  SaveRoom(0, path.data());

  int n = AllocLoadRoom(path.data(), false, true);
  ASSERT_GE(n, FIRST_PALETTE_ROOM);
  ASSERT_LT(n, FIRST_PALETTE_ROOM + MAX_PALETTE_ROOMS);
  room *rp = &Rooms[n];

  ASSERT_EQ(rp->num_verts, src->num_verts);
  ASSERT_EQ(rp->num_faces, src->num_faces);
  ExpectSamePositions(std::vector<vector>(rp->verts, rp->verts + rp->num_verts),
                      std::vector<vector>(src->verts, src->verts + src->num_verts));
  for (int f = 0; f < rp->num_faces; f++) {
    SCOPED_TRACE(f);
    const face &a = rp->faces[f], &b = src->faces[f];
    EXPECT_EQ(FaceVerts(a), FaceVerts(b));
    EXPECT_EQ(a.tmap, b.tmap);
    EXPECT_EQ(a.light_multiple, b.light_multiple);
    for (int v = 0; v < a.num_verts; v++) {
      EXPECT_EQ(a.face_uvls[v].u, b.face_uvls[v].u);
      EXPECT_EQ(a.face_uvls[v].v, b.face_uvls[v].v);
    }
    EXPECT_NEAR(vm_Dot3Product(a.normal, b.normal), 1.0f, kTol);
  }
}

TEST_F(RoomFileTest, LoadMapsTexturesByNameAndFallsBackToZero) {
  MakeDecoratedCube();
  std::string path = (dir_ / "cube.orf").string();
  SaveRoom(0, path.data());

  GameTextures[5].used = 0;
  GameTextures[9].used = 0;
  SetTexture(12, "Wall");

  int n = AllocLoadRoom(path.data(), false, true);
  GameTextures[12].used = 0;
  ASSERT_NE(n, -1);
  for (int f = 0; f < Rooms[n].num_faces; f++)
    EXPECT_EQ(Rooms[n].faces[f].tmap, (f % 3 == 0) ? 0 : 12) << "face " << f;
}

TEST_F(RoomFileTest, LoadCanCenterRoomOnOrigin) {
  room *src = MakeDecoratedCube();
  std::string path = (dir_ / "cube.orf").string();
  SaveRoom(0, path.data());

  int n = AllocLoadRoom(path.data(), true, true);
  ASSERT_NE(n, -1);
  room *rp = &Rooms[n];

  vector sum{};
  for (int v = 0; v < rp->num_verts; v++)
    sum += rp->verts[v];
  EXPECT_NEAR(sum.x(), 0, kTol);
  EXPECT_NEAR(sum.y(), 0, kTol);
  EXPECT_NEAR(sum.z(), 0, kTol);

  // Every vertex is shifted by the same amount (the cube's centre).
  for (int v = 0; v < rp->num_verts; v++) {
    vector d = src->verts[v] - rp->verts[v];
    EXPECT_NEAR(d.x(), 5, kTol);
    EXPECT_NEAR(d.y(), 5, kTol);
    EXPECT_NEAR(d.z(), 5, kTol);
  }
}

} // namespace
