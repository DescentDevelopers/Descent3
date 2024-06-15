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

 * $Logfile: /DescentIII/Main/editor/selectedroom.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:39 $
 * $Author: kevinb $
 *
 * Functions for dealing with selected list
 *
 * $Log: not supported by cvs2svn $
 *
 * 10    4/16/99 5:18p Matt
 * Don't free selected list memory if none was allocated.
 *
 * 9     4/11/99 2:41a Jeff
 * had to include stdafx.h for neweditor
 *
 * 8     4/07/99 7:07p Gwar
 * added #define NEWEDITOR checks
 *
 * 7     1/21/99 11:15p Jeff
 * pulled out some structs and defines from header files and moved them
 * into seperate header files so that multiplayer dlls don't require major
 * game headers, just those new headers.  Side effect is a shorter build
 * time.  Also cleaned up some header file #includes that weren't needed.
 * This affected polymodel.h, object.h, player.h, vecmat.h, room.h,
 * manage.h and multi.h
 *
 * 6     10/08/98 4:24p Kevin
 * Changed code to comply with memory library usage. Always use mem_malloc
 * , mem_free and mem_strdup
 *
 * 5     12/23/97 1:33p Samir
 * Added pserror.h
 *
 * 4     9/04/97 4:39p Matt
 * Added includes needed as a result of removing includes from d3edit.h
 *
 * 3     8/29/97 5:26p Matt
 * Fixed name of  ClearRoomSelectedList()
 *
 * 2     7/31/97 3:31p Jason
 * added functions to rotate portal rooms
 *
 * 1     7/31/97 10:35a Jason
 *
 * $NoKeywords: $
 */

#include "selectedroom.h"

#ifdef NEWEDITOR
#include "../neweditor/stdafx.h"
#include "../neweditor/globals.h"
#else
#include "d3edit.h"
#endif

#include "room.h"
#include "pserror.h"
#include "mem.h"

// Returns true if a room is in the selected list
int IsRoomSelected(int roomnum) {
  int i;

  for (i = 0; i < N_selected_rooms; i++)
    if (Selected_rooms[i] == roomnum)
      return 1;

  return 0;
}

// Adds a room to the selected list, if it's not already there
void AddRoomToSelectedList(int roomnum) {
  if (!IsRoomSelected(roomnum)) {
    Selected_rooms[N_selected_rooms++] = roomnum;
    State_changed = 1;
  }
}

// Removes a room from the selected list, if it's there
void RemoveRoomFromSelectedList(int roomnum) {
  int i;

  for (i = 0; i < N_selected_rooms; i++)
    if (Selected_rooms[i] == roomnum) {
      int j;
      for (j = i; j < N_selected_rooms - 1; j++)
        Selected_rooms[j] = Selected_rooms[j + 1];
      N_selected_rooms--;
      State_changed = 1;
      return;
    }
}

// Empties the selected list
void ClearRoomSelectedList() {
  N_selected_rooms = 0;
  State_changed = 1;
}

// Adds to selected list if not already in there, else removes from list
// Returns:	1 if room was selected, 0 if un-selected
int ToggleRoomSelectedState(int roomnum) {
  int i;

  State_changed = 1;

  for (i = 0; i < N_selected_rooms; i++)
    if (Selected_rooms[i] == roomnum) {
      int j;
      for (j = i; j < N_selected_rooms - 1; j++)
        Selected_rooms[j] = Selected_rooms[j + 1];
      N_selected_rooms--;
      return 0; // room was un-selected
    }

  // not found, so add to list
  Selected_rooms[N_selected_rooms++] = roomnum;

  return 1; // room was selected
}

// Add all the connected room to the selected list
// Parameters:	room - the starting room
// Returns:		the number of rooms added to the list
int SelectConnectedRooms(int roomnum) {
  int count;
  int s;

  if (IsRoomSelected(roomnum)) // this room already selected?
    return 0;

  // Add this rooom to selected list
  Selected_rooms[N_selected_rooms++] = roomnum;
  count = 1;

  State_changed = 1;

  // Now add this room's children
  for (s = 0; s < Rooms[roomnum].num_portals; s++) {
    if (Rooms[roomnum].portals[s].croom != -1)
      count += SelectConnectedRooms(Rooms[roomnum].portals[s].croom);
  }

  return count;
}

static int *Save_selected_rooms;
static int N_save_selected_rooms = -1; //-1 means empty list

// Save the (user's) selected list so that an internal function can use it
// You must call RestoreSelectedList() when you're done with the selected list
void SaveRoomSelectedList() {
  int i;

  if (N_save_selected_rooms != -1) { // already have saved list?
    Int3();                          //..yes, error
    return;
  }

  N_save_selected_rooms = N_selected_rooms;

  if (!N_save_selected_rooms) // are there any rooms in the list?
    return;                   //..no, so don't bother to save them

  Save_selected_rooms = (int *)mem_malloc(N_save_selected_rooms * sizeof(*Save_selected_rooms));

  ASSERT(Save_selected_rooms != NULL);

  for (i = 0; i < N_selected_rooms; i++)
    Save_selected_rooms[i] = Selected_rooms[i];
}

// Restore a previously-saved selected list.  You must have previously called SaveSelectedList()
void RestoreRoomSelectedList() {
  int i;

  if (N_save_selected_rooms == -1) { // no save list?
    Int3();                          //..then generate error
    return;
  }

  N_selected_rooms = N_save_selected_rooms;

  N_save_selected_rooms = -1; // say no saved list

  if (!N_selected_rooms) // are there any rooms in the list?
    return;              //..no, so don't restore them

  for (i = 0; i < N_selected_rooms; i++)
    Selected_rooms[i] = Save_selected_rooms[i];

  mem_free(Save_selected_rooms); // free up memory
}
