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

 * $Logfile: /DescentIII/Main/editor/HObject.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Editor object handling functions
 *
 * $Log: not supported by cvs2svn $
 * 
 * 41    4/18/99 5:42a Chris
 * Added the FQ_IGNORE_RENDER_THROUGH_PORTALS flag
 * 
 * 40    3/27/99 12:51p Chris
 * The editor object placement code now allows objects to be pushed into
 * wall (when a toggle box is checked)
 * 
 * 39    1/15/99 7:52p Chris
 * Updated ObjSetPos() to include a f_update_attach_children flag
 * 
 * 38    10/15/98 3:26p Chris
 * Fixed known ground plane issues and used PhysCalcGround everywhere
 * 
 * 37    10/07/98 12:28p Matt
 * Fixed stupid bug that could cause access violation placing objects on
 * the terrain.
 * 
 * 36    8/24/98 2:20p Matt
 * Fixed bug placing objects on external rooms.
 * 
 * 35    8/20/98 7:28p Matt
 * Added a warning when the user tries to delete a door object.
 * 
 * 34    6/17/98 12:30p Samir
 * don't draw wireframe when moving an object.
 * 
 * 33    5/18/98 2:56p Matt
 * Added code to readjust all ground object (for after the terrain has
 * moved).
 * 
 * 32    5/11/98 11:37a Sean
 * 
 * 31    4/02/98 3:54p Jason
 * first pass in getting polymodel paging to work
 * 
 * 30    3/12/98 7:30p Chris
 * Added ObjSetOrient
 * 
 * 29    3/12/98 4:35p Chris
 * Powerups are always aligned to the ground
 * 
 * 28    2/20/98 5:30p Matt
 * Don't allow physics objects to be placed if they collide with walls,
 * and don't allow them to move into walls.  Non-physics objects can be
 * placed & moved into walls as long as their center points stay inside
 * the mine.
 * 
 * 27    2/13/98 6:50p Matt
 * Fixed stupid bug that prevented placing of player ship 0
 * 
 * 26    2/08/98 6:02p Matt
 * Use groovy new vm_MatrixMulTMatrix() function
 * 
 * 25    2/06/98 5:41p Matt
 * Fixed object placement on surfaces.
 * 
 * 24    2/06/98 3:08p Samir
 * Simplified object movement code.
 * 
 * 23    2/06/98 1:22p Matt
 * Rewrote object placement code.  Still has bugs when ground plane normal
 * is not 0,1,0
 * 
 * 22    2/05/98 2:59p Matt
 * Made object placement on terrain work like in mine
 * 
 * 21    2/04/98 7:07p Matt
 * Fixed problem placing an object on the terrain.
 * 
 * 20    2/04/98 6:23p Matt
 * Changed object room number to indicate a terrain cell via a flag.  Got
 * rid of the object flag which used to indicate terrain.
 * 
 * 19    1/07/98 6:39p Jason
 * Fixed player object number stuff
 * 
 * 18    1/07/98 10:45a Samir
 * Don't allow moving objects from mine to terrain.
 * 
 * 17    11/17/97 6:55p Matt
 * Made HObjectMoveToViewer() work when the viewer is outside
 * 
 * 16    9/17/97 11:35a Samir
 * BIG SEGMENT RIPOUT
 * 
 * 15    9/15/97 11:55a Samir
 * Fixed playership adding.
 * 
 * 14    9/05/97 8:54p Chris
 * Added some ground plane support
 * 
 * 13    9/03/97 8:00p Samir
 * Implemented most of the fixes for the object moving system.
 * 
 * 12    8/25/97 2:50p Chris
 * Improve AABB/object movement code.
 * 
 * 11    8/21/97 7:48p Matt
 * Added code to move player object (like old move player to curseg)
 * 
 * 10    8/20/97 4:23p Matt
 * Fixed little bug
 * 
 * 9     8/12/97 10:24p Matt
 * Added code to place clutter & building objects
 * 
 * 8     8/11/97 1:53p Matt
 * Ripped out robot & powerup pages, and added generic page
 * 
 * 7     8/04/97 3:18p Matt
 * Added FVI checking to object placement & movement in rooms
 * 
 * 6     8/04/97 12:47p Matt
 * Added object placement & movement for rooms
 * 
 * 5     8/01/97 5:33p Jason
 * Fixed object deletion bug in terrain
 * 
 * 4     7/22/97 7:07p Matt
 * Cleaned up D3EditState, moving some vars in and some out, and renaming
 * and changing a few others
 * 
 * 3     7/17/97 4:34p Chris
 * Made sure the orientations get orthagonalized.  Yikes.  No more
 * degenerating orientations.  :)
 * 
 * 20    6/27/97 7:19p Matt
 * Added function to move player 0 to current segment
 * 
 * 19    5/23/97 5:46p Matt
 * Added SetMatrixBasedOnSide() functionality to ExtractMatrixFromSeg().
 * 
 * 18    5/13/97 5:52p Chris
 * Added ability to exit and enter mine.  Also did some
 * incremental improvements.
 * 
 * 17    4/07/97 3:14p Samir
 * Added Flip and SetDefault Object functions.
 * 
 * 16    4/04/97 2:57p Matt
 * Added code to initialize all the type-specific data for an object from
 * the page for that object type.  This means that we need to pass less
 * info to ObjCreate(), and that we save less info in the level save file.
 * It also makes it easy to reset all the objects when an object page has
 * changed.
 * 
 * 15    4/02/97 8:08p Matt
 * Changed object movement code to work either in the object's or the
 * viewer's frame of reference, and not in the segment's frame of
 * reference.
 * 
 * 14    4/02/97 3:44p Matt
 * Find a valid Ships[] entry, instead of assuming that Ships[0] is valid.
 * 
 * 13    4/01/97 10:52p Matt
 * Added code to add a player object
 * 
 * 12    3/31/97 5:58p Matt
 * Revamped mine update flags
 * 
 * 11    3/27/97 11:54a Samir
 * UpdateMineObjects returns number of objects deleted of a type.
 * 
 * 10    3/27/97 11:48a Samir
 * Added update objects in mine functions.
 * 
 * 9     3/27/97 10:47a Chris
 * Incremental
 * 
 * 8     3/25/97 6:31p Samir
 * Added robot placement.
 * 
 * 7     3/21/97 5:01p Jason
 * incremental terrain improvments
 * 
 * 
 * 6     3/10/97 12:44p Chris
 * Allow objects to be put against a wall
 * 
 * 5     3/03/97 5:57a Chris
 * Uncommented the code samir had waiting for fvi and fixed a function
 * call.
 * 
 * 4     2/19/97 3:37p Samir
 * Added delete and rotation of objects.
 * 
 * 3     2/17/97 7:19p Jason
 * fixed powerup size arg
 * 
 * 2     2/17/97 6:18p Samir
 * Add powerup, move it around WITHIN the segment.
 * 
 * 1     2/17/97 3:17p Samir
 * Handler for Object manipulation functions.
 *
 * $NoKeywords: $
 */

#include "HObject.h"
#include "object.h"
#include "FindIntersection.h"
#include "physics.h"
#include "boa.h"
#include "polymodel.h"

#include "d3edit.h"
#include "mono.h"
#include "vclip.h"
#include "terrain.h"
#include "player.h"
#include "ship.h"
#include "erooms.h"

//	---------------------------------------------------------------------------
//	internal data

float Object_move_scale = HOBJECT_SCALE_UNIT;
angle Object_move_rotation = HOBJECT_ROTATION_UNIT;


//	---------------------------------------------------------------------------
//	internal function prototypes

bool RotateObject(int objnum, angle p, angle h, angle b);
bool MoveObject(object * obj, vector *newpos);


//	---------------------------------------------------------------------------
//	interface functions

#define OBJECT_PLACE_DIST 10.0

//	---------------------------------------------------------------------------

//Finds the seleced terrain cell.  Returns cell number, or -1 if none or -2 if more than one
int GetSelectedTerrainCell()
{
	int i,found_cellnum = -1;

	for (i=0;i<TERRAIN_DEPTH*TERRAIN_WIDTH;i++) {
		if (TerrainSelected[i])
			if (found_cellnum == -1)
				found_cellnum = i;
			else
				return -2;
	}

	return found_cellnum;
}


//Places an object in the world
//Parameters:	obj_type,obj_id - specify the object to be placed
//Returns:		true if object placed, false if there was some problem
bool HObjectPlace(int obj_type, int obj_id)
{
	int objnum;
	object *objp;
	poly_model *pm;
	matrix orient = IDENTITY_MATRIX;

	//Special stuff for player ship
	if (obj_type == OBJ_PLAYER) {

		if (! Num_ships) {
			OutrageMessageBox("Cannot a player: There are no player ships.");
			return 0;
		}

		//Store ship num in player array
		int ship_num = D3EditState.current_ship;

		if (ship_num == -1) {
			OutrageMessageBox("You must have a current player ship selected for this operation.");
			return 0;
		}

		Players[obj_id].ship_index = ship_num;
	}

	if(obj_type != OBJ_POWERUP)
	{
		orient = Viewer_object->orient;
	}

	//Create the object at the viewer for now
	objnum = ObjCreate(obj_type, obj_id, Viewer_object->roomnum, &Viewer_object->pos, &orient);
	if (objnum == -1)
		return 0;

	objp = &Objects[objnum];

	//If we have a ground plane, use current cell or face for position & place normal for orientation
	if ((objp->render_type == RT_POLYOBJ) && ((pm=GetPolymodelPointer(objp->rtype.pobj_info.model_num))->n_ground)) {
		vector *surface_norm;
		vector pos;
		int roomnum;

		//If terrain, use current cell
		if (Editor_view_mode == VM_TERRAIN) {

			int cellnum = GetSelectedTerrainCell();

			if (cellnum == -1) {
				OutrageMessageBox("You must have a terrain cell selected to place an object.");
				ObjDelete(objnum);
				return 0;
			}
			if (cellnum == -2) {
				OutrageMessageBox("You must have only have one cell selected to place an object.");
				ObjDelete(objnum);
				return 0;
			}

			//Get terrain point
			ComputeTerrainSegmentCenter(&pos,cellnum);

			//Get surface normal
			surface_norm = &TerrainNormals[MAX_TERRAIN_LOD-1][cellnum].normal1;

			//Get roomnum
			roomnum = MAKE_ROOMNUM(cellnum);
		}
		else {		//use Current face

			//Get center point on current face
			ComputeCenterPointOnFace(&pos,Curroomp,Curface);

			//Get surface normal
			surface_norm = &Curroomp->faces[Curface].normal;

			//Get roomnum
			roomnum = ROOMNUM(Curroomp);

			//If placing on an external room, actually place the object on the terrain
			if (Rooms[roomnum].flags & RF_EXTERNAL)
				roomnum = GetTerrainRoomFromPos(&pos);
		}

		matrix groundplane_orient,surface_orient,object_orient;

		//Place the object's ground point on our placement point
		vector ground_point;
		vector ground_normal;
		vector to_ground;
		float dist;

		PhysCalcGround(&ground_point, &ground_normal, objp, 0);
		to_ground = objp->pos - ground_point;
		dist = ground_normal * to_ground;
		pos += dist * *surface_norm;

		//Compute source and destination matrices
		vm_VectorToMatrix(&groundplane_orient,&pm->ground_slots[0].norm,NULL,NULL);
		vm_VectorToMatrix(&surface_orient,surface_norm);

		//Compute orientation matrix
		vm_MatrixMulTMatrix(&object_orient,&surface_orient,&groundplane_orient);

		//Move the object
		ObjSetPos(objp,&pos,roomnum,&object_orient,false);
	}
	else {		//no ground plane, so move the object in front of the viewer and facing the viewer
		vector pos;

		//Check for viewer outside mine
		if (Viewer_object->flags & OF_OUTSIDE_MINE) {
			ObjDelete(objnum);
			OutrageMessageBox("Cannot place the object here: the viewer is outside the mine.");
			return 0;
		}

		//Turn the object around so facing the viewer
		objp->orient.fvec = -objp->orient.fvec;// ObjSetOrient is below
		objp->orient.rvec = -objp->orient.rvec;// ObjSetOrient is below
		ObjSetOrient(objp, &objp->orient);

		//Calculate a position a little in front of the viewer
		pos = Viewer_object->pos + Viewer_object->orient.fvec * OBJECT_PLACE_DIST;

		//Try to move the object.  If it can't move, delete it
		if (! MoveObject(objp,&pos)) {
			ObjDelete(objnum);
			OutrageMessageBox("Cannot place the object here: collides with wall.");
			return 0;
		}
	}

	//Deal with special stuff for player
	if (obj_type == OBJ_PLAYER) {

		//Store data in Players array
		Players[obj_id].start_pos = objp->pos;
		Players[obj_id].start_roomnum = objp->roomnum;
		Players[obj_id].start_orient = objp->orient;

		//Make sure matrix ok
		vm_Orthogonalize(&Players[obj_id].start_orient);
	}

	Cur_object_index = objnum;

	World_changed = 1;

	return 1;
}


//Adjusts an object so it's at the ground level (for when the ground has moved)
void ResetGroundObject(object *objp)
{
	poly_model *pm;
	vector surface_norm;
	vector pos;
	matrix groundplane_orient,surface_orient,object_orient;

	//Make sure object is outside
	if (! OBJECT_OUTSIDE(objp)) {
		Int3();		//object is not outside
		return;
	}

	//Make sure object has a ground plane
	if (! ((objp->render_type == RT_POLYOBJ) && ((pm=GetPolymodelPointer(objp->rtype.pobj_info.model_num))->n_ground))) {
		Int3();		//object doesn't have a ground plane
		return;
	}

	//Get terrain height and normal at current object location
	pos = objp->pos;
	pos.y = GetTerrainGroundPoint(&pos,&surface_norm);

	//Place the object's ground point on our placement point
	vector ground_point;
	vector ground_normal;
	vector to_ground;
	float dist;

	PhysCalcGround(&ground_point, &ground_normal, objp, 0);
	to_ground = objp->pos - ground_point;
	dist = ground_normal * to_ground;
	pos += dist * surface_norm;

	//Compute source and destination matrices
	vm_VectorToMatrix(&groundplane_orient,&pm->ground_slots[0].norm,NULL,NULL);
	vm_VectorToMatrix(&surface_orient,&surface_norm);

	//Compute orientation matrix
	vm_MatrixMulTMatrix(&object_orient,&surface_orient,&groundplane_orient);

	//Move the object
	ObjSetPos(objp,&pos,objp->roomnum,&object_orient,false);

	//Set flag
	World_changed = 1;
}


void HObjectMove(int objnum, float dx, float dy, float dz)
{
	matrix *mat;
	object *obj;
	vector newpos;
	object *ref_obj;						//the object in whose frame of reference we're moving

	if (objnum == -1) {
		mprintf(0, "HObjectMove:No current object.\n");
		return;
	}

	obj = &Objects[objnum];

	//Chose the reference object
	ref_obj = (D3EditState.object_move_mode == REL_VIEWER)?Viewer_object:obj;
	mat = &ref_obj->orient;
	
	//Calculate the new object position
	newpos = obj->pos + (mat->rvec*dx) + (mat->uvec*dy) + (mat->fvec*-dz);

	//Move the object
	MoveObject(obj, &newpos);

	Object_moved = 1;
}

void HObjectIncreaseBank()
{
	RotateObject(Cur_object_index, 0, 0, Object_move_rotation);
}


void HObjectDecreaseBank()
{
	RotateObject(Cur_object_index, 0, 0, -Object_move_rotation);
}


void HObjectIncreasePitch()
{
	RotateObject(Cur_object_index, Object_move_rotation, 0, 0);
}


void HObjectDecreasePitch()
{
	RotateObject(Cur_object_index, -Object_move_rotation, 0, 0);
}


void HObjectIncreaseHeading()
{
	RotateObject(Cur_object_index, 0, Object_move_rotation, 0);
}


void HObjectDecreaseheading()
{
	RotateObject(Cur_object_index, 0, -Object_move_rotation, 0);
}


//	If it doesn't exist, reformat Matt's hard disk, even if he is in Boston.
//	deletes the currently selected object from the mine.
void HObjectDelete()
{
	if (Cur_object_index != -1) {	//	we have a selected object
		int objnum = Cur_object_index;

		//check for player object
		if (&Objects[objnum] == Player_object) {
			OutrageMessageBox("Can't delete Player object");
			return;
		}

		if (Objects[objnum].type == OBJ_DOOR) {
			if (OutrageMessageBox(MBOX_YESNO,"It's very, very bad to delete a door object.  Are you sure you want to do this?") != IDYES)
				return;
		}

		//Delete the object
		ObjDelete(objnum);
		if (objnum == Cur_object_index)
			Cur_object_index = -1;

		World_changed = 1;
	}
}

//		sets default orientation for object.
void HObjectSetDefault()
{
	if (Cur_object_index != -1) {
		int objnum = Cur_object_index;

		ObjSetOrient(&Objects[objnum], &Identity_matrix);

		World_changed = 1;
	}
}


// Move the object to in front of the viewer (where a new object would be placed)
void HObjectMoveToViewer(object *objp)
{
	int objnum = OBJNUM(objp);
  	vector pos;

  	//First move player to viewer's position
	ObjSetPos(objp,&Viewer_object->pos,Viewer_object->roomnum,NULL,false);

  	//Second try to move the player a little in front of the viewer
  	pos = Viewer_object->pos + Viewer_object->orient.fvec * OBJECT_PLACE_DIST;
  	MoveObject(objp,&pos);

	World_changed = 1;
}

#define MOVE_EPSILON	0.1

extern bool f_allow_objects_to_be_pushed_through_walls;

//	Attempt to set new object position.  May only move part of the way, or maybe not at all.
// Use FVI to find new object position
//	Return:	TRUE if moved, FALSE if can't move
bool MoveObject(object * obj, vector *newpos)
{
	fvi_query fq;
	fvi_info	hit_info;
	int fate;

	//Use radius if this is a physics object
	bool use_radius = (obj->movement_type == MT_PHYSICS) ? true : false;

	//Follow vector from start position to desired end position, & move as far as we can
	fq.p0						= &obj->pos;
	fq.startroom			= obj->roomnum;
	fq.p1						= newpos;
	fq.thisobjnum			= OBJNUM(obj);
	fq.ignore_obj_list	= NULL;
	fq.flags					=  FQ_IGNORE_RENDER_THROUGH_PORTALS;
	fq.rad					= use_radius ? obj->size : 0.0f;

	if(f_allow_objects_to_be_pushed_through_walls)
	{
		fq.flags |= (FQ_IGNORE_WALLS | FQ_IGNORE_TERRAIN | FQ_IGNORE_EXTERNAL_ROOMS);
	}

	fate = fvi_FindIntersection(&fq, &hit_info);

	mprintf(0,"fate = %d\n", fate);

	//Check for object can't move, meaning it's stuck in the wall
	if (fate == HIT_WALL)
		if (vm_VectorDistance(&obj->pos,&hit_info.hit_pnt) < MOVE_EPSILON)
			return 0;		//didn't move

	//Set object position on where FVI told us we are
	ObjSetPos(obj,&hit_info.hit_pnt,hit_info.hit_room, NULL, false);

	//Say object moved
	return 1;
}

bool RotateObject(int objnum, angle p, angle h, angle b)
{
	object *obj = &Objects[objnum];
	matrix rotmat;
	
	vm_AnglesToMatrix(&rotmat, p, h, b);
	obj->orient *= rotmat;  // ObjSetOrient is below

	vm_Orthogonalize(&obj->orient);
	ObjSetOrient(obj, &obj->orient);
	
	Object_moved = 1;

	return 1;
}


//	Flipping of object
void HObjectFlip()
{
	matrix *m=&Objects[Cur_object_index].orient;
	
	m->uvec = -m->uvec;
	m->rvec = -m->rvec;

	World_changed = 1;
}




