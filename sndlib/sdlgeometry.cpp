/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 * Copyright (C) 2024 Descent Developers
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


///////////////////////////////////////////////////////////////////////
// llsGeometry
#include "ddsndgeometry.h"

// specify a sound library to associate geometry with
bool llsGeometry::Init(llsSystem *snd_sys) { return false; }

// closes low level geometry system.
void llsGeometry::Shutdown() {}

void llsGeometry::StartFrame() {}
void llsGeometry::EndFrame() {}

// clears out geometry info
void llsGeometry::Clear() {}

// polygon lists
// is a group cached?, check before rendering it.
void llsGeometry::IsGroupValid(int group) {}

// marks beginning of a list of polygons to render, (-1 group for non cache)
void llsGeometry::StartPolygonGroup(int group) {}

// ends a list of polygons to render.
void llsGeometry::EndPolygonGroup(int group) {}

// renders a group.
void llsGeometry::RenderGroup(int group) {}

// primatives, nv = number of verts, and verts is an array of pointers to vertices.
// you can pass a sound material value if you want special reflective properties on this polygon.
void llsGeometry::AddPoly(int nv, vector **verts, unsigned tag, tSoundMaterial material) {}

// 4 verts here.
void llsGeometry::AddQuad(unsigned tag, vector **verts) {}

// 3 verts here.
void llsGeometry::AddTriangle(unsigned tag, vector **verts) {}

void llsGeometry::CreateMaterial(tSoundMaterial material, float transmit_gain, float transmit_highfreq,
                                 float reflect_gain, float reflect_highfreq) {}

void llsGeometry::DestroyMaterial(tSoundMaterial material) {}
