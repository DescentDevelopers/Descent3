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

 * $Logfile: /DescentIII/Main/editor/EPath.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:57:38 $
 * $Author: kevinb $
 *
 * Code to create & modify paths
 *
 * $Log: not supported by cvs2svn $
 * 
 * 13    8/20/99 9:43p Gwar
 * changes to InsertNodeIntoPath and DrawAllPaths for NEWEDITOR
 * 
 * 12    8/18/99 7:00a Gwar
 * changes to add EPath.cpp to NEWEDITOR project
 * 
 * 11    4/18/99 5:42a Chris
 * Added the FQ_IGNORE_RENDER_THROUGH_PORTALS flag
 * 
 * 10    2/26/99 6:14p Matt
 * Made new nodes go at the viewer position instead of in front of it, and
 * make the selected path draw in a different color than the others.
 * 
 * 9     2/26/99 12:40p Matt
 * Changed occluded messages to (hopefully) be more understandable
 * 
 * 8     2/26/99 12:26p Matt
 * Fixed spelling error.
 * 
 * 7     2/18/99 2:01p Dan
 * Fixed outside/inside problem
 * 
 * 6     2/08/99 3:29p Chris
 * Added a no orient move for objects
 * 
 * 5     2/03/99 6:56p Chris
 * Added the "Move node to current object" function
 * 
 * 4     10/14/98 2:48p Kevin
 * Changed memory code to comply with mem lib
 * 
 * 3     5/03/98 8:36p Chris
 * Additional debug info
 * 
 * 2     2/10/98 10:48a Matt
 * Moved editor code from gamepath.cpp to epath.cpp
 * 
 * 1     2/10/98 10:13a Matt
 * 
 */

#include <memory.h>

#ifndef NEWEDITOR
	#include "d3edit.h"
#else
	#include "globals.h"
	#include "terrain.h"
	#include "renderer.h"
#endif

#include "epath.h"

#include "object.h"
#include "findintersection.h"

uint8_t Show_paths = 1;

// Allocs a gamepath that a robot will follow.  Returns an index into the GamePaths
// array 
int AllocGamePath ()
{
	int i;

	if(Viewer_object->flags & OF_OUTSIDE_MINE)
	{
		OutrageMessageBox("You must be in a room or on the terrain to create a path");
		return -1;
	}

	for (i=0;i<MAX_GAME_PATHS;i++)
	{
		if (GamePaths[i].used==0)
		{
			GamePaths[i].used=1;
			GamePaths[i].name[0]=0;
			GamePaths[i].num_nodes = 0;
			GamePaths[i].flags=0;

			GamePaths[i].pathnodes=(node *) mem_malloc (MAX_NODES_PER_PATH * sizeof(node));
			mprintf(0, "Path %d got some\n", i);
			
			Num_game_paths++;
			return i;
		}
	}

	OutrageMessageBox("ERROR: Too many paths to add another.");
	return -1;
}

int MovePathNode(int pathnum, int nodenum, vector *delta_pos)
{
	vector attempted_pos = GamePaths[pathnum].pathnodes[nodenum].pos + *delta_pos;

	return MovePathNodeToPos(pathnum, nodenum, &attempted_pos);
}

int MovePathNodeToPos(int pathnum, int nodenum, vector *attempted_pos)
{
	fvi_query fq;
	fvi_info	hit_info;
	
	fq.p0					= &GamePaths[pathnum].pathnodes[nodenum].pos;
	fq.startroom		= GamePaths[pathnum].pathnodes[nodenum].roomnum;
	fq.p1					= attempted_pos;
	fq.rad					= 0.0f;
	fq.thisobjnum			= -1;
	fq.ignore_obj_list	= NULL;
	fq.flags				= FQ_TRANSPOINT | FQ_IGNORE_RENDER_THROUGH_PORTALS;

	fvi_FindIntersection(&fq, &hit_info);

	// Check if new node is valid
	if(nodenum >= 1)
	{
		fvi_query fq1;
		fvi_info	hit_info1;

		fq1.p0					= &GamePaths[pathnum].pathnodes[nodenum - 1].pos;
		fq1.startroom			= GamePaths[pathnum].pathnodes[nodenum - 1].roomnum;
		fq1.p1					= &hit_info.hit_pnt;
  		fq1.rad					= 0.0f;
		fq1.thisobjnum			= -1;
		fq1.ignore_obj_list	= NULL;
  		fq1.flags				= FQ_TRANSPOINT | FQ_IGNORE_RENDER_THROUGH_PORTALS;

		fvi_FindIntersection(&fq1, &hit_info1);
		if(vm_VectorDistance(&hit_info.hit_pnt, &hit_info1.hit_pnt) > .005)
		{
			OutrageMessageBox("Cannot move point.  There is no line of sight from the previous node to the new position.");
			return -1;
		}

	}

	// Check if new node is valid
	if(nodenum < GamePaths[pathnum].num_nodes - 1)
	{
		fvi_query fq1;
		fvi_info	hit_info1;

		fq1.p0					= &GamePaths[pathnum].pathnodes[nodenum + 1].pos;
		fq1.startroom			= GamePaths[pathnum].pathnodes[nodenum + 1].roomnum;
		fq1.p1					= &hit_info.hit_pnt;
  		fq1.rad					= 0.0f;
		fq1.thisobjnum			= -1;
		fq1.ignore_obj_list	= NULL;
  		fq1.flags				= FQ_TRANSPOINT | FQ_IGNORE_RENDER_THROUGH_PORTALS;

		fvi_FindIntersection(&fq1, &hit_info1);
		if(vm_VectorDistance(&hit_info.hit_pnt, &hit_info1.hit_pnt) > .005)
		{
			OutrageMessageBox("Cannot move point.  There is no line of sight from the next node to the new position.");
			return -1;
		}
	}

	GamePaths[pathnum].pathnodes[nodenum].pos = hit_info.hit_pnt;
	GamePaths[pathnum].pathnodes[nodenum].roomnum = hit_info.hit_room;

	return 0;
}

// Given a path number, and a node number in that path, adds another node after the 
// specified node
// Returns the index number of the new node
// If nodenum is -1, this node was not added
// roomnum is only valid if this is the first node in a room
#ifndef NEWEDITOR
int InsertNodeIntoPath (int pathnum,int nodenum,int flags)
#else
int InsertNodeIntoPath (int pathnum,int nodenum,int flags,int roomnum,vector pos,matrix orient)
#endif
{
	int i;
	vector attempted_pos;
	
	fvi_query fq;
	fvi_info	hit_info;

	if(Viewer_object->flags & OF_OUTSIDE_MINE)
	{
		OutrageMessageBox("Error: Must be in mine or on terrain to insert a node.");
		return -1;
	}

	if(GamePaths[pathnum].num_nodes >= MAX_NODES_PER_PATH)
	{
		OutrageMessageBox("Error: Path already has its maximum amount of nodes.");
		return -1;
	}

	// First, move all nodes up one
	for (i=GamePaths[pathnum].num_nodes-1;i>nodenum;i--)
	{
		memcpy (&GamePaths[pathnum].pathnodes[i+1],&GamePaths[pathnum].pathnodes[i],sizeof(node));
	}

#ifndef NEWEDITOR
	attempted_pos = Viewer_object->pos;		// + Viewer_object->orient.fvec * 10.0;
#else
	attempted_pos = pos;		// + Viewer_object->orient.fvec * 10.0;
#endif


#ifndef NEWEDITOR
	fq.p0						= &Viewer_object->pos;
	fq.startroom			= Viewer_object->roomnum;
#else
	fq.p0						= &pos;
	fq.startroom			= roomnum;
#endif
	fq.p1						= &attempted_pos;
  	fq.rad					= 0.0f;
	fq.thisobjnum			= -1;
	fq.ignore_obj_list	= NULL;
  	fq.flags					= FQ_TRANSPOINT | FQ_IGNORE_RENDER_THROUGH_PORTALS;

	fvi_FindIntersection(&fq, &hit_info);

	// Check if new node is valid
	if(nodenum >= 0)
	{
		fvi_query fq1;
		fvi_info	hit_info1;

		fq1.p0						= &GamePaths[pathnum].pathnodes[nodenum].pos;
		fq1.startroom				= GamePaths[pathnum].pathnodes[nodenum].roomnum;
		fq1.p1						= &hit_info.hit_pnt;
  		fq1.rad					= 0.0f;
		fq1.thisobjnum			= -1;
		fq1.ignore_obj_list	= NULL;
  		fq1.flags					= FQ_TRANSPOINT | FQ_IGNORE_RENDER_THROUGH_PORTALS;

		fvi_FindIntersection(&fq1, &hit_info1);
		if(vm_VectorDistance(&hit_info.hit_pnt, &hit_info1.hit_pnt) > .005)
		{
			OutrageMessageBox("Cannot add new point.  There is no line of sight from the previous node.");
			return -1;
		}

	}

	// Check if new node is valid
	if(nodenum >= 0  && nodenum < GamePaths[pathnum].num_nodes - 1)
	{
		fvi_query fq1;
		fvi_info	hit_info1;

		fq1.p0						= &GamePaths[pathnum].pathnodes[nodenum + 1].pos;
		fq1.startroom				= GamePaths[pathnum].pathnodes[nodenum + 1].roomnum;
		fq1.p1						= &hit_info.hit_pnt;
  		fq1.rad					= 0.0f;
		fq1.thisobjnum			= -1;
		fq1.ignore_obj_list	= NULL;
  		fq1.flags					= FQ_TRANSPOINT | FQ_IGNORE_RENDER_THROUGH_PORTALS;

		fvi_FindIntersection(&fq1, &hit_info1);
		if(vm_VectorDistance(&hit_info.hit_pnt, &hit_info1.hit_pnt) > .005)
		{
			OutrageMessageBox("Cannot add new point.  There is no line of sight from the next node.");
			return -1;
		}
	}

	int newnode = nodenum + 1;

	GamePaths[pathnum].pathnodes[newnode].pos = hit_info.hit_pnt;
	GamePaths[pathnum].pathnodes[newnode].roomnum = hit_info.hit_room;

#ifndef NEWEDITOR
	GamePaths[pathnum].pathnodes[newnode].fvec = Viewer_object->orient.fvec;
	GamePaths[pathnum].pathnodes[newnode].uvec = Viewer_object->orient.uvec;
#else
	GamePaths[pathnum].pathnodes[newnode].fvec = orient.fvec;
	GamePaths[pathnum].pathnodes[newnode].uvec = orient.uvec;
#endif

	GamePaths[pathnum].num_nodes++;
	
	GamePaths[pathnum].pathnodes[newnode].flags = flags;

	return newnode;
}


// Given a pathnum and a node index, deletes that node and moves all the following nodes down
// by one
void DeleteNodeFromPath (int pathnum,int nodenum)
{
	int i;

	// First, move all nodes up one
	for (i=nodenum;i<GamePaths[pathnum].num_nodes-1;i++)
	{
		memcpy (&GamePaths[pathnum].pathnodes[i],&GamePaths[pathnum].pathnodes[i+1],sizeof(node));
	}

	GamePaths[pathnum].num_nodes--;
}


// Gets next path from n that has actually been alloced
int GetNextPath (int n)
{
	int i;

	ASSERT (n>=0 && n<MAX_GAME_PATHS);

	if (Num_game_paths==0)
		return -1;

	for (i=n+1;i<MAX_GAME_PATHS;i++)
		if (GamePaths[i].used)
			return i;
	for (i=0;i<n;i++)
		if (GamePaths[i].used)
			return i;

	// this is the only one

	return n;
}

// Gets previous path from n that has actually been alloced
int GetPrevPath (int n)
{
	int i;

	ASSERT (n>=0 && n<MAX_GAME_PATHS);

	if (Num_game_paths==0)
		return -1;

	for (i=n-1;i>=0;i--)
	{
		if (GamePaths[i].used)
			return i;
	}
	for (i=MAX_GAME_PATHS-1;i>n;i--)
	{
		if (GamePaths[i].used)
			return i;
	}

	// this is the only one
	return n;

}

// returns the index of the first path (from 0) alloced
// returns -1 if there are no paths
int GetFirstPath ()
{
	int i;
	for (i=0;i<MAX_GAME_PATHS;i++)
	{
		if (GamePaths[i].used)
			return i;
	}
	return -1;
}

#ifndef NEWEDITOR
#include "editor/d3edit.h"
#endif

#include "gr.h"

void DrawAllPaths (grViewport *vp,vector *viewer_eye,matrix *viewer_orient,float zoom)
{
	int i,current_path_index=0,t;
	g3Point rot_points[300];
	int sort_index[300];

	if (!Show_paths)
		return;

	current_path_index=GetFirstPath();

	for (i=0;i<Num_game_paths;i++,current_path_index=GetNextPath(current_path_index))
	{
		
		game_path *gp=&GamePaths[current_path_index];

#ifndef NEWEDITOR
		int curnode=D3EditState.current_node;
#else
		int curnode=Editor_state.GetCurrentNode();
#endif


		for (t=0;t<gp->num_nodes;t++)
		{
			g3_RotatePoint(&rot_points[t],&gp->pathnodes[t].pos);
			sort_index[t]=t;
		}

#ifndef NEWEDITOR
		ddgr_color path_color = (current_path_index==D3EditState.current_path) ? GR_RGB(255,255,255) : GR_RGB(36,99,238);
#else
		ddgr_color path_color = (current_path_index==Editor_state.GetCurrentPath()) ? GR_RGB(255,255,255) : GR_RGB(36,99,238);
#endif

		for (t=0;t<gp->num_nodes-1;t++)
		{
			g3Point p1=rot_points[t];
			g3Point p2=rot_points[t+1];

			g3_DrawLine (path_color,&p1,&p2);
		}


		for (t=0;t<gp->num_nodes;t++)
		{
			for (int k=0;k<gp->num_nodes;k++)
			{
				if (rot_points[k].p3_vec.z<rot_points[t].p3_vec.z)
				{
					g3Point temp;
					int tindex;


					memcpy (&temp,&rot_points[t],sizeof(g3Point));
					memcpy (&rot_points[t],&rot_points[k],sizeof(g3Point));
					memcpy (&rot_points[k],&temp,sizeof(g3Point));

					tindex=sort_index[t];
					sort_index[t]=sort_index[k];
					sort_index[k]=tindex;
				}
			}

		}
		
		for (t=0;t<gp->num_nodes;t++)
		{
			int color;
			float size;

			#ifdef NEWEDITOR
				ddgr_color oldcolor;

				if (TSearch_on)
				{
					rend_SetPixel (GR_RGB(16,255,16),TSearch_x,TSearch_y);
					oldcolor=rend_GetPixel (TSearch_x,TSearch_y);
				}
			#endif

#ifndef NEWEDITOR
			if (current_path_index==D3EditState.current_path && curnode == sort_index[t])
#else
			if (current_path_index==Editor_state.GetCurrentPath() && curnode == sort_index[t])
#endif
			{
				color=GR_RGB (100,128,255);
				size=1.2f;
			}
			else	
			{
				color=GR_RGB (0,128,160);
				size=1.2f;
			}

			g3_DrawSphere(color,&rot_points[t],size);

			vector fpos = gp->pathnodes[sort_index[t]].pos + gp->pathnodes[sort_index[t]].fvec * (size - size/10.0);
			vector upos = gp->pathnodes[sort_index[t]].pos + gp->pathnodes[sort_index[t]].uvec * (size - size/10.0);

			g3Point frot;
			g3Point urot;

			g3_RotatePoint(&frot,&fpos);
			g3_RotatePoint(&urot,&upos);

			g3_DrawLine (GR_RGB(0,255,0),&rot_points[t], &frot);
			g3_DrawLine (GR_RGB(0,0,255),&rot_points[t], &urot);
			g3_DrawSphere(GR_RGB(0,255,0),&frot,size/10.0);
			g3_DrawSphere(GR_RGB(0,0,255),&urot,size/10.0);

			#ifdef NEWEDITOR
				if (TSearch_on)
				{
					if (rend_GetPixel (TSearch_x,TSearch_y)!=oldcolor)
					{
						TSearch_found_type=TSEARCH_FOUND_NODE;
						TSearch_seg=current_path_index;
						TSearch_face=sort_index[t];
					}
				}
			#endif

			DrawNumber (sort_index[t] + 1,gp->pathnodes[sort_index[t]].pos,size * .5);
		}
	}
}

static float ArrayX[10][20]={	{-1,1,1,-1,-1},
								{-.25, 0.0, 0.0, 0.0, -1.0, 1.0},
								{-1.0, 1.0, 1.0, 1.0, -1.0, 1.0, -1.0, -1.0, -1.0, 1.0},
					            {-1.0, 1.0, 1.0, -1.0, 1.0, 1.0, -.5},
								{-1.0, -1.0, -1.0, 1.0, 1.0, 1.0},
								{-1.0, 1.0, -1.0, -1.0, -1.0, 1.0, 1.0, 1.0, -1.0, 1.0},
								{-1.0, 1.0, -1.0, -1.0, -1.0, 1.0, 1.0, 1.0, -1.0, 1.0},
								{-1.0, 1.0, 1.0, 1.0},
								{-1.0, 1.0, 1.0, 1.0, -1.0, 1.0, -1.0, -1.0, -1.0, 1.0},
								{1.0, -1.0, -1.0, 1.0, 1.0, 1.0}
							};

static float ArrayY[10][20]={	{1,1,-1,-1,1},
								{.75, 1.0, 1.0, -1.0, -1.0, -1.0},
								{1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, -1.0, -1.0, -1.0},
								{1.0, 1.0, -1.0, -1.0, -1.0, 0, 0.0},
								{1.0, 0.0, 0.0, 0.0, 1.0, -1.0},
								{1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, -1.0, -1.0, -1.0},
								{1.0, 1.0, 1.0, -1.0, -1.0, -1.0, -1.0, 0.0, 0.0, 0.0},
								{1.0, 1.0, 1.0, -1.0},
								{1.0, 1.0, 1.0, -1.0, -1.0, -1.0, -1.0, 1.0, 0.0, 0.0},
								{1.0, 1.0, 0.0, 0.0, 1.0, -1.0}
							};

static int NumOfPoints[]={5,6,10,7,6,10,10,4,10,6};

void DrawNumber (int num,vector pos,float size)
{
	g3Point basepnt,rot_pnt[20];
	int num_array[10];
	int i,j;
	int total = num;
	size/=2;

	if(num < 0) 
	{
		return;
	}

	int num_numbers = log10(num) + 1;
	if (num_numbers > 10)
	{
		mprintf(0, "Cannot represent a number with over 10 digits\n");
		Int3();
		return;
	}
	
	for(j = num_numbers - 1; j >= 0; j--)
	{
		num_array[j] = total / (pow(10, j));
		total -= num_array[j] * pow(10,j);
	}	

	for(j = 0; j < num_numbers; j++)
	{
		vector cur_pos; 
		
		if (num_numbers & 0x00000001)
			cur_pos = pos + (2.1 * size * ((num_numbers >> 1) - j)) * Viewer_object->orient.rvec;
		else
			cur_pos = pos + (2.1 * size * ((num_numbers >> 1) - j) - size) * Viewer_object->orient.rvec;

		g3_RotatePoint (&basepnt,&cur_pos);

		for (i=0;i<NumOfPoints[num_array[j]];i++)
		{
			rot_pnt[i]=basepnt;

			rot_pnt[i].p3_vec.x+=(ArrayX[num_array[j]][i]*size);
			rot_pnt[i].p3_vec.y+=(ArrayY[num_array[j]][i]*size);

			rot_pnt[i].p3_flags=0;
			g3_CodePoint (&rot_pnt[i]);
			g3_ProjectPoint (&rot_pnt[i]);
		}

		for (i=0;i<NumOfPoints[num_array[j]]-1;i++)
		{
			g3Point p1,p2;

			p1=rot_pnt[i];
			p2=rot_pnt[i+1];
			
			g3_DrawLine (GR_RGB(255,255,255),&p1,&p2);
		}
	}
}

