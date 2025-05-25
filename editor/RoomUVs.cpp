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

 * $Logfile: /DescentIII/Main/editor/RoomUVs.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Code to deal with texturing rooms
 *
 * $Log: not supported by cvs2svn $
 *
 * 9     9/30/99 6:35p Matt
 * Added code to scale the UVs of a texture based on the size of the
 * texture when the texture is applied to a face.  This will cause the
 * applied texture to have the make 3D texel size at the old texture.
 *
 * 8     4/25/99 2:38p Gwar
 * brought in HRoom.cpp and RoomUVs.cpp, and several misc game functions
 * were added to globals.cpp to make it possible
 *
 * 7     10/21/98 1:27p Jason
 * fixed texture u2,v2 sliding problems
 *
 * 6     10/07/98 3:19p Matt
 * Cleaned up and fixed Mike's old texture UV calculation code.  Who ever
 * told that boy  he could do 3D?
 *
 * 5     4/02/98 12:23p Jason
 * trimmed some fat from our structures
 *
 * 4     12/23/97 1:33p Samir
 * Added pserror.h
 *
 * 3     9/17/97 11:57a Matt
 * Ripped out segment code
 *
 * 2     7/18/97 7:38p Jason
 * finished room/terrain texture modifications
 *
 * $NoKeywords: $
 */

#include "RoomUVs.h"

#ifndef NEWEDITOR
#include "d3edit.h"
#else
#include "..\neweditor\globals.h"
#endif

#include "pserror.h"

// returns the magnatude of the 2d vector <a,b>
static float zhypot(float a, float b) { return sqrt(a * a + b * b); }

//	Given u,v coordinates at two vertices, assign u,v coordinates to the other vertices on a face.
//	va, vb = face-relative vertex indices corresponding to uva, uvb.  Ie, they are always in 0..num_verts_in_face
void AssignUVsToFace(room *rp, int facenum, roomUVL *uva, roomUVL *uvb, int va, int vb) {
  face *fp = &rp->faces[facenum];
  int nv = fp->num_verts;
  int vlo, vhi;
  vector fvec, rvec, tvec;
  roomUVL ruvmag, fuvmag, uvlo, uvhi;
  float fmag, mag01;
  int i;

  float saveu2[MAX_VERTS_PER_FACE], savev2[MAX_VERTS_PER_FACE];
  for (i = 0; i < fp->num_verts; i++) {
    saveu2[i] = fp->face_uvls[i].u2;
    savev2[i] = fp->face_uvls[i].v2;
  }

  ASSERT((va < nv) && (vb < nv));
  ASSERT((abs(va - vb) == 1) || (abs(va - vb) == nv - 1)); // make sure the verticies specify an edge

  // We want vlo precedes vhi, ie vlo < vhi, or vlo = num_verts, vhi = 0
  if (va == ((vb + 1) % nv)) { // va = vb + 1
    vlo = vb;
    vhi = va;
    uvlo = *uvb;
    uvhi = *uva;
  } else {
    vlo = va;
    vhi = vb;
    uvlo = *uva;
    uvhi = *uvb;
  }

  ASSERT(((vlo + 1) % nv) == vhi); // If we are on an edge, then uvhi is one more than uvlo (mod num_verts)
  fp->face_uvls[vlo] = uvlo;
  fp->face_uvls[vhi] = uvhi;

  // Now we have vlo precedes vhi, compute vertices ((vhi+1) % nv) and ((vhi+2) % nv)

  // Assign u,v scale to a unit length right vector.
  fmag = zhypot(uvhi.v - uvlo.v, uvhi.u - uvlo.u);
  if (fmag < 0.001) {
    // mprintf(0,"Warning: fmag = %7.3f, using approximate u,v values\n",f2fl(fmag));
    ruvmag.u = 256.0;
    ruvmag.v = 256.0;
    fuvmag.u = 256.0;
    fuvmag.v = 256.0;
  } else {
    ruvmag.u = uvhi.v - uvlo.v;
    ruvmag.v = uvlo.u - uvhi.u;

    fuvmag.u = uvhi.u - uvlo.u;
    fuvmag.v = uvhi.v - uvlo.v;
  }

  // Get pointers to our verts
  vector *vv0 = &rp->verts[fp->face_verts[vlo]], *vv1 = &rp->verts[fp->face_verts[vhi]];

  // Get forward vector from our edge
  fvec = *vv1 - *vv0;
  mag01 = vm_NormalizeVector(&fvec);

  // Check for bad vector
  if (mag01 < 0.001) {
    OutrageMessageBox("U, V bogosity in room #%i, probably on face #%i.  CLEAN UP YOUR MESS!", ROOMNUM(rp), facenum);
    return;
  }

  // Get right vector from the cross product of the forward vec with the surface normal
  rvec = fvec ^ fp->normal;

  // Normalize uv values
  ruvmag.u /= mag01;
  ruvmag.v /= mag01;
  fuvmag.u /= mag01;
  fuvmag.v /= mag01;

  // Compute UVs for each point
  for (i = 1; i < nv - 1; i++) {
    int fv = (vhi + i) % nv;     // vert index in face
    int rv = fp->face_verts[fv]; // vert index in room

    // Get the vector for this edge
    tvec = rp->verts[rv] - *vv0;

    // Project the current edge onto our forward & right vectors
    scalar rproj = vm_Dot3Product(tvec, rvec), fproj = vm_Dot3Product(tvec, fvec);

    // Compute and assign UV values
    fp->face_uvls[fv].u = uvlo.u + (ruvmag.u * rproj) + (fuvmag.u * fproj);
    fp->face_uvls[fv].v = uvlo.v + (ruvmag.v * rproj) + (fuvmag.v * fproj);
  }
}

// Stretches the UVS of a face
void StretchRoomUVs(room *rp, int facenum, int edge) {
  roomUVL uv0, uv1;
  int v0, v1;
  face *fp = &rp->faces[facenum];

  int i;
  float saveu2[MAX_VERTS_PER_FACE], savev2[MAX_VERTS_PER_FACE];
  for (i = 0; i < fp->num_verts; i++) {
    saveu2[i] = fp->face_uvls[i].u2;
    savev2[i] = fp->face_uvls[i].v2;
  }

  v0 = edge;
  v1 = (v0 + 1) % rp->faces[facenum].num_verts;

  uv0 = rp->faces[facenum].face_uvls[v0]; // copy uv AND l
  uv1 = rp->faces[facenum].face_uvls[v1];

  AssignUVsToFace(rp, facenum, &uv0, &uv1, v0, v1);

  for (i = 0; i < fp->num_verts; i++) {
    fp->face_uvls[i].u2 = saveu2[i];
    fp->face_uvls[i].v2 = savev2[i];
  }
}

// Scale all the UV values in a face from the center point (as defined by averaging the u & v values)
void ScaleFaceUVs(room *rp, int facenum, float scale) {
  face *fp = &rp->faces[facenum];

  int i;
  float midu = 0, midv = 0;

  for (i = 0; i < fp->num_verts; i++) {
    midu += fp->face_uvls[i].u;
    midv += fp->face_uvls[i].v;
  }

  midu /= fp->num_verts;
  midv /= fp->num_verts;

  for (i = 0; i < fp->num_verts; i++) {
    fp->face_uvls[i].u = midu + (fp->face_uvls[i].u - midu) * scale;
    fp->face_uvls[i].v = midv + (fp->face_uvls[i].v - midv) * scale;
  }

  World_changed = 1;
}
