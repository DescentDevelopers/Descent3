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

 * $Logfile: /DescentIII/Main/editor/moveworld.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Code to use the mouse to rotate and move the world in the editor
 *
 * $Log: not supported by cvs2svn $
 *
 * 6     10/04/98 1:25p Matt
 *
 * 5     1/20/98 1:36p Matt
 * Moved mine origin to the center of the terrain
 *
 * 4     10/03/97 3:39p Matt
 * Added seperate sets of wireframe view variables for mine & room views
 *
 * 3     7/22/97 7:08p Matt
 * Cleaned up D3EditState, moving some vars in and some out, and renaming
 * and changing a few others
 *
 * 8     6/13/97 4:24p Matt
 * Added functions to change wireframe view target
 *
 * 7     5/22/97 5:16p Jason
 * added "drawing" on the topo map
 *
 * 6     3/31/97 5:58p Matt
 * Revamped mine update flags
 *
 * 5     2/10/97 5:39p Matt
 * Added func to reset view
 *
 * 4     2/07/97 5:47p Matt
 * Renamed vector.h to vecmat.h to fix DevStudio problem
 *
 * $NoKeywords: $
 */

#include "Moveworld.h"

#include "ddio.h"
#include "vecmat.h"
#include "d3edit.h"
#include "mono.h"

#define dist_2d(x, y) sqrt((x) * (x) + (y) * (y))

void GetMouseRotation(int idx, int idy, matrix *RotMat) {
  float dr, cos_theta, sin_theta, denom, cos_theta1;
  float Radius = 100.0;
  float dx, dy;
  float dxdr, dydr;

  idy *= -1;

  dx = idx;
  dy = idy;

  dr = dist_2d(dx, dy);

  denom = dist_2d(Radius, dr);

  cos_theta = Radius / denom;
  sin_theta = dr / denom;

  cos_theta1 = 1.0 - cos_theta;

  dxdr = dx / dr;
  dydr = dy / dr;

  RotMat->rvec.x = cos_theta + ((dydr * dydr) * cos_theta1);
  RotMat->uvec.x = -((dxdr * dydr) * cos_theta1);
  RotMat->fvec.x = (dxdr * sin_theta);

  RotMat->rvec.y = RotMat->uvec.x;
  RotMat->uvec.y = cos_theta + ((dxdr * dxdr) * cos_theta1);
  RotMat->fvec.y = (dydr * sin_theta);

  RotMat->rvec.z = -RotMat->fvec.x;
  RotMat->uvec.z = -RotMat->fvec.y;
  RotMat->fvec.z = cos_theta;
}

// Variables for current view position and orientation
#define DEFAULT_VIEW_DIST 500
#define DEFAULT_VIEW_RAD 5000

// Wireframe views for the mine & room views
wireframe_view Wireframe_view_mine = {IDENTITY_MATRIX, {0, 0, 0}, DEFAULT_VIEW_DIST, DEFAULT_VIEW_RAD};
wireframe_view Wireframe_view_room = {IDENTITY_MATRIX, {0, 0, 0}, DEFAULT_VIEW_DIST, DEFAULT_VIEW_RAD};

// The current wireframe view
wireframe_view *Wireframe_view = &Wireframe_view_mine;

#define MOVE_SCALE 3.0
#define ZOOM_SCALE 10.0
#define RAD_SCALE 10.0

// Read the mouse and move the wireframe view
void MoveWorld() {
  int x, y, dx, dy;
  ddio_MouseGetState(&x, &y, &dx, &dy);

  if ((dx != 0) || (dy != 0)) {

    if (Editor_view_mode == VM_TERRAIN) {
      return;
    }

    if ((KEY_STATE(KEY_LCTRL) || KEY_STATE(KEY_RCTRL)) && !(KEY_STATE(KEY_LSHIFT) || KEY_STATE(KEY_RSHIFT))) {
      matrix rotmat, tempm;
      GetMouseRotation(dx, dy, &rotmat);
      tempm = Wireframe_view->orient * rotmat;
      Wireframe_view->orient = tempm;
      Edview_changed = 1;
    }

    if ((KEY_STATE(KEY_LCTRL) || KEY_STATE(KEY_RCTRL)) && (KEY_STATE(KEY_LSHIFT) || KEY_STATE(KEY_RSHIFT))) {
      Wireframe_view->target += Wireframe_view->orient.rvec * -dx * MOVE_SCALE;
      Wireframe_view->target += Wireframe_view->orient.uvec * dy * MOVE_SCALE;
      Edview_changed = 1;
    }

    if (KEY_STATE(KEY_Z) && !(KEY_STATE(KEY_LSHIFT) || KEY_STATE(KEY_RSHIFT))) {
      Wireframe_view->dist += dy * ZOOM_SCALE;
      if (Wireframe_view->dist < 0)
        Wireframe_view->dist = 0;
      Edview_changed = 1;
    }

    if (KEY_STATE(KEY_Z) && (KEY_STATE(KEY_LSHIFT) || KEY_STATE(KEY_RSHIFT))) {
      Wireframe_view->rad += dy * RAD_SCALE;
      if (Wireframe_view->rad < 0)
        Wireframe_view->rad = 0;
      EditorStatus("View radius = %.0f", Wireframe_view->rad);
      Edview_changed = 1;
    }
  }
}

extern vector Mine_origin;

// Reset the orienation, position, & distance of the wireframe view
void ResetWireframeView() {
  Wireframe_view->dist = DEFAULT_VIEW_DIST;
  Wireframe_view->rad = DEFAULT_VIEW_RAD;
  vm_MakeIdentity(&Wireframe_view->orient);

  Wireframe_view->target = Mine_origin;

  Edview_changed = 1;
}

// Reset the view radius of the wireframe view
void ResetWireframeViewRad() {
  Wireframe_view->rad = DEFAULT_VIEW_RAD;
  Edview_changed = 1;
}

// Set the wireframe view target to a specific location
// Does not reset the distance or orientation
void SetWireframeView(vector *pos) {
  Wireframe_view->target = *pos;

  Edview_changed = 1;
}
