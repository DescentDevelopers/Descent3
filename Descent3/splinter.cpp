
#include "fireball.h"
#include "object.h"
#include "splinter.h"
#include "polymodel.h"
#include "renderer.h"
#include "gametexture.h"

#include "Macros.h"
#include <stdlib.h>

#include "psrand.h"

#include <algorithm>

// Given an object, renders the representation of this splinter
void DrawSplinterObject(object *obj) {
  ASSERT(obj->type == OBJ_SPLINTER);
  int i;
  poly_model *pm = &Poly_models[obj->id];
  bsp_info *sm = &pm->submodel[obj->ctype.splinter_info.subobj_num];
  int facenum = obj->ctype.splinter_info.facenum;
  vector world_verts[MAX_VERTS_PER_SPLINTER];
  vector temp_vec;
  g3Point pnts[MAX_VERTS_PER_SPLINTER];
  g3Point *pntlist[MAX_VERTS_PER_SPLINTER];

  int limit = std::min<int>(MAX_VERTS_PER_SPLINTER, sm->faces[facenum].nverts);
  float lifenorm = 1.0 - ((obj->lifetime - obj->lifeleft) / obj->lifetime);

  rend_SetLighting(LS_NONE);
  rend_SetAlphaType(AT_CONSTANT);
  rend_SetAlphaValue(255 * lifenorm);
  rend_SetTextureType(TT_LINEAR);

  lifenorm = 1.0;

  for (i = 0; i < limit; i++) {
    // Scale a vert based on the life of the object
    temp_vec = obj->ctype.splinter_info.verts[i] * lifenorm;

    vm_MatrixMulVector(&world_verts[i], &temp_vec, &obj->orient);

    world_verts[i] += obj->pos;
    g3_RotatePoint(&pnts[i], &world_verts[i]);
    pntlist[i] = &pnts[i];
    pnts[i].p3_u = sm->faces[facenum].u[i];
    pnts[i].p3_v = sm->faces[facenum].v[i];
  }

  int bm_handle = GetTextureBitmap(pm->textures[sm->faces[facenum].texnum], 0);
  g3_DrawPoly(limit, pntlist, bm_handle);
}

// do whatever needs to be done for this piece of splinter for this frame
void DoSplinterFrame(object *obj) {
  ASSERT(obj->control_type == CT_SPLINTER);

  if ((obj->lifeleft <= 0.0) && ((ps_rand() % 2) == 0)) {
    // Mark the object to die
    SetObjectDeadFlag(obj);
  } else {
    // Create smoke for some splinters
    if (((obj - Objects) % 8) == 0) {
      if (ps_rand() % 4) {
        int type = ((obj - Objects) % 2) ? GetRandomSmallExplosion() : BLACK_SMOKE_INDEX;
        CreateFireball(&obj->pos, type, obj->roomnum, VISUAL_FIREBALL);
      }
    }
  }
}
