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

#include "room_test_expect.h"

#include <cmath>
#include <set>
#include <utility>

#include <gtest/gtest.h>

namespace room_test {

void ExpectSamePositions(const std::vector<vector> &a, const std::vector<vector> &b) {
  ASSERT_EQ(a.size(), b.size());
  for (size_t i = 0; i < a.size(); i++) {
    EXPECT_NEAR(a[i].x(), b[i].x(), kTol) << "corner " << i;
    EXPECT_NEAR(a[i].y(), b[i].y(), kTol) << "corner " << i;
    EXPECT_NEAR(a[i].z(), b[i].z(), kTol) << "corner " << i;
  }
}

void ExpectTriangulates(const room *rp, const std::vector<int> &tri_faces, const std::vector<int16_t> &orig) {
  ASSERT_EQ(tri_faces.size(), orig.size() - 2);
  std::multiset<std::pair<int, int>> edges;
  for (int f : tri_faces) {
    const face &fp = rp->faces[f];
    ASSERT_EQ(fp.num_verts, 3) << "face " << f;
    for (int v = 0; v < 3; v++)
      edges.insert({fp.face_verts[v], fp.face_verts[(v + 1) % 3]});
  }
  for (size_t v = 0; v < orig.size(); v++) {
    std::pair<int, int> e{orig[v], orig[(v + 1) % orig.size()]};
    EXPECT_EQ(edges.count(e), 1u) << "boundary edge " << e.first << "->" << e.second;
  }
}

void ExpectUVsUndistorted(const room *rp, int facenum) {
  const face &f = rp->faces[facenum];
  auto dist3 = [&](int i, int j) {
    return vm_VectorDistance(&rp->verts[f.face_verts[i]], &rp->verts[f.face_verts[j]]);
  };
  auto dist_uv = [&](int i, int j) {
    return std::hypot(f.face_uvls[i].u - f.face_uvls[j].u, f.face_uvls[i].v - f.face_uvls[j].v);
  };
  const float scale = dist3(0, 1) / dist_uv(0, 1);
  for (int i = 0; i < f.num_verts; i++)
    for (int j = i + 1; j < f.num_verts; j++)
      EXPECT_NEAR(dist_uv(i, j) * scale, dist3(i, j), 1e-3f * dist3(i, j)) << "corners " << i << "," << j;
}

} // namespace room_test
