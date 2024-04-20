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
	ObjectClipboard.cpp
		This file contains the cut,copy,paste, and delete routines used when you right click in the view
*/


#include "stdafx.h"

#include "TextureGrWnd.h"

#include "editor.h"
#include "render.h"
#include "manage.h"
#include "gr.h"	   
#include "gameloop.h"
#include "descent.h"
#include "terrain.h"
#include "HTexture.h"
#include "terraindialog.h"
#include "room.h"
#include <string.h>

#include <assert.h>

#include "mono.h"
#include "3d.h"
#include "gr.h"
#include "gamepath.h"
#include "ObjectClipboard.h"
#include "HObject.h"

object ClipBoardObject;
bool ObjectInBuffer;


// Returns the number if OBJ_PLAYER in the Objects list
int GetNumberPlayerObjs()
{
	int count;
	int i;
	count=0;

	for (i=0;i<=Highest_object_index;i++)
	{
		if (Objects[i].type==OBJ_PLAYER)
			count++;
	}
	mprintf((0,"GetNumberPlayerObjs returns %d\n",count));
	return count;
}


//Makes a copy of the current object to the clipboard object, then deletes this object
//Returns true on success
bool ObjectCut()
{
	mprintf((0,"ObjectCut()\n"));
	if(Cur_object_index!=-1)
	{
		int objnum=Cur_object_index;
		int num_player_objects=GetNumberPlayerObjs();
		if((num_player_objects==1) && Objects[objnum].type==OBJ_PLAYER)
		{
			mprintf((0,"Oops, Can't delete the only player...sorry\n"));
			return false;
		}
		mprintf((0,"Attempting to cut object #%d into buffer...",objnum));
		memcpy(&ClipBoardObject,&Objects[objnum],sizeof(object));
		mprintf((0,"Done!\n"));
		ObjectInBuffer=true;
		HObjectDelete();
		return true;
	}
	return false;
}


//Makes a copy of the current object to the clipboard object
//Returns true on success
bool ObjectCopy()
{
	mprintf((0,"ObjectCopy()\n"));
	if(Cur_object_index!=-1)
	{
		int objnum=Cur_object_index;
		mprintf((0,"Attempting to copy object #%d into buffer...",objnum));
		memcpy(&ClipBoardObject,&Objects[objnum],sizeof(object));
		mprintf((0,"Done!\n"));
		ObjectInBuffer=true;
		return true;
	}
			
	return false;
}


//Just deletes the current object
//Returns true on success
bool ObjectDelete()
{
	mprintf((0,"ObjectDelete()\n"));
	if(Cur_object_index!=-1)
	{
		int objnum=Cur_object_index;
		int num_player_objects=GetNumberPlayerObjs();
		if((num_player_objects==1) && Objects[objnum].type==OBJ_PLAYER)
		{
			mprintf((0,"Oops, Can't delete the only player...sorry\n"));
			return false;
		}
		HObjectDelete();
		return true;
	}
	return false;
}


//Pastes the object in the Clipboard to the middle of the current segment
//Returns true on success
bool ObjectPaste()
{
	mprintf((0,"ObjectPaste()\n"));

	Int3();	//I'm not so sure about this routine -- by copying everything over, it may copy
				//some data it shouldn't, such as script info.

	if(!ObjectInBuffer) return false;

	short next,prev;
	int roomnum;
	vector pos;
	matrix orient;
	vector last_pos;
	float creation_time;
	int handle;

	if(!HObjectPlace(ClipBoardObject.type,ClipBoardObject.id))
	{
		return false;
	}
	mprintf((0,"Saving data...\n"));
	handle=Objects[Cur_object_index].handle;
	next=Objects[Cur_object_index].next;
	prev=Objects[Cur_object_index].prev;
	roomnum=Objects[Cur_object_index].roomnum;
	pos=Objects[Cur_object_index].pos;
	orient=Objects[Cur_object_index].orient;
	last_pos=Objects[Cur_object_index].last_pos;
	creation_time=Objects[Cur_object_index].creation_time;
	
	mprintf((0,"Overwriting with buffer data\n"));
	memcpy(&Objects[Cur_object_index],&ClipBoardObject,sizeof(object));

	mprintf((0,"Restoring data...\n"));
	Objects[Cur_object_index].handle=handle;
	Objects[Cur_object_index].next=next;
	Objects[Cur_object_index].prev=prev;
	Objects[Cur_object_index].roomnum=roomnum;
	Objects[Cur_object_index].pos=pos;
	ObjSetOrient(&Objects[Cur_object_index], &orient);
	Objects[Cur_object_index].last_pos=last_pos;
	Objects[Cur_object_index].creation_time=creation_time;

	return true;
}

