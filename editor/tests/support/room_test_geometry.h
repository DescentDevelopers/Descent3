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

#include <cmath>
#include <cstdint>
#include <vector>

#include "vecmat.h"

namespace room_test {

inline vector V(float x, float y, float z) {
  vector v{};
  v.x() = x;
  v.y() = y;
  v.z() = z;
  return v;
}

using FaceList = std::vector<std::vector<int16_t>>;

// Faces are wound so that every edge is shared by exactly two faces, traversed in
// opposite directions (a closed, consistently oriented shell).
inline const std::vector<vector> kCubeVerts = {V(0, 0, 0),  V(10, 0, 0),  V(10, 10, 0),  V(0, 10, 0),
                                               V(0, 0, 10), V(10, 0, 10), V(10, 10, 10), V(0, 10, 10)};
inline const FaceList kCubeFaces = {{0, 3, 2, 1}, {4, 5, 6, 7}, {0, 1, 5, 4}, {3, 7, 6, 2}, {0, 4, 7, 3}, {1, 2, 6, 5}};

// A quad with one corner lifted out of the plane of the other three.
inline const std::vector<vector> kBentQuad = {V(0, 0, 0), V(0, 10, 0), V(10, 10, 0), V(10, 0, 3)};

inline std::vector<vector> TiltedPentagon() {
  std::vector<vector> pts;
  const vector a = V(1, 1, 0) / sqrtf(2.0f), b = V(0, 0, 1);
  for (int i = 0; i < 5; i++) {
    float t = (float)i * 2 * 3.14159265f / 5;
    pts.push_back(a * (10 * cosf(t)) + b * (10 * sinf(t)) + V(-4, 6, 1));
  }
  return pts;
}

} // namespace room_test
