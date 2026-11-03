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

#include "sim_object.h"

#include "physics.h"

namespace physics_test {

void InitSimObject(object &obj, float mass, float drag, const vector &pos, const vector &vel) {
  obj = object{};
  obj.type = OBJ_ROBOT;
  obj.movement_type = MT_PHYSICS;
  // A terrain cell rather than a room, so no room wind is involved.
  obj.roomnum = MAKE_ROOMNUM(0);
  vm_MakeIdentity(&obj.orient);
  obj.pos = pos;
  obj.mtype.phys_info.mass = mass;
  obj.mtype.phys_info.drag = drag;
  obj.mtype.phys_info.velocity = vel;
}

void StepLinear(object &obj, const vector &force, float dt) {
  vector vel = obj.mtype.phys_info.velocity;
  vector movement_vec, movement_pos;
  PhysicsDoSimLinear(obj, obj.pos, force, vel, movement_vec, movement_pos, dt, 0);
  obj.pos = movement_pos;
  obj.mtype.phys_info.velocity = vel;
}

void SimulateLinear(object &obj, const vector &force, float total_time, int frames) {
  for (int i = 0; i < frames; i++)
    StepLinear(obj, force, total_time / frames);
}

} // namespace physics_test
