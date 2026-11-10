/*
 * Descent 3
 * Copyright (C) 2026 Descent Developers
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

// Stand-ins for the globals of the Descent3 executable that the game libraries reference. The executable itself
// cannot be linked into a test, so the libraries under test get these instead. Each is declared as in the file named
// next to it, and starts out the way the game leaves it before a level is loaded.

#include <cstdint>

#include "BOA.h"
#include "damage_external.h"
#include "descent.h"
#include "door.h"
#include "fireball.h"
#include "game.h"
#include "game2dll.h"
#include "gamefile.h"
#include "gametexture.h"
#include "levelgoal.h"
#include "lighting.h"
#include "lightmap_info.h"
#include "marker.h"
#include "megacell.h"
#include "multi.h"
#include "object.h"
#include "objinfo.h"
#include "player.h"
#include "render.h"
#include "room.h"
#include "ship.h"
#include "terrain.h"
#include "vclip.h"
#include "viseffect.h"
#include "weapon.h"

class oeAppDatabase;

// appdatabase: the game's registry.
oeAppDatabase *Database = nullptr;

// descent.cpp: the application object.
oeApplication *Descent = nullptr;

// config.cpp, dedicated_server.cpp, editor
bool Game_fullscreen = false;
bool Dedicated_server = false;
bool Running_editor = false;

// GameLoop.cpp, game.cpp, demofile.cpp
float Gametime = 0.0f;
float Frametime = 0.1f;
int Game_mode = 0;
uint32_t Demo_flags = 0;
int Game_do_flying_sim = 1;
int Game_do_walking_sim = 1;
int Game_do_vis_sim = 1;

// gamesequence.cpp: level-loading progress counters, bumped by the bitmap and sound loaders.
int paged_in_count = 0;
int paged_in_num = 0;

// object.cpp, objinfo.cpp, player.cpp
object Objects[MAX_OBJECTS];
int Highest_object_index = 0;
object *Viewer_object = nullptr;
object *Player_object = nullptr;
int Num_big_objects = 0;
int16_t BigObjectList[MAX_BIG_OBJECTS];
object_info Object_info[MAX_OBJECT_IDS];
player Players[MAX_PLAYERS];
int Player_num = 0;

// room.cpp, terrain.cpp, door.cpp
room Rooms[MAX_ROOMS + MAX_PALETTE_ROOMS];
int Highest_room_index = -1;
terrain_segment Terrain_seg[(TERRAIN_WIDTH + 1) * (TERRAIN_DEPTH + 1)];
terrain_tex_segment Terrain_tex_seg[TERRAIN_TEX_WIDTH * TERRAIN_TEX_DEPTH];
terrain_normals *TerrainNormals[MAX_TERRAIN_LOD];
uint8_t *Terrain_max_height_int[7];
door Doors[MAX_DOORS];

// gametexture.cpp, vclip.cpp, gamefile.cpp, megacell.cpp, ship.cpp, weapon.cpp, viseffect.cpp
texture GameTextures[MAX_TEXTURES];
vclip GameVClips[MAX_VCLIPS];
gamefile Gamefiles[MAX_GAMEFILES];
megacell Megacells[MAX_MEGACELLS];
ship Ships[MAX_SHIPS];
weapon Weapons[MAX_WEAPONS];
vis_effect *VisEffects = nullptr;

// multi.cpp, Game2DLL.cpp, levelgoal.cpp, gamesave.cpp
netgame_info Netgame;
int Multi_requested_damage_type = PD_NONE;
float Multi_requested_damage_amount = 0;
dllinfo DLLInfo;
levelgoals Level_goals;
char MarkerMessages[MAX_PLAYERS * 2][MAX_MARKER_MESSAGE_LENGTH];

// BOA.cpp: the room-to-room path tables of a loaded level.
float BOA_cost_array[MAX_ROOMS + MAX_BOA_TERRAIN_REGIONS][MAX_PATH_PORTALS];
uint16_t BOA_Array[MAX_ROOMS + MAX_BOA_TERRAIN_REGIONS][MAX_ROOMS + MAX_BOA_TERRAIN_REGIONS];

// render.cpp, lighting.cpp, lightmap_info.cpp, fireball.cpp
state_limited_element State_elements[MAX_STATE_ELEMENTS];
float Specular_tables[3][MAX_SPECULAR_INCREMENTS];
lightmap_info *LightmapInfo = nullptr;
fireball Fireballs[NUM_FIREBALLS];

// game.cpp, WeaponFire.cpp
int sound_override_force_field = -1;
int sound_override_glass_breaking = -1;
bool Enable_omega_collions = false;

// physics, as set up by the game
bool Level_powerups_ignore_wind = false;
int force_field_bounce_texture[MAX_FORCE_FIELD_BOUNCE_TEXTURES] = {-1, -1, -1};
float force_field_bounce_multiplier[MAX_FORCE_FIELD_BOUNCE_TEXTURES] = {1.0f, 1.0f, 1.0f};
bool Tracking_FVI = false;
