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

 * $Logfile: /DescentIII/Main/editor/EDVARS.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Misc. variables for the editor
 *
 * $Log: not supported by cvs2svn $
 * 
 * 22    9/04/98 12:29p Matt
 * Added marked edge & vertex in the editor
 * 
 * 21    6/17/98 12:30p Samir
 * don't draw wireframe when moving an object.
 * 
 * 20    6/09/98 6:11p Matt
 * Removed unused variable
 * 
 * 19    1/26/98 4:33p Samir
 * Added ability to copy object ids.
 * 
 * 18    1/21/98 12:32p Matt
 * Revamped viewer system
 * 
 * 17    1/02/98 6:38p Matt
 * Got rid of Render_viewport
 * 
 * 16    12/29/97 5:45p Samir
 * Adder reference to Render viewport.
 * 
 * 15    12/02/97 5:30p Samir
 * New file dialog interface implemented.
 * 
 * 14    12/02/97 4:43p Samir
 * Repaired EDVARS.cpp
 * 
 * 13    12/01/97 6:06p Samir
 * Implemented new FileDialog tracker system.
 * 
 * 12    9/17/97 1:01p Matt
 * Ripped out segment code
 * 
 * 11    9/17/97 11:16a Matt
 * Ripped out segment code
 * 
 * 10    9/11/97 5:38p Jason
 * initial door coding for room engine
 * 
 * 9     9/06/97 3:16p Matt
 * Added current portal system
 * 
 * 8     9/04/97 4:39p Matt
 * Added includes needed as a result of removing includes from d3edit.h
 * 
 * 7     8/29/97 5:23p Matt
 * Added Placed_group
 * 
 * 6     8/18/97 6:59p Matt
 * Implemented Place Room/Attach room system
 * 
 * 5     7/22/97 7:07p Matt
 * Cleaned up D3EditState, moving some vars in and some out, and renaming
 * and changing a few others
 * 
 * 4      !/97 7:21p Matt
 * Added var for current level script
 * 
 * 13    4/02/97 8:07p Matt
 * Added variable & button to toggle object move state
 * 
 * 12    4/02/97 3:25p Jason
 * got rid of WV_changed and made TV_changed update the textured view and
 * flag "World_changed"
 * 
 * 11    3/31/97 5:57p Matt
 * Revamped mine update flags
 * 
 * 10    3/31/97 3:47p Matt
 * Added code to keep track of, render, and display current vertex.
 * 
 * 9     3/17/97 12:24a Matt
 * Added warning segs list
 * 
 * 8     3/12/97 3:27p Matt
 * Added scrap buffer
 * 
 * 7     3/11/97 2:54p Samir
 * Add some vars useful for rotation of segments.
 * 
 * 6     2/28/97 6:37p Matt
 * Added variable & toggle button for box selection mode
 * 
 * 5     2/28/97 12:11p Matt
 * Added vars for selected seg list
 * 
 * 4     2/24/97 5:51p Matt
 * Added vars for Markedsegp & Markedside
 * 
 * 3     2/10/97 5:36p Matt
 * Added a couple more vars
 * 
 * 2     2/07/97 6:01p Matt
 * Added Mine_changed
 * 
*/

#include "d3edit.h"
#include "room.h"

//Current room, face, edge, & vert.
room *Curroomp=NULL;
int Curface=0,Curedge=0,Curvert=0,Curportal=-1;

//Marked room & face
room *Markedroomp=NULL;
int Markedface=0,Markededge=0,Markedvert=0;

//Placed room info
int Placed_door=-1;
int Placed_room=-1;
group *Placed_group = NULL;
int Placed_room_face;
float Placed_room_angle;
vector Placed_room_origin;
matrix Placed_room_orient;
matrix Placed_room_rotmat;
vector Placed_room_attachpoint;
room *Placed_baseroomp;
int Placed_baseface;

//flags for the wireframe and textured views changed
int TV_changed=1;

//Set this flag if a new world is loaded/created
int New_mine = 1;			//init to 1 for initial redraw

//Set this when the mine has changed
int World_changed=0;

//Set this when the editor state (but not the world itself) has changed
int State_changed=0;

//Set this when the viewer (i.e., player) has moved
int Viewer_moved=0;

//Set this when the editor viewpoint has changed
int Edview_changed=0;

//Set this when an object has moved
int Object_moved=0;

//Vars for the list of selected rooms
int N_selected_rooms=0;
int Selected_rooms[MAX_ROOMS];

//Flag for if mine has changed (& thus needs to be saved)
int Mine_changed;

//Current object 
int Cur_object_index=0;

//How does object movement work?
int Object_move_mode=REL_OBJECT;

// The scrap buffer
group *Scrap=NULL;

//Pointer to the scripts for this level
char *Current_level_script=NULL;

// What mode we're currently in
int Editor_view_mode = VM_MINE;

//The ID of the most recent viewer object (not counting room view)
int Editor_viewer_id = -1;

//	current directories for a number of file dialogs
char Current_files_dir[_MAX_PATH] = "";
char Current_bitmap_dir[_MAX_PATH] = "";
char Current_room_dir[_MAX_PATH] = "";
char Current_scrap_dir[_MAX_PATH] = "";
char Current_model_dir[_MAX_PATH] = "";
char Current_sounds_dir[_MAX_PATH] = "";
char Current_weapon_dir[_MAX_PATH] = "";

// Object ID clipboard
int Copied_object_id = 0;
