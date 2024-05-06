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

 * $Logfile: /DescentIII/main/spew.cpp $
 * $Revision: 28 $
 * $Date: 10/31/99 5:41p $
 * $Author: Chris $
 *
 * Source file for spew system
 *
 * $Log: /DescentIII/main/spew.cpp $
 *
 * 28    10/31/99 5:41p Chris
 * Fixed a problem with spewers on attached objects
 *
 * 27    5/02/99 2:52a Jeff
 * fixed spewers for turrets/rotaters
 *
 * 26    4/30/99 8:32p Jeff
 * more info on mprintf
 *
 * 25    4/29/99 7:50p Jason
 * fixed spew errors
 *
 * 24    4/29/99 5:09p Jeff
 * fixed spewers on animated objects
 *
 * 23    4/22/99 8:29p Kevin
 * made psrand.h come after stdlib.h
 *
 * 22    4/21/99 11:05a Kevin
 * new ps_rand and ps_srand to replace rand & srand
 *
 * 21    4/19/99 3:17p Jason
 * made real vis effects work
 *
 * 20    4/15/99 1:42a Jeff
 * changes for linux compile
 *
 * 19    1/21/99 11:15p Jeff
 * pulled out some structs and defines from header files and moved them
 * into seperate header files so that multiplayer dlls don't require major
 * game headers, just those new headers.  Side effect is a shorter build
 * time.  Also cleaned up some header file #includes that weren't needed.
 * This affected polymodel.h, object.h, player.h, vecmat.h, room.h,
 * manage.h and multi.h
 *
 * 18    1/13/99 2:32p Jeff
 * added error checking on create...returns -1 on error
 *
 * 17    10/18/98 9:15p Jeff
 * ok....it's sped up now
 *
 * 16    10/18/98 9:04p Jeff
 * sped up spew by saving values?
 *
 * 15    8/19/98 5:36p Samir
 * moved some stuff from source to header file.
 *
 * 14    7/27/98 6:26p Jeff
 * added an mprintf for my sanity
 *
 * 13    2/16/98 2:33p Jeff
 * Now passing real flag when creating a spew
 *
 * 12    2/04/98 6:09p Matt
 * Changed object room number to indicate a terrain cell via a flag.  Got
 * rid of the object flag which used to indicate terrain.
 *
 * 11    2/02/98 5:05p Luke
 *
 * 10    2/02/98 4:38p Jeff
 * fixed handle system (stupid bug)
 *
 * 9     2/02/98 3:10p Jeff
 * new handle system, makes more reliable
 *
 * 8     1/27/98 10:49a Jeff
 * commented out mprintfs
 *
 * 7     1/26/98 4:20p Jeff
 * added some debugging mprintfs
 *
 * 6     1/26/98 11:01a Jeff
 * changed spew struct to use object handles
 *
 * 5     1/23/98 7:00p Jeff
 * checked object before using it bug fixed
 *
 * 4     1/23/98 5:18p Jeff
 * Added in new timer and 3/5 of the random flags
 *
 * 3     1/23/98 3:03p Jeff
 * Added mass into spew structure
 *
 * 2     1/23/98 12:14p Jeff
 * Took out an mprintf
 *
 * 1     1/23/98 12:07p Jeff
 *
 * 2     1/23/98 11:50a Jeff
 * Creation of Spew system
 *
 * $NoKeywords: $
 */

//	ancillary includes
#include "game.h"
#include "vecmat.h"
#include "ddio.h"
#include "pserror.h"
#include "object.h"
#include "damage.h"
#include "viseffect.h"
#include "fireball.h"
#include "spew.h"
#include "weapon.h"
#include "polymodel.h"

//	ANSI C includes
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "psrand.h"

#define MAX_SPEWS_PER_FRAME 5 // maximum number of spews 1 can emit per frame

static inline int FORM_HANDLE(int counter, int slot) { return (((counter & 0xFF) << 16) + (slot & 0xFF)); }

static inline int GET_SLOT(int handle) { return (handle & 0xFF); }

spewinfo SpewEffects[MAX_SPEW_EFFECTS];
int spew_count;

bool SpewObjectNeedsEveryFrameUpdate(object *obj, int gunpoint);

// Initializes the Spew system
void SpewInit() {
  mprintf((0, "Initializing Spew System\n"));
  int count;
  spew_count = 0;

  for (count = 0; count < MAX_SPEW_EFFECTS; count++) {
    SpewClearEvent(count, true);
  }
  mprintf((0, "Done Initializing Spew System\n"));
}

// Creates a Spew effect
// spew	->	Pointer to spewinfo structure (that is filled out)...may be deleted after function call
// returns a handle to the spew effect
int SpewCreate(spewinfo *spew) {
  int count;
  spewinfo *veffect;

  ASSERT(spew);

  if (spew->drag < 0)
    spew->drag = 0;
  if (spew->effect_type < 0 || spew->effect_type >= NUM_FIREBALLS)
    return -1;
  if (spew->lifetime < 0)
    spew->lifetime = 0;
  if (spew->longevity < 0)
    spew->longevity = 0;
  if (spew->mass < 0)
    spew->mass = 0;
  if (spew->size < 1)
    spew->size = 1;
  if (spew->speed < 0)
    spew->speed = 0;
  if (spew->time_int <= 0)
    spew->time_int = 0.001f;
  if (spew->use_gunpoint) {
    // make sure all the object data is valid
    object *obj = ObjGet(spew->gp.obj_handle);
    if (!obj)
      return -1;

    poly_model *pm = &Poly_models[obj->rtype.pobj_info.model_num];
    if (spew->gp.gunpoint < 0 || spew->gp.gunpoint >= pm->n_guns)
      return -1;
  }

  for (count = 0; count < MAX_SPEW_EFFECTS; count++) {
    if (SpewEffects[count].inuse == false) {
      spew_count++;
      // we found an open handle
      veffect = &SpewEffects[count];

      // fill in the info
      veffect->inuse = true;
      veffect->random = spew->random;
      veffect->use_gunpoint = spew->use_gunpoint;
      veffect->real_obj = spew->real_obj;
      veffect->flags = 0;

      if (veffect->use_gunpoint) {
        veffect->gp.obj_handle = spew->gp.obj_handle;

        // WHAT IS THIS? it used to be "if(veffect->gp.obj_handle==NULL)"
        if (veffect->gp.obj_handle == OBJECT_HANDLE_NONE) {
          SpewClearEvent(count, false);
          return -1;
        }
        veffect->gp.gunpoint = spew->gp.gunpoint;
        veffect->flags |= SF_FORCEUPDATE; // force an update so it has some values

        object *obj = ObjGet(spew->gp.obj_handle);
        ASSERT(obj);

        if (obj) {
          if (SpewObjectNeedsEveryFrameUpdate(obj, spew->gp.gunpoint)) {
            veffect->flags |= SF_UPDATEEVERYFRAME;
          }
        }
      } else {
        veffect->pt.origin = spew->pt.origin;
        veffect->pt.normal = spew->pt.normal;
        veffect->pt.room_num = spew->pt.room_num;
      }

      veffect->effect_type = spew->effect_type;
      veffect->phys_info = spew->phys_info;
      veffect->drag = spew->drag;
      veffect->mass = spew->mass;
      veffect->time_until_next_blob = veffect->time_int = spew->time_int;
      veffect->longevity = spew->longevity;
      veffect->lifetime = spew->lifetime;
      veffect->size = spew->size;
      veffect->speed = spew->speed;

      veffect->handle = FORM_HANDLE(spew_count, count);

      veffect->start_time = Gametime;

      // mprintf((0,"Creating Spew Effect (%d)\n",veffect->handle));

      return veffect->handle; // return the handle
    }

    if (count == MAX_SPEW_EFFECTS - 1)
      return -1; // we didn't find any open handles
  }
  return -1;
}

// Does a step in a spew system
void SpewEmitAll(void) {
  spewinfo *spew;
  int count;

  float size, speed, lifetime;

  for (count = 0; count < MAX_SPEW_EFFECTS; count++) {

    if (SpewEffects[count].inuse) {
      spew = &SpewEffects[count];

      spew->time_until_next_blob -= Frametime; // account for the current frame

      // no randomness if it's a real object!
      if (spew->real_obj)
        spew->random = 0;

      if (spew->time_until_next_blob >= 0) {
        // ok, since we won't be in the following while loop at all
        // this frame for the spew, we need to force an update on
        // it's gunpoint (IF it's a gunpoint) that way we can be sure
        // it has the correct position, in case we miss the obj->flags&OF_MOVED_THIS_FRAME
        if (spew->use_gunpoint) {
          object *obj = ObjGet(spew->gp.obj_handle);
          if (obj) {

            if ((obj->flags & OF_MOVED_THIS_FRAME) ||
                ((obj->flags & OF_ATTACHED) && ObjGet(obj->attach_ultimate_handle) &&
                 ObjGet(obj->attach_ultimate_handle)->movement_type == MT_PHYSICS &&
                 (ObjGet(obj->attach_ultimate_handle)->flags & OF_MOVED_THIS_FRAME)))
              spew->flags |= SF_FORCEUPDATE;

            // check to see if it is an animated polymodel, which means update everyframe
            if (obj->flags & OF_POLYGON_OBJECT) {
              if (obj->rtype.pobj_info.anim_end_frame != obj->rtype.pobj_info.anim_start_frame) {
                spew->flags |= SF_FORCEUPDATE;
              }
            }
          } else {
            // well, this spewer is dead, kill it
            SpewClearEvent(spew->handle, true);
          }
        }
      }

      if (spew->flags & SF_UPDATEEVERYFRAME)
        spew->flags |= SF_FORCEUPDATE;

      // turn this off, we will turn this on the first time we calc
      spew->flags &= ~SF_UPDATEDFORFRAME;

      // if time_until_next_blob is negative, it's time to spew
      int num_spewed = MAX_SPEWS_PER_FRAME;

      while (spew->time_until_next_blob < 0.0 && (num_spewed--)) {

        // do random computations
        if ((spew->random & SPEW_RAND_SIZE) != 0) {
          size = spew->size + (((RAND_MAX >> 1) - ps_rand()) / (float)RAND_MAX) * spew->size * 0.5f;
        } else
          size = spew->size;

        if ((spew->random & SPEW_RAND_SPEED) != 0) {
          speed = spew->speed + (((RAND_MAX >> 1) - ps_rand()) / (float)RAND_MAX) * spew->speed * 0.5f;
        } else
          speed = spew->speed;

        if ((spew->random & SPEW_RAND_LIFETIME) != 0) {
          lifetime = spew->lifetime + (((RAND_MAX >> 1) - ps_rand()) / (float)RAND_MAX) * spew->lifetime * 0.5f;
        } else
          lifetime = spew->lifetime;

        // move this code to within the following if statement and else
        if ((spew->random & SPEW_RAND_WIGGLE) != 0) {

        } else {
        }
        if ((spew->random & SPEW_RAND_SLIDE) != 0) {

        } else {
        }

        if (spew->use_gunpoint) {
          vector vel_vector;
          object *obj;

          obj = ObjGet(spew->gp.obj_handle);
          if (obj) {
            if (!(spew->flags & SF_UPDATEDFORFRAME)) {

              if ((obj->flags & OF_MOVED_THIS_FRAME) ||
                  ((obj->flags & OF_ATTACHED) && ObjGet(obj->attach_ultimate_handle) &&
                   ObjGet(obj->attach_ultimate_handle)->movement_type == MT_PHYSICS &&
                   (ObjGet(obj->attach_ultimate_handle)->flags & OF_MOVED_THIS_FRAME))) {
                // since the object moved this frame, force an update
                spew->flags |= SF_FORCEUPDATE;
              }

              // check to see if it is an animated polymodel, which means update everyframe
              if (obj->flags & OF_POLYGON_OBJECT) {
                if (obj->rtype.pobj_info.anim_end_frame != obj->rtype.pobj_info.anim_start_frame) {
                  spew->flags |= SF_FORCEUPDATE;
                }
              }

              if (spew->flags & SF_UPDATEEVERYFRAME)
                spew->flags |= SF_FORCEUPDATE;
            }

            // see if we need to recalc info
            if ((spew->flags & SF_FORCEUPDATE) && (!(spew->flags & SF_UPDATEDFORFRAME))) {
              // recalculate
              // Returns the position and the normal of a gun point
              WeaponCalcGun(&spew->gun_point, &spew->gp_normal, obj, spew->gp.gunpoint);
              spew->flags &= ~SF_FORCEUPDATE;
              spew->flags |= SF_UPDATEDFORFRAME;
            }

            // calc velocity vector
            vel_vector = spew->gp_normal * speed;

            // Create the viseffect
            VisEffectCreateControlled(VIS_FIREBALL, obj, (int)spew->effect_type, obj->roomnum, &spew->gun_point,
                                      lifetime, &vel_vector, spew->phys_info, size, spew->drag, spew->mass,
                                      spew->real_obj);
          } else {
            // object no longer exists
            SpewClearEvent(spew->handle, true);
          }
        } else {
          vector vel_vector;

          // mprintf((0,"Emitting Point based Spew %d\n",count));

          // calc velocity vector
          vel_vector = spew->pt.normal * speed;

          // Create the viseffect
          VisEffectCreateControlled(VIS_FIREBALL, NULL, (int)spew->effect_type, spew->pt.room_num, &spew->pt.origin,
                                    lifetime, &vel_vector, spew->phys_info, size, spew->drag, spew->mass,
                                    spew->real_obj);

        } // end else

        spew->time_until_next_blob += spew->time_int;
      } // end while

      if (num_spewed == 0) {
        mprintf((0, "Max spews per frame hit! Handle=%d Interval=%f Lifetime=%f Longevity=%f\n", spew->handle,
                 spew->time_int, spew->lifetime, spew->longevity));
        spew->time_until_next_blob = spew->time_int;
      }

      // see if this effect is done
      if (spew->longevity > 0.0f)
        if (Gametime - spew->start_time >= spew->longevity) {
          SpewClearEvent(spew->handle, true);
        }

    } // end if
  }   // end for
}

// Clears a Spew Event given a handle to it
void SpewClearEvent(int handle, bool force) {
  spewinfo *vis;

  int slot = GET_SLOT(handle);

  if ((slot < 0) || (slot > MAX_SPEW_EFFECTS))
    return;

  // mprintf((0,"Clearing spew event %d - %d....",handle,slot));

  vis = &SpewEffects[slot];

  if ((!force) && (handle != vis->handle))
    return;

  // mprintf((0,"handle OK\n"));

  vis->inuse = vis->random = vis->use_gunpoint = vis->real_obj = false;
  vis->pt.origin.x = vis->pt.origin.y = vis->pt.origin.z = 0.0f;
  vis->pt.room_num = 0;
  vis->pt.normal.x = vis->pt.normal.y = vis->pt.normal.z = 0.0f;
  vis->effect_type = MED_SMOKE_INDEX;
  vis->phys_info = PF_FIXED_VELOCITY;
  vis->drag = vis->mass = vis->time_int = vis->longevity = vis->lifetime = vis->size = vis->speed = 0.0f;
  vis->time_until_next_blob = vis->start_time = 0.0f;
  vis->handle = 0;
  vis->flags = 0;
}

bool SpewObjectNeedsEveryFrameUpdate(object *obj, int gun_num) {
  poly_model *pm;
  vector pnt;
  vector normal;
  int mn; // submodel number
  float normalized_time[MAX_SUBOBJECTS];

  if (!(obj->flags & OF_POLYGON_OBJECT)) {
    return false;
  }

  pm = &Poly_models[obj->rtype.pobj_info.model_num];

  if (pm->n_guns == 0) {
    return false;
  }

  SetNormalizedTimeObj(obj, normalized_time);
  SetModelAnglesAndPos(pm, normalized_time);

  if (gun_num < 0 || gun_num >= pm->n_guns) {
    return false;
  }

  pnt = pm->gun_slots[gun_num].pnt;
  normal = pm->gun_slots[gun_num].norm;
  mn = pm->gun_slots[gun_num].parent;

  // Instance up the tree for this gun
  while (mn != -1) {
    if (pm->submodel[mn].flags & (SOF_ROTATE | SOF_TURRET)) {
      return true;
    }

    mn = pm->submodel[mn].parent;
  }

  return false;
}
