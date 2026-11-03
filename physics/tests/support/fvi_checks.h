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

#include "vecmat.h"

#include "fvi_polygon.h"

namespace physics_test {

// Runs FastVectorBBox for the box [mn, mx] and the ray from origin along dir.
bool RayHitsBox(const vector &mn, const vector &mx, const vector &origin, const vector &dir);

// The outputs of a check_line_to_face call.
struct FaceContact {
  vector newp{}, colp{}, wall_norm{};
  float dist = -1.0f;
};

// Runs check_line_to_face for a sphere of radius rad moving from p0 to p1 against face, storing its outputs in contact.
int SweepSphereToFace(Polygon &face, const vector &p0, const vector &p1, float rad, FaceContact &contact);

// Drops a unit sphere onto face 0.5 from vertex (outside only edge expected_only_edge); expects it to touch vertex.
void ExpectDropNearVertexTouchesVertex(Polygon &face, const vector &vertex, vector offset_dir, int expected_only_edge,
                                       float tol);

} // namespace physics_test
