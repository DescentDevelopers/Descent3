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

 * $Logfile: /DescentIII/Main/editor/rotateroom.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 *
 * $Log: not supported by cvs2svn $
 *
 * 7     9/01/98 12:04p Matt
 * Ripped out multi-face portal code
 *
 * 6     4/08/98 12:10p Jason
 * fixed bug where portal faces would get splintered
 *
 * 5     12/23/97 1:33p Samir
 * Added pserror.h
 *
 * 4     8/01/97 12:50p Jason
 * added code to support scaling of rooms/faces/edges
 *
 * 3     7/31/97 4:37p Jason
 * rotate room now triangulates non-planar faces
 *
 * 2     7/31/97 3:31p Jason
 * added functions to rotate portal rooms
 *
 * 1     7/31/97 10:35a Jason
 * file for rotating rooms
 *
 * $NoKeywords: $
 */

#include "RotateRoom.h"
#include "d3edit.h"
#include "selectedroom.h"
#include "erooms.h"
#include "pserror.h"

void RotateRooms(angle p, angle h, angle b) {
  int checkfaces[MAX_FACES_PER_ROOM];
  int checkcount = 0;
  matrix rotmat, roommat;
  vector rotpoint, portal_normal;
  int t, v, i, k;
  int marked_portalnum = -1;
  int cur_portalnum = -1;

  if (Curroomp == Markedroomp || Markedroomp == NULL) {
    OutrageMessageBox("You do not have a valid room marked.");
    return;
  }

  for (i = 0; i < Markedroomp->num_portals; i++) {
    if (Markedroomp->portals[i].croom == (Curroomp - Rooms)) {
      marked_portalnum = i;
      break;
    }
  }

  if (marked_portalnum == -1) {
    OutrageMessageBox("The marked room is not connected to the current room!");
    return;
  }

  for (i = 0; i < Curroomp->num_portals; i++) {
    if (Curroomp->portals[i].croom == (Markedroomp - Rooms)) {
      cur_portalnum = i;
      break;
    }
  }

  if (cur_portalnum == -1) {
    OutrageMessageBox("The marked room is not connected to the current room!");
    return;
  }

  // Save the list so we can use it
  SaveRoomSelectedList();

  Curroomp->portals[cur_portalnum].croom = -1; // detach base
  SelectConnectedRooms(Curroomp - Rooms);
  Curroomp->portals[cur_portalnum].croom = Markedroomp - Rooms;

  // Make sure valid rooms are selected
  if (IsRoomSelected(Markedroomp - Rooms)) {
    RestoreRoomSelectedList();
    OutrageMessageBox("Cannot rotate: rooms connect back to base room.");
    return;
  }

  // Compute point & matrix for rotation
  ComputePortalCenter(&rotpoint, Curroomp, cur_portalnum);
  vm_AnglesToMatrix(&rotmat, p, h, b); // get desired rotation
  face *fp = &Curroomp->faces[Curroomp->portals[cur_portalnum].portal_face];
  ComputeNormal(&portal_normal, fp->num_verts, fp->face_verts, Curroomp->verts);
  portal_normal *= -1.0; // reverse it!

  vm_VectorToMatrix(&roommat, &portal_normal, NULL, NULL);
  rotmat = roommat * ~rotmat * ~roommat; // get rotation relative to start room

  // Now rotate all the points

  for (i = 0; i < N_selected_rooms; i++) {
    room *rp = &Rooms[Selected_rooms[i]];

    for (v = 0; v < rp->num_verts; v++) {
      if (rp == Curroomp) {
        face *fp = &Curroomp->faces[Curroomp->portals[cur_portalnum].portal_face];
        for (t = 0; t < fp->num_verts; t++)
          if (v == fp->face_verts[t])
            goto Skipit;
      }
      rp->verts[v] = ((rp->verts[v] - rotpoint) * rotmat) + rotpoint;

    Skipit:;
    }

    // Rotate all the normals
    for (t = 0; t < rp->num_faces; t++)
      rp->faces[t].normal = rp->faces[t].normal * rotmat;
  }

  // Now I guess we're done
  RestoreRoomSelectedList();

  // Go through face list and build a list of faces that touch the vertices
  // that make up the portal we're modifying...
  fp = &Curroomp->faces[Curroomp->portals[cur_portalnum].portal_face];
  for (i = 0; i < fp->num_verts; i++) {
    int checkvert = fp->face_verts[i];

    for (t = 0; t < Curroomp->num_faces; t++) {
      face *fp = &Curroomp->faces[t];

      for (int l = 0; l < Curroomp->num_portals; l++)
        if (Curroomp->portals[l].portal_face == t)
          goto skipface;

      for (v = 0; v < fp->num_verts; v++) {
        if (fp->face_verts[v] == checkvert) {
          for (k = 0; k < checkcount; k++) {
            if (checkfaces[k] == t)
              break;
          }
          if (k == checkcount) {
            checkfaces[checkcount] = t;
            checkcount++;
          }
        }
      }
    skipface:;
    }
  }

  // Now fix them!
  if (checkcount > 0)
    FixConcaveFaces(Curroomp, checkfaces, checkcount);

  // If the rotated seg is floating, recompute uvs for connected seg
  // CalcFloatUVs(&Segments[baseseg],baseside);

  // Set flags
  World_changed = 1;

  return;
}

/*
//Move a floating segment (and all the segments attached to it)
//Parameters:	dx,dy - how much to move the segment, releative to its right & up vectors
void MoveFloatSegment(float dx,float dy)
{
        uint8_t			selected_vert[MAX_VERTICES];
        matrix		segmat;
        vector		deltav;
        int			baseseg,baseside;
        int			s,v,i;
        segment		*segp;

        if (! (Cursegp->sides[Curside].flags & SDF_FLOATING)) {
                OutrageMessageBox("Curseg/Curside must be floating for this operation.");
                return;
        }

        //Save the list so we can use it
        SaveSelectedList();

        //Select all segments for rotation
        baseseg = Cursegp->children[Curside];
        Cursegp->children[Curside] = -1;				//detach base
        SelectConnectedSegs(Cursegp-Segments);
        Cursegp->children[Curside] = baseseg;		//re-attach base
        baseside = FindConnectSide(Cursegp,&Segments[baseseg]);

        //Make sure valid segments are selected
        if (IsSelected(baseseg)) {
                RestoreSelectedList();
                OutrageMessageBox("Cannot rotate: segments connect back to base segment.");
                return;
        }

        //Flag vertices to rotate
        for (v=0;v<MAX_VERTICES;v++)
                selected_vert[v] = 0;

        for (i=0;i<N_selected_segs;i++) {
                segp = &Segments[Selected_segs[i]];
                for (v=0;v<VPS;v++)
                        selected_vert[segp->verts[v]] = 1;
        }

        //Check and fix any selected verts used by non-selected segments
        for (s=0,segp=Segments;s<=Highest_segment_index;s++,segp)
                if (! segp->flags & SF_UNUSED_SEGMENT) {
                        for (v=0;v<VPS;v++) {
                                int vertnum = segp->verts[v];
                                if (selected_vert[vertnum]) {		 	//shared vert!
                                        int new_vertnum;
                                        new_vertnum = AddDuplicateVertex(&Vertices[segp->verts[v]]);
                                        selected_vert[vertnum] = 0;			//old is not in list
                                        selected_vert[new_vertnum] = 1;		//new is in list

                                        //Go through selected list, & remap verts
                                        for (i=0;i<N_selected_segs;i++) {
                                                segment *segp = &Segments[Selected_segs[i]];
                                                for (v=0;v<VPS;v++)
                                                        if (segp->verts[v] == vertnum)
                                                                segp->verts[v] = new_vertnum;
                                        }
                                }
                        }
                }

        //Compute the delta vector
        ExtractMatrixFromSeg(Cursegp,Side_opposite[Curside],&segmat);
        deltav = segmat.uvec * dy + segmat.rvec * dx;

        //Now translate all the points
        for (v=0;v<MAX_VERTICES;v++)
                if (selected_vert[v])
                        Vertices[v] += deltav;

        //Now move all the objects in the segments
        for (s=0;s<N_selected_segs;s++) {
                int o;
                for (o=Segments[Selected_segs[s]].objects;o!=-1;o=Objects[o].next)
                        Objects[o].pos += deltav;
        }

        //Now I guess we're done
        RestoreSelectedList();

        //Fix up modified segment
        ValidateSegment(Cursegp);

        //Warn user if segment broken
        WarnIfConcaveSegment(Cursegp);

        //If the rotated seg is floating, recompute uvs for connected seg
        CalcFloatUVs(&Segments[baseseg],baseside);

        //Set flags
        World_changed = 1;

        return;
}


//	Rotates a segment
//	segment must have 1 and only 1 connection for this to work.
int RotateSegment(segment *seg, angle p, angle h, angle b)
{
        vector rotpoint;
        matrix segmat;
        matrix rotmat;
        int v, s, count, attside, rotside;
        int o,side;

// Find side of attachment and side to be rotated.
        count = 0;
        for (s=0; s<SIDES_PER_SEGMENT; s++)
                if (IS_CHILD(seg->children[s])) {
                        count++;
                        attside = s;
                }

//	rotside = 0;
//	while ((destseg->children[destside] != seg-Segments) && (destside < MAX_SIDES_PER_SEGMENT))
//		destside++;
        rotside = Side_opposite[attside];

// Return if passed in segment is connected to other than 1 segment.
        if (count != 1) return 1;

        vm_AnglesToMatrix(&rotmat, p,h,b);

        ComputeCenterPointOnSide(&rotpoint,seg,attside);
        ExtractMatrixFromSeg(seg,rotside,&segmat);		//get this segment's orientation
        rotmat = segmat * ~rotmat * ~segmat;				//get rotation relative to start segment

//	remap free vertices to the new orientation
        for (v = 0; v < 4; v++)
        {
                Vertices[seg->verts[Side_to_verts[rotside][v]]] =
                        ((Vertices[seg->verts[Side_to_verts[rotside][v]]] - rotpoint) * rotmat) + rotpoint;
        }

//	Now rotate the surface normals and the objects in the segments
        for (o=seg->objects;o!=-1;o=Objects[o].next)
                Objects[o].pos = ((Objects[o].pos - rotpoint) * rotmat) + rotpoint;
        for (side=0;side<SPS;side++)
        {
                seg->sides[side].normals[0] = seg->sides[side].normals[0] * rotmat;
                seg->sides[side].normals[1] = seg->sides[side].normals[1] * rotmat;
        }

//	Fix up modified segment
        ValidateSegment(seg);

//	Warn user if segment broken
        WarnIfConcaveSegment(seg);

//	Set flags
        World_changed = 1;

        return 0;
}
*/
