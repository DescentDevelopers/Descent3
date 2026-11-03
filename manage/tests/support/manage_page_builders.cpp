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

#include "manage_page_builders.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>

#include <gtest/gtest.h>

#include "mem.h"

namespace manage_page_builders {

void SetStr(char *dst, size_t size, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vsnprintf(dst, size, fmt, args);
  va_end(args);
}

void FillPhysics(physics_info &p, FloatSeq &f) {
  p.mass = f();
  p.drag = f();
  p.full_thrust = f();
  p.flags = 0x00012345;
  p.rotdrag = f();
  p.full_rotthrust = f();
  p.num_bounces = 7;
  p.velocity.z() = f(); // only the z component is stored (initial forward speed)
  p.rotvel = {f(), f(), f()};
  p.wiggle_amplitude = f();
  p.wiggles_per_sec = f();
  p.coeff_restitution = f();
  p.hit_die_dot = f();
  p.max_turnroll_rate = f();
  p.turnroll_ratio = f();
}

void ExpectPhysicsEq(const physics_info &a, const physics_info &b) {
  EXPECT_EQ(a.mass, b.mass);
  EXPECT_EQ(a.drag, b.drag);
  EXPECT_EQ(a.full_thrust, b.full_thrust);
  EXPECT_EQ(a.flags, b.flags);
  EXPECT_EQ(a.rotdrag, b.rotdrag);
  EXPECT_EQ(a.full_rotthrust, b.full_rotthrust);
  EXPECT_EQ(a.num_bounces, b.num_bounces);
  EXPECT_EQ(a.velocity.z(), b.velocity.z());
  EXPECT_EQ(a.rotvel.x(), b.rotvel.x());
  EXPECT_EQ(a.rotvel.y(), b.rotvel.y());
  EXPECT_EQ(a.rotvel.z(), b.rotvel.z());
  EXPECT_EQ(a.wiggle_amplitude, b.wiggle_amplitude);
  EXPECT_EQ(a.wiggles_per_sec, b.wiggles_per_sec);
  EXPECT_EQ(a.coeff_restitution, b.coeff_restitution);
  EXPECT_EQ(a.hit_die_dot, b.hit_die_dot);
  EXPECT_EQ(a.max_turnroll_rate, b.max_turnroll_rate);
  EXPECT_EQ(a.turnroll_ratio, b.turnroll_ratio);
}

void FillLighting(light_info &l, FloatSeq &f) {
  l.light_distance = f();
  l.red_light1 = f();
  l.green_light1 = f();
  l.blue_light1 = f();
  l.time_interval = f();
  l.flicker_distance = f();
  l.directional_dot = f();
  l.red_light2 = f();
  l.green_light2 = f();
  l.blue_light2 = f();
  l.flags = 0x0000abcd;
  l.timebits = 0x55aa55aa;
  l.angle = 200;
  l.lighting_render_type = 3;
}

void ExpectLightingEq(const light_info &a, const light_info &b) {
  EXPECT_EQ(a.light_distance, b.light_distance);
  EXPECT_EQ(a.red_light1, b.red_light1);
  EXPECT_EQ(a.green_light1, b.green_light1);
  EXPECT_EQ(a.blue_light1, b.blue_light1);
  EXPECT_EQ(a.time_interval, b.time_interval);
  EXPECT_EQ(a.flicker_distance, b.flicker_distance);
  EXPECT_EQ(a.directional_dot, b.directional_dot);
  EXPECT_EQ(a.red_light2, b.red_light2);
  EXPECT_EQ(a.green_light2, b.green_light2);
  EXPECT_EQ(a.blue_light2, b.blue_light2);
  EXPECT_EQ(a.flags, b.flags);
  EXPECT_EQ(a.timebits, b.timebits);
  EXPECT_EQ(a.angle, b.angle);
  EXPECT_EQ(a.lighting_render_type, b.lighting_render_type);
}

void FillWeaponBattery(otype_wb_info &wb, int seed, FloatSeq &f) {
  wb.energy_usage = f();
  wb.ammo_usage = f();
  for (int j = 0; j < MAX_WB_GUNPOINTS; j++)
    wb.gp_weapon_index[j] = static_cast<uint16_t>(1000 + seed * 10 + j);
  for (int j = 0; j < MAX_WB_FIRING_MASKS; j++) {
    wb.gp_fire_masks[j] = static_cast<uint8_t>(j + 1);
    wb.gp_fire_wait[j] = f();
    wb.anim_time[j] = f();
    wb.anim_start_frame[j] = f();
    wb.anim_fire_frame[j] = f();
    wb.anim_end_frame[j] = f();
  }
  wb.num_masks = 5;
  wb.aiming_gp_index = static_cast<uint16_t>(3 + seed);
  wb.aiming_flags = 0x21;
  wb.aiming_3d_dot = f();
  wb.aiming_3d_dist = f();
  wb.aiming_XZ_dot = f();
  wb.flags = 0x1234; // wider than a byte: current pages store flags as 16 bits
  wb.gp_quad_fire_mask = 0x0f;
}

void ExpectWeaponBatteryEq(const otype_wb_info &a, const otype_wb_info &b) {
  EXPECT_EQ(a.energy_usage, b.energy_usage);
  EXPECT_EQ(a.ammo_usage, b.ammo_usage);
  for (int j = 0; j < MAX_WB_GUNPOINTS; j++)
    EXPECT_EQ(a.gp_weapon_index[j], b.gp_weapon_index[j]) << "gunpoint " << j;
  for (int j = 0; j < MAX_WB_FIRING_MASKS; j++) {
    EXPECT_EQ(a.gp_fire_masks[j], b.gp_fire_masks[j]) << "mask " << j;
    EXPECT_EQ(a.gp_fire_wait[j], b.gp_fire_wait[j]) << "mask " << j;
    EXPECT_EQ(a.anim_time[j], b.anim_time[j]) << "mask " << j;
    EXPECT_EQ(a.anim_start_frame[j], b.anim_start_frame[j]) << "mask " << j;
    EXPECT_EQ(a.anim_fire_frame[j], b.anim_fire_frame[j]) << "mask " << j;
    EXPECT_EQ(a.anim_end_frame[j], b.anim_end_frame[j]) << "mask " << j;
  }
  EXPECT_EQ(a.num_masks, b.num_masks);
  EXPECT_EQ(a.aiming_gp_index, b.aiming_gp_index);
  EXPECT_EQ(a.aiming_flags, b.aiming_flags);
  EXPECT_EQ(a.aiming_3d_dot, b.aiming_3d_dot);
  EXPECT_EQ(a.aiming_3d_dist, b.aiming_3d_dist);
  EXPECT_EQ(a.aiming_XZ_dot, b.aiming_XZ_dot);
  EXPECT_EQ(a.flags, b.flags);
  EXPECT_EQ(a.gp_quad_fire_mask, b.gp_quad_fire_mask);
}

std::unique_ptr<mngs_sound_page> MakeSoundPage() {
  auto p = std::make_unique<mngs_sound_page>();
  SET_STR(p->sound_struct.name, "Explosion Big");
  SET_STR(p->raw_name, "explode_big.wav");
  p->sound_struct.flags = 0x80000005u;
  p->sound_struct.loop_start = 1024;
  p->sound_struct.loop_end = 65536;
  p->sound_struct.outer_cone_volume = 0.25f;
  p->sound_struct.inner_cone_angle = 30;
  p->sound_struct.outer_cone_angle = 270;
  p->sound_struct.max_distance = 400.5f;
  p->sound_struct.min_distance = 12.75f;
  p->sound_struct.import_volume = 0.8125f;
  return p;
}

std::unique_ptr<mngs_door_page> MakeDoorPage() {
  auto p = std::make_unique<mngs_door_page>();
  SET_STR(p->door_struct.name, "Hangar Door");
  SET_STR(p->image_name, "hangardoor.oof");
  p->door_struct.total_open_time = 1.5f;
  p->door_struct.total_close_time = 2.25f;
  p->door_struct.total_time_open = 4.0f;
  p->door_struct.flags = 0x05;
  p->door_struct.hit_points = 1234;
  SET_STR(p->open_sound_name, "DoorOpen");
  SET_STR(p->close_sound_name, "DoorClose");
  SET_STR(p->door_struct.module_name, "hangar_script");
  return p;
}

std::unique_ptr<mngs_gamefile_page> MakeGamefilePage() {
  auto p = std::make_unique<mngs_gamefile_page>();
  SET_STR(p->gamefile_struct.name, "level1.d3l");
  SET_STR(p->gamefile_struct.dir_name, "levels");
  return p;
}

std::unique_ptr<mngs_megacell_page> MakeMegacellPage() {
  auto p = std::make_unique<mngs_megacell_page>();
  SET_STR(p->megacell_struct.name, "Rock Wall");
  for (int i = 0; i < MAX_MEGACELL_WIDTH * MAX_MEGACELL_HEIGHT; i++)
    SET_STR(p->cellname[i], "rockwall_%02d", i);
  p->megacell_struct.width = 6;
  p->megacell_struct.height = 3;
  return p;
}

std::unique_ptr<mngs_texture_page> MakeTexturePage(bool procedural) {
  auto p = std::make_unique<mngs_texture_page>();
  SET_STR(p->tex_struct.name, "Lava Flow");
  SET_STR(p->bitmap_name, "lava.ogf");
  SET_STR(p->destroy_name, "lava_broken.ogf");
  SET_STR(p->sound_name, "LavaBubble");
  p->tex_struct.r = 0.5f;
  p->tex_struct.g = 0.25f;
  p->tex_struct.b = 0.125f;
  p->tex_struct.alpha = 0.75f;
  p->tex_struct.speed = 2.5f;
  p->tex_struct.slide_u = 0.375f;
  p->tex_struct.slide_v = -0.625f;
  p->tex_struct.reflectivity = 0.3125f;
  p->tex_struct.corona_type = 2;
  p->tex_struct.damage = 15;
  p->tex_struct.flags = 0x00000011;
  p->tex_struct.sound_volume = 0.6875f;
  if (procedural) {
    p->tex_struct.flags |= TF_PROCEDURAL;
    for (int i = 0; i < 255; i++)
      p->proc_palette[i] = static_cast<uint16_t>(i * 257);
    p->proc_heat = 190;
    p->proc_light = 3;
    p->proc_thickness = 6;
    p->proc_evaluation_time = 0.0625f;
    p->osc_time = 1.75f;
    p->osc_value = 12;
    p->num_proc_elements = 3;
    for (int i = 0; i < p->num_proc_elements; i++) {
      p->proc_type[i] = static_cast<uint8_t>(1 + i);
      p->proc_frequency[i] = static_cast<uint8_t>(10 + i);
      p->proc_speed[i] = static_cast<uint8_t>(20 + i);
      p->proc_size[i] = static_cast<uint8_t>(30 + i);
      p->proc_x1[i] = static_cast<uint8_t>(40 + i);
      p->proc_y1[i] = static_cast<uint8_t>(50 + i);
      p->proc_x2[i] = static_cast<uint8_t>(60 + i);
      p->proc_y2[i] = static_cast<uint8_t>(70 + i);
    }
  }
  return p;
}

std::unique_ptr<mngs_weapon_page> MakeWeaponPage() {
  auto p = std::make_unique<mngs_weapon_page>();
  FloatSeq f(100.0f);
  weapon &w = p->weapon_struct;
  SET_STR(w.name, "Plasma Bolt");
  SET_STR(p->hud_image_name, "plasma_hud.ogf");
  SET_STR(p->fire_image_name, "plasma_fire.oof");
  SET_STR(p->particle_name, "plasma_particle.ogf");
  w.particle_count = 9;
  w.particle_life = f();
  w.particle_size = f();
  w.flags = 0x00402011;
  SET_STR(p->spawn_name, "Plasma Spark");
  w.spawn_count = 4;
  SET_STR(p->robot_spawn_name, "Tiny Bot");
  SET_STR(p->alternate_spawn_name, "Plasma Alt");
  w.alternate_chance = 35;
  w.gravity_time = f();
  w.gravity_size = f();
  w.homing_fov = f();
  w.custom_size = f();
  w.size = f();
  w.thrust_time = f();
  FillPhysics(w.phys_info, f);
  w.terrain_damage_size = f();
  w.terrain_damage_depth = 17;
  w.alpha = f();
  SET_STR(p->explode_image_name, "plasma_explode.oaf");
  w.explode_time = f();
  w.explode_size = f();
  w.player_damage = f();
  w.generic_damage = f();
  w.impact_size = f();
  w.impact_time = f();
  w.impact_player_damage = f();
  w.impact_generic_damage = f();
  w.impact_force = f();
  w.life_time = f();
  FillLighting(w.lighting_info, f);
  w.recoil_force = f();
  for (int i = 0; i < MAX_WEAPON_SOUNDS; i++)
    SET_STR(p->sound_name[i], "PlasmaSound%d", i);
  SET_STR(p->smoke_image_name, "plasma_smoke.ogf");
  SET_STR(p->scorch_image_name, "plasma_scorch.ogf");
  w.scorch_size = f();
  SET_STR(p->icon_name, "plasma_icon.ogf");
  return p;
}

std::unique_ptr<mngs_ship_page> MakeShipPage() {
  auto p = std::make_unique<mngs_ship_page>();
  FloatSeq f(200.0f);
  ship &s = p->ship_struct;
  SET_STR(s.name, "Test Ship");
  SET_STR(s.cockpit_name, "testcockpit.inf");
  SET_STR(s.hud_config_name, "testhud.inf");
  SET_STR(p->image_name, "testship.oof");
  SET_STR(p->dying_image_name, "testship_dying.oof");
  SET_STR(p->med_image_name, "testship_med.oof");
  SET_STR(p->lo_image_name, "testship_lo.oof");
  s.med_lod_distance = f();
  s.lo_lod_distance = f();
  FillPhysics(s.phys_info, f);
  s.size = f();
  s.armor_scalar = f();
  s.flags = 0x0000beef;
  for (int i = 0; i < MAX_PLAYER_WEAPONS; i++) {
    s.fire_flags[i] = static_cast<uint8_t>(i + 1);
    SET_STR(p->firing_sound_name[i], "FireLoop%d", i);
    SET_STR(p->release_sound_name[i], "Release%d", i);
    SET_STR(p->spew_powerup_name[i], "Spew%d", i);
    s.max_ammo[i] = 100 * (i + 1);
    FillWeaponBattery(s.static_wb[i], i, f);
    for (int j = 0; j < MAX_WB_GUNPOINTS; j++)
      SET_STR(p->weapon_name[i][j], "Weapon%d_%d", i, j);
    for (int j = 0; j < MAX_WB_FIRING_MASKS; j++)
      SET_STR(p->fire_sound_name[i][j], "FireSnd%d_%d", i, j);
  }
  return p;
}

std::unique_ptr<mngs_generic_page> MakeGenericPage(int type, const char *description) {
  auto p = std::make_unique<mngs_generic_page>();
  FloatSeq f(300.0f);
  object_info &o = p->objinfo_struct;
  t_ai_info &ai = p->ai_info;

  o.type = type;
  SET_STR(o.name, "Generic Thing");
  SET_STR(p->image_name, "thing.oof");
  SET_STR(p->med_image_name, "thing_med.oof");
  SET_STR(p->lo_image_name, "thing_lo.oof");
  o.impact_size = f();
  o.impact_time = f();
  o.damage = f();
  o.score = 1500;
  o.ammo_count = (type == OBJ_POWERUP) ? 250 : 0;
  SET_STR(o.module_name, "thing_module");
  SET_STR(o.script_name_override, "thing_override");
  if (description) {
    o.description = mem_rmalloc<char>(strlen(description) + 1);
    strcpy(o.description, description);
  }
  SET_STR(o.icon_name, "thing_icon");
  o.med_lod_distance = f();
  o.lo_lod_distance = f();
  FillPhysics(o.phys_info, f);
  o.size = f();
  FillLighting(o.lighting_info, f);
  o.hit_points = 321;
  o.flags = 0x00100401;

  ai.flags = 0x00010203;
  ai.ai_class = 2;
  ai.ai_type = 5;
  ai.movement_type = 1;
  ai.movement_subtype = 3;
  ai.fov = f();
  ai.max_velocity = f();
  ai.max_delta_velocity = f();
  ai.max_turn_rate = f();
  ai.notify_flags = 0x00000300;
  ai.max_delta_turn_rate = f();
  ai.circle_distance = f();
  ai.attack_vel_percent = f();
  ai.dodge_percent = f();
  ai.dodge_vel_percent = f();
  ai.flee_vel_percent = f();
  ai.melee_damage[0] = f();
  ai.melee_damage[1] = f();
  ai.melee_latency[0] = f();
  ai.melee_latency[1] = f();
  ai.curiousity = f();
  ai.night_vision = f();
  ai.fog_vision = f();
  ai.lead_accuracy = f();
  ai.lead_varience = f();
  ai.fire_spread = f();
  ai.fight_team = f();
  ai.fight_same = f();
  ai.aggression = f();
  ai.hearing = f();
  ai.frustration = f();
  ai.roaming = f();
  ai.life_preservation = f();
  ai.avoid_friends_distance = f();
  ai.biased_flight_importance = f();
  ai.biased_flight_min = f();
  ai.biased_flight_max = f();

  o.f_dspew = 3;
  for (int i = 0; i < MAX_DSPEW_TYPES; i++) {
    o.dspew_percent[i] = f();
    o.dspew_number[i] = static_cast<int16_t>(2 + i);
    SET_STR(p->dspew_name[i], "Spewed%d", i);
  }

  for (int i = 0; i < NUM_MOVEMENT_CLASSES; i++) {
    for (int j = 0; j < NUM_ANIMS_PER_CLASS; j++) {
      // Frame numbers above 255 need the 16-bit encoding used by current pages.
      p->anim[i].elem[j].from = static_cast<int16_t>(300 + i * 100 + j);
      p->anim[i].elem[j].to = static_cast<int16_t>(310 + i * 100 + j);
      p->anim[i].elem[j].spc = f();
      SET_STR(p->anim_sound_name[i][j], "Anim%d_%d", i, j);
    }
  }

  for (int i = 0; i < MAX_WBS_PER_OBJ; i++) {
    FillWeaponBattery(p->static_wb[i], i, f);
    for (int j = 0; j < MAX_WB_GUNPOINTS; j++)
      SET_STR(p->weapon_name[i][j], "Gun%d_%d", i, j);
    for (int j = 0; j < MAX_WB_FIRING_MASKS; j++)
      SET_STR(p->fire_sound_name[i][j], "Shot%d_%d", i, j);
  }

  for (int i = 0; i < MAX_OBJ_SOUNDS; i++)
    SET_STR(p->sound_name[i], "ObjSound%d", i);
  for (int i = 0; i < MAX_AI_SOUNDS; i++)
    SET_STR(p->ai_sound_name[i], "AISound%d", i);

  o.respawn_scalar = f();
  for (int i = 0; i < MAX_DEATH_TYPES; i++) {
    o.death_types[i].flags = 0x00000100u * (i + 1);
    o.death_types[i].delay_min = f();
    o.death_types[i].delay_max = f();
    o.death_probabilities[i] = static_cast<uint8_t>(10 * (i + 1));
  }
  return p;
}

void FreeGenericPage(mngs_generic_page *p) {
  if (p->objinfo_struct.description) {
    mem_free(p->objinfo_struct.description);
    p->objinfo_struct.description = nullptr;
  }
}

GenericPagePtr Own(std::unique_ptr<mngs_generic_page> p) { return GenericPagePtr(p.release()); }

} // namespace manage_page_builders
