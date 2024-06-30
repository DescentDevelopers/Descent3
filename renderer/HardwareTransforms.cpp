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

#include "3d.h"
#include "pserror.h"
#include "HardwareInternal.h"
#include "renderer.h"
#include <string.h>

// Whether or not to use T&L transforms or the pass-thru ones
static int sUseTransformPassthru = -1;

extern float Z_bias;
void g3_GetModelViewMatrix(const vector *viewPos, const matrix *viewMatrix, float *mvMat) {
  matrix localOrient = (*viewMatrix);
  vector localPos = -(*viewPos);
  mvMat[0] = localOrient.rvec.x;
  mvMat[1] = localOrient.uvec.x;
  mvMat[2] = localOrient.fvec.x;
  mvMat[3] = 0.0f;
  mvMat[4] = localOrient.rvec.y;
  mvMat[5] = localOrient.uvec.y;
  mvMat[6] = localOrient.fvec.y;
  mvMat[7] = 0.0f;
  mvMat[8] = localOrient.rvec.z;
  mvMat[9] = localOrient.uvec.z;
  mvMat[10] = localOrient.fvec.z;
  mvMat[11] = 0.0f;
  mvMat[12] = localPos * localOrient.rvec;
  mvMat[13] = localPos * localOrient.uvec;
  mvMat[14] = localPos * localOrient.fvec + Z_bias;
  mvMat[15] = 1.0f;
}

void g3_TransformVert(float res[4], float pt[4], float a[4][4]) {
  int y;
  for (y = 0; y < 4; ++y) {
    res[y] = (pt[0] * a[0][y]) + (pt[1] * a[1][y]) + (pt[2] * a[2][y]) + (pt[3] * a[3][y]);
  }
}

void g3_TransformMult(float res[4][4], float a[4][4], float b[4][4]) {
  float temp[4][4];

  int x, y;
  for (y = 0; y < 4; ++y) {
    for (x = 0; x < 4; ++x) {
      temp[y][x] = (a[y][0] * b[0][x]) + (a[y][1] * b[1][x]) + (a[y][2] * b[2][x]) + (a[y][3] * b[3][x]);
    }
  }
  memcpy(res, temp, 16 * sizeof(float));
}

void g3_TransformTrans(float res[4][4], float t[4][4]) {
  float temp[4][4];
  int y;
  for (y = 0; y < 4; ++y) {
    int x;
    for (x = 0; x < 4; ++x) {
      temp[x][y] = t[y][x];
    }
  }
  memcpy(res, temp, 16 * sizeof(float));
}

void g3_UpdateFullTransform() {
  // ModelView -> projection
  g3_TransformMult(gTransformFull, gTransformModelView, gTransformProjection);

  // projection  -> ViewPort
  g3_TransformMult(gTransformFull, gTransformFull, gTransformViewPort);
}

void g3_ForceTransformRefresh(void) { sUseTransformPassthru = -1; }

void g3_RefreshTransforms(bool usePassthru) {
  if (sUseTransformPassthru == 1 && usePassthru) {
    // we don't have to do anything because we are already setup for pass-thru
    return;
  }

  if (usePassthru) {
    // setup OpenGL to use pass-thru
    rend_TransformSetToPassthru();
  } else {
    // extract the viewport data from the renderer
    int viewportWidth, viewportHeight, viewportX, viewportY;
    rend_GetProjectionScreenParameters(viewportX, viewportY, viewportWidth, viewportHeight);

    // setup OpenGL to use full transform stack
    // TODO: in the future we only need to set those that have changed
    rend_TransformSetViewport(viewportX, viewportY, viewportWidth, viewportHeight);
    rend_TransformSetProjection(gTransformProjection);
    rend_TransformSetModelView(gTransformModelView);
  }

  // store the pass-thru
  sUseTransformPassthru = (usePassthru) ? 1 : 0;
}
