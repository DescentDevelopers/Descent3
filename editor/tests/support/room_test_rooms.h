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

#include <cstdint>
#include <vector>

#include "room.h"
#include "room_test_geometry.h"
#include "vecmat.h"

namespace room_test {

// Initialises Rooms[roomnum] with the given vertices and faces (zero UVs, computed normals).
room *MakeRoom(int roomnum, const std::vector<vector> &verts, const FaceList &faces, int nportals = 0);

// Builds Rooms[0] as the cube with per-face textures 5/9, light multiples and UVs set.
room *MakeDecoratedCube();

// Marks GameTextures[index] as used under the given name.
void SetTexture(int index, const char *name);

// The face's vertex indices, in order.
std::vector<int16_t> FaceVerts(const face &f);

// The positions of the face's corners, in order.
std::vector<vector> FacePositions(const room *rp, int facenum);

// The normal ComputeNormal gives for the polygon pts[0], pts[1], ...
vector NormalOf(std::vector<vector> pts);

} // namespace room_test
