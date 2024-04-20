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

#ifndef TERRAIN_H
#define TERRAIN_H

#include "vecmat.h"
#include "pstypes.h"
#include "3d.h"
#include "grdefs.h"
#include "object.h"

// How far we texture out to...after this distance we draw flat shaded polygons
#define DEFAULT_TEXTURE_DISTANCE 9999

#define TERRAIN_WIDTH 256 // How many cells across
#define TERRAIN_DEPTH 256 // How many cells down
#define TERRAIN_SIZE 16.0 // The size of each segment, must be a power of 2
#define TERRAIN_TEX_WIDTH 32
#define TERRAIN_TEX_DEPTH 32

// LOD STUFF
#define MAX_TERRAIN_LOD 4               // How many levels of detail we support
#define SHUTOFF_LOD_DELTA 800000.0f     // Forces lod engine not to work for a particular cell
#define SHUTOFF_LOD_INVISIBLE 900000.0f // This LOD is totally invisible

#define MAX_LOD_SIZE (1 << (MAX_TERRAIN_LOD - 1))

#define MAX_CELLS_TO_RENDER 8000 // The maximum number of cells we can render in a frame

#define MAX_TERRAIN_HEIGHT 350.0f                              // Max height of our terrain
#define TERRAIN_HEIGHT_INCREMENT (MAX_TERRAIN_HEIGHT / 255.0f) // Incremental jumps
#define DEFAULT_VISIBLE_TERRAIN_DISTANCE 80.0 * TERRAIN_SIZE

// Sky defines
#define MAX_STARS 600    // how many stars in our sky
#define MAX_SATELLITES 5 // max satellites in our sky
#define MAX_HORIZON_PIECES                                                                                             \
  16 // how many segments of the horizon
     // there are around our sphere

// Sky flags
#define TF_STARS 1      // whether or not our terrain is starred
#define TF_SATELLITES 2 // Draw satellites or no?
#define TF_FOG 4        // Draw fog?
#define TF_ROTATE_STARS 8
#define TF_ROTATE_SKY 16

// Satellite flags
#define TSF_HALO 1       // Draw halo?
#define TSF_ATMOSPHERE 2 // Draw atmosphere

// occlusion stuff
#define OCCLUSION_SIZE 16

// Mine/terrain joining
#define MAX_LINK_TILES 4 // how many terrain segments can be attached to mines

// Terrain segment flags
#define TF_DYNAMIC 1
#define TF_SPECIAL_WATER 4 // Water
#define TF_SPECIAL_MINE 8  // This segment has a mine attached to it
#define TF_INVISIBLE 16    // This segment is invisible
#define TFM_REGION_MASK (32 + 64 + 128)
// NOTE: 32 64 and 128 are reserved for AI stuff  (terrain region partitioning)

// Terrain cells are on a fixed grid so they have no x and z positions.  If you want the x and z
// positions you must calculate them yourself: gridx*TERRAIN_SIZE and gridz*TERRAIN_SIZE

typedef struct {
  float y;    // Y position of the lower left corner of the terrain cell
  float mody; // The modified y position of this cell - used for LOD

  ubyte l, r, g, b;

  short objects;      // Index of the first object in this cell
  short texseg_index; // index into the tex_segment array

  ubyte flags;   // various flags
  ubyte lm_quad; // which lightmap quad this index belongs to
  ubyte ypos;    // this is so we don't have to constantly convert
                 // floats to ints when traversing the terrain
                 // it's the integer version of pos.y
  ubyte pad;     // for alignment
} terrain_segment;

typedef struct {
  ubyte rotation;
  short tex_index;
} terrain_tex_segment;

// Data for LOD shutoff code
typedef struct {
  int cellnum;
  float save_delta[MAX_TERRAIN_LOD];
} lodoff;

// Data for the sky spherical map
typedef struct {
  int textured; // 1=use textures, 0=use gouraud shaded polygon

  // The two subscripts correspond to the top, middle, and bottom of the horizon piece
  vector horizon_vectors[MAX_HORIZON_PIECES][6];
  float horizon_u[MAX_HORIZON_PIECES][5];
  float horizon_v[MAX_HORIZON_PIECES][5];

  short dome_texture;

  float radius;
  float rotate_rate;

  ddgr_color sky_color;
  ddgr_color horizon_color;
  ddgr_color fog_color;

  float satellite_r[MAX_SATELLITES];
  float satellite_g[MAX_SATELLITES];
  float satellite_b[MAX_SATELLITES];

  vector star_vectors[MAX_STARS];
  vector satellite_vectors[MAX_SATELLITES];
  ubyte satellite_flags[MAX_SATELLITES];
  float satellite_size[MAX_SATELLITES];

  ubyte num_satellites;
  ubyte num_stars;

  short satellite_texture[MAX_SATELLITES];

  vector lightsource;
  angle lightangle;

  float damage_per_second;
  float fog_scalar;

  int star_color[MAX_STARS];
  int flags;
} terrain_sky;

typedef struct {
  int mine_seg;
  int mine_side;
  int portal_num;
  int terrain_seg;

} link_tile;

typedef struct {
  int terrain_seg;
  ubyte num_segs;
  short mine_segs[50];
} terrain_mine_list;

typedef struct {
  float z;
  ushort right_edge, left_edge, top_edge, bottom_edge; // for fixing tjoint problems
  ubyte right_count, left_count, top_count, bottom_count;
  ushort segment; // what segment to render
  ubyte lod;      // what level of detail: 0=16x16, 1=8x8, 2=4x4, 3=2x2, 4=just this segment (1x1)
  ubyte pad;
} terrain_render_info;

typedef struct {
  vector normal1; // Upper left triangle
  vector normal2; // Lower right triangle
} terrain_normals;

extern ubyte Terrain_dynamic_table[];
extern terrain_normals *TerrainNormals[MAX_TERRAIN_LOD];

// Occlusion data for knowing what to draw
extern ubyte Terrain_occlusion_map[256][32];
extern int Terrain_occlusion_checksum;

extern int Check_terrain_portal;
extern int Terrain_checksum;

// Our lighting maps for the terrain, one for each quadrant (starting at lower left)
extern int TerrainLightmaps[4];

extern int GlobalTransCount, TotalDepth;
extern int TerrainEdgeTest[MAX_TERRAIN_LOD][16];

extern terrain_render_info Terrain_list[];
extern ushort TS_FrameCount;

extern float VisibleTerrainZ;
extern float Terrain_average_height;

extern float Clip_scale_left, Clip_scale_right, Clip_scale_top, Clip_scale_bot;
extern ubyte Terrain_from_mine;

extern float Last_terrain_render_time;

extern terrain_segment Terrain_seg[TERRAIN_WIDTH * TERRAIN_DEPTH];
extern terrain_tex_segment Terrain_tex_seg[TERRAIN_TEX_WIDTH * TERRAIN_TEX_DEPTH];

// first object to render after cell has been rendered (only used for SW renderer)
extern short Terrain_seg_render_objs[];

#define TERRAIN_REGION(x) ((Terrain_seg[0x7FFFFFFF & x].flags & TFM_REGION_MASK) >> 5)

extern terrain_sky Terrain_sky;

#if (defined(EDITOR) || defined(NEWEDITOR))
extern int Editor_LOD_engine_off;
extern bool Terrain_render_ext_room_objs;
#endif

extern int Terrain_LOD_engine_off;

extern float Terrain_texture_distance; // how far we should texture before going to flat shad

extern ubyte TerrainJoinMap[];
extern float *TerrainDeltaBlocks[];
extern ubyte *Terrain_max_height_int[];
extern ubyte *Terrain_min_height_int[];
extern ubyte Fast_terrain;
extern ubyte Flat_terrain;
extern ubyte Show_invisible_terrain;

extern int Camera_direction, Sort_direction;

#if (defined(_DEBUG) || defined(NEWEDITOR))
extern ubyte TerrainSelected[];
extern int Num_terrain_selected;
#endif

extern ushort *Terrain_rotate_list; // which points have been sub/rotated this frame
extern g3Point *World_point_buffer; // Rotated points

#define TSEARCH_FOUND_TERRAIN 0
#define TSEARCH_FOUND_MINE 1
#define TSEARCH_FOUND_SATELLITE 2
#define TSEARCH_FOUND_SKY_DOME 3
#define TSEARCH_FOUND_SKY_BAND 4
#define TSEARCH_FOUND_OBJECT 5
#ifdef NEWEDITOR
#define TSEARCH_FOUND_NODE 6
#define TSEARCH_FOUND_BNODE 7
#endif

extern int TSearch_on, TSearch_found_type, TSearch_x, TSearch_y, TSearch_seg, TSearch_face;

extern void InitTerrain();

// Called whenever a new level is initted
extern void ResetTerrain(int force = 0);

extern int GetVisibleTerrain(vector *, matrix *);
extern void DisplayTerrainList(int, bool from_automap = 0);
extern int CheckToRenderSky(int);

// left,top,right,bot are optional parameters.  Omiting them (or setting them to -1) will
// render to the whole screen.  Passing valid values will only render tiles visible in the
// specified window (though it won't clip those tiles to the window)
void RenderTerrain(ubyte from_mine, int left = -1, int top = -1, int right = -1, int bot = -1);

void GetPreRotatedPoint(g3Point *, int, int, int);
void GenerateTerrainLight();

void BuildMinMaxTerrain();
void BuildTerrainNormals();

int DrawTerrainTriangles(int n);
int LoadPCXTerrain(char *);

// Given a position, returns the terrain segment that that position is in/over
// The return value is a pure cell number, meaning it doesn't have the outside flag set
int GetTerrainCellFromPos(vector *pos);

// Given a position, returns the terrain segment that that position is in/over
// The return value is a valid room number, meaning it has the outside flag set
int GetTerrainRoomFromPos(vector *pos);

// Given a position, returns the collision terrain segment that that position is in/over
int GetColTerrainSegFromPos(vector *pos);
// Given a terrain cell, returns the collision terrain segment that that position is in/over
int GetColTerrainSegFromTerrainSeg(int cell_index);

// Computes the center of the segment in x,z and also sets y touching the ground
void ComputeTerrainSegmentCenter(vector *pos, int segnum);
// Given an position, returns the terrain Y coord at that location
// Also now can return the normal at that ground point
float GetTerrainGroundPoint(vector *pos, vector *normal = NULL);

void SetupSky(float radius, int flags, ubyte randit = 0);

// Builds the surface normal for terrain segment n
void BuildNormalForTerrainSegment(int n);

// Builds the vertex normal for terrain segment n
void BuildLightingNormalForSegment(int n);

// Gets a prerotated point that does not fall exactly on one of our 255 height values
void GetSpecialRotatedPoint(g3Point *dest, int x, int z, float yvalue);

// Takes our light angle and fills in the appropriate values in the lightsource vector
void GenerateLightSource();

// Returns the terrain segment index of the farthest point that mine segment "mine_seg"
// touches.  Based on Camera_direction
int GetFurthestMineSegment(int mine_seg);

// codes a point for visibility in the window passed to RenderTerrain()
ubyte CodeTerrainPoint(g3Point *p);

// Generates info for the LOD engine...must be called after the geometry changes
void GenerateLODDeltas();

// Generates lightmaps based on the info given by the .light field of each Terrain_seg
void UpdateTerrainLightmaps();

// Makes a four sided rectangle pinched into a triangle
void MakePinchBitmap(int dest_bm, int src_bm);

// Gets the dynamic light value for this position
float GetTerrainDynamicScalar(vector *pos, int seg);

// Shuts off LOD for a given cell
void TurnOffLODForCell(int cellnum);

// Restores the terrain deltas to their original state
void ClearLODOffs();

// Gets the checksum for this terrain
int GetTerrainGeometryChecksum();

#endif
