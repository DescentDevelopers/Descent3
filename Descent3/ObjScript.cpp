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

 * $Logfile: /DescentIII/main/ObjScript.cpp $
 * $Revision: 37 $
 * $Date: 5/22/99 12:14a $
 * $Author: Jeff $
 *
 * Object scripting
 *
 * $Log: /DescentIII/main/ObjScript.cpp $
 *
 * 37    5/22/99 12:14a Jeff
 * don't init scripts if the object already has em
 *
 * 36    5/04/99 6:40p Chris
 *
 * 35    4/14/99 3:57a Jeff
 * fixed case mismatch in #includes
 *
 * 34    4/02/99 2:13p Chris
 *
 * 33    1/13/99 2:27p Jeff
 * added an is_dying flag for evt_destroy to determine whether the event
 * is due to  level end or because it really is dying
 *
 * 32    1/08/99 2:56p Samir
 * Ripped out OSIRIS1.
 *
 * 31    12/31/98 7:34p Jeff
 * call EVT_DESTROY from FreeObjectScripts (if a direct call to ObjDelete
 * was called, it would never get the event)
 *
 * 30    12/17/98 12:08p Jeff
 * first checkin of new implementation of OSIRIS (old OSIRIS no longer
 * works)
 *
 * 29    10/18/98 2:59p Jason
 * fixes for beta4
 *
 * 28    10/08/98 2:39p Chris
 *
 * 27    10/08/98 2:25p Chris
 * Clutter and building can now have scripts
 *
 * 26    9/17/98 11:11a Chris
 * Worked on goal system and improved integration with OSIRIS (better
 * EVT_AI_INIT event calling)
 *
 * 25    8/26/98 1:30p Jason
 * fixed client creation bugs
 *
 * 24    8/19/98 5:37p Samir
 * fixed bug in ReinitTriggerScripts.
 *
 * 23    8/19/98 1:35p Samir
 * added ReinitTrigger
 *
 * 22    8/19/98 12:22p Samir
 * is_custom is now valid.
 *
 * 21    8/18/98 12:57a Samir
 * created function to reinitialize scripts from savegame.
 *
 * 20    8/07/98 7:21p Samir
 * changed references to level script exec.
 *
 * 19    7/28/98 5:44p Samir
 * added hooks for level create and destroy
 *
 * 18    5/26/98 7:06p Samir
 * is_custom added to determine if named script is custom of default.
 *
 * 17    3/19/98 12:57p Chris
 * Changed '<Highest_object_index+1' to '<=Highest_object_index' like the
 * rest of the code
 *
 * 16    1/23/98 6:11p Samir
 * Addded some trigger functions.
 *
 * 15    1/23/98 3:06p Samir
 * Cleaned up and enhanced script_info structure.
 *
 * 14    1/20/98 4:12p Samir
 * New script housekeeping system.
 *
 * 13    1/20/98 11:13a Samir
 * Door parameters work.
 *
 * 12    1/19/98 2:44p Samir
 * Use one script per object and started parameter passing support.
 *
 * 11    10/07/97 3:12p Samir
 * Fixed level script initialization thread problem.
 *
 * 10    10/06/97 4:10p Samir
 * Initialize door default object scripts.
 *
 * 9     9/30/97 5:34p Samir
 * Fixed initialization of custom scripts over default.
 *
 * 8     9/24/97 6:18p Samir
 * Use script names instead of script id values to identify scripts.
 *
 * 7     9/22/97 6:00p Samir
 * Added a name field to scripts, so we create threads using name instead
 * of number
 *
 * 6     9/11/97 1:59p Matt
 * Added code to initialize & free trigger scripts
 *
 * 5     9/10/97 3:59p Samir
 * Run level script create event on initialization.
 *
 * 4     9/10/97 1:57p Samir
 * Added code for initializing level scripts.
 *
 * 3     8/22/97 10:52a Samir
 * Added call to script for creation.
 *
 * 2     8/21/97 5:56p Samir
 * Took out script specific stuff from ObjInit and moved to ObjScript
 *
 * 3     6/02/97 1:06p Samir
 * Added some more scripting functions.
 *
 * $NoKeywords: $
 */

#include "ObjScript.h"

#include "object.h"
#include "objinfo.h"
#include "Mission.h"
#include "pserror.h"
#include "trigger.h"
#include "door.h"
#include "game.h"
#include "multi.h"
#include "osiris_dll.h"

#include <string.h>

void InitTriggerScript(trigger *tp);
void FreeTriggerScript(trigger *tp);

//	We assign all the scripts needed for the level right here.
void AssignScriptsForLevel() {
  int i;

  // Initialize scripts for the objects
  for (i = 0; i <= Highest_object_index; i++) {
    if (Objects[i].type != OBJ_NONE && Objects[i].osiris_script == NULL) {
      InitObjectScripts(&Objects[i]);
    }
  }

  // Initialize scripts for the triggers
  for (i = 0; i < Num_triggers; i++) {
    InitTriggerScript(&Triggers[i]);
  }
}

//	free scripts for a level
void FreeScriptsForLevel() {
  int i;

  // Free scripts for the objects
  for (i = 0; i <= Highest_object_index; i++) {
    FreeObjectScripts(&Objects[i], true);
  }

  // Free scripts for the triggers
  for (i = 0; i < Num_triggers; i++) {
    FreeTriggerScript(&Triggers[i], true);
  }

  FreeLevelScript();
}

// allocates and initializes the scripts for an object.
//	robots, powerups, doors, etc.
void InitObjectScripts(object *objp, bool do_evt_created) {
  // Bind scripts to the object
  Osiris_BindScriptsToObject(objp);

  if (do_evt_created) {
    tOSIRISEventInfo ei;
    if (objp->control_type == CT_AI) {
      Osiris_CallEvent(objp, EVT_AI_INIT, &ei);
    }

    Osiris_CallEvent(objp, EVT_CREATED, &ei);

    /*
    if (Game_mode & GM_MULTI && Netgame.local_role==LR_CLIENT)
            D3XExecScript(objp, EVT_CLIENT_CREATED, objp);
    else
    {
            D3XExecScript(objp, EVT_CREATED, objp);
            if(objp->control_type == CT_AI)
            {
                    D3XExecScript(objp, EVT_AI_INIT, NULL, REF_OBJTYPE, NULL);
            }
    }
    */
  }
}

// frees all scripts for an object.
void FreeObjectScripts(object *objp, bool level_end) {
  tOSIRISEventInfo ei;
  ei.evt_destroy.is_dying = (level_end) ? 1 : 0;

  if (!level_end)
    Osiris_CallEvent(objp, EVT_DESTROY, &ei);

  Osiris_DetachScriptsFromObject(objp);
}

// allocates and initializes the script for a trigger
void InitTriggerScript(trigger *tp) {}

// frees the script for a trigger
void FreeTriggerScript(trigger *tp, bool level_end) {}
