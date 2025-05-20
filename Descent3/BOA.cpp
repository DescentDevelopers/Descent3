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

 * $Logfile: /DescentIII/main/BOA.cpp $
 * $Revision: 121 $
 * $Date: 4/19/00 5:09p $
 * $Author: Matt $
 *
 * Description goes here
 *
 * $Log: /DescentIII/main/BOA.cpp $
 *
 * 121   4/19/00 5:09p Matt
 * From Duane for 1.4
 * Added checks, asserts, and fixes for bad return values
 * Mac-only optimizations and mem alloc/free change
 *
 * 120   3/20/00 12:01p Matt
 * Merge of Duane's post-1.3 changes.
 * Changed some malloc/free calls to mem_malloc/mem_free (Mac only)
 *
 * 119   10/08/99 4:27p Chris
 * Fixed a bug with terrain regioning
 *
 * 118   6/11/99 6:06p Chris
 *
 * 117   6/11/99 6:00p Chris
 * Added status bars for boa_vis
 *
 * 116   6/11/99 12:22p Chris
 * NewEditor changes
 *
 * 115   6/11/99 12:20p Chris
 * New Editor changes
 *
 * 114   5/23/99 12:32a Chris
 * Forcefields are now ignored in vis stuff
 *
 * 113   5/21/99 3:27a Chris
 * Fuck me...  Broken fix...  Fixed?  I hope.
 *
 * 112   5/21/99 1:35a Chris
 * Fixed a bug in vis (outside areas could only see the first room down)
 * :(
 *
 * 111   5/20/99 1:16a Chris
 * Made going outside more expensive
 *
 * 110   5/10/99 10:10p Ardussi
 * changes to compile on Mac
 *
 * 109   5/09/99 3:25p Jason
 * fixed potential BOA problem
 *
 * 108   5/03/99 5:54p Kevin
 * removed logging code
 *
 * 107   5/03/99 5:45p Jason
 * trying to track down boa problem
 *
 * 106   5/03/99 5:12p Jason
 * fixing BOA vis problem...
 *
 * 105   5/01/99 5:12p Jason
 * made BOA checksum finally reliable
 *
 * 104   5/01/99 4:45p Chris
 * Fixed a bug with bogus vis checksums
 *
 * 103   4/30/99 2:46p Chris
 * Added a pragma for turning optimazatons off for checksum functions
 *
 * 102   4/30/99 11:54a Chris
 *
 * 101   4/29/99 5:43p Chris
 * Added the check for bad center points
 *
 * 100   4/28/99 1:20p Chris
 * Added the ability to block portals
 *
 * 99    4/27/99 12:57p Chris
 *
 * 98    4/27/99 11:42a Chris
 * Fixed a multi-system induced bug
 *
 * 97    4/27/99 10:48a Chris
 * Slowly and carefully improving BNodes
 *
 * 96    4/21/99 1:30p Matt
 * Make breakable glass use the new breakable flag, instead of
 * piggybacking on the destroyable flag.
 *
 * 95    4/21/99 3:03a Chris
 * Fixed bug with blastable doors and the AI code.  :)
 *
 * 94    4/20/99 8:55p Chris
 * Fixed problem with robots not being able to open locked doors that a
 * player has the key for.
 *
 * 93    4/18/99 5:39a Chris
 * Vastly improved the path node system
 *
 * $NoKeywords: $
 */

#include <cstdlib>
#include <cstring>

#ifdef EDITOR
#include "editor\d3edit.h"
#endif
#ifdef NEWEDITOR
#include "neweditor\globals.h"
#endif

#include "BOA.h"
#include "vecmat.h"
#include "room.h"
#include "object.h"
#include "pserror.h"
#include "findintersection.h"
#include "log.h"
#include "mem.h"
#include "doorway.h"

#define BOA_VERSION 25

static const uint8_t bbf_lookup[27] = {(0),
                                     (0x01),
                                     (0x02),
                                     (0x04),
                                     (0x08),
                                     (0x10),
                                     (0x20),
                                     (0x01 | 0x02),
                                     (0x01 | 0x04),
                                     (0x01 | 0x10),
                                     (0x01 | 0x20),
                                     (0x02 | 0x04),
                                     (0x02 | 0x08),
                                     (0x02 | 0x20),
                                     (0x04 | 0x08),
                                     (0x04 | 0x10),
                                     (0x08 | 0x10),
                                     (0x08 | 0x20),
                                     (0x10 | 0x20),
                                     (0x01 | 0x02 | 0x04),
                                     (0x01 | 0x02 | 0x20),
                                     (0x01 | 0x04 | 0x10),
                                     (0x01 | 0x10 | 0x20),
                                     (0x08 | 0x02 | 0x04),
                                     (0x08 | 0x02 | 0x20),
                                     (0x08 | 0x04 | 0x10),
                                     (0x08 | 0x10 | 0x20)};

uint16_t BOA_Array[MAX_ROOMS + MAX_BOA_TERRAIN_REGIONS][MAX_ROOMS + MAX_BOA_TERRAIN_REGIONS];
float BOA_cost_array[MAX_ROOMS + MAX_BOA_TERRAIN_REGIONS][MAX_PATH_PORTALS];
int BOA_mine_checksum = 0;
int BOA_vis_checksum = 0; // this checksum is for the VIS bit of the boa array
bool BOA_vis_valid = 0;   // Is the vis table up to date and valid to use?
int BOA_AABB_checksum = 0;
int BOA_AABB_ROOM_checksum[MAX_ROOMS + MAX_BOA_TERRAIN_REGIONS];

static bool BOA_f_making_boa = false;

int BOA_num_mines = 0;
int BOA_num_terrain_regions = 0;

int BOA_num_connect[MAX_BOA_TERRAIN_REGIONS];
connect_data BOA_connect[MAX_BOA_TERRAIN_REGIONS][MAX_PATH_PORTALS];

static void add_mine_room(int room, int mine, char *checked);
static void compute_mine_info();
static void add_terrain_cell(int cell, int t_region, char *checked);
static void compute_terrain_region_info();
static void compute_sound_dist_info();
static void clear_BOA();
static void compute_costs();
static void update_path_info(q_item *node_list[MAX_ROOMS], int start, int end);
static void FindPath(int i, int j);
static void compute_next_segs();
static void compute_blockage_info();
static void ComputeBOAVisFaceUpperLeft(room *rp, face *fp, vector *upper_left, float *xdiff, float *ydiff,
                                       vector *center);
static int BOAGetRoomChecksum(int i);
static bool IsPathPointValid(int room, vector *pos);
static void ValidateRoomPathPoint(int room, char *message, int len);
static void verify_connections();
static void find_small_portals();
static void compute_robot_path_info();

bool BOA_PassablePortal(int room, int portal_index, bool f_for_sound, bool f_making_robot_path_invalid_list) {
  if (room == -1) {
    return false;
  }

  room = BOA_INDEX(room);

  if (room > Highest_room_index && room <= Highest_room_index + BOA_num_terrain_regions) {
    int tr;

    tr = room - Highest_room_index - 1;

    // Inside room/portal
    int temp_room = BOA_connect[tr][portal_index].roomnum;
    int temp_portal_index = BOA_connect[tr][portal_index].portal;

    // External room/portal
    room = Rooms[temp_room].portals[temp_portal_index].croom;
    portal_index = Rooms[temp_room].portals[temp_portal_index].cportal;
  }

  ASSERT(room >= 0 && room <= Highest_room_index && Rooms[room].used);
  face *fp = &Rooms[room].faces[Rooms[room].portals[portal_index].portal_face];

  if (Rooms[room].portals[portal_index].croom < 0)
    return false;

  if (!BOA_f_making_boa) {
    if (BOA_cost_array[room][portal_index] < 0.0f && !(room <= Highest_room_index && (Rooms[room].flags & RF_EXTERNAL)))
      return false;

    if (!f_for_sound) {
      if (Rooms[room].portals[portal_index].flags & PF_TOO_SMALL_FOR_ROBOT)
        return false;
    }

    if (((Rooms[room].portals[portal_index].flags & PF_RENDER_FACES) &&
            !(Rooms[room].portals[portal_index].flags & PF_RENDERED_FLYTHROUGH)) ||
        (Rooms[room].portals[portal_index].flags & PF_BLOCK)) {
      return false;
    }
  } else {
    if (f_making_robot_path_invalid_list) {
      if (Rooms[room].portals[portal_index].flags & PF_TOO_SMALL_FOR_ROBOT)
        return false;
    }

    if ((Rooms[room].portals[portal_index].flags & PF_BLOCK) &&
        !(Rooms[room].portals[portal_index].flags & PF_BLOCK_REMOVABLE))
      return false;

    if ((Rooms[room].portals[portal_index].flags & PF_RENDER_FACES) &&
        !(Rooms[room].portals[portal_index].flags & PF_RENDERED_FLYTHROUGH)) {
      if (!(GameTextures[fp->tmap].flags & (TF_BREAKABLE | TF_FORCEFIELD))) {
        return false;
      }
    }
  }

  return true;
}

extern object *GetDoorObject(room *rp);

bool BOA_LockedDoor(object *obj, int roomnum) {
  if (roomnum >= 0 && roomnum <= Highest_room_index && Rooms[roomnum].used && (Rooms[roomnum].flags & RF_DOOR)) {
    if (!obj) {
      return DoorwayLocked(&Rooms[roomnum]) && DoorwayPosition(&Rooms[roomnum]) < 0.5f;
    } else {
      object *d_obj = GetDoorObject(&Rooms[roomnum]);

      if (d_obj)
        return (!DoorwayOpenable(d_obj->handle, obj->handle)) && DoorwayPosition(&Rooms[roomnum]) < 0.5f;
      else
        return false;
    }
  }

  return false;
}

// bool BOA_IsPathClearForRobot(int start_room, int end_room)
//{
//	start_room = BOA_INDEX(start_room);
//	end_room = BOA_INDEX(end_room);
//
//	if(BOA_TOO_SMALL_FOR_ROBOT(start_room, end_room))
//	{
//		return false;
//	}
//
//	if(!BOA_HasPossibleBlockage(start_room, end_room))
//	{
//		return true;
//	}
//
//	int last_room;
//	int next_room = start_room;
//	ASSERT(next_room >= 0 && next_room <= Highest_room_index + MAX_BOA_TERRAIN_REGIONS);
//
//	while(BOA_INDEX(next_room) != BOA_INDEX(start_room) && (BOA_INDEX(next_room) != BOA_INDEX(end_room)) &&
//(next_room != BOA_NO_PATH))
//	{
//		if(BOA_LockedDoor(next_room))
//		{
//			return false;
//		}
//
//		last_room = next_room;
//		next_room = BOA_GetNextRoom(next_room, end_room);
//
//		if(BOA_DetermineStartRoomPortal(last_room, NULL, next_room, NULL, false, NULL) == -1)
//		{
//			return false;
//		}
//	}
//
//	return true;
//}

int BOA_DetermineStartRoomPortal(int start_room, vector *start_pos, int end_room, vector *end_pos, bool f_for_sound,
                                 bool f_making_robot_path_invalid_list, int *blocked_portal) {
  int i;

  if (start_room == -1 || end_room == -1)
    return -1;
  if (start_room > Highest_room_index && end_room > Highest_room_index)
    return -1;

  start_room = BOA_INDEX(start_room);
  end_room = BOA_INDEX(end_room);

  if (start_room <= Highest_room_index) {
    for (i = 0; i < Rooms[start_room].num_portals; i++) {
      if (!BOA_PassablePortal(start_room, i, f_for_sound, f_making_robot_path_invalid_list))
        continue;

      if (end_room <= Highest_room_index) {
        if (Rooms[start_room].portals[i].croom == end_room)
          break;
      } else {
        if (Rooms[Rooms[start_room].portals[i].croom].flags & RF_EXTERNAL) {
          int cell = GetTerrainCellFromPos(&Rooms[start_room].portals[i].path_pnt);
          ASSERT(cell != -1); // DAJ -1FIX

          if (Highest_room_index + TERRAIN_REGION(cell) + 1 == end_room)
            break;
        }
      }
    }

    if (i >= Rooms[start_room].num_portals)
      i = -1;
  } else {
    for (i = 0; i < BOA_num_connect[start_room - Highest_room_index - 1]; i++) {
      ASSERT(end_room <= Highest_room_index);

      if (BOA_connect[start_room - Highest_room_index - 1][i].roomnum == end_room) {
        int next_portal = BOA_connect[start_room - Highest_room_index - 1][i].portal;
        int external_room = Rooms[end_room].portals[next_portal].croom;
        int external_portal = Rooms[end_room].portals[next_portal].cportal;

        if (BOA_PassablePortal(external_room, external_portal, f_for_sound, f_making_robot_path_invalid_list)) {
          break;
        }
      }
    }

    if (i >= BOA_num_connect[start_room - Highest_room_index - 1])
      i = -1;
  }

  return i;
}

bool BOA_ComputeMinDist(int start_room, int end_room, float max_check_dist, float *dist, int *num_blockages) {
  *dist = 0.0f;

  start_room = BOA_INDEX(start_room);
  end_room = BOA_INDEX(end_room);

  if (start_room == end_room) {
    return true;
  }

  if (start_room == Highest_room_index + 1 && end_room > Highest_room_index) {
    return true;
  }

  if (end_room == Highest_room_index + 1 && start_room > Highest_room_index) {
    return true;
  }

  int cur_room = end_room;
  int last_room;

  if (start_room < 0 || end_room < 0) {
    return false;
  }

  if (start_room > Highest_room_index + BOA_num_terrain_regions ||
      end_room > Highest_room_index + BOA_num_terrain_regions) {
    return false;
  }

  if (start_room <= Highest_room_index && !Rooms[start_room].used)
    return false;

  if (end_room <= Highest_room_index && !Rooms[end_room].used)
    return false;

  do {
    last_room = cur_room;

    if (cur_room <= Highest_room_index && num_blockages && (Rooms[cur_room].flags & RF_DOOR) &&
        (cur_room != end_room)) {
      float door_position = DoorwayGetPosition(&Rooms[cur_room]);

      *num_blockages += 1.0f - door_position;
    }

    cur_room = BOA_NEXT_ROOM(cur_room, start_room);
    int last_portal;

    if (last_room == cur_room || cur_room == BOA_NO_PATH)
      return false;

    if (BOA_INDEX(last_room) != BOA_INDEX(cur_room)) {
      last_portal = BOA_DetermineStartRoomPortal(last_room, NULL, cur_room, NULL);
    }

    if (last_room == end_room) {
      int this_portal = BOA_DetermineStartRoomPortal(cur_room, NULL, last_room, NULL);

      if (cur_room != start_room && this_portal >= 0)
        *dist += BOA_cost_array[cur_room][this_portal];
      if (max_check_dist > 0.0 && max_check_dist < *dist)
        return false;
    } else if (cur_room == start_room) {
      *dist += BOA_cost_array[last_room][last_portal];
      if (max_check_dist > 0.0 && max_check_dist < *dist)
        return false;
    } else if ((cur_room != last_room) && (cur_room != BOA_NO_PATH)) {
      int this_portal = BOA_DetermineStartRoomPortal(cur_room, NULL, last_room, NULL);
      if (last_portal >= 0 && this_portal >= 0) {

        *dist += BOA_cost_array[last_room][last_portal] + BOA_cost_array[cur_room][this_portal];
        if (max_check_dist > 0.0f && max_check_dist < *dist)
          return false;
      }
    }

  } while ((cur_room != start_room) && (cur_room != last_room) && (cur_room != BOA_NO_PATH));

  if (cur_room == BOA_NO_PATH) {
    return false;
  }
  return true;
}

bool BOA_IsSoundAudible(int start_room, int end_room) {
  int s_index = start_room;
  int e_index = end_room;

  if (start_room == -1 || end_room == -1) {
    return false;
  }

  if ((!ROOMNUM_OUTSIDE(s_index)) && s_index <= Highest_room_index) {
    if (!Rooms[s_index].used) {
      return false;
    }
  } else if (ROOMNUM_OUTSIDE(s_index)) {
    s_index = TERRAIN_REGION(start_room) + Highest_room_index + 1;
  } else {
    ASSERT(s_index <= Highest_room_index + MAX_BOA_TERRAIN_REGIONS);
  }

  if ((!ROOMNUM_OUTSIDE(e_index)) && e_index <= Highest_room_index) {
    if (!Rooms[e_index].used) {
      return false;
    }
  } else if (ROOMNUM_OUTSIDE(e_index)) {
    e_index = TERRAIN_REGION(end_room) + Highest_room_index + 1;
  } else {
    ASSERT(e_index <= Highest_room_index + MAX_BOA_TERRAIN_REGIONS);
  }

  return ((BOA_Array[s_index][e_index] & BOA_SOUND_PROP) != 0);
}

bool BOA_HasPossibleBlockage(int start_room, int end_room) {
  int s_index = start_room;
  int e_index = end_room;

  if (start_room == -1 || end_room == -1) {
    return false;
  }

  if ((!ROOMNUM_OUTSIDE(s_index)) && s_index <= Highest_room_index) {
    if (!Rooms[s_index].used) {
      return false;
    }
  } else if (ROOMNUM_OUTSIDE(s_index)) {
    s_index = TERRAIN_REGION(start_room) + Highest_room_index + 1;
  } else {
    ASSERT(s_index <= Highest_room_index + MAX_BOA_TERRAIN_REGIONS);
  }

  if ((!ROOMNUM_OUTSIDE(e_index)) && e_index <= Highest_room_index) {
    if (!Rooms[e_index].used) {
      return false;
    }
  } else if (ROOMNUM_OUTSIDE(e_index)) {
    e_index = TERRAIN_REGION(end_room) + Highest_room_index + 1;
  } else {
    ASSERT(e_index <= Highest_room_index + MAX_BOA_TERRAIN_REGIONS);
  }

  return ((BOA_Array[s_index][e_index] & BOAF_BLOCKAGE) != 0);
}
bool BOA_IsVisible(int start_room, int end_room) {
  int s_index = start_room;
  int e_index = end_room;

  if (!BOA_vis_valid)
    return true;

  if (start_room == end_room)
    return true;

  if (start_room == -1 || end_room == -1) {
    return false;
  }

  if ((!ROOMNUM_OUTSIDE(s_index)) && s_index <= Highest_room_index) {
    if (!Rooms[s_index].used) {
      return false;
    }
  } else if (ROOMNUM_OUTSIDE(s_index)) {
    s_index = TERRAIN_REGION(start_room) + Highest_room_index + 1;
  } else {
    ASSERT(s_index <= Highest_room_index + MAX_BOA_TERRAIN_REGIONS);
  }

  if ((!ROOMNUM_OUTSIDE(e_index)) && e_index <= Highest_room_index) {
    if (!Rooms[e_index].used) {
      return false;
    }
  } else if (ROOMNUM_OUTSIDE(e_index)) {
    e_index = TERRAIN_REGION(end_room) + Highest_room_index + 1;
  } else {
    ASSERT(e_index <= Highest_room_index + MAX_BOA_TERRAIN_REGIONS);
  }

  return ((BOA_Array[s_index][e_index] & BOAF_VIS) != 0);
}

int BOA_GetNextRoom(int start_room, int end_room) {
  int s_index = start_room;
  int e_index = end_room;

  if (start_room == -1 || end_room == -1) {
    return BOA_NO_PATH;
  }

  if ((!ROOMNUM_OUTSIDE(s_index)) && s_index <= Highest_room_index) {
    if (!Rooms[s_index].used) {
      return BOA_NO_PATH;
    }
  } else if (ROOMNUM_OUTSIDE(s_index)) {
    s_index = TERRAIN_REGION(start_room) + Highest_room_index + 1;
  } else {
    ASSERT(s_index <= Highest_room_index + MAX_BOA_TERRAIN_REGIONS);
  }

  if ((!ROOMNUM_OUTSIDE(e_index)) && e_index <= Highest_room_index) {
    if (!Rooms[e_index].used) {
      return BOA_NO_PATH;
    }
  } else if (ROOMNUM_OUTSIDE(e_index)) {
    e_index = TERRAIN_REGION(end_room) + Highest_room_index + 1;
  } else {
    ASSERT(e_index <= Highest_room_index + MAX_BOA_TERRAIN_REGIONS);
  }

  return ((BOA_Array[s_index][e_index] & BOA_ROOM_MASK));
}

void add_mine_room(int room, int mine, char *checked) {
  int i;

  Rooms[room].flags |= (mine << 20);
  checked[room] = 1;

  for (i = 0; i < Rooms[room].num_portals; i++) {
    if (Rooms[room].portals[i].croom >= 0 && !checked[Rooms[room].portals[i].croom]) {
      add_mine_room(Rooms[room].portals[i].croom, mine, checked);
    }
  }
}

void compute_mine_info() {
  int i;
  char checked[MAX_ROOMS];
  bool done = false;
  int first_free;
  int cur_mine = 0;

  for (i = 0; i <= Highest_room_index; i++) {
    room *rp = &Rooms[i];

    if (rp->used) {
      rp->flags &= ~RFM_MINE;
    }

    checked[i] = 0;
  }

  while (!done) {
    ASSERT(cur_mine < 32);

    done = true;

    for (i = 0; i <= Highest_room_index; i++) {
      room *rp = &Rooms[i];

      if (rp->used && !checked[i]) {
        first_free = i;
        done = false;
        break;
      }
    }

    if (!done) {
      add_mine_room(first_free, cur_mine, checked);
      cur_mine++;
    }
  }

  BOA_num_mines = cur_mine;
}

void add_terrain_cell(int cell, int t_region, char *checked) {
  int depth = 0;
  int i;
  uint16_t stack[TERRAIN_WIDTH * TERRAIN_DEPTH];
  char on_stack[TERRAIN_WIDTH * TERRAIN_DEPTH];

  for (i = 0; i < TERRAIN_WIDTH * TERRAIN_DEPTH; i++)
    on_stack[i] = false;

  stack[depth++] = cell;
  on_stack[cell] = true;

  while (depth > 0) {
    cell = stack[--depth];
    Terrain_seg[cell].flags |= (t_region << 5);
    checked[cell] = 1;

    int xcounter, ycounter;

    int xstart = cell % TERRAIN_WIDTH - 1;
    int xend = cell % TERRAIN_WIDTH + 1;
    int ystart = cell / TERRAIN_WIDTH - 1;
    int yend = cell / TERRAIN_WIDTH + 1;

    if (xstart < 0)
      xstart = 0;
    if (xend >= TERRAIN_WIDTH)
      xend = TERRAIN_WIDTH - 1;
    if (ystart < 0)
      ystart = 0;
    if (yend >= TERRAIN_DEPTH)
      yend = TERRAIN_DEPTH - 1;

    // This should be a faster iterative way to do a square with center at original position
    int cur_node = TERRAIN_WIDTH * ystart + xstart;
    int next_y_delta = TERRAIN_WIDTH - (xend - xstart) - 1;

    for (ycounter = ystart; ycounter <= yend; ycounter++) {
      for (xcounter = xstart; xcounter <= xend; xcounter++) {
        if (!on_stack[cur_node] && !checked[cur_node] && Terrain_seg[cur_node].y < MAX_TERRAIN_HEIGHT) {
          stack[depth++] = cur_node;
          on_stack[cur_node] = true;
        }
        cur_node += 1;
      }

      cur_node += next_y_delta;
    }
  }
}

void compute_terrain_region_info() {
  int i;
  bool done = false;
  bool f_warning = false;
  char checked[TERRAIN_WIDTH * TERRAIN_DEPTH];

  for (i = 0; i < TERRAIN_WIDTH * TERRAIN_DEPTH; i++) {
    Terrain_seg[i].flags &= (~TFM_REGION_MASK);
    ASSERT((Terrain_seg[i].flags & TFM_REGION_MASK) == 0);
    checked[i] = 0;
  }

#ifdef _DEBUG
  for (i = 0; i < TERRAIN_WIDTH * TERRAIN_DEPTH; i++) {
    ASSERT((Terrain_seg[i].flags & TFM_REGION_MASK) == 0);
    ASSERT(((Terrain_seg[i].flags & TFM_REGION_MASK) >> 5) == 0);
  }
#endif

  // Find saturated points
  for (i = 0; i < TERRAIN_WIDTH * TERRAIN_DEPTH; i++) {
    if (Terrain_seg[i].y > MAX_TERRAIN_HEIGHT - 6.0f) {
      checked[i] = 1;
    }
  }

  char t_region = 1;

  while (!done) {
    if (t_region >= MAX_BOA_TERRAIN_REGIONS) {
#ifdef EDITOR
      OutrageMessageBox("This terrain has too many regions!\nAI will not work correctly outside!\nSee Chris and-or "
                        "saturate useless areas!");
#endif
      break;
    }

    int first_free;

    done = true;

    for (i = 0; i < TERRAIN_WIDTH * TERRAIN_DEPTH; i++) {
      if (!checked[i]) {
        first_free = i;
        done = false;
        break;
      }
    }

    if (!done) {
      add_terrain_cell(first_free, t_region, checked);
      t_region++;
    }
  }

  BOA_num_terrain_regions = t_region;

#ifdef _DEBUG
  for (i = 0; i < TERRAIN_WIDTH * TERRAIN_DEPTH; i++) {
    ASSERT(TERRAIN_REGION(i) < BOA_num_terrain_regions);
  }
#endif

  for (i = 0; i < MAX_BOA_TERRAIN_REGIONS; i++) {
    BOA_num_connect[i] = 0;
  }

  for (i = 0; i <= Highest_room_index; i++) {
    if ((Rooms[i].used) && (Rooms[i].flags & RF_EXTERNAL)) {
      int j;

      for (j = 0; j < Rooms[i].num_portals; j++) {
        int cell = GetTerrainCellFromPos(&Rooms[i].portals[j].path_pnt);
        ASSERT(cell != -1); // DAJ -1FIX

        int region = TERRAIN_REGION(cell);

        if (!(BOA_num_connect[region] < MAX_PATH_PORTALS)) {
          f_warning = true;
          break;
        }

        //				if(region != 0)
        //				{
        //					BOA_connect[0][BOA_num_connect[region]].roomnum =
        // Rooms[i].portals[j].croom; 					BOA_connect[0][BOA_num_connect[region]].portal =
        // Rooms[i].portals[j].cportal;
        //				}

        if (!(Rooms[Rooms[i].portals[j].croom].flags & RF_EXTERNAL)) {
          if (BOA_PassablePortal(i, j)) {
            BOA_connect[region][BOA_num_connect[region]].roomnum = Rooms[i].portals[j].croom;
            BOA_connect[region][BOA_num_connect[region]].portal = Rooms[i].portals[j].cportal;
            BOA_num_connect[region]++;
          }
        }
      }
    }
  }

#ifdef EDITOR
  if (f_warning) {
    OutrageMessageBox(
        "This terrain has too many fly through\nterrain-mine connections!\n\nAI will not work correctly outside!\nIf "
        "you really cannot fly outside\nignore this message.\n\nSee Chris for specific instructions!");
  }
#endif
}

#define MAX_SOUND_PROP_DIST 400.0f

void compute_sound_dist_info() {
  int i;
  int j;

  for (i = 0; i <= Highest_room_index + MAX_BOA_TERRAIN_REGIONS; i++) {
    for (j = 0; j <= i; j++) {
      BOA_Array[i][j] |= BOA_SOUND_PROP;
      BOA_Array[j][i] |= BOA_SOUND_PROP;

      if ((i > Highest_room_index || j > Highest_room_index) && (i != j)) {
        BOA_Array[i][j] &= ~BOA_SOUND_PROP;
        BOA_Array[j][i] &= ~BOA_SOUND_PROP;
      }
    }
  }

  for (i = 0; i <= Highest_room_index; i++) {
    for (j = 0; j < i; j++) {
      float dist;
      bool f_ok = BOA_ComputeMinDist(i, j, MAX_SOUND_PROP_DIST, &dist);

      if (!f_ok || dist > MAX_SOUND_PROP_DIST) {
        BOA_Array[i][j] &= ~BOA_SOUND_PROP;
        BOA_Array[j][i] &= ~BOA_SOUND_PROP;
      }
    }
  }

  for (i = 0; i < BOA_num_terrain_regions; i++) {
    int j;
    int k;

    for (j = 0; j < BOA_num_connect[i]; j++) {
      int croom = BOA_connect[i][j].roomnum;

      for (k = 0; k <= Highest_room_index + MAX_BOA_TERRAIN_REGIONS; k++) {
        if (BOA_Array[croom][k] & BOA_SOUND_PROP) {
          BOA_Array[Highest_room_index + i + 1][k] |= BOA_SOUND_PROP;
          BOA_Array[k][Highest_room_index + i + 1] |= BOA_SOUND_PROP;
        }
      }
    }
  }
}

void clear_BOA() {
  int i, j;

  for (i = 0; i <= Highest_room_index + MAX_BOA_TERRAIN_REGIONS; i++) {
    for (j = 0; j <= Highest_room_index + MAX_BOA_TERRAIN_REGIONS; j++) {
      BOA_Array[i][j] = i; // No flags are set and i to j points to i (so, no path exists)
    }
  }

  for (i = 0; i < MAX_ROOMS; i++) {
    BOA_AABB_ROOM_checksum[i] = 0;
  }

  BOA_num_mines = 0;
  BOA_num_terrain_regions = 0;
}

void compute_costs() {
  int i, j;
  vector from_pnt; //, to_pnt;
  vector portal_pnt;

  for (i = 0; i <= Highest_room_index; i++) {

    if (Rooms[i].used) {
      ASSERT(Rooms[i].num_portals <= MAX_PATH_PORTALS);

      for (j = 0; j < Rooms[i].num_portals; j++) {
        if (BOA_PassablePortal(i, j)) {
          ComputeRoomCenter(&from_pnt, &Rooms[i]);
          // ComputeRoomCenter(&to_pnt, &Rooms[Rooms[i].portals[j].croom]);

          ComputePortalCenter(&portal_pnt, &Rooms[i], j);

          BOA_cost_array[i][j] = vm_VectorDistance(&from_pnt, &portal_pnt);
        } else {
          BOA_cost_array[i][j] = -1.0;
        }
      }
    } else {
      for (j = 0; j < Rooms[i].num_portals; j++) {
        BOA_cost_array[i][j] = -1.0;
      }
    }
  }

  for (i = Highest_room_index + 1; i <= Highest_room_index + BOA_num_terrain_regions; i++) {
    int j;
    for (j = 0; j < BOA_num_connect[i]; j++) {
      BOA_cost_array[i][j] = 100000.0f;
    }
  }
}

void update_path_info(q_item *node_list[MAX_ROOMS], int start, int end) {
  int cur_room;
  int par_room;

  while (end != start) {
    cur_room = end;
    par_room = node_list[end]->parent;

    while (par_room != -1) {
      BOA_Array[par_room][end] = cur_room;

      cur_room = node_list[cur_room]->parent;
      par_room = node_list[cur_room]->parent;
    }

    end = node_list[end]->parent;
  }
}

void FindPath(int i, int j) {
  pq PQPath;
  int counter;
  q_item *start_node = new q_item(BOA_INDEX(i), -1, 0.0);
  q_item *cur_node;

  q_item *node_list[MAX_ROOMS + MAX_BOA_TERRAIN_REGIONS];

  if (i == -1 || j == -1) {
    delete start_node;
    return;
  }

  memset(node_list, 0, sizeof(q_item *) * (MAX_ROOMS + MAX_BOA_TERRAIN_REGIONS));

  PQPath.push(start_node);
  ASSERT(start_node->roomnum <= Highest_room_index + BOA_num_terrain_regions);

  while ((cur_node = PQPath.pop())) {
    node_list[BOA_INDEX(cur_node->roomnum)] = cur_node;
    ASSERT(BOA_INDEX(cur_node->roomnum) >= 0 &&
           BOA_INDEX(cur_node->roomnum) <= Highest_room_index + MAX_BOA_TERRAIN_REGIONS);

    if (cur_node->roomnum == j) {
      update_path_info(node_list, i, j);
      goto done;
    }

    if (i > j) {
      int num_portals;
      bool f_room = true;
      int t_index;

      if (cur_node->roomnum <= Highest_room_index) {
        num_portals = Rooms[cur_node->roomnum].num_portals;
      } else {
        t_index = cur_node->roomnum - Highest_room_index - 1;
        num_portals = BOA_num_connect[t_index];
        f_room = false;
      }

      for (counter = 0; counter < num_portals; counter++) {
        int next_room;
        q_item *list_item;
        float new_cost;

        if (!BOA_PassablePortal(cur_node->roomnum, counter))
          continue;

        if (f_room)
          next_room = Rooms[cur_node->roomnum].portals[counter].croom;
        else
          next_room = BOA_connect[t_index][counter].roomnum;

        if (next_room < 0 || next_room == BOA_NO_PATH)
          continue;

        if ((next_room <= Highest_room_index) && (Rooms[next_room].flags & RF_EXTERNAL)) {
          ASSERT(cur_node->roomnum <= Highest_room_index);

          int cell = GetTerrainCellFromPos(&Rooms[cur_node->roomnum].portals[counter].path_pnt);
          ASSERT(cell >= 0 && cell < TERRAIN_WIDTH * TERRAIN_DEPTH);

          next_room = Highest_room_index + TERRAIN_REGION(cell) + 1;
          ASSERT(next_room <= Highest_room_index + BOA_num_terrain_regions);
        }

        int next_portal;
        if (BOA_INDEX(next_room) != BOA_INDEX(cur_node->roomnum)) {
          next_portal = BOA_DetermineStartRoomPortal(next_room, NULL, cur_node->roomnum, NULL);
        }

        new_cost = cur_node->cost + BOA_cost_array[BOA_INDEX(cur_node->roomnum)][counter] +
                   BOA_cost_array[BOA_INDEX(next_room)][next_portal];

        list_item = node_list[BOA_INDEX(next_room)];
        if (list_item != NULL && list_item->cost <= new_cost)
          continue;

        if (list_item == NULL) {
          list_item = new q_item(BOA_INDEX(next_room), cur_node->roomnum, new_cost);
          node_list[BOA_INDEX(next_room)] = list_item;
          PQPath.push(list_item);
          ASSERT(list_item->roomnum <= Highest_room_index + BOA_num_terrain_regions);
        } else {
          list_item->cost = new_cost;
          list_item->parent = cur_node->roomnum;
        }
      }
    } else {
      int num_portals;
      bool f_room = true;
      int t_index;

      if (cur_node->roomnum <= Highest_room_index) {
        num_portals = Rooms[cur_node->roomnum].num_portals;
      } else {
        t_index = cur_node->roomnum - Highest_room_index - 1;
        num_portals = BOA_num_connect[t_index];
        f_room = false;
      }

      for (counter = 0; counter < num_portals; counter++) {
        int next_room;
        q_item *list_item;
        float new_cost;

        if (!BOA_PassablePortal(cur_node->roomnum, counter))
          continue;

        if (f_room)
          next_room = Rooms[cur_node->roomnum].portals[counter].croom;
        else
          next_room = BOA_connect[t_index][counter].roomnum;

        if (next_room < 0 || next_room == BOA_NO_PATH)
          continue;

        if ((next_room <= Highest_room_index) && (Rooms[next_room].flags & RF_EXTERNAL)) {
          ASSERT(cur_node->roomnum <= Highest_room_index);
          int cell = GetTerrainCellFromPos(&Rooms[cur_node->roomnum].portals[counter].path_pnt);
          ASSERT(cell != -1); // DAJ -1FIX
          next_room = Highest_room_index + TERRAIN_REGION(cell) + 1;
        }

        int next_portal;
        if (BOA_INDEX(next_room) != BOA_INDEX(cur_node->roomnum)) {
          next_portal = BOA_DetermineStartRoomPortal(next_room, NULL, cur_node->roomnum, NULL);
        }

        new_cost = cur_node->cost + BOA_cost_array[BOA_INDEX(cur_node->roomnum)][counter] +
                   BOA_cost_array[BOA_INDEX(next_room)][next_portal];

        list_item = node_list[BOA_INDEX(next_room)];
        if (list_item != NULL && list_item->cost <= new_cost)
          continue;

        if (list_item == NULL) {
          list_item = new q_item(BOA_INDEX(next_room), cur_node->roomnum, new_cost);
          node_list[BOA_INDEX(next_room)] = list_item;
          PQPath.push(list_item);
          ASSERT(list_item->roomnum <= Highest_room_index + BOA_num_terrain_regions);
        } else {
          list_item->cost = new_cost;
          list_item->parent = cur_node->roomnum;
        }
      }
    }
  }

  // Mark as an impossible path.
  BOA_Array[i][j] = BOA_NO_PATH;

done:
  for (counter = 0; counter <= Highest_room_index + MAX_BOA_TERRAIN_REGIONS; counter++) {
    if (node_list[counter])
      delete node_list[counter];
  }

  return;
}

void compute_next_segs() {
  int i, j;

  for (i = 0; i <= Highest_room_index + MAX_BOA_TERRAIN_REGIONS; i++) {
    if (i <= Highest_room_index && (!Rooms[i].used))
      continue;

    if (i <= Highest_room_index && (Rooms[i].flags & RF_EXTERNAL))
      continue;

    if (i > Highest_room_index + BOA_num_terrain_regions)
      continue;

    for (j = Highest_room_index + MAX_BOA_TERRAIN_REGIONS; j >= 0; j--) {
      if (j <= Highest_room_index && (!Rooms[j].used))
        continue;

      if (j <= Highest_room_index && (Rooms[j].flags & RF_EXTERNAL))
        continue;

      if (j > Highest_room_index + BOA_num_terrain_regions)
        continue;

      if (i == Highest_room_index + 1 && j > Highest_room_index) {
        BOA_Array[i][j] = j;
        BOA_Array[j][i] = i;
        continue;
      }

      if (j == Highest_room_index + 1 && i > Highest_room_index) {
        BOA_Array[i][j] = j;
        BOA_Array[j][i] = i;
        continue;
      }

      if (i != j && BOA_Array[i][j] == i)
        FindPath(i, j);
      if (i != j && BOA_Array[j][i] == j)
        FindPath(j, i);
    }
  }
}

void compute_blockage_info() {
  int i, j;

  for (i = 0; i <= Highest_room_index + BOA_num_terrain_regions; i++) {
    if (i <= Highest_room_index && (!Rooms[i].used))
      continue;

    if (i <= Highest_room_index && (Rooms[i].flags & RF_EXTERNAL))
      continue;

    if (i > Highest_room_index + BOA_num_terrain_regions)
      continue;

    for (j = 0; j <= Highest_room_index + BOA_num_terrain_regions; j++) {
      int cur_room = i;

      if (i == j)
        continue;

      if (i == Highest_room_index + 1 && j > Highest_room_index)
        continue;

      if (j == Highest_room_index + 1 && i > Highest_room_index)
        continue;

      if (j <= Highest_room_index && (!Rooms[i].used))
        continue;

      if (j <= Highest_room_index && (Rooms[i].flags & RF_EXTERNAL))
        continue;

      if (BOA_NEXT_ROOM(cur_room, j) != BOA_NO_PATH && BOA_NEXT_ROOM(cur_room, j) != cur_room) {
        int last_room = cur_room;

        do {
          if (cur_room <= Highest_room_index && (Rooms[cur_room].flags & RF_DOOR)) {
            BOA_Array[i][j] |= BOAF_BLOCKAGE;
            break;
          }

          last_room = cur_room;
          cur_room = BOA_NEXT_ROOM(cur_room, j);

          if (last_room != cur_room) {
            BOA_f_making_boa = false;
            if (BOA_DetermineStartRoomPortal(last_room, NULL, cur_room, NULL, true) == -1) {
              BOA_Array[i][j] |= BOAF_BLOCKAGE;
              BOA_f_making_boa = true;
              break;
            }
            BOA_f_making_boa = true;
          }

        } while (cur_room != j);
      }
    }
  }
}

// Goes through all the valid points in the indoor engine and returns a unique
// checksum
int BOAGetMineChecksum() {
  int i, t, k;
  int total = 0;

  for (i = 0; i <= Highest_room_index; i++) {
    room *rp = &Rooms[i];

    if (!Rooms[i].used)
      continue;

    for (t = 0; t < rp->num_faces; t++) {
      face *fp = &rp->faces[t];

      for (k = 0; k < fp->num_verts; k++) {
        int x, y, z;
        x = floor(rp->verts[fp->face_verts[k]].x());
        y = floor(rp->verts[fp->face_verts[k]].y());
        z = floor(rp->verts[fp->face_verts[k]].z());
        total += x + y + z;
      }

      total += fp->num_verts << 4;

      if (fp->portal_num != -1) {
        // factor in portal
        portal *pp = &rp->portals[fp->portal_num];

        total += fp->portal_num;

        int flags = pp->flags;

        flags &= (PF_BLOCK | PF_BLOCK_REMOVABLE | PF_RENDERED_FLYTHROUGH);

        total += flags;
      }
    }
    total += rp->num_faces << 8;

    total += (rp->num_portals) << (i % 16);

    total += i;
  }

  // Now do terrain
  for (i = 0; i < TERRAIN_WIDTH * TERRAIN_DEPTH; i++)
    total += Terrain_seg[i].ypos;

  total += BOA_VERSION << 24;

  LOG_INFO.printf("Computed Checksum is %d", (int)total);
  return total;
}

int BOAGetRoomChecksum(int i) {
  int t, k;
  float total = 0;

  {
    room *rp = &Rooms[i];

    if (!Rooms[i].used)
      return 0;

    for (t = 0; t < rp->num_faces; t++) {
      face *fp = &rp->faces[t];

      for (k = 0; k < fp->num_verts; k++) {
        total += rp->verts[fp->face_verts[k]].x();
        total += rp->verts[fp->face_verts[k]].y();
        total += rp->verts[fp->face_verts[k]].z();
      }

      total += fp->num_verts << 4;
    }
    total += rp->num_faces << 8;

    total += (rp->num_portals) << (i % 16);

    total += i;
  }

  total += BOA_VERSION << 24;

  return (int)total;
}

bool IsPathPointValid(int room, vector *pos) {
  // vector c_pnt = Rooms[room].path_pnt;
  int i;

  if (Rooms[room].flags & RF_EXTERNAL)
    return true;

  for (i = 0; i < Rooms[room].num_portals; i++) {
    fvi_info hit_info;
    fvi_query fq;
    int fate;

    if (!BOA_PassablePortal(room, i))
      continue;

    vector portal_pnt = Rooms[room].portals[i].path_pnt;

    // shoot a ray from the light position to the current vertex
    fq.p0 = &portal_pnt;
    fq.p1 = pos;
    fq.startroom = room;

    fq.rad = 5.0f;
    fq.flags =
        FQ_SOLID_PORTALS | FQ_NO_RELINK; // chrishack -- Might want to make FQ_IGNORE_MOVING_OBJECTS into a passed arg
    fq.thisobjnum = -1;
    fq.ignore_obj_list = NULL;

    fate = fvi_FindIntersection(&fq, &hit_info);

    if (fate != HIT_NONE) {
      return false;
    }
  }

  return true;
}

#define MAX_SUBDIVISIONS 12 // Actually 19 (we don't do extremities)

void ValidateRoomPathPoint(int room, char *message, int len) {
  vector pos;
  int i, j, k;

  pos = Rooms[room].path_pnt;

  if (IsPathPointValid(room, &pos))
    return;

  // vector best_pnt;
  // float best_distance;

  vector diff = (Rooms[room].max_xyz - Rooms[room].min_xyz) / MAX_SUBDIVISIONS;

  for (i = 1; i < MAX_SUBDIVISIONS - 1; i++) {
    for (j = 1; j < MAX_SUBDIVISIONS - 1; j++) {
      for (k = 1; k < MAX_SUBDIVISIONS - 1; k++) {
        vector t_pnt = Rooms[room].min_xyz;
        t_pnt.x() += diff.x() * i;
        t_pnt.y() += diff.y() * j;
        t_pnt.z() += diff.z() * k;

        if (IsPathPointValid(room, &t_pnt)) {
          Rooms[room].path_pnt = t_pnt;
          return;
        }
      }
    }
  }

  if (!message)
    LOG_WARNING.printf("Room %d has a bad center point", room);
  else {
    char new_message[300];
    snprintf(new_message, sizeof(new_message), "Room %d has a bad center point\n", room);

    if (strlen(message) + strlen(new_message) < (uint32_t)len) {
      strcat(message, new_message);
    }
  }
}

void BOA_ComputePathPoints(char *message, int len) {
  int i;
  int j;

  for (i = 0; i <= Highest_room_index; i++) {
    if (Rooms[i].used) {
      for (j = 0; j < Rooms[i].num_portals; j++) {
        ComputePortalCenter(&Rooms[i].portals[j].path_pnt, &Rooms[i], j);
      }

      if (!(Rooms[i].flags & RF_MANUAL_PATH_PNT)) {
        Rooms[i].path_pnt = (Rooms[i].max_xyz + Rooms[i].min_xyz) / 2.0f;
        ValidateRoomPathPoint(i, message, len);
      }
    }
  }
}

// Given a face, computes the upper left corner of the face
void ComputeBOAVisFaceUpperLeft(room *rp, face *fp, vector *upper_left, float *xdiff, float *ydiff, vector *center) {
  matrix face_matrix, trans_matrix;
  vector fvec;
  vector avg_vert;
  vector verts[MAX_VERTS_PER_FACE];
  vector rot_vert;
  int i;

  // find the center point of this face
  vm_MakeZero(&avg_vert);
  for (i = 0; i < fp->num_verts; i++)
    avg_vert += rp->verts[fp->face_verts[i]];

  avg_vert /= fp->num_verts;

  // Make the orientation matrix
  // Reverse the normal because we're looking "at" the face, not from it
  fvec = -fp->normal;

  vm_VectorToMatrix(&face_matrix, &fvec, NULL, NULL);
  // Make the transformation matrix

  angvec avec;
  vm_ExtractAnglesFromMatrix(&avec, &face_matrix);
  vm_AnglesToMatrix(&trans_matrix, avec.p(), avec.h(), avec.b());

  // Rotate all the points
  for (i = 0; i < fp->num_verts; i++) {
    vector vert = rp->verts[fp->face_verts[i]];

    vert -= avg_vert;
    vm_MatrixMulVector(&rot_vert, &vert, &trans_matrix);

    verts[i] = rot_vert;
  }

  // Find left most point
  int leftmost_point = -1;
  float leftmost_x = 900000.00f; // a big number

  for (i = 0; i < fp->num_verts; i++) {
    if (verts[i].x() < leftmost_x) {
      leftmost_point = i;
      leftmost_x = verts[i].x();
    }
  }

  ASSERT(leftmost_point != -1);

  // Find top most point
  int topmost_point = -1;
  float topmost_y = -900000.0f; // a big number

  for (i = 0; i < fp->num_verts; i++) {
    if (verts[i].y() > topmost_y) {
      topmost_point = i;
      topmost_y = verts[i].y();
    }
  }

  ASSERT(topmost_point != -1);

  // Find right most point
  int rightmost_point = -1;
  float rightmost_x = -900000.00f; // a big number

  for (i = 0; i < fp->num_verts; i++) {
    if (verts[i].x() > rightmost_x) {
      rightmost_point = i;
      rightmost_x = verts[i].x();
    }
  }

  ASSERT(rightmost_point != -1);

  // Find bottom most point
  int bottommost_point = -1;
  float bottommost_y = 900000.0f; // a big number

  for (i = 0; i < fp->num_verts; i++) {
    if (verts[i].y() < bottommost_y) {
      bottommost_point = i;
      bottommost_y = verts[i].y();
    }
  }

  ASSERT(bottommost_point != -1);

  // now set the base vertex, which is where we base uv 0,0 on

  vector base_vector;

  base_vector.x() = verts[leftmost_point].x();
  base_vector.y() = verts[topmost_point].y();
  base_vector.z() = 0;

  // Figure out grid resolution
  if (xdiff)
    *xdiff = verts[rightmost_point].x() - verts[leftmost_point].x();

  if (ydiff)
    *ydiff = verts[topmost_point].y() - verts[bottommost_point].y();

  // Find upper left corner
  vm_TransposeMatrix(&trans_matrix);
  vm_MatrixMulVector(&rot_vert, &base_vector, &trans_matrix);

  if (upper_left)
    *upper_left = rot_vert + avg_vert;

  if (center)
    *center = avg_vert;
}

#if (defined(EDITOR) || defined(NEWEDITOR))

#ifdef NEWEDITOR
#include "neweditor/stdafx.h"
#include "neweditor/resource.h"
#include "neweditor/ProgressDialog.h"

// Displays/Updates the Visibility Progress bar
// state = 0 //create
// state = 1 //update
// state = 2 //destroy
void DoBOAVisProgressDialog(float percentage, int state, char *message = NULL) {
  int pos = percentage * 100.0f;
  if (pos < 0)
    pos = 0;
  if (pos > 100)
    pos = 100;

  static CProgressDialog *gProgressDialog = NULL;
  switch (state) {
  case 0: {
    gProgressDialog = new CProgressDialog;

    gProgressDialog->Create(IDD_LOADLEVELPROGRESS, NULL);
    gProgressDialog->m_TitleText = message;
    gProgressDialog->ShowWindow(SW_SHOW);

    gProgressDialog->UpdateData(false);
    gProgressDialog->m_ProgressBar.SetStep(100);
    gProgressDialog->m_ProgressBar.SetPos(pos);

  } break;

  case 1: {
    if (gProgressDialog && gProgressDialog->m_hWnd) {
      gProgressDialog->m_ProgressBar.SetPos(pos);
      defer();
    }
  } break;

  case 2: {
    if (gProgressDialog && gProgressDialog->m_hWnd) {
      gProgressDialog->DestroyWindow();
      delete gProgressDialog;
      gProgressDialog = NULL;
    }

  } break;
  }
}
#endif

// Higher res means longer vis times :(
#define VIS_TABLE_RESOLUTION 6
// Goes through all the rooms and determines their visibility in relation to one another
void MakeBOAVisTable(bool from_lighting) {
  int cur_check = BOAGetMineChecksum();
  int vis_stack[MAX_ROOMS * 10];
  int stack_count = 0;
  uint8_t already_checked[MAX_ROOMS + MAX_BOA_TERRAIN_REGIONS];
  uint8_t precomputed[MAX_ROOMS + MAX_BOA_TERRAIN_REGIONS][MAX_ROOMS + MAX_BOA_TERRAIN_REGIONS];

  // Removed this as we have to make a better checksum

  if (cur_check == BOA_vis_checksum) {
    // Already done, so bail
    if (!from_lighting)
      EditorMessageBox("BOA vis table already computed.");
    return;
  }

  BOA_vis_checksum = BOA_mine_checksum = 0;
#ifdef NEWEDITOR
  DoBOAVisProgressDialog(0.0f, 0, "Computing BOA");
  DoBOAVisProgressDialog(0.0f, 1);
#endif
  MakeBOA();
#ifdef NEWEDITOR
  DoBOAVisProgressDialog(100.0f, 2);
#endif

  // Now compute all room to room visibility stuff
  int i, t, j;

  LOG_DEBUG.printf("Computing visibility for %d rooms.", Highest_room_index);

  for (i = 0; i <= Highest_room_index + MAX_BOA_TERRAIN_REGIONS; i++) {
    for (t = 0; t <= Highest_room_index + MAX_BOA_TERRAIN_REGIONS; t++) {
      BOA_Array[i][t] &= ~BOAF_VIS;
      precomputed[i][t] = 255;
    }

    BOA_Array[i][i] |= BOAF_VIS;
  }

#ifdef NEWEDITOR
  DoBOAVisProgressDialog(0.0f, 0, "Computing BOA Vis. Table");
#endif

  for (i = 0; i <= Highest_room_index; i++) {
    if (Rooms[i].used == 0)
      continue;

    room *rp = &Rooms[i];

#ifdef NEWEDITOR
    DoBOAVisProgressDialog((float)(i + 1) / (float)(Highest_room_index + 1), 1);
#endif
    if (rp->flags & RF_EXTERNAL)
      continue;

    // Clear the already_checked array
    memset(already_checked, 0, MAX_ROOMS + MAX_BOA_TERRAIN_REGIONS);

    already_checked[i] = 1; // don't check self

    // Make all the rooms connecting to this room automatically visible
    // Make all the rooms connecting to this room automatically visible
    for (t = 0; t < rp->num_portals; t++) {
      int croom = rp->portals[t].croom;

      if (croom >= 0) {
        BOA_Array[i][croom] |= BOAF_VIS;
        already_checked[croom] = 1;

        // If this is an external room, mark the terrain as visible
        if (Rooms[croom].flags & RF_EXTERNAL) {
          int cp;
          int xxx;

          BOA_Array[i][Highest_room_index + 1] |= BOAF_VIS;

          for (xxx = 0; xxx < Rooms[croom].num_portals; xxx++) {
            int cell = GetTerrainCellFromPos(&Rooms[croom].portals[xxx].path_pnt);
            ASSERT(cell != -1); // DAJ -1FIX
            int region = TERRAIN_REGION(cell);

            BOA_Array[i][Highest_room_index + region + 1] |= BOAF_VIS;
          }

          for (cp = 0; cp <= Highest_room_index; cp++) {
            if (Rooms[cp].used && (Rooms[cp].flags & RF_EXTERNAL) && !already_checked[cp]) {
              vis_stack[stack_count] = cp;
              stack_count++;
              ASSERT(stack_count < MAX_ROOMS * 10);
            }
          }
        }
      }
    }

    // Now push all of the connecting rooms portals onto the stack
    for (t = 0; t < rp->num_portals; t++) {
      int roomnum = rp->portals[t].croom;
      for (j = 0; j < Rooms[roomnum].num_portals; j++) {
        int croom = Rooms[roomnum].portals[j].croom;

        if (croom >= 0 && !already_checked[croom]) {
          vis_stack[stack_count] = croom;
          stack_count++;
          ASSERT(stack_count < MAX_ROOMS * 10);
        }
      }
    }

    // Check visibility until we are done with this room
    while (stack_count > 0) {
      int check_room = vis_stack[--stack_count];
      if (already_checked[check_room])
        continue;

      already_checked[check_room] = 1;

      if (precomputed[check_room][i] != 255) {
        precomputed[i][check_room] = precomputed[check_room][i];
        if (precomputed[i][check_room] == 1) {
          BOA_Array[i][check_room] |= BOAF_VIS;

          // if this portal can be seen, add all its portals to the stack
          // and then set this room to be visible
          for (int tk = 0; tk < Rooms[check_room].num_portals; tk++) {
            int croom = Rooms[check_room].portals[tk].croom;
            if (croom >= 0) {
              vis_stack[stack_count] = croom;
              stack_count++;
              ASSERT(stack_count < MAX_ROOMS * 10);
            }
          }
        }

        continue;
      }

      int done = 0;

      for (t = 0; t < rp->num_portals && !done; t++) {
        face *src_fp = &rp->faces[rp->portals[t].portal_face];
        float src_width, src_height;
        vector src_upper_left, src_center;
        matrix src_matrix;
        vector src_verts[MAX_VERTS_PER_FACE], *src_vertp[MAX_VERTS_PER_FACE];

        for (j = 0; j < src_fp->num_verts; j++) {
          src_verts[j] = rp->verts[src_fp->face_verts[j]];
          src_vertp[j] = &src_verts[j];
        }

        vector fvec = -src_fp->normal;
        vm_VectorToMatrix(&src_matrix, &fvec, NULL, NULL);

        ComputeBOAVisFaceUpperLeft(rp, src_fp, &src_upper_left, &src_width, &src_height, &src_center);

        if (src_width > VIS_TABLE_RESOLUTION) {
          float num = src_width / VIS_TABLE_RESOLUTION;
          src_width = VIS_TABLE_RESOLUTION;

          src_upper_left += (src_matrix.rvec * (num / 2));
          src_matrix.rvec *= num;
        }

        if (src_height > VIS_TABLE_RESOLUTION) {
          float num = src_height / VIS_TABLE_RESOLUTION;
          src_height = VIS_TABLE_RESOLUTION;

          src_upper_left -= (src_matrix.uvec * (num / 2));
          src_matrix.uvec *= num;
        }

        for (j = 0; j < Rooms[check_room].num_portals && !done; j++) {
          face *dest_fp = &Rooms[check_room].faces[Rooms[check_room].portals[j].portal_face];
          float dest_width, dest_height;
          vector dest_upper_left, dest_center;
          matrix dest_matrix;
          vector dest_verts[MAX_VERTS_PER_FACE], *dest_vertp[MAX_VERTS_PER_FACE];

          for (int tj = 0; tj < dest_fp->num_verts; tj++) {
            dest_verts[tj] = Rooms[check_room].verts[dest_fp->face_verts[tj]];
            dest_vertp[tj] = &dest_verts[tj];
          }

          fvec = -dest_fp->normal;
          vm_VectorToMatrix(&dest_matrix, &fvec, NULL, NULL);

          ComputeBOAVisFaceUpperLeft(&Rooms[check_room], dest_fp, &dest_upper_left, &dest_width, &dest_height,
                                     &dest_center);

          if (dest_width > VIS_TABLE_RESOLUTION) {
            float num = dest_width / VIS_TABLE_RESOLUTION;
            dest_width = VIS_TABLE_RESOLUTION;

            dest_upper_left += (dest_matrix.rvec * (num / 2));
            dest_matrix.rvec *= num;
          }

          if (dest_height > VIS_TABLE_RESOLUTION) {
            float num = dest_height / VIS_TABLE_RESOLUTION;
            dest_height = VIS_TABLE_RESOLUTION;

            dest_upper_left -= (dest_matrix.uvec * (num / 2));
            dest_matrix.uvec *= num;
          }

          vector src_vector, src_ybase;
          src_ybase = src_upper_left;

          for (int sy = 0; sy < src_height && !done; sy++, src_ybase -= src_matrix.uvec) {
            src_vector = src_ybase;

            for (int sx = 0; sx < src_width && !done; sx++, src_vector += src_matrix.rvec) {
              vector src2 = src_vector;
              vector subvec;

              if ((check_point_to_face(&src2, &src_fp->normal, src_fp->num_verts, src_vertp)))
                continue;

              vector dest_vector, dest_ybase;
              dest_ybase = dest_upper_left;

              for (int dy = 0; dy < dest_height && !done; dy++, dest_ybase -= dest_matrix.uvec) {
                dest_vector = dest_ybase;

                for (int dx = 0; dx < dest_width && !done; dx++, dest_vector += dest_matrix.rvec) {
                  src2 += 0.1f * rp->faces[rp->portals[t].portal_face].normal;
                  dest_vector += 0.1f * Rooms[check_room].faces[Rooms[check_room].portals[j].portal_face].normal;

                  subvec = dest_vector - src2;
                  if (vm_Dot3Product(subvec, src_fp->normal) > 0.0f)
                    continue;
                  /*subvec=src2-dest_vector;
                  if (subvec * dest_fp->normal>0)
                          continue;*/

                  if ((check_point_to_face(&dest_vector, &dest_fp->normal, dest_fp->num_verts, dest_vertp)))
                    continue;

                  if (!fvi_QuickRoomCheck(&src2, rp, false) && !fvi_QuickRoomCheck(&src2, rp, true))
                    continue;

                  if (!(Rooms[check_room].flags & RF_EXTERNAL) &&
                      (!fvi_QuickRoomCheck(&dest_vector, &Rooms[check_room], false) &&
                       !fvi_QuickRoomCheck(&dest_vector, &Rooms[check_room], true)))
                    continue;

                  // Check to see if we can see to this portal point
                  fvi_query fq;
                  fvi_info hit_data;
                  int fate;

                  fq.p0 = &src2;
                  fq.startroom = i;
                  fq.p1 = &dest_vector;
                  fq.rad = .01f;
                  fq.thisobjnum = -1;
                  fq.ignore_obj_list = NULL;
                  fq.flags =
                      FQ_BACKFACE |
                      FQ_IGNORE_RENDER_THROUGH_PORTALS; // Gaurentees in weird geometry cases (as the ray will hit as it
                                                        // comes back in the mine) that the ray cannot start outside the
                                                        // mine like with a slightly non-planar portal or other non-nice
                                                        // situations like zero-width door portal/room-face combos

                  fate = fvi_FindIntersection(&fq, &hit_data);
                  if (fate == HIT_NONE) {
                    // if this portal can be seen, add all its portals to the stack
                    // and then set this room to be visible
                    for (int k = 0; k < Rooms[check_room].num_portals; k++) {
                      int croom = Rooms[check_room].portals[k].croom;
                      if (croom >= 0) {
                        vis_stack[stack_count] = croom;
                        stack_count++;
                        ASSERT(stack_count < MAX_ROOMS * 10);
                      }
                    }

                    BOA_Array[i][check_room] |= BOAF_VIS;
                    precomputed[i][check_room] = 1;

                    if (Rooms[check_room].flags & RF_EXTERNAL) {
                      int cp;
                      int xxx;

                      BOA_Array[i][Highest_room_index + 1] |= BOAF_VIS;

                      for (xxx = 0; xxx < Rooms[check_room].num_portals; xxx++) {
                        int cell = GetTerrainCellFromPos(&Rooms[check_room].portals[xxx].path_pnt);
                        ASSERT(cell != -1); // DAJ -1FIX
                        int region = TERRAIN_REGION(cell);

                        BOA_Array[i][Highest_room_index + region + 1] |= BOAF_VIS;
                      }

                      for (cp = 0; cp <= Highest_room_index; cp++) {
                        if (Rooms[cp].used && (Rooms[cp].flags & RF_EXTERNAL) && !already_checked[cp]) {
                          vis_stack[stack_count] = cp;
                          stack_count++;
                          ASSERT(stack_count < MAX_ROOMS * 10);
                        }
                      }
                    }

                    done = 1;
                  }

                  src2 -= 0.1f * rp->faces[rp->portals[t].portal_face].normal;
                  dest_vector -= 0.1f * Rooms[check_room].faces[Rooms[check_room].portals[j].portal_face].normal;
                }
              }
            }
          }
        }
      }

      if (done == 0)
        precomputed[i][check_room] = 0;
    }
  }

  for (i = 0; i <= Highest_room_index; i++) {
    if (Rooms[i].used == 0)
      continue;

    for (t = 0; t <= Highest_room_index + MAX_BOA_TERRAIN_REGIONS; t++) {
      if ((BOA_Array[i][t] & BOAF_VIS)) {
        BOA_Array[t][i] |= BOAF_VIS;
      }
    }
  }

#ifdef NEWEDITOR
  DoBOAVisProgressDialog(100.0f, 2);
#endif

  if (!from_lighting)
    EditorMessageBox("BOA vis table computed!");

  BOA_vis_checksum = BOA_mine_checksum;
  BOA_vis_valid = 1;
}
#endif

void verify_connections() {
  int i;
  int j;

  for (i = 0; i <= Highest_room_index + BOA_num_terrain_regions; i++) {
    if (i <= Highest_room_index && (!Rooms[i].used))
      continue;

    if (i <= Highest_room_index && (Rooms[i].flags & RF_EXTERNAL))
      continue;

    for (j = 0; j <= Highest_room_index + BOA_num_terrain_regions; j++) {
      if (j <= Highest_room_index && (!Rooms[j].used))
        continue;

      if (j <= Highest_room_index && (Rooms[j].flags & RF_EXTERNAL))
        continue;

      if (i == Highest_room_index + 1 && j > Highest_room_index) {
        continue;
      }

      if (j == Highest_room_index + 1 && i > Highest_room_index) {
        continue;
      }

      if (i == j)
        continue;

      int next_room = BOA_NEXT_ROOM(i, j);

      if (next_room != i && next_room != BOA_NO_PATH) {
        int portal;
        portal = BOA_DetermineStartRoomPortal(i, NULL, next_room, NULL);
        portal = BOA_DetermineStartRoomPortal(next_room, NULL, i, NULL);
      }
    }
  }
}

void find_small_portals() {
  int i, j;
  int counter = 0;

  for (i = 0; i <= Highest_room_index; i++) {
    if (Rooms[i].used) {
      for (j = 0; j < Rooms[i].num_portals; j++) {
        face *fp = &Rooms[i].faces[Rooms[i].portals[j].portal_face];

        float xdiff;
        float ydiff;
        ComputeBOAVisFaceUpperLeft(&Rooms[i], fp, NULL, &xdiff, &ydiff, NULL);

        if (xdiff < 6.0f || ydiff < 6.0f) {
          counter++;
          Rooms[i].portals[j].flags |= PF_TOO_SMALL_FOR_ROBOT;
        } else {
          Rooms[i].portals[j].flags &= (~PF_TOO_SMALL_FOR_ROBOT);
        }
      }
    }
  }

  LOG_DEBUG.printf("   Found %d small portals...  :)\n", counter);
}

void compute_robot_path_info() {
  int i, j;

  for (i = 0; i <= Highest_room_index + BOA_num_terrain_regions; i++) {
    if (i <= Highest_room_index && (!Rooms[i].used))
      continue;

    if (i <= Highest_room_index && (Rooms[i].flags & RF_EXTERNAL))
      continue;

    if (i > Highest_room_index + BOA_num_terrain_regions)
      continue;

    for (j = 0; j <= Highest_room_index + BOA_num_terrain_regions; j++) {
      int cur_room = i;

      if (j <= Highest_room_index && (!Rooms[i].used))
        continue;

      if (j <= Highest_room_index && (Rooms[i].flags & RF_EXTERNAL))
        continue;

      if (i == j)
        continue;

      if (i == Highest_room_index + 1 && j > Highest_room_index)
        continue;

      if (j == Highest_room_index + 1 && i > Highest_room_index)
        continue;

      if (BOA_NEXT_ROOM(cur_room, j) != BOA_NO_PATH && BOA_NEXT_ROOM(cur_room, j) != cur_room) {
        int last_room = cur_room;

        do {
          last_room = cur_room;
          cur_room = BOA_NEXT_ROOM(cur_room, j);

          if (last_room != cur_room) {
            if (BOA_DetermineStartRoomPortal(last_room, NULL, cur_room, NULL, false, true) == -1) {
              BOA_Array[i][j] |= BOAF_TOO_SMALL_FOR_ROBOT;
              break;
            }
          }

        } while (cur_room != j);
      }
    }
  }
}

void MakeBOA(void) {
  ASSERT(BOA_ROOM_MASK > MAX_ROOMS + MAX_BOA_TERRAIN_REGIONS);
  int cur_check = BOAGetMineChecksum();

  if (cur_check == BOA_vis_checksum)
    BOA_vis_valid = 1;
  else
    BOA_vis_valid = 0;

  if (cur_check == BOA_mine_checksum)
    return;

  // OutrageMessageBox("Reminder: You need to make BOA Vis on this level.\nThis is either because it hasn't\nbeen done
  // or Chris updated BOA.");

  BOA_mine_checksum = cur_check;
  BOA_f_making_boa = true;

  LOG_INFO << "Making BOA and friends";

  LOG_DEBUG << "Finding small portals";
  find_small_portals();
  LOG_DEBUG << "Done Finding small portals";

  LOG_DEBUG << "  Start computing path points.";
  BOA_ComputePathPoints();
  LOG_DEBUG << "  Done computing path points.";

  clear_BOA();
  compute_costs();

  LOG_DEBUG << "  Start computing mines.";
  compute_mine_info();
  LOG_DEBUG.printf("  Done computing %d mines.", BOA_num_mines);

  LOG_DEBUG << "  Start computing terrain regions.";
  compute_terrain_region_info();
  LOG_DEBUG.printf("  Done computing %d terrain regions.", BOA_num_terrain_regions);

  LOG_DEBUG << "  Making designers wait for no particular reason...";
  compute_next_segs();
  LOG_DEBUG << "  Done with the sodomy...";

  LOG_DEBUG << "  Start computing blockage info.";
  compute_blockage_info();
  LOG_DEBUG << "  Done computing blockage info.";

  LOG_DEBUG << "  Start computing sound prop.";
  compute_sound_dist_info();
  LOG_DEBUG << "  Done computing sound prop.";

  LOG_DEBUG << "  Start computing invalid robot path info.";
  compute_robot_path_info();
  LOG_DEBUG << "  Done computing invalid robot path info.";

  LOG_DEBUG << "  Verifying connections";
  verify_connections();
  LOG_DEBUG << "  Done with verification";

  //	{
  //		int cur_seg = 0;
  //		vector pos;
  //		matrix orient  = IDENTITY_MATRIX;
  //
  //		while (cur_seg != Highest_segment_index) {
  //
  //			pos = average_pnt(&Segments[cur_seg]);
  //
  //			ObjCreate( 7, 0, cur_seg, &pos, &orient, 0);
  //			cur_seg = BOA_Array[cur_seg][Highest_segment_index] & BOA_SEG_MASK;
  //
  //		}
  //	}

  BOA_f_making_boa = false;
  LOG_INFO << "BOA is done";
}

static int Current_sort_room;

static int face_sort_func1(const int16_t *a, const int16_t *b) {
  if (Rooms[Current_sort_room].faces[*a].min_xyz.y() > Rooms[Current_sort_room].faces[*b].min_xyz.y())
    return -1;
  else if (Rooms[Current_sort_room].faces[*a].min_xyz.y() < Rooms[Current_sort_room].faces[*b].min_xyz.y())
    return 1;
  else
    return 0;
}

static int face_sort_func2(const int16_t *a, const int16_t *b) {
  if (Rooms[Current_sort_room].faces[*a].max_xyz.y() < Rooms[Current_sort_room].faces[*b].max_xyz.y())
    return -1;
  else if (Rooms[Current_sort_room].faces[*a].max_xyz.y() > Rooms[Current_sort_room].faces[*b].max_xyz.y())
    return 1;
  else
    return 0;
}

#define MAX_REGIONS_PER_ROOM 200

void ComputeAABB(bool f_full) {
  int i;
  int j;
  int k;

  int computed_room_check[MAX_ROOMS];

  // Determines the room and face min/max information
  int cur_check = BOAGetMineChecksum();

  if (cur_check == BOA_AABB_checksum && !f_full) {
    LOG_DEBUG << "Computing AABB's (partial)!";
  } else {
    for (i = 0; i < MAX_ROOMS; i++) {
      if (Rooms[i].used) {
        computed_room_check[i] = BOAGetRoomChecksum(i);
      } else {
        BOA_AABB_ROOM_checksum[i] = computed_room_check[i] = 0;
      }
    }

    int16_t *num_structs_per_room = (int16_t *)mem_malloc((Highest_room_index + 1) * sizeof(int16_t));
    int16_t **r_struct_list;

    // Allocate the structure that tells what struct each face is in
    r_struct_list = (int16_t **)mem_malloc((Highest_room_index + 1) * sizeof(int16_t *));
    for (i = 0; i <= Highest_room_index; i++) {
      if (Rooms[i].used) {
        if (BOA_AABB_ROOM_checksum[i] != 0 && BOA_AABB_ROOM_checksum[i] == computed_room_check[i])
          continue;

        r_struct_list[i] = mem_rmalloc<int16_t>(Rooms[i].num_faces);
      }
    }

    int16_t count;

    BOA_AABB_checksum = cur_check;
    LOG_DEBUG << "Computing AABB's (full)!";

    for (i = 0; i <= Highest_room_index; i++) {
      Current_sort_room = i;
      if (Rooms[i].used) {
        float average_y;
        average_y = 0.0f;

        if (BOA_AABB_ROOM_checksum[i] != 0 && BOA_AABB_ROOM_checksum[i] == computed_room_check[i])
          continue;

        for (j = 0; j < Rooms[i].num_faces; j++) {
          vector face_min;
          vector face_max;

          for (k = 0; k < Rooms[i].faces[j].num_verts; k++) {
            if (k == 0 || Rooms[i].verts[Rooms[i].faces[j].face_verts[k]].x() < face_min.x())
              face_min.x() = Rooms[i].verts[Rooms[i].faces[j].face_verts[k]].x();

            if (k == 0 || Rooms[i].verts[Rooms[i].faces[j].face_verts[k]].y() < face_min.y())
              face_min.y() = Rooms[i].verts[Rooms[i].faces[j].face_verts[k]].y();

            if (k == 0 || Rooms[i].verts[Rooms[i].faces[j].face_verts[k]].z() < face_min.z())
              face_min.z() = Rooms[i].verts[Rooms[i].faces[j].face_verts[k]].z();

            if (k == 0 || Rooms[i].verts[Rooms[i].faces[j].face_verts[k]].x() > face_max.x())
              face_max.x() = Rooms[i].verts[Rooms[i].faces[j].face_verts[k]].x();

            if (k == 0 || Rooms[i].verts[Rooms[i].faces[j].face_verts[k]].y() > face_max.y())
              face_max.y() = Rooms[i].verts[Rooms[i].faces[j].face_verts[k]].y();

            if (k == 0 || Rooms[i].verts[Rooms[i].faces[j].face_verts[k]].z() > face_max.z())
              face_max.z() = Rooms[i].verts[Rooms[i].faces[j].face_verts[k]].z();
          }

          Rooms[i].faces[j].min_xyz = face_min;
          Rooms[i].faces[j].max_xyz = face_max;

          average_y += ((face_min.y() + face_max.y()) / 2);
        }

        average_y /= Rooms[i].num_faces;

        // Do the rooms now

        room *rp = &Rooms[i];

        rp->max_xyz.x() = rp->max_xyz.y() = rp->max_xyz.z() = -9999999.0;
        rp->min_xyz.x() = rp->min_xyz.y() = rp->min_xyz.z() = 9999999.0;

        for (int t = 0; t < rp->num_faces; t++) {
          face *fp = &rp->faces[t];

          if (fp->max_xyz.x() > rp->max_xyz.x())
            rp->max_xyz.x() = fp->max_xyz.x();

          if (fp->max_xyz.y() > rp->max_xyz.y())
            rp->max_xyz.y() = fp->max_xyz.y();

          if (fp->max_xyz.z() > rp->max_xyz.z())
            rp->max_xyz.z() = fp->max_xyz.z();

          if (fp->min_xyz.x() < rp->min_xyz.x())
            rp->min_xyz.x() = fp->min_xyz.x();

          if (fp->min_xyz.y() < rp->min_xyz.y())
            rp->min_xyz.y() = fp->min_xyz.y();

          if (fp->min_xyz.z() < rp->min_xyz.z())
            rp->min_xyz.z() = fp->min_xyz.z();
        }
      }
    }

    // Determine number of independent structures and classify each face
    for (i = 0; i <= Highest_room_index; i++) {
      if (Rooms[i].used) {
        room *rp = &Rooms[i];
        int num_struct = 0;
        int count1, count2;

        if (BOA_AABB_ROOM_checksum[i] != 0 && BOA_AABB_ROOM_checksum[i] == computed_room_check[i])
          continue;

        int16_t *nfaces;
        bool *used;

        int n_new;

        nfaces = mem_rmalloc<int16_t>(rp->num_faces);
        used = mem_rmalloc<bool>(rp->num_faces);

        for (count1 = 0; count1 < rp->num_faces; count1++) {
          used[count1] = false;
        }

      next_struct:

        for (count1 = 0; count1 < rp->num_faces; count1++) {
          if (!used[count1])
            break;
        }

        if (count1 >= rp->num_faces)
          goto done;

        num_struct++;

        n_new = 1;
        nfaces[0] = count1;
        used[count1] = true;
        r_struct_list[i][count1] = num_struct - 1;
        //			r_struct_list[i][count1] = 0;

        while (n_new > 0) {
          int seed = nfaces[--n_new];
          int count3;

          for (count1 = 0; count1 < rp->faces[seed].num_verts; count1++) {
            int c_vert = rp->faces[seed].face_verts[count1];

            for (count2 = 0; count2 < rp->num_faces; count2++) {
              if (used[count2] == false) {
                for (count3 = 0; count3 < rp->faces[count2].num_verts; count3++) {
                  if (c_vert == rp->faces[count2].face_verts[count3]) {
                    nfaces[n_new++] = count2;
                    used[count2] = true;
                    r_struct_list[i][count2] = num_struct - 1;
                    //									r_struct_list[i][count2] = 0;
                    break;
                  }
                }
              }
            }
          }
        }

        goto next_struct;

      done:

        mem_free(nfaces);
        mem_free(used);

        ASSERT(num_struct < MAX_REGIONS_PER_ROOM); // get chris
        num_structs_per_room[i] = num_struct;
      }
    }

    // Determine Area of each region for external shell remap
    for (i = 0; i <= Highest_room_index; i++) {
      if (Rooms[i].used) {
        room *rp = &Rooms[i];

        if (BOA_AABB_ROOM_checksum[i] != 0 && BOA_AABB_ROOM_checksum[i] == computed_room_check[i])
          continue;

        if (num_structs_per_room[i] == 1)
          continue;
        //			else
        //				num_structs_per_room[i] = 1;
        //
        //			continue;

        vector *s_max_xyz = mem_rmalloc<vector>(num_structs_per_room[i]);
        vector *s_min_xyz = mem_rmalloc<vector>(num_structs_per_room[i]);

        for (count = 0; count < num_structs_per_room[i]; count++) {

          s_max_xyz[count].x() = s_max_xyz[count].y() = s_max_xyz[count].z() = -9999999.0;
          s_min_xyz[count].x() = s_min_xyz[count].y() = s_min_xyz[count].z() = 9999999.0;

          for (int t = 0; t < rp->num_faces; t++) {
            if (r_struct_list[i][t] != count)
              continue;

            face *fp = &rp->faces[t];

            if (fp->max_xyz.x() > s_max_xyz[count].x())
              s_max_xyz[count].x() = fp->max_xyz.x();

            if (fp->max_xyz.y() > s_max_xyz[count].y())
              s_max_xyz[count].y() = fp->max_xyz.y();

            if (fp->max_xyz.z() > s_max_xyz[count].z())
              s_max_xyz[count].z() = fp->max_xyz.z();

            if (fp->min_xyz.x() < s_min_xyz[count].x())
              s_min_xyz[count].x() = fp->min_xyz.x();

            if (fp->min_xyz.y() < s_min_xyz[count].y())
              s_min_xyz[count].y() = fp->min_xyz.y();

            if (fp->min_xyz.z() < s_min_xyz[count].z())
              s_min_xyz[count].z() = fp->min_xyz.z();
          }
        }

        int best = 0;
        vector diff = s_max_xyz[0] - s_min_xyz[0];
        scalar best_size = fabs(diff.x() * diff.y() * diff.z());

        for (count = 1; count < num_structs_per_room[i]; count++) {
          diff = s_max_xyz[count] - s_min_xyz[count];
          scalar size = fabs(diff.x() * diff.y() * diff.z());

          if (size > best_size) {
            best = count;
            best_size = size;
          }
        }

        if (best != 0) {
          for (count = 0; count < rp->num_faces; count++) {
            if (r_struct_list[i][count] == 0) {
              r_struct_list[i][count] = best;
            } else if (r_struct_list[i][count] == best) {
              r_struct_list[i][count] = 0;
            }
          }
        }

        mem_free(s_max_xyz);
        mem_free(s_min_xyz);
      }
    }

    // Breaks up the main shell by cube and pushes faces into the appropriate region list
    for (i = 0; i <= Highest_room_index; i++) {
      if (Rooms[i].used) {
        int x;
        Current_sort_room = i;
        room *rp = &Rooms[i];

        if (BOA_AABB_ROOM_checksum[i] != 0 && BOA_AABB_ROOM_checksum[i] == computed_room_check[i])
          continue;

        vector min_xyz;
        vector max_xyz;
        int pamount[6];
        int nonpart = 0;

        if (rp->num_bbf_regions != 0) {
          for (x = 0; x < rp->num_bbf_regions; x++) {
            mem_free(rp->bbf_list[x]);
          }
          mem_free(rp->bbf_list);
          mem_free(rp->num_bbf);
          mem_free(rp->bbf_list_min_xyz);
          mem_free(rp->bbf_list_max_xyz);
          mem_free(rp->bbf_list_sector);
          rp->num_bbf_regions = 0;
        }

        // temporary malloc
        rp->num_bbf_regions = 27 + num_structs_per_room[i] - 1;
        rp->bbf_list = mem_rmalloc<int16_t *>(MAX_REGIONS_PER_ROOM);
        for (x = 0; x < MAX_REGIONS_PER_ROOM; x++) {
          rp->bbf_list[x] = mem_rmalloc<int16_t>(rp->num_faces);
        }
        rp->num_bbf = mem_rmalloc<int16_t>(MAX_REGIONS_PER_ROOM);
        rp->bbf_list_min_xyz = mem_rmalloc<vector>(MAX_REGIONS_PER_ROOM);
        rp->bbf_list_max_xyz = mem_rmalloc<vector>(MAX_REGIONS_PER_ROOM);
        rp->bbf_list_sector = mem_rmalloc<uint8_t>(MAX_REGIONS_PER_ROOM);

        for (x = 0; x < 27; x++) {
          rp->bbf_list_sector[x] = bbf_lookup[x];
        }

        vector diff = { (scalar)15, (scalar)15, (scalar)15 };
        min_xyz = max_xyz = (rp->min_xyz + rp->max_xyz) / 2.0f;

        min_xyz -= diff;
        max_xyz += diff;

        if (min_xyz.x() <= rp->min_xyz.x())
          min_xyz.x() = rp->min_xyz.x() + 2.5f;
        if (min_xyz.y() <= rp->min_xyz.y())
          min_xyz.y() = rp->min_xyz.y() + 2.5f;
        if (min_xyz.z() <= rp->min_xyz.z())
          min_xyz.z() = rp->min_xyz.z() + 2.5f;
        if (max_xyz.x() >= rp->max_xyz.x())
          max_xyz.x() = rp->max_xyz.x() - 2.5f;
        if (max_xyz.y() >= rp->max_xyz.y())
          max_xyz.y() = rp->max_xyz.y() - 2.5f;
        if (max_xyz.z() >= rp->max_xyz.z())
          max_xyz.z() = rp->max_xyz.z() - 2.5f;

        if (min_xyz.x() >= max_xyz.x()) {
          min_xyz.x() = rp->min_xyz.x();
          max_xyz.x() = rp->max_xyz.x();
        }
        if (min_xyz.y() >= max_xyz.y()) {
          min_xyz.y() = rp->min_xyz.y();
          max_xyz.y() = rp->max_xyz.y();
        }
        if (min_xyz.z() >= max_xyz.z()) {
          min_xyz.z() = rp->min_xyz.z();
          max_xyz.z() = rp->max_xyz.z();
        }

        rp->bbf_min_xyz = min_xyz;
        rp->bbf_max_xyz = max_xyz;

        // chrishack -- add post face add snap

        for (count = 0; count < 6; count++) {
          pamount[count] = 0;
        }

        for (count = 0; count < rp->num_bbf_regions; count++) {
          rp->num_bbf[count] = 0;
        }

        for (count = 0; count < rp->num_faces; count++) {
          face *fp = &rp->faces[count];
          bool f_part = false;
          bool list[6];
          int x;

          for (x = 0; x < 6; x++)
            list[x] = false;

          if (fp->max_xyz.x() <= min_xyz.x()) {
            list[0] = true;
            f_part = true;
          }
          if (fp->max_xyz.y() <= min_xyz.y()) {
            list[1] = true;
            f_part = true;
          }
          if (fp->max_xyz.z() <= min_xyz.z()) {
            list[2] = true;
            f_part = true;
          }
          if ((!list[0]) && fp->min_xyz.x() >= max_xyz.x()) {
            list[3] = true;
            f_part = true;
          }
          if ((!list[1]) && fp->min_xyz.y() >= max_xyz.y()) {
            list[4] = true;
            f_part = true;
          }
          if ((!list[2]) && fp->min_xyz.z() >= max_xyz.z()) {
            list[5] = true;
            f_part = true;
          }

          uint8_t sector = 0;

          if (f_part == false) {
            if (r_struct_list[i][count] == 0)
              rp->bbf_list[0][rp->num_bbf[0]++] = count;
            else {
              if (rp->num_bbf[27 + r_struct_list[i][count] - 1] == 0)
                rp->bbf_list_sector[27 + r_struct_list[i][count] - 1] = sector;
              else
                rp->bbf_list_sector[27 + r_struct_list[i][count] - 1] &= sector;

              rp->bbf_list[27 + r_struct_list[i][count] - 1][rp->num_bbf[27 + r_struct_list[i][count] - 1]++] = count;
            }

            nonpart++;
          } else {
            //					bool f_found = false;
            //					int best;

            for (x = 0; x < 6; x++) {
              if (list[x]) {
                sector |= 0x01 << x;
              }
            }

            int slot;

            switch (sector) {
            case 0x01:
              slot = 1;
              break;
            case 0x02:
              slot = 2;
              break;
            case 0x04:
              slot = 3;
              break;
            case 0x08:
              slot = 4;
              break;
            case 0x10:
              slot = 5;
              break;
            case 0x20:
              slot = 6;
              break;
            case (0x01 | 0x02):
              slot = 7;
              break;
            case (0x01 | 0x04):
              slot = 8;
              break;
            case (0x01 | 0x10):
              slot = 9;
              break;
            case (0x01 | 0x20):
              slot = 10;
              break;
            case (0x02 | 0x04):
              slot = 11;
              break;
            case (0x02 | 0x08):
              slot = 12;
              break;
            case (0x02 | 0x20):
              slot = 13;
              break;
            case (0x04 | 0x08):
              slot = 14;
              break;
            case (0x04 | 0x10):
              slot = 15;
              break;
            case (0x08 | 0x10):
              slot = 16;
              break;
            case (0x08 | 0x20):
              slot = 17;
              break;
            case (0x10 | 0x20):
              slot = 18;
              break;
            case 0x01 | 0x02 | 0x04:
              slot = 19;
              break;
            case 0x01 | 0x02 | 0x20:
              slot = 20;
              break;
            case 0x01 | 0x04 | 0x10:
              slot = 21;
              break;
            case 0x01 | 0x10 | 0x20:
              slot = 22;
              break;
            case 0x08 | 0x02 | 0x04:
              slot = 23;
              break;
            case 0x08 | 0x02 | 0x20:
              slot = 24;
              break;
            case 0x08 | 0x04 | 0x10:
              slot = 25;
              break;
            case 0x08 | 0x10 | 0x20:
              slot = 26;
              break;
            default:
              Int3();
            }

            if (r_struct_list[i][count] == 0)
              rp->bbf_list[slot][rp->num_bbf[slot]++] = count;
            else {
              if (rp->num_bbf[27 + r_struct_list[i][count] - 1] == 0)
                rp->bbf_list_sector[27 + r_struct_list[i][count] - 1] = sector;
              else
                rp->bbf_list_sector[27 + r_struct_list[i][count] - 1] &= sector;

              rp->bbf_list[27 + r_struct_list[i][count] - 1][rp->num_bbf[27 + r_struct_list[i][count] - 1]++] = count;
            }

            ASSERT(slot > 0 && slot < rp->num_bbf_regions);
          }
        }
      }
    }

    mem_free(num_structs_per_room);
    for (i = 0; i <= Highest_room_index; i++) {
      if (Rooms[i].used) {
        if (BOA_AABB_ROOM_checksum[i] != 0 && BOA_AABB_ROOM_checksum[i] == computed_room_check[i])
          continue;

        mem_free(r_struct_list[i]);
      }
    }
    mem_free(r_struct_list);
    num_structs_per_room = NULL;
    r_struct_list = NULL;

    // Finds the min/max of each region
    for (i = 0; i <= Highest_room_index; i++) {
      if (Rooms[i].used) {
        room *rp = &Rooms[i];

        if (BOA_AABB_ROOM_checksum[i] != 0 && BOA_AABB_ROOM_checksum[i] == computed_room_check[i])
          continue;

        for (count = 0; count < rp->num_bbf_regions; count++) {
          rp->bbf_list_max_xyz[count].x() = rp->bbf_list_max_xyz[count].y() = rp->bbf_list_max_xyz[count].z() = -9999999.0;
          rp->bbf_list_min_xyz[count].x() = rp->bbf_list_min_xyz[count].y() = rp->bbf_list_min_xyz[count].z() = 9999999.0;

          for (int t = 0; t < rp->num_bbf[count]; t++) {
            face *fp = &rp->faces[rp->bbf_list[count][t]];

            if (fp->max_xyz.x() > rp->bbf_list_max_xyz[count].x())
              rp->bbf_list_max_xyz[count].x() = fp->max_xyz.x();

            if (fp->max_xyz.y() > rp->bbf_list_max_xyz[count].y())
              rp->bbf_list_max_xyz[count].y() = fp->max_xyz.y();

            if (fp->max_xyz.z() > rp->bbf_list_max_xyz[count].z())
              rp->bbf_list_max_xyz[count].z() = fp->max_xyz.z();

            if (fp->min_xyz.x() < rp->bbf_list_min_xyz[count].x())
              rp->bbf_list_min_xyz[count].x() = fp->min_xyz.x();

            if (fp->min_xyz.y() < rp->bbf_list_min_xyz[count].y())
              rp->bbf_list_min_xyz[count].y() = fp->min_xyz.y();

            if (fp->min_xyz.z() < rp->bbf_list_min_xyz[count].z())
              rp->bbf_list_min_xyz[count].z() = fp->min_xyz.z();
          }
        }
      }
    }

    // Remove unnecessary groups
    for (count = 0; count <= Highest_room_index; count++) {
      if (Rooms[count].used) {
        if (BOA_AABB_ROOM_checksum[count] != 0 && BOA_AABB_ROOM_checksum[count] == computed_room_check[count])
          continue;

        room *rp = &Rooms[count];
        for (i = 0; i < rp->num_bbf_regions; i++) {
          if (rp->num_bbf[i] == 0) {
            for (j = i + 1; j < rp->num_bbf_regions; j++) {
              int16_t *temp = rp->bbf_list[j - 1];

              rp->bbf_list[j - 1] = rp->bbf_list[j];
              rp->bbf_list[j] = temp;

              rp->num_bbf[j - 1] = rp->num_bbf[j];
              rp->bbf_list_min_xyz[j - 1] = rp->bbf_list_min_xyz[j];
              rp->bbf_list_max_xyz[j - 1] = rp->bbf_list_max_xyz[j];
              rp->bbf_list_sector[j - 1] = rp->bbf_list_sector[j];
            }

            rp->num_bbf_regions--;
            i--;
          }
        }
      }
    }

    // Sub-divide structures
    for (i = 0; i <= Highest_room_index; i++) {
      if (Rooms[i].used) {
        room *rp = &Rooms[i];
        int original_bbf_regions = rp->num_bbf_regions;

        if (BOA_AABB_ROOM_checksum[i] != 0 && BOA_AABB_ROOM_checksum[i] == computed_room_check[i])
          continue;

        for (j = 0; j < original_bbf_regions; j++) {
          if (rp->num_bbf[j] > 15) {
            // Test 3 split type
            vector split = (rp->bbf_list_min_xyz[j] + rp->bbf_list_max_xyz[j]) / 2.0f;
            int num_faces[3][3];

            int sp;
            int gel;

            for (sp = 0; sp < 3; sp++) {
              for (gel = 0; gel < 3; gel++) {
                num_faces[sp][gel] = 0;
              }
            }

            for (sp = 0; sp < 3; sp++) {
              float val = ((float *)&split)[sp];

              for (count = 0; count < rp->num_bbf[j]; count++) {
                if (((float *)(&rp->faces[rp->bbf_list[j][count]].max_xyz))[sp] <= val)
                  gel = 2;
                else if (((float *)(&rp->faces[rp->bbf_list[j][count]].min_xyz))[sp] >= val)
                  gel = 0;
                else
                  gel = 1;

                num_faces[sp][gel]++;
              }
            }

            int diff[3];
            for (sp = 0; sp < 3; sp++) {
              if (abs(num_faces[sp][0] - num_faces[sp][1]) > abs(num_faces[sp][1] - num_faces[sp][2]))
                diff[sp] = abs(num_faces[sp][0] - num_faces[sp][1]);
              else
                diff[sp] = abs(num_faces[sp][1] - num_faces[sp][2]);
            }

            int best = 0;
            float b_diff = diff[0];

            for (sp = 1; sp < 3; sp++) {
              if (b_diff > diff[sp]) {
                best = sp;
                b_diff = diff[sp];
              }
            }

            if (rp->num_bbf_regions <= MAX_REGIONS_PER_ROOM - 2) {
              rp->num_bbf[rp->num_bbf_regions] = 0;
              rp->num_bbf[rp->num_bbf_regions + 1] = 0;

              sp = best;
              for (count = 0; count < rp->num_bbf[j]; count++) {
                float val = ((float *)&split)[sp];

                if (((float *)(&rp->faces[rp->bbf_list[j][count]].max_xyz))[sp] <= val) {
                  rp->bbf_list[rp->num_bbf_regions][rp->num_bbf[rp->num_bbf_regions]++] = rp->bbf_list[j][count];

                  int t;
                  for (t = count; t < rp->num_bbf[j] - 1; t++) {
                    rp->bbf_list[j][t] = rp->bbf_list[j][t + 1];
                  }

                  rp->num_bbf[j]--;
                  count--;
                } else if (((float *)(&rp->faces[rp->bbf_list[j][count]].min_xyz))[sp] >= val) {
                  rp->bbf_list[rp->num_bbf_regions + 1][rp->num_bbf[rp->num_bbf_regions + 1]++] =
                      rp->bbf_list[j][count];

                  int t;
                  for (t = count; t < rp->num_bbf[j] - 1; t++) {
                    rp->bbf_list[j][t] = rp->bbf_list[j][t + 1];
                  }

                  rp->num_bbf[j]--;
                  count--;
                }
              }

              rp->num_bbf_regions += 2;
            }
          }
        }
      }
    }

    // Remove unnecessary groups
    for (count = 0; count <= Highest_room_index; count++) {
      if (Rooms[count].used) {
        if (BOA_AABB_ROOM_checksum[count] != 0 && BOA_AABB_ROOM_checksum[count] == computed_room_check[count])
          continue;

        room *rp = &Rooms[count];
        for (i = 0; i < rp->num_bbf_regions; i++) {
          if (rp->num_bbf[i] == 0) {
            for (j = i + 1; j < rp->num_bbf_regions; j++) {
              int16_t *temp = rp->bbf_list[j - 1];

              rp->bbf_list[j - 1] = rp->bbf_list[j];
              rp->bbf_list[j] = temp;

              rp->num_bbf[j - 1] = rp->num_bbf[j];
              rp->bbf_list_min_xyz[j - 1] = rp->bbf_list_min_xyz[j];
              rp->bbf_list_max_xyz[j - 1] = rp->bbf_list_max_xyz[j];
              rp->bbf_list_sector[j - 1] = rp->bbf_list_sector[j];
            }

            rp->num_bbf_regions--;
            i--;
          }
        }
      }
    }

    // Finds the min/max of each region
    for (i = 0; i <= Highest_room_index; i++) {
      if (Rooms[i].used) {
        room *rp = &Rooms[i];

        if (BOA_AABB_ROOM_checksum[i] != 0 && BOA_AABB_ROOM_checksum[i] == computed_room_check[i])
          continue;

        for (count = 0; count < rp->num_bbf_regions; count++) {
          rp->bbf_list_max_xyz[count].x() = rp->bbf_list_max_xyz[count].y() = rp->bbf_list_max_xyz[count].z() = -9999999.0;
          rp->bbf_list_min_xyz[count].x() = rp->bbf_list_min_xyz[count].y() = rp->bbf_list_min_xyz[count].z() = 9999999.0;

          for (int t = 0; t < rp->num_bbf[count]; t++) {
            face *fp = &rp->faces[rp->bbf_list[count][t]];

            if (fp->max_xyz.x() > rp->bbf_list_max_xyz[count].x())
              rp->bbf_list_max_xyz[count].x() = fp->max_xyz.x();

            if (fp->max_xyz.y() > rp->bbf_list_max_xyz[count].y())
              rp->bbf_list_max_xyz[count].y() = fp->max_xyz.y();

            if (fp->max_xyz.z() > rp->bbf_list_max_xyz[count].z())
              rp->bbf_list_max_xyz[count].z() = fp->max_xyz.z();

            if (fp->min_xyz.x() < rp->bbf_list_min_xyz[count].x())
              rp->bbf_list_min_xyz[count].x() = fp->min_xyz.x();

            if (fp->min_xyz.y() < rp->bbf_list_min_xyz[count].y())
              rp->bbf_list_min_xyz[count].y() = fp->min_xyz.y();

            if (fp->min_xyz.z() < rp->bbf_list_min_xyz[count].z())
              rp->bbf_list_min_xyz[count].z() = fp->min_xyz.z();
          }
        }
      }
    }

    // Remaps all the regions to their best sectors
    for (i = 0; i <= Highest_room_index; i++) {
      if (Rooms[i].used) {
        room *rp = &Rooms[i];
        vector min_xyz = rp->bbf_min_xyz;
        vector max_xyz = rp->bbf_max_xyz;

        if (BOA_AABB_ROOM_checksum[i] != 0 && BOA_AABB_ROOM_checksum[i] == computed_room_check[i])
          continue;

        for (j = 0; j < rp->num_bbf_regions; j++) {
          for (count = 0; count < rp->num_bbf[j]; count++) {
            face *fp = &rp->faces[rp->bbf_list[j][count]];
            bool list[6];
            int x;

            for (x = 0; x < 6; x++)
              list[x] = false;

            if (fp->max_xyz.x() <= min_xyz.x()) {
              list[0] = true;
            }
            if (fp->max_xyz.y() <= min_xyz.y()) {
              list[1] = true;
            }
            if (fp->max_xyz.z() <= min_xyz.z()) {
              list[2] = true;
            }
            if ((!list[0]) && fp->min_xyz.x() >= max_xyz.x()) {
              list[3] = true;
            }
            if ((!list[1]) && fp->min_xyz.y() >= max_xyz.y()) {
              list[4] = true;
            }
            if ((!list[2]) && fp->min_xyz.z() >= max_xyz.z()) {
              list[5] = true;
            }

            uint8_t sector = 0;

            for (x = 0; x < 6; x++) {
              if (list[x]) {
                sector |= 0x01 << x;
              }
            }

            if (count == 0)
              rp->bbf_list_sector[j] = sector;
            else
              rp->bbf_list_sector[j] &= sector;
          }
        }
      }
    }

    // Reallocate remaining data structures
    // Remove extra slots
    for (count = 0; count <= Highest_room_index; count++) {
      if (Rooms[count].used) {
        room *rp = &Rooms[count];
        if (BOA_AABB_ROOM_checksum[count] != 0 && BOA_AABB_ROOM_checksum[count] == computed_room_check[count])
          continue;

        for (j = 0; j < rp->num_bbf_regions; j++) {
          rp->bbf_list[j] = (int16_t *)mem_realloc(rp->bbf_list[j], (sizeof(int16_t) * rp->num_bbf[j]));
        }
        for (j = rp->num_bbf_regions; j < MAX_REGIONS_PER_ROOM; j++) {
          mem_free(rp->bbf_list[j]);
        }

        rp->bbf_list = (int16_t **)mem_realloc(rp->bbf_list, rp->num_bbf_regions * sizeof(int16_t *));
        rp->num_bbf = (int16_t *)mem_realloc(rp->num_bbf, rp->num_bbf_regions * sizeof(int16_t));
        rp->bbf_list_min_xyz = (vector *)mem_realloc(rp->bbf_list_min_xyz, rp->num_bbf_regions * sizeof(vector));
        rp->bbf_list_max_xyz = (vector *)mem_realloc(rp->bbf_list_max_xyz, rp->num_bbf_regions * sizeof(vector));
        rp->bbf_list_sector =
            (uint8_t *)mem_realloc(rp->bbf_list_sector, rp->num_bbf_regions * sizeof(uint8_t));
      }
    }

    //	// Print out remaining groups
    //	for(count = 0; count <= Highest_room_index; count++)
    //	{
    //		if(Rooms[count].used)
    //		{
    //			mprintf(0, "==================\n", count);
    //			mprintf(0, "Room %d\n", count);
    //
    //			room *rp = &Rooms[count];
    //			for(i = 0; i < rp->num_bbf_regions; i++)
    //			{
    //				mprintf(0, "Region %d has %d faces in it.\n", i, rp->num_bbf[i]);
    //			}
    //		}
    //	}

    for (i = 0; i < MAX_ROOMS; i++) {
      BOA_AABB_ROOM_checksum[i] = computed_room_check[i];
    }
  }

  // I had to add it :(  Rooms need this done even the checksum is correct
  for (i = 0; i <= Highest_room_index; i++) {
    if (Rooms[i].used) {
      room *rp = &Rooms[i];

      rp->max_xyz.x() = rp->max_xyz.y() = rp->max_xyz.z() = -9999999.0;
      rp->min_xyz.x() = rp->min_xyz.y() = rp->min_xyz.z() = 9999999.0;

      for (int t = 0; t < rp->num_faces; t++) {
        face *fp = &rp->faces[t];

        if (fp->max_xyz.x() > rp->max_xyz.x())
          rp->max_xyz.x() = fp->max_xyz.x();

        if (fp->max_xyz.y() > rp->max_xyz.y())
          rp->max_xyz.y() = fp->max_xyz.y();

        if (fp->max_xyz.z() > rp->max_xyz.z())
          rp->max_xyz.z() = fp->max_xyz.z();

        if (fp->min_xyz.x() < rp->min_xyz.x())
          rp->min_xyz.x() = fp->min_xyz.x();

        if (fp->min_xyz.y() < rp->min_xyz.y())
          rp->min_xyz.y() = fp->min_xyz.y();

        if (fp->min_xyz.z() < rp->min_xyz.z())
          rp->min_xyz.z() = fp->min_xyz.z();
      }
    }
  }

  for (i = 0; i <= Highest_object_index; i++) {
    if (Objects[i].type != OBJ_NONE) {
      if (!(Objects[i].flags & OF_BIG_OBJECT))
        if (Objects[i].size >= MIN_BIG_OBJ_RAD)
          BigObjAdd(i);

      if (Objects[i].type == OBJ_ROOM) {
        Objects[i].min_xyz = Rooms[Objects[i].id].min_xyz;
        Objects[i].max_xyz = Rooms[Objects[i].id].max_xyz;
      } else {
        ObjSetAABB(&Objects[i]);
        ObjSetOrient(&Objects[i], &Objects[i].orient);
      }
    }
  }

  LOG_DEBUG << "Done Computing AABB's.";
}
