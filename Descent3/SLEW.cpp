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

 * $Logfile: /DescentIII/main/SLEW.cpp $
 * $Revision: 46 $
 * $Date: 4/18/99 5:42a $
 * $Author: Chris $
 *
 * Basic slew system
 *
 * $Log: /DescentIII/main/SLEW.cpp $
 *
 * 46    4/18/99 5:42a Chris
 * Added the FQ_IGNORE_RENDER_THROUGH_PORTALS flag
 *
 * 45    4/17/99 2:41p Matt
 * Added code to limit movement by axis.
 *
 * 44    4/14/99 3:57a Jeff
 * fixed case mismatch in #includes
 *
 * 43    4/10/99 5:53p Matt
 * Re-added terrain bound limit, on terrain only
 *
 * 42    4/09/99 12:02p Samir
 * joystick changes (Win32 DirectInput support)
 *
 * 41    3/01/99 6:27p Matt
 * Cleaned up handling of the outside-mine flag
 *
 * 40    2/25/99 10:44p Matt
 * Return separate flags for move & rotate
 *
 * 39    2/24/99 9:39p Matt
 * Disabled terrain bounds limitation
 *
 * 38    2/23/99 5:47p Matt
 * Fixed terrain bound limit code
 *
 * 37    1/15/99 7:52p Chris
 * Updated ObjSetPos() to include a f_update_attach_children flag
 *
 * 36    10/20/98 5:35p Sean
 * Temporary slew fix
 *
 * 35    10/15/98 8:47a Matt
 * Changed _DEBUG/RELEASE inconsistancy with slew
 *
 * 34    10/14/98 4:39p Matt
 * Changed OutrageMessageBox() calls to use either Error() or
 * EditorMessageBox()
 *
 * 33    9/22/98 3:54p Samir
 * no include slew in release version.
 *
 * 32    9/11/98 3:00p Craig
 * Added ifdef around some editor code
 *
 * 31    9/04/98 12:23p Samir
 * control whether to allow joystick slewing.
 *
 * 30    7/01/98 4:58p Samir
 * moved slew joysitck init from Init.cpp to here.
 *
 * 29    6/15/98 1:02p Samir
 * added pageup and pagedown.
 *
 * 28    5/20/98 6:59p Samir
 * added slew speed slider.
 *
 * 27    5/18/98 6:58p Samir
 * took out annoying mprinrfs.
 *
 * 26    3/12/98 7:30p Chris
 * Added ObjSetOrient
 *
 * 25    2/17/98 5:31p Samir
 * Upped joy slew.
 *
 * 24    2/15/98 7:57p Matt
 * Renamed NewSlewFrame() to be SlewFrame(), and added some mprintfs.
 *
 * 23    2/13/98 1:21p Samir
 * Maybe fixed evil slewing problem.
 *
 * 22    2/13/98 11:47a Samir
 * debug info.
 *
 * 21    2/12/98 4:08p Samir
 * Fixed a boo-boo.
 *
 * 20    2/12/98 4:01p Samir
 * Maybe this will fix slewing.
 *
 * 19    2/05/98 2:58p Matt
 * Cleaned up slew code, and made view mode switch when slewing between
 * mine and terrain happen in editor, not slew.
 *
 * 18    2/04/98 6:09p Matt
 * Changed object room number to indicate a terrain cell via a flag.  Got
 * rid of the object flag which used to indicate terrain.
 *
 * 17    1/21/98 1:08p Matt
 * Made SetViewMode() not take new_viewer parm, since it's not needed
 * after my revamp of the editor view system.
 *
 * 16    1/21/98 12:25p Matt
 * Removed unneeded include
 *
 * 15    1/07/98 10:50a Chris
 * Made it safe to slew to the edge of the terrain
 *
 * 14    12/03/97 7:35p Samir
 * New joystick library sturff.
 *
 * 13    11/16/97 5:53p Matt
 * Added some code so that slewing in Room View doesn't call FVI to update
 * the room number
 *
 * 12    11/12/97 12:57p Chris
 * Now compile under the main project
 *
 * 11    10/22/97 4:34p Chris
 * Removed a warning
 *
 * 10    10/22/97 4:30p Chris
 * We can now slew between the mine and the terrain
 *
 * 9     9/22/97 6:18p Matt
 * Removed include of stub.h
 *
 * 8     9/03/97 2:14p Chris
 * Made a few const int's to const float's as they actually are and
 * removed a literal  and replaced it with a FVIF_OUTSIDE
 *
 * 7     8/25/97 12:41p Chris
 * Made sure that all object movements and mine changes result in AABB
 * updates.
 *
 * 6     8/22/97 4:57p Matt
 * Only search for new room if slew object has moved
 *
 * 5     8/05/97 10:50a Matt
 * Fixed slewing on terrain
 *
 * 4     8/04/97 7:40p Matt
 * Use FVI to update room number when slewing
 *
 * 3     8/01/97 10:48a Samir
 * Increased null-zone for joystick.  Still some calibration problems.
 *
 * 2     7/17/97 4:34p Chris
 * Made sure the orientations get orthagonalized.  Yikes.  No more
 * degenerating orientations.  :)
 *
 * 13    5/13/97 11:40a Samir
 * No joystick stuff for noneditor
 *
 * 12    4/01/97 9:45p Matt
 * Don't require an object to have control type of slew in order to slew
 * (so we can slew the editor's viewer object)
 *
 * 11    2/27/97 10:35a Matt
 * Got joystick slewing working
 *
 * 10    2/20/97 4:30p Samir
 * Adjust slewing code to return whether a key was pressed as well as
 * disallowing slewing when control keys are down.
 *
 * 9     2/18/97 6:31p Matt
 * Changed Frame_time to Frametime, & added Gametime
 *
 * 8     2/07/97 5:48p Matt
 * Renamed vector.h to vecmat.h to fix DevStudio problem
 *
 * $NoKeywords: $
 */

#if (defined(_DEBUG) || defined(EDITOR))

#ifdef EDITOR
#include "editor\mainfrm.h"
#include "editor\d3edit.h"
#include "pserror.h"
#endif

#include <cstdlib>

#include "slew.h"
#include "vecmat.h"
#include "ddio.h"
#include "object.h"
#include "log.h"
#include "game.h"
#include "joystick.h"
#include "findintersection.h"
#include "room.h"

// variables for slew system

#define ROT_SPEED (1.0 / 8.0) // rate of rotation while key held down
#define VEL_SPEED (110.0)     // rate of acceleration while key held down
#define JOY_NULL 32

float Slew_key_speed = 1.0f;
int Joystick_active = -1;

// -------------------------------------------------------------------

#ifdef EDITOR
void SlewControlInit() {
  Joystick_active = -1;

  if (!D3EditState.joy_slewing)
    return;

  if (joy_IsValid(JOYSTICK_1)) {
    tJoyPos joystate;
    tJoystick joyid = JOYSTICK_1;

    Joystick_active = (int)joyid;
    joy_GetPos((tJoystick)Joystick_active, &joystate); // get all the stick values

    if ((abs(joystate.x) > 32) || (abs(joystate.y) > 32))
      EditorMessageBox("Warning: Your joystick is not centered.  You should either center it now or recalibrate.");
  }
}
#endif

int SlewStop(object *obj) {
  if (!obj)
    return 0;

  vm_MakeZero(&obj->mtype.phys_info.velocity);

  return 1;
}

// Resets object's orientation
void SlewResetOrient(object *obj) {
  if (!obj)
    return;

  ObjSetOrient(obj, &Identity_matrix);
}

// Moves the object for one frame
int SlewFrame(object *obj, int movement_limitations) {
  static int16_t old_joy_x = 0, old_joy_y = 0; // position last time around
  int ret_flags = 0;
  vector svel, movement; // scaled velocity (per this frame)
  matrix rotmat, new_pm;
  angvec rotang;
  vector rottime;
  vector new_pos;
  int new_room;
  fvi_query fq;
  fvi_info hit_info;
  int fate;

  float key_timex1 = 0, key_timex0 = 0;
  float key_timey1 = 0, key_timey0 = 0;
  float key_timez1 = 0, key_timez0 = 0;
  float key_timep1 = 0, key_timep0 = 0;
  float key_timeh1 = 0, key_timeh0 = 0;
  float key_timeb1 = 0, key_timeb0 = 0;

  if (!obj)
    return 0;

  //	check keyboard for slewing.
  key_timex1 = ddio_KeyDownTime(KEY_PAD9);
  key_timex0 = ddio_KeyDownTime(KEY_PAD7);
  key_timey1 = ddio_KeyDownTime(KEY_PADMINUS);
  key_timey0 = ddio_KeyDownTime(KEY_PADPLUS);
  key_timez1 = ddio_KeyDownTime(KEY_PAD8);
  key_timez0 = ddio_KeyDownTime(KEY_PAD2);
  key_timep1 = ddio_KeyDownTime(KEY_LBRACKET);
  key_timep0 = ddio_KeyDownTime(KEY_RBRACKET);
  key_timeh1 = ddio_KeyDownTime(KEY_PAD6);
  key_timeh0 = ddio_KeyDownTime(KEY_PAD4);
  key_timeb1 = ddio_KeyDownTime(KEY_PAD1);
  key_timeb0 = ddio_KeyDownTime(KEY_PAD3);

  if (!key_timep1)
    key_timep1 = ddio_KeyDownTime(KEY_PAGEDOWN);
  if (!key_timep0)
    key_timep0 = ddio_KeyDownTime(KEY_PAGEUP);

  if (key_timex1 || key_timex0 || key_timey1 || key_timey0 || key_timez1 || key_timez0)
    ret_flags |= SLEW_KEY;
  if (key_timep1 || key_timep0 || key_timeh1 || key_timeh0 || key_timeb1 || key_timeb0)
    ret_flags |= SLEW_KEY;

  //	adjust physics info of object accordingly to keyboard input.
  obj->mtype.phys_info.velocity.x += VEL_SPEED * (key_timex1 - key_timex0) * Slew_key_speed;
  obj->mtype.phys_info.velocity.y += VEL_SPEED * (key_timey1 - key_timey0) * Slew_key_speed;
  obj->mtype.phys_info.velocity.z += VEL_SPEED * (key_timez1 - key_timez0) * Slew_key_speed;

  rottime.x = key_timep1 - key_timep0;
  rottime.y = key_timeh1 - key_timeh0;
  rottime.z = key_timeb1 - key_timeb0;
  rotang.p = (int16_t)(65536.0 * rottime.x * ROT_SPEED * Slew_key_speed);
  rotang.h = (int16_t)(65536.0 * rottime.y * ROT_SPEED * Slew_key_speed);
  rotang.b = (int16_t)(65536.0 * rottime.z * ROT_SPEED * Slew_key_speed);

// joystick movement
#ifdef EDITOR
  if (Joystick_active != -1) {
    int joy_x, joy_y, btns;
    tJoyPos joystate;
    bool joyx_moved = false;
    bool joyy_moved = false;

    joy_GetPos((tJoystick)Joystick_active, &joystate); // get all the stick values

    joy_x = joystate.x;
    joy_y = joystate.y;
    btns = joystate.buttons;

    if (abs(joy_x) < JOY_NULL)
      joy_x = 0;
    if (abs(joy_y) < JOY_NULL)
      joy_y = 0;

    joyx_moved = (abs(joy_x - old_joy_x) > JOY_NULL);
    joyy_moved = (abs(joy_y - old_joy_y) > JOY_NULL);

    if (btns) {
      if (!rotang.p)
        rotang.p = -joy_y * 256 * Frametime;
    } else {
      if (joyy_moved)
        obj->mtype.phys_info.velocity.z = (float)-joy_y / 4.0;
    }

    if (!rotang.h)
      rotang.h = joy_x * 256 * Frametime;

    if (joyx_moved)
      old_joy_x = joy_x;
    if (joyy_moved)
      old_joy_y = joy_y;
  }
#endif

  vm_AnglesToMatrix(&rotmat, rotang.p, rotang.h, rotang.b);

  new_pm = obj->orient * rotmat;
  vm_Orthogonalize(&new_pm);

  ObjSetOrient(obj, &new_pm);

  vm_TransposeMatrix(&new_pm); // make those columns rows

  svel = obj->mtype.phys_info.velocity * Frametime;
  movement = svel * new_pm;

  if (movement_limitations & 1)
    movement.x = 0;
  if (movement_limitations & 2)
    movement.y = 0;
  if (movement_limitations & 4)
    movement.z = 0;

  new_pos = obj->pos + movement;

  // Did the object position change?
  if ((movement.x != 0.0f) || (movement.y != 0.0f) || (movement.z != 0.0f))
    ret_flags |= SLEW_MOVE;

  if (ret_flags & SLEW_MOVE) { // Get the new room
    bool outside_mine = ((obj->flags & OF_OUTSIDE_MINE) != 0);

    LOG_DEBUG << "SLEW: Moved";

#ifdef EDITOR
    if (Editor_view_mode == VM_ROOM) {
      // Room number is bogus in room view, so don't update it
      new_room = obj->roomnum;
    } else
#endif
      // NOTE LINK TO ABOVE IF
      if (outside_mine) { // starting outside the mine?

        // See if we've moved back into a room
        new_room = FindPointRoom(&new_pos);

        if (new_room != -1) { // back in the mine
          outside_mine = 0;
          LOG_DEBUG.printf("SLEW: Re-entered mine at room %d", new_room);
        } else // not back in the mine
          new_room = obj->roomnum;
      } else {
        bool was_outside = (ROOMNUM_OUTSIDE(obj->roomnum) != 0);

        // Limit new position to terrain bounds if outside
        if (was_outside) {
          if (new_pos.x < 1.0)
            new_pos.x = 1.0;
          if (new_pos.x > TERRAIN_WIDTH * TERRAIN_SIZE - 1.0)
            new_pos.x = TERRAIN_WIDTH * TERRAIN_SIZE - 1.0;
          if (new_pos.z < 1.0)
            new_pos.z = 1.0;
          if (new_pos.z > TERRAIN_DEPTH * TERRAIN_SIZE - 1.0)
            new_pos.z = TERRAIN_WIDTH * TERRAIN_SIZE - 1.0;
        }

        // Call FVI up get updated room number
        fq.p0 = &obj->pos;
        fq.startroom = obj->roomnum;
        fq.p1 = &new_pos;
        fq.rad = 0;
        fq.thisobjnum = OBJNUM(obj);
        fq.ignore_obj_list = NULL;
        fq.flags = FQ_IGNORE_RENDER_THROUGH_PORTALS;
        fate = fvi_FindIntersection(&fq, &hit_info);

        // If bad room, don't move
        if ((fate == HIT_OUT_OF_TERRAIN_BOUNDS) || (hit_info.hit_room == -1)) {
          new_room = obj->roomnum;
          new_pos = obj->pos;
        } else
          new_room = hit_info.hit_room;

        // The object hit a wall, and maybe went outside the mine.
        if (fate == HIT_WALL) {
          int t;

          LOG_DEBUG << "SLEW: hit wall";

          // Check if we're in a room
          t = FindPointRoom(&new_pos);

          if (t != -1) { // We're in a room
            new_room = t;
            LOG_DEBUG.printf("SLEW: still in mine in room %d", new_room);
          } else { // Not in a room.  Set a special flag
            outside_mine = 1;
            LOG_DEBUG.printf("SLEW: left mine from room %d", new_room);
          }
        }

        if (new_room != obj->roomnum) { // if we've changed rooms, say so
          if (ROOMNUM_OUTSIDE(new_room))
            if (was_outside)
              LOG_DEBUG.printf("SLEW: Moved to cell %d, BOA TR %d", CELLNUM(new_room), TERRAIN_REGION(new_room));
            else
              LOG_DEBUG.printf("SLEW: Moved outside to cell %d", CELLNUM(new_room));
          else if (was_outside)
            LOG_DEBUG.printf("SLEW: Moved inside to room %d", new_room);
          else
            LOG_DEBUG.printf("SLEW: Moved into room %d", new_room);
        }
      }

    // Now we have the new room, so update the object position
    ObjSetPos(obj, &new_pos, new_room, NULL, false);

    // Set outside-mine flag if we're outside
    if (outside_mine)
      obj->flags |= OF_OUTSIDE_MINE;
  }

  // Set flag if rotation changed
  if ((rotang.p != 0) || (rotang.h != 0) || (rotang.b != 0))
    ret_flags |= SLEW_ROTATE;

  return ret_flags;
}

#endif
