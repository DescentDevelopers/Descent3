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

/*
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

#include "hlsoundlib.h"
#include "ddsndgeometry.h"
#include "sndrender.h"
#include "room.h"
#include "object.h"
#include "descent.h"
#include "application.h"
#include "ddio.h"
#include "soundload.h"
#include "gametexture.h"

#ifdef LINUX
#include <string.h>
#endif

static llsGeometry *Geometry = NULL;
static short Sound_room_list[SOUND_RENDER_ROOM_LIMIT + 1];

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

// places listener in world
void sound_render_listener_in_world(pos_state *listener_pos) {
  ASSERT(Geometry);

  // update listener
  Int3();
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
          //	mprintf((0, "reflect "));
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
short *sound_render_audible_rooms(pos_state *listener_pos, float max_radius) {
  int iroom = 0;

  sound_render_room_geometry(listener_pos->roomnum, iroom);
  Sound_room_list[iroom + 1] = -1;

  return (short *)Sound_room_list;
}

/////////////////////////////////////////////////////////////////////////////
void start_sound_frame();
void end_sound_frame();
bool render_sound_objects_in_geometry();
bool render_sound_geometry_frame();
void play_3d_sound(const char *name, object *obj, float vol, int uid);

play_information sound_play_info[32];
static pos_state listener_pos_state;

/////////////////////////////////////////////////////////////////////////////
#if defined(WIN32)
#include <windows.h>
#endif

void aureal_test() {
#if defined(WIN32)
  llsSystem *lls;
  char old_mixer = Sound_system.GetSoundMixer();
  bool quit = false, start_sounds = true;
  int i, s;
  angvec listener_ang;
  matrix listener_orient, new_orient;
  vector listener_pos;
  vector listener_vel;
  float frame_time = 0.1f;

  const char *sound_names[] = {"Raindrop", "BreakingGlass", "Cheater!", NULL};

  // initialize aureal	and debug viewer
  Sound_system.SetSoundMixer(SOUND_MIXER_AUREAL);
  lls = Sound_system.m_ll_sound_ptr;

  //	listener_pos.position = &Viewer_object->pos;
  //	listener_pos.velocity = &Viewer_object->mtype.phys_info.velocity;
  //	listener_pos.orient = &Viewer_object->orient;
  listener_pos = Viewer_object->pos;
  listener_orient = Viewer_object->orient;
  listener_vel = Viewer_object->mtype.phys_info.velocity;

  listener_pos_state.position = &listener_pos;
  listener_pos_state.velocity = &listener_vel;
  listener_pos_state.orient = &listener_orient;
  listener_pos_state.roomnum = Viewer_object->roomnum;

  //	use D3 geo
  while (!quit) {
    int key;
    MSG msg;
    matrix rotmat;
    float frame_start_time;

    frame_start_time = timer_GetTime();
    while (1) {
      if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
          exit(1);
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      } else {
        ddio_Frame();
        Sleep(50);
        break;
      }
    }

    start_sound_frame();

    while ((key = ddio_KeyInKey()) != 0) {
      if (key == KEY_ESC) {
        lls->StopAllSounds();
        quit = true;
      } else if (key == KEY_2) {
        mprintf((0, "aureal test stop sounds.\n"));
        lls->StopAllSounds();
      } else if (key == KEY_1) {
        start_sounds = true;
      }
    }

    listener_ang.p = 0;
    listener_ang.b = 0;
    listener_ang.h = 0;
    listener_vel.z = 0;

    if (KEY_STATE(KEY_LEFT)) {
      listener_ang.h = -4096.0f * frame_time;
    }
    if (KEY_STATE(KEY_RIGHT)) {
      listener_ang.h = 4096.0f * frame_time;
    }
    if (KEY_STATE(KEY_UP)) {
      listener_ang.p = -4096.0f * frame_time;
    }
    if (KEY_STATE(KEY_DOWN)) {
      listener_ang.p = 4096.0f * frame_time;
    }
    if (KEY_STATE(KEY_A)) {
      listener_vel.z = 8.0f * frame_time;
    }
    if (KEY_STATE(KEY_Z)) {
      listener_vel.z = -8.0f * frame_time;
    }

    vm_AnglesToMatrix(&rotmat, listener_ang.p, listener_ang.h, listener_ang.b);
    new_orient = listener_orient * rotmat;
    vm_Orthogonalize(&new_orient);
    listener_orient = new_orient;
    vm_TransposeMatrix(&new_orient);
    listener_pos = listener_pos + (listener_vel * new_orient);

    if (start_sounds) {
      if (!ROOMNUM_OUTSIDE(listener_pos_state.roomnum)) {
        int obji;
        room *rp = &Rooms[listener_pos_state.roomnum];
        mprintf((0, "aureal test start sounds.\n"));
        s = 0, i = 0;
        obji = rp->objects;
        while (obji != -1 && i < 8) {
          if (sound_names[s] && Objects[obji].type != OBJ_VIEWER && Objects[obji].type != OBJ_PLAYER) {
            mprintf((0, "aureal test adding sound.\n"));
            play_3d_sound(sound_names[s], &Objects[obji], 1.0f, i);
            s++;
          } else if (!sound_names[s]) {
            s = 0;
          }
          obji = Objects[obji].next;
          i++;
        }
      }
      start_sounds = false;
    }

    end_sound_frame();

    frame_time = timer_GetTime() - frame_start_time;
  }
#ifndef RELEASE
  // restore old mixer and ask user to terminate debug viewer before we do.
  OutrageMessageBox("Please terminate debug viewer");
#endif
  Sound_system.SetSoundMixer(old_mixer);
#endif // #ifdef WIN32
}

/////////////////////////////////////////////////////////////////////////////
// sequencing functions
void start_sound_frame() {
#if defined(WIN32)
  // update lowlevel sound library
  Sound_system.m_ll_sound_ptr->SoundStartFrame();
  sound_render_start_frame();

  //	render sound geometry!
  render_sound_geometry_frame();
#endif
}

void end_sound_frame() {
#if defined(WIN32)
  sound_render_end_frame();
  Sound_system.m_ll_sound_ptr->SoundEndFrame();
#endif
}

// render_sound_geometry_frame
//		returns: false if sound system can't do this internally, or other error.
//					in this case, the caller should emulate 3d sound.
bool render_sound_geometry_frame() {
#if defined(WIN32)
  llsGeometry *geometry = Sound_system.m_ll_sound_ptr->GetGeometryInterface();
  int iroom;

  if (!render_sound_objects_in_geometry()) {
    return false;
  }

  if (!geometry) { // we couldn't access a geometry interface
    return false;
  }

  iroom = Viewer_object->roomnum;
  sound_render_room_geometry(iroom, 0);

  return true;
#else
  return false;
#endif
}

bool render_sound_objects_in_geometry() {
#if defined(WIN32)
  llsGeometry *geometry = Sound_system.m_ll_sound_ptr->GetGeometryInterface();

  if (!geometry) { // we couldn't access a geometry interface
    return false;
  }

  // update listener
  sound_render_listener_in_world(&listener_pos_state);

  return true;
#else
  return false;
#endif
}

void play_3d_sound(const char *name, object *obj, float vol, int uid) {
  llsSystem *lls = Sound_system.m_ll_sound_ptr;
  int index = FindSoundName((char *)name);
  pos_state cur_pos;

  if (!lls->CheckAndForceSoundDataAlloc(index)) {
    mprintf((0, "failed to play 3d sound due to alloc error.\n"));
    return;
  }

  cur_pos.position = &obj->pos;
  cur_pos.orient = &obj->orient;
  cur_pos.velocity = &obj->mtype.phys_info.velocity;
  cur_pos.roomnum = obj->roomnum;

  memset(&sound_play_info[uid], 0, sizeof(play_information));
  sound_play_info[uid].samples_per_22khz_sample = 1.0;
  sound_play_info[uid].sample_skip_interval = 0;

  if (lls->PlaySound3d(&sound_play_info[uid], index, &cur_pos, vol, true) == -1) {
    mprintf((0, "failed to play 3d sound.\n"));
  }
}
