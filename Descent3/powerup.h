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

#ifndef POWERUP_H
#define POWERUP_H

#include "pstypes.h"
#include "manage.h"
#include "object.h"

#define MAX_POWERUPS 100
#define MAX_STATIC_POWERUPS 50

#define PF_IMAGE_BITMAP 1

// These defines must correspond to the Static_powerup_names array
#define POW_SHIELD 0
#define POW_ENERGY 1

#define POW_LASER 2
#define POW_VULCAN_WEAPON 3
#define POW_SPREADFIRE_WEAPON 4
#define POW_PLASMA_WEAPON 5
#define POW_FUSION_WEAPON 6

#define POW_SUPER_LASER 7
#define POW_GAUSS_WEAPON 8
#define POW_HELIX_WEAPON 9
#define POW_PHOENIX_WEAPON 10
#define POW_OMEGA_WEAPON 11

#define POW_MISSILE_1 12
#define POW_MISSILE_4 13 // 4-pack MUST follow single missile
#define POW_HOMING_MISSILE_1 14
#define POW_HOMING_MISSILE_4 15 // 4-pack MUST follow single missile
#define POW_PROXIMITY_WEAPON 16
#define POW_SMART_MISSILE_WEAPON 17
#define POW_MEGA_WEAPON 18

#define POW_FLASH_MISSILE_1 19
#define POW_FLASH_MISSILE_4 20 // 4-pack MUST follow single missile
#define POW_GUIDED_MISSILE_1 21
#define POW_GUIDED_MISSILE_4 22 // 4-pack MUST follow single missile
#define POW_SMART_MINE 23
#define POW_MERCURY_MISSILE_1 24
#define POW_MERCURY_MISSILE_4 25 // 4-pack MUST follow single missile
#define POW_EARTHSHAKER_MISSILE 26

#define POW_EXTRA_LIFE 27
#define POW_QUAD_FIRE 28
#define POW_VULCAN_AMMO 29
#define POW_CLOAK 30
#define POW_TURBO 31
#define POW_INVULNERABILITY 32
#define POW_FULL_MAP 33
#define POW_CONVERTER 34
#define POW_AMMO_RACK 35
#define POW_AFTERBURNER 36
#define POW_HEADLIGHT 37

#define POW_FLAG_BLUE 38
#define POW_FLAG_RED 39
#define POW_HOARD_ORB 40

// sound stuff

#define MAX_POWERUP_SOUNDS 7

#define PSI_PICKUP 0

struct powerup {
  char name[PAGENAME_LEN];
  float size;
  int score;
  int image_handle;              // Either a vclip or a polygon model
  char model_name[PAGENAME_LEN]; // used for remapping powerups which contain models
  int flags;
  uint16_t used;

  int16_t sounds[MAX_POWERUP_SOUNDS];

  // Default physics information for this powerup type
  physics_info phys_info; // the physics data for this obj type.

};

extern int Num_powerups;
extern powerup Powerups[MAX_POWERUPS];
extern char *Static_powerup_names[];

// Sets all powerups to unused
void InitPowerups();

// Allocs a powerup for use, returns -1 if error, else index on success
int AllocPowerup();

// Frees powerup index n
void FreePowerup(int n);

// Gets next powerup from n that has actually been alloced
int GetNextPowerup(int n);

// Gets previous powerup from n that has actually been alloced
int GetPrevPowerup(int n);

// Searches thru all powerups for a specific name, returns -1 if not found
// or index of powerup with name
int FindPowerupName(char *name);

// Given a filename, loads either the model or vclip found in that file.  If type
// is not NULL, sets it to 1 if file is model, otherwise sets it to zero
int LoadPowerupImage(char *filename, int *type);

// Given a powerup handle, returns that powerups image for framenum
int GetPowerupImage(int handle, int framenum);

// Given an object, renders the representation of this powerup
// Currently only handles bitmap types, not poly models
void DrawPowerupObject(object *obj);

// Given a powerup name, assigns that powerup to a specific index into
// the Powerups array.  Returns -1 if the named powerup is not found, 0 if the powerup
// is already in its place, or 1 if successfully moved
int MatchPowerupToIndex(char *name, int dest_index);

// Moves a powerup from a given index into a new one (above MAX_STATIC_POWERUPS)
// returns new index
int MovePowerupFromIndex(int index);

// This is a very confusing function.  It takes all the powerups that we have loaded
// and remaps then into their proper places (if they are static).
void RemapPowerups();

// goes thru every entity that could possible have a powerup index (ie objects, robots, etc)
// and changes the old index to the new index
void RemapAllPowerupObjects(int old_index, int new_index);

// Player activated this powerup
int DoPowerup(object *obj);

#endif
