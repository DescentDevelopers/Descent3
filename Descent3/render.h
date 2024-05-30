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

 * $Logfile: /DescentIII/main/render.h $
 * $Revision: 38 $
 * $Date: 10/21/99 9:29p $
 * $Author: Jeff $
 *
 * Header for render.c
 *
 * $Log: /DescentIII/main/render.h $
 *
 * 38    10/21/99 9:29p Jeff
 * B.A. Macintosh code merge
 *
 * 37    8/10/99 11:19p Gwar
 *
 * 36    4/18/99 5:42a Chris
 * Added the FQ_IGNORE_RENDER_THROUGH_PORTALS flag
 *
 * 35    4/15/99 12:19p Jason
 * made mirrors more robust and able to have multiple mirrored faces in
 * the same room (as long as they are all on the same plane)
 *
 * 34    3/31/99 5:15p Matt
 * Added checkboxes on the room tab to control rendering of faces based on
 * shell flag.
 *
 * 33    2/17/99 1:05p Jason
 * revamped object/face/terrain selection code
 *
 * 32    2/09/99 12:10p Jason
 * rewriting indoor engine
 *
 * 31    10/16/98 1:54p Kevin
 * Changes for Demo Beta 4
 *
 * 30    10/12/98 3:01p Jeff
 * added vsync_enable global
 *
 * 29    10/08/98 3:11p Matt
 * Constant for all outline modes didn't include sky bit.
 *
 * 28    10/04/98 2:35p Matt
 * Added debug options to limit rendering
 *
 * 27    10/03/98 11:21p Matt
 * Added system to seperately control outline mode for mine, terrain, sky,
 * & objects
 *
 * 26    9/24/98 12:57p Jason
 * more state limited optimizations
 *
 * 25    9/22/98 3:55p Samir
 * ifdef out stuff for non-debug version.
 *
 * 24    8/27/98 5:19p Jason
 * added first rev of reflected surfaces
 *
 * 23    8/19/98 2:19p Jeff
 * moved detail globals to a struct
 *
 * 22    8/18/98 11:38a Jason
 * fixed polymodel fog lighting
 *
 * 21    8/13/98 6:56p Jason
 * made objects foggable correctly
 *
 * 20    6/19/98 6:42p Jason
 * made specular mapping a config detail item
 *
 * 19    5/25/98 3:46p Jason
 * added better light glows
 *
 * 18    5/19/98 12:27p Jason
 * cleaned up some 3d stuff
 *
 * 17    4/30/98 6:46p Jason
 * more framerate testing
 *
 * 16    4/30/98 3:40p Jason
 * framerate optimizations
 *
 * 15    2/03/98 11:43p Matt
 * Added defaults for RenderMine() parameters
 *
 * 14    1/12/98 3:34p Jason
 * sped up indoor rendering by clipping faces against portals
 *
 * 13    1/06/98 1:28p Matt
 * Cleaned up interfaces to rendering routines, deleted unused code, etc.
 *
 * 12    1/02/98 6:40p Matt
 * User renderer library (instead of viewport) functions to draw lines and
 * to set and read pixels.  Also made FindRoomFace() work without
 * Render_viewport.
 *
 * 11    12/29/97 5:44p Samir
 * Took out references to grViewport and old 2d library.
 *
 * 10    12/01/97 4:20p Jason
 * shadow and lighting changes
 *
 * 9     11/04/97 6:25p Matt
 * Added code to render all external rooms for editing purposes
 *
 * 8     9/16/97 5:50p Matt
 * Changed conditional for debug code
 *
 * 7     9/16/97 4:09p Jason
 * implemented software zbuffer
 *
 * 6     9/11/97 3:14p Matt
 * Added code to render floating triggers in the editor
 *
 * 5     9/04/97 5:23p Matt
 *
 * 4     9/04/97 12:12p Jason
 * added lightmap visibility
 *
 * 3     7/23/97 6:36p Matt
 * Changed Outline_mode & Lighting_on to be type bool
 *
 * 12    6/12/97 1:19p Matt
 * Added version of rotate_list that takes list of int point numbers
 *
 * 11    5/27/97 3:30p Matt
 * Added editor button to toggle lighting
 *
 * 10    5/08/97 7:46p Matt
 * Added called_from_terrain parm for RenderMine()
 *
 * 9     5/07/97 1:30p Jason
 * more changes for terrain/mine integration
 *
 * 8     5/05/97 4:47p Jason
 * made terrain and mine rendering work without explicit calls to
 * g3_Startframe in their main function calls
 * This allows us to integrate mine/terrain engines nicely
 *
 * 7     4/29/97 5:15p Jason
 * added mirrored textures...I don't know if we'll keep them in though
 *
 * 6     2/26/97 6:00p Matt
 * Renamed 3d lib structs for D3 naming convention
 *
 * $NoKeywords: $
 */
#ifndef RENDER_H
#define RENDER_H

#include "3d.h"

// Variables for debug/test
#if (defined(_DEBUG) || defined(NEWEDITOR))

#define SRF_NO_SHELL 1     // don't render the shell
#define SRF_NO_NON_SHELL 2 // don't render the non-shell

extern int Render_portals;
extern bool Lighting_on;   // If true, draw w/ normal lighting, else draw full brightness
extern uint8_t Outline_mode; // Controls outline drawing.  See constants below
extern uint8_t Shell_render_flag;
extern bool Render_floating_triggers; // If true, render the floating triggers
extern bool Outline_lightmaps;
extern bool Use_software_zbuffer;
extern bool Render_all_external_rooms; // If true, draw all the outside rooms
extern bool Render_one_room_only;
extern bool Render_inside_only;

#else
#define Lighting_on 1
#define Outline_mode 0
#endif
extern int16_t use_opengl_1555_format; // DAJ

#ifndef RELEASE
extern int Mine_depth;
#endif
// Macro for checking Outline mode
#define OUTLINE_ON(flag) ((Outline_mode & (flag + OM_ON)) == (flag + OM_ON))

// Constants for outline mode
#define OM_ON 1
#define OM_MINE 2
#define OM_TERRAIN 4
#define OM_OBJECTS 8
#define OM_SKY 16
#define OM_ALL (OM_MINE + OM_TERRAIN + OM_OBJECTS + OM_SKY)

extern float Fog_zone_start, Fog_zone_end;
extern bool DoBumpmaps;
extern bool Render_mirror_for_room;
extern bool Vsync_enabled;

extern float Room_light_val;
extern int Room_fog_plane_check;
extern float Room_fog_distance;
extern float Room_fog_eye_distance;
extern vector Room_fog_plane, Room_fog_portal_vert;

struct face;

struct fog_portal_data {
  int16_t roomnum;
  float close_dist;
  face *close_face;
};

extern fog_portal_data Fog_portal_data[];

extern int Num_fogged_rooms_this_frame;

// Sets fogzone start and end points
void SetFogZoneStart(float z);
void SetFogZoneEnd(float z);

struct room;

// For sorting our textures in state limited environments
struct state_limited_element {
  int facenum;
  int sort_key;
};

#define MAX_STATE_ELEMENTS 8000
extern state_limited_element State_elements[MAX_STATE_ELEMENTS];

extern g3Point SolidFogPoints[], AlphaFogPoints[];

// Takes a face and adds the appropriate vertices for drawing in the fog zone
// Returns number of points in new polygon
// New polygon points are in FogPoints array
int FogBlendFace(g3Point **src, int nv, int *num_solid, int *num_alpha);

// Draws the mine, starting at a the specified room
// The rendering surface must be set up, and g3_StartFrame() must have been called
// Parameters:	viewer_roomnum - what room the viewer is in
//					flag_automap - if true, flag segments as visited when rendered
//					called_from_terrain - set if calling this routine from the terrain renderer
void RenderMine(int viewer_roomnum, int flag_automap = 0, int called_from_terrain = 0);

// Finds what room & face is visible at a given screen x & y
// Everything must be set up just like for RenderMineRoom(), and presumably is the same as
// for the last frame rendered (though it doesn't have to be)
// Parameters:	x,y - the screen coordinates
//					start_roomnum - where to start rendering
//					roomnum,facenum - these are filled in with the found values
//					if room<0, then an object was found, and the object number is -room-1
// Returns:		1 if found a room, else 0
int FindRoomFace(int16_t x, int16_t y, int start_roomnum, int *roomnum, int *facenum);

// finds what room,face,lumel is visible at a given screen x & y
// Everything must be set up just like for RenderMineRoom(), and presumably is the same as
// for the last frame rendered (though it doesn't have to be)
// Parameters:	x,y - the screen coordinates
//					start_roomnum - where to start rendering
//					roomnum,facenum,lumel_num - these are filled in with the found values
// Returns:		1 if found a room, else 0
int FindLightmapFace(int16_t x, int16_t y, int start_roomnum, int *roomnum, int *facenum, int *lumel_num);

// This is needed for small view cameras
// It clears the facing array so that it is recomputed
void ResetFacings();

// Renders all the lights glows for this frame
void RenderLightGlows();

// Called before a frame starts to render - sets all of our light glows to decreasing
void PreUpdateAllLightGlows();

// Called after a frame has been rendered - slowly morphs our light glows into nothing
void PostUpdateAllLightGlows();

// Resets our light glows to zero
void ResetLightGlows();

// Gets the dynamic light value for this position
void GetRoomDynamicScalar(vector *pos, room *rp, float *r, float *g, float *b);

// Sorts our texture states using the quicksort algorithm
void SortStates(state_limited_element *state_array, int cellcount);

// Sets up fog if this room is fogged
void SetupRoomFog(room *rp, vector *eye, matrix *orient, int viewer_room);

// Draw the specified face
// Parameters:	rp - pointer to the room the face is un
//				facenum - which face in the specified room
void RenderFace(room *rp, int facenum);

// Renders a specular face
void RenderSpecularFacesFlat(room *rp);

// Renders fog faces for a room
void RenderFogFaces(room *rp);

// Builds a list of mirror faces for each room and allocs memory accordingly
void ConsolidateMineMirrors();

extern int Num_specular_faces_to_render, Num_fog_faces_to_render;

#endif
