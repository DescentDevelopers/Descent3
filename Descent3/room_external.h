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

 * $Logfile: /DescentIII/main/room_external.h $
 * $Revision: 21 $
 * $Date: 10/21/99 1:32p $
 * $Author: Jeff $
 *
 * Defines and structs for room definitions (for DLL export)
 *
 * $Log: /DescentIII/main/room_external.h $
 *
 * 21    10/21/99 1:32p Jeff
 * added checkbox to prevent certain rooms from lighting
 *
 * 20    4/28/99 1:39p Chris
 * Added the ability to block portals
 *
 * 19    4/26/99 11:11a Chris
 * Updated Bnode system
 *
 * 18    4/18/99 5:39a Chris
 * Vastly improved the path node system
 *
 * 17    4/15/99 5:49p Chris
 * Fixed a bug with rendering the BOAPath nodes
 *
 * 16    4/15/99 12:20p Jason
 * made mirrors more robust and able to have multiple mirrored faces in
 * the same room (as long as they are all on the same plane)
 *
 * 15    4/14/99 3:13p Chris
 * Beginning to add BoaNode stuff
 *
 * 14    4/14/99 11:40a Jason
 * added secret flag
 *
 * 13    4/05/99 10:54a Matt
 * Added auto-waypoint system
 *
 * 12    3/31/99 5:14p Matt
 * Added shell flag for faces
 *
 * 11    3/11/99 1:08p Jason
 * more fixes for smooth specular lighting
 *
 * 10    3/01/99 8:10p Matt
 * Added flag for manually-set path points
 *
 * 9     2/21/99 4:35p Chris
 * Improving the level goal system...  Not done.
 *
 * 8     2/19/99 4:26p Jason
 * more work on Katmai support
 *
 * 7     2/09/99 9:59a Chris
 * Massive BOA update  :)  Terrain happy now.  Vis happy now.  Sound happy
 * now.
 *
 * 6     2/03/99 5:49p Matt
 * Added room damage system
 *
 * 5     2/03/99 4:26p Jason
 * made multiplayer coop actually work!
 *
 * 4     2/01/99 4:17p Jason
 * more changes for multisafe
 *
 * 3     1/29/99 12:48p Matt
 * Rewrote the doorway system
 *
 * 2     1/21/99 11:15p Jeff
 * pulled out some structs and defines from header files and moved them
 * into seperate header files so that multiplayer dlls don't require major
 * game headers, just those new headers.  Side effect is a shorter build
 * time.  Also cleaned up some header file #includes that weren't needed.
 * This affected polymodel.h, object.h, player.h, vecmat.h, room.h,
 * manage.h and multi.h
 *
 * $NoKeywords: $
 */

#ifndef _ROOM_EXTERNAL_H_
#define _ROOM_EXTERNAL_H_

#include "pstypes.h"
#include "vecmat_external.h"
#include "bnode.h"

#define MAX_FACES_PER_ROOM 3000  // max number of faces per room
#define MAX_VERTS_PER_ROOM 10000 // max vertices per room
#define MAX_VERTS_PER_FACE 64    // max vertices per face

// Face flags
// NOTE:  If you add a flag here, please check the function CopyFaceFlags()
#define FF_LIGHTMAP 0x0001        // Render this face with a lightmap on top
#define FF_VERTEX_ALPHA 0x0002    // This face has vertex alpha blending
#define FF_CORONA 0x0004          // This face has a lighting corona
#define FF_TEXTURE_CHANGED 0x0008 // The texture on this face has changed
#define FF_HAS_TRIGGER 0x0010     // This face has a trigger
#define FF_SPEC_INVISIBLE 0x0020  // This face needs to be not rendered during specularity pass
#define FF_FLOATING_TRIG 0x0040   // This face only exists as a floating trigger
#define FF_DESTROYED 0x0080       // This face has been blown up
#define FF_VOLUMETRIC 0x0100      // This face is a volumetric face
#define FF_TRIANGULATED 0x0200    // ??
#define FF_VISIBLE 0x0400         // This face is visible this frame (Valid only during render)
#define FF_NOT_SHELL 0x0800       // This face is not part of the room shell
#define FF_TOUCHED 0x1000         // This face has been touched by fvi_QuickDistFaceList
#define FF_GOALFACE 0x2000        // This face is a goal texture face
#define FF_NOT_FACING 0x4000      // This face is not facing us this frame (Valid only during render)
#define FF_SCORCHED 0x8000        // This face has one or more scorch marks
// NOTE:  If you add a flag here, please check the function CopyFaceFlags()

// UVLs for room verts
struct roomUVL {
  float u, v; // texture coordinates
  float u2, v2;
  uint8_t alpha; // alpha for this vertex
};

// an n-sided polygon used as part of a room or portal
struct face {
  uint16_t flags;     // flags for this face (see above)
  uint8_t num_verts;  // how many vertices in this face
  int8_t portal_num; // which portal this face is part of, or -1 if none

  int16_t *face_verts;       // index into list of vertices for this face
  roomUVL *face_uvls;      // index into list of uvls for this face
  vector normal;           // the surface normal of this face
  int16_t tmap;              // texture numbers for this face
  uint16_t lmi_handle;       // the lightmap info number for this face
  int16_t special_handle;    // the index into the special_faces array
  uint8_t renderframe;       // what frame this face was last rendered (for lighting)
  uint8_t light_multiple;    // what multiple to times by
  vector min_xyz, max_xyz; // min & max extents of this face (for FVI)
};

// Portal flags
#define PF_RENDER_FACES 1        // render the face(s) in the portal
#define PF_RENDERED_FLYTHROUGH 2 // allow flythrough of rendered faces
#define PF_TOO_SMALL_FOR_ROBOT 4 // Too small for a robot to use for path following (like a small window)
#define PF_COMBINED 8            // This portal has been combined with another for rendering purposes
#define PF_CHANGED 16            // Used for multiplayer - this portal has been changed
#define PF_BLOCK 32
#define PF_BLOCK_REMOVABLE 64

// a connection between two rooms
struct portal {
  int flags;         // flags for this portal
  int16_t portal_face; // the face for this portal
  int16_t croom;       // the room this portal connects to
  int16_t cportal;     // the portal in croom this portal connects to
  int16_t bnode_index;
  int combine_master; // For rendering combined portals
  vector path_pnt;    // Point used by the path system
};

// Room flags
#define RF_FUELCEN 1                // room is a refueling center
#define RF_DOOR (1 << 1)            // a 3d door is here.
#define RF_EXTERNAL (1 << 2)        // this is an external room (i.e. a building)
#define RF_GOAL1 (1 << 3)           // this room is goal 1
#define RF_GOAL2 (1 << 4)           // this room is goal 2
#define RF_TOUCHES_TERRAIN (1 << 5) // this room should recieve lighting from satellites
#define RF_SORTED_INC_Y (1 << 6)    // Faces are sorted with increasing y
#define RF_GOAL3 (1 << 7)           // this room is goal 3
#define RF_GOAL4 (1 << 8)           // this room is goal 4
#define RF_FOG (1 << 9)             // this room is fogged
#define RF_SPECIAL1 (1 << 10)       // This room is a special room
#define RF_SPECIAL2 (1 << 11)       // This room is a special room
#define RF_SPECIAL3 (1 << 12)       // This room is a special room
#define RF_SPECIAL4 (1 << 13)       // This room is a special room
#define RF_SPECIAL5 (1 << 14)       // This room is a special room
#define RF_SPECIAL6 (1 << 15)       // This room is a special room
#define RF_MIRROR_VISIBLE (1 << 16) // The mirror is this room is visible
#define RF_TRIANGULATE (1 << 17)    // All the faces in this room should be drawn with triagulation on
#define RF_STROBE (1 << 18)         // This room strobes with pulse lighting
#define RF_FLICKER (1 << 19)        // This room flickers with pulse lighting
#define RFM_MINE                                                                                                       \
  ((1 << 20) | (1 << 21) | (1 << 22) | (1 << 23) |                                                                     \
   (1 << 24)) // Mine index of this room (we support up to 32 individual mines without a problem)
#define RF_INFORM_RELINK_TO_LG (1 << 25) // Informs the level goal system on player relinking to this room
#define RF_MANUAL_PATH_PNT (1 << 26)     // The room path_pnt has been set manually (i.e. by the designer)
#define RF_WAYPOINT (1 << 27)            // This room has a waypoint in it
#define RF_SECRET (1 << 28)              // This room is a secret room
#define RF_NO_LIGHT (1 << 29)            // This room does not get lit

#define GOALROOM (RF_GOAL1 | RF_GOAL2 | RF_GOAL3 | RF_GOAL4)

#define ROOM_NAME_LEN 19 // how long a room name can be (not counting null terminator)

struct doorway;

// the basic building-block of a Descent 3 level
struct room {
  int flags; // various room flags

  int num_faces;   // how many poygons in this room
  int num_portals; // how many connections in this room
  int num_verts;   // how many verts in the room
  face *faces;     // pointer to list of faces
  portal *portals; // pointer to list of portals
  vector *verts;   // array of vertices for this room
  vector4 *verts4; // array of 16byte vertices for this room

  doorway *doorway_data;   // pointer to this room's doorway data, or NULL if not a doorway
  char *name;              // name of this room, or NULL
  int objects;             // index of first object in this room
  vector max_xyz, min_xyz; // for external room visibility checking

  float last_render_time; // Last time we rendered this room

  // Hierarchical bounding boxes for this room
  vector bbf_min_xyz;
  vector bbf_max_xyz;
  int16_t num_bbf_regions;
  int16_t pad1;
  int16_t **bbf_list;
  int16_t *num_bbf;
  vector *bbf_list_min_xyz;
  vector *bbf_list_max_xyz;
  uint8_t *bbf_list_sector;

  bn_list bn_info;

  int16_t wpb_index;           // world point buffer index - where this room starts
  uint8_t pulse_time;          // each room can has a pulse time
  uint8_t pulse_offset;        // each room has a timer offset for which it pulses
  vector wind;               // Wind vector for the room
  int ambient_sound;         // Index of ambient sound pattern for this room, or -1 if none
  int16_t vis_effects;         // index of first visual effect in this room
  int16_t mirror_face;         // Index of face that this room is to be mirrored by
  uint8_t num_mirror_faces;    // Number of faces in this room that have the same texture as the mirror
  uint16_t *mirror_faces_list; // the list of faces in this room that have the same texture as the mirror
  float damage;              // The damage per second applied to players (& maybe others) in room

  vector path_pnt;      // Point used by the path system
  uint8_t *volume_lights; // Pointer to memory for our volumetric lighting
  int16_t volume_width;   // The dimensions of our volumetric room
  int16_t volume_height;
  int16_t volume_depth;
  float fog_depth;           // How far until fog is totally opaque
  float fog_r, fog_g, fog_b; // Fog color

  uint8_t env_reverb;        // environmental reverb preset
  uint8_t room_change_flags; // For multiplayer, detects what characteristics have to be sent
  uint8_t damage_type;       // What type of damage this rooms does (for sound) if damage > 0
  uint8_t used;              // is this room holding data?

};

#endif
