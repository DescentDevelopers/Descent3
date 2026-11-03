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

#include <string>
#include <utility>
#include <vector>

#include "pof_writer.h"

namespace model_test {

constexpr int kAngle90 = 0x4000;

inline pof::SubObject Triangle(int parent, pof::Vec offset = {}, std::string props = "") {
  pof::SubObject so;
  so.parent = parent;
  so.offset = offset;
  so.props = std::move(props);
  so.verts = {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}};
  pof::Face f;
  f.normal = {0, 0, 1};
  f.texnum = 0;
  f.verts = {{0, 0, 0}, {1, 1, 0}, {2, 0, 1}};
  so.faces = {f};
  return so;
}

// Untimed angles are per-key increments, so this gives cumulative orientations of 0, 90 and 180.
inline std::vector<pof::RotKey> YawKeys_0_90_180() {
  return {{0, {0, 1, 0}, 0}, {0, {0, 1, 0}, kAngle90}, {0, {0, 1, 0}, kAngle90}};
}

} // namespace model_test
