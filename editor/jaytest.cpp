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

#include <time.h>
#include "3d.h"
#include "texture.h"
#include "object.h"
#include "vector.h"
#include "hlsoundlib.h"
#include "mono.h"
#include "d3edit.h"

vms_vertex square_data[50] = {-5.0, 5.0, -5, 5.0, 5.0, -5, 5.0, -5.0, -5, -5.0, -5.0, -5,
                              -5.0, 5.0, 5,  5.0, 5.0, 5,  5.0, -5.0, 5,  -5.0, -5.0, 5};

g3s_face SquareSides[] = {{4, {0, 1, 2, 3}}, {4, {1, 5, 6, 2}}, {4, {5, 4, 7, 6}},
                          {4, {4, 0, 3, 7}}, {4, {4, 5, 1, 0}}, {4, {3, 2, 6, 7}}};

g3s_polymodel SquareModel = {6, 8, SquareSides, square_data};

object *Square = NULL;
object *Square2 = NULL;

extern SoundLib SndLib;

void DrawWireframeList(grViewport *vp, g3s_point *p, int nv, ddgr_color color) {
  g3s_point *p1, *p2;
  int i;

  for (i = 0; i < nv; i++) {
    p1 = &p[i];
    p2 = &p[(i + 1) % nv];

    vp->line(color, p1->screenx, p1->screeny, p2->screenx, p2->screeny);
  }
}

void RenderTexFrame(grViewport *my_viewport) {
  int nv, i, t;
  g3s_point phold[70], dhold[70], fhold[70];
  g3s_point *plist = phold, *dlist = dhold, *flist = fhold;
  vms_vector vpos = {0, 0, 0};
  vms_orient vang = {0, 0, 0};

  static float sx = 0, sy = 0, sz = 15, rl = 0.0;
  static float xdir = .4;
  static float ydir = .5;
  static float zdir = .7;
  static float ldir = .05;

  static frame = 0;
  static time_t oldtime = 0;
  static int framecount = 0;
  time_t newtime;

  static char f_play_sound = 1;

  framecount++;
  g3_SetViewAngles(&vpos, &vang, 0);

  newtime = time(NULL);
  if (newtime != oldtime) {
    mprintf_at(1, 2, 0, "%d FPS    ", framecount);
    oldtime = newtime;
    framecount = 0;
  }

  SndLib.begin_sound_frame();

  if (Square2 == NULL) {
    if ((Square2 = AllocateObject()) == NULL)
      return;
    mng_LoadAllTextures();
    D3EditState.texdlg_texture = 0;
  }

  frame++;
  sx = sx + xdir;
  sy = sy + ydir;
  sz = sz + zdir;
  rl = rl + ldir;

  if (rl < 0 || rl > 1) {
    ldir = -ldir;
    rl = rl + ldir;
  }
  if (sx > 10 || sx < -10) {
    xdir = -xdir;
    //		f_play_sound = 1;
  }
  if (sy > 10 || sy < -10) {
    ydir = -ydir;
    //		f_play_sound = 1;
  }
  if (sz > 170 || sz < .000001) {
    zdir = -zdir;
    sz += zdir;
    //		f_play_sound = 1;
  }

  PositionObject(Square2, sx, sy, sz);
  if (f_play_sound) {
    f_play_sound = 0;
    pos_state cur_pos;
    cur_pos.position.x = 0;
    cur_pos.position.y = 0;
    cur_pos.position.z = 80;
    SndLib.play_3d_sound(1, cur_pos, OF_LOOPED);
  }
  SndLib.update_listener_properties(Square2->cur_pos);

  g3_BeginFrame();
  g3_TransModelToCamera(plist, &SquareModel, Square2);

  Square2->cur_pos.orientation_angles.heading += GAME_ANGLE * 3;
  Square2->cur_pos.orientation_angles.pitch += GAME_ANGLE * 2;

  Current_surface = my_viewport->lock();

  for (i = 0; i < SquareModel.num_of_polys; i++) {

    flist = fhold;
    dlist = dhold;

    for (t = 0; t < 4; t++)
      fhold[t] = plist[SquareModel.faces[i].points[t]];

    fhold[0].monolight = fhold[1].monolight = fhold[2].monolight = fhold[3].monolight = rl;

    fhold[0].ucoord = 0;
    fhold[0].vcoord = 0;
    fhold[1].ucoord = 1.0;
    fhold[1].vcoord = 0;
    fhold[2].ucoord = 1.0;
    fhold[2].vcoord = 1.0;
    fhold[3].ucoord = 0;
    fhold[3].vcoord = 1.0;
    fhold[0].flags = fhold[1].flags = fhold[2].flags = fhold[3].flags =
        PF_TEXTURE | PF_COLORED_LIGHTING | PF_MONO_LIGHTING;

    nv = g3_CheckToClip(&flist, &dlist, 4);

    if (sz > 60)
      tex_SetRenderType(RENDERTYPE_LIN_NON_UNLIT);
    else
      tex_SetRenderType(RENDERTYPE_LIN_NON_UNLIT);

    if (nv > 0) {
      if (g3_ProjectPointListToCanvas(dlist, nv)) {
        if (g3_PointsVisible(dlist, nv)) {
          tex_TriangulatePointList(D3EditState.texdlg_texture, dlist, nv);
        }
      }
    }
  }

  my_viewport->unlock();
  SndLib.end_sound_frame();
}

void RenderWireFrame(grViewport *my_viewport) {
  int nv = 4;
  g3s_point phold[10], dhold[10];
  g3s_point *plist = phold, *dlist = dhold;
  static float sx = 0, sy = 0, sz = 20;
  static float xdir = .4;
  static float ydir = .3;
  static float zdir = .6;

  if (Square == NULL)
    if ((Square = AllocateObject()) == NULL)
      return;

  PositionObject(Square, sx, sy, sz);
  sx = sx + xdir;
  sy = sy + ydir;
  sz = sz + zdir;
  if (sx > 10 || sx < -10)
    xdir = -xdir;
  if (sy > 10 || sy < -10)
    ydir = -ydir;
  if (sz > 50 || sz < 5)
    zdir = -zdir;

  g3_BeginFrame();
  g3_TransVertsToCamera(plist, square_data, Square, 4);
  nv = g3_CheckToClip(&plist, &dlist, 4);

  Square->cur_pos.orientation_angles.heading += GAME_ANGLE * 2;
  Square->cur_pos.orientation_angles.pitch += GAME_ANGLE * 2;

  if (nv > 0) {
    g3_ProjectPointListToCanvas(dlist, nv);
    Current_surface = my_viewport->lock();
    DrawWireframeList(my_viewport, dlist, nv, GR_RGB(255, 0, 255));
    my_viewport->unlock();
  }
}
