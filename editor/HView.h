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

 * $Logfile: /DescentIII/Main/editor/HView.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Header for hview.cpp
 *
 * $Log: not supported by cvs2svn $
 *
 * 9     4/29/99 11:51p Matt
 * Improved viewer move functions.
 *
 * 8     1/21/98 12:33p Matt
 * Revamped viewer system
 *
 * 7     1/19/98 10:04a Matt
 * Removed unused function
 *
 * 6     9/17/97 11:51a Samir
 * BIG SEGMENT RIPOUT
 *
 * 5     8/21/97 6:02p Matt
 * Added prototype for FindViewerObject()
 *
 * 4     8/04/97 12:43p Matt
 * Added SetViewerFromRoomFace()
 *
 * 2     4/01/97 11:00p Matt
 * Changed editor to keep a viewer object (type camera) separate from the
 * player object.  This camera, and not the player, is now moved by
 * slewing, the C key, etc.  When going into the game, the viewer position
 * & orientation are copied to the player.  When going back to the editor,
 * the player position is copied to the viewer, and the player object is
 * reset to its start location.
 *
 * 1     3/17/97 2:25p Matt
 *
 * $NoKeywords: $
 */

#include "room.h"

struct object;

// Set the viewer in the specified room facing the specified face
// If room_center is true, put viewer at the center of the room facing the face
// If room_center is false, put the viewer directly in front of the selected face
// If the room is external, put the viewer a distance away from the room,
// facing either the center (if room_center is true) or the specified face
void SetViewerFromRoomFace(room *roomp, int facenum, bool room_center);

// Returns the number (not the id) of the current viewer, in the range 0..MAX_VIEWERS
int GetViewerNum();

// Creates a new viewer object.  Copies position & orientation from the current viewer
void CreateNewViewer();

// Select next viewer
void SelectNextViewer();

// Deletes the current viewer object
void DeleteViewer();

// Sets the viewer object for the editor, creating if not already in mine
void SetEditorViewer();

// Finds a specific viewer object it one exists
// Parameters:	id - which viewer id
// Returns:		object number of a viewer object, or -1 if none
int FindViewerObject(int id);

// Finds a viewer object if one exists.
// Starts looking at the specified id and searches through all possible ids
// Parameters:	id - which viewer id
//					view_mode - if -1, find any viewer, else find one that matches view mode
// Returns:		object number of a viewer object, or -1 if none
int FindNextViewerObject(int id, int view_mode);

// Moves the room viewer to the origin, if there is a room viewer
void ResetRoomViewer();
