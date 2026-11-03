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

// Link-time stand-ins for the editor/game globals and functions that
// editor/Erooms.cpp, editor/RoomUVs.cpp and Descent3/room.cpp reference but
// that live elsewhere in the (unlinkable) Descent3Editor executable.
//
// Only a few of these are reached by the tests: the BNode hooks are no-ops
// (test rooms have no BNodes), and GameTextures/FindTextureName and the
// alpha conversions provide what the room-file save/load round trip needs
// (the tests do not check vertex alpha through it). Everything
// else is referenced only by code paths the tests never execute; those abort
// so that an unexpected call cannot go unnoticed.

#include <cmath>
#include <cstdlib>
#include <cstring>

#include "bitmap.h"
#include "d3edit.h"
#include "gametexture.h"
#include "lighting.h"
#include "object.h"
#include "objinfo.h"
#include "room.h"
#include "special_face.h"
#include "trigger.h"

namespace {
[[noreturn]] void unexpected_call() { std::abort(); }
} // namespace

bool Katmai = false;
float Gametime = 0.0f;

int World_changed = 0;
room *Curroomp = nullptr;
int Curface = 0;
int Markedface = 0;

float Room_multiplier[MAX_ROOMS + MAX_PALETTE_ROOMS];
float Room_ambience_r[MAX_ROOMS + MAX_PALETTE_ROOMS];
float Room_ambience_g[MAX_ROOMS + MAX_PALETTE_ROOMS];
float Room_ambience_b[MAX_ROOMS + MAX_PALETTE_ROOMS];

// The same table that InitLighting() builds in lighting.cpp.
float Ubyte_to_float[256];
static const bool ubyte_to_float_initialized = [] {
  for (int i = 0; i < 256; i++)
    Ubyte_to_float[i] = (float)i / 255.0f;
  return true;
}();

texture GameTextures[MAX_TEXTURES];

int Num_triggers = 0;
int Highest_object_index = -1;

// Never indexed by the code paths under test; only their types matter.
trigger Triggers[1];
object Objects[1];
object_info Object_info[1];
char *Object_type_names[MAX_OBJECT_TYPES];
bms_bitmap GameBitmaps[MAX_BITMAPS];
special_face SpecialFaces[1];
uint8_t Terrain_dynamic_table[1];
int BOA_vis_checksum = 0;
int Terrain_checksum = 0;
int Terrain_occlusion_checksum = 0;

void BNode_FreeRoom(room *) {}
void BNode_RemapTerrainRooms(int, int) {}

int FindTextureName(const char *name) {
  for (int i = 0; i < MAX_TEXTURES; i++)
    if (GameTextures[i].used && !stricmp(GameTextures[i].name, name))
      return i;
  return -1;
}

uint8_t Float_to_ubyte(float fnum) { return (uint8_t)std::lround(fnum * 255.0f); }

// Test rooms never get lightmaps or special faces. These can still be reached
// on cleanup when the code under test leaves a face handle uninitialised.
void FreeLightmapInfo(int) {}
void FreeSpecialFace(int) {}

void BNode_ClearBNodeInfo() { unexpected_call(); }
void DeleteTrigger(int, int) { unexpected_call(); }
int BOAGetMineChecksum() { unexpected_call(); }
bool fvi_QuickRoomCheck(vector *, room *, bool) { unexpected_call(); }
int GetTerrainRoomFromPos(vector *) { unexpected_call(); }
int GetTextureBitmap(int, int, bool) { unexpected_call(); }
int bm_w(int, int) { unexpected_call(); }
int bm_h(int, int) { unexpected_call(); }
bool bm_pixel_transparent(int, int, int) { unexpected_call(); }
int ObjCreate(uint8_t, uint16_t, int, vector *, const matrix *, int) { unexpected_call(); }
void ObjDelete(int) { unexpected_call(); }
void ObjSetAABB(object *) { unexpected_call(); }
void BigObjAdd(int) { unexpected_call(); }
int LoadLevel(char *, void (*)(const char *, int, int)) { unexpected_call(); }
