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
 * $Author: Jeff $
 * $Date: 10/21/99 9:28p $
 *
 * Hardware occlusion and reflection sound support
 *
 * $Log: /DescentIII/Main/dd_sndlib/geometry.cpp $
 *
 * 8     10/21/99 9:28p Jeff
 * B.A. Macintosh code merge
 *
 * 7     8/13/99 2:00p Samir
 * more aureal and geometry fixes.
 *
 * 6     8/11/99 3:12p Samir
 * fixes for aureal support.
 *
 * 5     4/13/99 4:09p Samir
 * more priority stuff.
 *
 * 4     4/06/99 8:30p Samir
 * added reflection support.
 *
 * 3     3/30/99 5:36p Matt
 * Fixed compile warnings
 *
 * 2     3/29/99 10:52a Samir
 * occlusion support almost complete.
 *
 */

#include "ds3dlib_internal.h"
#include "auddev.h"
#include "pserror.h"

//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
llsGeometry::llsGeometry() {
  m_snd_system = NULL;
  m_snd_mixer = SOUND_MIXER_NONE;
  m_lib_init = false;
}

// specify a sound library to associate geometry with
bool llsGeometry::Init(llsSystem *snd_sys) {
  int i;

  if (m_lib_init) {
    Int3(); // really, this shouldn't happen.
    return true;
  }
  if (!snd_sys) {
    Int3();
    return false;
  }

  if (!m_lib_init) {
    mprintf((0, "DDSNDGEO: Failed to initialize geometry interface.\n"));
    return false;
  }

  m_snd_mixer = snd_sys->GetSoundMixer();

  m_snd_system = snd_sys;
  m_lib_init = true;

  // create material list.
  for (i = 0; i < SNDGEO_MATERIAL_COUNT; i++) {
    m_snd_materials[i] = NULL;
  }

  CreateMaterial(SNDGEO_MATERIAL_ROCK, 0.2f, 0.5f, 0.9f, 0.8f);
  CreateMaterial(SNDGEO_MATERIAL_WATER, 1.0f, 0.3f, 0.8f, 0.7f);
  CreateMaterial(SNDGEO_MATERIAL_METAL, 0.1f, 0.1f, 0.95f, 0.85f);

  mprintf((0, "DDSNDGEO: Initialized.\n"));
  return true;
}

// closes low level geometry system.
void llsGeometry::Shutdown() {
  int i;

  if (!m_lib_init) { // damn this shouldn't happen.
    Int3();
    return;
  }

  // destroy materials list
  for (i = 0; i < SNDGEO_MATERIAL_COUNT; i++) {
    DestroyMaterial(i);
  }

  m_snd_mixer = SOUND_MIXER_NONE;
  m_snd_system = NULL;
  m_lib_init = false;

  mprintf((0, "DDSNDGEO: Shutdown.\n"));
}

void llsGeometry::StartFrame() {
  n_primatives_used = 0;
  n_reflections_used = 0;
  n_materials_used = 0;
}

void llsGeometry::EndFrame() {
  mprintf_at((3, 4, 20, "sndpoly=%04d", n_primatives_used));
  mprintf_at((3, 4, 38, "sndmat=%04d", n_materials_used));
  mprintf_at((3, 5, 20, "sndref=%04d", n_reflections_used));
}

// polygon lists

// marks beginning of a list of polygons to render
//	-1 group if non cached (user doesn't want to reuse this.
void llsGeometry::StartPolygonGroup(int group) {
  ASSERT(m_lib_init);
}

// ends a list of polygons to render.
void llsGeometry::EndPolygonGroup(int group) {
  ASSERT(m_lib_init);
}

// renders a group.
void llsGeometry::RenderGroup(int group) {
  ASSERT(m_lib_init);
}

void llsGeometry::Clear() {
  ASSERT(m_lib_init);
}

// primatives
// 4 verts here.
void llsGeometry::AddQuad(unsigned tag, vector **verts) {
  n_primatives_used++;
}

// 3 verts here.
void llsGeometry::AddTriangle(unsigned tag, vector **verts) {
  n_primatives_used++;
}

void llsGeometry::AddPoly(int nv, vector **verts, unsigned tag, tSoundMaterial material) {
  int i, saved_primatives_used; //,p;
  void *matp = NULL;

  if (nv < 3) {
    Int3();
    return;
  }

  if (material >= 0 && material < SNDGEO_MATERIAL_COUNT) {
    matp = m_snd_materials[material];
  }

  if (matp) {
    n_materials_used++;
  }
  saved_primatives_used = n_primatives_used;

  // add polygons or split.
  switch (nv) {
  case 3:
    AddTriangle(tag, verts);
    break;
  case 4:
    AddQuad(tag, verts);
    break;
  default:
    Int3();
  }

  if (matp) {
    n_reflections_used += (n_primatives_used - saved_primatives_used);
  }
}

// values MUST be from 0 to 1 for gain and highfreq.
void llsGeometry::CreateMaterial(tSoundMaterial material, float transmit_gain, float transmit_highfreq,
                                 float reflect_gain, float reflect_highfreq) {
  if (m_snd_materials[material] || material < 0 || material >= SNDGEO_MATERIAL_COUNT) {
    Int3(); // get samir, trying to intiialize a material in an existing slot.
    return;
  }

  // check values.
  if (transmit_gain < 0) {
    transmit_gain = 0;
  } else if (transmit_gain > 1) {
    transmit_gain = 1;
  }
  if (transmit_highfreq < 0) {
    transmit_highfreq = 0;
  } else if (transmit_highfreq > 1) {
    transmit_highfreq = 1;
  }
  if (reflect_gain < 0) {
    reflect_gain = 0;
  } else if (reflect_gain > 1) {
    reflect_gain = 1;
  }
  if (reflect_highfreq < 0) {
    reflect_highfreq = 0;
  } else if (reflect_highfreq > 1) {
    reflect_highfreq = 1;
  }
}

void llsGeometry::DestroyMaterial(tSoundMaterial material) {
  if (!m_snd_materials[material]) {
    if (material < 0 || material >= SNDGEO_MATERIAL_COUNT) {
      Int3(); // get samir, trying to destroy a material in an nonexisting slot.
    }
    return;
  }

  if (m_snd_materials[material]) {
    m_snd_materials[material] = NULL;
  }
}
