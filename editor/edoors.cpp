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

// Edoors.cpp

#ifndef NEWEDITOR
#include "d3edit.h"
#else
#include "globals.h"
#define texdlg_texture GetCurrentTexture()
#endif

#include "polymodel.h"
#include "erooms.h"
#include "room.h"
#include "door.h"
#include "hroom.h"

#include "pserror.h"


void PlaceDoor(room *baseroomp,int baseface,int placed_door)
{
	poly_model *po=GetPolymodelPointer (GetDoorImage(placed_door));
	int i,t;
	int num_faces=0,num_verts=0;
	int got_shell=0,got_front=0;
	bsp_info *front_sm,*shell_sm;
	room *rp;
	int front_face_index;
	int front_remap_points[30];

	for (i=0;i<po->n_models;i++)
	{
		bsp_info *sm=&po->submodel[i];
		if (sm->flags & SOF_SHELL)
		{
			got_shell=1;
			num_verts+=sm->nverts;
			num_faces+=sm->num_faces;
			shell_sm=&po->submodel[i];
		}
		if (sm->flags & SOF_FRONTFACE)
		{
			got_front=1;
			num_verts+=sm->nverts;
			num_faces++;
			front_sm=&po->submodel[i];
		}
	}
	
	if (!got_shell || !got_front)
	{
		Int3();	// This door is improperly specified in 3dsmax
		return;
	}

	rp = CreateNewRoom(num_verts,num_faces,1);

	ASSERT (rp!=NULL);

	// Create the actual room walls
	int index=0;
	for (i=0;i<shell_sm->nverts;i++,index++)
	{
		rp->verts[index]=shell_sm->verts[i];
	}
	for (i=0;i<front_sm->nverts;i++,index++)
	{
		rp->verts[index]=front_sm->verts[i];
	}

	index=0;

	for (i=0;i<shell_sm->num_faces;i++,index++)
	{
		InitRoomFace(&rp->faces[index],shell_sm->faces[i].nverts);
		
		ASSERT (rp->faces[index].face_verts);
		ASSERT (rp->faces[index].face_uvls);
		
		rp->faces[index].tmap=D3EditState.texdlg_texture;

		for (t=0;t<rp->faces[index].num_verts;t++)
			rp->faces[index].face_verts[t]=shell_sm->faces[i].vertnums[t];
	}

	ASSERT (front_sm->num_faces==1);

	front_face_index=index;
	InitRoomFace(&rp->faces[index],front_sm->faces[0].nverts);
	rp->faces[index].tmap=D3EditState.texdlg_texture;

	// Now find the points on the front face that are the same as the shell
	for (i=0;i<front_sm->nverts;i++)
		front_remap_points[i]=-1;

	vector diff_vec=front_sm->offset-shell_sm->offset;
	
	for (i=0;i<shell_sm->nverts;i++)
		for (t=0;t<front_sm->nverts;t++)
		{
			vector testvec=front_sm->verts[t]+diff_vec;

			if ((PointsAreSame(&shell_sm->verts[i],&testvec))) 
				front_remap_points[t]=i;
		}

	for (i=0;i<front_sm->nverts;i++)
		ASSERT (front_remap_points[i]!=-1);

	for (t=0;t<rp->faces[index].num_verts;t++)
		rp->faces[index].face_verts[t]=front_remap_points[front_sm->faces[0].vertnums[t]];
	
	// Reset some values

	if (! ResetRoomFaceNormals (rp))
		Int3();	//Get Matt

	AssignDefaultUVsToRoom (rp);

	// Now use to the attach room code to actually place this door's room
	PlaceRoom(baseroomp,baseface,rp-Rooms,front_face_index,placed_door);
}