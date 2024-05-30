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

 * $Logfile: /DescentIII/main/Inventory.h $
 * $Revision: 46 $
 * $Date: 4/28/99 5:22a $
 * $Author: Jeff $
 *
 * Inventory control header
 *
 * $Log: /DescentIII/main/Inventory.h $
 *
 * 46    4/28/99 5:22a Jeff
 * added function to get list of inventory items, without having to go
 * through inventory one by one
 *
 * 45    4/20/99 3:06p Jeff
 * fixed inventory displaying of non-usable items
 *
 * 44    4/20/99 1:14p Samir
 * added function to determine if inventory item is usable.
 *
 * 43    4/14/99 3:56a Jeff
 * fixed case mismatch in #includes
 *
 * 42    3/02/99 4:41p Jeff
 * Fixed inventory save/load
 *
 * 41    2/25/99 8:54p Jeff
 * Inventory supports level change persistant items.  Inventory supports
 * time-out objects.  Inventory Reset changed (takes a level of reset
 * now).  Quad lasers stay across level change (single player).  Guidebot
 * bug fixed (now back in ship on level start).  Quads time out when
 * spewed.  Invulnerability and cloak powerups no longer use game
 * event/callbacks, so they can be saved in game saves (moved to
 * MakePlayerInvulnerable and MakeObjectInvisible)
 *
 * 40    2/23/99 7:37p Jeff
 * fixed reset so it doesn't remove non-spewable items...made multiplayer
 * friendly
 *
 * 39    2/14/99 4:29a Jeff
 * able to set custom descriptions for an item when adding it
 *
 * 38    2/13/99 12:37a Jeff
 * new inventory system.  Supports objects that don't die when put in (by
 * objhandles).  Also changed Inventory::Reset()
 *
 * 37    2/08/99 5:25p Jeff
 * removed all calls to MultiSendRemoveObject, incorportated into
 * SetObjectDeadFlag.  Fixes sequencing issues in multiplayer
 *
 * 36    1/18/99 2:46p Matt
 * Combined flags & flags2 fields in object struct
 *
 * 35    1/13/99 2:28a Chris
 * Massive AI, OSIRIS update
 *
 * 34    1/08/99 2:55p Samir
 * Ripped out OSIRIS1.
 *
 * 33    11/11/98 12:05p Jeff
 * made inventory.add(object *) a private function...dangerous for
 * multiplayer if called
 *
 * 32    8/19/98 2:17p Jeff
 * added a function to get the aux type/id
 *
 * 31    8/19/98 12:38p Jason
 * made countermeasure spewing work correctly
 *
 * 30    8/13/98 11:56a Jeff
 * handle new flags for inventory use
 *
 * 29    8/10/98 2:20p Jeff
 * reset takes a bool now whether to reset everything
 *
 * 28    7/10/98 7:49p Jeff
 * fixed way of getting icon info for countermeasure
 *
 * 27    7/08/98 6:01p Jeff
 * first chance at making multiplayer friendly
 *
 * 26    7/06/98 7:17p Jeff
 * countermeasure support added
 *
 * 25    7/03/98 3:10p Jeff
 * some error handling and added functions to get inventory information
 *
 * 24    4/24/98 7:09p Jeff
 * added a flag for non-useable inventory items
 *
 * 23    4/23/98 12:02p Jeff
 * added a limit to how many items you can put in your inventory
 *
 * 22    4/19/98 7:32p Jeff
 * Moved inventory wrappers to DLLWrappers
 *
 * 21    4/03/98 11:49a Jeff
 * Added another function wrapper, renamed others to fit naming convention
 *
 * 20    3/31/98 3:55p Jeff
 * Added some inventory wrappers
 *
 * 19    3/26/98 2:58p Jeff
 * Added a GetTypeIDCount function
 *
 * 18    3/23/98 5:36p Jeff
 * added a parameter to Add to specify if you want it destroyed when it's
 * added
 *
 * 17    3/20/98 2:44p Jeff
 * removed ValidatePos() from ResetPos()....ValidatePos() should be called
 * after a ResetPos() if you want to make sure you are on a selectable
 * item
 *
 * 16    2/20/98 5:02p Jeff
 * fixed bug so you can switch to a nonselectable item while playing the
 * game
 *
 * 15    2/20/98 4:56p Jeff
 * Changed inventory so it now supports non selectable items, plus made
 * the list into a circular list
 *
 * 14    2/15/98 4:49p Jeff
 * Added a reset function to clear the inventory
 *
 * 13    2/11/98 4:54p Jeff
 * Moved the inventory into the Player struct
 *
 * 12    2/09/98 4:58p Jeff
 * Connected inventory to hud
 *
 * 11    2/07/98 6:34p Jeff
 *
 * 10    2/06/98 2:04p Jeff
 * inventory system much more stable now, and works with telcom
 *
 * 9     2/05/98 7:38p Jeff
 * changed inventory system completly...now is classes...works in telcom
 * to, added use feature
 *
 * 8     2/04/98 7:42p Jeff
 * began connecting telcom to inventory
 *
 * 7     2/04/98 12:18p Jeff
 *
 * 6     2/03/98 6:15p Jeff
 *
 * 5     1/31/98 8:49p Jeff
 * Added a function to count the number of items in the inventory
 *
 * 4     1/31/98 8:12p Jeff
 * Added more inventory functions
 *
 * 3     1/30/98 7:02p Jeff
 * wrote basic internals inven system
 *
 * 2     1/29/98 3:52p Jeff
 * initial creation
 *
 * $NoKeywords: $
 */

#ifndef __INVENTORY_H__
#define __INVENTORY_H__

#include "pstypes.h"
#include "cfile.h"

struct object;

#define MAX_UNIQUE_INVEN_ITEMS 10

#define INVF_SELECTABLE 0x0003  // old 1
#define INVF_USEABLE 0x0003     // old 2
#define INVF_MISSIONITEM 0x0004 // this item lasts across levels
#define INVF_NOREMOVEONUSE 0x0008
#define INVF_VISWHENUSED 0x0010
#define INVF_OBJECT 0x0020        // this item isn't a type/id...but an objhandle
#define INVF_NOTSPEWABLE 0x0040   // this object should not spew...stay in inventory after death
#define INVF_TIMEOUTONSPEW 0x0080 // this object/item times out when it's spewed

// Inventory reset stage settings
#define INVRESET_ALL 0
#define INVRESET_LEVELCHANGE 1
#define INVRESET_DEATHSPEW 2

struct tInvenList {
  bool selectable;
  char *hud_name;
  int amount;
};

struct tInvenInfo {
  int count;
  char *description;
  char *icon_name;
  char *name;
  int flags;
  uint16_t iflags; // Inventory item flags
  int type;
  int id;
};

struct inven_item {

  int type;    // what type of object this is... robot, weapon, hostage, powerup, fireball
               // if INVF_OBJECT, this is the object handle
  int otype;   // countermeasure powerup type
  int id;      // which form of object...which powerup, robot, etc.
               // if INVF_OBJECT, this is -1
  int oid;     // countermeasure powerup id
  int flags;   // misc flags
  uint16_t pad2; // keep alignment

  int count; // how many of this type/id (not INVF_OBJECT)

  char *description;
  char *icon_name;
  char *name;

  uint16_t iflags; // Inventory item flags

  inven_item *next, *prev; // pointer to next inventory item
};

class Inventory {
public:
  Inventory();
  ~Inventory();
  // adds a type/id item to the inventory (returns true on success)
  bool Add(int type, int id, object *parent = NULL, int aux_type = -1, int aux_id = -1, int flags = 0,
           const char *description = NULL);
  // adds an object to the inventory (marked by it's objhandle)
  bool AddObject(int object_handle, int flags = 0, const char *description = NULL);
  // adds a special cased CounterMeasure into the inventory
  bool AddCounterMeasure(int id, int aux_type = -1, int aux_id = -1, int flags = 0, const char *description = NULL);
  // removes an item from the inventory (reduces it's count by one...if there is no more, then it goes bye-bye)
  bool Remove(int type, int id = -1);
  // uses an item in the inventory (also reduces its count by one...if there is no more, then it goes bye-bye)
  bool Use(int type, int id, object *parent = NULL);
  bool Use(int objhandle, object *parent = NULL);
  // returns how many unique type/ids are in the inventory
  int Size(void);
  // returns true if there is an item in the inventory with the given type/id (or objhandle if id is -1)
  bool CheckItem(int type, int id = -1);
  // Resets the inventory, cleaning it out
  // in_game: set to true if this is being called from during gameplay
  // reset_stage:
  //		INVRESET_ALL:			Reset _EVERYTHING_
  //		INVRESET_LEVELCHANGE:	Remove everything except those that last across levels
  //		INVRESET_DEATHSPEW:		Remove everything except those that do not spew (Default)
  void Reset(bool in_game, int reset_stage = INVRESET_DEATHSPEW);
  // resets the position pointer in the list to the beginning
  void ResetPos(void);
  // moves the position pointer to the next inventory item
  void NextPos(bool skip = false);
  // moves the position pointer to the previous inventory item
  void PrevPos(bool skip = false);
  // returns the type/id of the item at the current position
  // return true if it is a real object
  bool GetPosTypeID(int &type, int &id);
  // returns the aux type/id of the item
  // return true if it is a real object
  bool GetAuxPosTypeID(int &type, int &id);
  // returns the description of the item at the current position
  char *GetPosDescription(void);
  // returns the icon name of the item at the current position
  char *GetPosIconName(void);
  // returns the name of the item at the current position
  char *GetPosName(void);
  // return information about the current position item
  // return true if it is a real object
  bool GetPosInfo(uint16_t &iflags, int &flags);
  // returns the count of the item at the current position
  int GetPosCount(void);
  // returns true if the position pointer is at the begining of the inventory list
  bool AtBeginning(void);
  // returns false if the position pointer is at the end of the inventory list
  bool AtEnd(void);
  // jump right to an item in the inventory
  void GotoPos(int newpos);
  void GotoPos(int type, int id);
  // uses the currently selected item
  bool UsePos(object *parent = NULL);
  // returns the 'index' position of the current position
  int GetPos(void);
  // checks the pos, if its on a nonselectable item it will move to the next selectable (NULL if none)
  void ValidatePos(bool forward = true);
  // returns whether an item is selectable
  bool IsSelectable(void);
  bool IsUsable(void);
  // returns how many of an type/id is in the inventory
  int GetTypeIDCount(int type, int id);
  // searches the inventory for the specified type/id, sets the pos to it
  bool FindPos(int type, int id = -1);
  // saves the inventory to the file (returns number of bytes written)
  int SaveInventory(CFILE *file);
  // restores the inventory from file (returns number of bytes read)
  int ReadInventory(CFILE *file);
  // gets a detailed list of information about what is in the inventory
  // returns the number of items filled in.
  int GetInventoryItemList(tInvenList *list, int max_amount, int *cur_sel);

private:
  // adds an item to the inventory (returns true on success)
  bool AddObjectItem(int otype, int oid, int oauxt, int oauxi, int flags, const char *description = NULL);
  void RemoveNode(inven_item *node);
  // sends a request to the server to use a particular item in the inventory
  void SendRequestToServerToUse(int type, int id = -1);
  inven_item *FindItem(int type, int id = -1);
  inven_item *root;
  int count;
  inven_item *pos; // used when looking through inventory
};

void InventorySwitch(bool forward);
// moves to the next/prev item in the counter measures list (forward==TRUE means forward, forward==FALSE means go
// backwards)
void CounterMeasuresSwitch(bool forward);
// Checks for an object in any of the players inventorys and removes it
void InventoryRemoveObject(int objhandle);

#endif
