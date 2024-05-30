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

 * $Logfile: /DescentIII/main/spew.h $
 * $Revision: 14 $
 * $Date: 5/02/99 2:52a $
 * $Author: Jeff $
 *
 * header file for spew system
 *
 * $Log: /DescentIII/main/spew.h $
 *
 * 14    5/02/99 2:52a Jeff
 * fixed spewers for turrets/rotaters
 *
 * 13    4/29/99 7:50p Jason
 * fixed spew errors
 *
 * 12    10/18/98 9:04p Jeff
 * sped up spew by saving values?
 *
 * 11    8/19/98 5:36p Samir
 * moved some stuff from source to header file.
 *
 * 10    8/19/98 1:44p Samir
 * added array of spew extern
 *
 * 9     2/04/98 6:09p Matt
 * Changed object room number to indicate a terrain cell via a flag.  Got
 * rid of the object flag which used to indicate terrain.
 *
 * 8     2/02/98 5:40p Luke
 *
 * 7     2/02/98 3:10p Jeff
 * new handle system, makes more reliable
 *
 * 6     1/28/98 6:29p Jeff
 * Added the "RISCy" layer between predef statements and the game
 *
 * 5     1/26/98 11:01a Jeff
 * changed spew struct to use object handles
 *
 * 4     1/23/98 7:00p Jeff
 * checked object before using it bug fixed
 *
 * 3     1/23/98 5:18p Jeff
 * Added in new timer and 3/5 of the random flags
 *
 * 2     1/23/98 3:03p Jeff
 * Added mass into spew structure
 *
 * 1     1/23/98 12:07p Jeff
 *
 * 2     1/23/98 11:50a Jeff
 * Creation of Spew system
 *
 * $NoKeywords: $
 */

#ifndef __SPEW_H_
#define __SPEW_H_

#include "vecmat.h"

#define SF_FORCEUPDATE 0x01     // this spew needs to do an update no matter what next time it spews (gunpoint)
#define SF_UPDATEDFORFRAME 0x02 // thie spew has already updated it's position for this frame, no need to do it again
#define SF_UPDATEEVERYFRAME 0x04

struct guninfo {
  int obj_handle; // handle to object (needed if it's gunpoint based)
  int gunpoint;   // gunpoint number		(needed if it's gunpoint based)
};

struct pointinfo {
  vector origin; // origin of the viseffect (needed if it's NOT gunpoint based)
  vector normal; // normal of the point
  int room_num;  // room number of point of origin
};

struct spewinfo {
  uint8_t flags;       // flags
  bool inuse;        // if this slot is in use
  bool use_gunpoint; // is this a gunpoint based object (yes=true)
  bool real_obj;     // if this object should be treated as a real object (can hurt you) (yes=true)

  union {
    pointinfo pt; // point info structure (needed if it's point based)
    guninfo gp;   // gunpoint info structure (needed if it's gunpoint based)
  };

  int effect_type; // viseffect type (see fireball.h)
  int phys_info;   // physics info type (see physics.h)
  int random;      // should the viseffect have some randomness to it (must be 0 if real)
  int handle;      // stored handle for speweffect

  float drag;                 // drag of viseffect
  float mass;                 // mass of viseffect
  float time_int;             // how often it should be 'updated'
  float longevity;            // lifespan of the viseffect (i.e. how long should the puff of smoke last)
  float lifetime;             // how long viseffect should run (0=infinity, until asked to stop)
  float size;                 // size of viseffect blob
  float speed;                // speed of viseffect blob
  float time_until_next_blob; // per spewer
  float start_time;           // time viseffect started

  vector gp_normal, gun_point; // vectors saved to keep from recalculating
};

#define SPEW_RAND_WIGGLE 1
#define SPEW_RAND_SPEED 2
#define SPEW_RAND_SIZE 4
#define SPEW_RAND_SLIDE 8
#define SPEW_RAND_LIFETIME 16

#define MAX_SPEW_EFFECTS 50 // maximum viseffects to be handled by at once

// spew array
extern spewinfo SpewEffects[];

// spew count value.
extern int spew_count;

// Initializes the Spew system
void SpewInit();

// Creates a Spew effect
// spew	->	Pointer to spewinfo structure (that is filled out)...may be deleted after function call
// returns a handle to the spew effect
int SpewCreate(spewinfo *spew);

// Does a step in a spew system
void SpewEmitAll(void);

// Clears a Spew Event given a handle to it
void SpewClearEvent(int handle, bool force = true);

#endif
