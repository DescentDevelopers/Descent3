#ifndef _ATTACH_H_
#define _ATTACH_H_

#include "object.h"

bool AttachDoPosOrient(object *parent, char parent_ap, object *child, char child_ap, bool f_parent,
                       bool f_move_obj = true, vector *pos = NULL, matrix *orient = NULL, bool f_dropping_off = false);
bool AttachDoPosOrientRad(object *parent, char p_ap, object *child, float rad_percent, vector *pos);
void AttachUpdateSubObjects(object *obj);

bool AttachObject(object *parent, char parent_ap, object *child, char child_ap, bool f_use_aligned = false);
bool AttachObject(object *parent, char parent_ap, object *child, float percent_rad);
bool UnattachFromParent(object *child);
bool UnattachChild(object *parent, char parent_ap);
bool UnattachChildren(object *parent);

#endif