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

#include "3d.h"

// Orthonormal orientation with the given forward direction, and up vector as
// close as possible to `up` (neither needs to be normalized).
matrix OrientationFrom(vector forward, vector up);

// An orientation that is not axis-aligned, so tests do not pass by accident
// because some matrix happens to be the identity.
matrix TiltedOrientation();

// World-space position of a point given in camera (unscaled view) coordinates:
// cam.x along rvec, cam.y along uvec, cam.z along fvec.
vector CameraToWorld(const vector &view_pos, const matrix &view_orient, const vector &cam);

// Projects a world-space point (or an instance-local point while an instance
// is active) through gTransformFull, the model-view * projection * viewport
// matrix that the GPU path uses, returning screen coordinates in pixels.
// The matrices use the row-vector convention: [x y z 1] * M.
void ProjectWithFullTransform(const vector &p, float &sx, float &sy);

// Starts a frame on a width x height viewport of the fake renderer. g3_StartFrame does not reset the custom clip
// plane, so this clears it.
void StartTestFrame(int width, int height, const vector &view_pos, const matrix &view_orient, float zoom);

g3Point MakePoint(float x, float y, float z);

g3Point MakeUVPoint(float x, float y, float z, float u, float v);

// A tilted object orientation; rows are the object's right/up/forward axes in world space.
matrix ObjectOrientation();

// World-space position of a point given in an object's local coordinates.
vector LocalToWorld(const vector &obj_pos, const matrix &obj_orient, const vector &local);

// A point rotated into view space by g3_RotatePoint.
g3Point RotatedPoint(const vector &v);

// A point rotated into view space by g3_RotatePoint, carrying UVs (u, v).
g3Point RotatedUVPoint(const vector &world, float u = 0.0f, float v = 0.0f);

// A point rotated into view space and projected to the screen.
g3Point RotateAndProject(const vector &world);
