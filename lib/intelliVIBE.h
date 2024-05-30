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

#ifndef __INTELLIVIBE_H_
#define __INTELLIVIBE_H_

#include <windows.h> //needed for HWND and HINSTANCE

struct fvector {
  float x, y, z;
};

struct d3_init_info {
  HWND hwnd;       // handle to the Window associated with Descent 3
  HINSTANCE hinst; // instance of the Descent 3 application
};

struct d3_frame_info {
  float frametime; // time, in seconds, that the last frame of the game took
  float gametime;  // time, in seconds, that we have been actively playing the current level
  int game_paused; // 1 if the game is currently paused, 0 if it isn't.  Note: if the game is paused, Gametime and
                   // Frametime will be invalid
};

struct d3_fire_info {
  int weapon_index; // what kind of weapon the player is firing, see weapon defines)
};

struct d3_controls_info {
  // Values for thrust are from -1.0 to 1.0)
  float pitch_thrust;
  float heading_thrust;
  float bank_thrust;
  float vertical_thrust;
  float sideways_thrust;
  float forward_thrust;
  float afterburn_thrust;

  fvector current_velocity; // current velocity of the ship
  float shake_magnitude;    // If the player's ship is shaking due to some external force, it will be in this value,
                            // 0<=magnitude<=120

};

struct d3_force_info {
  fvector force_vector; // direction and magnitude of the instantaneous force
};

struct d3_damage_info {
  float damage_amount; // how much damage is being done
};

struct d3_intellivibe {
  d3_frame_info frame_info;
  d3_fire_info fire_info;
  d3_controls_info controls_info;
  d3_force_info force_info;
  d3_damage_info damage_info;
  int flags;
};

////////////////////////////////////////////////////////////
// Flag Defines
#define VIBEFLAG_PLAYER_DEAD 0x00000001
#define VIBEFLAG_PLAYER_RESPAWN 0x00000002
#define VIBEFLAG_LEVEL_END 0x00000004
#define VIBEFLAG_WEAPON_FIRED 0x00000008
#define VIBEFLAG_FORCE_APPLIED 0x00000010
#define VIBEFLAG_PLAYER_DAMAGED 0x00000020
#define VIBEFLAG_QUATERFRAME_0 0x00000040
#define VIBEFLAG_QUATERFRAME_1 0x00000080
#define VIBEFLAG_QUATERFRAME_2 0x00000100
#define VIBEFLAG_QUATERFRAME_3 0x00000200

////////////////////////////////////////////////////////////
// Weapon Defines
#define WEAPON_LASER 0
#define WEAPON_VAUSS 1
#define WEAPON_MICROWAVE 2
#define WEAPON_PLASMA 3
#define WEAPON_FUSION 4
#define WEAPON_SUPER_LASER 5
#define WEAPON_MASSDRIVER 6
#define WEAPON_NAPALM 7
#define WEAPON_EMD 8
#define WEAPON_OMEGA 9
#define WEAPON_CONCUSSION 10
#define WEAPON_HOMING 11
#define WEAPON_IMPACTMORTAR 12
#define WEAPON_SMART 13
#define WEAPON_MEGA 14
#define WEAPON_FRAG 15
#define WEAPON_GUIDED 16
#define WEAPON_NAPALMROCKET 17
#define WEAPON_CYCLONE 18
#define WEAPON_BLACKSHARK 19
#define WEAPON_FLARE 20

////////////////////////////////////////////////////////////
// Functions

#define STDCALLFUNC _stdcall

#ifdef __cplusplus
#define CEXTERN extern "C"
#else
#define CEXTERN
#endif

// Called once, during initialization to initialize the IntelliVIBE device.
// If initialization fails, then it should return 0, else return 1.
CEXTERN int STDCALLFUNC IntelliVIBE_Initialize(d3_init_info *init_info);
typedef int(STDCALLFUNC *IntelliVIBE_Initialize_fp)(d3_init_info *init_info);

// Called once when Descent 3 is about to shutdown, to do any final shutdown
// procedures needed by the device.
CEXTERN void STDCALLFUNC IntelliVIBE_Shutdown(void);
typedef void(STDCALLFUNC *IntelliVIBE_Shutdown_fp)(void);

// Called once per frame during game play.  This allows the device to perform
// anything that needs to be done on a frame interval.
CEXTERN void STDCALLFUNC IntelliVIBE_DoQuaterFrame(d3_intellivibe *frame_info);
typedef void(STDCALLFUNC *IntelliVIBE_DoQuaterFrame_fp)(d3_intellivibe *frame_info);

#endif
