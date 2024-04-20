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
*/

#include "object.h"
#include "marker.h"
#include "polymodel.h"
#include "player.h"
#include "multi.h"
#include "game.h"
#ifdef _DEBUG
#include "AIMain.h"
#endif

int Marker_polynum;
int Marker_message = 0;

char MarkerMessages[MAX_PLAYERS * 2][MAX_MARKER_MESSAGE_LENGTH];

// Drops a market at the players current position
void DropMarker(char *message) {
  int cur_marker_num;

#ifdef _DEBUG
  char teststring[80];
  strcpy(teststring, message);
  teststring[8] = '\0';

  if (strcmp("ai debug", teststring) == 0) {
    sscanf(message, "ai debug %d", &AI_debug_robot_index);
    mprintf((0, "Debug robot is object index %d\n", AI_debug_robot_index));
  }
#endif

  if (Player_object->type != OBJ_PLAYER)
    return;

  if (Game_mode & GM_MULTI) {
    MultiSendRequestForMarker(message);
    return;
  }

  int limit = 8;

  if (Players[Player_num].num_markers >= limit) {
    // Delete the oldest marker
    int found = -1;
    float low_time = 999999999.0f;
    for (int i = 0; i <= Highest_object_index; i++) {
      if (Objects[i].type == OBJ_MARKER && Objects[i].parent_handle == Player_object->handle &&
          Objects[i].creation_time < low_time) {
        found = i;
        low_time = Objects[i].creation_time;
      }
    }

    ASSERT(found != -1);

    ObjDelete(found);

    cur_marker_num = Objects[found].id;
  } else {
    cur_marker_num = Players[Player_num].num_markers;
    Players[Player_num].num_markers++;
  }

  int objnum = ObjCreate(OBJ_MARKER, cur_marker_num, Player_object->roomnum, &Player_object->pos,
                         &Player_object->orient, Player_object->handle);
  if (objnum >= 0) {
    mprintf((0, "Marker %d created!\n", cur_marker_num));
    strcpy(MarkerMessages[(Player_num * 2) + cur_marker_num], message);
  } else
    mprintf((0, "Marker NOT created!\n"));
}

// Resets markers before a level stars
void ResetMarkers() { Marker_message = 0; }

// Inits markers at game load
void InitMarkers() {
  Marker_polynum = LoadPolyModel("marker.oof", 0);
  ASSERT(Marker_polynum >= 0);
}
