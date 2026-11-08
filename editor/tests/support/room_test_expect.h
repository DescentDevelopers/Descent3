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
#include "vecmat.h"

namespace room_test {

// Positional tolerance used by the room geometry tests.
constexpr float kTol = 1e-4f;

// Expects a and b to hold the same points, in order, within kTol.
void ExpectSamePositions(const std::vector<vector> &a, const std::vector<vector> &b);

// Expects tri_faces to be triangles that together cover the polygon orig, boundary edge for edge.
void ExpectTriangulates(const room *rp, const std::vector<int> &tri_faces, const std::vector<int16_t> &orig);

// Expects the face's UVs to be a uniformly scaled copy of its corner distances.
void ExpectUVsUndistorted(const room *rp, int facenum);

} // namespace room_test
