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

#include "renderer_helpers.h"

#include "d3_test/renderer.h"


matrix OrientationFrom(vector forward, vector up) {
  matrix m;
  vm_VectorToMatrix(&m, &forward, &up, nullptr);
  return m;
}

matrix TiltedOrientation() { return OrientationFrom(vector{0.4f, -0.3f, 0.85f}, vector{0.2f, 1.0f, 0.1f}); }

vector CameraToWorld(const vector &view_pos, const matrix &view_orient, const vector &cam) {
  return view_pos + view_orient.rvec * cam.x() + view_orient.uvec * cam.y() + view_orient.fvec * cam.z();
}

void StartTestFrame(int width, int height, const vector &view_pos, const matrix &view_orient, float zoom) {
  d3_test::RendererViewport() = d3_test::FakeViewport{0, 0, width, height};
  vector pos = view_pos;
  matrix orient = view_orient;
  g3_StartFrame(&pos, &orient, zoom);
  g3_SetCustomClipPlane(0, nullptr, nullptr);
}

void ProjectWithFullTransform(const vector &p, float &sx, float &sy) {
  const float in[4] = {p.x(), p.y(), p.z(), 1.0f};
  float out[4];
  for (int col = 0; col < 4; ++col) {
    out[col] = 0.0f;
    for (int row = 0; row < 4; ++row) {
      out[col] += in[row] * gTransformFull[row][col];
    }
  }
  sx = out[0] / out[3];
  sy = out[1] / out[3];
}

g3Point MakePoint(float x, float y, float z) {
  g3Point p{};
  p.p3_vec = vector{x, y, z};
  p.p3_flags = 0;
  g3_CodePoint(&p);
  return p;
}

g3Point MakeUVPoint(float x, float y, float z, float u, float v) {
  g3Point p = MakePoint(x, y, z);
  p.p3_u = u;
  p.p3_v = v;
  p.p3_flags |= PF_UV;
  return p;
}

matrix ObjectOrientation() { return OrientationFrom(vector{-0.6f, 0.5f, 0.3f}, vector{0.3f, 0.2f, 1.0f}); }

vector LocalToWorld(const vector &obj_pos, const matrix &obj_orient, const vector &local) {
  return obj_pos + obj_orient.rvec * local.x() + obj_orient.uvec * local.y() + obj_orient.fvec * local.z();
}

g3Point RotatedPoint(const vector &v) {
  g3Point p{};
  vector tmp = v;
  g3_RotatePoint(&p, &tmp);
  return p;
}

g3Point RotatedUVPoint(const vector &world, float u, float v) {
  g3Point p = RotatedPoint(world);
  p.p3_u = u;
  p.p3_v = v;
  p.p3_flags |= PF_UV;
  return p;
}

g3Point RotateAndProject(const vector &world) {
  g3Point p = RotatedPoint(world);
  g3_ProjectPoint(&p);
  return p;
}
