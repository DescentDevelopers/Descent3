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

 * $Logfile: /DescentIII/main/doorway.h $
 * $Revision: 15 $
 * $Date: 5/05/99 12:58a $
 * $Author: Matt $
 *
 * Doorway structures
 *
 * $Log: /DescentIII/main/doorway.h $
 *
 * 15    5/05/99 12:58a Matt
 * Added doorway flag to tell the Guide-Bot to ignore the locked state of
 * the door.
 *
 * 14    4/23/99 8:28p Kevin
 * trying to get doors working in save/load games
 *
 * 13    4/20/99 8:35p Matt
 * Changed ActivateDoorway() to not exit if a door is locked.   It's the
 * caller's responsibility to check if the door is openable (if he cares)
 * before activating.
 *
 * 12    4/20/99 6:55p Matt
 * Added code to keep a bitmask of all keys held by all players, and to
 * allow a robot to open a door if any player has the key(s) for that
 * door.
 *
 * 11    2/06/99 10:03p Matt
 * Added keys system
 *
 * 10    2/04/99 2:05p Matt
 * Added blastable doors
 *
 * 9     1/29/99 12:47p Matt
 * Rewrote the doorway system
 *
 * 8     9/08/98 12:06p Jason
 * added automatic reinitting of doors when rooms get deleted
 *
 * 7     10/06/97 1:03p Samir
 * Took out script name.  Doorways are objects, and if there is a custom
 * script for a doorway, we look at it's object.
 *
 * 6     10/06/97 1:01p Jason
 * made doors work with scripts (sort of)
 *
 * 5     10/03/97 12:51p Jason
 * added GetDoorwayNumFromObject function
 *
 * 4     10/03/97 12:24p Jason
 * added new keys for doorways
 *
 * 3     9/29/97 12:09p Jason
 * added functionality to doorway system
 *
 * 2     9/12/97 5:38p Jason
 * got doors working
 *
 * 3     3/14/97 12:51p Samir
 * Added code to clear active state of doorways.
 *
 * 2     3/13/97 11:58a Samir
 * Added stuff for active doorway and opening/closing
 *
 * 1     3/12/97 2:08p Samir
 * Moved structures and prototypes from door.h
 *
 * $NoKeywords: $
 */

#ifndef DOORWAY_H
#define DOORWAY_H

#include "pstypes.h"

// IMPORTANT!!!!!!!!!!!
// "Doors" refers to a predefined door that is in memory
// "Doorways" are specific doors that are in the mine
// So, there can be several Doorways that all point to the same Door
// Get it?  If not, talk to Samir or Jason

//	doorway state
#define DOORWAY_STOPPED 0      // door is not moving
#define DOORWAY_OPENING 1      // door is opening
#define DOORWAY_CLOSING 2      // door is closing
#define DOORWAY_WAITING 3      // door is waiting to be closed
#define DOORWAY_OPENING_AUTO 4 // door is opening and will automatically close

//	doorway flags
#define DF_BLASTED 1           // it's been blasted away
#define DF_AUTO 2              // doorway closes after time.
#define DF_LOCKED 4            // doorway can't open for now
#define DF_KEY_ONLY_ONE 8      // only one key is needed to open (not all keys)
#define DF_GB_IGNORE_LOCKED 16 // the Guide-bot ignores the locked state of this door

//	keymasks
#define KF_KEY1 1 // Each key is a bit in the key_mask set in the door/object
#define KF_KEY2 2
#define KF_KEY3 4
#define KF_KEY4 8

#define MAX_ACTIVE_DOORWAYS 30

extern int Num_active_doorways; // number of active doors in game
extern int Active_doorways[MAX_ACTIVE_DOORWAYS];

// A doorway (room) in the mine
struct doorway {
  int doornum;       // door type of this doorway
  uint8_t state;       // current state of doorway
  uint8_t flags;       // flags associated with a doorway
  uint8_t keys_needed; // used by trigger system.  these bits need to be set to activate door
  int8_t activenum;   // index into active doorways array, or -1 if not active
  float position;    // current position of door
  float dest_pos;    // destination position
  int sound_handle;  // handle of last sound played
};

//	Macros

// returns the bitflag for the given key number
#define KEY_FLAG(keynum) (1 << (keynum - 1))

//	Variables

// This is a mask of all keys held by all players. Robots use this to determine if a door is openable.
extern int Global_keys;

//
//	Editor Functions
//

// Define a couple types
struct room;
struct object;

// Adds a doorway to the specified room
// Returns a pointer to the doorway struct
doorway *DoorwayAdd(room *rp, int doornum);

// Updates the animation when the door position is changed
void DoorwayUpdateAnimation(room *rp);

//
// General Functions
//

//	executes all active doorways in the game
void DoorwayDoFrame();

//	clears the active state of all doorways that are active back to closed
void DoorwayDeactivateAll();

// Opens a door and, if it has the auto flag set, closes it
// Does not check to see if the door is openable -- the caller must do that.
void DoorwayActivate(int door_obj_handle);

// Stops the doorway if it's moving
void DoorwayStop(int door_obj_handle);

// Locks a given doorway
void DoorwayLockUnlock(int door_obj_handle, bool state);

// Sets the current position of the door.  0.0 = totally closed, 1.0 = totally open
// Does not check to see if the door is openable -- the caller must do that.
void DoorwaySetPosition(int door_obj_handle, float pos);

// Called after loading a saved game to rebuild the active list
void DoorwayRebuildActiveList();

// Called when a door is blown up
void DoorwayDestroy(object *objp);

//
// Query Functions
//

// Returns true if the doorway is locked, else false
bool DoorwayLocked(int door_obj_handle);

// Returns true if the doorway is locked, else false
bool DoorwayLocked(room *rp);

// Returns true if the doorway is openable by the specified player, else false
bool DoorwayOpenable(int door_obj_handle, int opener_handle);

// Returns the current state of the specified door
int DoorwayState(int door_obj_handle);

// Returns the current position of the door.  0.0 = totally closed, 1.0 = totally open
float DoorwayPosition(room *rp);

// Returns the current position of the door.  0.0 = totally closed, 1.0 = totally open
float DoorwayPosition(int door_obj_handle);

// Make old name work
#define DoorwayGetPosition DoorwayPosition

#endif
