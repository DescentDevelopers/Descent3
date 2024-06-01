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

 * $Logfile: /DescentIII/main/SmallViews.cpp $
 * $Revision: 33 $
 * $Date: 4/19/00 5:07p $
 * $Author: Matt $
 *
 * System to draw small 3D views on the screen
 *
 * $Log: /DescentIII/main/SmallViews.cpp $
 *
 * 33    4/19/00 5:07p Matt
 * From Duane for 1.4
 * Added checks, asserts, and fixes for bad return values
 *
 * 32    4/06/00 9:25a Matt
 * Fixed a screen clear problem on at ATI Rage Fury Maxx in dual-chip mode
 * by forcing the screen to clear four times (instead of three).
 *
 * 31    5/17/99 11:19a Samir
 * scale small views in full hud when shrinking screen.
 *
 * 30    5/08/99 1:06a Samir
 * shrink font according to screen size.
 *
 * 29    5/01/99 12:41a Matt
 * Added static when a small viewer object dies.
 *
 * 28    4/29/99 2:00a Jason
 * fixed guided missile zbuffer problem
 *
 * 27    4/19/99 1:31p Matt
 * Made guided missile small view use title and crosshairs flag, instead
 * of doing special check for guided missile.
 *
 * 26    4/14/99 3:57a Jeff
 * fixed case mismatch in #includes
 *
 * 25    4/12/99 1:10a Matt
 * Added labels for small views.
 *
 * 24    3/31/99 3:01p Jason
 * fixed bug with guided missile
 *
 * 23    3/22/99 4:26p Samir
 * added toggles for guided missile view and reticle.
 *
 * 22    3/04/99 2:56p Samir
 * clear out flags for displaying small views for cockpit in
 * ResetSmallViews.
 *
 * 21    2/27/99 5:09p Jason
 * clear screen 3 times with z buffer
 *
 * 20    2/10/99 1:47p Matt
 * Changed object handle symbolic constants
 *
 * 19    2/08/99 5:37p Jason
 * added comment
 *
 * 18    1/31/99 7:26p Matt
 * Renamed a bunch of functions to have HUD capitalized
 *
 * 17    1/13/99 12:42p Matt
 * Added code to close a popup view
 *
 * 16    1/08/99 4:34p Matt
 * Added an Int3()
 *
 * 15    12/08/98 1:47p Matt
 * If the main view is showing the rear view, make the small rear views
 * show the forward view (got it?)
 *
 * 14    10/23/98 12:05p Matt
 * Position the small views in the cockpit relative to the window
 * position.
 *
 * 13    10/21/98 10:18a Matt
 * Don't do small views in letterbox mode
 *
 * 12    10/20/98 9:23p Matt
 * When rendering the small views, set Viewer_object to the local viewer,
 * to keep the player ship from rendering in the small views when using
 * the external camera.
 *
 * 11    10/20/98 12:42p Matt
 * Made the small views work on the cockpit.
 *
 * 10    10/20/98 12:28a Matt
 * When turning off a small view, redraw the background screen.
 *
 * 9     10/14/98 6:01p Jason
 * fixed small views going too high on the terrain
 *
 * 8     10/08/98 5:59p Jason
 * fixed object and room popping with small views
 *
 * 7     5/26/98 11:36a Matt
 * Changed small view system to allow the popup window in any of the three
 * positions, to allow any window to be the "bigger" size, and to restore
 * the old view when a popup view goes away.
 *
 * 6     4/30/98 6:46p Jason
 * more framerate testing
 *
 * 5     2/09/98 3:19p Matt
 * Added function to return the viewer object for a small view
 *
 * 4     2/04/98 6:09p Matt
 * Changed object room number to indicate a terrain cell via a flag.  Got
 * rid of the object flag which used to indicate terrain.
 *
 * 3     2/04/98 12:50a Matt
 * Added ability to view from a gun point in small views.
 * Made pop-up view separate from and bigger than other small views.
 * Added real keys to set view mode in the three small views.
 * Changed rendering functions a bit to accommodate smallview changes.
 *
 * 2     1/30/98 2:55p Matt
 * Added SmallViews system
 *
 * 1     1/29/98 11:15a Matt
 *
 */

#include "SmallViews.h"

#include "descent.h"
#include "pserror.h"

#include "game.h"
#include "object.h"
#include "terrain.h"
#include "render.h"
#include "gameloop.h"
#include "weapon.h"
#include "findintersection.h"
#include "config.h"
#include "terrain.h"
#include "gauges.h"
#include "cockpit.h"
#include "player.h"
#include "grtext.h"
#include "stringtable.h"
#include "gamefont.h"

// How many small views
#define NUM_SMALL_VIEWS 3

// How many chars to allow for the window label
#define LABEL_LEN 19

// Struct to keep data for small views
struct small_view {
  int objhandle;             // the object we're viewing from, or OBJECT_HANDLE_NONE if view not active
  int flags;                 // various flags
  float timer;               // how much longer time window stays alive, if SVF_TIMED flag is set
  float zoom;                // the zoom for this window
  int gun_num;               // which gun to view from, or -1 if none
  char label[LABEL_LEN + 1]; // the label for the window
};

// The array of small views
static small_view Small_views[NUM_SMALL_VIEWS];

// Saved copies of small views for while a pop-up is active
static small_view Small_views_save[NUM_SMALL_VIEWS];

// guided missile in small view? if so, then true.
bool Guided_missile_smallview = false;
static int Guided_missile_objhandle = OBJECT_HANDLE_NONE;

// Returns the viewer object for the specified small view.
// If the view isn't active, returns OBJECT_HANDLE_NONE.
int GetSmallViewer(int window) {
  ASSERT((window >= 0) && (window < NUM_SMALL_VIEWS));

  return Small_views[window].objhandle;
}

// Create a new small view.  If there is already a view in the given window, the old view gets blown away.
// Parameters:	window - which window to open.  See constants in SmallViews.h
//					objhandle - handle for the object to view from
//					flags - various view attributes.  See defines in header file.
//					time - how long to keep the window up.  If 0, keep up indefinitely
//					zoom - the zoom for this window.  If 0, use the default zoom
//					gun_num - which gun to view from. if -1, use viewer's center and orientation.
//					label - the label for the window
// Returns:		which window was opened, or -1 if window couldn't be created
int CreateSmallView(int window, int objhandle, int flags, float time, float zoom, int gun_num, const char *label) {
  small_view *svp;

  // Check for valid window
  if ((window < 0) || (window >= NUM_SMALL_VIEWS)) {
    Int3();
    window = SVW_LEFT;
  }

  // Make sure the object exists
  if (!ObjGet(objhandle))
    return -1;

  svp = &Small_views[window];

  // If this is a pop-up, and there was a non-popup active, save previous view
  if ((flags & SVF_POPUP) && (svp->objhandle != OBJECT_HANDLE_NONE) && !(svp->flags & SVF_POPUP))
    Small_views_save[window] = *svp;

  // Set vars for new view
  svp->objhandle = objhandle;
  svp->zoom = zoom;
  svp->gun_num = gun_num;
  svp->flags = flags;
  svp->timer = time;
  if (time != 0.0)
    svp->flags |= SVF_TIMED;

  strncpy(svp->label, label ? label : "", LABEL_LEN);
  svp->label[LABEL_LEN] = 0; // strncpy won't terminate if at max len

  if (window == SVW_LEFT)
    Disable_primary_monitor = 1;
  else if (window == SVW_RIGHT)
    Disable_secondary_monitor = 1;

  // if guided view, set flag to true
  if (ObjGet(objhandle) == Players[Player_num].guided_obj) {
    Guided_missile_smallview = true;
    Guided_missile_objhandle = objhandle;
  }

  return window;
}

// Called to get rid of all the small views & init system
void ResetSmallViews() {
  for (int v = 0; v < NUM_SMALL_VIEWS; v++) {
    Small_views[v].objhandle = OBJECT_HANDLE_NONE;
    Small_views_save[v].objhandle = OBJECT_HANDLE_NONE;
  }
  Disable_primary_monitor = 0;
  Disable_secondary_monitor = 0;
  Guided_missile_smallview = false;
  Guided_missile_objhandle = OBJECT_HANDLE_NONE;
}

#include "vclip.h"

// How long the static is visbile
#define STATIC_TIME 0.25f

// Render into one of the small windows
void RenderSmallWindow(int left, int top, int right, int bot, object *viewer, vector *viewer_eye, int viewer_roomnum,
                       matrix *viewer_orient, int flags, float zoom, ddgr_color outline_color, char *label,
                       float timer) {
  int width = right - left, height = bot - top;

  // Set up for rendering
  StartFrame(left, top, right, bot);

  //	scale font according to screen size
  float font_aspect_x = (float)Game_window_w / Max_window_w;

  if (font_aspect_x <= 0.60f) {
    grtext_SetFontScale(0.60f);
  } else if (font_aspect_x <= 0.80f) {
    grtext_SetFontScale(0.80f);
  } else {
    grtext_SetFontScale(1.0f);
  }

  // Draw either static or 3D
  if (flags & SVF_STATIC) {
    // draw static
    static int handle = -1;
    if (handle == -1)
      handle = FindTextureName("Static");

    ASSERT(handle != -1); // DAJ -1FIX

    PageInVClip(GameTextures[handle].bm_handle);
    vclip *vc = &GameVClips[GameTextures[handle].bm_handle];

    int frame = vc->num_frames * (STATIC_TIME - timer) / STATIC_TIME;
    if (frame == vc->num_frames)
      frame--;
    int bm_handle = vc->frames[frame];

    rend_SetAlphaType(AT_CONSTANT_TEXTURE);
    rend_SetAlphaValue(255);
    rend_SetLighting(LS_NONE);
    rend_SetColorModel(CM_MONO);
    rend_SetOverlayType(OT_NONE);
    rend_SetWrapType(WT_CLAMP);
    rend_SetFiltering(0);

    rend_DrawScaledBitmap(0, 0, width - 1, height - 1, bm_handle, 0.0, 0.0, 1.0, 1.0);
  } else {
    // draw 3D view
    ASSERT(viewer != NULL);
    ASSERT(viewer_eye != NULL);
    object *save_viewer_object = Viewer_object;
    bool rear_view = ((flags & SVF_REARVIEW) != 0);

    // Reset facings for mine stuff
    ResetFacings();

    // Set the viewer for this render
    Viewer_object = viewer;

    // Render the world
    GameRenderWorld(viewer, viewer_eye, viewer_roomnum, viewer_orient, zoom, rear_view);

    // Restore the viewer
    Viewer_object = save_viewer_object;
  }

  // Draw window outline
  if (outline_color != -1) {
    rend_SetFlatColor(outline_color);
    rend_DrawLine(0, 0, width - 1, 0);
    rend_DrawLine(width - 1, 0, width - 1, height - 1);
    rend_DrawLine(width - 1, height - 1, 0, height - 1);
    rend_DrawLine(0, height - 1, 0, 0);
  }

  // Draw the label
  if (label[0]) {
    int w = grtext_GetLineWidth(label);
    grtext_SetFont(HUD_FONT);
    grtext_SetColor(HUD_COLOR);
    grtext_SetAlpha(255);
    grtext_SetFlags(0);
    grtext_Puts((width - w) / 2, 2, label);
    grtext_Flush();
  }

  // Draw the crosshairs
  if (flags & SVF_CROSSHAIRS) {
    int cx = width / 2, cy = height / 2;
    int line_w = width / 50, line_h = height / 50;
    rend_SetZBufferState(0);
    rend_SetFlatColor(GR_GREEN);
    rend_DrawLine(cx - line_w, cy, cx + line_w, cy);
    rend_DrawLine(cx, cy - line_h, cx, cy + line_h);
    rend_SetZBufferState(1);
  }

  // Done rendering
  EndFrame();
}

// Find the room number of a gun point
int GetGunRoom(vector *p0, int roomnum, int objnum, vector *p1) {
  fvi_query fq;
  fvi_info hit_data;
  int fate;

  fq.p0 = p0;
  fq.startroom = roomnum;
  fq.p1 = p1;
  fq.rad = 0;
  fq.thisobjnum = objnum;
  fq.ignore_obj_list = NULL;
  fq.flags = 0;

  fate = fvi_FindIntersection(&fq, &hit_data);
  if (fate != HIT_NONE)
    Int3();

  return hit_data.hit_room;
}

#define SMALL_WINDOW_W 120
#define SMALL_WINDOW_H 120
#define SPACE_BETWEEN_WINDOWS ((640 - 3 * SMALL_WINDOW_W) / 3)

extern int Point_visible_last_frame;

// Draw all the active small views
void DrawSmallViews() {
  int v;
  small_view *svp;
  int left, top, right, bot;
  ddgr_color outline_color;
  vector *viewer_eye, gun_pos;
  matrix *viewer_orient, gun_orient;
  matrix save_orient;
  vector save_eye;
  int viewer_roomnum;

  // if letterbox, don't draw the small views
  if (GetHUDMode() == HUD_LETTERBOX)
    return;

  bool lsave = Detail_settings.Dynamic_lighting;

  g3_GetUnscaledMatrix(&save_orient);
  g3_GetViewPosition(&save_eye);

  Detail_settings.Dynamic_lighting = false;

  for (v = 0, svp = Small_views; v < NUM_SMALL_VIEWS; v++, svp++) {
    if (svp->objhandle == OBJECT_HANDLE_NONE)
      continue;

    object *viewer = ObjGet(svp->objhandle);
    bool kill_viewer = false;

    // Bail if object not around any more, or if its gone too high
    if (!viewer || (viewer->type == OBJ_DUMMY))
      kill_viewer = true;

    if (viewer && OBJECT_OUTSIDE(viewer) && viewer->pos.y >= MAX_TERRAIN_HEIGHT)
      kill_viewer = true;

    // If not view & not already static, switch to static
    if (kill_viewer && !(svp->flags & SVF_STATIC)) {
      // Viewer is dead, so switch to static
      svp->flags |= SVF_STATIC | SVF_TIMED;
      svp->timer = STATIC_TIME;
      svp->gun_num = -1;
    }

    if (v == SVW_CENTER) // no center window for now
      Int3();

    if (CockpitState() == COCKPIT_STATE_QUASI) {
      continue;
    }

    else if (CockpitState() == COCKPIT_STATE_FUNCTIONAL) {
      // draw on the cockpit
      if (!GetCockpitWindowCoords((v == SVW_LEFT) ? 0 : 1, &left, &top, &right, &bot)) {
        continue;
      }

      // Move relative to the current window
      left += Game_window_x;
      right += Game_window_x;
      top += Game_window_y;
      bot += Game_window_y;

      outline_color = -1;
    } else {
      // draw windows on HUD based on the screen size
      ASSERT(CockpitState() == COCKPIT_STATE_DORMANT);

      // Compute size of window
      int x, y, w, h, spacing;
      w = Game_window_h / 4;
      spacing = ((Game_window_w - (NUM_SMALL_VIEWS)*w) / 3);
      x = ((spacing + w) / 2) + (v * (spacing + w)) + Game_window_x;
      h = w;
      y = Game_window_y + (Game_window_h - h / 2 - Game_window_h / 24);

      // If bigger window, increase size
      if (svp->flags & SVF_BIGGER) {
        w += w / 4;
        h += h / 4;
      }

      left = x - (w / 2);
      top = y - (h / 2);
      right = x + (w / 2);
      bot = y + (h / 2);

      // Set window color
      outline_color = (svp->flags & SVF_POPUP) ? GR_RGB(255, 0, 0) : GR_RGB(0, 0, 255);
    }

    // Get view info from the gun or from the object itself
    if (svp->gun_num != -1) {
      // using a gun
      vector gun_vector;
      WeaponCalcGun(&gun_pos, &gun_vector, viewer, svp->gun_num);
      vm_VectorToMatrix(&gun_orient, &gun_vector, NULL, NULL);
      viewer_eye = &gun_pos;
      viewer_orient = &gun_orient;
      viewer_roomnum = GetGunRoom(&viewer->pos, viewer->roomnum, OBJNUM(viewer), &gun_pos);
    } else if (viewer) {
      // no gun, so just use the object
      viewer_eye = &viewer->pos;
      viewer_orient = &viewer->orient;
      viewer_roomnum = viewer->roomnum;
    } else {
      ASSERT(svp->flags & SVF_STATIC);
      viewer_eye = NULL;
      viewer_orient = NULL;
      viewer_roomnum = -1;
    }

    // Determine whether to draw rear view.  If view has rear view set and the viewer is
    // a player and the player has rear view on, then draw forward view
    int flags = svp->flags;
    if ((flags & SVF_REARVIEW) && (viewer == Viewer_object) && (viewer->type == OBJ_PLAYER) &&
        (Players[viewer->id].flags & PLAYER_FLAGS_REARVIEW))
      flags &= ~SVF_REARVIEW;

    // Let's render
    Point_visible_last_frame = -1;
    RenderSmallWindow(left, top, right, bot, viewer, viewer_eye, viewer_roomnum, viewer_orient, flags, svp->zoom,
                      outline_color, svp->label, svp->timer);
    Point_visible_last_frame = -1;

    // Update time for this view
    if (svp->flags & SVF_TIMED) {
      svp->timer -= Frametime;
      if (svp->timer < 0.0) {
        CloseSmallView(v);
      }
    }
  }

  Detail_settings.Dynamic_lighting = lsave;

  // Resets aspect ratio
  g3_StartFrame(&save_eye, &save_orient, Render_zoom);
  g3_EndFrame();
}

// Parameters:	window - the window to get rid of
void CloseSmallView(int window) {
  // Mark the window as inactive
  if (Small_views[window].objhandle == Guided_missile_objhandle) {
    // mark guided missile small view as inactive.
    Guided_missile_objhandle = OBJECT_HANDLE_NONE;
    Guided_missile_smallview = false;
  }

  Small_views[window].objhandle = OBJECT_HANDLE_NONE;

  // Restore old view if there was one
  if (Small_views_save[window].objhandle != OBJECT_HANDLE_NONE) {
    Small_views[window] = Small_views_save[window];
    Small_views_save[window].objhandle = OBJECT_HANDLE_NONE;
  } else {
    Clear_screen = 4; // force background redraw

    if (window == SVW_LEFT)
      Disable_primary_monitor = 0;
    else if (window == SVW_RIGHT)
      Disable_secondary_monitor = 0;
  }
}

// Get rid of a small view if it's a popup window
// Parameters:	window - the window to get rid of
void ClosePopupView(int window) {
  if (Small_views[window].flags & SVF_POPUP)
    CloseSmallView(window);
}
