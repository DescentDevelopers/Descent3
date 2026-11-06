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

// Stand-ins for the functions of the Descent3 executable that the game libraries reference.
//
// Most do nothing and report "nothing there", which is what the code under test sees in a game that has no level
// loaded. The ones no test is expected to reach end the test process with a message instead, so that a test that
// starts depending on one shows up rather than quietly testing a stub.

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>

#include "d3_test/game.h"

#include "AIMain.h"
#include "attach.h"
#include "BOA.h"
#include "damage.h"
#include "demofile.h"
#include "fireball.h"
#include "game2dll.h"
#include "hud.h"
#include "levelgoal.h"
#include "osiris_dll.h"
#include "player.h"
#include "render.h"
#include "scorch.h"
#include "trigger.h"
#include "voice.h"
#include "bitmap.h"
#include "D3ForceFeedback.h"
#include "door.h"
#include "doorway.h"
#include "gamefile.h"
#include "gametexture.h"
#include "Inventory.h"
#include "megacell.h"
#include "mem.h"
#include "object.h"
#include "objinfo.h"
#include "room.h"
#include "ship.h"
#include "terrain.h"
#include "vclip.h"
#include "viseffect.h"
#include "weapon.h"

namespace {

[[noreturn]] void Unexpected(const char *function) {
  std::fprintf(stderr, "\nFAILURE: the test reached %s, which the tests only stand in for (tests/support/game).\n",
               function);
  std::fflush(stderr);
  std::abort();
}

std::map<std::string, int> texture_names;
std::map<std::string, int> texture_bitmap_names;

std::string Lowercase(const char *name) {
  std::string key(name);
  std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c) { return std::tolower(c); });
  return key;
}

int Lookup(const std::map<std::string, int> &names, const char *name) {
  auto it = names.find(Lowercase(name));
  return it == names.end() ? -1 : it->second;
}

} // namespace

namespace d3_test {

void SetTextureName(const char *name, int texture) { texture_names[Lowercase(name)] = texture; }
void SetTextureBitmapName(const char *name, int texture) { texture_bitmap_names[Lowercase(name)] = texture; }

void ClearTextureNames() {
  texture_names.clear();
  texture_bitmap_names.clear();
}

void EnsureBitmaps() {
  static const bool started = (bm_InitBitmaps(), true);
  (void)started;
}

void EnsureMem() {
  static const bool started = (mem_Init(), true);
  (void)started;
}

} // namespace d3_test

// gametexture.cpp. Like the real lookups, these match names case-insensitively.
int FindTextureName(const char *name) { return Lookup(texture_names, name); }
int FindTextureBitmapName(const char *name) { return Lookup(texture_bitmap_names, name); }
int GetTextureBitmap(int handle, int framenum, bool force) { return 0; }
int AllocTexture() { Unexpected("AllocTexture"); }
void FreeTexture(int) { Unexpected("FreeTexture"); }
int LoadTextureImage(const char *, int *, int, int, int, int) { Unexpected("LoadTextureImage"); }
int AllocateProceduralForTexture(int) { Unexpected("AllocateProceduralForTexture"); }
void AllocateStaticProceduralsForTexture(int, int) { Unexpected("AllocateStaticProceduralsForTexture"); }
void FreeProceduralForTexture(int) { Unexpected("FreeProceduralForTexture"); }

// object.cpp, attach.cpp
int ObjCreate(uint8_t type, uint16_t id, int roomnum, vector *pos, const matrix *orient, int parent_handle) {
  return -1;
}
object *ObjGet(int handle) { return nullptr; }
void ObjSetPos(object *obj, vector *pos, int roomnum, matrix *orient, bool f_update_attached_children) {}
void ObjSetOrient(object *obj, const matrix *orient) {}
void ObjSetAABB(object *obj) {}
void SetObjectDeadFlag(object *obj, bool tell_clients_to_remove, bool play_sound_on_clients) {}
void DestroyObject(object *objp, float explosion_mag, int death_flags) {}
void AttachUpdateSubObjects(object *obj) {}
bool ObjectsAreRelated(int o1, int o2) { return false; }

// objinfo.cpp, door.cpp, ship.cpp, weapon.cpp, gamefile.cpp, megacell.cpp, vclip.cpp
int AllocObjectID(int, bool, bool, bool) { Unexpected("AllocObjectID"); }
void FreeObjectID(int) { Unexpected("FreeObjectID"); }
int FindObjectIDName(const char *) { Unexpected("FindObjectIDName"); }
int AllocDoor() { Unexpected("AllocDoor"); }
void FreeDoor(int) { Unexpected("FreeDoor"); }
int FindDoorName(const char *) { Unexpected("FindDoorName"); }
int AllocShip() { Unexpected("AllocShip"); }
void FreeShip(int) { Unexpected("FreeShip"); }
int FindShipName(const char *) { Unexpected("FindShipName"); }
int AllocWeapon() { Unexpected("AllocWeapon"); }
void FreeWeapon(int) { Unexpected("FreeWeapon"); }
int FindWeaponName(const char *) { Unexpected("FindWeaponName"); }
int AllocGamefile() { Unexpected("AllocGamefile"); }
int FindGamefileName(char *) { Unexpected("FindGamefileName"); }
int AllocMegacell() { Unexpected("AllocMegacell"); }
int FindMegacellName(char *) { Unexpected("FindMegacellName"); }
int AllocLoadVClip(const char *, int, int, int, int) { Unexpected("AllocLoadVClip"); }
void FreeVClip(int) { Unexpected("FreeVClip"); }

// viseffect.cpp, D3ForceFeedback.cpp, doorway.cpp, room.cpp, terrain.cpp
void VisEffectDelete(int visnum) {}
void VisEffectRelink(int visnum, int newroomnum) {}
void ForceEffectsPlay(int id, float *scale, vector *direction) {}
float DoorwayPosition(room *rp) { return 0.0f; }
int CheckTransparentPoint(const vector *pnt, const room *rp, const int facenum) { return 0; }
int GetTerrainRoomFromPos(vector *pos) { return -1; }
int GetTerrainCellFromPos(vector *pos) { return -1; }

// Inventory.cpp: every player (Players[]) embeds one.
Inventory::Inventory() {}
Inventory::~Inventory() {}

// AImain.cpp, BOA.cpp, Game2DLL.cpp, OsirisLoadandBind.cpp, trigger.cpp
bool AINotify(object *obj, uint8_t notify_type, void *info) { return false; }
int BOA_DetermineStartRoomPortal(int start_room, vector *start_pos, int end_room, vector *end_pos, bool f_for_sound,
                                 bool f_making_robot_path_invalid_list, int *blocked_portal) {
  return -1;
}
bool BOA_IsSoundAudible(int start_room, int end_room) { return false; }
void CallGameDLL(int eventnum, dllinfo *data) {}
bool Osiris_CallEvent(object *obj, int event, tOSIRISEventInfo *data) { return false; }
void CheckTrigger(int roomnum, int facenum, object *objp, int type) {}
void levelgoals::Inform(char type, int comp_type, int handle) {}

// damage.cpp, WeaponFire.cpp, fireball.cpp, scorch.cpp, Player.cpp
bool ApplyDamageToPlayer(object *playerobj, object *killer, int damage_type, float damage_amount, int server_says,
                         int weapon_id, bool playsound) {
  return false;
}
bool ApplyDamageToGeneric(object *robotobj, object *killer, int damage_type, float damage, int server_says,
                          int weapon_id) {
  return false;
}
void SetNapalmDamageEffect(object *obj, object *killer, int weapon_id) {}
void BreakGlassFace(room *rp, int facenum, vector *hitpnt, vector *hitvec) {}
void DoWeaponExploded(object *, vector *norm, vector *collision_point, object *hit_object) {}
void CreateImpactSpawnFromWeapon(object *obj, vector *norm) {}
void CreateRobotSpawnFromWeapon(object *obj) {}
int CreateFireball(vector *pos, int fireball_num, int roomnum, int realtype) { return -1; }
void AddScorch(int roomnum, int facenum, vector *pos, int texture_handle, float size) {}
void StartPlayerExplosion(int slot) {}
object *ObjGetUltimateParent(object *child) { return child; }

// viseffect.cpp, D3ForceFeedback.cpp, hudmessage.cpp, demofile.cpp, voice.cpp, terrain.cpp, render.cpp
int VisEffectCreate(uint8_t type, uint8_t id, int roomnum, vector *pos) { return -1; }
void CreateRandomSparks(int num_sparks, vector *pos, int roomnum, int which_index, float force_scalar) {}
void CreateRandomLineSparks(int num_sparks, vector *pos, int roomnum, uint16_t color, float force_scalar) {}
bool ForceIsEnabled() { return false; }
void DoForceForWeapon(object *me_obj, object *it_obj, vector *force) {}
void DoForceForWall(object *playerobj, float hitspeed, int hitseg, int hitwall, vector *wall_normal) {}
bool AddHUDMessage(const char *format, ...) { return false; }
void DemoWrite3DSound(int16_t soundidx, uint16_t objnum, int priority, float volume) {}
bool InitVoices() { return true; }
void CloseVoices() {}
void DeformTerrain(vector *pos, int depth, float size) {}
#ifndef RELEASE
// A function only in debug builds; release builds use the macro, which this computes the same way.
int TERRAIN_REGION(int x) { return (Terrain_seg[0x7FFFFFFF & x].flags & TFM_REGION_MASK) >> 5; }
#endif
void SortStates(state_limited_element *state_array, int cellcount) {}
void GetIJ(const vector *normal, int *ii, int *jj) { Unexpected("GetIJ"); }

// init.cpp, gamesequence.cpp
void InitMessage(const char *, float) { Unexpected("InitMessage"); }
void RemapEverything() { Unexpected("RemapEverything"); }
