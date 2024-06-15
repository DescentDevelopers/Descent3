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

 * $Logfile: /DescentIII/Main/editor/selectedroom.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:39 $
 * $Author: kevinb $
 *
 * Header for selected.cpp
 *
 * $Log: not supported by cvs2svn $
 *
 * 2     7/31/97 3:31p Jason
 * added functions to rotate portal rooms
 *
 * 1     7/31/97 1:05p Jason
 *
 * $NoKeywords: $
 */

// Returns true if a room is in the selected list
int IsRoomSelected(int roomnum);

// Adds a room to the selected list, if it's not already there
void AddRoomToSelectedList(int roomnum);

// Removes a room from the selected list, if it's there
void RemoveRoomFromSelectedList(int roomnum);

// Empties the selected list
void ClearRoomSelectedList();

// Adds to selected list if not already in there, else removes from list
// Returns:	1 if room was selected, 0 if un-selected
int ToggleRoomSelectedState(int roomnum);

// Add all the connected room to the selected list
// Parameters:	roomnum - the starting room
// Returns:		the number of rooms added to the list
int SelectConnectedRooms(int roomnum);

// Save the (user's) selected list so that an internal function can use it
// You must call RestoreSelectedList() when you're done with the selected list
void SaveRoomSelectedList();

// Restore a previously-saved selected list.  You must have previously called SaveSelectedList()
void RestoreRoomSelectedList();
