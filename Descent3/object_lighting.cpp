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

 * $Logfile: /DescentIII/main/object_lighting.cpp $
 * $Revision: 64 $
 * $Date: 3/20/00 12:58p $
 * $Author: Matt $
 *
 * Code to deal with object lighting
 *
 * $Log: /DescentIII/main/object_lighting.cpp $
 *
 * 64    3/20/00 12:58p Matt
 * Merge of Duane's post-1.3 changes.
 * Lots of lightmap stuff.  Jason looked at this and said nothing jumps
 * out at him as being wrong.
 *
 * 63    10/26/99 6:33p Chris
 * Added the green flare
 *
 * 62    10/12/99 11:06a Jeff
 * negative lighting effect
 *
 * 61    7/20/99 1:00p Jason
 * added auto katmai support
 *
 * 60    4/28/99 11:28a Jason
 * made real viseffects not cast light
 *
 * 59    4/26/99 4:28a Jeff
 * reset cloak time to 0 on MakeObjectVisible() to prevent some weird
 * anomalies (like the thief stealing cloak)
 *
 * 58    4/21/99 11:05a Kevin
 * new ps_rand and ps_srand to replace rand & srand
 *
 * 57    4/12/99 6:15p Samir
 * Sound priorities pass 1
 *
 * 56    4/03/99 2:20p Jason
 * added better heat damage effect
 *
 * 55    3/26/99 3:26p Jeff
 * option to display hud message when cloaking
 *
 * 54    3/22/99 11:16a Jason
 * Katmai enhancements
 *
 * 53    3/05/99 12:03p Jason
 * fixed lighting with popup views
 *
 * 52    2/25/99 8:55p Jeff
 * Inventory supports level change persistant items.  Inventory supports
 * time-out objects.  Inventory Reset changed (takes a level of reset
 * now).  Quad lasers stay across level change (single player).  Guidebot
 * bug fixed (now back in ship on level start).  Quads time out when
 * spewed.  Invulnerability and cloak powerups no longer use game
 * event/callbacks, so they can be saved in game saves (moved to
 * MakePlayerInvulnerable and MakeObjectInvisible)
 *
 * 51    2/15/99 4:10p Jason
 * made non-vis objects kill all their attach sounds/objects
 *
 * 50    1/20/99 5:29p Jason
 * took out thruster glow for players
 *
 * 49    1/19/99 4:22p Matt
 * Added the ability for objects to have their own lighting info,
 * different from the default lighting for that type of object.
 *
 */

#include "object_lighting.h"
#include "object.h"
#include "lighting.h"
#include "objinfo.h"
#include "weapon.h"
#include "descent.h"
#include "game.h"
#include "polymodel.h"
#include "renderobject.h"
#include <stdlib.h>
#include "lightmap_info.h"
#include "fireball.h"
#include "player.h"
#include "mem.h"
#include "config.h"
#include "findintersection.h"
#include "psrand.h"

// How far the headlight casts light
#define HEADLIGHT_DISTANCE 150.0f
// Defines the headlight cone of light
#define HEADLIGHT_DOT .75f

// How big the fast headlight is
#define FAST_HEADLIGHT_SIZE 20.0f

// Defines the headlight cone of light
#define FAST_HEADLIGHT_DOT .80f

// returns true if this light point is visible
bool IsLightPointVisible(vector *pos, float size, object *obj) {
  if (obj != NULL) {
    if ((obj->flags & OF_RENDERED) || IsPointVisible(pos, size, NULL))
      return true;
  } else {
    if (IsPointVisible(pos, size, NULL))
      return true;
  }

  return false;
}

// Fills in the color of the passed in players thrusters.  For lighting
void FindPlayerThrusterColor(int slot, float *r, float *g, float *b) {
  ASSERT(slot >= 0 && slot < MAX_PLAYERS);
  object *obj = &Objects[Players[slot].objnum];
  int model_num = obj->rtype.pobj_info.model_num;

  *r = 0;
  *g = 0;
  *b = 0;

  if (obj->render_type != RT_POLYOBJ)
    return;

  poly_model *pm = &Poly_models[model_num];

  for (int i = 0; i < pm->n_models; i++) {
    if (pm->submodel[i].flags & SOF_GLOW) {
      *r = pm->submodel[i].glow_info->glow_r;
      *g = pm->submodel[i].glow_info->glow_g;
      *b = pm->submodel[i].glow_info->glow_b;

      return;
    }
  }

  Int3(); // No thruster glow?
}

ubyte EasterEgg = 0;
#define EASTER_EGG_TIMER (3600 * 4)

void ReflectRay(vector *, vector *, vector *);
// Casts light from an object onto the rooms or terrain
void DoObjectLight(object *obj) {
  light_info *li;
  static int first = 1, flare_id = -1, greenflare_id = -1;

  if (obj->flags & OF_DEAD)
    return;

  if (obj->render_type == RT_NONE)
    return;

  float negative_light = 1.0f;
  if (obj->effect_info && obj->effect_info->type_flags & EF_NEGATIVE_LIGHT)
    negative_light = -1.0f;

  // Temporary hack to make explosions light up
  if (obj->type == OBJ_FIREBALL) {
    if (!Detail_settings.Dynamic_lighting)
      return;

    if (obj->flags & OF_USES_LIFELEFT && Fireballs[obj->id].type == FT_EXPLOSION) {
      float scalar;

      scalar = (obj->lifetime - obj->lifeleft) / obj->lifetime;

      if (scalar > .5) {
        scalar -= .5;

        scalar = 1.0 - (scalar / .5);
      } else {
        scalar *= 2;
      }

      if (scalar > .05) {
        if (OBJECT_OUTSIDE(obj))
          ApplyLightingToTerrain(&obj->pos, CELLNUM(obj->roomnum), 40 * scalar, negative_light * 1.0f,
                                 negative_light * 0.5f, negative_light * 0.0f);
        else
          ApplyLightingToRooms(&obj->pos, obj->roomnum, 40 * scalar, negative_light * 1.0f, negative_light * 0.5f,
                               negative_light * 0.0f);
      }
    }
    return;
  }

  switch (obj->type) {
  case OBJ_PLAYER: {
    if ((Players[obj->id].flags) & PLAYER_FLAGS_HEADLIGHT) {

      if (Detail_settings.Fast_headlight_on) {
        // Do fast headlight
        fvi_info hit_info;
        fvi_query fq;

        // shoot a ray from the light position to the current vertex
        vector end_pos = obj->pos + (obj->orient.fvec * HEADLIGHT_DISTANCE * 2);
        fq.p0 = &obj->pos;
        fq.p1 = &end_pos;

        fq.startroom = obj->roomnum;

        fq.rad = 0.0f;
        fq.flags = FQ_CHECK_OBJS;
        fq.thisobjnum = -1;
        fq.ignore_obj_list = NULL;

        fvi_FindIntersection(&fq, &hit_info);

        hit_info.hit_pnt -= obj->orient.fvec / 4;

        float dist = vm_VectorDistanceQuick(&hit_info.hit_pnt, &obj->pos);

        // Now light up the hit area
        if (ROOMNUM_OUTSIDE(hit_info.hit_room))
          ApplyLightingToTerrain(&hit_info.hit_pnt, CELLNUM(hit_info.hit_room), FAST_HEADLIGHT_SIZE,
                                 negative_light * 1.0, negative_light * 1.0, negative_light * 1.0);
        else {
          ApplyLightingToRooms(&hit_info.hit_pnt, hit_info.hit_room, FAST_HEADLIGHT_SIZE, negative_light * 1.0,
                               negative_light * 1.0, negative_light * 1.0);
          // Do stupid easter egg trick
          face *fp = &Rooms[hit_info.hit_room].faces[hit_info.hit_face[0]];
          if (Gametime > EASTER_EGG_TIMER && GameTextures[fp->tmap].flags & (TF_PROCEDURAL | TF_WATER_PROCEDURAL)) {
            EasterEgg = 1;
          }
        }
      } else {
        // Do slow headlight

        if (OBJECT_OUTSIDE(obj))
          ApplyLightingToTerrain(&obj->pos, CELLNUM(obj->roomnum), HEADLIGHT_DISTANCE, negative_light * 1.0,
                                 negative_light * 1.0, negative_light * 1.0, &obj->orient.fvec, HEADLIGHT_DOT);
        else
          ApplyLightingToRooms(&obj->pos, obj->roomnum, HEADLIGHT_DISTANCE, negative_light * 1.0, negative_light * 1.0,
                               negative_light * 1.0, &obj->orient.fvec, HEADLIGHT_DOT);
      }
    }

    int slot = obj->id;

    /*// Do thruster glow
    if (Players[slot].thrust_mag && Detail_settings.Dynamic_lighting)
    {
            float lightsize=Players[slot].thrust_mag*30.0;
            lightsize+=Players[slot].afterburner_mag*40.0;

            float r,g,b;
            vector negz=-obj->orient.fvec;
            vector newpos=obj->pos-(obj->orient.fvec*(obj->size*.7));

            if (IsLightPointVisible (&newpos,lightsize,NULL))
            {
                    FindPlayerThrusterColor (slot,&r,&g,&b);

                    ASSERT (r>=0 && r<=1.0);
                    ASSERT (g>=0 && g<=1.0);
                    ASSERT (b>=0 && b<=1.0);

                    if (r!=0 || g!=0 || b!=0)
                    {
                            if	(OBJECT_OUTSIDE(obj))
                                    ApplyLightingToTerrain (&newpos,CELLNUM(obj->roomnum),lightsize,r,g,b,&negz,0);
                            else
                                    ApplyLightingToRooms (&newpos,obj->roomnum,lightsize,r,g,b,&negz,0);
                    }
            }
    }*/

    // If the player is glowing
    if (Players[slot].light_dist > .5 && Detail_settings.Dynamic_lighting) {
      if (IsLightPointVisible(&obj->pos, Players[slot].light_dist, obj)) {
        if (OBJECT_OUTSIDE(obj))
          ApplyLightingToTerrain(&obj->pos, CELLNUM(obj->roomnum), Players[slot].light_dist,
                                 Players[slot].r * negative_light, Players[slot].g * negative_light,
                                 Players[slot].b * negative_light);
        else
          ApplyLightingToRooms(&obj->pos, obj->roomnum, Players[slot].light_dist, Players[slot].r * negative_light,
                               Players[slot].g * negative_light, Players[slot].b * negative_light);
      }
    }

    // Do stupid ball glow
    if (Players[slot].num_balls > 0 && Detail_settings.Dynamic_lighting) {
      vector ballpos;
      for (int i = 0; i < Players[slot].num_balls; i++) {
        float light_size = 25;
        PlayerGetBallPosition(&ballpos, slot, i);

        if (!IsLightPointVisible(&ballpos, light_size, obj))
          return;

        if (OBJECT_OUTSIDE(obj))
          ApplyLightingToTerrain(&ballpos, CELLNUM(obj->roomnum), light_size, Players[slot].ball_r[i] * negative_light,
                                 Players[slot].ball_g[i] * negative_light, Players[slot].ball_b[i] * negative_light);
        else
          ApplyLightingToRooms(&ballpos, obj->roomnum, light_size, Players[slot].ball_r[i] * negative_light,
                               Players[slot].ball_g[i] * negative_light, Players[slot].ball_b[i] * negative_light);
      }
    }

    return;
  }

  case OBJ_WEAPON: {
    weapon *w = &Weapons[obj->id];
    li = &w->lighting_info;

    // Check to see if we should have dynamic light
    if (first) {
      flare_id = FindWeaponName("Yellow flare");
      greenflare_id = FindWeaponName("GreenFlare");
      first = 0;
    }

    if (!Detail_settings.Dynamic_lighting) {
      if (obj->id != flare_id && obj->id != greenflare_id)
        return;
    }
    if (obj->ctype.laser_info.casts_light == false)
      return;

    break;
  }
  case OBJ_POWERUP:
  case OBJ_ROBOT:
  case OBJ_CLUTTER:
  case OBJ_BUILDING: {
    li = ObjGetLightInfo(obj);
    if (!Detail_settings.Dynamic_lighting && (obj->type == OBJ_ROBOT || obj->type == OBJ_POWERUP))
      return;
    break;
  }
  default:
    return;
  }

  float scalar = 1.0;
  float red, green, blue;
  float light_distance = li->light_distance;
  vector *direction = NULL;

  // Make katmai lights bigger
  if (Katmai)
    light_distance *= 1.5;

  if (light_distance > 0) {
    if (!IsLightPointVisible(&obj->pos, light_distance, obj))
      return;

    if (li->flags & OLF_DIRECTIONAL)
      direction = &obj->orient.fvec;

    if (li->flags & OLF_FLICKER_SLIGHTLY) {
      int factor = li->flicker_distance;

      if (factor == 0) {
        mprintf((0, "You have a flicker_slightly light that has a flicker factor of zero!\n"));
        return;
      }

      light_distance += ((ps_rand() % factor) - (factor / 2));
      if (light_distance < 0)
        return;
    }

    red = li->red_light1;
    green = li->green_light1;
    blue = li->blue_light1;

    if ((li->flags & OLF_PULSE) && li->time_interval > 0) {
      float ptime = li->time_interval;

      int int_time = (int)(Gametime / (ptime * 2));
      float left_time = Gametime - (int_time * ptime * 2);
      float norm_time = left_time / (ptime);

      if (norm_time > 1)
        scalar = 1 - (norm_time - 1.0);
      else
        scalar = norm_time;

      scalar = li->flicker_distance + (scalar * (1.0 - li->flicker_distance));
    }

    if (li->flags & OLF_PULSE_TO_SECOND && li->time_interval > 0) {
      float ptime = li->time_interval;

      int int_time = (int)(Gametime / (ptime * 2));
      float left_time = Gametime - (int_time * ptime * 2);
      float norm_time = left_time / (ptime);

      if (norm_time > 1) {
        norm_time -= 1.0;

        red = ((1.0 - norm_time) * li->red_light2) + (li->red_light1 * norm_time);
        green = ((1.0 - norm_time) * li->green_light2) + (li->green_light1 * norm_time);
        blue = ((1.0 - norm_time) * li->blue_light2) + (li->blue_light1 * norm_time);
      } else {
        red = ((1.0 - norm_time) * li->red_light1) + (li->red_light2 * norm_time);
        green = ((1.0 - norm_time) * li->green_light1) + (li->green_light2 * norm_time);
        blue = ((1.0 - norm_time) * li->blue_light1) + (li->blue_light2 * norm_time);
      }
    }

    if (scalar < 0.0f)
      scalar = 0.0f;

    // If this is object is flickering, just compare it with a random value
    if (li->flags & OLF_FLICKERING) {
      if (ps_rand() % 2 == 1)
        return;
    }

    // If this object is based on timebits, then return if the corresponding bit
    // is zero for this gametime
    if (li->flags & OLF_TIMEBITS) {
      int int_time = (Gametime / li->time_interval);
      float left_time = (Gametime - (int_time * li->time_interval));
      float slice_time = li->time_interval / 8;

      int timebit_num = left_time / slice_time;

      if (!(li->timebits & (1 << timebit_num)))
        return;
    }

    if (OBJECT_OUTSIDE(obj))
      ApplyLightingToTerrain(&obj->pos, CELLNUM(obj->roomnum), light_distance * scalar, red * negative_light,
                             green * negative_light, blue * negative_light, direction, li->directional_dot);
    else
      ApplyLightingToRooms(&obj->pos, obj->roomnum, light_distance * scalar, red * negative_light,
                           green * negative_light, blue * negative_light, direction, li->directional_dot);
  }
}

// Frees all the memory associated with this objects lightmap
void ClearObjectLightmaps(object *obj) {
  int Mnum, Fnum;

  if (obj->lm_object.used == 1) {
    obj->lm_object.used = 0;
    poly_model *pm = &Poly_models[obj->rtype.pobj_info.model_num];
    ASSERT(pm->n_models < MAX_SUBOBJECTS);

    mprintf((1, "CLEAR %d %s", obj->handle, obj->name));

    int faceCount = 0;
    for (Mnum = 0; Mnum < pm->n_models; Mnum++) {
      if (!IsNonRenderableSubmodel(pm, Mnum)) {
        mprintf((1, " %X\n", obj->lm_object.lightmap_faces[Mnum][0].u2));
        mem_free(obj->lm_object.lightmap_faces[Mnum][0].u2);
        break;
      }
    }
    for (Mnum = 0; Mnum < pm->n_models; Mnum++) {
      if (IsNonRenderableSubmodel(pm, Mnum))
        continue;

      for (Fnum = 0; Fnum < pm->submodel[Mnum].num_faces; Fnum++) {
        lightmap_object_face *lof = &obj->lm_object.lightmap_faces[Mnum][Fnum];

        if (lof->lmi_handle != BAD_LMI_INDEX) {
          FreeLightmapInfo(lof->lmi_handle);
          lof->lmi_handle = BAD_LMI_INDEX;
        }
        lof->num_verts = 0;
      }

      if (obj->lm_object.num_faces[Mnum]) {
        mem_free(obj->lm_object.lightmap_faces[Mnum]);
        obj->lm_object.lightmap_faces[Mnum] = NULL;
      }
      obj->lm_object.num_faces[Mnum] = 0;
    }
  }
}

// Frees all the memory associated with lightmap objects
void ClearAllObjectLightmaps(int terrain) {
  int i;

  for (i = 0; i <= Highest_object_index; i++) {
    object *obj = &Objects[i];

    if ((OBJECT_OUTSIDE(obj) != 0) != (terrain != 0))
      continue;

    if (obj->lm_object.used == 1) {
      ClearObjectLightmaps(obj);
    }
  }
}

// Sets up the memory to be used by an object for lightmaps
void SetupObjectLightmapMemory(object *obj) {
  int Mnum, Fnum;
  lightmap_object_face *lof;

  ASSERT(obj->lm_object.used == 0);
  obj->lm_object.used = 1;

  poly_model *pm = &Poly_models[obj->rtype.pobj_info.model_num];
  ASSERT(pm->new_style);

  obj->lm_object.num_models = pm->n_models;

  ASSERT(pm->n_models < MAX_SUBOBJECTS);
  int uv2size = 0;

  mprintf((1, "SETUP %d %s", obj->handle, obj->name));
  for (Mnum = 0; Mnum < pm->n_models; Mnum++) {
    if (IsNonRenderableSubmodel(pm, Mnum)) {
      obj->lm_object.num_faces[Mnum] = 0;
      obj->lm_object.lightmap_faces[Mnum] = NULL;
      continue;
    }

    obj->lm_object.num_faces[Mnum] = pm->submodel[Mnum].num_faces;
    obj->lm_object.lightmap_faces[Mnum] =
        (lightmap_object_face *)mem_malloc(obj->lm_object.num_faces[Mnum] * sizeof(lightmap_object_face));
    ASSERT(obj->lm_object.lightmap_faces[Mnum]);

    for (Fnum = 0; Fnum < pm->submodel[Mnum].num_faces; Fnum++) {
      lof = &obj->lm_object.lightmap_faces[Mnum][Fnum];

      lof->num_verts = pm->submodel[Mnum].faces[Fnum].nverts;
      lof->lmi_handle = BAD_LMI_INDEX;

      uv2size += lof->num_verts;
    }
  }
  if (uv2size) {
    float *uvblock = (float *)mem_malloc(2 * uv2size * sizeof(float));
    mprintf((1, " %X %d\n", uvblock, uv2size));

    for (Mnum = 0; Mnum < pm->n_models; Mnum++) {
      if (!IsNonRenderableSubmodel(pm, Mnum)) {
        for (Fnum = 0; Fnum < pm->submodel[Mnum].num_faces; Fnum++) {
          obj->lm_object.lightmap_faces[Mnum][Fnum].u2 = uvblock;
          uvblock += pm->submodel[Mnum].faces[Fnum].nverts;
          obj->lm_object.lightmap_faces[Mnum][Fnum].v2 = uvblock;
          uvblock += pm->submodel[Mnum].faces[Fnum].nverts;
        }
      }
    }
  }
}

// SHOULDN'T THE FOLLOWING TWO FUNCTIONS REALLY BE IN OBJECT.CPP?
#include "hlsoundlib.h"
#include "soundload.h"
#include "hud.h"
#include "stringtable.h"
//	makes the an object cloaked
void MakeObjectInvisible(object *obj, float time, float fade_time, bool no_hud_message) {
  if (obj->effect_info) {
    obj->effect_info->cloak_time = time;
    obj->effect_info->type_flags |= EF_FADING_OUT;
    obj->effect_info->fade_time = fade_time;
    obj->effect_info->fade_max_time = fade_time;
  }

  if (obj->type == OBJ_PLAYER && !no_hud_message) {
    static int cloak_sound_on_id = -2;
    if (cloak_sound_on_id == -2)
      cloak_sound_on_id = FindSoundName("Cloak on");

    if (cloak_sound_on_id != -1)
      Sound_system.Play3dSound(cloak_sound_on_id, SND_PRIORITY_HIGHEST, obj, MAX_GAME_VOLUME / 2);

    if (obj->id == Player_num) {
      AddHUDMessage(TXT_CLOAKON);
    }

    if (obj->effect_info)
      obj->effect_info->type_flags |= EF_CLOAK_WITH_MSG;
  }
}

//	makes an object visbile
void MakeObjectVisible(object *obj) {
  if (obj->effect_info) {
    //	Play sound maybe.
    obj->effect_info->type_flags &= ~EF_CLOAKED;
    obj->effect_info->type_flags |= EF_FADING_IN;
    obj->effect_info->fade_time = obj->effect_info->fade_max_time;
    obj->effect_info->cloak_time = 0;
  }

  if (obj->type == OBJ_PLAYER && obj->effect_info && obj->effect_info->type_flags & EF_CLOAK_WITH_MSG) {
    static int cloak_sound_off_id = -2;
    if (cloak_sound_off_id == -2)
      cloak_sound_off_id = FindSoundName("Cloak off");

    if (cloak_sound_off_id != -1)
      Sound_system.Play3dSound(cloak_sound_off_id, SND_PRIORITY_HIGHEST, obj, MAX_GAME_VOLUME / 2);

    if (obj->id == Player_num) {
      AddHUDMessage(TXT_MSG_CLOAKOFF);
    }
  }
}

// Sets an object to have local lighting info
void ObjSetLocalLighting(object *objp) {
  // If this object doesn't have its own lighting info, create it
  if (objp->lighting_info == NULL) {

    // make sure this is a generic object
    ASSERT((objp->type == OBJ_POWERUP) || (objp->type == OBJ_ROBOT) || (objp->type == OBJ_CLUTTER) ||
           (objp->type == OBJ_BUILDING));

    // allocate a light info for this object
    objp->lighting_info = (light_info *)mem_malloc(sizeof(*objp->lighting_info));

    // copy the lighting info from the type for this object
    *objp->lighting_info = Object_info[objp->id].lighting_info;
  }
}

// Returns a pointer to the lighting info for the specified object
light_info *ObjGetLightInfo(object *objp) {
  if (objp->lighting_info)
    return objp->lighting_info;

  if ((objp->type == OBJ_POWERUP) || (objp->type == OBJ_ROBOT) || (objp->type == OBJ_CLUTTER) ||
      (objp->type == OBJ_BUILDING))
    return &Object_info[objp->id].lighting_info;

  if (objp->type == OBJ_WEAPON)
    return &Weapons[objp->id].lighting_info;

  Int3();

  return NULL;
}
