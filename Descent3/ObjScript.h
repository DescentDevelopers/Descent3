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

/*
 * $Logfile: /DescentIII/main/ObjScript.h $
 * $Revision: 21 $
 * $Date: 1/13/99 2:27p $
 * $Author: Jeff $
 *
 * Object scripting
 *
 * $Log: /DescentIII/main/ObjScript.h $
 *
 * 21    1/13/99 2:27p Jeff
 * added an is_dying flag for evt_destroy to determine whether the event
 * is due to  level end or because it really is dying
 *
 * 20    1/08/99 2:56p Samir
 * Ripped out OSIRIS1.
 *
 * 19    10/18/98 2:59p Jason
 * fixes for beta4
 *
 * 18    8/19/98 1:35p Samir
 * added ReinitTrigger
 *
 * 17    8/18/98 12:57a Samir
 * created function to reinitialize scripts from savegame.
 *
 * 16    5/26/98 7:06p Samir
 * is_custom added to determine if named script is custom of default.
 *
 * 15    4/19/98 9:56p Chris
 * AI system is integrated with OSIRIS.  Path system is integrated with
 * the AI system.  Bugs will ensue.
 *
 * 14    4/15/98 5:56p Chris
 * Made the AI system script friendly
 *
 * 13    3/16/98 11:19a Jason
 * got scripts working with multiplayer
 *
 * 12    2/09/98 11:07a Samir
 * Added reset function to script_info.
 *
 * 11    1/23/98 6:11p Samir
 * Addded some trigger functions.
 *
 * 10    1/23/98 3:06p Samir
 * Cleaned up and enhanced script_info structure.
 *
 * 9     1/22/98 6:23p Samir
 * Moved constants from ObjCScript to ObjScript.h
 *
 * 8     1/21/98 5:02p Samir
 * Don't delete pointers in script_info when destroyed.
 *
 * 7     1/21/98 4:33p Luke
 * Null pointers when destroying script_info.
 *
 * 6     1/20/98 4:12p Samir
 * New script housekeeping system.
 *
 * 5     9/24/97 2:58p Samir
 * Moved some script defines from mission.h to ObjScript.h
 *
 * 4     9/10/97 4:00p Samir
 * added define for default level script id.
 *
 * 3     9/10/97 2:00p Samir
 * added extern to Level_script D3X Program pointer.
 *
 * 2     8/21/97 5:56p Samir
 * Took out script specific stuff from ObjInit and moved to ObjScript
 *
 * 2     6/02/97 1:06p Samir
 * Added some more scripting functions.
 *
 * $NoKeywords: $
 */

#ifndef OBJSCRIPT_H
#define OBJSCRIPT_H

#include "pstypes.h"
#include "d3x_op.h"
#include "vecmat.h"

struct object;
struct trigger;

//	assigns scripts for a level.
extern void AssignScriptsForLevel();

//	free scripts for a level
extern void FreeScriptsForLevel();

// allocates and initializes the scripts for an object.
extern void InitObjectScripts(object *objp, bool do_evt_created = true);

// frees all scripts for an object.
extern void FreeObjectScripts(object *objp, bool level_end = false);

// allocates and initializes the scripts for a trigger.
extern void InitTriggerScript(trigger *tp);

// frees all scripts for an trigger.
extern void FreeTriggerScript(trigger *tp, bool level_end = false);

//@@// called to reinitialize an object's 'state' given the current script element of object.
//@@//	refuses to call EVT_CREATED too.  this just restores the state of a script given the current script_info
//@@// op is the object where the script will be assigned.  the script should already be freed and ready
//@@void ReinitObjectScripts(object *op, script_info *script, int mem_size, vector *mem);
//@@
//@@// called to reinitialize a trigger's 'state' given the current script element of trigger.
//@@//	refuses to call EVT_CREATED too.  this just restores the state of a script given the current script_info
//@@// tp is the trogger where the script will be assigned.  the script should already be freed and ready
//@@void ReinitTriggerScripts(trigger *tp, script_info *script, int mem_size, vector *mem);

#endif
