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

#include <cmath>
#include <cstring>
#include <filesystem>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "cfile.h"
#include "fix.h"
#include "grdefs.h"
#include "polymodel.h"
#include "vecmat.h"

#include "support/pof_builders.h"
#include "support/pof_writer.h"
#include "support/polymodel_harness.h"
#include "d3_test/game.h"
#include "d3_test/game.h"
#include "support/test_stubs.h"

extern float Gametime;

using namespace model_test;

namespace {

const float kHalfSqrt2 = std::sqrt(2.0f) / 2.0f;

// Version 23.00: timed animation, per-vertex alpha.
constexpr int kTimedVersion = 2300;
// Version 20.00: untimed (shared keyframe count) animation, gun points have parents.
constexpr int kUntimedVersion = 2000;

class PolyModelTest : public ::testing::Test {
protected:
  // The game builds the sin/cos lookup tables at startup (Descent3/init.cpp); the keyframe and
  // orientation math depends on them.
  static void SetUpTestSuite() { InitMathTables(); }

  void SetUp() override {
    ResetStubs();
    Gametime = 0.0f;
  }

  void TearDown() override {
    FreeAllPolyModels();
    cf_ClearAllSearchPaths();
    Gametime = 0.0f;

    EXPECT_LE(asserts_.count(), max_assertions_) << "unexpected ASSERT/Int3 inside polymodel code";
  }

  ModelDir models_;
  AssertionCounter asserts_;
  int max_assertions_ = 0;
};

TEST_F(PolyModelTest, LoadsSubobjectHierarchyGeometryAndFaces) {
  pof::Writer w(kTimedVersion);
  w.header(3); // header mins/maxs left at zero: the loader recomputes them from the geometry

  pof::SubObject root;
  root.parent = -1;
  root.name = "body";
  root.verts = {{0, 0, 0}, {-1, -2, -3}, {4, 5, 6}};
  pof::Face textured;
  textured.normal = {0, 0, 1};
  textured.texnum = 0;
  textured.verts = {{0, 0.0f, 0.25f}, {1, 0.5f, 0.75f}, {2, 1.0f, 0.125f}};
  root.faces = {textured};
  w.subobject(0, root);

  pof::SubObject arm;
  arm.parent = 0;
  arm.name = "arm";
  arm.offset = {10, 0, 0};
  arm.verts = {{-1, -1, -1}, {1, 1, 1}, {1, -1, 1}};
  pof::Face flat;
  flat.normal = {0, 1, 0};
  flat.r = 255;
  flat.g = 128;
  flat.b = 0;
  flat.verts = {{2, 0, 0}, {1, 0, 0}, {0, 0, 0}};
  arm.faces = {flat};
  w.subobject(1, arm);

  pof::SubObject hand;
  hand.parent = 1;
  hand.offset = {0, 20, 0};
  hand.verts = {{0, 0, 0}, {2, 2, 2}, {0, 2, 0}};
  w.subobject(2, hand);

  poly_model *pm = models_.LoadOrFail(w);
  ASSERT_NE(pm, nullptr);

  EXPECT_FALSE(pm->flags & PMF_NOT_RESIDENT);
  ASSERT_EQ(pm->n_models, 3);

  bsp_info *sm = pm->submodel;
  EXPECT_EQ(sm[0].parent, -1);
  EXPECT_EQ(sm[1].parent, 0);
  EXPECT_EQ(sm[2].parent, 1);
  ASSERT_EQ(sm[0].num_children, 1);
  EXPECT_EQ(sm[0].children[0], 1);
  ASSERT_EQ(sm[1].num_children, 1);
  EXPECT_EQ(sm[1].children[0], 2);
  EXPECT_EQ(sm[2].num_children, 0);

  EXPECT_STREQ(sm[0].name, "body");
  EXPECT_STREQ(sm[1].name, "arm");

  ASSERT_EQ(sm[0].nverts, 3);
  EXPECT_VEC_NEAR(sm[0].verts[1], -1, -2, -3);
  EXPECT_VEC_NEAR(sm[0].min, -1, -2, -3);
  EXPECT_VEC_NEAR(sm[0].max, 4, 5, 6);

  ASSERT_EQ(sm[0].num_faces, 1);
  const polyface &tf = sm[0].faces[0];
  EXPECT_EQ(tf.texnum, 0);
  ASSERT_EQ(tf.nverts, 3);
  EXPECT_EQ(tf.vertnums[0], 0);
  EXPECT_EQ(tf.vertnums[1], 1);
  EXPECT_EQ(tf.vertnums[2], 2);
  EXPECT_FLOAT_EQ(tf.u[1], 0.5f);
  EXPECT_FLOAT_EQ(tf.v[1], 0.75f);
  EXPECT_FLOAT_EQ(tf.v[2], 0.125f);
  EXPECT_VEC_NEAR(tf.normal, 0, 0, 1);
  EXPECT_VEC_NEAR(sm[0].face_min[0], -1, -2, -3);
  EXPECT_VEC_NEAR(sm[0].face_max[0], 4, 5, 6);

  ASSERT_EQ(sm[1].num_faces, 1);
  EXPECT_EQ(sm[1].faces[0].texnum, -1);
  EXPECT_EQ(sm[1].faces[0].color, GR_RGB(255, 128, 0));
  EXPECT_EQ(sm[1].faces[0].vertnums[0], 2);

  EXPECT_EQ(sm[2].num_faces, 0);

  // Whole-model bounds: union of subobject bounds, each shifted by its accumulated offset
  // (hand sits at arm offset + hand offset = (10, 20, 0)).
  EXPECT_VEC_NEAR(pm->mins, -1, -2, -3);
  EXPECT_VEC_NEAR(pm->maxs, 12, 22, 6);
}

TEST_F(PolyModelTest, SubobjectParentedToItselfIsTreatedAsRoot) {
  pof::Writer w(kTimedVersion);
  w.header(2);
  w.subobject(0, Triangle(/*parent=*/0));
  w.subobject(1, Triangle(/*parent=*/0, {5, 0, 0}));

  poly_model *pm = models_.LoadOrFail(w);
  ASSERT_NE(pm, nullptr);

  EXPECT_EQ(pm->submodel[0].parent, -1);
  ASSERT_EQ(pm->submodel[0].num_children, 1);
  EXPECT_EQ(pm->submodel[0].children[0], 1);
  EXPECT_VEC_NEAR(pm->mins, 0, 0, 0);
  EXPECT_VEC_NEAR(pm->maxs, 6, 1, 0);
}

TEST_F(PolyModelTest, TranslucentVertexAlphaMarksModelAsAlpha) {
  {
    pof::Writer w(kTimedVersion);
    w.header(1);
    pof::SubObject so = Triangle(-1);
    so.alpha = {1.0f, 1.0f, 1.0f};
    w.subobject(0, so);
    poly_model *pm = models_.LoadOrFail(w, "opaque.oof");
    ASSERT_NE(pm, nullptr);
    EXPECT_FALSE(pm->flags & PMF_ALPHA);
  }
  {
    pof::Writer w(kTimedVersion);
    w.header(1);
    pof::SubObject so = Triangle(-1);
    so.alpha = {1.0f, 0.5f, 1.0f};
    w.subobject(0, so);
    poly_model *pm = models_.LoadOrFail(w, "translucent.oof");
    ASSERT_NE(pm, nullptr);
    EXPECT_TRUE(pm->flags & PMF_ALPHA);
    EXPECT_FLOAT_EQ(pm->submodel[0].alpha[1], 0.5f);
  }
}

TEST_F(PolyModelTest, ReadsGunGroundAndAttachPoints) {
  pof::Writer w(kTimedVersion);
  w.header(2);
  w.subobject(0, Triangle(-1));
  w.subobject(1, Triangle(0));
  w.gun_points({{1, {1, 2, 3}, {0, 0, 1}}, {0, {-4, 5, -6}, {1, 0, 0}}});
  w.ground_planes({{0, {0, -1, 0}, {0, -1, 0}}});
  w.attach_points({{1, {7, 8, 9}, {0, 1, 0}}, {0, {0, 0, 2}, {0, 0, 1}}});
  w.attach_uvecs({{1, 0, 0}, {0, 1, 0}});

  poly_model *pm = models_.LoadOrFail(w);
  ASSERT_NE(pm, nullptr);

  ASSERT_EQ(pm->n_guns, 2);
  EXPECT_EQ(pm->gun_slots[0].parent, 1);
  EXPECT_VEC_NEAR(pm->gun_slots[0].pnt, 1, 2, 3);
  EXPECT_VEC_NEAR(pm->gun_slots[0].norm, 0, 0, 1);
  EXPECT_EQ(pm->gun_slots[1].parent, 0);
  EXPECT_VEC_NEAR(pm->gun_slots[1].pnt, -4, 5, -6);

  ASSERT_EQ(pm->n_ground, 1);
  EXPECT_VEC_NEAR(pm->ground_slots[0].pnt, 0, -1, 0);

  ASSERT_EQ(pm->n_attach, 2);
  EXPECT_EQ(pm->attach_slots[0].parent, 1);
  EXPECT_VEC_NEAR(pm->attach_slots[0].pnt, 7, 8, 9);
  EXPECT_VEC_NEAR(pm->attach_slots[0].norm, 0, 1, 0);
  EXPECT_TRUE(pm->attach_slots[0].f_uvec);
  EXPECT_VEC_NEAR(pm->attach_slots[0].uvec, 1, 0, 0);
  EXPECT_TRUE(pm->attach_slots[1].f_uvec);
  EXPECT_VEC_NEAR(pm->attach_slots[1].uvec, 0, 1, 0);
}

TEST_F(PolyModelTest, GunPointsBeforeVersion1908HaveNoParentAndAttachToRoot) {
  pof::Writer w(1900);
  w.header(2);
  w.subobject(0, Triangle(-1));
  w.subobject(1, Triangle(0));
  w.gun_points({{/*parent: not written*/ 1, {1, 2, 3}, {0, 0, 1}}});

  poly_model *pm = models_.LoadOrFail(w);
  ASSERT_NE(pm, nullptr);

  ASSERT_EQ(pm->n_guns, 1);
  EXPECT_EQ(pm->gun_slots[0].parent, 0);
  EXPECT_VEC_NEAR(pm->gun_slots[0].pnt, 1, 2, 3);
  EXPECT_VEC_NEAR(pm->gun_slots[0].norm, 0, 0, 1);
}

TEST_F(PolyModelTest, AttachUvecsAreIgnoredWhenCountDoesNotMatchAttachPoints) {
  pof::Writer w(kTimedVersion);
  w.header(1);
  w.subobject(0, Triangle(-1));
  w.attach_points({{0, {1, 0, 0}, {0, 1, 0}}, {0, {2, 0, 0}, {0, 1, 0}}});
  w.attach_uvecs({{1, 0, 0}});

  poly_model *pm = models_.LoadOrFail(w);
  ASSERT_NE(pm, nullptr);

  ASSERT_EQ(pm->n_attach, 2);
  EXPECT_FALSE(pm->attach_slots[0].f_uvec);
  EXPECT_FALSE(pm->attach_slots[1].f_uvec);
  EXPECT_EQ(model_test::DataErrorCount(), 1);
}

TEST_F(PolyModelTest, UnknownChunksAreSkipped) {
  pof::Writer w(kTimedVersion);
  w.header(1);
  w.unknown_chunk("ZZZZ", std::vector<uint8_t>(37, 0xAB));
  w.subobject(0, Triangle(-1));
  w.unknown_chunk("INFO", {'h', 'e', 'l', 'l', 'o'});
  w.gun_points({{0, {1, 2, 3}, {0, 0, 1}}});

  poly_model *pm = models_.LoadOrFail(w);
  ASSERT_NE(pm, nullptr);

  EXPECT_EQ(pm->submodel[0].nverts, 3);
  ASSERT_EQ(pm->n_guns, 1);
  EXPECT_VEC_NEAR(pm->gun_slots[0].pnt, 1, 2, 3);
}

TEST_F(PolyModelTest, TextureNamesResolveByBitmapNameThenTextureName) {
  d3_test::SetTextureBitmapName("hull.ogf", 5);
  d3_test::SetTextureName("hull", 99);
  d3_test::SetTextureName("glass", 7);

  pof::Writer w(kTimedVersion);
  w.header(1);
  w.textures({"hull", "glass", "nonexistent"});
  w.subobject(0, Triangle(-1));

  poly_model *pm = models_.LoadOrFail(w);
  ASSERT_NE(pm, nullptr);

  ASSERT_EQ(pm->n_textures, 3);
  EXPECT_EQ(pm->textures[0], 5);
  EXPECT_EQ(pm->textures[1], 7);
  EXPECT_EQ(pm->textures[2], 0);
  EXPECT_FALSE(pm->flags & PMF_ALPHA);
}

TEST_F(PolyModelTest, TranslucentTextureMarksModelAsAlpha) {
  d3_test::SetTextureName("glass", 7);
  GameTextures[7].alpha = 0.5f;

  pof::Writer w(kTimedVersion);
  w.header(1);
  w.textures({"glass"});
  w.subobject(0, Triangle(-1));

  poly_model *pm = models_.LoadOrFail(w);
  ASSERT_NE(pm, nullptr);
  EXPECT_TRUE(pm->flags & PMF_ALPHA);
}

TEST_F(PolyModelTest, FlagPropertiesSetSubobjectFlags) {
  struct Case {
    const char *props;
    int flag;
  };
  const std::vector<Case> cases = {
      {"$shell", SOF_SHELL},         {"$SHELL", SOF_SHELL}, // commands are case-insensitive
      {"$frontface", SOF_FRONTFACE}, {"$jitter", SOF_JITTER},
      {"$monitor01", SOF_MONITOR1},  {"$monitor02", SOF_MONITOR2},
      {"$monitor03", SOF_MONITOR3},  {"$monitor04", SOF_MONITOR4},
      {"$Monitor05", SOF_MONITOR5},  {"$monitor06", SOF_MONITOR6},
      {"$monitor07", SOF_MONITOR7},  {"$monitor08", SOF_MONITOR8},
      {"$viewer", SOF_VIEWER},       {"$layer", SOF_LAYER},
      {"$custom", SOF_CUSTOM},       {"", 0},
      {"$nonsense", 0},              {"just some notes", 0},
  };
  ASSERT_LE(cases.size(), static_cast<size_t>(MAX_SUBOBJECTS));

  pof::Writer w(kTimedVersion);
  w.header(static_cast<int>(cases.size()));
  for (size_t i = 0; i < cases.size(); i++)
    w.subobject(static_cast<int>(i), Triangle(i == 0 ? -1 : 0, {}, cases[i].props));

  poly_model *pm = models_.LoadOrFail(w);
  ASSERT_NE(pm, nullptr);

  for (size_t i = 0; i < cases.size(); i++)
    EXPECT_EQ(pm->submodel[i].flags, cases[i].flag) << "props = \"" << cases[i].props << "\"";
}

TEST_F(PolyModelTest, RotateAndFovPropertiesConfigureRotatorsAndTurrets) {
  const std::vector<std::string> props = {
      "$rotate=4",          // one revolution every 4 seconds
      "$rotate=0",          // out of range (must be > 0): ignored
      "$rotate=25",         // out of range (must be <= 20): ignored
      "$fov=90,2,1.5",      // 90 degree arc, 2 seconds per revolution, 1.5s reaction time
  };

  pof::Writer w(kTimedVersion);
  w.header(static_cast<int>(props.size()));
  std::vector<pof::Track> tracks;
  for (size_t i = 0; i < props.size(); i++) {
    w.subobject(static_cast<int>(i), Triangle(i == 0 ? -1 : 0, {}, props[i]));
    // Rotators and turrets need rotation keyframes (the axis comes from them).
    pof::Track t;
    t.track_min = 0;
    t.track_max = 10;
    t.rot = {{0, {0, 1, 0}, 0}, {10, {0, 1, 0}, 0}};
    tracks.push_back(t);
  }
  w.rot_anim(tracks);

  poly_model *pm = models_.LoadOrFail(w);
  ASSERT_NE(pm, nullptr);
  bsp_info *sm = pm->submodel;

  EXPECT_EQ(sm[0].flags, SOF_ROTATE);
  EXPECT_NEAR(sm[0].rps, 0.25f, 1e-6f);

  EXPECT_EQ(sm[1].flags, 0);
  EXPECT_EQ(sm[2].flags, 0);

  EXPECT_EQ(sm[3].flags, SOF_TURRET);
  // fov is half the allowed arc as a fraction of a full turn: 45 / 360.
  EXPECT_NEAR(sm[3].fov, 0.125f, 1e-6f);
  EXPECT_NEAR(sm[3].rps, 0.5f, 1e-6f);
  EXPECT_NEAR(sm[3].think_interval, 1.5f, 1e-6f);
}

TEST_F(PolyModelTest, RotatorsAndTurretsWithoutKeyframesAreDisabled) {
  pof::Writer w(kTimedVersion);
  w.header(2);
  w.subobject(0, Triangle(-1, {}, "$rotate=4"));
  w.subobject(1, Triangle(0, {}, "$fov=90,2,1.5"));

  poly_model *pm = models_.LoadOrFail(w);
  ASSERT_NE(pm, nullptr);

  EXPECT_FALSE(pm->submodel[0].flags & SOF_ROTATE);
  EXPECT_FALSE(pm->submodel[1].flags & SOF_TURRET);
}

TEST_F(PolyModelTest, GlowAndThrusterPropertiesStoreColorSizeAndFaceNormal) {
  pof::Writer w(kTimedVersion);
  w.header(2);
  w.subobject(0, Triangle(-1, {}, "$glow=1,0.5,0.25,3"));
  w.subobject(1, Triangle(0, {}, "$thruster=0.1,0.2,0.3,4.5"));

  poly_model *pm = models_.LoadOrFail(w);
  ASSERT_NE(pm, nullptr);

  const bsp_info &glow = pm->submodel[0];
  EXPECT_TRUE(glow.flags & SOF_GLOW);
  ASSERT_NE(glow.glow_info, nullptr);
  EXPECT_FLOAT_EQ(glow.glow_info->glow_r, 1.0f);
  EXPECT_FLOAT_EQ(glow.glow_info->glow_g, 0.5f);
  EXPECT_FLOAT_EQ(glow.glow_info->glow_b, 0.25f);
  EXPECT_FLOAT_EQ(glow.glow_info->glow_size, 3.0f);
  // The glow faces along its (single) face's normal; the triangle lies in the XY plane.
  EXPECT_NEAR(std::fabs(glow.glow_info->normal.z()), 1.0f, kTol);

  const bsp_info &thruster = pm->submodel[1];
  EXPECT_TRUE(thruster.flags & SOF_THRUSTER);
  EXPECT_FALSE(thruster.flags & SOF_GLOW);
  ASSERT_NE(thruster.glow_info, nullptr);
  EXPECT_FLOAT_EQ(thruster.glow_info->glow_r, 0.1f);
  EXPECT_FLOAT_EQ(thruster.glow_info->glow_size, 4.5f);

  // Glows are drawn in the facing pass.
  EXPECT_TRUE(pm->flags & PMF_FACING);
}

TEST_F(PolyModelTest, DoorShellAndFaceAndEmptySubobjectsAreNotRenderable) {
  pof::Writer w(kTimedVersion);
  w.header(4);
  w.subobject(0, Triangle(-1));
  w.subobject(1, Triangle(0, {}, "$shell"));
  w.subobject(2, Triangle(0, {}, "$frontface"));
  pof::SubObject empty;
  empty.parent = 0;
  w.subobject(3, empty);

  poly_model *pm = models_.LoadOrFail(w);
  ASSERT_NE(pm, nullptr);

  EXPECT_FALSE(IsNonRenderableSubmodel(pm, 0));
  EXPECT_TRUE(IsNonRenderableSubmodel(pm, 1));
  EXPECT_TRUE(IsNonRenderableSubmodel(pm, 2));
  EXPECT_TRUE(IsNonRenderableSubmodel(pm, 3));
  EXPECT_EQ(CountFacesInPolymodel(pm), 1);
}

TEST_F(PolyModelTest, WeaponBatteryTurretTagsItsWholeSubtree) {
  // 0 root
  // +- 1 turret A (battery 0)
  // +- 2 plain
  //    +- 3 turret B (battery 1)
  //       +- 4 barrel
  pof::Writer w(kTimedVersion);
  w.header(5);
  w.subobject(0, Triangle(-1));
  w.subobject(1, Triangle(0, {}, "$fov=90,2,1"));
  w.subobject(2, Triangle(0));
  w.subobject(3, Triangle(2, {}, "$fov=90,2,1"));
  w.subobject(4, Triangle(3));
  w.weapon_batteries({{{0}, {1}}, {{1}, {3}}});
  std::vector<pof::Track> tracks(5);
  for (auto &t : tracks) {
    t.track_max = 10;
    t.rot = {{0, {0, 1, 0}, 0}, {10, {0, 1, 0}, 0}};
  }
  w.rot_anim(tracks);

  poly_model *pm = models_.LoadOrFail(w);
  ASSERT_NE(pm, nullptr);
  ASSERT_EQ(pm->num_wbs, 2);

  auto battery = [&](int sub) { return (pm->submodel[sub].flags & SOF_WB_MASKS) >> WB_INDEX_SHIFT; };

  EXPECT_FALSE(pm->submodel[0].flags & SOF_WB);
  EXPECT_FALSE(pm->submodel[2].flags & SOF_WB);

  EXPECT_TRUE(pm->submodel[1].flags & SOF_WB);
  EXPECT_EQ(battery(1), 0);

  EXPECT_TRUE(pm->submodel[3].flags & SOF_WB);
  EXPECT_EQ(battery(3), 1);
  EXPECT_TRUE(pm->submodel[4].flags & SOF_WB);
  EXPECT_EQ(battery(4), 1);
}

TEST_F(PolyModelTest, UntimedKeyframesInterpolatePositionAndRotation) {
  // Three evenly spaced keyframes: normalized time 0, 0.5 and 1.
  // Positions (0,0,0) -> (10,0,0) -> (10,20,0); yaw 0 -> 90 -> 180 degrees.
  pof::Writer w(kUntimedVersion);
  w.header(1);
  w.subobject(0, Triangle(-1));
  pof::Track t;
  t.rot = YawKeys_0_90_180();
  t.pos = {{0, {0, 0, 0}}, {0, {10, 0, 0}}, {0, {10, 20, 0}}};
  w.rot_anim({t});
  w.pos_anim({t});

  poly_model *pm = models_.LoadOrFail(w);
  ASSERT_NE(pm, nullptr);
  EXPECT_FALSE(pm->flags & PMF_TIMED);

  // A point one unit along the subobject's +X axis. A yaw of `a` about +Y maps it to
  // (cos a, 0, -sin a) (same convention as vm_AnglesToMatrix heading).
  const vector px{1, 0, 0};
  float nt[MAX_SUBOBJECTS] = {};

  nt[0] = 0.0f;
  EXPECT_VEC_NEAR(ModelPoint(pm, 0, px, nt), 1, 0, 0);

  nt[0] = 0.25f; // halfway between keys 0 and 1
  EXPECT_VEC_NEAR(ModelPoint(pm, 0, px, nt), 5 + kHalfSqrt2, 0, -kHalfSqrt2);

  nt[0] = 0.5f; // exactly key 1
  EXPECT_VEC_NEAR(ModelPoint(pm, 0, px, nt), 10, 0, -1);

  nt[0] = 0.75f; // halfway between keys 1 and 2
  EXPECT_VEC_NEAR(ModelPoint(pm, 0, px, nt), 10 - kHalfSqrt2, 10, -kHalfSqrt2);

  nt[0] = 1.0f;
  EXPECT_VEC_NEAR(ModelPoint(pm, 0, px, nt), 9, 20, 0);
}

TEST_F(PolyModelTest, TimedKeyframesInterpolateByKeyStartTime) {
  pof::Writer w(kTimedVersion);
  w.header(2);
  w.subobject(0, Triangle(-1));
  w.subobject(1, Triangle(0));

  pof::Track anim;
  anim.track_min = 0;
  anim.track_max = 30;
  anim.rot = {{0, {0, 1, 0}, 0}, {10, {0, 1, 0}, kAngle90}, {30, {0, 1, 0}, kAngle90}};
  anim.pos = {{0, {0, 0, 0}}, {10, {10, 0, 0}}, {30, {10, 20, 0}}};
  // A second, static subobject with a longer track extends the model's overall frame range.
  pof::Track still;
  still.track_min = 0;
  still.track_max = 50;
  w.rot_anim({anim, still});
  w.pos_anim({anim, still});

  poly_model *pm = models_.LoadOrFail(w);
  ASSERT_NE(pm, nullptr);
  EXPECT_TRUE(pm->flags & PMF_TIMED);
  EXPECT_EQ(pm->frame_min, 0);
  EXPECT_EQ(pm->frame_max, 50);

  const vector px{1, 0, 0};
  float nt[MAX_SUBOBJECTS] = {};
  auto at_frame = [&](float frame) {
    SetNormalizedTimeAnim(frame, nt, pm);
    return ModelPoint(pm, 0, px, nt);
  };

  EXPECT_VEC_NEAR(at_frame(0), 1, 0, 0);
  // Frame 5: halfway between the keys at 0 and 10.
  EXPECT_VEC_NEAR(at_frame(5), 5 + kHalfSqrt2, 0, -kHalfSqrt2);
  EXPECT_VEC_NEAR(at_frame(10), 10, 0, -1);
  // Frame 20: halfway between the keys at 10 and 30.
  EXPECT_VEC_NEAR(at_frame(20), 10 - kHalfSqrt2, 10, -kHalfSqrt2);
  EXPECT_VEC_NEAR(at_frame(30), 9, 20, 0);
  // Frames outside a subobject's track clamp to its ends.
  EXPECT_VEC_NEAR(at_frame(45), 9, 20, 0);
  EXPECT_VEC_NEAR(at_frame(-5), 1, 0, 0);
}

TEST_F(PolyModelTest, RotatorCompletesOneRevolutionPerSpinPeriod) {
  pof::Writer w(kTimedVersion);
  w.header(1);
  w.subobject(0, Triangle(-1, {}, "$rotate=4"));
  pof::Track t;
  t.track_max = 10;
  t.rot = {{0, {0, 1, 0}, 0}, {10, {0, 1, 0}, 0}};
  w.rot_anim({t});

  poly_model *pm = models_.LoadOrFail(w);
  ASSERT_NE(pm, nullptr);
  ASSERT_TRUE(pm->submodel[0].flags & SOF_ROTATE);

  const vector px{1, 0, 0};
  float nt[MAX_SUBOBJECTS] = {};

  Gametime = 1.0f; // quarter turn
  EXPECT_VEC_NEAR(ModelPoint(pm, 0, px, nt), 0, 0, -1);
  Gametime = 2.0f; // half turn
  EXPECT_VEC_NEAR(ModelPoint(pm, 0, px, nt), -1, 0, 0);
  Gametime = 7.0f; // one full turn plus three quarters
  EXPECT_VEC_NEAR(ModelPoint(pm, 0, px, nt), 0, 0, 1);
}

TEST_F(PolyModelTest, PointInWorldComposesSubobjectChainAndObjectPlacement) {
  pof::Writer w(kTimedVersion);
  w.header(2);
  w.subobject(0, Triangle(-1, {0, 3, 0}));
  w.subobject(1, Triangle(0, {0, 0, 5}));
  pof::Track turn;
  turn.track_max = 10;
  turn.rot = {{0, {0, 1, 0}, 0}, {10, {0, 1, 0}, kAngle90}};
  pof::Track none;
  w.rot_anim({turn, none});

  poly_model *pm = models_.LoadOrFail(w);
  ASSERT_NE(pm, nullptr);

  float nt[MAX_SUBOBJECTS] = {};
  SetNormalizedTimeAnim(10, nt, pm);

  // In model space: the root's yaw maps its +Z to +X and +X to -Z.
  EXPECT_VEC_NEAR(ModelPoint(pm, 1, vector{0, 0, 0}, nt), 5, 3, 0);
  EXPECT_VEC_NEAR(ModelPoint(pm, 1, vector{1, 0, 0}, nt), 5, 3, -1);

  {
    vector dest{}, wpos{}, p{0, 0, 0}, n{1, 0, 0};
    matrix orient = IDENTITY_MATRIX;
    GetPolyModelPointInWorld(&dest, pm, &wpos, &orient, 1, nt, &p, &n);
    EXPECT_VEC_NEAR(n, 0, 0, -1);
  }

  // Object placement: a matrix's rows are the object's axes in world space, so a model-space point
  // p lands at wpos + p.x*rvec + p.y*uvec + p.z*fvec. The overload without normalized_time uses the
  // rest pose (frame 0, no yaw), where child point (1,0,0) is at (1,3,5) in model space.
  matrix orient;
  vm_AnglesToMatrix(&orient, 0x2000, 0x1234, 0x0800); // arbitrary pitch/heading/bank
  vector wpos{100, -50, 25};
  vector p{1, 0, 0};
  vector dest{};
  GetPolyModelPointInWorld(&dest, pm, &wpos, &orient, 1, &p);
  vector expected = wpos + 1.0f * orient.rvec + 3.0f * orient.uvec + 5.0f * orient.fvec;
  EXPECT_VEC_NEAR(dest, expected.x(), expected.y(), expected.z());
}

TEST_F(PolyModelTest, ChangePolyModelNameStripsDirectoryAndKeepsExtension) {
  EXPECT_EQ(ChangePolyModelName(std::filesystem::path("data") / "models" / "Pyro.oof"), "Pyro.oof");
  EXPECT_EQ(ChangePolyModelName("plain.oof"), "plain.oof");
}

TEST_F(PolyModelTest, ChangePolyModelNameTruncatesToFitModelNameField) {
  const std::string stem(60, 'x');
  std::string name = ChangePolyModelName(std::filesystem::path("dir") / (stem + ".oof")).string();

  EXPECT_LT(name.size(), sizeof(poly_model::name)); // must fit, with its terminator
  ASSERT_GE(name.size(), 4u);
  EXPECT_EQ(name.substr(name.size() - 4), ".oof");
  EXPECT_EQ(name.substr(0, name.size() - 4), stem.substr(0, name.size() - 4));
}

TEST_F(PolyModelTest, LoadRegistersModelUnderFileNameCaseInsensitively) {
  pof::Writer w(kTimedVersion);
  w.header(1);
  w.subobject(0, Triangle(-1));
  std::filesystem::create_directories(models_.path() / "sub");
  int handle = LoadPolyModel(models_.Save(w, "sub/Probe.oof"), 0);
  ASSERT_GE(handle, 0);

  EXPECT_STREQ(Poly_models[handle].name, "Probe.oof");
  EXPECT_EQ(FindPolyModelName("probe.OOF"), handle);
  EXPECT_EQ(FindPolyModelName("other.oof"), -1);
}

TEST_F(PolyModelTest, RepeatedLoadsShareOneSlotAndAreReferenceCounted) {
  pof::Writer w(kTimedVersion);
  w.header(1);
  w.subobject(0, Triangle(-1));
  std::filesystem::path path = models_.Save(w, "shared.oof");

  int first = LoadPolyModel(path, 0);
  ASSERT_GE(first, 0);
  int second = LoadPolyModel(path, 0);
  EXPECT_EQ(second, first);
  EXPECT_EQ(Poly_models[first].used, 2);
  EXPECT_FALSE(Poly_models[first].flags & PMF_NOT_RESIDENT);
  EXPECT_EQ(Poly_models[first].n_models, 1);

  FreePolyModel(first);
  EXPECT_EQ(FindPolyModelName("shared.oof"), first);
  FreePolyModel(first);
  EXPECT_EQ(FindPolyModelName("shared.oof"), -1);
}

TEST_F(PolyModelTest, LoadingMissingFileFailsWithoutRegisteringModel) {
  EXPECT_EQ(LoadPolyModel(models_.path() / "does_not_exist.oof", 0), -1);
  EXPECT_EQ(FindPolyModelName("does_not_exist.oof"), -1);
}

TEST_F(PolyModelTest, PageableLoadDefersReadingUntilFirstUse) {
  pof::Writer w(kTimedVersion);
  w.header(2);
  w.subobject(0, Triangle(-1));
  w.subobject(1, Triangle(0, {4, 0, 0}));
  std::filesystem::path path = models_.Save(w, "paged.oof");
  // Paging in reopens the file by bare name through the cfile search path.
  ASSERT_TRUE(cf_SetSearchPath(models_.path()));

  int handle = LoadPolyModel(path, 1);
  ASSERT_GE(handle, 0);
  EXPECT_TRUE(Poly_models[handle].flags & PMF_NOT_RESIDENT);
  EXPECT_EQ(Poly_models[handle].n_models, 0);

  poly_model *pm = GetPolymodelPointer(handle);
  EXPECT_EQ(pm, &Poly_models[handle]);
  EXPECT_FALSE(pm->flags & PMF_NOT_RESIDENT);
  ASSERT_EQ(pm->n_models, 2);
  EXPECT_EQ(pm->submodel[1].parent, 0);
  EXPECT_VEC_NEAR(pm->maxs, 5, 1, 0);
}

TEST_F(PolyModelTest, VersionsOutsideSupportedRangeAreRejected) {
  max_assertions_ = 2; // the loader Int3()s on each bad version
  {
    pof::Writer w(PM_OBJFILE_VERSION + 100);
    w.header(1);
    w.subobject(0, Triangle(-1));
    EXPECT_EQ(models_.Load(w, "future.oof"), -1);
  }
  {
    pof::Writer w(PM_COMPATIBLE_VERSION - 1);
    w.header(1);
    w.subobject(0, Triangle(-1));
    EXPECT_EQ(models_.Load(w, "ancient.oof"), -1);
  }
}

TEST_F(PolyModelTest, PageableModelWithBadVersionStaysOwnedAfterFailedPageIn) {
  max_assertions_ = 2;
  pof::Writer bad(PM_OBJFILE_VERSION + 100);
  bad.header(1);
  bad.subobject(0, Triangle(-1));
  std::filesystem::path path = models_.Save(bad, "future.oof");
  ASSERT_TRUE(cf_SetSearchPath(models_.path()));

  int handle = LoadPolyModel(path, 1);
  ASSERT_GE(handle, 0);
  GetPolymodelPointer(handle);

  EXPECT_EQ(Poly_models[handle].used, 1);
  EXPECT_EQ(FindPolyModelName("future.oof"), handle);

  pof::Writer good(kTimedVersion);
  good.header(1);
  good.subobject(0, Triangle(-1));
  int other = models_.Load(good, "other.oof");
  ASSERT_GE(other, 0);
  EXPECT_NE(other, handle);
}

} // namespace
