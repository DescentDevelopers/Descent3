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

 * $Source: $
 * $Revision: 8 $
 * $Author: Samir $
 * $Date: 8/13/99 2:00p $
 *
 *
 *
 * $Log: /DescentIII/Main/sndlib/sndrender.cpp $
 *
 * 8     8/13/99 2:00p Samir
 * more aureal and geometry fixes.
 *
 * 7     8/11/99 3:12p Samir
 * fixes for aureal support.
 *
 * 6     5/13/99 3:49p Ardussi
 * changes for compiling on the Mac
 *
 * 5     4/16/99 3:33a Jeff
 * ifdefs added to remove windows specific code
 *
 * 4     4/14/99 10:46a Kevin
 * Removed OutrageMessageBox from release builds
 *
 * 3     4/06/99 8:30p Samir
 * added reflection support.
 *
 * 2     4/01/99 4:28p Samir
 * hardware geometry integration if it's available.
 *
 */

#include <cstring>

#include "hlsoundlib.h"
#include "ddsndgeometry.h"
#include "sndrender.h"
#include "room.h"
#include "object.h"
#include "application.h"
#include "soundload.h"
#include "gametexture.h"


static llsGeometry *Geometry = NULL;
static int16_t Sound_room_list[SOUND_RENDER_ROOM_LIMIT + 1];

//	resets sound geometry system (called beginning of each level)
void SoundRenderReset() { Sound_room_list[0] = -1; }

// if this fails, renderer won't work, do emulation.
bool sound_render_start_frame() {
  Geometry = Sound_system.m_ll_sound_ptr->GetGeometryInterface();
  if (Geometry) {
    Geometry->StartFrame();
  }
  return Geometry ? true : false;
}

// ends a frame
void sound_render_end_frame() {
  if (Geometry) {
    Geometry->EndFrame();
  }
  Geometry = NULL;
}

// render one room's geometry
// dont render portal faces!
void sound_render_room_geometry(int iroom, int slot) {
  int iface, reflect_face;
  room *rp;
  face *rfacelist;
  vector *vertlist[MAX_VERTS_PER_FACE];
  unsigned room_tag = (iroom << 20); // 12 bits for room (4096 rooms max per level)
                                     // 14 bits for faces (16384 faces per room)
                                     // 6 bits per face (64 primatives per face.)
  unsigned face_tag;

  ASSERT(Geometry);
  if (iroom == -1) {
    return;
  }

  if (iroom != Sound_room_list[slot]) {
    if (ROOMNUM_OUTSIDE(iroom)) {
      Geometry->Clear();
      Sound_room_list[slot] = iroom;
      return;
    }

    Geometry->StartPolygonGroup(slot);

    rp = &Rooms[iroom];
    rfacelist = rp->faces;

    //	for now, lets create the geometry for the current room the viewer is in.
    for (iface = 0, reflect_face = 0; iface < rp->num_faces; iface++, reflect_face = (iface / 3)) {
      //	add polygon to geometry manager
      tSoundMaterial material = SNDGEO_MATERIAL_NONE;
      face *rfacep = &rfacelist[iface];
      int ivert;

      face_tag = (iface << 6);

      // don't render portals!
      if (rfacep->portal_num > -1) {

      } else {
        for (ivert = 0; ivert < rfacep->num_verts; ivert++) {
          vertlist[ivert] = &rp->verts[rfacep->face_verts[ivert]];
        }

        // do reflections
        if (iface == (reflect_face * 3)) {
          //	mprintf(0, "reflect ");
          int flags = GameTextures[rfacep->tmap].flags;

          if (flags & TF_WATER) {
            material = SNDGEO_MATERIAL_WATER;
          } else if (flags & TF_METAL) {
            material = SNDGEO_MATERIAL_METAL;
          } else {
            material = SNDGEO_MATERIAL_ROCK;
          }
        }
        Geometry->AddPoly(rfacep->num_verts, vertlist, (room_tag + face_tag), material);
      }
    }
    Geometry->EndPolygonGroup(slot);
    Geometry->RenderGroup(slot);
    Sound_room_list[slot] = iroom;
  }
}

// render all rooms within a certain radius from player
// the radius is actually scaled according to orientation.  radius is maximum radius, really, which is forward
//
//	returns rooms rendered, terminate list with -1.
//
//	algorithm:
//
//	routine (Ci,Ri)
//		start at listener position in room Ri (i=0)
//		render room Ri
//		for each portal, connect to room Rj.
//			take care of special cases like doors, which are 'openings'
//			find center point in Cj.
//			if Distance(Cj,Ci)) < (max_radius*Orient(R0)) then
//				call routine(Cj,Rj)
//			else
//				end routine
//			endif
//		next portal
//	end routine
//
int16_t *sound_render_audible_rooms(pos_state *listener_pos, float max_radius) {
  int iroom = 0;

  sound_render_room_geometry(listener_pos->roomnum, iroom);
  Sound_room_list[iroom + 1] = -1;

  return (int16_t *)Sound_room_list;
}
