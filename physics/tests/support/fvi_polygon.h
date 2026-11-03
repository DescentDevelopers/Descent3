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

#include <utility>
#include <vector>

#include "vecmat.h"

namespace physics_test {

// The face normal is computed the way the game computes room face normals (vm_GetNormal
// over consecutive vertices), so the tests follow the engine's winding convention.
struct Polygon {
  std::vector<vector> verts;
  std::vector<vector *> ptrs;
  vector normal;

  explicit Polygon(std::vector<vector> v) : verts(std::move(v)) {
    for (auto &p : verts)
      ptrs.push_back(&p);
    vm_GetNormal(&normal, &verts[0], &verts[1], &verts[2]);
  }
  Polygon(const Polygon &) = delete; // ptrs point into verts
  Polygon &operator=(const Polygon &) = delete;
  int nv() const { return static_cast<int>(verts.size()); }
  vector **list() { return ptrs.data(); }
};

} // namespace physics_test
