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

#include <ostream>
#include <string>

#include <gtest/gtest.h>

#include "3d.h"
#include "HardwareInternal.h"
#include "support/renderer_helpers.h"

namespace {

// The game's default zoom: tan(72 deg / 2), see D3_DEFAULT_ZOOM and
// StartFrame() in Descent3/game.cpp, where zoom = tan(FOV / 2) for the
// horizontal FOV of a 4:3 view.
constexpr float kZoom = 0.726f;

constexpr float kPixelTol = 0.01f;

const vector kViewPos{12.0f, -7.0f, 30.0f};

struct Viewport {
  const char *name;
  int width;
  int height;
};

std::string ViewportName(const testing::TestParamInfo<Viewport> &info) { return info.param.name; }

void PrintTo(const Viewport &v, std::ostream *os) { *os << v.width << "x" << v.height; }

const Viewport kLandscapeViewports[] = {
    {"Wide4x3", 640, 480},
    {"Wide16x9", 1920, 1080},
    {"Wide5x4", 1280, 1024},
};

const Viewport kAllViewports[] = {
    {"Wide4x3", 640, 480}, {"Wide16x9", 1920, 1080}, {"Square", 512, 512},
    {"Tall3x4", 480, 640}, {"Tall9x16", 1080, 1920},
};

class ProjectionTest : public testing::TestWithParam<Viewport> {
protected:
  void SetUp() override {
    orient_ = TiltedOrientation();
    w_ = static_cast<float>(GetParam().width);
    h_ = static_cast<float>(GetParam().height);
    StartTestFrame(GetParam().width, GetParam().height, kViewPos, orient_, kZoom);
  }

  matrix orient_;
  float w_ = 0.0f;
  float h_ = 0.0f;
};

using LandscapeProjectionTest = ProjectionTest;

TEST_P(ProjectionTest, PointStraightAheadProjectsToViewportCenter) {
  g3Point p = RotateAndProject(CameraToWorld(kViewPos, orient_, vector{0.0f, 0.0f, 25.0f}));

  EXPECT_EQ(p.p3_codes, 0);
  ASSERT_TRUE(p.p3_flags & PF_PROJECTED);
  EXPECT_NEAR(p.p3_sx, w_ / 2, kPixelTol);
  EXPECT_NEAR(p.p3_sy, h_ / 2, kPixelTol);
}

TEST_P(ProjectionTest, Point2VecRoundTripsThroughProjection) {
  const int w = GetParam().width;
  const int h = GetParam().height;
  const int16_t pixels[][2] = {
      {0, 0}, {static_cast<int16_t>(w / 2), static_cast<int16_t>(h / 2)},
      {static_cast<int16_t>(w - 1), static_cast<int16_t>(h - 1)},
      {static_cast<int16_t>(w / 4), static_cast<int16_t>(3 * h / 4)},
  };
  for (const auto &px : pixels) {
    SCOPED_TRACE(testing::Message() << "pixel " << px[0] << "," << px[1]);
    vector dir;
    g3_Point2Vec(&dir, px[0], px[1]);

    g3Point p = RotateAndProject(kViewPos + dir * 40.0f);

    EXPECT_NEAR(p.p3_sx, px[0], kPixelTol);
    EXPECT_NEAR(p.p3_sy, px[1], kPixelTol);
  }
}

INSTANTIATE_TEST_SUITE_P(AllAspects, ProjectionTest, testing::ValuesIn(kAllViewports), ViewportName);

// zoom is tan(half the horizontal FOV) of a 4:3 view, and g3_StartFrame
// converts it to the vertical half-FOV tangent (zoom * 3/4). For landscape
// windows the vertical FOV is therefore fixed and the horizontal FOV grows
// with the aspect ratio ("Hor+").
TEST_P(LandscapeProjectionTest, ViewEdgesFollowHorPlusFov) {
  const float d = 20.0f;
  const float tan_half_vfov = kZoom * 3.0f / 4.0f;
  const float tan_half_hfov = tan_half_vfov * w_ / h_;

  g3Point top = RotateAndProject(CameraToWorld(kViewPos, orient_, vector{0.0f, tan_half_vfov * d, d}));
  g3Point right = RotateAndProject(CameraToWorld(kViewPos, orient_, vector{tan_half_hfov * d, 0.0f, d}));

  EXPECT_NEAR(top.p3_sy, 0.0f, kPixelTol);
  EXPECT_NEAR(top.p3_sx, w_ / 2, kPixelTol);
  EXPECT_NEAR(right.p3_sx, w_, kPixelTol);
  EXPECT_NEAR(right.p3_sy, h_ / 2, kPixelTol);
}

TEST_P(LandscapeProjectionTest, ClipCodesMatchViewEdges) {
  const float d = 20.0f;
  const float ty = kZoom * 3.0f / 4.0f * d; // half-height of the view at depth d
  const float tx = ty * w_ / h_;            // half-width of the view at depth d
  const float in = 0.99f, out = 1.01f;

  struct Case {
    float x, y;
    uint8_t expected;
  } cases[] = {
      {0.0f, 0.0f, 0},
      {tx * in, 0.0f, 0},
      {tx * out, 0.0f, CC_OFF_RIGHT},
      {-tx * in, 0.0f, 0},
      {-tx * out, 0.0f, CC_OFF_LEFT},
      {0.0f, ty * in, 0},
      {0.0f, ty * out, CC_OFF_TOP},
      {0.0f, -ty * in, 0},
      {0.0f, -ty * out, CC_OFF_BOT},
      {-tx * out, -ty * out, CC_OFF_LEFT | CC_OFF_BOT},
      {tx * in, ty * in, 0},
  };
  for (const Case &c : cases) {
    SCOPED_TRACE(testing::Message() << "camera-space x=" << c.x << " y=" << c.y);
    vector world = CameraToWorld(kViewPos, orient_, vector{c.x, c.y, d});
    g3Point p{};
    EXPECT_EQ(g3_RotatePoint(&p, &world), c.expected);
    EXPECT_EQ(p.p3_codes, c.expected);
  }
}

INSTANTIATE_TEST_SUITE_P(Landscape, LandscapeProjectionTest, testing::ValuesIn(kLandscapeViewports), ViewportName);

class FrameTest : public testing::Test {
protected:
  void SetUp() override {
    orient_ = TiltedOrientation();
    StartTestFrame(640, 480, kViewPos, orient_, kZoom);
  }
  matrix orient_;
};

TEST_F(FrameTest, PointBehindViewerIsCodedBehindAndNotProjected) {
  vector world = CameraToWorld(kViewPos, orient_, vector{0.0f, 0.0f, -5.0f});
  g3Point p{};
  uint8_t codes = g3_RotatePoint(&p, &world);
  g3_ProjectPoint(&p);

  EXPECT_TRUE(codes & CC_BEHIND);
  EXPECT_FALSE(p.p3_flags & PF_PROJECTED);
}

TEST_F(FrameTest, FarClipPlaneCodesDistantPointsAndIsResetByStartFrame) {
  vector near_pt = CameraToWorld(kViewPos, orient_, vector{0.0f, 0.0f, 50.0f});
  vector far_pt = CameraToWorld(kViewPos, orient_, vector{0.0f, 0.0f, 150.0f});
  g3Point p{};

  EXPECT_EQ(g3_RotatePoint(&p, &far_pt), 0);

  g3_SetFarClipZ(100.0f);
  EXPECT_EQ(g3_RotatePoint(&p, &near_pt), 0);
  EXPECT_EQ(g3_RotatePoint(&p, &far_pt), CC_OFF_FAR);

  StartTestFrame(640, 480, kViewPos, orient_, kZoom);
  EXPECT_EQ(g3_RotatePoint(&p, &far_pt), 0);
}

TEST_F(FrameTest, PointDepthIsDistanceAlongViewDirection) {
  vector world = CameraToWorld(kViewPos, orient_, vector{3.0f, -2.0f, 7.0f});
  g3Point p{};
  g3_RotatePoint(&p, &world);

  EXPECT_NEAR(g3_CalcPointDepth(&world), 7.0f, 1e-4f);
  EXPECT_NEAR(p.p3_z, 7.0f, 1e-4f);
}

} // namespace
