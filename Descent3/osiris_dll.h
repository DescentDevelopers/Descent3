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

 * $Logfile: /DescentIII/main/osiris_dll.h $
 * $Revision: 22 $
 * $Date: 4/14/99 3:57a $
 * $Author: Jeff $
 *
 * OSIRIS system header
 *
 * $Log: /DescentIII/main/osiris_dll.h $
 *
 * 22    4/14/99 3:57a Jeff
 * fixed case mismatch in #includes
 *
 * 21    3/10/99 6:21p Jeff
 * many fixes to demo system.  Fixed IGC so cameras move...fixed osiris to
 * be restored correctly, and it handles errors on restore
 *
 * 20    3/05/99 5:25p Jeff
 * fixed saving demos in multiplayer...required adding a parameter to
 * Osiris_LoadMissionModule() to pass in the name of the d3m file (for
 * storage)
 *
 * 19    2/28/99 8:52p Jeff
 * added functions to enable/disable creation events
 *
 * 18    2/18/99 11:06a Jeff
 * added event masks (so you can enable/disable object/trigger/level
 * events)
 *
 * 17    2/17/99 3:27a Jeff
 * added game checksum function to handle out-of-sync dlls
 *
 * 16    2/10/99 3:29p Jeff
 * extracted dll manager knows the difference between game hogs and
 * mission hogs
 *
 * 15    1/22/99 8:54p Jeff
 * added custom-default script overrides
 *
 * 14    1/22/99 5:15p Jeff
 * added a key to toggle osiris debug messages
 *
 * 13    1/21/99 11:15p Jeff
 * pulled out some structs and defines from header files and moved them
 * into separate header files so that multiplayer dlls don't require major
 * game headers, just those new headers.  Side effect is a shorter build
 * time.  Also cleaned up some header file #includes that weren't needed.
 * This affected polymodel.h, object.h, player.h, vecmat.h, room.h,
 * manage.h and multi.h
 *
 * 12    1/16/99 10:39a Jeff
 * added mission memory management to Osiris...only slightly tested.  Need
 * to solve game save/restore problem still
 *
 * 11    1/08/99 1:50p Jeff
 * finished support on loading scripts from hogs
 *
 * 10    1/04/99 12:23p Jeff
 * added to evt_use and support for mission module scripts
 *
 * 9     12/31/98 7:33p Jeff
 * improved Osiris timers to have unique IDs, along with adding an
 * EVT_TIMERCANCEL and a new flag to autodestruct the timer if a given
 * object handle dies.
 *
 * 8     12/18/98 10:42a Jeff
 * added timer support and auto-save memory manager for new osiris.  New
 * events evt_memrestore state added also
 *
 * 7     12/17/98 7:26p Jeff
 * added memory manager for autosaving script data
 *
 * 6     12/17/98 5:43p Jeff
 * created timer system for osiris and save restore events
 *
 * 5     12/17/98 12:08p Jeff
 * first checkin of new implementation of OSIRIS (old OSIRIS no longer
 * works)
 *
 * 4     12/16/98 10:16p Jeff
 * added functions to call level and trigger events
 *
 * 3     12/16/98 10:58a Jeff
 * checked in so Jason can use
 *
 * 2     12/14/98 11:32a Jeff
 * started work on osiris load and bind functions
 *
 * 1     12/13/98 9:10p Jeff
 *
 * $NoKeywords: $
 */

#ifndef __OSIRIS_H_
#define __OSIRIS_H_

#include "cfile.h"
#include "module.h"
#include "object_external_struct.h"
#include "osiris_share.h"

extern uint32_t Osiris_game_checksum;
extern tOSIRISModuleInit Osiris_module_init;

//	Osiris_InitModuleLoader
//	Purpose:
//		Initializes the OSIRIS module loader and handling system
extern void Osiris_InitModuleLoader(void);

//	Osiris_FreeModule
//	Purpose:
//		Removes a currently loaded module from the OSIRIS system
extern void Osiris_FreeModule(int id);

//	Osiris_ShutdownModuleLoader
//	Purpose:
//		Closes down the OSIRIS module loader and handling system
extern void Osiris_ShutdownModuleLoader(void);

//	Osiris_FindLoadedModule
//	Purpose:
//		Given the name of a module, it returns the id of a loaded OSIRIS module.  -1 if it isn't loaded.
extern int Osiris_FindLoadedModule(const std::filesystem::path &module_name);

//	Osiris_LoadLevelModule
//	Purpose:
//		Given a module name, it will attempt to load the module as a level module.  If it succeeds
//	it will return the id of the module where it has been loaded.  If the module was already loaded
//	before calling this function, it will return the id to where the module is, and will not reload
//	the module.  Returns -1 if the module does not exist.  Returns -2 if the module couldn't initialize.
//	Returns -3 if the module is not a level module. Returns -4 if no module slots are available.
extern int Osiris_LoadLevelModule(const std::filesystem::path &module_name);

//	Osiris_UnloadLevelModule
//	Purpose:
//		Instead of saving the handle returned from Osiris_LoadLevelModule() and calling
//	Osiris_UnloadModule() with that handle, you can just call this, instead of the call
//	to Osiris_UnloadModule().
extern void Osiris_UnloadLevelModule(void);

//	Osiris_LoadGameModule
//	Purpose:
//		Given a module name, it will attempt to load the module as a game module.  If it succeeds
//	it will return the id of the module where it has been loaded.  If the module was already loaded
//	before calling this function, it will return the id to where the module is, and will not reload
//	the module.  Returns -1 if the module does not exist.  Returns -2 if the module couldn't initialize.
//	Returns -3 if the module is not a game module. Returns -4 if no module slots are available.
extern int Osiris_LoadGameModule(const std::filesystem::path &module_name);

//	Osiris_UnloadModule
//	Purpose:
//		Given a module id, it will decrement the reference count to that module by one.  If the reference
//	count becomes zero, the module will be unloaded from memory.
extern void Osiris_UnloadModule(int module_id);

//	Osiris_LoadMissionModule
//	Purpose:
//		It will attempt to load the module as a game module.  If it succeeds
//	it will return the id of the module where it has been loaded.  If the module was already loaded
//	before calling this function, it will return the id to where the module is, and will not reload
//	the module.  Returns -1 if the module does not exist.  Returns -2 if the module couldn't initialize.
//	Returns -3 if the module is not a game module. Returns -4 if no module slots are available.
//	This technically doesn't load a mission module, as it should already be loaded by
//	Descent 3 prior.
extern int Osiris_LoadMissionModule(module *module_handle, const char *filename);

//	Osiris_UnloadMissionModule
//	Purpose:
//		Instead of saving the handle returned from Osiris_LoadMissionModule() and calling
//	Osiris_UnloadModule() with that handle, you can just call this, instead of the call
//	to Osiris_UnloadModule().
extern void Osiris_UnloadMissionModule(void);

//	Osiris_BindScriptsToObject
//	Purpose:
//		Call this function after an object has been created to bind all the scripts associated
//	with it to the object.  This function must be called near the end of its initialization,
//	to make sure that all fields have been filled in.  This function does not call any events.
//	This function will also load any dll's needed for its game script.
//	returns false if nothing was bound.
extern bool Osiris_BindScriptsToObject(object *obj);

//	Osiris_DetachScriptsFromObject
//	Purpose:
//		Call this function when an object is about to be destroyed.  This will unbind and remove
//	all scripts associated with that object.  This function does not call any events.
extern void Osiris_DetachScriptsFromObject(object *obj);

//	Osiris_CallEvent
//	Purpose:
//		Triggers an event for an object.  Pass in the event number and the associated
//	structure of data.  All events will be chained through the associated scripts of the
//	object (as long as they are available) in the order: custom script, level script,
//	mission script, and finally its default script.  The chain breaks if one of the scripts
//	returns false on the call to their CallInstanceEvent().
extern bool Osiris_CallEvent(object *obj, int event, tOSIRISEventInfo *data);

//	Osiris_CallLevelEvent
//	Purpose:
//		Triggers an event for a level script.  Returns true if the default action should continue
//	to process.
extern bool Osiris_CallLevelEvent(int event, tOSIRISEventInfo *data);

//	Osiris_CallTriggerEvent
//	Purpose:
//		Triggers an event for a trigger script.  Returns true if the default action should continue
//	to process.
extern bool Osiris_CallTriggerEvent(int trignum, int event, tOSIRISEventInfo *ei);

//	Osiris_ProcessTimers
//	Purpose:
//		This function checks all timers currently running, if any need to be signaled it signals them.
extern void Osiris_ProcessTimers(void);

//	Osiris_CreateTimer
//	Pupose:
//		Adds a timer to the list to be processed.  You'll receive a EVT_TIMER when the timer is signaled.
//	Returns an id to the timer, which can be used to cancel a timer. -1 on error.
extern int Osiris_CreateTimer(tOSIRISTIMER *ot);

//	Osiris_ResetAllTimers
//	Purpose:
//		Flushes all the timers
extern void Osiris_ResetAllTimers(void);

//	Osiris_CancelTimer
//	Purpose:
//		Cancels a timer thats in use, given its ID
extern void Osiris_CancelTimer(int handle);

//	Osiris_TimerExists
//	Purpose:
//		Returns true if the timer is valid
extern uint8_t Osiris_TimerExists(int handle);

//	Osiris_SaveSystemState
//	Purpose:
//		Saves the current state of the system (not the scripts!) to file
extern void Osiris_SaveSystemState(CFILE *file);

//	Osiris_RestoreSystemState
//	Purpose:
//		Restore the state of the system from file (does not restore scripts!)
extern bool Osiris_RestoreSystemState(CFILE *file);

//	Osiris_InitMemoryManager
//	Purpose:
//		Initializes the memory manager for the scripts, for buffers that the scripts want
//	automatically restored/save.
extern void Osiris_InitMemoryManager(void);

//	Osiris_CloseMemoryManager
//	Purpose:
//		Shuts down the Osiris memory manager, freeing any unfreed memory
extern void Osiris_CloseMemoryManager(void);

//	Osiris_AllocateMemory
//	Purpose:
//		Allocates a chunk of memory to be associated with a script.  It will automatically
//	save this memory to disk on game save, and will pass the pointer to this memory on EVT_RESTORE
extern void *Osiris_AllocateMemory(tOSIRISMEMCHUNK *mc);

//	Osiris_FreeMemory
//	Purpose:
//		Frees a chunk of memory that was allocated by Osiris_AllocateMemory().
extern void Osiris_FreeMemory(void *mem_ptr);

//	Osiris_FreeMemoryForScript
//	Purpose:
//		Frees all memory allocated for a given script
extern void Osiris_FreeMemoryForScript(tOSIRISSCRIPTID *sid);

//	Osiris_RestoreMemoryChunks
//	Purpose:
//		Restores the 'auto manage' from file, calls the EVT_MEMRESTORE
extern void Osiris_RestoreMemoryChunks(CFILE *file);

//	Osiris_SaveMemoryChunks
//	Purpose:
//		Saves out the 'auto manage' script memory to file
extern void Osiris_SaveMemoryChunks(CFILE *file);

//	Osiris_ExtractScriptsFromHog
//	Given the handle of a loaded hog file, this extracts all the scripts out to a temp directory
//	Pass false for the second parameter if it's a game hog (d3.hog for example)
extern int Osiris_ExtractScriptsFromHog(int library_handle, bool is_mission_hog = true);

//	Osiris_ClearExtractedScripts
//	Deletes the temp files created when the scripts where extracted from the hog
//	Pass false if you want it to remove _all_ extracted hogs...else only mission related ones
extern void Osiris_ClearExtractedScripts(bool misson_only = true);

// Initializes the Osiris Mission Memory System
extern void Osiris_InitOMMS(void);
// Shutsdown the Osiris Mission Memory System (frees all memory associated, call at end of mission)
extern void Osiris_CloseOMMS(void);

extern bool Show_osiris_debug;

#define OEM_OBJECTS 0x01
#define OEM_TRIGGERS 0x02
#define OEM_LEVELS 0x04
//	Osiris_EnableEvents
//	Purpose:
//		Enables the passed in mask of event types to be called
extern void Osiris_EnableEvents(uint8_t mask);
//	Osiris_DisableEvents
//	Purpose:
//		Disables the passed in mask of event types
extern void Osiris_DisableEvents(uint8_t mask);

//	Osiris_DisableCreateEvents
//	Purpose:
//		Disables any events involved when an object is created.  This is to be used for
//	Loading games/viewing demos, as so not to re-initialize good data.
extern void Osiris_DisableCreateEvents(void);
//	Osiris_EnablesCreateEvents
//	Purpose:
//		Enables any events involved when an object is created.  This is to be used for
//	Loading games/viewing demos, as so not to re-initialize good data. (call when done with
// Osiris_DisableCreateEvents())
extern void Osiris_EnableCreateEvents(void);

#endif
