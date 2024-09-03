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

 * $Logfile: /DescentIII/Main/editor/HView.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Code to implement view menu functions
 *
 * $Log: not supported by cvs2svn $
 *
 * 24    4/30/99 11:18p Matt
 * Limit viewer to terrain bounds when moving the viewer to an external
 * room.
 *
 * 23    4/29/99 11:51p Matt
 * Improved viewer move functions.
 *
 * 22    4/17/99 2:42p Matt
 * Set Viewer_moved when a new viewer is selected.
 *
 * 21    2/28/99 8:11p Matt
 * Fixed problems when at max viewers
 *
 * 20    1/15/99 7:52p Chris
 * Updated ObjSetPos() to include a f_update_attach_children flag
 *
 * 19    5/12/98 2:49p Matt
 * Link the room viewer to the terrain, since there's not always a room to
 * put him in.
 *
 * 18    3/12/98 7:30p Chris
 * Added ObjSetOrient
 *
 * 17    2/09/98 10:55a Matt
 * When searching for rooms to put the viewer in, don't consider external
 * rooms.
 *
 * 16    2/05/98 3:11p Matt
 * Don't move the viewer to an external room.
 *
 * 15    2/05/98 2:57p Matt
 * Changed code to use ObjSetPos()
 *
 * 14    2/04/98 6:23p Matt
 * Changed object room number to indicate a terrain cell via a flag.  Got
 * rid of the object flag which used to indicate terrain.
 *
 * 13    1/29/98 5:50p Matt
 * Changed old camera object type to be viewer object (for editor), and
 * now camera objects will just be for game cameras.
 *
 * 12    1/21/98 1:08p Matt
 * Made SetViewMode() not take new_viewer parm, since it's not needed
 * after my revamp of the editor view system.
 *
 * 11    1/21/98 12:33p Matt
 * Revamped viewer system
 *
 * 10    1/19/98 10:04a Matt
 * Removed unused function
 *
 * 9     11/14/97 1:01p Matt
 * When can't find camera, don't just make one in the center of room 0.
 * Instead, find the first room that's used.
 *
 * 8     9/17/97 11:51a Samir
 * BIG SEGMENT RIPOUT
 *
 * 7     8/25/97 2:50p Chris
 * Improve AABB/object movement code.
 *
 * 6     8/04/97 7:39p Matt
 * Pass new room number to ObjSetPos()
 *
 * 5     8/04/97 12:43p Matt
 * Added SetViewerFromRoomFace()
 *
 * 4     7/22/97 7:08p Matt
 * Cleaned up D3EditState, moving some vars in and some out, and renaming
 * and changing a few others
 *
 * 4     4/01/97 11:00p Matt
 * Changed editor to keep a viewer object (type camera) separate from the
 * player object.  This camera, and not the player, is now moved by
 * slewing, the C key, etc.  When going into the game, the viewer position
 * & orientation are copied to the player.  When going back to the editor,
 * the player position is copied to the viewer, and the player object is
 * reset to its start location.
 *
 * 3     3/31/97 5:58p Matt
 * Revamped mine update flags
 *
 * 2     3/17/97 2:26p Matt
 * Added function to set player view from curseg/curside
 *
 * 1     3/17/97 1:47p Matt
 *
 * $NoKeywords: $
 */

#include "mainfrm.h"
#include "HView.h"
#include "d3edit.h"
#include "terrain.h"
#include "erooms.h"
#include "editor.h"

#define FACE_VIEW_DIST 5.0

// Set the viewer in the specified room facing the specified face
// If room_center is true, put viewer at the center of the room facing the face
// If room_center is false, put the viewer directly in front of the selected face
// If the room is external, put the viewer a distance away from the room,
// facing either the center (if room_center is true) or the specified face
void SetViewerFromRoomFace(room *roomp, int facenum, bool room_center) {
  vector vp, newpos;
  matrix orient;
  int roomnum;
  bool outside_mine = 0;

  ComputeCenterPointOnFace(&vp, roomp, facenum);
  roomnum = ROOMNUM(roomp);

  if (room_center) {

    // Get position
    ComputeRoomCenter(&newpos, roomp);

    if (roomp->flags & RF_EXTERNAL) {
      vector t;
      float rad = ComputeRoomBoundingSphere(&t, roomp);

      newpos.z -= rad * 1.5;

      if (newpos.x < 1.0)
        newpos.x = 1.0;
      if (newpos.x > TERRAIN_WIDTH * TERRAIN_SIZE - 1.0)
        newpos.x = TERRAIN_WIDTH * TERRAIN_SIZE - 1.0;
      if (newpos.z < 1.0)
        newpos.z = 1.0;
      if (newpos.z > TERRAIN_DEPTH * TERRAIN_SIZE - 1.0)
        newpos.z = TERRAIN_WIDTH * TERRAIN_SIZE - 1.0;

      orient = Identity_matrix;

      roomnum = GetTerrainRoomFromPos(&newpos);
    } else {
      // Get orientation
      vp -= newpos; // vector from center of room to face
      vm_VectorToMatrix(&orient, &vp, NULL, NULL);
    }
  } else {
    face *fp = &roomp->faces[facenum];

    newpos = vp + fp->normal * FACE_VIEW_DIST;

    vector t = -fp->normal;
    vm_VectorToMatrix(&orient, &t, NULL, NULL);

    if (roomp->flags & RF_EXTERNAL) {
      if (newpos.x < 1.0)
        newpos.x = 1.0;
      if (newpos.x > TERRAIN_WIDTH * TERRAIN_SIZE - 1.0)
        newpos.x = TERRAIN_WIDTH * TERRAIN_SIZE - 1.0;
      if (newpos.z < 1.0)
        newpos.z = 1.0;
      if (newpos.z > TERRAIN_DEPTH * TERRAIN_SIZE - 1.0)
        newpos.z = TERRAIN_WIDTH * TERRAIN_SIZE - 1.0;
      roomnum = GetTerrainRoomFromPos(&newpos);
    } else {
      int new_roomnum = FindPointRoom(&newpos);
      if (new_roomnum == -1)
        outside_mine = 1;
      else
        roomnum = new_roomnum;
    }
  }

  // Reset viewer
  if (Editor_view_mode == VM_ROOM) {
    Viewer_object->pos = newpos;
    Viewer_object->orient = orient;
  } else
    MoveViewer(&newpos, roomnum, &orient);

  if (outside_mine)
    Viewer_object->flags |= OF_OUTSIDE_MINE;

  Viewer_moved = 1;
}

// max viewers for each type
#define MAX_VIEWERS 20

// The viewer in room mode has this ID
#define ROOM_VIEWER_ID MAX_VIEWERS

// Finds a specific viewer object it one exists
// Parameters:	id - which viewer id
// Returns:		object number of a viewer object, or -1 if none
int FindViewerObject(int id) {
  object *objp;
  int objnum = -1;

  // trace through all objects looking for viewer
  for (objnum = 0, objp = Objects; objnum <= Highest_object_index; objnum++, objp++)
    if ((objp->type == OBJ_VIEWER) && (objp->id == id))
      break;

  if (objnum <= Highest_object_index) // found one
    return objnum;
  else
    return -1;
}

// Finds a viewer object if one exists.
// Starts looking at the specified id and searches through all possible ids
// Parameters:	id - which viewer id
//					view_mode - if -1, find any viewer, else find one that matches view mode
// Returns:		object number of a viewer object, or -1 if none
int FindNextViewerObject(int id, int view_mode) {
  int i;
  int objnum;
  int terrain_flag;

  if (id == -1)
    id = 0;

  ASSERT((id >= 0) && (id <= MAX_VIEWERS));

  // Get flags
  terrain_flag = (view_mode == VM_TERRAIN);

  // Try all viewer id's, starting at the one passed in
  for (i = 0; i < MAX_VIEWERS; i++) {
    int check_id = ((id + i) % MAX_VIEWERS);

    objnum = FindViewerObject(check_id);

    if ((objnum != -1) && ((view_mode == -1) || ((OBJECT_OUTSIDE(&Objects[objnum]) != 0) == terrain_flag)))
      break;
  }

  if (i == MAX_VIEWERS) // didn't find one
    return -1;

  return objnum;
}

// Creates a viewer object of the specified type
// Parameters:	view_mode - mine,terrain,or room.  See constants in 3dedit.h
//					pos - initial position of this object
//					roomnum - initial room/terrain cell of this object
// Returns:		object number of the object created, or -1 if at max number of viewers of that type
int CreateViewerObject(int view_mode, vector *pos, int roomnum) {
  int id;
  object *objp;
  int objnum = -1;

  if (view_mode == VM_ROOM) {
    id = ROOM_VIEWER_ID;

    for (objnum = 0, objp = Objects; objnum <= Highest_object_index; objnum++, objp++)
      if ((objp->type == OBJ_VIEWER) && (objp->id == id))
        return -1; // this one already used
  } else {
    // for each id, loop through all objects to see if it's used
    for (id = 0; id < MAX_VIEWERS; id++) {
      for (objnum = 0, objp = Objects; objnum <= Highest_object_index; objnum++, objp++)
        if ((objp->type == OBJ_VIEWER) && (objp->id == id))
          break;                         // this one already used
      if (objnum > Highest_object_index) // didn't find objec twith this id
        break;
    }

    if (id == MAX_VIEWERS) // no unused viewer id's
      return -1;
  }

  // Create the new object
  objnum = ObjCreate(OBJ_VIEWER, id, roomnum, pos, NULL);

  if (objnum == -1) {
    Int3();
    return -1;
  }

  return objnum;
}

// Returns the number (not the id) of the current viewer, in the range 0..MAX_VIEWERS
int GetViewerNum() {
  ASSERT(Editor_view_mode != VM_ROOM);

  return Viewer_object->id;
}

// Set the specified object to be the viewer
void SetViewer(int objnum) {
  Viewer_object = &Objects[objnum];

  if (Editor_view_mode != VM_ROOM)
    Editor_viewer_id = Viewer_object->id;

  if ((Editor_view_mode == VM_MINE) && OBJECT_OUTSIDE(Viewer_object))
    SetViewMode(VM_TERRAIN);

  if ((Editor_view_mode == VM_TERRAIN) && !(OBJECT_OUTSIDE(Viewer_object)))
    SetViewMode(VM_MINE);

  State_changed = Viewer_moved = 1;
}

// Creates a new viewer object.  Copies position & orientation from the current viewer
void CreateNewViewer() {
  int objnum;

  objnum = CreateViewerObject(Editor_view_mode, &Viewer_object->pos, Viewer_object->roomnum);

  if (objnum == -1) {
    OutrageMessageBox("Cannot create new viewer: you already have the maximum number of viewers.");
    return;
  }

  ObjSetOrient(&Objects[objnum], &Viewer_object->orient);

  SetViewer(objnum);

  EditorStatus("Viewer %d created.", GetViewerNum());

  World_changed = 1;
}

// Select next viewer
void SelectNextViewer() {
  int objnum;

  if (Editor_view_mode == VM_ROOM) {
    EditorStatus("There is only one viewer in Room View");
    return;
  }

  ASSERT(Viewer_object != NULL);

  objnum = FindNextViewerObject(Viewer_object->id + 1, -1);

  ASSERT(objnum != -1);

  if (objnum == OBJNUM(Viewer_object)) {
    EditorStatus("No other viewers.");
    return;
  }

  SetViewer(objnum);
  EditorStatus("Viewer %d selected.", GetViewerNum());
}

// Deletes the current viewer object
void DeleteViewer() {
  int objnum, old_num;

  if (Viewer_object->id == ROOM_VIEWER_ID) {
    OutrageMessageBox("You cannot delete the Room viewer.");
    return;
  }

  objnum = FindNextViewerObject(Viewer_object->id + 1, -1);

  ASSERT(objnum != -1);

  if (objnum == OBJNUM(Viewer_object)) { // couldn't find another viewer
    OutrageMessageBox("You cannot delete your only viewer.");
    return;
  }

  old_num = GetViewerNum();

  // Kill the old viewer
  ObjDelete(OBJNUM(Viewer_object));

  // Set the new viewer
  SetViewer(objnum);

  EditorStatus("Viewer %d deleted.  Viewer %d selected.", old_num, GetViewerNum());

  World_changed = 1;
}

// Sets the viewer object for the editor, creating if not already in mine
// Keeps separate viewer objects for mine & terrain views
void SetEditorViewer() {
  int objnum, roomnum;

  // First, see if a camera object already exists in the level
  if (Editor_view_mode == VM_ROOM)
    objnum = FindViewerObject(ROOM_VIEWER_ID);
  else
    objnum = FindNextViewerObject(Editor_viewer_id, Editor_view_mode);

  // If no viewer object, create one
  if (objnum == -1) {
    vector pos;

    // get position for viewer
    if (Editor_view_mode == VM_TERRAIN) { // if terrain, put viewer at center of world
      pos.x = TERRAIN_SIZE * TERRAIN_WIDTH / 2;
      pos.y = Terrain_seg[0].y + 30;
      pos.z = TERRAIN_SIZE * TERRAIN_DEPTH / 2;
      roomnum = MAKE_ROOMNUM(0);              // any value ok, so long as it has terrain flag
    } else if (Editor_view_mode == VM_MINE) { // if mine, put in center of any room
      for (roomnum = 0; roomnum <= Highest_room_index; roomnum++)
        if (Rooms[roomnum].used && !(Rooms[roomnum].flags & RF_EXTERNAL)) {
          ComputeRoomCenter(&pos, &Rooms[roomnum]);
          break;
        }
      ASSERT(roomnum <= Highest_room_index);
    } else if (Editor_view_mode == VM_ROOM) { // if room, put at 0,0,0
      vm_MakeZero(&pos);
      roomnum = MAKE_ROOMNUM(0);
    } else
      Int3(); // unknown view mode

    objnum = CreateViewerObject(Editor_view_mode, &pos, roomnum);

    // If no free viewer slots, grab any viewer and move it
    if (objnum == -1) {
      ASSERT(Viewer_object != NULL);
      if (Viewer_object->type == OBJ_VIEWER)
        objnum = OBJNUM(Viewer_object);
      else {
        objnum = FindNextViewerObject(Editor_viewer_id, -1);
        ASSERT(objnum != -1);
      }
      ObjSetPos(&Objects[objnum], &pos, roomnum, NULL, true);
    }
  }

  ASSERT(objnum != -1);
  SetViewer(objnum);
}

// Moves the room viewer to the origin, if there is a room viewer
void ResetRoomViewer() {
  int objnum = FindViewerObject(ROOM_VIEWER_ID);

  if (objnum != -1) {
    vm_MakeZero(&Objects[objnum].pos);
    ObjSetOrient(&Objects[objnum], &Identity_matrix);
  }
}
