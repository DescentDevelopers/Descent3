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

#ifndef EDITOR_LIGHTING_H
#define EDITOR_LIGHTING_H

#include "3d.h"
#include "radiosity.h"
#include "room.h"
#include "object.h"
#include "lightmap_info.h"

extern vector ScratchCenters[MAX_LIGHTMAP_INFOS];
extern vector ScratchRVecs[MAX_LIGHTMAP_INFOS];
extern vector ScratchUVecs[MAX_LIGHTMAP_INFOS];
extern int AllowCombining;

extern float Room_multiplier[];
extern float Room_ambience_r[],Room_ambience_g[],Room_ambience_b[];

void DoRadiosityForRooms ();
void DoRadiosityForTerrain();

void BuildElementListForRoomFace (int roomnum,int facenum,rad_surface *surf);
void BuildElementListForObjectFace (int objnum,int subobj_num,int facenum,rad_surface *surf);

// Allocates and sets a lightmap based on the surface patches given
void AssignRoomSurfaceToLightmap (int roomnum,int facenum,rad_surface *sp);

// Allocates and sets a lightmap based on the surface patches given
void AssignObjectSurfaceToLightmap (int objnum,int sobj_num,int facenum,rad_surface *sp);

// Goes through all objects int a room and fills in the lightmap data for them
void AssignLightmapsToObjectSurfacesForSingleRoom (int surface_index,int roomnum);

int ClipRadPointList (rad_point **src,rad_point **dest,int *nv,int code);


// Clips to a specific plane, putting resulting points in dest and returning number of points in dest
int ClipRadToPlane (int plane,rad_point *src,rad_point *dest,int nv);


// Takes two points and a plane, and clips. 
void ClipRadEdge (int plane_flag,rad_point *on_pnt,rad_point *off_pnt);

void SetRadClipLines (vector *tp,vector *rp,vector *bp,vector *lp);

void ClipSurfaceElement (vector *surf_verts,rad_element *ep,vector *clip_verts,int nv);

void BuildLightmapUVs (int *room_list,int *face_list,int count,vector *lightmap_poly,int nv,int external);

// Returns number of verts in dest if face a can be safely combined with face b
// Returns 0 if not
int CombineLightFaces( vector *dest_verts,vector *averts, int nva, vector *norma,vector *bverts, int nvb,vector *normb,int aroom=-1,int broom=-1);

// Given a roomnumber and a face, goes through the entire mine and checks to see
// if this face can share a lightmap with any other face
int TestLightAdjacency (int roomnum,int facenum,int external);

// Sets up radiosity surfaces for objects in a room
// Returns the number of new surfaces
int ComputeSurfacesForObjectsForSingleRoom (int surface_index,int roomnum);

// Computes the the mines UVs
// Faces can now share one lightmap, so this routine goes through and tries to
// combine as many faces as it can into one lightmap
void ComputeAllRoomLightmapUVs (int external);

// Computes lightmap uv sharing for a single room
void ComputeRoomLightmapUVs (room *rp);

// Sets up radiosity surfaces for objects in the mine
// Returns the number of new surfaces
int ComputeSurfacesForObjects (int surface_index,int terrain);

// Gets the total number of object faces that exist 
int GetTotalObjectFaces (int terrain);

// Gets the total number of object faces that exist in a room
int GetTotalObjectFacesForSingleRoom (int roomnum);

void BuildObjectLightmapUVs (object *obj,int *sublist,int *facelist,int count,vector *lightmap_poly,int nv,int lm_type);

// Goes through all objects and fills in the lightmap data for them
void AssignLightmapsToObjectSurfaces (int surface_index,int terrain);

void CombineObjectLightmapUVs (object *obj,int lmi_type);

// Frees memory for specular lighting
void CleanupSpecularLighting (int external);

// Sets up memory usage for specular lighting
void SetupSpecularLighting (int external);

// Calculates radiosity and sets lightmaps for indoor faces only
void DoRadiosityForCurrentRoom (room *rp);

extern rad_surface *Light_surfaces;
extern rad_surface *Terrain_light_surfaces;

extern uint8_t *TerrainLightSpeedup[];

extern int Square_surfaces;
extern int LightSpacing;
extern int BestFit;
extern float Ambient_red,Ambient_green,Ambient_blue;

#endif