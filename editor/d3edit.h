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

 * $Logfile: /DescentIII/Main/editor/d3edit.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:37 $
 * $Author: kevinb $
 *
 * <insert description of file here>
 *
 * $Log: not supported by cvs2svn $
 * 
 * 45    3/30/99 6:24p Matt
 * Added new error system, where instead of putting up an error message, a
 * function returns a failure code & returns an error message that the
 * caller can display if it wants.
 * 
 * 44    1/29/99 12:48p Matt
 * Rewrote the doorway system
 * 
 * 43    12/01/98 11:17p Matt
 * Added menu option to disable drawing objects in the wireframe view
 * 
 * 42    10/14/98 4:34p Matt
 * Added a macro to make EditorMessageBox() do OutrageMessageBox() if
 * EDITOR is defined.
 * 
 * 41    9/04/98 3:16p Samir
 * control whether to allow joystick slewing.
 * 
 * 40    9/04/98 12:29p Matt
 * Added marked edge & vertex in the editor
 * 
 * 39    8/27/98 6:42p Chris
 * Added the goal keypad and the start of the matcen keypad
 * 
 * 38    6/17/98 12:30p Samir
 * don't draw wireframe when moving an object.
 * 
 * 37    6/15/98 4:00p Jason
 * replaced monochromatic polymodel lighting with rgb lighting
 * 
 * 36    6/10/98 12:18p Matt
 * Added hack to fix bug until Samir can fix it for real.
 * 
 * 35    2/10/98 1:58p Chris
 * Improved the path system
 * 
 * 34    1/28/98 5:07p Samir
 * Object modeless settings added.
 * 
 * 33    1/26/98 4:33p Samir
 * Added ability to copy object ids.
 * 
 * 32    1/21/98 12:32p Matt
 * Revamped viewer system
 * 
 * 31    1/02/98 6:35p Matt
 * Got rid of Render_viewport
 * 
 * 30    12/29/97 5:44p Samir
 * Took out references to grViewport and old 2d library.
 * 
 * 29    12/19/97 11:25a Samir
 * g3_StartFrame and g3_EndFrame replaced by EditorStartFrame and
 * EditorEndFrame
 * 
 * 28    12/02/97 5:30p Samir
 * New file dialog interface implemented.
 * 
 * 27    12/01/97 6:06p Samir
 * Implemented new FileDialog tracker system.
 * 
 * 26    10/28/97 3:41p Jason
 * added "use hemicubes" option
 * 
 * 25    9/29/97 12:09p Jason
 * added functionality to doorway system
 * 
 * 24    9/17/97 12:58p Samir
 * Got rid of Curve Keypad.
 * 
 * 23    9/17/97 11:40a Matt
 * Ripped out segment code
 * 
 * 22    9/17/97 11:16a Matt
 * Ripped out segment code
 * 
 * 21    9/11/97 5:46p Jason
 * first pass at getting doors to work with room engine
 * 
 * 20    9/08/97 4:05p Samir
 * Fixed some warnings and took out extranneous includes to windows.h in
 * important headers.
 * 
 * 19    9/06/97 3:15p Matt
 * Ripped out current trigger, added current portal
 * 
 * 18    9/04/97 4:44p Matt
 * Declare room, group, & segment types here so we don't need to include
 * those header files, reducing compile times when one of them changes.
 * 
 * 17    8/29/97 5:25p Matt
 * Added Placed_group
 * 
 * 16    8/29/97 3:31p Samir
 * Fixed problem with room keypad not showing up.  had to remove segment
 * tab constant.
 * 
 * 15    8/29/97 1:50p Samir
 * Added object_move_axis to d3editstate.
 * 
 * 14    8/18/97 6:59p Matt
 * Implemented Place Room/Attach room system
 * 
 * 13    8/12/97 4:09p Matt
 * Added vars for current building & clutter
 * 
 * 12    8/01/97 11:59a Samir
 * Added a preference for debugging in fullscreen.
 * 
 * 11    7/29/97 12:26p Jason
 * added gamefile field to d3editstate
 * 
 * 10    7/24/97 2:56p Matt
 * Got rid of D3EditState fields that duplicated other variables
 * 
 * 9     7/22/97 7:07p Matt
 * Cleaned up D3EditState, moving some vars in and some out, and renaming
 * and changing a few others
 * 
 * 8     7/17/97 7:21p Matt
 * Added var for current level script
 * 
 * 51    6/30/97 1:29p Jason
 * added yet more room functions (netherspace stuff)
 * 
 * 50    6/27/97 3:04p Jason
 * added cool room stuff
 * 
 * 49    6/27/97 12:18p Samir
 * Added RoomKeypadDialog
 * 
 * 48    6/16/97 2:34p Jason
 * added 3dfx support
 * 
 * 47    6/05/97 6:10p Jason
 * added support for zbuffering and bilinear filtering
 * 
 * 46    6/05/97 3:38p Samir
 * Added megacell keypad.
 * 
 * 45    6/05/97 2:52p Jason
 * added megacell functions
 * 
 * 44    5/27/97 5:41p Jason
 * added toggling of opengl renderer
 * 
 * 43    5/21/97 3:39p Jason
 * added terrain editing stuff
 * 
 * 42    5/21/97 12:29p Jason
 * added the ability to draw terrain dots in mine view
 * 
 * 41    4/30/97 5:43p Jason
 * added ability to toggle whether or not you want to run in 16 bit mode
 * 
 * 40    4/08/97 10:41a Jason
 * implemented path following interface
 * 
 * 39    4/04/97 3:15p Samir
 * Added paths keypad, removed player keypad since it's joined with object
 * keypad.
 * 
 * 38    4/02/97 8:07p Matt
 * Added variable & button to toggle object move state
 * 
 * 37    4/02/97 3:24p Jason
 * got rid of WV_changed and made TV_changed update the textured view and
 * flag "World_changed"
 * 
 * 36    3/31/97 5:57p Matt
 * Revamped mine update flags
 * 
 * 35    3/31/97 3:47p Matt
 * Added code to keep track of, render, and display current vertex.
 * 
 * 34    3/31/97 3:13p Jason
 * added ship,weapon,and sounds to d3edit and zeroed it out upon startup
 * 
 * 33    3/26/97 6:32p Samir
 * Changed order of keypads.
 * 
 * 32    3/26/97 4:38p Samir
 * Added SplashMessage function.
 * 
 * 31    3/25/97 2:50p Jason
 * added current_robot field
 * 
 * 30    3/24/97 12:33p Samir
 * Added current trigger type.
 * 
 * 29    3/20/97 4:22p Samir
 * Added Segment Sizing mode and side sizing.
 * 
 * 28    3/17/97 12:24a Matt
 * Added warning segs list
 * 
 * 27    3/12/97 3:27p Matt
 * Added scrap buffer
 * 
 * 26    3/11/97 2:54p Samir
 * Add some vars useful for rotation of segments.
 * 
 * 25    3/07/97 4:18p Jason
 * implemented terrain functionality in the editor
 * 
 * 24    3/05/97 1:01p Samir
 * Added current doorway global
 * 
 * 23    3/05/97 11:49a Samir
 * Added doorway and terrain keypad tabs.
 * 
 * 22    3/04/97 7:13p Samir
 * Added current_door and Current_trigger globals.
 * 
 * 21    2/28/97 6:37p Matt
 * Added variable & toggle button for box selection mode
 * 
 * 20    2/28/97 12:11p Matt
 * Added vars for selected seg list
 * 
 * 19    2/24/97 5:51p Matt
 * Added vars for Markedsegp & Markedside
 * 
 * 18    2/19/97 5:39p Matt
 * Made EditorStatus() take printf-type args, as the D1/D2 function did.
 * 
 * 17    2/18/97 6:21p Samir
 * Added EditorStatus function to print messages on the status bar.
 * 
 * 16    2/18/97 12:46p Samir
 * Added current object type editor state.
 * 
 * 15    2/10/97 5:41p Matt
 * Added a couple more vars
 * 
 * 14    2/07/97 6:00p Matt
 * Added Mine_changed
 * 
 * 13    2/06/97 11:59a Matt
 * Added externs for Cursegp,Curside,Curedge
 * 
 * 12    2/05/97 11:13a Samir
 * Added D3windowed preference
 * 
 * 11    2/03/97 5:57p Matt
 * Added missing include
 * 
 * 10    1/30/97 7:48p Jason
 * added support for powerup pages
 * 
 * 9     1/23/97 4:57p Samir
 * Added a bunch of window position vars.
 * 
 * 8     1/21/97 12:52p Samir
 * Added texture and wireframe screen dims to d3editstate struct.
 * 
 * 7     1/20/97 3:51p Samir
 * Took out EDITOR define.  This should be put in the MAKEFILE for
 * INFERNO.INI style implementation.
 *
 * $NoKeywords: $
 */

/*	D3 Editor Globals */

#ifndef _D3EDIT_H
#define _D3EDIT_H

#include "pstypes.h"
#include "descent.h"
#include "vecmat.h"

#include <stdlib.h>

//For putting up editor messageboxes in the main body (& should be in the editor code too, probably)
#ifdef EDITOR
#define EditorMessageBox OutrageMessageBox
#endif

//Define group & room structs so we don't have to include group.h & room.h
struct group group;
struct room room;

const int	TEXSCREEN_WIDTH		= 512,	// Texture screen base width and height
			TEXSCREEN_HEIGHT	= 384,
			WIRESCREEN_WIDTH	= 640,	// Wirefrane screen base width and height
			WIRESCREEN_HEIGHT	= 480;

const int	EDITOR_RESOLUTION_X	= 1024,	// Used to try to make editor resolution independent
			EDITOR_RESOLUTION_Y	= 768;

int CALC_PIXELS_WITH_ASPECTX(int pixels);
int CALC_PIXELS_WITH_ASPECTY(int pixels);


//	keypad constants
enum {	TAB_TEXTURE_KEYPAD,
			TAB_MEGACELL_KEYPAD,
			TAB_TERRAIN_KEYPAD,
			TAB_OBJECTS_KEYPAD,
			TAB_ROOM_KEYPAD,
			TAB_DOORWAY_KEYPAD,
			TAB_TRIGGER_KEYPAD,
			TAB_LIGHTING_KEYPAD,
			TAB_PATHS_KEYPAD,
			TAB_LEVEL_KEYPAD,
			TAB_MATCEN_KEYPAD};

//Constants for d3edit_state variables
enum {IN_WINDOW,ACROSS_EDGE};									//values for box_selection_mode
enum {REL_OBJECT,REL_VIEWER};									//values for object_move_mode
enum {GM_WINDOWED,GM_FULLSCREEN_SW,GM_FULLSCREEN_HW};	//values for game_render_mode

class grSurface;
class grViewport;

//Structure to store various editor state & preference values
struct d3edit_state {

	// Values for current item in the various dialogs
	int	texdlg_texture;						// current texture in texdialog
	int	current_obj_type;					  	// current type of object
	int	current_powerup;						// current powerup id
	int	current_door;							// current door in door page dialog
	int	current_robot;						  	// current robot in robot page dialog
	int	current_ship;							// current ship in ship page dialog
	int	current_sound;						  	// current sound in sound page dialog
	int	current_weapon;						// current weapon in weapon page dialog
	int	current_path;							// currently selected path for a robot to follow
	int	current_node;							// currently selected node of preceding path
	int	current_megacell;					  	// currently selected megacell
	int	current_room;							// currently selected room
	int	current_gamefile;						// currently selected gamefile
	int	current_building;						// currently selected building
	int	current_clutter;						// currently selected clutter

	//	Values for the different editor windows
	bool	texscr_visible;						// is texture mine view up?
	int	texscr_x, texscr_y, 
			texscr_w, texscr_h;					// dims of floating texture mine view

	bool	wirescr_visible;					  	// is wireframe model up?
	int	wirescr_x, wirescr_y, 
			wirescr_w, wirescr_h;				// dims of floating wireframe model

	bool	keypad_visible;						// is keypad visible?
	int	keypad_current;						// which keypad tab are we on?
	bool	float_keypad_moved;				  	// has floating keypad moved?
	int	float_keypad_x, float_keypad_y, 
			float_keypad_w, float_keypad_h;	// floating keypad width and height, x, y
	int	objmodeless_x, objmodeless_y;		// object modeless list x and y.
	bool	objmodeless_on;						// is modeless on?

	bool	tile_views;								// tile or floating view windows, keypad

	//Values for terrain renderer
	bool	terrain_dots;						  	// show terrain dots?
	bool	terrain_flat_shade;				  	// flat shade terrain?

	//Misc preferences
	int	game_render_mode;						// what mode to we play the game in?  See constants above.
	bool	randomize_megacell;				  	// randomize when placing a megacell?
	int	box_selection_mode; 					// How editor box selection works.  See constants above.
	int	object_move_mode;						// How object movements works.  See constants above.
	int	object_move_axis;						// This is the axis on which objects move with mouse.
	bool	fullscreen_debug_state;				// do we allow for fullscreen debugging?
	bool	hemicube_radiosity;
	float	node_movement_inc;
	int	texture_display_flags;				// which textures to display on the texture tab
	float texscale;								// the scalar for moving texture UVs
	bool	joy_slewing;							// shall we allow joystick slewing?
	bool	objects_in_wireframe;				// should we draw objects in the wireframe view?

};

//	Editor.cpp:: Current state of the editor UI.
extern d3edit_state D3EditState;

//	Editor.cpp:: Surface describing the actual desktop where the editor is running.
extern grSurface *Desktop_surf;			

//flags for the textured views changed
extern int TV_changed;

//Set this flag if a new world is loaded/created
extern int New_mine;

//Set this when the mine has changed
extern int World_changed;

//Set this when the editor state (but not the world itself) has changed
extern int State_changed;

//Set this when the viewer (i.e., player) has moved
extern int Viewer_moved;

//Set this when an object has moved
extern int Object_moved;

//Set this when the editor viewpoint has changed
extern int Edview_changed;

//Current room & face
extern room *Curroomp;
extern int Curface,Curedge,Curvert;
extern int Curportal;

//Current object 
extern int Cur_object_index;

//Marked room & face
extern room *Markedroomp;
extern int Markedface,Markededge,Markedvert;

//Placed room info
extern int Placed_room;
extern group *Placed_group;
extern int Placed_room_face;
extern int Placed_door;
extern float Placed_room_angle;
extern vector Placed_room_origin;
extern matrix Placed_room_orient;
extern vector Placed_room_attachpoint;
extern matrix Placed_room_rotmat;
extern room *Placed_baseroomp;
extern int Placed_baseface;

//Vars for the list of selected rooms
extern int N_selected_rooms;
extern int Selected_rooms[];

//Flag for if mine has changed (& thus needs to be saved)
extern int Mine_changed;

//	Current trigger in mine displayed in trigger dialog
extern int Current_trigger;

// The scrap buffer
extern group *Scrap;

//Pointer to the scripts for this level
extern char *Current_level_script;

// What mode we're currently in
enum {VM_MINE,VM_TERRAIN,VM_ROOM,NUM_VIEW_MODES};
extern int Editor_view_mode;

//The ID of the most recent viewer object (not counting room view)
extern int Editor_viewer_id;

//	current directories for file dialogs.
extern char Current_files_dir[_MAX_PATH];
extern char Current_bitmap_dir[_MAX_PATH];
extern char Current_scrap_dir[_MAX_PATH];
extern char Current_room_dir[_MAX_PATH];
extern char Current_model_dir[_MAX_PATH];
extern char Current_sounds_dir[_MAX_PATH];
extern char Current_weapon_dir[_MAX_PATH];

//	object id clipboard.
extern int Copied_object_id;


//	FUNCTIONS
void EditorStatus(const char *format,...);
void SplashMessage(const char *format, ...);
void StartEditorFrame(grViewport *vp, vector *view_vec, matrix *id_mat, float zoom);
void EndEditorFrame();

//Set the editor error message.  A function that's going to return a failure
//code should call this with the error message.
void SetErrorMessage(const char *fmt,...);

//Get the error message from the last function that returned failure
const char *GetErrorMessage();

#endif
