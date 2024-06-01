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

 * $Logfile: /DescentIII/main/trigger.cpp $
 * $Revision: 26 $
 * $Date: 5/20/99 9:03p $
 * $Author: Matt $
 *
 * Trigger management
 *
 * $Log: /DescentIII/main/trigger.cpp $
 *
 * 26    5/20/99 9:03p Matt
 * Changed trigger system so that collisions with rendered portals don't
 * set off triggers on the portals.
 *
 * 25    5/13/99 3:42p Ardussi
 * changes for compiling on the Mac
 *
 * 24    3/23/99 5:52p Matt
 * Added clutter activators for triggers
 *
 * 23    2/21/99 4:35p Chris
 * Improving the level goal system...  Not done.
 *
 * 22    2/15/99 7:47p Matt
 * Added a default case to CheckTrigger
 *
 * 21    2/11/99 8:30p Matt
 * Took out Int3() when couldn't find an object's parent
 *
 * 20    2/11/99 1:10p Matt
 * Added functions to get & set a triggers enabled/disabled state
 *
 * 19    2/02/99 8:43a Chris
 * I made buildings with AI work correctly (ie really big robots should be
 * buildings)
 * anim to and from states are now shorts instead of bytes
 *
 * 18    1/21/99 11:34a Matt
 * Got rid of portal triggers.  Since we don't have multi-face portals, a
 * face trigger works fine for a portal.  Also fixed a few editor/trigger
 * bugs.
 *
 * 17    1/14/99 11:06a Matt
 * Added names to triggers
 *
 * 16    1/08/99 2:56p Samir
 * Ripped out OSIRIS1.
 *
 * 15    12/17/98 12:08p Jeff
 * first checkin of new implementation of OSIRIS (old OSIRIS no longer
 * works)
 *
 * 14    10/21/98 9:44p Matt
 * Fixed bug that caused 1-shot portal triggers to not be found after the
 * first time.
 *
 * 13    9/01/98 12:04p Matt
 * Ripped out multi-face portal code
 *
 * 12    8/11/98 1:04p Matt
 * Fixed bug that caused one-shot triggers to be marked as used even when
 * hit by an object that wasn't a valid activator.
 *
 * 11    5/26/98 7:06p Samir
 * took script.is_custom into account for triggers.
 *
 * 10    4/24/98 1:53a Samir
 * added trigger init and free.
 *
 * 9     1/23/98 5:54p Samir
 * New style script  trigger support.
 *
 * 8     1/20/98 4:12p Samir
 * New script housekeeping system.
 *
 * 7     12/23/97 1:33p Samir
 * Added pserror.h
 *
 * 6     9/24/97 6:18p Samir
 * Use script names instead of script id values to identify scripts.
 *
 * 5     9/22/97 12:24p Matt
 * Fixed bugs with face flags for portal triggers
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

#include "trigger.h"
#include "room.h"
#include "object.h"
//@$-#include "d3x.h"
#include "pserror.h"
#include "osiris_dll.h"
#include "levelgoal.h"

#include <stdlib.h>
#include <string.h>

// The maximum number of triggers that can be in the mine
#define MAX_TRIGGERS 100

// The list of triggers for the mine
trigger Triggers[MAX_TRIGGERS];

// The number of triggers currently in the mine
int Num_triggers = 0;

void FreeTriggers();

//	initializes trigger system
void InitTriggers() {
  int i;

  for (i = 0; i < MAX_TRIGGERS; i++) {
    Triggers[i].roomnum = -1;
    Triggers[i].facenum = -1;
    Triggers[i].flags = 0;

    Triggers[i].osiris_script.script_id = -1;
    Triggers[i].osiris_script.script_instance = NULL;
  }

  Num_triggers = 0;

  atexit(FreeTriggers);
}

// Free up all the triggers
void FreeTriggers() {
  int i;

  for (i = 0; i < Num_triggers; i++)
    FreeTriggerScript(&Triggers[i]);

  Num_triggers = 0;
}

// Returns a pointer to the trigger attached to the given room:face
// Generates an error if the trigger cannot be found
// Paramaters:	roomnum,facenum - the room and face with the trigger
trigger *FindTrigger(int roomnum, int facenum) {
  face *fp = &Rooms[roomnum].faces[facenum];
  trigger *tp;
  int i;

  // Go through all the triggers & look for the requested one
  for (i = 0, tp = Triggers; i < Num_triggers; i++, tp++)
    if (tp->roomnum == roomnum)
      if (tp->facenum == facenum)
        return tp;

  Int3(); // Didn't find the requested trigger -- very bad!

  return NULL;
}

// Called to see if a trigger was tripped
void CheckTrigger(int roomnum, int facenum, object *objp, int event_type) {
  room *rp = &Rooms[roomnum];
  face *fp = &rp->faces[facenum];

  ASSERT((event_type == TT_PASS_THROUGH) || (event_type == TT_COLLIDE));

  if (fp->flags & FF_HAS_TRIGGER) {
    trigger *tp;
    int type;

    tp = FindTrigger(roomnum, facenum);
    ASSERT(tp != NULL);

    if (tp == NULL) {
      Int3(); // very bad!  Get Matt.
      return;
    }

    // Check for dead or disabled trigger
    if (tp->flags & (TF_DISABLED | TF_DEAD))
      return;

    // If portal trigger, make sure this is a pass-through event
    if ((fp->portal_num != -1) && (event_type != TT_PASS_THROUGH))
      return;

    // Get the activator type for the object that hit the trigger
    switch (objp->type) {
    case OBJ_PLAYER:
      type = AF_PLAYER;
      break;
    case OBJ_BUILDING:
    case OBJ_ROBOT:
      type = AF_ROBOT;
      break;
    case OBJ_CLUTTER:
      type = AF_CLUTTER;
      break;
    case OBJ_WEAPON: {
      object *parent = ObjGetUltimateParent(objp);
      int parent_type = parent->type;
      if (parent_type == OBJ_PLAYER)
        type = AF_PLAYER_WEAPON;
      else if (parent_type == OBJ_ROBOT || parent_type == OBJ_BUILDING)
        type = AF_ROBOT_WEAPON;
      else
        return; // unknown parent for weapon
      break;
    }
    default:
      return;
    }

    // Check if this object is a valid activator for this trigger
    if (tp->activator & type) {
      mprintf(0, "Hit trigger %d\n", tp - Triggers);

      // Execute this trigger's script
      tOSIRISEventInfo ei;
      ei.evt_collide.it_handle = objp->handle;
      Osiris_CallTriggerEvent((tp - Triggers), EVT_COLLIDE, &ei);

      // If one-shot, kill this trigger, and mark face as destroyed
      if (tp->flags & TF_ONESHOT) {
        tp->flags |= TF_DEAD;
        if (fp->portal_num == -1) // don't destroy a portal face
          fp->flags |= FF_DESTROYED;
      }

      if (tp->flags & TF_INFORM_ACTIVATE_TO_LG) {
        Level_goals.Inform(LIT_TRIGGER, LGF_COMP_ACTIVATE, tp - Triggers);
      }
    }
  }
}

// Enable or disable a trigger
void TriggerSetState(int trigger_num, bool enabled) {
  trigger *tp = &Triggers[trigger_num];

  if (enabled)
    tp->flags &= ~TF_DISABLED;
  else
    tp->flags |= TF_DISABLED;
}

// Determines if a trigger is enabled or disabled
// Returns TRUE if enabled, else false
bool TriggerGetState(int trigger_num) {
  trigger *tp = &Triggers[trigger_num];

  return ((tp->flags & TF_DISABLED) == 0);
}

#ifdef EDITOR

#include "editor\d3edit.h"

//
// EDITOR functions follow
//

// Create a new trigger
// Parameters:	roomnum,facenum - where the trigger is
//					flags - flags for this trigger
//					activator - activator mask
//					script - handle for the script for this trigger
// Returns:	trigger number of new trigger, or -1 if error
int AddTrigger(char *name, int roomnum, int facenum, int activator, const char *script) {
  room *rp = &Rooms[roomnum];
  face *fp = &rp->faces[facenum];
  trigger *tp;

  if (Num_triggers >= MAX_TRIGGERS)
    return -1;

  if (strlen(name) > TRIG_NAME_LEN) {
    Int3();
    return -1;
  }

  tp = &Triggers[Num_triggers];

  strcpy(tp->name, name);
  tp->roomnum = roomnum;
  tp->facenum = facenum;
  tp->activator = activator;
  tp->flags = 0;

  // Flag the face
  fp->flags |= FF_HAS_TRIGGER;

  // Update count
  Num_triggers++;

  // Update flag
  World_changed = 1;

  // Everything ok
  return Num_triggers - 1;
}

// Remove a trigger
// Paramters:	trig_num - the trigger to delete
void DeleteTrigger(int trig_num) {
  trigger *tp = &Triggers[trig_num];
  room *rp = &Rooms[tp->roomnum];
  face *fp = &rp->faces[tp->facenum];

  // Clear the face flag
  fp->flags &= ~FF_HAS_TRIGGER;

  // Free the script for this trigger
  FreeTriggerScript(tp);

  // Move other triggers down in list
  for (int i = trig_num; i < Num_triggers - 1; i++)
    Triggers[i] = Triggers[i + 1];

  // Update count
  Num_triggers--;
}

// Remove a trigger
// Paramters:	roomnum,facenum - where the trigger is
void DeleteTrigger(int roomnum, int facenum) {
  trigger *tp;

  tp = FindTrigger(roomnum, facenum);

  ASSERT(tp != NULL);

  DeleteTrigger(tp - Triggers);
}

#endif
