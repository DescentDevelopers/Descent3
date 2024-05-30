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

 * Lighting functions and variables
 *
 */

#ifndef _LIGHTING_H
#define _LIGHTING_H

#include "object.h"
#include "room.h"
#include "terrain.h"

#define MAX_LIGHT 1.0
#define LIGHTMAP_SPACING 5.0

#define VOLUME_SPACING 10.0

#define INVISIBLE_VOLUME_ELEMENT 0x01

#define MAX_SPECULAR_INCREMENTS 4096

extern float Specular_tables[3][MAX_SPECULAR_INCREMENTS];

struct dynamic_lightmap {
  uint16_t *mem_ptr;
  uint8_t used;
};

struct dynamic_face {
  uint16_t lmi_handle;
};

struct dynamic_cell {
  uint16_t cellnum;
  uint8_t r, g, b;
};

// Sets up our dynamic lighting maps
void InitDynamicLighting();

// Returns the class index of a specified lightmap resolution
int GetLightmapClass(int res);

// Returns an index into the Dynamic_lightmaps array.  Index returned is marked as unused
int FindFreeDynamicLightmap(int cl);

// Applies dynamic lighting to the room faces based on the light coming from
// an object
void ApplyLightingToRooms(vector *, int, float, float, float, float, vector *light_direction = NULL,
                          float dot_range = 0);

// Clears the used flag for the dynamic lightmaps
void ClearDynamicLightmaps();

// Changes the terrain shading to approximate lighting
void ApplyLightingToTerrain(vector *pos, int cellnum, float light_dist, float red_scale, float green_scale,
                            float blue_scale, vector *light_direction = NULL, float dot_range = 0);

// Gets the viewable lightmap elements
void FindValidLightmapElements(face *fp, dynamic_face *dynamic_fp, vector *light_pos, float light_dist);
// Does a quad tree algo to find what lightmap elements are viewable from our lightsource
int SearchLightQuadTree(face *fp, vector *rvec, vector *uvec, dynamic_face *dynamic_fp, vector *light_pos,
                        float light_dist);

// Given a float, returns the index into the Ubyte_float_table that this number
// corresponds to
uint8_t Float_to_ubyte(float fnum);

extern float Ubyte_to_float[];

// Draws the shadows that happen to be falling on a particular face
void DoShadowsForFace(room *rp, int facenum);

// Sets the num_timer_faces field in Room structure to correspond to the number of faces
// that have flickering lights
void CountFaceLights();

// Goes through all rooms and all faces and lights any faces that have their timer values
// set
void DoFaceLighting();

// Sets pulse parameters for an entire room
void SetRoomPulse(room *rp, uint8_t pulse_time, uint8_t pulse_offset);

// Returns the total number of bytes needed for volume lighting in this room
int GetVolumeSizeOfRoom(room *rp, int *w = NULL, int *h = NULL, int *d = NULL);

// Returns a lightmap that can be applied for specular lighting
int GetSpecularLightmapForFace(vector *pos, room *rp, face *fp);

// Kills the lighting that a face casts and dampens all the faces that light influences
void DestroyLight(int roomnum, int facenum);

// Adds to our list of destroyable lights that got destroyed this frame
void AddToDestroyableLightList(int roomnum, int facenum);

// Goes through our destroyable light list and actually kills all the lights
void DoDestroyedLightsForFrame();

// Blends all the edges that need blending for this frame
void BlendAllLightingEdges();

#endif
