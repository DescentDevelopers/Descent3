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

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include "cfile.h"
#include "gamecinematics.h"
#include "multisafe.h"
#include "osiris_dll.h"
#include "osiris_predefs.h"
#include "fake_engine.h"

// The engine's function table; the bridge reads it to check the table order.
tOSIRISModuleInit Osiris_module_init;

std::vector<std::string> g_calls;
static std::string g_cur;
void rec_begin(const char *name) { g_cur = std::string(name) + "("; }
static void sep() { if (g_cur.back() != '(') g_cur += ", "; }
void rec_i(long long v) { sep(); g_cur += std::to_string(v); }
void rec_f(double v) { sep(); char b[64]; snprintf(b, sizeof b, "%g", v); g_cur += b; }
void rec_str(const char *s) { sep(); g_cur += s ? "\"" + std::string(s) + "\"" : "null"; }
void rec_ptr(const void *p) { sep(); g_cur += p ? "ptr" : "null"; }
void rec_vec(const vector *v) { sep(); if (!v) { g_cur += "null"; return; } char b[96]; snprintf(b, sizeof b, "(%g,%g,%g)", v->x(), v->y(), v->z()); g_cur += b; }
void rec_end() { g_cur += ")"; g_calls.push_back(g_cur); }

// Name lookups return a stable ID per (kind, name) so handles can be followed.
std::map<std::string, int> g_ids;
int fake_id(const char *kind, const char *name) {
  std::string key = std::string(kind) + ":" + (name ? name : "");
  auto it = g_ids.find(key);
  if (it != g_ids.end()) return it->second;
  int id = 1000 + static_cast<int>(g_ids.size()) * 7;
  g_ids[key] = id;
  return id;
}
#define FIND(fn, kind) int fn(const char *name) { rec_begin(#fn); rec_str(name); rec_end(); return fake_id(kind, name); }
FIND(osipf_FindDoorName, "door")
FIND(osipf_FindObjectName, "object")
FIND(osipf_FindRoomName, "room")
FIND(osipf_FindTriggerName, "trigger")
FIND(osipf_FindSoundName, "sound")
FIND(osipf_FindTextureName, "texture")
FIND(osipf_FindPathName, "path")
FIND(osipf_FindMatcenName, "matcen")
FIND(osipf_FindLevelGoalName, "goal")

int osipf_GetLanguageSetting() { rec_begin("osipf_GetLanguageSetting"); rec_end(); return 0; }
float osipf_GameTime() { return 100.0f; }
float osipf_FrameTime() { return 0.05f; }

// ---- files: an in-memory file per CFILE ----
struct FakeFile { std::string name; std::vector<uint8_t> data; size_t pos = 0; };
std::map<std::string, std::string> g_files; // name -> contents served by cfopen
static FakeFile *F(CFILE *f) { return reinterpret_cast<FakeFile *>(f); }
CFILE *fake_new_file(const std::vector<uint8_t> &data) { auto *f = new FakeFile; f->data = data; return reinterpret_cast<CFILE *>(f); }
const std::vector<uint8_t> &fake_file_data(CFILE *f) { return F(f)->data; }
void fake_free_file(CFILE *f) { delete F(f); }

CFILE *cfopen(const std::filesystem::path &filename, const char *mode) {
  rec_begin("cfopen"); rec_str(filename.u8string().c_str()); rec_str(mode); rec_end();
  auto it = g_files.find(filename.u8string());
  if (it == g_files.end()) return nullptr;
  auto *f = new FakeFile; f->name = it->first; f->data.assign(it->second.begin(), it->second.end());
  return reinterpret_cast<CFILE *>(f);
}
void cfclose(CFILE *f) { rec_begin("cfclose"); rec_end(); delete F(f); }
void *osipf_CFopen(const char *, const char *) { return nullptr; }
void osipf_CFclose(CFILE *f) { cfclose(f); }
uint8_t osipf_CFeof(CFILE *f) { return F(f)->pos >= F(f)->data.size(); }
int osipf_CFReadString(char *buf, size_t n, CFILE *f) {
  FakeFile *ff = F(f); size_t i = 0;
  while (ff->pos < ff->data.size()) {
    char c = static_cast<char>(ff->data[ff->pos++]);
    if (c == '\n') break;
    if (c == '\r') continue;
    if (i + 1 < n) buf[i++] = c;
  }
  if (n) buf[i] = 0;
  return static_cast<int>(i);
}
template <class T> static void put(CFILE *f, T v) { auto *p = reinterpret_cast<uint8_t *>(&v); F(f)->data.insert(F(f)->data.end(), p, p + sizeof v); }
template <class T> static T get(CFILE *f) { T v{}; if (F(f)->pos + sizeof v <= F(f)->data.size()) { memcpy(&v, &F(f)->data[F(f)->pos], sizeof v); F(f)->pos += sizeof v; } return v; }
void osipf_CFWriteInt(int i, CFILE *f) { put(f, i); }
void osipf_CFWriteFloat(float v, CFILE *f) { put(f, v); }
void osipf_CFWriteByte(int8_t v, CFILE *f) { put(f, v); }
int osipf_CFReadInt(CFILE *f) { return get<int>(f); }
float osipf_CFReadFloat(CFILE *f) { return get<float>(f); }
int8_t osipf_CFReadByte(CFILE *f) { return get<int8_t>(f); }
int osipf_CFWriteBytes(const uint8_t *buf, int count, CFILE *f) { F(f)->data.insert(F(f)->data.end(), buf, buf + count); return count; }
int osipf_CFReadBytes(uint8_t *buf, int count, CFILE *f) { int n = 0; while (n < count && F(f)->pos < F(f)->data.size()) buf[n++] = F(f)->data[F(f)->pos++]; return n; }

// ---- the rest ----
void osipf_ObjectValue(int handle, char op, char vtype, void *ptr, int index) {
  rec_begin("osipf_ObjectValue"); rec_i(handle); rec_i(op); rec_i(vtype); rec_ptr(ptr); rec_i(index); rec_end();
}
bool osipf_CallObjectEvent(int objnum, int event, tOSIRISEventInfo *ei) {
  rec_begin("osipf_CallObjectEvent"); rec_i(objnum); rec_i(event); rec_ptr(ei); rec_end(); return true;
}
int osipf_AIGoalAdd(int objhandle, int goal_type, int level, float influence, int guid, int flags, ...) {
  rec_begin("osipf_AIGoalAdd"); rec_i(objhandle); rec_i(goal_type); rec_i(level); rec_f(influence); rec_i(guid); rec_i(flags); rec_end();
  return 3;
}
void osipf_PlayerAddHudMessage(int handle, const char *str) { rec_begin("osipf_PlayerAddHudMessage"); rec_i(handle); rec_str(str); rec_end(); }
int osipf_RayCast(int objhandle, vector *p0, vector *p1, int room, float rad, int flags, ray_info *ri) {
  rec_begin("osipf_RayCast"); rec_i(objhandle); rec_vec(p0); rec_vec(p1); rec_i(room); rec_f(rad); rec_i(flags); rec_end();
  memset(ri, 0, sizeof *ri); return 0;
}
vector osipf_AIGetRoomPathPoint(int roomnum) { rec_begin("osipf_AIGetRoomPathPoint"); rec_i(roomnum); rec_end(); vector v{}; v.x() = 1; v.y() = 2; v.z() = 3; return v; }

static int g_next_spew = 7000;
void fake_reset() {
  g_calls.clear();
  g_next_spew = 7000;
}
void msafe_CallFunction(uint8_t type, msafe_struct *m) {
  rec_begin("msafe_CallFunction");
  rec_i(type); rec_i(m->objhandle); rec_i(m->roomnum); rec_i(m->index); rec_i(m->state);
  rec_f(m->volume); rec_f(m->fog_r); rec_f(m->fog_g); rec_f(m->fog_b); rec_f(m->fog_depth);
  rec_f(m->mass); rec_f(m->drag); rec_f(m->lifetime); rec_f(m->interval); rec_f(m->longevity); rec_f(m->size); rec_f(m->speed);
  rec_i(m->effect_type); rec_i(m->gunpoint); rec_i(m->phys_info); rec_i(m->is_real); rec_i(m->random);
  rec_str(m->message);
  rec_end();
  if (type == MSAFE_OBJECT_START_SPEW)
    m->id = g_next_spew++;
}
void msafe_GetValue(int type, msafe_struct *m) { rec_begin("msafe_GetValue"); rec_i(type); rec_i(m->objhandle); rec_end(); m->state = 1; }
void msafe_DoPowerup(msafe_struct *m) { rec_begin("msafe_DoPowerup"); rec_i(m->objhandle); rec_end(); }

int Osiris_CreateTimer(tOSIRISTIMER *t) {
  rec_begin("Osiris_CreateTimer"); rec_i(t->flags); rec_i(t->id); rec_i(t->repeat_count); rec_f(t->timer_interval); rec_i(t->object_handle_detonator); rec_end();
  return 42;
}
void Osiris_CancelTimer(int h) { rec_begin("Osiris_CancelTimer"); rec_i(h); rec_end(); }
void Osiris_CancelTimerID(int id) { rec_begin("Osiris_CancelTimerID"); rec_i(id); rec_end(); }
uint8_t Osiris_TimerExists(int) { return 0; }
std::map<void *, int> g_chunks;
void *Osiris_AllocateMemory(tOSIRISMEMCHUNK *mc) { void *p = calloc(1, mc->size); g_chunks[p] = mc->size; rec_begin("Osiris_AllocateMemory"); rec_i(mc->id); rec_i(mc->size); rec_end(); return p; }
void Osiris_FreeMemory(void *p) { g_chunks.erase(p); free(p); }
int Osiris_GetMemoryChunkSize(void *p) { auto it = g_chunks.find(p); return it == g_chunks.end() ? -1 : it->second; }
bool Cinematic_Start(tGameCinematic *info, char *text) { rec_begin("Cinematic_Start"); rec_i(info->flags); rec_str(text); rec_end(); return true; }
void Cinematic_Stop() { rec_begin("Cinematic_Stop"); rec_end(); }
void Cinematic_StartCannedScript(tCannedCinematicInfo *info) {
  rec_begin("Cinematic_StartCannedScript"); rec_i(info->type); rec_i(info->camera_pathid); rec_i(info->target_pathid);
  rec_str(info->text_to_display); rec_i(info->target_objhandle); rec_f(info->time); rec_end();
}


