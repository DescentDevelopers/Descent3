#ifndef AIPATH_H_
#define AIPATH_H_

#include "object.h"
#include "pstypes.h"
#include "aistruct.h"

// Exported functions
void AIPathMoveTurnTowardsNode(object *obj, vector *mdir, bool *f_moved);
//void AIPathMoveTowardsRoom(object *obj, int room_index);
bool AIPathSetAsStaticPath(object *obj, void *goal_ptr, int path_id, int start_node, int end_node, int cur_node);
bool AIPathAllocPath(object *obj, ai_frame *ai_info, void *goal_ptr, int *start_room, vector *start_pos, int *end_room, vector *end_pos, float rad, int flags, int handle, int ignore_obj = -1);
bool AIPathInitPath(ai_path_info *aip);
bool AIPathFreePath(ai_path_info *aip);
bool AIFindAltPath(object *obj, int i, int j, float *dist = NULL);

#ifdef _DEBUG
bool MakeTestPath(int *start_room, vector *pos);

extern int AIPath_test_end_room;
extern vector AIPath_test_end_pos;
#endif

#endif