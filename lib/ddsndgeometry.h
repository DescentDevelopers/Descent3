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
 * $Revision: 5 $
 * $Author: Samir $
 * $Date: 8/13/99 2:00p $
 *
 * Hardware occlusion and reflection sound support.
 *
 * $Log: /DescentIII/Main/Lib/ddsndgeometry.h $
 *
 * 5     8/13/99 2:00p Samir
 * more aureal and geometry fixes.
 *
 * 4     8/11/99 3:12p Samir
 * fixes for aureal support.
 *
 * 3     4/06/99 8:30p Samir
 * added reflection support.
 *
 * 2     3/29/99 10:52a Samir
 * occlusion support almost complete.
 *
 */

#ifndef DDSNDGEOMETRY_H
#define DDSNDGEOMETRY_H

#include "vecmat.h"

//	constants
typedef int tSoundMaterial; // value returned by geometry system.

// a list of predefined sound materials.
const int SNDGEO_MATERIAL_COUNT = 8;
const tSoundMaterial SNDGEO_MATERIAL_NONE = -1, SNDGEO_MATERIAL_ROCK = 0, SNDGEO_MATERIAL_WATER = 1,
                     SNDGEO_MATERIAL_METAL = 2;

//////////////////////////////////////////////////////////////////////////////

class llsSystem;

class llsGeometry {
public:
  llsGeometry();

  bool Init(llsSystem *snd_sys); // specify a sound library to associate geometry with
  void Shutdown();               // closes low level geometry system.

  void StartFrame();
  void EndFrame();

  // polygon lists
  void IsGroupValid(int group);      // is a group cached?, check before rendering it.
  void StartPolygonGroup(int group); // marks beginning of a list of polygons to render, (-1 group for non cache)
  void EndPolygonGroup(int group);   // ends a list of polygons to render.
  void RenderGroup(int group);       // renders a group.

  // clears out geometry info
  void Clear();

  // primatives, nv = number of verts, and verts is an array of pointers to vertices.
  // you can pass a sound material value if you want special reflective properties on this polygon.
  void AddPoly(int nv, vector **verts, unsigned tag, tSoundMaterial material = SNDGEO_MATERIAL_NONE);

private:
  void AddQuad(unsigned tag, vector **verts);     // 4 verts here.
  void AddTriangle(unsigned tag, vector **verts); // 3 verts here.

private:
  llsSystem *m_snd_system; // sound library attached.
  int n_primatives_used;   // number of primatives rendered this frame.
  int n_reflections_used;
  int n_materials_used;
  char m_snd_mixer; // current sound mixer attached.
  bool m_in_group;  // a group of polys is being defined.
  bool m_lib_init;

private:
  void CreateMaterial(tSoundMaterial material, float transmit_gain, float transmit_highfreq, float reflect_gain,
                      float reflect_highfreq);
  void DestroyMaterial(tSoundMaterial material);

private: // sound material list.
  void *m_snd_materials[SNDGEO_MATERIAL_COUNT];
};

#endif
