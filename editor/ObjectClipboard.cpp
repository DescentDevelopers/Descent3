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

        ObjectClipboard.cpp
                This file contains the cut,copy,paste, and delete routines used when you right click in the view
*/

#include "stdafx.h"

#include "TextureGrWnd.h"

#include "editor.h"
#include "render.h"
#include "manage.h"
#include "gr.h"
#include "gameloop.h"
#include "descent.h"
#include "terrain.h"
#include "HTexture.h"
#include "terraindialog.h"
#include "room.h"
#include <string.h>

#include <assert.h>

#include "mono.h"
#include "3d.h"
#include "gr.h"
#include "gamepath.h"
#include "ObjectClipboard.h"
#include "HObject.h"

object ClipBoardObject;
bool ObjectInBuffer;

// Copies all the plain fields of src to dst. The members an object owns (name, scripts, AI/effect/lighting info,
// weapon battery info, attach list and lightmaps) are never copied: dst keeps its own, so no two objects share them.
static void CopyObjectPlainFields(object *dst, const object *src) {
  // The turret keyframe buffers inside the rtype union are owned too (ObjDelete frees them), so keep dst's own
  float *keyframes = nullptr, *last_keyframes = nullptr;
  if (dst->flags & OF_POLYGON_OBJECT) {
    keyframes = dst->rtype.pobj_info.multi_turret_info.keyframes;
    last_keyframes = dst->rtype.pobj_info.multi_turret_info.last_keyframes;
  }

  dst->type = src->type;
  dst->dummy_type = src->dummy_type;
  dst->id = src->id;
  dst->flags = src->flags;
  dst->handle = src->handle;
  dst->next = src->next;
  dst->prev = src->prev;
  dst->control_type = src->control_type;
  dst->movement_type = src->movement_type;
  dst->render_type = src->render_type;
  dst->lighting_render_type = src->lighting_render_type;
  dst->roomnum = src->roomnum;
  dst->pos = src->pos;
  dst->orient = src->orient;
  dst->last_pos = src->last_pos;
  dst->renderframe = src->renderframe;
  dst->wall_sphere_offset = src->wall_sphere_offset;
  dst->anim_sphere_offset = src->anim_sphere_offset;
  dst->size = src->size;
  dst->shields = src->shields;
  dst->contains_type = src->contains_type;
  dst->contains_id = src->contains_id;
  dst->contains_count = src->contains_count;
  dst->pad3 = src->pad3;
  dst->creation_time = src->creation_time;
  dst->lifeleft = src->lifeleft;
  dst->lifetime = src->lifetime;
  dst->parent_handle = src->parent_handle;
  dst->attach_ultimate_handle = src->attach_ultimate_handle;
  dst->attach_parent_handle = src->attach_parent_handle;
  dst->weapon_fire_flags = src->weapon_fire_flags;
  dst->attach_type = src->attach_type;
  dst->lowest_attached_vis = src->lowest_attached_vis;
  memcpy(&dst->attach_dist, &src->attach_dist, sizeof(dst->attach_dist)); // anonymous union with attach_index
  dst->mtype = src->mtype;
  dst->min_xyz = src->min_xyz;
  dst->max_xyz = src->max_xyz;
  dst->impact_size = src->impact_size;
  dst->impact_time = src->impact_time;
  dst->impact_player_damage = src->impact_player_damage;
  dst->impact_generic_damage = src->impact_generic_damage;
  dst->impact_force = src->impact_force;
  dst->change_flags = src->change_flags;
  dst->generic_nonvis_flags = src->generic_nonvis_flags;
  dst->generic_sent_nonvis = src->generic_sent_nonvis;
  dst->ctype = src->ctype;
  dst->rtype = src->rtype;
  dst->position_counter = src->position_counter;

  if (src->flags & OF_POLYGON_OBJECT) {
    dst->rtype.pobj_info.multi_turret_info.keyframes = keyframes;
    dst->rtype.pobj_info.multi_turret_info.last_keyframes = last_keyframes;
  }
}

// Returns the number if OBJ_PLAYER in the Objects list
int GetNumberPlayerObjs() {
  int count;
  int i;
  count = 0;

  for (i = 0; i <= Highest_object_index; i++) {
    if (Objects[i].type == OBJ_PLAYER)
      count++;
  }
  mprintf(0, "GetNumberPlayerObjs returns %d\n", count);
  return count;
}

// Makes a copy of the current object to the clipboard object, then deletes this object
// Returns true on success
bool ObjectCut() {
  mprintf(0, "ObjectCut()\n");
  if (Cur_object_index != -1) {
    int objnum = Cur_object_index;
    int num_player_objects = GetNumberPlayerObjs();
    if ((num_player_objects == 1) && Objects[objnum].type == OBJ_PLAYER) {
      mprintf(0, "Oops, Can't delete the only player...sorry\n");
      return false;
    }
    mprintf(0, "Attempting to cut object #%d into buffer...", objnum);
    CopyObjectPlainFields(&ClipBoardObject, &Objects[objnum]);
    mprintf(0, "Done!\n");
    ObjectInBuffer = true;
    HObjectDelete();
    return true;
  }
  return false;
}

// Makes a copy of the current object to the clipboard object
// Returns true on success
bool ObjectCopy() {
  mprintf(0, "ObjectCopy()\n");
  if (Cur_object_index != -1) {
    int objnum = Cur_object_index;
    mprintf(0, "Attempting to copy object #%d into buffer...", objnum);
    CopyObjectPlainFields(&ClipBoardObject, &Objects[objnum]);
    mprintf(0, "Done!\n");
    ObjectInBuffer = true;
    return true;
  }

  return false;
}

// Just deletes the current object
// Returns true on success
bool ObjectDelete() {
  mprintf(0, "ObjectDelete()\n");
  if (Cur_object_index != -1) {
    int objnum = Cur_object_index;
    int num_player_objects = GetNumberPlayerObjs();
    if ((num_player_objects == 1) && Objects[objnum].type == OBJ_PLAYER) {
      mprintf(0, "Oops, Can't delete the only player...sorry\n");
      return false;
    }
    HObjectDelete();
    return true;
  }
  return false;
}

// Pastes the object in the Clipboard to the middle of the current segment
// Returns true on success
bool ObjectPaste() {
  mprintf(0, "ObjectPaste()\n");

  Int3(); // I'm not so sure about this routine -- by copying everything over, it may copy
          // some data it shouldn't, such as script info.

  if (!ObjectInBuffer)
    return false;

  int16_t next, prev;
  int roomnum;
  vector pos;
  matrix orient;
  vector last_pos;
  float creation_time;
  int handle;

  if (!HObjectPlace(ClipBoardObject.type, ClipBoardObject.id)) {
    return false;
  }
  mprintf(0, "Saving data...\n");
  handle = Objects[Cur_object_index].handle;
  next = Objects[Cur_object_index].next;
  prev = Objects[Cur_object_index].prev;
  roomnum = Objects[Cur_object_index].roomnum;
  pos = Objects[Cur_object_index].pos;
  orient = Objects[Cur_object_index].orient;
  last_pos = Objects[Cur_object_index].last_pos;
  creation_time = Objects[Cur_object_index].creation_time;

  mprintf(0, "Overwriting with buffer data\n");
  CopyObjectPlainFields(&Objects[Cur_object_index], &ClipBoardObject);

  mprintf(0, "Restoring data...\n");
  Objects[Cur_object_index].handle = handle;
  Objects[Cur_object_index].next = next;
  Objects[Cur_object_index].prev = prev;
  Objects[Cur_object_index].roomnum = roomnum;
  Objects[Cur_object_index].pos = pos;
  ObjSetOrient(&Objects[Cur_object_index], &orient);
  Objects[Cur_object_index].last_pos = last_pos;
  Objects[Cur_object_index].creation_time = creation_time;

  return true;
}
