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

 * $Logfile: /DescentIII/Main/editor/drawworld.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:37 $
 * $Author: kevinb $
 *
 * Code to draw wireframe world for editor
 *
 * $Log: not supported by cvs2svn $
 * 
 * 36    4/22/99 5:17p Matt
 * Changed the and & or fields in the grCodes struct to cc_and & cc_or,
 * since the former names didn't work with some compilers.
 * 
 * 35    3/01/99 2:58p Dan
 * Set lighting type before wireframe search, because sometimes it was set
 * wrong & the search wouldn't work.  (MattT on Dan's machine)
 * 
 * 34    2/28/99 6:18p Matt
 * Commented-out Int3()
 * 
 * 33    2/13/99 6:29p Matt
 * Always check current room when searching, because if the render radius
 * is low it was getting checked.
 * 
 * 32    2/03/99 1:10p Matt
 * Changed the paletted room current faces to be stored in a seperate
 * array, instead of in the room structure.
 * 
 * 31    12/01/98 11:17p Matt
 * Added menu option to disable drawing objects in the wireframe view
 * 
 * 30    10/04/98 1:25p Matt
 * 
 * 29    9/04/98 12:29p Matt
 * Added marked edge & vertex in the editor
 * 
 * 28    9/01/98 12:04p Matt
 * Ripped out multi-face portal code
 * 
 * 27    5/20/98 3:48p Matt
 * Added suport for variable-depth face selection in wireframe view.
 * 
 * 26    5/14/98 12:56p Jason
 * changes to help lower memory usage
 * 
 * 25    2/16/98 1:08p Matt
 * Draw the current vertex in the wireframe view
 * 
 * 24    1/29/98 5:50p Matt
 * Changed old camera object type to be viewer object (for editor), and
 * now camera objects will just be for game cameras.
 * 
 * 23    1/12/98 3:34p Jason
 * sped up indoor rendering by clipping faces against portals
 * 
 * 22    12/19/97 5:23p Samir
 * Clear before frame in Wireframe.
 * 
 * 21    12/19/97 12:26p Samir
 * Replaced g3_StartFrame with StartEditorFrame
 * 
 * 20    10/15/97 5:20p Jason
 * did a HUGE overhaul of the bitmap system
 * 
 * 19    9/29/97 12:09p Jason
 * added functionality to doorway system
 * 
 * 18    9/22/97 5:21p Matt
 * Got box selection working for room engine
 * 
 * 17    9/17/97 1:01p Matt
 * Ripped out segment code
 * 
 * 16    9/17/97 11:34a Matt
 * Ripped out segment code
 * 
 * 15    9/11/97 7:08p Matt
 * Added code to draw floating trigger faces
 * 
 * 14    9/09/97 3:58p Matt
 * Ripped out segment engine renderer
 * 
 * 13    9/06/97 3:16p Matt
 * Added current portal system
 * 
 * 12    9/04/97 4:39p Matt
 * Added includes needed as a result of removing includes from d3edit.h
 * 
 * 11    8/29/97 5:35p Matt
 * Massive changes:  ripped out segment code, make current & selection
 * rooms draw special, made facing & nonfacing edges draw in different
 * colors.
 * 
 * 10    8/22/97 9:31a Matt
 * Terrain portals now draw in blue.
 * 
 * 9     8/20/97 10:39a Matt
 * If a frame hasn't been previously rendered when the findroomface
 * function is called, handle it gracefully.
 * 
 * 8     8/18/97 6:59p Matt
 * Implemented Place Room/Attach room system
 * 
 * 7     8/05/97 10:48a Matt
 * Draw marked face
 * 
 * 6     7/29/97 12:16p Matt
 * Made wireframe seach work for rooms & faces
 * 
 * 5     7/24/97 6:11p Matt
 * Created symbolic constant for default zoom, and used it every place
 * that specifies zoom
 * 
 * 4     7/22/97 7:07p Matt
 * Cleaned up D3EditState, moving some vars in and some out, and renaming
 * and changing a few others
 * 
 * 3     7/16/97 4:18p Matt
 * Changes to work with changed 3d draw functions
 * 
 * 22    5/21/97 12:29p Jason
 * added the ability to draw terrain dots in mine view
 * 
 * 21    5/15/97 3:27a Jason
 * implemented terrain magnification
 * 
 * 20    5/13/97 7:47p Matt
 * Added code to compress mine before saving.
 * Cleaned up code dealing with num vertices & segments.
 * 
 * 19    5/12/97 11:41a Jason
 * made game work (default) to 16bit no mip maps mode
 * Saves us alot of memory
 * 
 * 18    5/01/97 6:06p Matt
 * Deleted commented-out code
 * 
 * 17    4/30/97 3:15p Jason
 * changes to support both 8bit and 16bit rendering in software
 * 
 * 16    4/10/97 4:39p Jason
 * Made selection manager stuff work with terrain topographical map
 * 
 * 15    4/01/97 10:51p Matt
 * Draw viewer based on object type
 * 
 * 14    4/01/97 5:18p Matt
 * Draw objects as spheres in wireframe view
 * 
 * 13    3/20/97 11:55a Jason
 * changes for terrain editing/drawing
 * 
 * 12    3/17/97 11:59a Matt
 * Added code to draw warning segments
 * 
 * 11    3/17/97 11:10a Jason
 * added wireframe terrain view
 * 
 * 10    2/28/97 6:38p Matt
 * Fixed box selection bug, & added supprt for two box selection modes
 * 
 * 9     2/28/97 4:00p Matt
 * Added code to handle the list of selected segments
 * 
 * 8     2/28/97 9:33a Matt
 * Improved code that checked for segments by writing a pixel and reading
 * that pixel after rendering.
 * 
 * 7     2/26/97 5:59p Matt
 * Renamed 3d lib structs for D3 naming convention
 * 
 * 6     2/24/97 5:57p Matt
 * Restored code to draw marked seg in different color
 * 
 * 5     2/24/97 12:23p Matt
 * Changed the name of some segment struct elements
 * 
 * 4     2/19/97 5:37p Matt
 * Added code to set curseg from clicking in wireframe window
 * 
 * 3     2/07/97 6:02p Matt
 * Enabled code that draws current seg, side, & edge
 *
 * $NoKeywords: $
 */

#include "drawworld.h"

#include <stdlib.h>

#include "3d.h"

#include "descent.h"
#include "d3edit.h"
#include "render.h"
#include "selectedroom.h"
#include "terrain.h"
#include "texture.h"
#include "gametexture.h"
#include "object.h"
#include "erooms.h"
#include "group.h"

int Draw_all_rooms = 1;

//the viewport we're currently drawing to
grViewport *Draw_viewport;

//Colors for rooms/edges
#define	FACING_COLOR		GR_RGB( 198, 198, 198)
#define	NOTFACING_COLOR	GR_RGB( 125, 125, 125)	//GR_RGB( 158, 158, 158)
#define	PLACED_COLOR		GR_RGB( 255,   0, 255)
#define	SELECTED_COLOR		GR_RGB( 255, 166,   0)
#define	TERR_PORTAL_COLOR	GR_RGB(   0,   0, 255)
#define	CURROOM_COLOR		GR_RGB( 255, 255, 255)
#define	CURFACE_COLOR		GR_RGB( 255, 255,   0)
#define	CUREDGE_COLOR		GR_RGB(   0, 255,   0)
#define	MARKEDFACE_COLOR	GR_RGB(   0, 255, 255)
#define	MARKEDEDGE_COLOR	GR_RGB(   0, 150, 150)
#define  CURPORTAL_COLOR	GR_RGB( 200, 150, 255)
#define	FLOAT_TRIG_COLOR	GR_RGB( 255, 100, 100)

//Unused for now
#define	FOUND_COLOR			GR_RGB(   0, 121, 182)
#define	WARNING_COLOR		GR_RGB( 255,   0,   0)
#define	MARKEDROOM_COLOR	GR_RGB(   0, 255,   0)

//Colors for objects
#define	ROBOT_COLOR			GR_RGB( 255,   0,   0)		//a robot
#define	PLAYER_COLOR		GR_RGB(   0, 255,   0)		//a player object
#define	VIEWER_COLOR		GR_RGB( 100,   0, 100)		//a viewer
#define  POWERUP_COLOR		GR_RGB(   0,   0, 255)		//a powerup
#define	MISCOBJ_COLOR		GR_RGB(   0, 100, 100)		//some other object
#define  CAMERA_COLOR		GR_RGB( 255, 255,   0)		//a camera

//vars for WireframeFindRoomFace()
static int		Search_mode=0;             //if 1, searching for point, if 2, searching for box
static int		Search_x,Search_y;
static int		Search_find_mode;
static float	Search_min_dist = 0.0;
static int		Search_left,Search_top,Search_right,Search_bot;
static int		Search_roomnum,Search_facenum;
static vector	Search_viewer;					//used to determine closest room
static int		Found_roomnum;
static int		Found_facenum;
static float	Found_dist;
static grViewport	*last_vp;
static vector		last_viewer_position;
static vector		last_viewer_target;
static matrix		last_view_orient;
static int			last_start_seg;
static int			last_start_room;
static float		last_rad;
static float		last_zoom;


extern vector *TS_View_position;
extern matrix *TS_View_matrix;
extern matrix View_matrix;
void PreRotateTerrain();
void Terrain_start_frame (vector *eye,matrix *view_orient);

void DrawTerrainPoints (vector *view_pos,matrix *view_orient)
{
	g3Point terr_point;
	int i;

	Terrain_start_frame (view_pos,&View_matrix);

	for (i=0;i<TERRAIN_DEPTH*TERRAIN_WIDTH;i++)
	{
		GetPreRotatedPoint (&terr_point.p3_vec,i%TERRAIN_WIDTH,i/TERRAIN_WIDTH,Terrain_seg[i].ypos);
		terr_point.p3_flags=0;
		g3_CodePoint (&terr_point);
		if (!terr_point.p3_codes)
		{
			g3_ProjectPoint (&terr_point);
			Draw_viewport->setpixel (GR_RGB(255,255,255),terr_point.p3_sx,terr_point.p3_sy);
		}

	}

	
}

//edge types - lower number types have precedence
#define ET_EMPTY		255	//this entry in array is empty
#define ET_FACING		0		//this edge on a facing face
#define ET_NOTFACING	1		//this edge on a non-facing face
#define N_EDGE_TYPES	2		//how many edge types there are

struct seg_edge {
	union {
		struct {int16_t v0,v1;};
    int32_t vv;
	};
	uint16_t	type;
};

#define MAX_EDGES (MAX_VERTS_PER_ROOM*2)

seg_edge edge_list[MAX_EDGES];

int16_t	used_list[MAX_EDGES];	//which entries in edge_list have been used
int n_used;

int edge_list_size;		//set each frame

#define HASH(a,b)  ((a*5+b) % edge_list_size)

#define swap(a,b) do {int t; t=(a); (a)=(b); (b)=t;} while (0)

//finds edge, filling in edge_ptr. if found old edge, returns index, else return -1
int FindEdge(int v0,int v1,seg_edge **edge_ptr)
{
  int32_t vv;
	int16_t hash,oldhash;
	int ret;

	vv = (v1<<16) + v0;

	oldhash = hash = HASH(v0,v1);

	ret = -1;

	while (ret==-1) {

		if (edge_list[hash].type == ET_EMPTY) ret=0;
		else if (edge_list[hash].vv == vv) ret=1;
		else {
			if (++hash==edge_list_size) hash=0;
			if (hash==oldhash) Error("Edge list full!");
		}
	}

	*edge_ptr = &edge_list[hash];

	if (ret == 0)
		return -1;
	else
		return hash;

}

//adds an edge to the edge list
void AddEdge(int v0,int v1,uint8_t type)
{
	int found;

	seg_edge *e;

//mprintf(0, "Verts = %2i %2i, type = %i ", v0, v1, type);
	if (v0 > v1) swap(v0,v1);

	found = FindEdge(v0,v1,&e);

	if (found == -1) {
		e->v0 = v0;
		e->v1 = v1;
		e->type = type;
		used_list[n_used] = e-edge_list;
		n_used++;
	} else {
		if (type < e->type)
			e->type = type;
	}
}

//If searching for a point, see if this room draws at the search x,y position, and if so, 
//if it's the closest to the viewer.  If searching for a box, see if any of the room points are
//inside the box.
void CheckRoom(room *rp)
{
	uint8_t	codes_and = 0xff;
	int vn;

	//Rotate all the points
	for (vn=0;vn<rp->num_verts;vn++)
		codes_and &= g3_RotatePoint((g3Point *)&World_point_buffer[vn],&rp->verts[vn]);

	if (! codes_and) {		//all off screen?

		if (Search_mode == 1) {			//searching for a point
			int fn;
			ddgr_color oldcolor;
			face *fp;

			rend_SetLighting(LS_NONE);
			rend_SetTextureType (TT_FLAT);

			for (fn=0,fp=rp->faces;fn<rp->num_faces;fn++,fp++) {
				g3Point *vert_list[MAX_VERTS_PER_FACE];

				codes_and = 0xff;
				for (vn=0;vn<fp->num_verts;vn++) {
					vert_list[vn] = (g3Point *)&World_point_buffer[fp->face_verts[vn]];
					codes_and &= vert_list[vn]->p3_codes;
				}

				if (! codes_and) {

				  	Draw_viewport->setpixel(GR_RGB(0,0,0),Search_x,Search_y);
					oldcolor = Draw_viewport->getpixel(Search_x,Search_y); 	//will be different in 15/16-bit color
					rend_SetFlatColor (GR_RGB(255,255,255));

					g3_CheckAndDrawPoly(fp->num_verts,vert_list,0,NULL,NULL);
	
				  	if (Draw_viewport->getpixel(Search_x,Search_y) != oldcolor) {
						vector t;
						float dist;
	
						ComputeCenterPointOnFace(&t,rp,fn);
	
						dist = vm_VectorDistance(&t,&Search_viewer);
	
						if (Search_find_mode == FM_SPECIFIC) {
							if ((ROOMNUM(rp) == Search_roomnum) && (fn == Search_facenum)) {
								Found_roomnum = ROOMNUM(rp);
								Found_facenum = fn;
								Found_dist = dist;
							}
						}
						else if ((Found_roomnum == -1  ||  dist < Found_dist) && (dist > Search_min_dist)) {
							Found_roomnum = ROOMNUM(rp);
							Found_facenum = fn;
							Found_dist = dist;
						}
					}
				}
			}
  		}
		else if (Search_mode == 2) {				//searching in a box
			int v;

			for (v=0;v<rp->num_verts;v++) {
				int x,y;

				g3_ProjectPoint((g3Point *)&World_point_buffer[v]);

				x = Round(World_point_buffer[v].p3_sx);
				y = Round(World_point_buffer[v].p3_sy);

				if ((x >= Search_left) && (y >= Search_top) && (x <= Search_right) && (y <= Search_bot)) {
					if ((D3EditState.box_selection_mode==ACROSS_EDGE) || (v==rp->num_verts-1)) {
						AddRoomToSelectedList(ROOMNUM(rp));
						Found_roomnum++;	//doubles as a counter
						break;				//no need to check rest of verts
					}
				}
				else													//this point not in window
					if (D3EditState.box_selection_mode == IN_WINDOW)	//seg not entirely in window
						break;										//..so stop checking this room
			}
		}
		else
			Int3();		//unknown Search_mode
		
	}
}


void DrawLine(ddgr_color color,int pnum0,int pnum1)
{
	g3Point pnt1,pnt2;
	
	
	pnt1=*((g3Point *)&World_point_buffer[pnum0]);
	pnt2=*((g3Point *)&World_point_buffer[pnum1]);

	g3_DrawLine(color,&pnt1,&pnt2);
}

void DrawEdges(int room_color=-1);

void DrawEdges(int room_color)
{
	int i,type;
	seg_edge *e;
	ddgr_color edge_colors[] = {FACING_COLOR, NOTFACING_COLOR};

	for (type=N_EDGE_TYPES-1;type>=0;type--) {
		for (i=0;i<n_used;i++) {
			e = &edge_list[used_list[i]];
			if (e->type == type)
				DrawLine((room_color==-1)?edge_colors[type]:room_color,e->v0,e->v1);
		}
	}
}

void DrawAllRooms(vector *view_target,float rad);
void DrawRoom(room *rp,int room_type = -1);

//Finds the closest room:face to the viewer at a given x,y screen position in the wireframe view
//Parameters:	vp - the viewport we're checking.  Must be the same viewport as the last wireframe view rendered
//					x,y - the screen coordinates clicked on
//					roomnum,facenum - pointers to variables to be filled in
//					find_mode - see values in header file
//Returns:		true if found a room/face
bool WireframeFindRoomFace(grViewport *vp,int x,int y,int *roomnum,int *facenum,int find_mode)
{
	if (last_vp == NULL)
		return 0;

	if (vp != last_vp) {
		//Int3();
		return 0;
	}

	Draw_viewport = vp;

	vp->clear();

	StartEditorFrame(vp,&last_viewer_position,&last_view_orient,last_zoom);

	Search_mode = 1;
	Search_find_mode = find_mode;
	Search_x = x; Search_y = y;
	Search_viewer = last_viewer_position;
	Search_min_dist = (find_mode == FM_NEXT)?Found_dist:0.0;
	Search_roomnum = *roomnum;
	Search_facenum = *facenum;
	Found_roomnum = -1;
	Found_facenum = -1;

	if (Editor_view_mode == VM_ROOM) {
		ASSERT(D3EditState.current_room != -1);
		CheckRoom(&Rooms[D3EditState.current_room]);
	}
	else {
		DrawAllRooms(&last_viewer_target,last_rad);
		CheckRoom(Curroomp);
	}

	EndEditorFrame();

	Search_mode = 0;

	if (Found_roomnum != -1) {
		*roomnum = Found_roomnum;
		*facenum = Found_facenum;
		return 1;
	}
	else
		return 0;
}

//Adds all the rooms that have a vertex inside of a given screen box to the selected list
//Parameters:	vp - the viewport we're checking.  Must be the same viewport as the last wireframe view rendered
//					left,top,right,bot - the screen coordinates of the box
//Returns:		the number of rooms found
int SelectRoomsInBox(grViewport *vp,int left,int top,int right,int bot)
{

//TEMP: The caller doesn't know the viewport, so we set it here
vp = last_vp;

	if (vp != last_vp) {
		Int3();
		return -1;
	}

	Draw_viewport = vp;

	vp->clear();

	StartEditorFrame(vp,&last_viewer_position,&last_view_orient,last_zoom);

	Search_mode = 2;
	Search_left = left; Search_top = top; Search_right = right; Search_bot = bot;
	Search_viewer = last_viewer_position;
	Found_roomnum = 0;			//we use this as a counter

	DrawAllRooms(&last_viewer_target,last_rad);

	EndEditorFrame();

	Search_mode = 0;

	return Found_roomnum;

}

//Resets the edge list
void ResetEdgeList(int size)
{
	int i;
	
	edge_list_size = __min(size,MAX_EDGES);		//make maybe smaller than max

	for (i=0; i<edge_list_size; i++) {
		edge_list[i].type = ET_EMPTY;
	}

	n_used = 0;
}

//	Draw a room
void DrawRoom(room *rp,int room_color)
{
	int	fn,vn;
	face	*fp;
	int	edge_type;

	ResetEdgeList(MAX_EDGES);

	//Rotate all the points
	for (vn=0;vn<rp->num_verts;vn++)
         g3_RotatePoint((g3Point *)&World_point_buffer[vn],&rp->verts[vn]);

	for (fn=0,fp=rp->faces;fn<rp->num_faces;fn++,fp++) {
		if (fp->flags & FF_FLOATING_TRIG) {
			for (vn=0;vn<fp->num_verts;vn++)
				DrawLine(FLOAT_TRIG_COLOR,fp->face_verts[vn],fp->face_verts[(vn+1)%fp->num_verts]);
			DrawLine(FLOAT_TRIG_COLOR,fp->face_verts[0],fp->face_verts[2]);
			DrawLine(FLOAT_TRIG_COLOR,fp->face_verts[1],fp->face_verts[3]);
		}
		else if (fp->portal_num == -1) {
			bool facing = g3_CheckNormalFacing(&rp->verts[fp->face_verts[0]],&fp->normal);
			for (vn=0;vn<fp->num_verts;vn++) {
				if (facing)
					edge_type = ET_NOTFACING;
				else
					edge_type = ET_FACING;

				AddEdge(fp->face_verts[vn],fp->face_verts[(vn+1)%fp->num_verts],edge_type);
			}
		}
	}

	DrawEdges(room_color);

	//Now draw the terrain portals
	for (int p=0;p<rp->num_portals;p++) {
		portal *pp = &rp->portals[p];
		if (pp->croom == -1) {			//terrain portal
			fp = &rp->faces[pp->portal_face];
			for (vn=0;vn<fp->num_verts;vn++)
				DrawLine(TERR_PORTAL_COLOR,fp->face_verts[vn],fp->face_verts[(vn+1)%fp->num_verts]);
		}
	}

}

//	Draw a room rotated and placed in space
void DrawRoomRotated(room *rp,vector *rotpoint,matrix *rotmat,vector *placepoint,int room_color)
{
	int	fn,vn;
	face	*fp;

	ResetEdgeList(MAX_EDGES);
	
	//Rotate all the points
	for (vn=0;vn<rp->num_verts;vn++) {
		vector tv;
		tv = (rp->verts[vn] - *rotpoint) * *rotmat + *placepoint;
		g3_RotatePoint((g3Point *)&World_point_buffer[vn],&tv);
	}

	for (fn=0,fp=rp->faces;fn<rp->num_faces;fn++,fp++)
		for (vn=0;vn<fp->num_verts;vn++)
			AddEdge(fp->face_verts[vn],fp->face_verts[(vn+1)%fp->num_verts],ET_FACING);

	DrawEdges(room_color);
}

void DrawRoomFace(ddgr_color color,room *rp,int facenum)
{
	face *fp = &rp->faces[facenum];
	g3Codes cc;
	int i;

	//Rotate the points in this face
   cc.cc_and = 0xff;  cc.cc_or = 0;
	for (i=0;i<fp->num_verts;i++) {
		int vertnum = fp->face_verts[i];
		g3_RotatePoint((g3Point *)&World_point_buffer[vertnum],&rp->verts[vertnum]);
      cc.cc_and &= World_point_buffer[vertnum].p3_codes;
      cc.cc_or  |= World_point_buffer[vertnum].p3_codes;
	}

	if (! cc.cc_and)		//if not all off screen
		for (i=0;i<fp->num_verts;i++)
			DrawLine(color,fp->face_verts[i],fp->face_verts[(i+1)%fp->num_verts]);
}

void DrawFaceEdge(ddgr_color color,room *rp,int facenum,int edgenum)
{
	face *fp = &rp->faces[facenum];
	int v0,v1;

	v0 = fp->face_verts[edgenum];
	v1 = fp->face_verts[(edgenum+1)%fp->num_verts];
	
	g3_RotatePoint((g3Point *)&World_point_buffer[v0],&rp->verts[v0]);
	g3_RotatePoint((g3Point *)&World_point_buffer[v1],&rp->verts[v1]);

	if (! (World_point_buffer[v0].p3_codes & World_point_buffer[v1].p3_codes))
		DrawLine(color,v0,v1);
}

//Draw the objects in a room as spheres
void DrawRoomObjects(room *rp)
{
	int objnum;

	if (! D3EditState.objects_in_wireframe)
		return;

	for (objnum=rp->objects;objnum!=-1;objnum=Objects[objnum].next) {
		object *obj = &Objects[objnum];
		g3Point sphere_point;
		ddgr_color color;

  		switch (obj->type) {
  			case OBJ_POWERUP:	color = POWERUP_COLOR; break;
  			case OBJ_ROBOT:	color = ROBOT_COLOR; break;
  			case OBJ_PLAYER:	color = PLAYER_COLOR; break;
  			case OBJ_VIEWER:	color = VIEWER_COLOR; break;
  			case OBJ_CAMERA:	color = CAMERA_COLOR; break;
  			default:				color = MISCOBJ_COLOR; break;
  		}

		if (obj->type==OBJ_DOOR)		// don't draw doors
			continue;

		g3_RotatePoint(&sphere_point,&obj->pos);
		g3_DrawSphere(color,&sphere_point,obj->size);
	}
}

void DrawAllRooms(vector *view_target,float rad)
{
	int r;
	room *rp;

	for (r=0,rp=Rooms; r<=Highest_room_index; r++,rp++) {
		if (rp->used && (vm_VectorDistance(&rp->verts[0],view_target) < rad)) {
			if (Search_mode)
				CheckRoom(rp);
			else {
				DrawRoom(rp);
				DrawRoomObjects(rp);
			}
		}
	}
}

#define CROSS_WIDTH  8.0
#define CROSS_HEIGHT 8.0

void DrawVertBox(vector *v,ddgr_color color)
{
  	//Draw a box at the marked vert
  	g3Point p0;
  	uint8_t c0 = g3_RotatePoint(&p0,v);
  	if (! c0) {		//on screen?
  		g3_ProjectPoint(&p0);
  		rend_SetFlatColor(color);
  		rend_DrawLine(p0.p3_sx-CROSS_WIDTH,p0.p3_sy,p0.p3_sx,p0.p3_sy-CROSS_HEIGHT);
  		rend_DrawLine(p0.p3_sx,p0.p3_sy-CROSS_HEIGHT,p0.p3_sx+CROSS_WIDTH,p0.p3_sy);
  		rend_DrawLine(p0.p3_sx+CROSS_WIDTH,p0.p3_sy,p0.p3_sx,p0.p3_sy+CROSS_HEIGHT);
  		rend_DrawLine(p0.p3_sx,p0.p3_sy+CROSS_HEIGHT,p0.p3_sx-CROSS_WIDTH,p0.p3_sy);
  	}
}

void DrawWorld(grViewport *vp,vector *view_target,matrix *view_orient,float view_dist,int start_room,float rad)
{
	vector viewer_position;
	float zoom = D3_DEFAULT_ZOOM;

	ASSERT(Curroomp != NULL);

	viewer_position = *view_target - (view_orient->fvec * view_dist);

	//save for possible use by find function
	last_vp = vp;
	last_viewer_position = viewer_position;
	last_viewer_target = *view_target;
	last_view_orient = *view_orient;
	last_start_room = start_room;
	last_rad = rad;
	last_zoom = zoom;

	Draw_viewport = vp;

	vp->clear();
	StartEditorFrame(vp,&viewer_position,view_orient,zoom);

	if (Editor_view_mode == VM_ROOM) {
		ASSERT(D3EditState.current_room != -1);
		if (Search_mode)
			CheckRoom(&Rooms[D3EditState.current_room]);
		else {
			room *rp = &Rooms[D3EditState.current_room];
			DrawRoom(rp);
			ASSERT(D3EditState.current_room >= FIRST_PALETTE_ROOM);
			if (Current_faces[D3EditState.current_room - FIRST_PALETTE_ROOM] != -1)
				DrawRoomFace(CURFACE_COLOR,rp,Current_faces[D3EditState.current_room - FIRST_PALETTE_ROOM]);
		}
	}
	else {
		if (D3EditState.terrain_dots)
			DrawTerrainPoints (&viewer_position,view_orient);

		// Draw all rooms or only connected rooms.
		if (Draw_all_rooms) {
			int r;
			room *rp;
			for (r=0,rp=Rooms; r<=Highest_room_index; r++,rp++) {
				if (rp->used && (vm_VectorDistance(&rp->verts[0],view_target) < rad)) {
					if (rp != Curroomp) {
						int color = -1;
						if (IsRoomSelected(r))
							color = SELECTED_COLOR;
						DrawRoom(rp,color);
					}
					DrawRoomObjects(rp);
				}
			}
		}
		else
			Error("No support yet for Draw_all_rooms==0");
			//??DrawMine(mine_ptr,depth);

		if (Placed_room != -1)
			if (Rooms[Placed_room].used)
				DrawRoomRotated(&Rooms[Placed_room],&Placed_room_origin,&Placed_room_rotmat,&Placed_room_attachpoint,PLACED_COLOR);

		if (Placed_group)
			for (int r=0;r<Placed_group->nrooms;r++)
				DrawRoomRotated(&Placed_group->rooms[r],&Placed_room_origin,&Placed_room_rotmat,&Placed_room_attachpoint,PLACED_COLOR);

		//@@DrawWarningSegments();
		//@@draw_group_segments();
		//@@draw_found_segments();
		//@@DrawSelectedSegments();
		//@@draw_special_segments();

		// Highlight marked room side.
		if (Markedroomp) {
			DrawRoomFace(MARKEDFACE_COLOR,Markedroomp,Markedface);
			DrawFaceEdge(MARKEDEDGE_COLOR,Markedroomp,Markedface,Markededge);
			DrawVertBox(&Markedroomp->verts[Markedroomp->faces[Markedface].face_verts[Markedvert]],MARKEDEDGE_COLOR);
		}

		//Draw current room, face, & edge
		DrawRoom(Curroomp,CURROOM_COLOR);
		if (Curportal != -1)
			DrawRoomFace(CURPORTAL_COLOR,Curroomp,Curroomp->portals[Curportal].portal_face);
		DrawRoomFace(CURFACE_COLOR,Curroomp,Curface);
		DrawFaceEdge(CUREDGE_COLOR,Curroomp,Curface,Curedge);
		DrawVertBox(&Curroomp->verts[Curroomp->faces[Curface].face_verts[Curvert]],CUREDGE_COLOR);
	}

	EndEditorFrame();
}

