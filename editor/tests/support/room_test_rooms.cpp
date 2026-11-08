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

#include "room_test_rooms.h"

#include <algorithm>
#include <cstring>

#include <gtest/gtest.h>

#include "erooms.h"
#include "gametexture.h"

namespace room_test {

room *MakeRoom(int roomnum, const std::vector<vector> &verts, const FaceList &faces, int nportals) {
  room *rp = &Rooms[roomnum];
  InitRoom(rp, (int)verts.size(), (int)faces.size(), nportals);
  for (size_t v = 0; v < verts.size(); v++)
    rp->verts[v] = verts[v];
  for (size_t f = 0; f < faces.size(); f++) {
    InitRoomFace(&rp->faces[f], (int)faces[f].size());
    for (size_t v = 0; v < faces[f].size(); v++) {
      rp->faces[f].face_verts[v] = faces[f][v];
      rp->faces[f].face_uvls[v].u = 0;
      rp->faces[f].face_uvls[v].v = 0;
      rp->faces[f].face_uvls[v].u2 = 0;
      rp->faces[f].face_uvls[v].v2 = 0;
    }
    EXPECT_TRUE(ComputeFaceNormal(rp, (int)f)) << "test geometry has a degenerate face " << f;
  }
  Highest_room_index = std::max(Highest_room_index, roomnum);
  return rp;
}

room *MakeDecoratedCube() {
  room *rp = MakeRoom(0, kCubeVerts, kCubeFaces);
  for (int f = 0; f < rp->num_faces; f++) {
    rp->faces[f].tmap = (f % 3 == 0) ? 9 : 5;
    rp->faces[f].light_multiple = (uint8_t)(2 + f);
    for (int v = 0; v < rp->faces[f].num_verts; v++) {
      rp->faces[f].face_uvls[v].u = 0.25f * f + v;
      rp->faces[f].face_uvls[v].v = -0.5f * v;
    }
  }
  return rp;
}

void SetTexture(int index, const char *name) {
  GameTextures[index].used = 1;
  strcpy(GameTextures[index].name, name);
}

std::vector<int16_t> FaceVerts(const face &f) { return {f.face_verts, f.face_verts + f.num_verts}; }

std::vector<vector> FacePositions(const room *rp, int facenum) {
  std::vector<vector> out;
  const face &f = rp->faces[facenum];
  for (int v = 0; v < f.num_verts; v++)
    out.push_back(rp->verts[f.face_verts[v]]);
  return out;
}

vector NormalOf(std::vector<vector> pts) {
  std::vector<int16_t> idx(pts.size());
  for (size_t i = 0; i < idx.size(); i++)
    idx[i] = (int16_t)i;
  vector n{};
  EXPECT_TRUE(ComputeNormal(&n, (int)pts.size(), idx.data(), pts.data()));
  return n;
}

} // namespace room_test
