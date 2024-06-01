/*
* Descent 3 
* Copyright (C) 2024 Parallax Software
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

#ifndef ROBOTFIRESTRUCT_H_
#define ROBOTFIRESTRUCT_H_

#include "pstypes.h"
#include "vecmat.h"
#include "robotfirestruct_external.h"

// NOTE: CHANGE gunbattery.h (IN POFGEN PROJECT) if constants are changed
// NOTE: Robots are limited to the number of wb configurations of the player.  This seem like an
//       adequit number (currently 21)
// (MAX_PRIMARY_WEAPONS + MAX_SECONDARY_WEAPONS + 1/*Flare*/)
// NOTE: Cannot include weapon.h because of circular dependances.
#define MAX_WBS_PER_OBJ 21

// Attach to the polymodel
struct poly_wb_info {
  // Static Data  (Add to robot generic page)
  uint16_t num_gps;
  uint8_t gp_index[MAX_WB_GUNPOINTS];

  // Turrets are listed from most important (greatest mobility) to least important
  uint8_t num_turrets;
  uint16_t turret_index[MAX_WB_TURRETS];

};

// Next free WBF is 32

// Attach to a object type
struct otype_wb_info {
  uint16_t gp_weapon_index[MAX_WB_GUNPOINTS];
  uint16_t fm_fire_sound_index[MAX_WB_FIRING_MASKS];
  uint16_t aiming_gp_index;

  uint8_t num_masks;
  uint8_t gp_fire_masks[MAX_WB_FIRING_MASKS];
  float gp_fire_wait[MAX_WB_FIRING_MASKS];

  uint8_t gp_quad_fire_mask;

  uint8_t num_levels;
  uint16_t gp_level_weapon_index[MAX_WB_UPGRADES];
  uint16_t gp_level_fire_sound_index[MAX_WB_UPGRADES];

  uint8_t aiming_flags;
  float aiming_3d_dot; // These can be reused.
  float aiming_3d_dist;
  float aiming_XZ_dot;

  float anim_start_frame[MAX_WB_FIRING_MASKS];
  float anim_fire_frame[MAX_WB_FIRING_MASKS];
  float anim_end_frame[MAX_WB_FIRING_MASKS];
  float anim_time[MAX_WB_FIRING_MASKS];

  uint16_t flags;

  float energy_usage, ammo_usage;
};

#define WB_MOVE_STILL 0
#define WB_MOVE_RIGHT 1
#define WB_MOVE_LEFT 2

// Goes with an individual robot's instance
struct dynamic_wb_info {
  // Dynamic Data
  float last_fire_time;
  uint8_t cur_firing_mask;

  float norm_turret_angle[MAX_WB_TURRETS];
  float turret_next_think_time[MAX_WB_TURRETS];
  uint8_t turret_direction[MAX_WB_TURRETS];

  uint8_t wb_anim_mask;
  float wb_anim_frame;

  vector cur_target;

  char upgrade_level; // For multi-level weapons ( 0 to MAX_WB_UPGRADES-1)

  int flags;
};

#endif
