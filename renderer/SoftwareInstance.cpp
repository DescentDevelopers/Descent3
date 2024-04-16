#include "RendererConfig.h"
#ifdef USE_SOFTWARE_TNL

#include "3d.h"
#include "SoftwareInternal.h"
#include <string.h>

struct InstanceContext {
  matrix m_viewMatrix;     // matrix
  matrix m_unscaledMatrix; // unscaled matrix
  vector m_viewPosition;   // position
  float m_modelView[4][4]; // model/view transform
};

#define MAX_INSTANCE_DEPTH 30
static InstanceContext gInstanceStack[MAX_INSTANCE_DEPTH];
static int gInstanceDepth = 0;

// instance at specified point with specified orientation
void g3_StartInstanceMatrix(vector *pos, matrix *orient) {
  ASSERT(orient != NULL);
  ASSERT(gInstanceDepth < MAX_INSTANCE_DEPTH);

  gInstanceStack[gInstanceDepth].m_viewMatrix = View_matrix;
  gInstanceStack[gInstanceDepth].m_viewPosition = View_position;
  gInstanceStack[gInstanceDepth].m_unscaledMatrix = Unscaled_matrix;
  memcpy(gInstanceStack[gInstanceDepth].m_modelView, gTransformModelView, sizeof(gTransformModelView));
  ++gInstanceDepth;

  // step 1: subtract object position from view position
  vector tempv = View_position - *pos;

  // step 2: rotate view vector through object matrix
  View_position = tempv * *orient;

  // step 3: rotate object matrix through view_matrix (vm = ob * vm)
  matrix tempm, tempm2 = ~*orient;

  tempm = tempm2 * View_matrix;
  View_matrix = tempm;

  tempm = tempm2 * Unscaled_matrix;
  Unscaled_matrix = tempm;

  // transform the model/view matrix
  g3_GetModelViewMatrix(&View_position, &Unscaled_matrix, (float *)gTransformModelView);
  g3_UpdateFullTransform();
}

// instance at specified point with specified orientation
void g3_StartInstanceAngles(vector *pos, angvec *angles) {
  if (angles == NULL) {
    matrix ident;
    vm_MakeIdentity(&ident);
    g3_StartInstanceMatrix(pos, &ident);
    return;
  }

  matrix tm;
  vm_AnglesToMatrix(&tm, angles->p, angles->h, angles->b);

  g3_StartInstanceMatrix(pos, &tm);
}

// pops the old context
void g3_DoneInstance() {
  --gInstanceDepth;
  ASSERT(gInstanceDepth >= 0);

  View_position = gInstanceStack[gInstanceDepth].m_viewPosition;
  View_matrix = gInstanceStack[gInstanceDepth].m_viewMatrix;
  Unscaled_matrix = gInstanceStack[gInstanceDepth].m_unscaledMatrix;
  memcpy(gTransformModelView, gInstanceStack[gInstanceDepth].m_modelView, sizeof(gTransformModelView));
  g3_UpdateFullTransform();
}

#endif
