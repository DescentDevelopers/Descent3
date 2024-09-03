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

#ifndef DLLWRAPPERS_H_
#define DLLWRAPPERS_H_

#include "pserror.h"
#include "cfile.h"
#include "gamefont.h"
#include "grdefs.h"
#include "descent.h"
#include "ddio.h"
#include "d3movie.h"
#include "program.h"
#include "object.h"
#include "hlsoundlib.h"
#include "Inventory.h"

// plays a 3dsound
int D3W_Play3dSound(int sound_index, object *cur_obj, float volume = MAX_GAME_VOLUME, int flags = 0);
int D3W_Play2dSound(int sound_index, float volume);
void D3W_TouchSound(int sound_index);

// for mprintfs
void MonoPrintf(int n, char *format, ...);
// checks a players inventory for an object type/id
bool InvCheckItem(int playernum, int type, int id);
// adds a type/id to a player inventory
bool InvAddTypeID(int playernum, int type, int id, int aux_type = -1, int aux_id = -1, int flags = 0, const char *description = NULL);
// removes a type/id from a players inventory
bool InvRemove(int playernum, int type, int id);
// returns how many of a type/id a player has in his inventory
int InvGetTypeIDCount(int playernum, int type, int id);
// Inventory Reset wrapper
void InvReset(int playernum, bool reset_all = true);
// wraps cfopen
void OpenCFILE(CFILE **handle, const std::filesystem::path &filename, const char *mode);
// wrapper for ObjGetUltimateParent
void GetUltimateParentForObject(object **parent, object *child);
// sets an object's flag dead
void SetObjectDeadFlagDLL(object *obj, bool tell_client_to_remove = false, bool tell_clients_to_play_sound = false);
// Assert for the DLLs
void assertdll(int x, const char *expression, const char *file, int line);
void RetrieveUICallback(void **fn);

// Registry database functions
bool dDatabaseRead1(const char *label, char *entry, int *entrylen);
bool dDatabaseRead2(const char *label, void *entry, int wordsize);
bool dDatabaseRead3(const char *label, bool *entry);
bool dDatabaseWrite1(const char *label, const char *entry, int entrylen);
bool dDatabaseWrite2(const char *label, int entry);
bool dObjGet(int handle, object **obj);

// Attaches 2 objects via attach points on each.  The f_used_aligned allows for an aligned connection.
// NOTE: The child always moves to the parent
bool dAttachObject(object *parent, char parent_ap, object *child, char child_ap, bool f_use_aligned = false);
// Attaches a child object to a parent object by a percent of the radius of the child.
// NOTE: The child always moves to the parent and not the reverse
bool dAttachObjectRadius(object *parent, char parent_ap, object *child, float percent_rad);
// Unattaches all children from a parent object
bool dUnattachChildren(object *parent);
// Unattaches a child from an attach point
bool dUnattachChild(object *parent, char parent_ap);
// Unattaches an object from its parent
bool dUnattachFromParent(object *child);
// Set a vector to {0,0,0}
void dvm_MakeZeroVector(vector *v);
void dvm_MakeZeroAngle(angvec *v);
void dStartFrame(int x, int y, int x2, int y2, bool clear);

// ObjSetPos, that automatically sets the OF_MOVED_THIS_FRAME
void ObjSetPosAndMarkMoved(object *obj, vector *pos, int roomnum, matrix *orient, bool f_update_attached_children);
// SetObjectDeadFlag that automatically writes out demo data
void dSetObjectDeadFlagWDemo(object *obj, bool tell_clients_to_remove = false, bool tell_clients_to_play_sound = false);
void dSetAudioTaunts(bool enable);

// Changes the ship_index for a given player
void ChangePlayerShipIndex(int pnum, int ship_index);

// adds a type/id item to the inventory (returns true on success)
bool dInven_Add(Inventory *inven, int type, int id, object *parent = NULL, int aux_type = -1, int aux_id = -1,
                int flags = 0, char *description = NULL);
// adds an object to the inventory (marked by it's objhandle)
bool dInven_AddObject(Inventory *inven, int object_handle, int flags = 0, char *description = NULL);
// adds a special cased CounterMeasure into the inventory
bool dInven_AddCounterMeasure(Inventory *inven, int id, int aux_type = -1, int aux_id = -1, int flags = 0,
                              char *description = NULL);
//	removes an item from the inventory (reduces it's count by one...if there is no more, then it goes bye-bye)
//	to remove an object that was added via objhandle, then pass the objhandle
//	in the 'type' parameter, and don't pass in an id.
bool dInven_Remove(Inventory *inven, int type, int id = -1);
// uses an item in the inventory (also reduces its count by one...if there is no more, then it goes bye-bye)
bool dInven_Use(Inventory *inven, int type, int id, object *parent = NULL);
// uses an item in the inventory (given it's objhandle).
bool dInven_UseObjHandle(Inventory *inven, int objhandle, object *parent = NULL);
// returns how many unique type/ids are in the inventory
int dInven_Size(Inventory *inven);
// returns true if there is an item in the inventory with the given type/id (or objhandle if id is -1, pass the object
// handle as the type parameter)
bool dInven_CheckItem(Inventory *inven, int type, int id = -1);
// Resets the inventory, cleaning it out
// in_game: set to true if this is being called from during gameplay
// reset_stage:
//		INVRESET_ALL:			Reset _EVERYTHING_
//		INVRESET_LEVELCHANGE:	Remove everything except those that last across levels
//		INVRESET_DEATHSPEW:		Remove everything except those that do not spew (Default)
void dInven_Reset(Inventory *inven, bool in_game, int reset_stage = INVRESET_DEATHSPEW);
// resets the position pointer in the list to the beginning
void dInven_ResetPos(Inventory *inven);
// moves the position pointer to the next inventory item
// skip : if true then skip over non-selectable items
void dInven_NextPos(Inventory *inven, bool skip = false);
// moves the position pointer to the previous inventory item
// skip : if true then skip over non-selectable items
void dInven_PrevPos(Inventory *inven, bool skip = false);
// returns the type/id of the item at the current position
// return true if it is a real object (meaning it was placed in the inventory
// via an object handle...so id will be -1)
bool dInven_GetPosTypeID(Inventory *inven, int &type, int &id);
// returns the aux type/id of the item
// return true if it is a real object (meaning it was placed in the inventory
// via an object handle...so id will be -1)
bool dInven_GetAuxPosTypeID(Inventory *inven, int &type, int &id);
// returns the description of the item at the current position
char *dInven_GetPosDescription(Inventory *inven);
// returns the icon name of the item at the current position
char *dInven_GetPosIconName(Inventory *inven);
// returns the name of the item at the current position
char *dInven_GetPosName(Inventory *inven);
// return information about the current position item
// return true if it is a real object
bool dInven_GetPosInfo(Inventory *inven, uint16_t &iflags, int &flags);
// returns the count of the item at the current position
int dInven_GetPosCount(Inventory *inven);
// returns true if the position pointer is at the begining of the inventory list
bool dInven_AtBeginning(Inventory *inven);
// returns false if the position pointer is at the end of the inventory list
bool dInven_AtEnd(Inventory *inven);
// jump right to an item in the inventory
void dInven_GotoPos(Inventory *inven, int newpos);
// jump right to an item in the inventory give it's type and id
// to jump to an item that was added via object handle, pass the object
// handle in as the type, and make id -1.
void dInven_GotoPosTypeID(Inventory *inven, int type, int id);
// uses the currently selected item
bool dInven_UsePos(Inventory *inven, object *parent = NULL);
// returns the 'index' position of the current position
int dInven_GetPos(Inventory *inven);
// checks the pos, if it is on a nonselectable item, it will move to the next selectable (NULL if none)
void dInven_ValidatePos(Inventory *inven, bool forward = true);
// returns whether an item is selectable
bool dInven_IsSelectable(Inventory *inven);
bool dInven_IsUsable(Inventory *inven);
// returns how many of an type/id is in the inventory
// for object handle items, pass the object handle in as the type and leave
// id as -1
int dInven_GetTypeIDCount(Inventory *inven, int type, int id);
// searches the inventory for the specified type/id, sets the pos to it
// for object handle items, pass the object handle in as the type and leave
// id as -1
bool dInven_FindPos(Inventory *inven, int type, int id = -1);
// gets a detailed list of information about what is in the inventory
// returns the number of items filled in.
int dInven_GetInventoryItemList(Inventory *inven, tInvenList *list, int max_amount, int *cur_sel);

#endif
