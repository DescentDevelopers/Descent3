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

 * $Logfile: /DescentIII/Main/editor/rad_cast.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Ray casting for radiosity, I think.
 *
 * $Log: not supported by cvs2svn $
 *
 * 87    5/20/99 11:50a Jason
 * made super detail not do volume lights super detail
 *
 * 86    4/18/99 3:37p 3dsmax
 * tweaks
 *
 * 85    4/18/99 5:42a Chris
 * Added the FQ_IGNORE_RENDER_THROUGH_PORTALS flag
 *
 * 84    4/16/99 5:50p Jason
 * tweaked smooth specular stuff some more
 *
 * 83    4/15/99 2:56p Jason
 * fixed stupid lighting bug
 *
 * 82    4/05/99 10:36a 3dsmax
 * took out mprintfs
 *
 * 81    4/02/99 5:34p Jason
 * added verbose mprintfs for super detail
 *
 * 80    3/23/99 4:09p Jason
 * upped smooth specular sensitivity
 *
 * 79    3/11/99 1:07p Jason
 * more fixes for smooth specular lighting
 *
 * 78    3/10/99 7:12p Jason
 * added smooth specular shading for curved surfaces
 *
 * 77    3/10/99 2:09p Jason
 * made better specular lighting
 *
 * 76    2/24/99 5:14p 3dsmax
 *
 *
 * 75    2/09/99 9:59a Chris
 * Massive BOA update  :)  Terrain happy now.  Vis happy now.  Sound happy
 * now.
 *
 * 74    1/27/99 12:19p Dan
 * another clipping fix
 *
 * 73    1/27/99 12:16p Dan
 * finally fixed dumb clipping problem
 *
 * 72    1/27/99 11:06a Jason
 * fixed satellite clipping problem
 *
 * 71    1/24/99 1:17p Jason
 * fixed satellite casting problem
 *
 * 70    12/21/98 11:52a Jason
 * changes for specular lighting
 *
 * 69    11/19/98 12:22p Jason
 * optimizations
 *
 * 68    10/28/98 12:18p Jason
 * sped up volume lighting a bit
 *
 * 67    10/05/98 6:53p Jason
 * took out some unused variables
 *
 * 66    10/02/98 11:55a Jason
 * more fixes for satellite clipping
 *
 * 65    9/24/98 11:27a Jason
 * fixed satellite clipping
 *
 * 64    8/21/98 12:29p Jason
 * upped specular lighting threshold
 *
 * 63    8/10/98 11:25a Jason
 * added better specular lighting
 *
 * 62    6/30/98 3:12p Jason
 * sped up super detail by not doing surper-detail from surfaces that
 * aren't lightsources
 *
 * 61    6/03/98 12:22p Jason
 * specular lighting optimizations
 *
 * 60    6/02/98 6:03p Jason
 * added specular lightmaps
 *
 * 59    6/02/98 11:01a Jason
 * Post E3 Checkin
 *
 * 58    5/26/98 10:11p Sean
 * don't do specular lighting for single rooms
 *
 * 57    5/25/98 3:46p Jason
 * added better light glows
 *
 * 56    5/22/98 3:28p Jason
 * added specular lighting
 *
 * 55    5/21/98 11:51a Jason
 * made volume lights work with super detail
 *
 * 54    5/20/98 1:37p Jason
 * upped facelimit on volume raycasting
 *
 * 53    5/19/98 2:36p Jason
 * improved volume light casting
 *
 * 52    5/15/98 5:41p Jason
 * implemented volume lighting system
 *
 * 51    5/01/98 4:32p Jason
 * temp fix for lighting
 *
 * 50    4/19/98 5:23p Jason
 * made terrain satellites only cast light on rooms that touch terrain
 *
 * 49    4/08/98 3:26p Jason
 * made terrain satellites automatically cast light into rooms
 *
 * 48    3/17/98 5:15p Jason
 * changed normal casting length
 *
 * 47    3/16/98 5:47p Jason
 * added FQ_NO_RELINK
 *
 * 46    3/11/98 6:14p Jason
 * better lighting, again
 *
 * 45    3/11/98 5:13p Jason
 * always draw an object if any point is behind you
 *
 * 44    3/06/98 3:54p Jason
 * fixed bug with satellite lighting indoors
 *
 * 43    3/06/98 3:23p Jason
 * added lighting from satellites to indoor rooms
 *
 * 42    2/27/98 5:58p Jason
 * fixed terrain lighting problem
 *
 * 41    2/25/98 1:02p Jason
 * offset lights and dests by 1/8 of a normal
 *
 * 40    2/23/98 6:57p Jason
 * fixed bug caused by my last rev
 *
 * 39    2/23/98 6:50p Jason
 * changes to help facilitate fast lighting with shadow volumes
 *
 *
 * 38    2/22/98 3:05p Jason
 * sped up raycasting by switching the directions rays get shot.  This
 * allows us to ignore backfaces
 *
 * 37    2/19/98 5:44p Jason
 * made super detail work correctly
 *
 * 36    2/09/98 3:45p Jason
 * changed normal distance offset to be friendly with BSP tree stuff
 *
 * 35    2/06/98 12:35p Jason
 * fixed problem with external rooms
 *
 * 34    2/05/98 4:20p Matt
 * Fixed terrain lighting broken in roomnum/cellnum change
 *
 * 33    2/04/98 6:23p Matt
 * Changed object room number to indicate a terrain cell via a flag.  Got
 * rid of the object flag which used to indicate terrain.
 *
 */

#include <stdlib.h>
#include "mono.h"
#include "radiosity.h"
#include "rad_cast.h"
#include "terrain.h"
#include "room.h"
#include "editor_lighting.h"
#include "findintersection.h"
#include "special_face.h"
#include "bsp.h"
#include "boa.h"

// A ray must contribute at least this amount to be accepted (currently .5%)
#define DEFAULT_IGNORE_LIMIT .005f

float Ignore_limit = DEFAULT_IGNORE_LIMIT;
extern rad_element *rad_MaxElement;

float Current_max_specular_strength;
spectra Current_max_specular_color;

// Returns 1 if a src vector can hit dest vector for a volume point
int ShootRayToVolumePoint(vector *src, vector *dest, int start_room) {
  fvi_info hit_info;
  fvi_query fq;

  if (UseBSP) {
    int fate = BSPRayOccluded(src, dest, MineBSP.root);
    if (!fate)
      return 1;
    return 0;
  }

  // shoot a ray from the light position to the current vertex
  fq.p0 = src;
  fq.p1 = dest;

  fq.startroom = start_room;

  fq.rad = 0.0f;
  fq.flags = FQ_CHECK_OBJS | FQ_IGNORE_NON_LIGHTMAP_OBJECTS | FQ_NO_RELINK | FQ_IGNORE_RENDER_THROUGH_PORTALS;
  fq.thisobjnum = -1;
  fq.ignore_obj_list = NULL;

  int fate = fvi_FindIntersection(&fq, &hit_info);

  float dist = vm_VectorDistance(&hit_info.hit_pnt, dest);
  if (dist > .1)
    return 0;
  return 1;
}

void ClipSatelliteToTerrain(vector *answer, vector *src_vec, vector *dest_vec) {
  vector cur_vec = *src_vec;
  scalar terrain_limit = 256 * TERRAIN_SIZE;
  scalar mag, diff;
  vector ray;

  // Check ceiling
  if (cur_vec.y() > ((scalar)MAX_TERRAIN_HEIGHT * 30)) {
    ray = cur_vec - *dest_vec;

    mag = vm_GetMagnitude(&ray);
    ray /= mag;

    diff = (((scalar)MAX_TERRAIN_HEIGHT * 30) - dest_vec->y()) / ray.y();

    cur_vec = (*dest_vec + (ray * diff)) - (ray / 4);
  }

  // Check right edge
  if (cur_vec.x() > terrain_limit) {
    ray = cur_vec - *dest_vec;

    mag = vm_GetMagnitude(&ray);
    ray /= mag;

    diff = (terrain_limit - dest_vec->x()) / ray.x();

    cur_vec = (*dest_vec + (ray * diff)) - (ray / 4);
  }

  // Check left edge
  if (cur_vec.x() < 0) {
    ray = cur_vec - *dest_vec;

    mag = vm_GetMagnitude(&ray);
    ray /= mag;

    diff = (-dest_vec->x()) / ray.x();

    cur_vec = (*dest_vec + (ray * diff)) - (ray / 4);
  }

  // Check top edge
  if (cur_vec.z() > terrain_limit) {
    ray = cur_vec - *dest_vec;

    mag = vm_GetMagnitude(&ray);
    ray /= mag;

    diff = (terrain_limit - dest_vec->z()) / ray.z();

    cur_vec = (*dest_vec + (ray * diff)) - (ray / 4);
  }

  // Check bottom edge
  if (cur_vec.z() < 0) {
    ray = cur_vec - *dest_vec;

    mag = vm_GetMagnitude(&ray);
    ray /= mag;

    diff = (-dest_vec->z()) / ray.z();

    cur_vec = (*dest_vec + (ray * diff)) - (ray / 4);
  }

  if (cur_vec.x() > 4095)
    cur_vec.x() = 4095;
  if (cur_vec.z() > 4095)
    cur_vec.z() = 4095;

  *answer = cur_vec;
}

// Returns 1 if a src vector can hit dest vector unobstructed, else 0
int ShootRayFromPoint(vector *src, vector *dest, rad_surface *src_surf, rad_surface *dest_surf) {
  scalar dist;
  bool do_backface = 0;
  fvi_info hit_info;
  fvi_query fq;
  vector temp_src = *src;
  vector temp_dest = *dest;
  int from_satellite = 0;

  // Trivially reject all rooms
  if (dest_surf->surface_type == ST_ROOM || dest_surf->surface_type == ST_ROOM_OBJECT) {
    if (src_surf->surface_type == ST_ROOM || src_surf->surface_type == ST_ROOM_OBJECT) {
      if (!BOA_IsVisible(dest_surf->roomnum, src_surf->roomnum))
        return 0;
    }
  }

  if (UseBSP) {
    if (dest_surf->surface_type == ST_ROOM || dest_surf->surface_type == ST_ROOM_OBJECT) {
      if (src_surf->surface_type == ST_ROOM || src_surf->surface_type == ST_ROOM_OBJECT) {
        int fate = BSPRayOccluded(src, dest, MineBSP.root);
        if (!fate)
          return 1;
        return 0;
      }
    }
  }

  // If this ray is too high, clip it to the ceiling
  if (src_surf->surface_type == ST_SATELLITE) {
    if (1 || dest->y() >= (scalar)MAX_TERRAIN_HEIGHT) {
      from_satellite = 1;
      // swap the src/dest the variables because we now want to shoot from the ground to the satellite
      rad_surface *temp_surf;
      vector *temp_vec;

      temp_surf = src_surf;
      src_surf = dest_surf;
      dest_surf = temp_surf;

      temp_vec = src;
      src = dest;
      dest = temp_vec;

      temp_dest = *dest;
      temp_src = *src;

      ClipSatelliteToTerrain(&temp_src, src, &temp_dest);
    } else {
      ClipSatelliteToTerrain(&temp_src, src, &temp_dest);
      int src_cell = GetTerrainCellFromPos(&temp_src);

      if (src_cell < 0) {
        src_cell = 0;
        Int3(); // Get Jason, satellite clipped off terrain?
      }

      src_surf->roomnum = MAKE_ROOMNUM(src_cell);
    }
  }

  // shoot a ray from the light position to the current vertex
  fq.p0 = &temp_src;
  fq.p1 = &temp_dest;

  if (src_surf->surface_type == ST_EXTERNAL_ROOM)
    fq.startroom = GetTerrainRoomFromPos(src);
  else
    fq.startroom = src_surf->roomnum;

  fq.rad = 0.0f;
  fq.flags = FQ_CHECK_OBJS | FQ_IGNORE_NON_LIGHTMAP_OBJECTS | FQ_NO_RELINK | FQ_IGNORE_RENDER_THROUGH_PORTALS;
  fq.thisobjnum = -1;
  fq.ignore_obj_list = NULL;

  int fate = fvi_FindIntersection(&fq, &hit_info);

  if (from_satellite) {
    if (fate == HIT_NONE || fate == HIT_OUT_OF_TERRAIN_BOUNDS)
      return 1;
    else
      return 0;
  }

  dist = vm_VectorDistance(&hit_info.hit_pnt, &temp_dest);
  if (dist > .1) {
    // mprintf(0,"Didn't hit!\n");
    return 0;
  }

  return 1;
}

int Rays_ignored = 0;

float GetMaxColor(spectra *sp);

float GetFormFactorForElementAndSatellite(rad_surface *dest_surf, rad_element *dest_element, vector *src_center) {
  vector dest_center;
  vector light_center = *src_center;
  scalar form_factor = 0.0;

  if (Ignore_satellites)
    return 0;

  // Bash src center to first element vertex (if sun or moon)
  light_center = rad_MaxSurface->elements[0].verts[0];

  for (int j = 0; j < dest_element->num_verts; j++) {
    dest_center = dest_element->verts[j];
    dest_center += (dest_surf->normal / 32.0);
    scalar temp_factor = 0;

    int hit = 0;
    vector ray = light_center - dest_center;

    if (dest_surf->surface_type != ST_TERRAIN) {
      if ((vm_DotProduct(&ray, &dest_surf->normal)) < 0)
        continue;
    }

    // If this surface is a terrain surface, use the terrain speedup table
    if (dest_surf->surface_type == ST_TERRAIN) {
      ASSERT(ROOMNUM_OUTSIDE(dest_surf->roomnum));
      int cellnum = CELLNUM(dest_surf->roomnum);
      if (dest_surf->facenum == 0) {
        if (j == 1)
          cellnum += TERRAIN_WIDTH;
        else if (j == 2)
          cellnum += (TERRAIN_WIDTH + 1);
      } else {
        if (j == 1)
          cellnum += (TERRAIN_WIDTH + 1);
        else if (j == 2)
          cellnum++;
      }

      hit = TerrainLightSpeedup[rad_MaxSurface->roomnum][cellnum];
    } else
      hit = ShootRayFromPoint(&light_center, &dest_center, rad_MaxSurface, dest_surf);

    if (hit) {
      float ray_length = vm_GetMagnitude(&ray);
      vector dest_norm_ray = ray / ray_length;
      vector dest_normal = dest_surf->normal;
      float ff;

      ff = vm_DotProduct(&dest_normal, &dest_norm_ray);

      if (ff > 0)
        temp_factor += ff;
    }

    form_factor += temp_factor;
  }

  form_factor /= dest_element->num_verts;

  if (form_factor > 1)
    mprintf(0, "form factor >1!\n");

  return form_factor;
}

// Calculates the percentage of specular light that is coming from the
// currently shooting patch
void CheckToUpdateSpecularFace(rad_surface *dest_surf, spectra *color, vector *src_center) {
  vector light_center;

  float total_strength = (color->r * .3) + (color->g * .33) + (color->b * .33);
  float threshold = .7f;

  if (dest_surf->surface_type != ST_ROOM)
    return;

  if (Rooms[dest_surf->roomnum].faces[dest_surf->facenum].special_handle == BAD_SPECIAL_FACE_INDEX)
    return;

  if (src_center == NULL)
    GetCenterOfSurface(rad_MaxSurface, &light_center);
  else
    light_center = *src_center;

  room *rp = &Rooms[dest_surf->roomnum];
  face *fp = &rp->faces[dest_surf->facenum];

  if (GameTextures[fp->tmap].flags & TF_SMOOTH_SPECULAR)
    threshold = .01f;
  else {
    if (!(rad_MaxSurface->flags & SF_LIGHTSOURCE))
      return;
  }

  int i, t;

  for (i = 0; i < 4; i++) {
    if (total_strength > threshold &&
        total_strength > Room_strongest_value[dest_surf->roomnum][i][dest_surf->facenum]) {
      float scalar = (total_strength / 50.0) + .5;
      if (scalar > 1)
        scalar = 1.0;

      int special_index = Rooms[dest_surf->roomnum].faces[dest_surf->facenum].special_handle;

      // Move the others down
      for (t = 2; t >= i; t--) {
        Room_strongest_value[dest_surf->roomnum][t + 1][dest_surf->facenum] =
            Room_strongest_value[dest_surf->roomnum][t][dest_surf->facenum];
        SpecialFaces[special_index].spec_instance[t + 1].bright_color =
            SpecialFaces[special_index].spec_instance[t].bright_color;
        SpecialFaces[special_index].spec_instance[t + 1].bright_center =
            SpecialFaces[special_index].spec_instance[t].bright_center;
      }

      // Update our arrays with strongest light and src patch center
      Room_strongest_value[dest_surf->roomnum][i][dest_surf->facenum] = total_strength;

      float rmax = GetMaxColor(color);

      float r = color->r;
      float g = color->g;
      float b = color->b;

      // Normalize colors
      if (rmax > 1) {
        r /= rmax;
        g /= rmax;
        b /= rmax;
      }

      r *= (255.0 * scalar);
      g *= (255.0 * scalar);
      b *= (255.0 * scalar);

      SpecialFaces[special_index].spec_instance[i].bright_color = GR_RGB16((int)r, (int)g, (int)b);
      SpecialFaces[special_index].spec_instance[i].bright_center = light_center;

      return;
    }
  }
}

float GetFormFactorForElement(rad_surface *dest_surf, rad_element *dest_element, vector *src_center) {
  int touched = 0;
  vector dest_center;
  int i, limit;
  float ray_area;
  int ignored = 0;
  vector light_center = *src_center;
  float form_factor = 0.0;
  int multiple_shoots = 0;

  ray_area = rad_MaxSurface->area;

  float rmax = GetMaxColor(&rad_MaxSurface->exitance);

  multiple_shoots = 0;
  limit = 1;

  for (i = 0; i < limit; i++) {
    float temp_factor = 0.0;

    if (multiple_shoots && (rad_MaxSurface->elements[i].flags & EF_IGNORE))
      continue;

    if (multiple_shoots) {
      GetCenterOfElement(&rad_MaxSurface->elements[i], &light_center);
      light_center += (rad_MaxSurface->normal / 16.0);
      ray_area = rad_MaxSurface->elements[i].area;
    } else
      ray_area = rad_MaxSurface->area;

    GetCenterOfElement(dest_element, &dest_center);
    dest_center += (dest_surf->normal / 16.0);

    int hit = 0;
    vector ray = light_center - dest_center;
    vector revray = dest_center - light_center;

    if ((vm_DotProduct(&ray, &dest_surf->normal)) < 0)
      continue;
    if ((vm_DotProduct(&revray, &rad_MaxSurface->normal)) < 0)
      continue;

    // Check to see if this ray even matters
    float ray_length = vm_GetMagnitudeFast(&ray);
    vector dest_norm_ray = ray / ray_length;
    vector src_norm_ray = -dest_norm_ray;
    vector dest_normal = dest_surf->normal;
    float ff;
    vector src_normal = rad_MaxSurface->normal;
    ff = (vm_DotProduct(&src_norm_ray, &src_normal) * vm_DotProduct(&dest_normal, &dest_norm_ray)) /
         ((3.14 * ray_length * ray_length) + ray_area);
    ff *= ray_area;
    ff *= rmax;

    // If shooting from elements, scale the test up
    if (multiple_shoots)
      ff *= rad_MaxSurface->area / rad_MaxSurface->elements[i].area;

    if (ff < Ignore_limit) {
      ignored++;
      continue;
    }

    hit = ShootRayFromPoint(&light_center, &dest_center, rad_MaxSurface, dest_surf);

    if (hit) {
      // We got a hit...figure out how much light contribution this light has

      ray_length = vm_GetMagnitude(&ray);
      dest_norm_ray = ray / ray_length;
      src_norm_ray = -dest_norm_ray;
      dest_normal = dest_surf->normal;
      src_normal = rad_MaxSurface->normal;

      ff = (vm_DotProduct(&src_norm_ray, &src_normal) * vm_DotProduct(&dest_normal, &dest_norm_ray)) /
           ((3.14 * ray_length * ray_length) + ray_area);

      if (ff > 1)
        mprintf(0, "ff >1!\n");

      if (ff > 0)
        temp_factor += ff;

      // Update specularity if needed
      if (rad_MaxSurface->surface_type == ST_ROOM && Calculate_specular_lighting) {
        spectra color;
        float scalar = ff;

        scalar *= ray_area;
        scalar *= 5;

        color.r = rad_MaxSurface->exitance.r * scalar;
        color.g = rad_MaxSurface->exitance.g * scalar;
        color.b = rad_MaxSurface->exitance.b * scalar;

        float total_strength = (color.r * .33) + (color.g * .33) + (color.b * .33);
        if (total_strength > Current_max_specular_strength) {
          Current_max_specular_strength = total_strength;
          Current_max_specular_color = color;
        }
      }
    }

    temp_factor *= ray_area;

    // temp_factor/=dest_element->num_verts;

    // if (multiple_shoots)
    //	temp_factor*=(rad_MaxSurface->elements[i].area/rad_MaxSurface->area);

    form_factor += temp_factor;

    if (form_factor > 1)
      mprintf(0, "form factor >1! val=%f\n", form_factor);
  }

  if (ignored > 0)
    Rays_ignored++;

  return form_factor;
}

float GetFormFactorForElementSuperDetail(rad_surface *dest_surf, rad_element *dest_element) {
  int touched = 0;
  vector dest_center;
  float ray_area;
  int ignored = 0;
  vector light_center;
  vector patch_center;
  float form_factor = 0.0;
  int multiple_shoots = 0;

  ray_area = rad_MaxElement->area;

  if (rad_MaxElement->flags & EF_IGNORE)
    return 0;

  float rmax = GetMaxColor(&rad_MaxElement->exitance);

  float temp_factor = 0.0;

  GetCenterOfSurface(rad_MaxSurface, &patch_center);
  GetCenterOfElement(rad_MaxElement, &light_center);
  light_center += (rad_MaxSurface->normal / 16.0);

  GetCenterOfElement(dest_element, &dest_center);
  dest_center += (dest_surf->normal / 16.0);

  int hit = 0;
  vector ray = light_center - dest_center;
  vector revray = dest_center - light_center;

  if ((vm_DotProduct(&ray, &dest_surf->normal)) < 0)
    return 0;
  if ((vm_DotProduct(&revray, &rad_MaxSurface->normal)) < 0)
    return 0;

  // Check to see if this ray even matters
  float ray_length = vm_GetMagnitudeFast(&ray);
  vector dest_norm_ray = ray / ray_length;
  vector src_norm_ray = -dest_norm_ray;
  vector dest_normal = dest_surf->normal;
  float ff;
  vector src_normal = rad_MaxSurface->normal;
  ff = (vm_DotProduct(&src_norm_ray, &src_normal) * vm_DotProduct(&dest_normal, &dest_norm_ray)) /
       ((3.14 * ray_length * ray_length) + ray_area);
  ff *= ray_area;
  ff *= rmax;

  // If shooting from elements, scale the test up
  ff *= rad_MaxSurface->area / rad_MaxElement->area;

  if (ff < Ignore_limit) {
    Rays_ignored++;
    return 0;
  }

  hit = ShootRayFromPoint(&light_center, &dest_center, rad_MaxSurface, dest_surf);

  if (hit) {
    // We got a hit...figure out how much light contribution this light has

    ray_length = vm_GetMagnitude(&ray);
    dest_norm_ray = ray / ray_length;
    src_norm_ray = -dest_norm_ray;
    dest_normal = dest_surf->normal;
    src_normal = rad_MaxSurface->normal;

    ff = (vm_DotProduct(&src_norm_ray, &src_normal) * vm_DotProduct(&dest_normal, &dest_norm_ray)) /
         ((3.14 * ray_length * ray_length) + ray_area);

    if (ff > 1)
      mprintf(0, "ff >1!\n");

    if (ff > 0)
      temp_factor += ff;

    // Update specularity if needed
    if (rad_MaxSurface->surface_type == ST_ROOM && Calculate_specular_lighting) {
      spectra color;
      float scalar = ff;

      scalar *= ray_area;
      scalar *= (rad_MaxSurface->area / ray_area);
      scalar *= 5;

      color.r = rad_MaxSurface->exitance.r * scalar;
      color.g = rad_MaxSurface->exitance.g * scalar;
      color.b = rad_MaxSurface->exitance.b * scalar;

      float total_strength = (color.r * .33) + (color.g * .33) + (color.b * .33);
      if (total_strength > Current_max_specular_strength) {
        Current_max_specular_strength = total_strength;
        Current_max_specular_color = color;
      }
    }
  }

  temp_factor *= ray_area;

  form_factor += temp_factor;

  if (form_factor > 1)
    mprintf(0, "form factor >1! val=%f\n", form_factor);

  return form_factor;
}

// Calculates the volume lighting for the currently shooting patch
void CalculateVolumeLightsForRay(float total_sphere_dist, vector *src_center) {
  float sphere_dist = total_sphere_dist;
  int i, t;

  // Do volume lighting
  if ((rad_MaxSurface->surface_type == ST_ROOM || rad_MaxSurface->surface_type == ST_ROOM_OBJECT)) {
    if (sphere_dist > .1) {
      fvi_face_room_list facelist[5000];
      uint8_t check_room[MAX_VOLUME_ELEMENTS];

      memset(check_room, 0, MAX_VOLUME_ELEMENTS);

      // Build a list of rooms to check
      int num_faces = fvi_QuickDistFaceList(rad_MaxSurface->roomnum, src_center, sphere_dist, facelist, 4000);
      check_room[rad_MaxSurface->roomnum] = 1;

      for (i = 0; i < num_faces; i++) {
        if (Rooms[facelist[i].room_index].flags & RF_EXTERNAL)
          continue;
        check_room[facelist[i].room_index] = 1;
      }

      for (int roomnum = 0; roomnum < MAX_VOLUME_ELEMENTS; roomnum++) {
        if (Volume_elements[roomnum] == NULL)
          continue;
        if (check_room[roomnum] == 0)
          continue;

        if (!BOA_IsVisible(roomnum, rad_MaxSurface->roomnum)) {
          continue;
        }

        int w = Rooms[roomnum].volume_width;
        int h = Rooms[roomnum].volume_height;
        int d = Rooms[roomnum].volume_depth;

        if (0 && !Shoot_from_patch && (rad_MaxSurface->flags & SF_LIGHTSOURCE)) // super detail
        {
          int src_num_elements = rad_MaxSurface->xresolution * rad_MaxSurface->yresolution;

          for (int k = 0; k < src_num_elements; k++) {
            vector light_center;
            rad_MaxElement = &rad_MaxSurface->elements[k];

            if (rad_MaxElement->flags & EF_IGNORE)
              continue;

            GetCenterOfElement(rad_MaxElement, &light_center);
            light_center += (rad_MaxSurface->normal / 4);

            sphere_dist = total_sphere_dist;
            // sphere_dist*=(rad_MaxElement->area/rad_MaxSurface->area);

            for (i = 0; i < d; i++) {
              for (t = 0; t < h; t++) {
                for (int j = 0; j < w; j++) {
                  volume_element *ve = &Volume_elements[roomnum][(i * w * h) + (t * w) + j];

                  if (ve->color.r < 0)
                    continue;

                  vector subvec = ve->pos - light_center;
                  float mag = vm_GetMagnitudeFast(&subvec);

                  if (mag > sphere_dist)
                    continue;

                  subvec /= mag;
                  int hit;

                  float scalar = vm_Dot3Product(subvec, rad_MaxSurface->normal);

                  if (scalar < 0)
                    continue;

                  scalar /= ((3.14 * mag * mag) + rad_MaxElement->area);
                  scalar *= rad_MaxElement->area;

                  // Trivially reject all rooms

                  if (!BOA_IsVisible(roomnum, rad_MaxSurface->roomnum)) {
                    hit = 0;
                  } else {
                    if (ve->flags & VEF_REVERSE_SHOOT)
                      hit = ShootRayToVolumePoint(&ve->pos, &light_center, roomnum);
                    else
                      hit = ShootRayToVolumePoint(&light_center, &ve->pos, rad_MaxSurface->roomnum);
                  }

                  if (hit) {

                    ve->color.r += rad_MaxElement->exitance.r * scalar;
                    ve->color.g += rad_MaxElement->exitance.g * scalar;
                    ve->color.b += rad_MaxElement->exitance.b * scalar;
                  }
                }
              }
            }
          }

        } else {
          for (i = 0; i < d; i++) {
            for (t = 0; t < h; t++) {
              for (int j = 0; j < w; j++) {
                volume_element *ve = &Volume_elements[roomnum][(i * w * h) + (t * w) + j];

                if (ve->color.r < 0)
                  continue;

                vector subvec = ve->pos - *src_center;
                float mag = vm_GetMagnitudeFast(&subvec);

                if (mag > sphere_dist)
                  continue;

                subvec /= mag;
                int hit;

                float scalar = vm_Dot3Product(subvec, rad_MaxSurface->normal);

                if (scalar < 0)
                  continue;

                scalar /= ((3.14 * mag * mag) + rad_MaxSurface->area);
                scalar *= rad_MaxSurface->area;

                if (!BOA_IsVisible(roomnum, rad_MaxSurface->roomnum)) {
                  hit = 0;
                } else {
                  if (ve->flags & VEF_REVERSE_SHOOT)
                    hit = ShootRayToVolumePoint(&ve->pos, src_center, roomnum);
                  else
                    hit = ShootRayToVolumePoint(src_center, &ve->pos, rad_MaxSurface->roomnum);
                }

                if (hit) {
                  ve->color.r += rad_MaxSurface->exitance.r * scalar;
                  ve->color.g += rad_MaxSurface->exitance.g * scalar;
                  ve->color.b += rad_MaxSurface->exitance.b * scalar;
                }
              }
            }
          }
        }
      }
    }
  }
}

// Calculates the form factors using a raycasting method
void CalculateFormFactorsRaycast() {
  int i, t, k;
  int ignore;
  vector src_center;
  float form_factor;
  int raycount = 0;
  Rays_ignored = 0;

  if (rad_MaxSurface->surface_type == ST_SATELLITE) {
    src_center = rad_MaxSurface->verts[0];
  } else {
    GetCenterOfSurface(rad_MaxSurface, &src_center);
    src_center += (rad_MaxSurface->normal / 16.0);
  }

  float sphere_dist = 0;
  // Get the max area of influence for this light
  if (rad_MaxSurface->surface_type == ST_ROOM || rad_MaxSurface->surface_type == ST_ROOM_OBJECT) {
    float rmax = GetMaxColor(&rad_MaxSurface->exitance);
    float power = rmax * rad_MaxSurface->area;
    float temp_ignore = Ignore_limit / power;
    float express = 1.0 / temp_ignore;
    express /= 3.14f;
    express -= rad_MaxSurface->area;
    sphere_dist = sqrt(express);
    if (sphere_dist > 0)
      fvi_QuickDistFaceList(rad_MaxSurface->roomnum, &src_center, sphere_dist, NULL, rad_NumSurfaces);
  }

  // Do volume lighting
  if (Do_volume_lighting)
    CalculateVolumeLightsForRay(sphere_dist, &src_center);

  // Shoot this patches light to each element within range
  for (i = 0; i < rad_NumSurfaces; i++) {
    rad_surface *dest_surf = &rad_Surfaces[i];

    Current_max_specular_strength = 0;
    Current_max_specular_color.r = 0;
    Current_max_specular_color.g = 0;
    Current_max_specular_color.b = 0;

    // Check for self surface

    if (rad_MaxSurface == dest_surf)
      ignore = 1;
    else
      ignore = 0;

    if (dest_surf->surface_type == ST_PORTAL)
      ignore = 1;

    if (dest_surf->surface_type == ST_SATELLITE)
      ignore = 1;

    if (rad_MaxSurface->surface_type == ST_ROOM || rad_MaxSurface->surface_type == ST_ROOM_OBJECT) {
      if (dest_surf->surface_type == ST_ROOM) {
        if (!(Rooms[dest_surf->roomnum].faces[dest_surf->facenum].flags & FF_TOUCHED))
          ignore = 1;

        Rooms[dest_surf->roomnum].faces[dest_surf->facenum].flags &= ~FF_TOUCHED;
      }
    }

    if (dest_surf->surface_type == ST_ROOM)
      Rooms[dest_surf->roomnum].faces[dest_surf->facenum].flags &= ~FF_TOUCHED;

    int dest_num_elements = dest_surf->xresolution * dest_surf->yresolution;

    // Ignore this surface if we're shooting from a satellite and we cant possibly see it
    if (rad_MaxSurface->surface_type == ST_SATELLITE) {
      if (dest_surf->surface_type == ST_ROOM || dest_surf->surface_type == ST_ROOM_OBJECT) {
        if (!dest_surf->flags & SF_TOUCHES_TERRAIN)
          ignore = 1;
      }
    }

    if (ignore) {
      raycount += dest_num_elements;
      continue;
    }

    for (t = 0; t < dest_num_elements; t++) {
      rad_element *dest_element = &dest_surf->elements[t];

      raycount++;

      if (ignore)
        continue;

      if (dest_element->flags & EF_IGNORE)
        continue;

      if (Shoot_from_patch || rad_MaxSurface->surface_type == ST_SATELLITE ||
          !(rad_MaxSurface->flags & SF_LIGHTSOURCE)) {
        if (rad_MaxSurface->surface_type == ST_SATELLITE)
          form_factor = GetFormFactorForElementAndSatellite(dest_surf, dest_element, &src_center);
        else
          form_factor = GetFormFactorForElement(dest_surf, dest_element, &src_center);

        // Now extract the form factor info out to exitances
        if (form_factor > 0) {
          spectra shoot = rad_MaxSurface->exitance;
          spectra delta;
          float reflect_factor = dest_surf->reflectivity;

          delta.r = shoot.r * reflect_factor * form_factor;
          delta.g = shoot.g * reflect_factor * form_factor;
          delta.b = shoot.b * reflect_factor * form_factor;

          // update element exitance
          dest_element->exitance.r += delta.r;
          dest_element->exitance.g += delta.g;
          dest_element->exitance.b += delta.b;

          dest_surf->exitance.r += ((dest_element->area / dest_surf->area) * delta.r);
          dest_surf->exitance.g += ((dest_element->area / dest_surf->area) * delta.g);
          dest_surf->exitance.b += ((dest_element->area / dest_surf->area) * delta.b);
        }
      } else // Do super detail
      {
        int src_num_elements = rad_MaxSurface->xresolution * rad_MaxSurface->yresolution;

        for (k = 0; k < src_num_elements; k++) {
          rad_MaxElement = &rad_MaxSurface->elements[k];

          if (rad_MaxElement->flags & EF_IGNORE)
            continue;

          form_factor = GetFormFactorForElementSuperDetail(dest_surf, dest_element);
          // Now extract the form factor info out to exitances
          if (form_factor > 0) {
            spectra shoot = rad_MaxElement->exitance;
            spectra delta;
            float reflect_factor = dest_surf->reflectivity;
            delta.r = shoot.r * reflect_factor * form_factor;
            delta.g = shoot.g * reflect_factor * form_factor;
            delta.b = shoot.b * reflect_factor * form_factor;
            // update element exitance
            dest_element->exitance.r += delta.r;
            dest_element->exitance.g += delta.g;
            dest_element->exitance.b += delta.b;

            dest_surf->exitance.r += ((dest_element->area / dest_surf->area) * delta.r);
            dest_surf->exitance.g += ((dest_element->area / dest_surf->area) * delta.g);
            dest_surf->exitance.b += ((dest_element->area / dest_surf->area) * delta.b);
          }
        }
      }
    }

    if (rad_MaxSurface->surface_type == ST_ROOM && Calculate_specular_lighting)
      CheckToUpdateSpecularFace(dest_surf, &Current_max_specular_color, NULL);
  }
}
