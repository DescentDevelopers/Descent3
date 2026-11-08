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

// Linear motion obeys m dv/dt = F - drag * v (F constant over a step).

#include <cmath>
#include <memory>

#include <gtest/gtest.h>

#include "object.h"
#include "physics.h"
#include "vecmat.h"

#include "support/sim_object.h"
#include "support/vector_helpers.h"

namespace {

using physics_test::InitSimObject;
using physics_test::SimulateLinear;
using physics_test::StepLinear;
using physics_test::V;

class PhysicsSim : public testing::Test {
protected:
  // object is large; keep it off the stack.
  std::unique_ptr<object> obj_holder = std::make_unique<object>();
  object &obj = *obj_holder;
};

TEST_F(PhysicsSim, ResultDoesNotDependOnFrameRate) {
  const vector p0 = V(1, 2, 3), v0 = V(10, -3, 4), force = V(1, -20, 3);
  const float mass = 2.0f, drag = 0.5f, total = 2.0f;

  InitSimObject(obj, mass, drag, p0, v0);
  SimulateLinear(obj, force, total, 1);
  const vector pos1 = obj.pos, vel1 = obj.mtype.phys_info.velocity;

  for (int frames : {7, 60}) {
    SCOPED_TRACE(testing::Message() << frames << " frames");
    InitSimObject(obj, mass, drag, p0, v0);
    SimulateLinear(obj, force, total, frames);
    EXPECT_VEC_NEAR(pos1, obj.pos, 1e-3f);
    EXPECT_VEC_NEAR(vel1, obj.mtype.phys_info.velocity, 1e-3f);
  }
}

TEST_F(PhysicsSim, InstantaneousAccelerationIsNetForceOverMass) {
  const vector v0 = V(2, 0, 0), force = V(0, -40, 4);
  const float mass = 4.0f, drag = 0.5f, dt = 1e-3f;
  InitSimObject(obj, mass, drag, V(0, 0, 0), v0);
  StepLinear(obj, force, dt);

  const vector expected_accel = (force - v0 * drag) / mass; // (-0.25, -10, 1)
  const vector accel = (obj.mtype.phys_info.velocity - v0) / dt;
  EXPECT_VEC_NEAR(expected_accel, accel, 0.02f);
}

TEST_F(PhysicsSim, ConstantForceApproachesTerminalVelocity) {
  // Terminal velocity is where drag balances the force: v = F / drag.
  const vector force = V(3, -10, 0);
  const float drag = 0.5f;
  for (float mass : {1.0f, 2.5f}) {
    for (const vector &v0 : {V(0, 0, 0), V(-30, 50, 7)}) {
      SCOPED_TRACE(testing::Message() << "mass " << mass << " v0.x " << v0.x());
      InitSimObject(obj, mass, drag, V(0, 0, 0), v0);
      SimulateLinear(obj, force, 120.0f, 4);
      EXPECT_VEC_NEAR(force / drag, obj.mtype.phys_info.velocity, 1e-2f);
    }
  }
}

TEST_F(PhysicsSim, CoastingObjectStopsAfterMassOverDragTimesInitialSpeed) {
  // With no force, v(t) = v0 exp(-drag t / m), so the total distance travelled is
  // v0 * m / drag.
  const vector p0 = V(5, 5, 5), v0 = V(6, 0, -3);
  const float mass = 2.0f, drag = 0.5f;
  InitSimObject(obj, mass, drag, p0, v0);
  SimulateLinear(obj, V(0, 0, 0), 200.0f, 10);
  EXPECT_VEC_NEAR(p0 + v0 * (mass / drag), obj.pos, 1e-2f);
  EXPECT_VEC_NEAR(V(0, 0, 0), obj.mtype.phys_info.velocity, 1e-3f);
}

TEST_F(PhysicsSim, FixedVelocityObjectIgnoresForces) {
  const vector p0 = V(0, 1, 0), v0 = V(4, 0, -2);
  InitSimObject(obj, 1.0f, 0.5f, p0, v0);
  obj.mtype.phys_info.flags |= PF_FIXED_VELOCITY;
  SimulateLinear(obj, V(100, 100, 100), 3.0f, 3);
  EXPECT_VEC_NEAR(p0 + v0 * 3.0f, obj.pos, 1e-4f);
  EXPECT_VEC_NEAR(v0, obj.mtype.phys_info.velocity, 1e-6f);
}

TEST_F(PhysicsSim, ApplyForceChangesVelocityByForceOverMass) {
  InitSimObject(obj, 4.0f, 0.5f, V(0, 0, 0), V(1, 1, 1));
  obj.id = 1; // not the local player (Player_num), so no force feedback
  vector impulse = V(8, -4, 0);
  phys_apply_force(&obj, &impulse);
  EXPECT_VEC_NEAR(V(3, 0, 1), obj.mtype.phys_info.velocity, 1e-5f);
}

// Angles are 16-bit fixed point (65536 per revolution); rotvel is in those units per second.

TEST_F(PhysicsSim, NegativeTurnRateRotatesTheOppositeWay) {
  // A float->unsigned conversion of a negative turn amount once collapsed it to zero on
  // some platforms.
  InitMathTables(); // FixSin/FixCos lookup tables (idempotent; the game does this at startup)

  const float eighth_turn = 65536.0f / 8.0f;
  vector fvec[2];
  for (int i = 0; i < 2; i++) {
    InitSimObject(obj, 1.0f, 0.0f, V(0, 0, 0), V(0, 0, 0));
    matrix orient = obj.orient;
    vector rotthrust = V(0, 0, 0);
    vector rotvel = V(0, (i == 0 ? 1.0f : -1.0f) * eighth_turn, 0);
    angle turnroll = 0;
    ASSERT_TRUE(PhysicsDoSimRot(&obj, 1.0f, &orient, &rotthrust, &rotvel, &turnroll));
    fvec[i] = orient.fvec;
  }
  const float c = std::cos(3.14159265f / 4.0f);
  for (const vector &f : fvec) {
    EXPECT_NEAR(0.0f, f.y(), 1e-3f); // heading only: stays level
    EXPECT_NEAR(c, f.z(), 1e-3f);    // 45 degrees away from the initial forward (+z)
    EXPECT_NEAR(c, std::fabs(f.x()), 1e-3f);
  }
  EXPECT_NEAR(fvec[0].x(), -fvec[1].x(), 1e-3f);
}

} // namespace
