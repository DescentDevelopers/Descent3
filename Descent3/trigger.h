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

 * $Logfile: /DescentIII/main/trigger.h $
 * $Revision: 17 $
 * $Date: 5/20/99 9:03p $
 * $Author: Matt $
 *
 * Trigger header
 *
 * $Log: /DescentIII/main/trigger.h $
 *
 * 17    5/20/99 9:03p Matt
 * Changed trigger system so that collisions with rendered portals don't
 * set off triggers on the portals.
 *
 * 16    4/14/99 3:57a Jeff
 * fixed case mismatch in #includes
 *
 * 15    3/23/99 5:52p Matt
 * Added clutter activators for triggers
 *
 * 14    2/21/99 4:35p Chris
 * Improving the level goal system...  Not done.
 *
 * 13    2/11/99 1:10p Matt
 * Added functions to get & set a triggers enabled/disabled state
 *
 * 12    1/21/99 11:34a Matt
 * Got rid of portal triggers.  Since we don't have multi-face portals, a
 * face trigger works fine for a portal.  Also fixed a few editor/trigger
 * bugs.
 *
 * 11    1/14/99 11:06a Matt
 * Added names to triggers
 *
 * 10    1/08/99 2:56p Samir
 * Ripped out OSIRIS1.
 *
 * 9     12/16/98 10:18p Jeff
 * added new osiris struct members
 *
 * 8     4/24/98 1:53a Samir
 * added trigger init and free.
 *
 * 7     2/03/98 11:12a Samir
 * If pointer passed to TRIGNUM is NULL return -1.
 *
 * 6     1/20/98 4:12p Samir
 * New script housekeeping system.
 *
 * 5     9/24/97 6:18p Samir
 * Use script names instead of script id values to identify scripts.
 *
 * 4     9/12/97 3:58p Matt
 * Added support for activators and one-shot triggers
 *
 * 3     9/11/97 1:57p Matt
 * Added trigger struct & a bunch of functions.  System is not done yet,
 * though.
 *
 * 2     9/08/97 10:01a Matt
 * Ripped out old trigger code
 *
 * $NoKeywords: $
 */

#ifndef TRIGGER_H
#define TRIGGER_H

#include "pstypes.h"
#include "ObjScript.h"
#include "object_external_struct.h"
#include "vecmat.h"

// Trigger flags
#define TF_UNUSED 1                 //
#define TF_DISABLED 2               // This trigger is currently not active
#define TF_DEAD 4                   // This trigger has been tripped and is now dead
#define TF_ONESHOT 8                // This trigger only works once
#define TF_INFORM_ACTIVATE_TO_LG 16 // This trigger is involved in the level goal system

// Activator flags
#define AF_PLAYER 1        // The player can activate this trigger
#define AF_PLAYER_WEAPON 2 // The player's weapons can activate this trigger
#define AF_ROBOT 4         // A robot can activate this trigger
#define AF_ROBOT_WEAPON 8  // A robot's weapon can activate this trigger
#define AF_CLUTTER 16      // A piece of clutter

// Declare this here so we don't have to include the header file
//@$-struct tD3XThread;

struct tOSIRISTriggerScript {
  int script_id;
  void *script_instance;
};

#define TRIG_NAME_LEN 19

// The trigger structure
struct trigger {
  char name[TRIG_NAME_LEN + 1]; // the name of this trigger
  int roomnum;                  // the room this trigger is in
  int facenum;                  // the face to which this trigger is attched
  int16_t flags;                  // flags for this trigger
  int16_t activator;              // flags for what can activate this trigger
  // This is allocated when the level is started
  tOSIRISTriggerScript osiris_script;
};

// The number of triggers currently in the mine
extern int Num_triggers;

// The list of triggers for the mine
extern trigger Triggers[];

// Macro to get trigger number
#define TRIGNUM(tp) (tp) ? ((tp)-Triggers) : -1

//	initializes trigger system
void InitTriggers();

// Free triggers.
void FreeTriggers();

// types for CheckTrigger()
#define TT_PASS_THROUGH 0
#define TT_COLLIDE 1

// Called to see if a trigger was tripped
void CheckTrigger(int roomnum, int facenum, object *objp, int type);

// Enable or disable a trigger
void TriggerSetState(int trigger_num, bool enabled);

// Determines if a trigger is enabled or disabled
// Returns TRUE if enabled, else false
bool TriggerGetState(int trigger_num);

// Create a new trigger
// Parameters:	roomnum,facenum - where the trigger is
//					activator - activator mask
//					script - handle for the script for this trigger
// Returns:	trigger number of new trigger, or -1 if error
int AddTrigger(char *name, int roomnum, int facenum, int activator, const char *script);

// Remove a trigger
// Paramters:	trig_num - the trigger to delete
void DeleteTrigger(int trig_num);

// Remove a trigger
// Paramters:	roomnum,facenum - where the trigger is
void DeleteTrigger(int roomnum, int facenum);

#endif
