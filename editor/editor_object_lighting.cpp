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

#include <algorithm>

#include "3d.h"
#include "texture.h"
#include "gametexture.h"
#include "erooms.h"
#include "editor_lighting.h"
#include "descent.h"
#include "room.h"
#include "lightmap.h"
#include "polymodel.h"
#include "objinfo.h"
#include <string.h>
#include <stdlib.h>
#include "terrain.h"
#include "radiosity.h"
#include "lighting.h"
#include "findintersection.h"
#include "lightmap_info.h"
#include "object_lighting.h"
#include "mem.h"

void ComputeObjectSurfaceRes (rad_surface *surf,object *obj,int subnum,int facenum)
{
	int i;
	float left=1.1f,right=-1,top=1.1f,bottom=-1;
	lightmap_object_face *lfp=&obj->lm_object.lightmap_faces[subnum][facenum];
	int lw=lmi_w(lfp->lmi_handle);
	int lh=lmi_h(lfp->lmi_handle);

	for (i=0;i<lfp->num_verts;i++)
	{
		if (lfp->u2[i]<left)
			left=lfp->u2[i];
		if (lfp->u2[i]>right)
			right=lfp->u2[i];
		if (lfp->v2[i]<top)
			top=lfp->v2[i];
		if (lfp->v2[i]>bottom)
			bottom=lfp->v2[i];
	}

	float left_result=(left*lw)+.0001;
	float right_result=(right*lw)+.0001;
	float top_result=(top*lh)+.0001;
	float bottom_result=(bottom*lh)+.0001;

	surf->x1=floor (left_result);
	surf->x2=floor (right_result);

	surf->y1=floor(top_result);
	surf->y2=floor(bottom_result);
	
	surf->xresolution=(surf->x2-surf->x1);
	surf->yresolution=(surf->y2-surf->y1);

	// Adjust for a accuracy errors
	if ( ( (right_result)-(float)surf->x2)>.005)
		surf->xresolution++;

	if ( ( (bottom_result)-(float)surf->y2)>.005)
		surf->yresolution++;

	if ( ( (top_result)-(float)surf->y1)>.99)
		surf->y1++;

	if ( ( (left_result)-(float)surf->x1)>.99)
		surf->x1++;

}


void ApplyLightmapToObjectSurface (object *obj,int subnum,int facenum,rad_surface *sp)
{
	lightmap_object_face *fp=&obj->lm_object.lightmap_faces[subnum][facenum];
	int i,t,lmi_handle;
	int xres,yres;
	int lw,lh;
	int x1=sp->x1;
	int y1=sp->y1;

	xres=sp->xresolution;
	yres=sp->yresolution;

	ASSERT (fp->lmi_handle!=BAD_LMI_INDEX);
	lmi_handle=fp->lmi_handle;

	lw=lmi_w(lmi_handle);
	lh=lmi_h(lmi_handle);

	ASSERT ((xres+x1)<=lw);
	ASSERT ((yres+y1)<=lh);

	ASSERT (lw>=2);
	ASSERT (lh>=2);

	uint16_t *dest_data=lm_data (LightmapInfo[lmi_handle].lm_handle);

	for (i=0;i<yres;i++)
	{
		for (t=0;t<xres;t++)
		{
			if (!(sp->elements[i*xres+t].flags & EF_IGNORE))
			{
				ddgr_color color=GR_16_TO_COLOR(dest_data[(i+y1)*lw+(t+x1)]);
				int red=GR_COLOR_RED(color);
				int green=GR_COLOR_GREEN(color);
				int blue=GR_COLOR_BLUE(color);

				float fr,fg,fb;

				if (!(dest_data[(i+y1)*lw+(t+x1)] & OPAQUE_FLAG))
				{
					red=green=blue=0;
				}
				
				fr=std::min(1.0f,sp->elements[i*xres+t].exitance.r+Ambient_red);
				fg=std::min(1.0f,sp->elements[i*xres+t].exitance.g+Ambient_green);
				fb=std::min(1.0f,sp->elements[i*xres+t].exitance.b+Ambient_blue);
				
				fr=(fr*255)+.5;
				fg=(fg*255)+.5;
				fb=(fb*255)+.5;
				
				red+=(int)fr;
				green+=(int)fg;
				blue+=(int)fb;
								
				if (dest_data[(i+y1)*lw+(t+x1)] & OPAQUE_FLAG)
				{
					
					red/=2;
					green/=2;
					blue/=2;
				}
			
				red=std::min(red,255);
				green=std::min(green,255);
				blue=std::min(blue,255);
				
				dest_data[(i+y1)*lw+(t+x1)]=OPAQUE_FLAG|GR_RGB16(red,green,blue);
			}
		}
	}
}

void GetPointInObjectSpace (vector *dest,vector *pos,object *obj,int subnum,int world)
{
	poly_model *pm=&Poly_models[obj->rtype.pobj_info.model_num];
	bsp_info *sm=&pm->submodel[subnum];
	float normalized_time[MAX_SUBOBJECTS];
	int i;
	int rotate_list[MAX_SUBOBJECTS];
	int num_to_rotate=0;

	if (!pm->new_style)
		return;

	for (i=0;i<MAX_SUBOBJECTS;i++)
		normalized_time[i]=0.0;

	SetModelAnglesAndPos (pm,normalized_time);
	
	vector pnt    = *pos;
	int mn     = subnum;
	vector tpnt;
	matrix m;

	while (mn!=-1) 
	{
		rotate_list[num_to_rotate]=mn;
		num_to_rotate++;
		mn=pm->submodel[mn].parent;
	}

	// Subtract and rotate position
	if (world)
		tpnt = pnt - obj->pos;
	else
		tpnt=pnt;
	
	pnt = tpnt * obj->orient;

	for (i=num_to_rotate-1;i>=0;i--)
	{
		// Subtract and rotate position for this submodel
		mn=rotate_list[i];


		if (world)
			tpnt = pnt - pm->submodel[mn].offset;
		else
			tpnt=pnt;
	
		vm_AnglesToMatrix(&m, pm->submodel[mn].angs.p,pm->submodel[mn].angs.h, pm->submodel[mn].angs.b);

		pnt = tpnt * m;
	}


	*dest=pnt;
		
}

// Goes through all objects and fills in the lightmap data for them
void AssignLightmapsToObjectSurfaces (int surface_index,int terrain)
{
	int i,t,j;
	uint8_t rotated[MAX_LIGHTMAP_INFOS];

	memset (rotated,0,MAX_LIGHTMAP_INFOS);

	for (i=0;i<=Highest_object_index;i++)
	{
		if ((terrain != 0) != (OBJECT_OUTSIDE(&Objects[i]) != 0))
			continue;

		if (Objects[i].type!=OBJ_NONE && Objects[i].lighting_render_type==LRT_LIGHTMAPS)
		{
			poly_model *po=&Poly_models[Objects[i].rtype.pobj_info.model_num];

			if (!po->new_style)
				continue;

			for (t=0;t<po->n_models;t++)
			{
				bsp_info *sm=&po->submodel[t];
				
				if (IsNonRenderableSubmodel (po,t))
					continue;

				for (j=0;j<sm->num_faces;j++,surface_index++)
				{	
					ApplyLightmapToObjectSurface (&Objects[i],t,j,&Light_surfaces[surface_index]);
						
					// Rotate the lightmap  upper left 
					object *obj=&Objects[i];
					lightmap_object_face *fp=&obj->lm_object.lightmap_faces[t][j];
					lightmap_info *lmi_ptr=&LightmapInfo[fp->lmi_handle];
				
					if (!rotated[fp->lmi_handle])
					{
						vector uleft,rvec,uvec,norm;
						GetPointInObjectSpace (&uleft,&lmi_ptr->upper_left,obj,t,1);
						GetPointInObjectSpace (&norm,&lmi_ptr->normal,obj,t,0);
						lmi_ptr->normal=norm;
						lmi_ptr->upper_left=uleft;

						GetPointInObjectSpace (&rvec,&ScratchRVecs[fp->lmi_handle],obj,t,0);
						GetPointInObjectSpace (&uvec,&ScratchUVecs[fp->lmi_handle],obj,t,0);
												
						rotated[fp->lmi_handle]=1;

						// Find all the faces in this submodel that have this lightmap info handle
						for (int k=0;k<sm->num_faces;k++)
						{
							lightmap_object_face *this_fp=&obj->lm_object.lightmap_faces[t][k];
							if (fp->lmi_handle==this_fp->lmi_handle)
							{
								this_fp->rvec=rvec;
								this_fp->uvec=uvec;
							}
						}
						
					}
					
				}
			}
		}
	}
}

// Goes through all objects int a room and fills in the lightmap data for them
void AssignLightmapsToObjectSurfacesForSingleRoom (int surface_index,int roomnum)
{
	int i,t,j;

	for (i=0;i<=Highest_object_index;i++)
	{

		if (Objects[i].type!=OBJ_NONE && Objects[i].lighting_render_type==LRT_LIGHTMAPS && Objects[i].roomnum==roomnum)
		{
			poly_model *po=&Poly_models[Objects[i].rtype.pobj_info.model_num];

			if (!po->new_style)
				continue;

			for (t=0;t<po->n_models;t++)
			{
				bsp_info *sm=&po->submodel[t];

				if (IsNonRenderableSubmodel (po,t))
					continue;

				for (j=0;j<sm->num_faces;j++,surface_index++)
					ApplyLightmapToObjectSurface (&Objects[i],t,j,&Light_surfaces[surface_index]);
			}
		}
	}
}


// Sets up radiosity surfaces for objects in the mine
// Returns the number of new surfaces
int ComputeSurfacesForObjects (int surface_index,int terrain)
{
	int i,t,j;

	for (i=0;i<=Highest_object_index;i++)
	{
		if ((terrain != 0) != (OBJECT_OUTSIDE(&Objects[i]) != 0))
			continue;

		if (Objects[i].type!=OBJ_NONE && Objects[i].lighting_render_type==LRT_LIGHTMAPS)
		{
			poly_model *po=&Poly_models[Objects[i].rtype.pobj_info.model_num];

			if (!po->new_style)
				continue;

			SetupObjectLightmapMemory (&Objects[i]);

			if (terrain)
				CombineObjectLightmapUVs (&Objects[i],LMI_TERRAIN_OBJECT);
			else
				CombineObjectLightmapUVs (&Objects[i],LMI_ROOM_OBJECT);
			
			for (t=0;t<po->n_models;t++)
			{
				bsp_info *sm=&po->submodel[t];

				if (IsNonRenderableSubmodel (po,t))
					continue;				

				for (j=0;j<sm->num_faces;j++,surface_index++)
				{
					ComputeObjectSurfaceRes (&Light_surfaces[surface_index],&Objects[i],t,j);

					if (sm->faces[j].nverts>0)
					{
						Light_surfaces[surface_index].verts=(vector *)mem_malloc (sm->faces[j].nverts*sizeof(vector));
						ASSERT (Light_surfaces[surface_index].verts!=NULL);
					}
					else
						Light_surfaces[surface_index].verts=NULL;

					if (Light_surfaces[surface_index].xresolution*Light_surfaces[surface_index].yresolution>0)
					{
						Light_surfaces[surface_index].elements=(rad_element *)mem_malloc (Light_surfaces[surface_index].xresolution*Light_surfaces[surface_index].yresolution*sizeof(rad_element));
						ASSERT (Light_surfaces[surface_index].elements!=NULL);
					}
					else
						Light_surfaces[surface_index].elements=NULL;

					Light_surfaces[surface_index].flags=0;
									
					if (sm->faces[j].texnum==-1)
					{
						Light_surfaces[surface_index].emittance.r=0;
						Light_surfaces[surface_index].emittance.g=0;
						Light_surfaces[surface_index].emittance.b=0;
						Light_surfaces[surface_index].reflectivity=.5;
					}
					else
					{
						Light_surfaces[surface_index].emittance.r=(float)GameTextures[po->textures[sm->faces[j].texnum]].r;
						Light_surfaces[surface_index].emittance.g=(float)GameTextures[po->textures[sm->faces[j].texnum]].g;
						Light_surfaces[surface_index].emittance.b=(float)GameTextures[po->textures[sm->faces[j].texnum]].b;
						Light_surfaces[surface_index].reflectivity=GameTextures[po->textures[sm->faces[j].texnum]].reflectivity;
						if ((GetMaxColor (&Light_surfaces[surface_index].emittance))>.005)
							Light_surfaces[surface_index].flags|=SF_LIGHTSOURCE;
						
					}
					
					if (terrain)
						Light_surfaces[surface_index].surface_type=ST_TERRAIN_OBJECT;
					else
						Light_surfaces[surface_index].surface_type=ST_ROOM_OBJECT;
					
					Light_surfaces[surface_index].normal=LightmapInfo[Objects[i].lm_object.lightmap_faces[t][j].lmi_handle].normal;
					Light_surfaces[surface_index].roomnum=Objects[i].roomnum;
					
					if (Light_surfaces[surface_index].surface_type==ST_ROOM_OBJECT)
					{	
						if (Rooms[Objects[i].roomnum].flags & RF_TOUCHES_TERRAIN)
							Light_surfaces[surface_index].flags|=SF_TOUCHES_TERRAIN;

						for (int k=0;k<Rooms[Objects[i].roomnum].num_portals;k++)
						{
							if (Rooms[Objects[i].roomnum].portals[k].croom==-1 || (Rooms[Rooms[Objects[i].roomnum].portals[k].croom].flags & RF_EXTERNAL))
								Light_surfaces[surface_index].flags|=SF_TOUCHES_TERRAIN;
								
						}
					}
																		
					// Set the vertices for each element
					BuildElementListForObjectFace (i,t,j,&Light_surfaces[surface_index]);
				}
			}
		}
	}
		
	return 0;	
}

// Sets up radiosity surfaces for objects in a room
// Returns the number of new surfaces
int ComputeSurfacesForObjectsForSingleRoom (int surface_index,int roomnum)
{
	int i,t,j;

	for (i=0;i<=Highest_object_index;i++)
	{
		

		if (Objects[i].type!=OBJ_NONE && Objects[i].lighting_render_type==LRT_LIGHTMAPS && Objects[i].roomnum==roomnum)
		{
			poly_model *po=&Poly_models[Objects[i].rtype.pobj_info.model_num];

			if (!po->new_style)
				continue;

			SetupObjectLightmapMemory (&Objects[i]);
			CombineObjectLightmapUVs (&Objects[i],LMI_ROOM_OBJECT);
			
			for (t=0;t<po->n_models;t++)
			{
				bsp_info *sm=&po->submodel[t];

				if (IsNonRenderableSubmodel (po,t))
					continue;

				for (j=0;j<sm->num_faces;j++,surface_index++)
				{
					ComputeObjectSurfaceRes (&Light_surfaces[surface_index],&Objects[i],t,j);

					if (sm->faces[j].nverts>0)
					{
						Light_surfaces[surface_index].verts=(vector *)mem_malloc (sm->faces[j].nverts*sizeof(vector));
						ASSERT (Light_surfaces[surface_index].verts!=NULL);
					}
					else
						Light_surfaces[surface_index].verts=NULL;

					if (Light_surfaces[surface_index].xresolution*Light_surfaces[surface_index].yresolution>0)
					{
						Light_surfaces[surface_index].elements=(rad_element *)mem_malloc (Light_surfaces[surface_index].xresolution*Light_surfaces[surface_index].yresolution*sizeof(rad_element));
						ASSERT (Light_surfaces[surface_index].elements!=NULL);
					}
					else
						Light_surfaces[surface_index].elements=NULL;
									
					if (sm->faces[j].texnum==-1)
					{
						Light_surfaces[surface_index].emittance.r=0;
						Light_surfaces[surface_index].emittance.g=0;
						Light_surfaces[surface_index].emittance.b=0;
						Light_surfaces[surface_index].reflectivity=.5;
					}
					else
					{
						Light_surfaces[surface_index].emittance.r=(float)GameTextures[po->textures[sm->faces[j].texnum]].r;
						Light_surfaces[surface_index].emittance.g=(float)GameTextures[po->textures[sm->faces[j].texnum]].g;
						Light_surfaces[surface_index].emittance.b=(float)GameTextures[po->textures[sm->faces[j].texnum]].b;
						Light_surfaces[surface_index].reflectivity=GameTextures[po->textures[sm->faces[j].texnum]].reflectivity;
						
					}
					
					Light_surfaces[surface_index].surface_type=ST_ROOM_OBJECT;
					
					Light_surfaces[surface_index].normal=LightmapInfo[Objects[i].lm_object.lightmap_faces[t][j].lmi_handle].normal;
					Light_surfaces[surface_index].roomnum=Objects[i].roomnum;
																		
					// Set the vertices for each element
					BuildElementListForObjectFace (i,t,j,&Light_surfaces[surface_index]);
				}
			}
		}
	}
		
	return 0;	
}


// Gets the total number of object faces that exist in a mine
int GetTotalObjectFaces (int terrain)
{
	int i;
	int facecount=0;

	for (i=0;i<=Highest_object_index;i++)
	{
		if (Objects[i].type!=OBJ_NONE)
		{
			if ((terrain != 0) != (OBJECT_OUTSIDE(&Objects[i]) != 0))
				continue;

			if (Objects[i].lighting_render_type==LRT_LIGHTMAPS)
			{
				poly_model *po=&Poly_models[Objects[i].rtype.pobj_info.model_num];
				
				if (!po->new_style)
					continue;
			
				facecount+=CountFacesInPolymodel (po);
			}
		}
	}

	return facecount;
}


// Gets the total number of object faces that exist in a room
int GetTotalObjectFacesForSingleRoom (int roomnum)
{
	int i;
	int facecount=0;

	for (i=0;i<=Highest_object_index;i++)
	{
		if (Objects[i].type!=OBJ_NONE)
		{
			if (Objects[i].roomnum!=roomnum)
				continue;

			if (Objects[i].lighting_render_type==LRT_LIGHTMAPS)
			{
				poly_model *po=&Poly_models[Objects[i].rtype.pobj_info.model_num];
				
				if (!po->new_style)
					continue;
			
				facecount+=CountFacesInPolymodel (po);
			}
		}
	}

	return facecount;
}

void BuildObjectLightmapUVs (object *obj,int *sublist,int *facelist,int count,vector *lightmap_poly,int nv,int lm_type)
{
	matrix face_matrix,trans_matrix;
	vector fvec;
	vector avg_vert;
	vector verts[MAX_VERTS_PER_FACE*5];
	vector facevert;
	vector rot_vert;
	int i,t;
	int lmi_handle;
	vector world_verts[32];
	
	poly_model *pm=&Poly_models[obj->rtype.pobj_info.model_num];

	for (i=0;i<pm->submodel[sublist[0]].faces[facelist[0]].nverts;i++)
		GetObjectPointInWorld (&world_verts[i],obj,sublist[0],pm->submodel[sublist[0]].faces[facelist[0]].vertnums[i]);

	// find the center point of this face
	vm_MakeZero (&avg_vert);
	for (i=0;i<nv;i++)
		avg_vert+=lightmap_poly[i];

	avg_vert/=nv;

	// Make the orientation matrix
	// Reverse the normal because we're looking "at" the face, not from it

	vm_GetNormal(&fvec,&world_verts[0],&world_verts[1],&world_verts[2]);
	fvec=-fvec;
	
	if ((vm_NormalizeVector(&fvec))!=0)
		vm_VectorToMatrix(&face_matrix,&fvec,NULL,NULL);
	else
		vm_MakeIdentity (&face_matrix);
	// Make the transformation matrix 
	
	angvec avec;	
	vm_ExtractAnglesFromMatrix(&avec,&face_matrix);
	vm_AnglesToMatrix (&trans_matrix,avec.p,avec.h,avec.b);
	
	// Rotate all the points
	for (i=0;i<nv;i++)
	{
		vector vert=lightmap_poly[i];
		
		vert-=avg_vert;
		vm_MatrixMulVector (&rot_vert,&vert,&trans_matrix);

		verts[i]=rot_vert;
	}

	// Find left most point
	int leftmost_point=-1;
	float leftmost_x=900000.00f;	// a big number

	for (i=0;i<nv;i++)
	{
		if (verts[i].x<leftmost_x)
		{
			leftmost_point=i;
			leftmost_x=verts[i].x;
		}
	}

	ASSERT (leftmost_point!=-1);

	// Find top most point
	int topmost_point=-1;
	float topmost_y=-900000.0f;	// a big number

	for (i=0;i<nv;i++)
	{
		if (verts[i].y>topmost_y)
		{
			topmost_point=i;
			topmost_y=verts[i].y;
		}
	}

	ASSERT (topmost_point!=-1);

	// Find right most point
	int rightmost_point=-1;
	float rightmost_x=-900000.00f;	// a big number

	for (i=0;i<nv;i++)
	{
		if (verts[i].x>rightmost_x)
		{
			rightmost_point=i;
			rightmost_x=verts[i].x;
		}
	}

	ASSERT (rightmost_point!=-1);

	// Find bottom most point
	int bottommost_point=-1;
	float bottommost_y=900000.0f;	// a big number

	for (i=0;i<nv;i++)
	{
		if (verts[i].y<bottommost_y)
		{
			bottommost_point=i;
			bottommost_y=verts[i].y;
		}
	}

	ASSERT (bottommost_point!=-1);

	// now set the base vertex, which is where we base uv 0,0 on

	vector base_vector;

	base_vector.x=verts[leftmost_point].x;
	base_vector.y=verts[topmost_point].y;
	base_vector.z=0;

		// Figure out lightmap resolution
	float xdiff=verts[rightmost_point].x-verts[leftmost_point].x;
	float ydiff=verts[topmost_point].y-verts[bottommost_point].y;
	float max_diff=(float)std::max(xdiff,ydiff);
	
	int lightmap_x_res=-1,lightmap_y_res=-1;
	float xspacing=LightSpacing;
	float yspacing=LightSpacing;
	float spacing=LightSpacing;
	int res,done_spacing=0;
	int xspace_int,yspace_int;
	
	// If the default spacing would make us go over our lightmap resolution
	// limit, then increase the spacing and try again
	while (!done_spacing)
	{
		res=(xdiff/xspacing);
		if (((xdiff/xspacing)-res)>0)
			res++;

		res++;
	
		if (res>126)
			xspacing+=1;
		else
			done_spacing=1;
	}

	// Set a mininum, at least
	if (res<2)
		res=2;

	lightmap_x_res=res;

	done_spacing=0;
	while (!done_spacing)
	{
		res=(ydiff/yspacing);
		if (((ydiff/yspacing)-res)>0)
			res++;

		res++;
	
		if (res>126)
			yspacing+=1;
		else
			done_spacing=1;
	}

	// Set a mininum, at least
	if (res<2)
		res=2;

	lightmap_y_res=res;

/*
	// Find power of 2 number
	for (i=0;i<=7;i++)
	{
		int low_num=1<i;
		int hi_num=2<<i;
		if (res<=hi_num && res>low_num)	
		{
			lightmap_res=hi_num;
			break;
		}
	}*/

	lmi_handle=AllocLightmapInfo (lightmap_x_res,lightmap_y_res,lm_type);
	ASSERT (lmi_handle!=BAD_LMI_INDEX);

	// Now do best fit spacing
	if (BestFit)
	{
		xspace_int=(xdiff/lightmap_x_res);
		if ((xdiff-(lightmap_x_res*xspace_int))>0)
			xspace_int++;

		yspace_int=(ydiff/lightmap_y_res);
		if ((ydiff-(lightmap_y_res*yspace_int))>0)
			yspace_int++;
	}
	else
	{
		xspace_int=xspacing;
		yspace_int=yspacing;
	}
	
	// Figure out lightmap uvs

	// Rotate all the face points
	for (i=0;i<count;i++)
	{
		obj->lm_object.lightmap_faces[sublist[i]][facelist[i]].lmi_handle=lmi_handle;
		bsp_info *sm=&pm->submodel[sublist[i]];
		polyface *fp=&sm->faces[facelist[i]];
		lightmap_object_face *lfp=&obj->lm_object.lightmap_faces[sublist[i]][facelist[i]];

		for (t=0;t<fp->nverts;t++)
			GetObjectPointInWorld (&world_verts[t],obj,sublist[i],fp->vertnums[t]);
							
		for (t=0;t<fp->nverts;t++)
		{
			vector vert=world_verts[t];
		
			vert-=avg_vert;
			vm_MatrixMulVector (&rot_vert,&vert,&trans_matrix);

			facevert=rot_vert;

			// Find uv2s for this vertex
			lfp->u2[t]=(facevert.x-verts[leftmost_point].x)/(float)(lightmap_x_res*xspace_int);
			lfp->v2[t]=fabs((verts[topmost_point].y-facevert.y))/(float)(lightmap_y_res*yspace_int);

			ASSERT (lfp->u2[t]>=0 && lfp->u2[t]<=1.0);
			ASSERT (lfp->v2[t]>=0 && lfp->v2[t]<=1.0);
		}
	}

	// Find upper left corner
	vm_TransposeMatrix (&trans_matrix);
	vm_MatrixMulVector (&rot_vert,&base_vector,&trans_matrix);
	LightmapInfo[lmi_handle].upper_left=rot_vert+avg_vert;
	
	LightmapInfo[lmi_handle].xspacing=xspace_int;
	LightmapInfo[lmi_handle].yspacing=yspace_int;
	LightmapInfo[lmi_handle].normal=-fvec;
	ScratchCenters[lmi_handle]=avg_vert;
}

// Important - vertnum is the index into the face_verts[] array in the face structure,
// not an index into the verts[] array of the room structure
void BuildElementListForObjectFace (int objnum,int subnum,int facenum,rad_surface *surf)
{
	matrix face_matrix,trans_matrix;
	vector fvec;
	vector avg_vert;
	vector verts[MAX_VERTS_PER_FACE*5];
	vector rot_vert;
	vector vert;
	vector world_verts[32];
	int i,t;
	int xres,yres;
	int lmi_handle;
	int x1=surf->x1,y1=surf->y1;
	poly_model *pm=&Poly_models[Objects[objnum].rtype.pobj_info.model_num];
	bsp_info *sm=&pm->submodel[subnum];
	polyface *fp=&sm->faces[facenum];

	xres=surf->xresolution;
	yres=surf->yresolution;

	ASSERT (pm->used);
	ASSERT (fp->nverts>=3);
	ASSERT (Objects[objnum].lm_object.lightmap_faces[subnum][facenum].lmi_handle!=BAD_LMI_INDEX);

	ASSERT (fp->nverts<32);

	for (i=0;i<fp->nverts;i++)
		GetObjectPointInWorld (&world_verts[i],&Objects[objnum],subnum,fp->vertnums[i]);

	lmi_handle=Objects[objnum].lm_object.lightmap_faces[subnum][facenum].lmi_handle;
	avg_vert=ScratchCenters[lmi_handle];
	
	// Make the orientation matrix
	// Reverse the normal because we're looking "at" the face, not from it
	fvec=-LightmapInfo[lmi_handle].normal;
	
	if ((vm_NormalizeVector(&fvec))!=0)
		vm_VectorToMatrix(&face_matrix,&fvec,NULL,NULL);
	else
		vm_MakeIdentity (&face_matrix);

	ScratchRVecs[lmi_handle]=face_matrix.rvec;
	ScratchUVecs[lmi_handle]=face_matrix.uvec;
	
	// Make the transformation matrix 
	
	angvec avec;	
	vm_ExtractAnglesFromMatrix(&avec,&face_matrix);
	vm_AnglesToMatrix (&trans_matrix,avec.p,avec.h,avec.b);
	
	// Rotate all the points
	for (i=0;i<fp->nverts;i++)
	{
		vert=world_verts[i];
	
		vert-=avg_vert;
		vm_MatrixMulVector (&rot_vert,&vert,&trans_matrix);

		verts[i]=rot_vert;
	}


	// Find a base vector
	vector base_vector;
	vector xdiff,ydiff;

	vm_MakeZero (&xdiff);
	vm_MakeZero (&ydiff);

	// Rotate our upper left point into our 2d space
	vert=LightmapInfo[lmi_handle].upper_left-avg_vert;
	vm_MatrixMulVector (&base_vector,&vert,&trans_matrix);

	vm_TransposeMatrix (&trans_matrix);
	
	xdiff.x=LightmapInfo[lmi_handle].xspacing;
	ydiff.y=LightmapInfo[lmi_handle].yspacing;
		
	for (i=0;i<yres;i++)
	{
		for (t=0;t<xres;t++)
		{
			int element_index=i*xres+t;
			vector clip_verts[4];
			
			rad_element *ep=&surf->elements[element_index];

			clip_verts[0]=base_vector+(xdiff*(t+x1))-(ydiff*(i+y1));
			clip_verts[1]=base_vector+(xdiff*(t+x1+1))-(ydiff*(i+y1));
			clip_verts[2]=base_vector+(xdiff*(t+x1+1))-(ydiff*(i+y1+1));
			clip_verts[3]=base_vector+(xdiff*(t+x1))-(ydiff*(i+y1+1));
			ClipSurfaceElement (verts,ep,clip_verts,fp->nverts);



			for (int k=0;k<ep->num_verts;k++)
			{
				vm_MatrixMulVector (&rot_vert,&ep->verts[k],&trans_matrix);
				ep->verts[k]=rot_vert+avg_vert;
			}
		}
	}

	if (Square_surfaces)
	{
		surf->verts[0]=base_vector;
		surf->verts[1]=base_vector+(xdiff*xres);
		surf->verts[2]=base_vector+(xdiff*xres)-(ydiff*yres);
		surf->verts[3]=base_vector-(ydiff*yres);

		for (int k=0;k<4;k++)
		{
			vm_MatrixMulVector (&rot_vert,&surf->verts[k],&trans_matrix);
			surf->verts[k]=rot_vert+avg_vert;
		}
		surf->num_verts=4;
	}
	else
	{
		surf->num_verts=fp->nverts;
		for (int k=0;k<surf->num_verts;k++)
		{
			surf->verts[k]=world_verts[k];
		}
	}
}




#define MAX_COMBINES		50
#define LM_ADJACENT_FACE_THRESHOLD	.95
uint8_t *ObjectsAlreadyCombined[MAX_OBJECTS];

// Given a submodel and a face, goes through the entire object and checks to see
// if this face can share a lightmap with any other face
int TestObjectLightAdjacency (object *obj,int subnum,int facenum,int lmi_type)
{
	int i,t,k;	
	poly_model *pm=&Poly_models[obj->rtype.pobj_info.model_num];
	bsp_info *a_sm=&pm->submodel[subnum];
	polyface *afp=&a_sm->faces[facenum];
	vector anormal;

	vector averts[MAX_VERTS_PER_FACE*5];
	vector bverts[MAX_VERTS_PER_FACE*5];
	vector dest_verts[MAX_VERTS_PER_FACE*5];

	int face_combine_list[MAX_COMBINES];
	int submodel_combine_list[MAX_COMBINES];
	
	if (afp->texnum==-1)
		return 0;
	int tex=pm->textures[afp->texnum];

	if (GameTextures[tex].r>0 || GameTextures[tex].g>0 || GameTextures[tex].b>0)
		return 0;

	// Setup our 'base' face

	int anv=afp->nverts;
	int total_faces=1;

	submodel_combine_list[0]=subnum;
	face_combine_list[0]=facenum;

	for (i=0;i<afp->nverts;i++)
		GetObjectPointInWorld (&averts[i],obj,subnum,afp->vertnums[i]);
	vm_GetNormal (&anormal,&averts[0],&averts[1],&averts[2]);
			
	StartOver:

	// Go through each room and find an adjacent face
	for (i=0;i<pm->n_models;i++)
	{
		bsp_info *bsm=&pm->submodel[i];

		if (IsNonRenderableSubmodel (pm,i))
			continue;

		if (bsm!=a_sm)	// only combine faces in the same submodel
			continue;

		for (t=0;t<bsm->num_faces;t++)
		{
			if (total_faces>=MAX_COMBINES-1)
				continue;

			if (bsm==a_sm && t==facenum)
				continue;		// don't do self
				
			// Don't do if already spoken fore
			if (ObjectsAlreadyCombined[i][t])
				continue;

			polyface *bfp=&bsm->faces[t];
			vector bnormal;

			// Don't do combine light sources

			tex=pm->textures[bfp->texnum];
			if (GameTextures[tex].r>0 || GameTextures[tex].g>0 || GameTextures[tex].b>0)
				continue;
		
			for (k=0;k<bfp->nverts;k++)
				GetObjectPointInWorld (&bverts[k],obj,i,bfp->vertnums[k]);

			vm_GetNormal (&bnormal,&bverts[0],&bverts[1],&bverts[2]);
			
			int nv=CombineLightFaces (dest_verts,averts,anv,&anormal,bverts,bfp->nverts,&bnormal);

			// We have a combine!  Mark this face in the appropriate list
			// And update our new polygon
			if (nv>0)
			{
				submodel_combine_list[total_faces]=i;
				face_combine_list[total_faces]=t;
				total_faces++;

				ObjectsAlreadyCombined[subnum][facenum]=1;
				ObjectsAlreadyCombined[i][t]=1;
				for (k=0;k<nv;k++)
					averts[k]=dest_verts[k];
				
				anv=nv;

				goto StartOver;
			}
		}
	}

	// Now build 1 lightmap to be shared across all the faces that were combined
	if (total_faces>1)
	{
		BuildObjectLightmapUVs (obj,submodel_combine_list,face_combine_list,total_faces,averts,anv,lmi_type);
	}

	return 1;
}

// Computes the the mines UVs
// Faces can now share one lightmap, so this routine goes through and tries to
// combine as many faces as it can into one lightmap
void CombineObjectLightmapUVs (object *obj,int lmi_type)
{
	int i,t,k;
	int not_combined=0;

	poly_model *pm=&Poly_models[obj->rtype.pobj_info.model_num];
	ASSERT (obj->lm_object.used);
	
	for (i=0;i<pm->n_models;i++)
	{
		bsp_info *sm=&pm->submodel[i];

		if (IsNonRenderableSubmodel (pm,i))
			continue;
		
		ObjectsAlreadyCombined[i]=(uint8_t *)mem_malloc (sm->num_faces);
		ASSERT (ObjectsAlreadyCombined[i]);
		for (k=0;k<sm->num_faces;k++)
			ObjectsAlreadyCombined[i][k]=0;
	}
	
	for (i=0;i<pm->n_models;i++)
	{
		bsp_info *sm=&pm->submodel[i];

		if (IsNonRenderableSubmodel (pm,i))
			continue;

		for (t=0;t<sm->num_faces;t++)
		{
			if (*(ObjectsAlreadyCombined[i]+t)==0)
				TestObjectLightAdjacency (obj,i,t,lmi_type);
		}
	}

	// Now build lightmaps for any faces that couldn't be combined
	for (i=0;i<pm->n_models;i++)
	{
		bsp_info *sm=&pm->submodel[i];

		if (IsNonRenderableSubmodel (pm,i))
			continue;

		for (t=0;t<sm->num_faces;t++)
		{
			if (!ObjectsAlreadyCombined[i][t])
			{
				vector verts[MAX_VERTS_PER_FACE*5];
				int submodel_list[2],face_list[2];
				for (k=0;k<sm->faces[t].nverts;k++)
				{
					GetObjectPointInWorld (&verts[k],obj,i,sm->faces[t].vertnums[k]);
				}

				submodel_list[0]=i;
				face_list[0]=t;
				BuildObjectLightmapUVs (obj,submodel_list,face_list,1,verts,sm->faces[t].nverts,lmi_type);
				not_combined++;
			}
		}
	}

	mprintf(0,"%d %s faces couldn't be combined!\n",not_combined,pm->name);

	// Free memory
	for (i=0;i<pm->n_models;i++)
	{
		bsp_info *sm=&pm->submodel[i];
		
		if (IsNonRenderableSubmodel (pm,i))
			continue;
		mem_free (ObjectsAlreadyCombined[i]);
	}

}




