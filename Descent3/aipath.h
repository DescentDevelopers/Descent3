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

#ifndef AIPATH_H_
#define AIPATH_H_

#include "object.h"
#include "pstypes.h"
#include "aistruct.h"

// Exported functions
extern void AIPathMoveTurnTowardsNode(object *obj, vector *mdir, bool *f_moved);
// void AIPathMoveTowardsRoom(object *obj, int room_index);
extern bool AIPathSetAsStaticPath(object *obj, void *goal_ptr, int path_id, int start_node, int end_node, int cur_node);
extern bool AIPathAllocPath(object *obj, ai_frame *ai_info, void *goal_ptr, int *start_room, vector *start_pos,
                            int *end_room, vector *end_pos, float rad, int flags, int handle, int ignore_obj = -1);
extern bool AIPathInitPath(ai_path_info *aip);
extern bool AIPathFreePath(ai_path_info *aip);
extern bool AIFindAltPath(object *obj, int i, int j, float *dist = NULL);

extern bool AIPathAtEnd(ai_path_info *aip);
extern bool AIPathAtStart(ai_path_info *aip);
extern bool AIPathGetNextNodePos(ai_path_info *aip, vector *pos, int *room = NULL);
extern bool AIPathGetPrevNodePos(ai_path_info *aip, vector *pos, int *room = NULL);
extern bool AIPathGetCurrentNodePos(ai_path_info *aip, vector *pos, int *room = NULL);

#ifdef _DEBUG
extern bool MakeTestPath(int *start_room, vector *pos);

extern int AIPath_test_end_room;
extern vector AIPath_test_end_pos;
#endif

#endif
