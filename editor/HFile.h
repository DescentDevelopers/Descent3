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

 * $Logfile: /DescentIII/Main/editor/HFile.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Header for HFile.cpp
 *
 * $Log: not supported by cvs2svn $
 * 
 * 6     3/22/99 6:26p Matt
 * Cleaned up error handling in cfile and editor level loads.
 * 
 * 5     4/21/98 2:40p Matt
 * Added option to show level stats
 * 
 * 4     9/17/97 11:21a Matt
 * Ripped out segment code
 * 
 * 6     4/01/97 11:00p Matt
 * Changed editor to keep a viewer object (type camera) seperate from the
 * player object.  This camera, and not the player, is now moved by
 * slewing, the C key, etc.  When going into the game, the viewer position
 * & orientation are copied to the player.  When going back to the editor,
 * the player position is copied to the viewer, and the player object is
 * reset to its start location.
 * 
 * 5     3/04/97 7:13p Samir
 * Added prototype to move player object to segment, side.  This should
 * probably be moved to another file.
 * 
 * 4     2/20/97 9:56a Matt
 * Moved some general-purpose segment code from HFile.cpp to ESegments.cpp
 * 
 * 3     2/11/97 6:44p Matt
 * Reset player when do a new level
 * Added new function: ResetFreeSegList()
 * 
 * 2     2/10/97 5:40p Matt
 * Code to handle several File menu options
 * 
 * 1     2/10/97 11:21a Matt
 *
 * $NoKeywords: $
 */

#include "d3edit.h"

//Create a new mine with one segment
void CreateNewMine();

//Load a new level
bool EditorLoadLevel(char *filename);

//Save the current level
//Returns 1 if level saved sucessfully
int EditorSaveLevel(char *filename);

//Shows the stats for a level in a messagebox, and copies to the clipboard
void ShowLevelStats();
