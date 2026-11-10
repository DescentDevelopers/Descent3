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

#include <gtest/gtest.h>

#include "3d.h"
#include "HardwareInternal.h"
#include "support/renderer_checks.h"
#include "support/renderer_helpers.h"

namespace {

constexpr float kPixelTol = 0.01f;

const vector kViewPos{12.0f, -7.0f, 30.0f};

class InstanceTest : public testing::Test {
protected:
  void SetUp() override { StartTestFrame(640, 480, kViewPos, TiltedOrientation(), 0.726f); }
};

TEST_F(InstanceTest, LocalPointsRotateLikeTheirWorldPositions) {
  vector obj_pos = CameraToWorld(kViewPos, TiltedOrientation(), vector{1.0f, -0.5f, 20.0f});
  matrix obj_orient = ObjectOrientation();
  const vector locals[] = {{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 2.0f, 0.0f}, {-1.5f, 0.5f, 3.0f}};

  g3Point expected[4];
  for (int i = 0; i < 4; ++i)
    expected[i] = RotatedPoint(LocalToWorld(obj_pos, obj_orient, locals[i]));

  g3_StartInstanceMatrix(&obj_pos, &obj_orient);
  for (int i = 0; i < 4; ++i) {
    SCOPED_TRACE(testing::Message() << "local point " << i);
    ExpectSameRotation(RotatedPoint(locals[i]), expected[i]);
  }
  g3_DoneInstance();
}

TEST_F(InstanceTest, GpuTransformFollowsInstance) {
  vector obj_pos = CameraToWorld(kViewPos, TiltedOrientation(), vector{-2.0f, 1.0f, 15.0f});
  matrix obj_orient = ObjectOrientation();
  const vector local{0.5f, -1.0f, 2.0f};

  g3_StartInstanceMatrix(&obj_pos, &obj_orient);
  g3Point p = RotatedPoint(local);
  g3_ProjectPoint(&p);
  float gpu_sx, gpu_sy;
  ProjectWithFullTransform(local, gpu_sx, gpu_sy);
  g3_DoneInstance();

  ASSERT_TRUE(p.p3_flags & PF_PROJECTED);
  EXPECT_NEAR(p.p3_sx, gpu_sx, kPixelTol);
  EXPECT_NEAR(p.p3_sy, gpu_sy, kPixelTol);
}

TEST_F(InstanceTest, NestedInstancesComposeAndUnwind) {
  vector outer_pos = CameraToWorld(kViewPos, TiltedOrientation(), vector{0.0f, 0.0f, 25.0f});
  matrix outer_orient = ObjectOrientation();
  vector inner_pos{0.0f, 3.0f, 1.0f}; // relative to the outer object
  matrix inner_orient = OrientationFrom(vector{1.0f, 0.0f, 0.2f}, vector{0.0f, 0.3f, 1.0f});

  const vector inner_local{1.0f, 1.0f, 0.0f};
  const vector world_probe = CameraToWorld(kViewPos, TiltedOrientation(), vector{3.0f, 2.0f, 12.0f});

  vector in_outer = LocalToWorld(inner_pos, inner_orient, inner_local);
  g3Point expected_inner = RotatedPoint(LocalToWorld(outer_pos, outer_orient, in_outer));
  g3Point probe_before = RotatedPoint(world_probe);
  float gpu_before_sx, gpu_before_sy;
  ProjectWithFullTransform(world_probe, gpu_before_sx, gpu_before_sy);

  g3_StartInstanceMatrix(&outer_pos, &outer_orient);
  g3Point outer_probe_before = RotatedPoint(in_outer);

  g3_StartInstanceMatrix(&inner_pos, &inner_orient);
  ExpectSameRotation(RotatedPoint(inner_local), expected_inner);
  g3_DoneInstance();

  ExpectSameRotation(RotatedPoint(in_outer), outer_probe_before);
  g3_DoneInstance();

  ExpectSameRotation(RotatedPoint(world_probe), probe_before);
  float gpu_after_sx, gpu_after_sy;
  ProjectWithFullTransform(world_probe, gpu_after_sx, gpu_after_sy);
  EXPECT_NEAR(gpu_after_sx, gpu_before_sx, kPixelTol);
  EXPECT_NEAR(gpu_after_sy, gpu_before_sy, kPixelTol);
}

} // namespace
