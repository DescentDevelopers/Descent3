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

--- HISTORICAL COMMENTS FOLLOW ---

 * $Logfile: /DescentIII/main/player_external_struct.h $
 * $Revision: 13 $
 * $Date: 5/20/99 2:48a $
 * $Author: Matt $
 *
 * Contains the structs needed for player definition (for DLL export)
 *
 * $Log: /DescentIII/main/player_external_struct.h $
 *
 * 13    5/20/99 2:48a Matt
 * Auto-waypoints now stored & used per player.  Manual waypoints will all
 * players, once Jason makes it work.
 *
 * 12    5/10/99 12:23a Chris
 * Fixed another hearing/seeing case.  :)  Buddy bot now is in the player
 * ship at respawn
 *
 * 11    5/03/99 2:36p Jason
 * added start index
 *
 * 10    4/23/99 1:56a Matt
 * Added system for counting kills of friendly objects
 *
 * 9     4/06/99 6:02p Matt
 * Added score system
 *
 * 8     4/05/99 3:34p Matt
 * Cleaned up player start flags
 *
 * 7     3/23/99 12:33p Kevin
 * More post level results improvements
 *
 * 6     2/16/99 3:48p Jason
 * added marker updates to multiplayer server stream
 *
 * 5     2/06/99 10:03p Matt
 * Added keys system
 *
 * 4     2/02/99 7:06p Jason
 * added ranking system
 *
 * 3     1/28/99 6:17p Jason
 * added markers
 *
 * 2     1/21/99 11:15p Jeff
 * pulled out some structs and defines from header files and moved them
 * into seperate header files so that multiplayer dlls don't require major
 * game headers, just those new headers.  Side effect is a shorter build
 * time.  Also cleaned up some header file #includes that weren't needed.
 * This affected polymodel.h, object.h, player.h, vecmat.h, room.h,
 * manage.h and multi.h
 *
 * $NoKeywords: $
 */

#ifndef _PLAYER_EXTERNAL_STRUCT_H_
#define _PLAYER_EXTERNAL_STRUCT_H_

#include <cstdint>

#include "pstypes.h"
#include "vecmat_external.h"
#include "object_external_struct.h"
#include "multi_external.h"
#include "Inventory.h"

#define MAX_PLAYERS MAX_NET_PLAYERS
#define MAX_TEAMS 4

#define CALLSIGN_LEN 19
//	THIS CONSTANT MUST NEVER BE OVER 32!!!
#define MAX_PLAYER_WEAPONS MAX_WBS_PER_OBJ

#define TRACKER_ID_LEN 10 // if this is changed, make sure it's changed in pilottrack.h

// Info on player weapon firing.
// There is one of these each for the primary & secondary weapons
struct player_weapon {
  int32_t index;         // the index of the current primary or secondary weapon
  float firing_time; // how long the current weapon has been firing
  int32_t sound_handle;  // the handle for the sound the firing weapon is making
};

// The structure for a player.  Most of this data will be for multiplayer
struct player {

  // positional data for player starts
  int32_t start_index;
  vector start_pos; // where the player starts
  int32_t start_roomnum;
  matrix start_orient;

  int32_t startpos_flags; // these flags apply to the start position (used for teams)

  int32_t ship_index; // the index into the Ships array that this player is flying

  // Who am I data
  char callsign[CALLSIGN_LEN + 1]; // The callsign of this player, for net purposes.

  // Game data
  uint32_t flags;                           // Powerup flags, see above...
  int32_t score;                            // The player's current score
  float damage_magnitude;               // for shield effects
  float edrain_magnitude;               // for energy drain effects
  float invul_magnitude;                // for invulnerability effects
  float energy;                         // Amount of energy remaining.
  uint8_t lives;                          // Lives remaining, 0 = game over.
  int8_t level;                          // Current level player is playing. (must be signed for secret levels)
  int8_t starting_level;                 // What level the player started on.
  uint8_t keys;                           // Which keys the player has
  int16_t killer_objnum;                  // Who killed me.... (-1 if no one)
  float invulnerable_time;              // Time left invulnerable
  float last_hit_wall_sound_time;       // Last time we played a hit wall sound
  float last_homing_warning_sound_time; // Obvious  :)
  float last_thrust_time;               // Last time the player thrusted
  float last_afterburner_time;          // Last time the player used the afterburner
  int16_t objnum;                         // The object number of this player
  int8_t team;                           // The team number this guy is on

  // The current auto-waypoint for this player
  int32_t current_auto_waypoint_room; // the most recent auto-waypoint, or -1

  // Statistics...
  //	int		score;								// Current score.
  float time_level; // Level time played
  float time_total; // Game time played (high word = seconds)

  int32_t num_hits_level; // used for accuracy calculation.
  int32_t num_discharges_level;

  int16_t num_kills_level;      // Number of kills this level
  int16_t friendly_kills_level; // Number of friendly kills this level
  int16_t num_kills_total;      // Number of kills total

  // Player weapon info
  uint32_t weapon_flags;                      // Mask of currently owned weapons
  uint16_t weapon_ammo[MAX_PLAYER_WEAPONS]; // Ammo for each weapon

  // Weapons
  player_weapon weapon[2]; // Info on the player weapons
  uint8_t laser_level;       // Current level of the laser.

  // lighting
  float light_dist;
  float r, g, b;

  float ballspeed;
  uint8_t num_balls;
  float ball_r[3], ball_g[3], ball_b[3];

  // Room tracking
  int32_t oldroom;

  // Inventory
  Inventory inventory;

  // CounterMeasures Inventory
  Inventory counter_measures;

  // Last time the player fired a weapon
  float last_fire_weapon_time;

  // Afterburner stuff
  float afterburner_mag; // How big the thrust is for the afterburner
  float thrust_mag;
  int32_t afterburner_sound_handle;
  float afterburn_time_left;

  int32_t thruster_sound_handle;
  int32_t thruster_sound_state;

  // For small views and external cameras
  int32_t small_left_obj, small_right_obj, small_dll_obj;

  // Multiplayer stuff
  uint8_t multiplayer_flags;
  uint8_t last_multiplayer_flags;
  float last_guided_time;

  char tracker_id[TRACKER_ID_LEN];
  int32_t kills;
  int32_t deaths;
  int32_t suicides;
  float rank;
  float lateral_thrust;      // total lateral movement over the whole game
  float rotational_thrust;   // total rotational movement over the whole game
  uint32_t time_in_game; // seconds in game
  object *guided_obj, *user_timeout_obj;

  float zoom_distance;

  // Scalar values
  float movement_scalar;
  float damage_scalar;
  float armor_scalar;
  float turn_scalar;
  float weapon_recharge_scalar;
  float weapon_speed_scalar;

  // Observer stuff
  int32_t piggy_objnum;
  int32_t piggy_sig;

  // Custom texture stuff
  int32_t custom_texture_handle;

  // Ship permissions (1 bit per ship)
  int32_t ship_permissions;

  // For invul hit effect
  vector invul_vector;

  // Used to disable various controller input from scripting
  uint32_t controller_bitflags;

  // Marker stuff
  int16_t num_markers;

  int16_t num_deaths_level; // Number of kills this level
  int16_t num_deaths_total; // Number of kills total

};

#endif
