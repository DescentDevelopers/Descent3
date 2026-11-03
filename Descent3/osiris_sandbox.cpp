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

/*
 * Bridge between the OSIRIS loader and the x86 sandbox.
 *
 * A sandboxed script sees the same 123-entry function table (tOSIRISModuleInit::fp)
 * as a native script. Each entry points at an x86 stub that exits to
 * Module::HostCall, which reads the arguments from the guest stack, converts them
 * to host types and calls the engine function. Structures are converted field by
 * field where the 32-bit layout differs from the host layout (any struct with a
 * pointer in it). Host pointers are never given to the guest: files become small
 * handles, script memory chunks are mirrored in guest memory, and the event
 * extra_info pointer is only passed between scripts in the same sandbox.
 *
 * Restrictions compared with a native script:
 *  - File_Open only opens existing files for reading, by bare file name.
 *  - OMMS shared memory is private to the module and is not kept in save games.
 */

#include "osiris_sandbox.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <memory>
#include <utility>
#include <vector>

#include "cfile.h"
#include "gamecinematics.h"
#include "log.h"
#include "multisafe.h"
#include "osiris_dll.h"
#include "osiris_predefs.h"
#include "x86sandbox.h"

extern tOSIRISModuleInit Osiris_module_init;
extern int Osiris_GetMemoryChunkSize(void *memory);
extern void Osiris_CancelTimerID(int id);
extern void Cinematic_StartCannedScript(tCannedCinematicInfo *info);

using x86sandbox::Sandbox;
using x86sandbox::Status;

namespace {

// ---------------------------------------------------------------------------
// The API table, in the order of Osiris_CreateModuleInitStruct and of the
// scripts' osiris_import.h. This order is part of the script ABI.
// ---------------------------------------------------------------------------
enum Api : uint32_t {
  API_mprintf,
  API_MSafe_CallFunction,
  API_MSafe_GetValue,
  API_Obj_CallEvent,
  API_Trgr_CallEvent,
  API_Sound_TouchFile,
  API_Obj_FindID,
  API_Wpn_FindID,
  API_Obj_GetTimeLived,
  API_Obj_GetGunPos,
  API_Room_Value,
  API_Room_IsValid,
  API_Obj_GetAttachParent,
  API_Obj_GetNumAttachSlots,
  API_Obj_GetAttachChildHandle,
  API_Obj_AttachObjectAP,
  API_Obj_AttachObjectRad,
  API_Obj_UnattachFromParent,
  API_Obj_UnattachChild,
  API_Obj_UnattachChildren,
  API_FVI_RayCast,
  API_AI_GetPathID,
  API_AI_GoalFollowPathSimple,
  API_AI_PowerSwitch,
  API_AI_TurnTowardsVectors,
  API_AI_SetType,
  API_AI_FindHidePos,
  API_AI_GoalAddEnabler,
  API_AI_AddGoal,
  API_AI_ClearGoal,
  API_AI_Value,
  API_AI_FindObjOfType,
  API_AI_GetRoomPathPoint,
  API_AI_FindEnergyCenter,
  API_AI_GetDistToObj,
  API_AI_SetGoalFlags,
  API_AI_SetGoalCircleDist,
  API_File_ReadBytes,
  API_File_ReadInt,
  API_File_ReadShort,
  API_File_ReadByte,
  API_File_ReadFloat,
  API_File_ReadDouble,
  API_File_ReadString,
  API_File_WriteBytes,
  API_File_WriteString,
  API_File_WriteInt,
  API_File_WriteShort,
  API_File_WriteByte,
  API_File_WriteFloat,
  API_File_WriteDouble,
  API_Scrpt_MemAlloc,
  API_Scrpt_MemFree,
  API_Scrpt_CancelTimer,
  API_Scrpt_CreateTimer,
  API_MSafe_DoPowerup,
  API_Obj_Create,
  API_Game_GetTime,
  API_Game_GetFrameTime,
  API_Obj_WBValue,
  API_Scrpt_TimerExists,
  API_Obj_Value,
  API_Matcen_Value,
  API_Matcen_Reset,
  API_Matcen_Copy,
  API_Matcen_Create,
  API_Matcen_FindID,
  API_Msn_FlagSet,
  API_Msn_FlagGet,
  API_Player_Value,
  API_Obj_SetCustomAnim,
  API_Player_AddHudMessage,
  API_Obj_Ghost,
  API_Obj_Burning,
  API_Obj_IsEffect,
  API_File_Open,
  API_File_Close,
  API_File_Tell,
  API_File_eof,
  API_Sound_Stop,
  API_Sound_Play2d,
  API_Sound_Play3d,
  API_Sound_FindId,
  API_AI_IsObjFriend,
  API_AI_IsObjEnemy,
  API_AI_GoalValue,
  API_AI_GetNearbyObjs,
  API_AI_GetCurGoalIndex,
  API_OMMS_Malloc,
  API_OMMS_Attach,
  API_OMMS_Detach,
  API_OMMS_Free,
  API_OMMS_Find,
  API_OMMS_GetInfo,
  API_Cine_Start,
  API_Cine_Stop,
  API_Scrpt_FindSoundName,
  API_Scrpt_FindRoomName,
  API_Scrpt_FindTriggerName,
  API_Scrpt_FindObjectName,
  API_Scrpt_GetTriggerRoom,
  API_Scrpt_GetTriggerFace,
  API_Scrpt_FindDoorName,
  API_Scrpt_FindTextureName,
  API_Game_CreateRandomSparks,
  API_Scrpt_CancelTimerID,
  API_Obj_GetGroundPos,
  API_Game_EnableShip,
  API_Game_IsShipEnabled,
  API_Path_GetInformation,
  API_Cine_StartCanned,
  API_Scrpt_FindMatcenName,
  API_Scrpt_FindPathName,
  API_Scrpt_FindLevelGoalName,
  API_Obj_FindType,
  API_LGoal_Value,
  API_Obj_MakeListOfType,
  API_Obj_Kill,
  API_AI_IsDestReachable,
  API_AI_IsObjReachable,
  API_Game_GetDiffLevel,
  API_Game_GetLanguage,
  API_Path_Value,
  API_COUNT
};
static_assert(API_COUNT == 123, "the OSIRIS function table has 123 entries");

// Host functions at the indices that can be checked against Osiris_module_init.
// If these do not match, the table order changed and sandboxed scripts would call
// the wrong functions, so sandbox loading is refused.
struct ExpectedEntry {
  Api index;
  void *fn;
};
#define EXPECT(idx, fn) {idx, reinterpret_cast<void *>(fn)}
const ExpectedEntry kExpected[] = {
    EXPECT(API_MSafe_CallFunction, msafe_CallFunction),
    EXPECT(API_MSafe_GetValue, msafe_GetValue),
    EXPECT(API_Obj_CallEvent, osipf_CallObjectEvent),
    EXPECT(API_Obj_GetGunPos, osipf_GetGunPos),
    EXPECT(API_FVI_RayCast, osipf_RayCast),
    EXPECT(API_AI_AddGoal, osipf_AIGoalAdd),
    EXPECT(API_File_ReadBytes, osipf_CFReadBytes),
    EXPECT(API_File_WriteDouble, osipf_CFWriteDouble),
    EXPECT(API_Scrpt_MemAlloc, Osiris_AllocateMemory),
    EXPECT(API_Scrpt_CreateTimer, Osiris_CreateTimer),
    EXPECT(API_Obj_Create, osipf_ObjCreate),
    EXPECT(API_Obj_Value, osipf_ObjectValue),
    EXPECT(API_Player_Value, osipf_PlayerValue),
    EXPECT(API_File_Open, osipf_CFopen),
    EXPECT(API_AI_GoalValue, osipf_AIGoalValue),
    EXPECT(API_AI_GetNearbyObjs, osipf_AIGetNearbyObjs),
    EXPECT(API_Cine_Start, Cinematic_Start),
    EXPECT(API_Cine_Stop, Cinematic_Stop),
    EXPECT(API_Scrpt_FindTextureName, osipf_FindTextureName),
    EXPECT(API_Path_GetInformation, osipf_PathGetInformation),
    EXPECT(API_LGoal_Value, osipf_LGoalValue),
    EXPECT(API_Obj_Kill, osipf_ObjKill),
    EXPECT(API_AI_IsObjReachable, osipf_AIIsObjReachable),
    EXPECT(API_Path_Value, osipf_PathValue),
};
#undef EXPECT

bool ApiTableMatches(std::string *why) {
  for (const ExpectedEntry &e : kExpected) {
    if (reinterpret_cast<void *>(Osiris_module_init.fp[e.index]) != e.fn) {
      *why = "OSIRIS function table entry " + std::to_string(e.index) + " is not the expected function";
      return false;
    }
  }
  for (int i = 0; i < MAX_MODULEFUNCS; i++) {
    bool used = Osiris_module_init.fp[i] != nullptr;
    if (used != (i < static_cast<int>(API_COUNT))) {
      *why = "OSIRIS function table does not have exactly " + std::to_string(API_COUNT) + " entries";
      return false;
    }
  }
  return true;
}

// ---------------------------------------------------------------------------
// 32-bit (guest) layouts of the structures that differ from the host layout.
// Offsets are from the original Win32 headers, compiled for i686 MSVC.
// ---------------------------------------------------------------------------
namespace guest {
// tOSIRISModuleInit
constexpr uint32_t kInitFp = 0, kInitStringTable = 1024, kInitStringCount = 1028, kInitModuleId = 1032,
                   kInitIsStatic = 1036, kInitScriptId = 1040, kInitChecksum = 1044, kInitSize = 1048;
// tOSIRISEventInfo: a 20-byte union, then me_handle and extra_info.
constexpr uint32_t kEventUnionSize = 20, kEventMeHandle = 20, kEventExtraInfo = 24, kEventSize = 28;
// msafe_struct: identical to the host up to `list`; the fields after it are shifted.
constexpr uint32_t kMsafeList = 596, kMsafeAfterList = 600, kMsafeSize = 920;
// tGameCinematic
constexpr uint32_t kCineOrient = 32, kCineRoom = 36, kCineMaxTime = 40, kCineCallback = 44, kCineRanges = 48,
                   kCineSize = 88;
// tCannedCinematicInfo
constexpr uint32_t kCannedText = 12, kCannedTargetObj = 16, kCannedSize = 80;
} // namespace guest

static_assert(sizeof(vector) == 12 && sizeof(matrix) == 36, "vector/matrix layout");
static_assert(sizeof(ray_info) == 60, "ray_info layout");
static_assert(sizeof(tOSIRISTIMER) == 24 && sizeof(tOSIRISMEMCHUNK) == 16, "timer/memchunk layout");
static_assert(sizeof(tKillObjectItem) == 12, "tKillObjectItem layout");
static_assert(offsetof(msafe_struct, list) >= guest::kMsafeList, "msafe_struct layout");
static_assert(sizeof(msafe_struct) - offsetof(msafe_struct, control_mask) == guest::kMsafeSize - guest::kMsafeAfterList,
              "msafe_struct tail layout");
static_assert(offsetof(tGameCinematic, orient) == guest::kCineOrient, "tGameCinematic layout");
static_assert(sizeof(tGameCinematic) - offsetof(tGameCinematic, text_display) == guest::kCineSize - guest::kCineRanges,
              "tGameCinematic tail layout");
static_assert(sizeof(tCannedCinematicInfo) - offsetof(tCannedCinematicInfo, target_objhandle) ==
                  guest::kCannedSize - guest::kCannedTargetObj,
              "tCannedCinematicInfo tail layout");

constexpr uint32_t kFileHandleBase = 0x7f000000u; // guest-visible file handles: kFileHandleBase + index
constexpr uint32_t kOmmsHandleBase = 1;
constexpr uint32_t kArenaSize = 256 * 1024;  // guest scratch for marshalled arguments
constexpr uint32_t kMaxString = 4096;
constexpr uint32_t kValueScratch = 4096;   // largest value any *_Value function writes
constexpr uint32_t kMaxList = 4096;        // largest handle list a script may ask for
constexpr uint32_t kInstructionBudget = 50000000u;

class Module;
Module *g_slots[OSIRIS_SANDBOX_MAX_SLOTS];

// The cinematic callback the engine holds is a host function; it forwards to the
// guest callback of the module that started the cinematic.
struct {
  Module *module = nullptr;
  uint32_t guest_fn = 0;
} g_cine;
matrix g_cine_orient;

// Reads stack arguments of an API call. Small integer types occupy a full 32-bit
// slot; only their low bits are meaningful.
class Args {
public:
  Args(Sandbox &sb, uint32_t esp) : sb_(sb), at_(esp + 4) {}
  uint32_t u32() {
    uint32_t v = 0;
    if (!sb_.Read32(at_, &v))
      sb_.AbortGuest(Status::BadArgs);
    at_ += 4;
    return v;
  }
  int32_t i32() { return static_cast<int32_t>(u32()); }
  uint8_t u8() { return static_cast<uint8_t>(u32()); }
  int8_t i8() { return static_cast<int8_t>(u32()); }
  char c() { return static_cast<char>(u32()); }
  int16_t i16() { return static_cast<int16_t>(u32()); }
  uint16_t u16() { return static_cast<uint16_t>(u32()); }
  bool b() { return (u32() & 0xff) != 0; }
  float f32() {
    uint32_t bits = u32();
    float f;
    memcpy(&f, &bits, 4);
    return f;
  }
  double f64() {
    uint32_t lo = u32(), hi = u32();
    uint64_t bits = (static_cast<uint64_t>(hi) << 32) | lo;
    double d;
    memcpy(&d, &bits, 8);
    return d;
  }
  uint32_t addr() const { return at_; } // guest address of the next argument

private:
  Sandbox &sb_;
  uint32_t at_;
};

// A host copy of a guest object, written back when it goes out of scope if `out`.
template <class T> class GuestObj {
public:
  GuestObj(Sandbox &sb, uint32_t addr, bool out) : sb_(sb), addr_(addr), out_(out) {
    if (addr_ && !sb_.Read(addr_, &value_, sizeof(T))) {
      sb_.AbortGuest(Status::BadArgs);
      addr_ = 0;
    }
  }
  ~GuestObj() {
    if (addr_ && out_)
      sb_.Write(addr_, &value_, sizeof(T));
  }
  T *get() { return addr_ ? &value_ : nullptr; }
  GuestObj(const GuestObj &) = delete;
  GuestObj &operator=(const GuestObj &) = delete;

private:
  Sandbox &sb_;
  uint32_t addr_;
  bool out_;
  T value_{};
};
using GVec = GuestObj<vector>;

class Module : public x86sandbox::HostCallHandler {
public:
  Module(int slot, std::string name) : slot_(slot), name_(std::move(name)) {}
  ~Module() override;
  bool Load(const std::vector<uint8_t> &image, std::string *error);

  // The nine module entry points
  char InitializeDLL(tOSIRISModuleInit *mi);
  void ShutdownDLL();
  int GetGOScriptID(const char *name, uint8_t isdoor);
  void *CreateInstance(int id);
  void DestroyInstance(int id, void *ptr);
  int16_t CallInstanceEvent(int id, void *ptr, int event, tOSIRISEventInfo *data);
  int GetTriggerScriptID(int room, int face);
  int GetCOScriptList(int **list, int **id_list);
  int SaveRestoreState(void *file_ptr, uint8_t saving);

  int32_t HostCall(Sandbox &sb, uint32_t index, uint32_t esp) override;

  void CineCallback(int type);
  void SyncChunk(void *host, int size);
  void ForgetChunk(void *host);

private:
  struct Mirror {
    uint32_t guest;
    void *host;
    uint32_t size;
  };
  struct OmmsBlock {
    uint32_t uid;
    std::string script;
    uint32_t guest;
    uint32_t size;
    uint16_t refs;
    bool freed;
  };

  // Guest scratch memory for marshalled arguments; nested calls stack on it.
  class ArenaScope {
  public:
    explicit ArenaScope(Module &m) : m_(m), saved_(m.arena_top_) {}
    ~ArenaScope() { m_.arena_top_ = saved_; }
    uint32_t Alloc(uint32_t size) {
      size = (size + 15u) & ~15u;
      if (m_.arena_top_ + size > m_.arena_end_ || m_.arena_top_ + size < m_.arena_top_)
        return 0;
      uint32_t a = m_.arena_top_;
      m_.arena_top_ += size;
      uint8_t *p = m_.sb_->Ptr(a, size);
      if (p)
        memset(p, 0, size);
      return a;
    }

  private:
    Module &m_;
    uint32_t saved_;
  };

  bool Run(const char *what, uint32_t fn, std::initializer_list<uint32_t> args, uint32_t *eax);
  void Fail(const char *what, Status s);
  bool Alive() const { return sb_ && !failed_; }

  std::string ReadStr(uint32_t addr);
  uint32_t PutStr(ArenaScope &arena, const char *s);
  CFILE *File(uint32_t handle);
  uint32_t AddFile(CFILE *f);
  void RemoveFile(uint32_t handle);
  template <class F> void WithValue(uint32_t ptr, F &&fn);
  void ToHostEvent(uint32_t gaddr, int event, tOSIRISEventInfo *ei);
  void FromHostEvent(const tOSIRISEventInfo *ei, int event, uint32_t gaddr, uint32_t temp_file);
  void CopyEventBack(uint32_t gaddr, int event, tOSIRISEventInfo *ei);
  bool ToHostMsafe(uint32_t gaddr, msafe_struct *m);
  void FromHostMsafe(const msafe_struct *m, uint32_t gaddr);
  int32_t CallEventApi(Args &a, bool trigger);
  int32_t AddGoal(Args &a);
  int32_t MonoPrintf(Args &a);
  int32_t CineStart(Args &a);
  int32_t CannedCine(Args &a);
  int32_t FileOpen(Args &a);

  int slot_;
  std::string name_;
  std::unique_ptr<Sandbox> sb_;
  bool failed_ = false;
  uint32_t exports_[9] = {};
  uint32_t init_struct_ = 0;
  uint32_t arena_ = 0, arena_top_ = 0, arena_end_ = 0;
  std::vector<CFILE *> files_;
  std::vector<Mirror> mirrors_;
  std::vector<OmmsBlock> omms_;
  std::vector<int> co_list_, co_ids_;
  std::vector<std::string> strings_;
  std::string cine_text_;
  uint32_t pending_extra_info_ = 0;
  bool warned_omms_ = false;
  bool warned_file_ = false;
};

enum { E_INIT, E_SHUTDOWN, E_GETGO, E_CREATE, E_DESTROY, E_EVENT, E_GETTRIG, E_GETCO, E_SAVERESTORE };
const struct {
  const char *name;
  int arg_bytes;
} kExports[9] = {
    {"InitializeDLL", 4},   {"ShutdownDLL", 0},        {"GetGOScriptID", 8},
    {"CreateInstance", 4},  {"DestroyInstance", 8},    {"CallInstanceEvent", 16},
    {"GetTriggerScriptID", 8}, {"GetCOScriptList", 8}, {"SaveRestoreState", 8},
};

Module::~Module() {
  for (CFILE *f : files_)
    if (f)
      cfclose(f);
  // Memory chunks stay with the engine, which frees them at the end of the level.
  if (g_cine.module == this)
    g_cine = {};
}

bool Module::Load(const std::vector<uint8_t> &image, std::string *error) {
  std::string why;
  if (!ApiTableMatches(&why)) {
    *error = why;
    return false;
  }
  sb_ = Sandbox::Create(Sandbox::kDefaultGuestMemory, error);
  if (!sb_)
    return false;
  sb_->SetHandler(this);
  sb_->SetInstructionBudget(kInstructionBudget);
  Status s = sb_->Load(image.data(), image.size());
  if (s != Status::Ok) {
    *error = sb_->LastError();
    return false;
  }
  for (auto &imp : sb_->Imports())
    if (!imp.second)
      LOG_WARNING.printf("OSIRIS sandbox: %s imports %s, which is not implemented; calling it stops the script",
                         name_.c_str(), imp.first.c_str());
  for (int i = 0; i < 9; i++) {
    uint32_t a = sb_->FindExport(kExports[i].name);
    if (!a) {
      std::string decorated = std::string("_") + kExports[i].name + "@" + std::to_string(kExports[i].arg_bytes);
      a = sb_->FindExport(decorated.c_str());
    }
    if (!a) {
      *error = std::string("missing export ") + kExports[i].name;
      return false;
    }
    exports_[i] = a;
  }
  arena_ = sb_->Alloc(kArenaSize);
  init_struct_ = sb_->Alloc(guest::kInitSize);
  if (!arena_ || !init_struct_) {
    *error = "out of guest memory";
    return false;
  }
  arena_top_ = arena_;
  arena_end_ = arena_ + kArenaSize;
  return true;
}

void Module::Fail(const char *what, Status s) {
  if (!failed_)
    LOG_ERROR.printf("OSIRIS sandbox: %s: %s failed (%s): %s. The script is disabled.", name_.c_str(), what,
                     x86sandbox::StatusName(s), sb_ ? sb_->LastError().c_str() : "");
  failed_ = true;
}

bool Module::Run(const char *what, uint32_t fn, std::initializer_list<uint32_t> args, uint32_t *eax) {
  if (!Alive())
    return false;
  Status s = sb_->Call(fn, args, eax);
  if (s != Status::Ok) {
    Fail(what, s);
    return false;
  }
  return true;
}

std::string Module::ReadStr(uint32_t addr) {
  std::string s;
  if (addr && !sb_->ReadString(addr, s, kMaxString))
    sb_->AbortGuest(Status::BadArgs);
  return s;
}

uint32_t Module::PutStr(ArenaScope &arena, const char *s) {
  if (!s)
    return 0;
  uint32_t n = static_cast<uint32_t>(strlen(s)) + 1;
  uint32_t a = arena.Alloc(n);
  if (a)
    sb_->Write(a, s, n);
  return a;
}

CFILE *Module::File(uint32_t handle) {
  uint32_t i = handle - kFileHandleBase;
  if (handle < kFileHandleBase || i >= files_.size() || !files_[i]) {
    sb_->AbortGuest(Status::BadArgs);
    return nullptr;
  }
  return files_[i];
}

uint32_t Module::AddFile(CFILE *f) {
  for (size_t i = 0; i < files_.size(); i++)
    if (!files_[i]) {
      files_[i] = f;
      return kFileHandleBase + static_cast<uint32_t>(i);
    }
  files_.push_back(f);
  return kFileHandleBase + static_cast<uint32_t>(files_.size() - 1);
}

void Module::RemoveFile(uint32_t handle) {
  uint32_t i = handle - kFileHandleBase;
  if (handle >= kFileHandleBase && i < files_.size())
    files_[i] = nullptr;
}

// For the *_Value functions, whose `ptr` points at data of a size that depends on
// the value type: the engine works on a host copy, and only the bytes it changed
// are written back to the guest.
template <class F> void Module::WithValue(uint32_t ptr, F &&fn) {
  if (ptr == 0) {
    fn(nullptr);
    return;
  }
  // The extra zero bytes after the guest data end any string the engine reads.
  alignas(16) uint8_t buf[kValueScratch + 16];
  alignas(16) uint8_t orig[kValueScratch + 16];
  uint32_t avail = 0;
  if (ptr < sb_->GuestSize())
    avail = std::min<uint32_t>(kValueScratch, sb_->GuestSize() - ptr);
  memset(buf, 0, sizeof(buf));
  if (avail == 0 || !sb_->Read(ptr, buf, avail)) {
    sb_->AbortGuest(Status::BadArgs);
    return;
  }
  memcpy(orig, buf, sizeof(buf));
  fn(buf);
  for (uint32_t i = 0; i < avail; i++)
    if (buf[i] != orig[i])
      sb_->Write(ptr + i, &buf[i], 1);
}

// ---- tOSIRISEventInfo ----

void Module::ToHostEvent(uint32_t gaddr, int event, tOSIRISEventInfo *ei) {
  memset(ei, 0, sizeof(*ei));
  uint8_t g[guest::kEventSize];
  if (!sb_->Read(gaddr, g, sizeof(g))) {
    sb_->AbortGuest(Status::BadArgs);
    return;
  }
  if (event == EVT_SAVESTATE || event == EVT_RESTORESTATE) {
    // A script passing a file handle to another script: only valid in this module.
    ei->evt_savestate.fileptr = nullptr;
  } else if (event == EVT_MEMRESTORE) {
    memcpy(&ei->evt_memrestore.id, g, 4);
    ei->evt_memrestore.memory_ptr = nullptr;
  } else {
    memcpy(ei, g, guest::kEventUnionSize);
  }
  memcpy(&ei->me_handle, g + guest::kEventMeHandle, 4);
  uint32_t extra;
  memcpy(&extra, g + guest::kEventExtraInfo, 4);
  // Guest pointers mean nothing to native scripts; pass them only within this sandbox.
  pending_extra_info_ = extra;
  ei->extra_info = nullptr;
}

void Module::CopyEventBack(uint32_t gaddr, int event, tOSIRISEventInfo *ei) {
  if (event == EVT_SAVESTATE || event == EVT_RESTORESTATE || event == EVT_MEMRESTORE)
    return;
  sb_->Write(gaddr, ei, guest::kEventUnionSize);
}

void Module::FromHostEvent(const tOSIRISEventInfo *ei, int event, uint32_t gaddr, uint32_t temp_file) {
  uint8_t g[guest::kEventSize] = {};
  if (ei) {
    if (event == EVT_SAVESTATE || event == EVT_RESTORESTATE) {
      memcpy(g, &temp_file, 4);
    } else if (event == EVT_MEMRESTORE) {
      memcpy(g, &ei->evt_memrestore.id, 4);
      uint32_t guest_mem = 0;
      void *host = ei->evt_memrestore.memory_ptr;
      for (const Mirror &m : mirrors_)
        if (m.host == host)
          guest_mem = m.guest;
      if (!guest_mem && host) {
        int size = Osiris_GetMemoryChunkSize(host);
        if (size > 0 && (guest_mem = sb_->Alloc(static_cast<uint32_t>(size))) != 0) {
          sb_->Write(guest_mem, host, static_cast<uint32_t>(size));
          mirrors_.push_back({guest_mem, host, static_cast<uint32_t>(size)});
        }
      }
      memcpy(g + 4, &guest_mem, 4);
    } else {
      memcpy(g, ei, guest::kEventUnionSize);
    }
    memcpy(g + guest::kEventMeHandle, &ei->me_handle, 4);
    // extra_info from the host is a host pointer unless it came from this sandbox.
    uint32_t extra = ei->extra_info == nullptr ? pending_extra_info_ : 0;
    memcpy(g + guest::kEventExtraInfo, &extra, 4);
  }
  sb_->Write(gaddr, g, sizeof(g));
}

// ---- msafe_struct ----

bool Module::ToHostMsafe(uint32_t gaddr, msafe_struct *m) {
  uint8_t g[guest::kMsafeSize];
  memset(m, 0, sizeof(*m));
  if (!sb_->Read(gaddr, g, sizeof(g))) {
    sb_->AbortGuest(Status::BadArgs);
    return false;
  }
  memcpy(m, g, guest::kMsafeList);
  memcpy(reinterpret_cast<uint8_t *>(m) + offsetof(msafe_struct, control_mask), g + guest::kMsafeAfterList,
         guest::kMsafeSize - guest::kMsafeAfterList);
  uint32_t list;
  memcpy(&list, g + guest::kMsafeList, 4);
  // The engine treats these as C strings.
  m->message[sizeof(m->message) - 1] = '\0';
  m->name[sizeof(m->name) - 1] = '\0';
  m->message2[sizeof(m->message2) - 1] = '\0';
  if (list) {
    // The only list the engine reads is MSAFE_OBJECT_DESTROY_ROBOTS_EXCEPT's tKillObjectItem array.
    if (m->count < 0 || m->count > static_cast<int>(kMaxList)) {
      sb_->AbortGuest(Status::BadArgs);
      return false;
    }
    m->list = sb_->Ptr(list, static_cast<uint32_t>(m->count) * sizeof(tKillObjectItem));
    if (!m->list && m->count) {
      sb_->AbortGuest(Status::BadArgs);
      return false;
    }
  }
  return true;
}

void Module::FromHostMsafe(const msafe_struct *m, uint32_t gaddr) {
  uint8_t *g = sb_->Ptr(gaddr, guest::kMsafeSize);
  if (!g)
    return;
  memcpy(g, m, guest::kMsafeList);
  memcpy(g + guest::kMsafeAfterList, reinterpret_cast<const uint8_t *>(m) + offsetof(msafe_struct, control_mask),
         guest::kMsafeSize - guest::kMsafeAfterList);
}

// ---- Module entry points (called by the engine through the trampolines) ----

char Module::InitializeDLL(tOSIRISModuleInit *mi) {
  if (!Alive())
    return 0;
  uint8_t g[guest::kInitSize] = {};
  for (uint32_t i = 0; i < API_COUNT; i++) {
    uint32_t stub = sb_->ApiStub(i);
    memcpy(g + guest::kInitFp + 4 * i, &stub, 4);
  }
  // String table: copied into guest memory once, for the life of the module.
  uint32_t table = 0;
  if (mi->string_table && mi->string_count > 0) {
    table = sb_->Alloc(4u * static_cast<uint32_t>(mi->string_count));
    for (int i = 0; table && i < mi->string_count; i++) {
      const char *s = mi->string_table[i] ? mi->string_table[i] : "";
      uint32_t n = static_cast<uint32_t>(strlen(s)) + 1;
      uint32_t a = sb_->Alloc(n);
      if (a)
        sb_->Write(a, s, n);
      sb_->Write32(table + 4 * i, a);
    }
  }
  uint32_t script_id = 0;
  if (mi->script_identifier) {
    uint32_t n = static_cast<uint32_t>(strlen(mi->script_identifier)) + 1;
    script_id = sb_->Alloc(n);
    if (script_id)
      sb_->Write(script_id, mi->script_identifier, n);
  }
  int32_t count = table ? mi->string_count : 0, module_id = mi->module_identifier;
  uint32_t checksum = OSIRIS_SANDBOX_GUEST_CHECKSUM;
  memcpy(g + guest::kInitStringTable, &table, 4);
  memcpy(g + guest::kInitStringCount, &count, 4);
  memcpy(g + guest::kInitModuleId, &module_id, 4);
  g[guest::kInitIsStatic] = mi->module_is_static ? 1 : 0;
  memcpy(g + guest::kInitScriptId, &script_id, 4);
  memcpy(g + guest::kInitChecksum, &checksum, 4);
  sb_->Write(init_struct_, g, sizeof(g));

  uint32_t ok = 0;
  if (!Run("InitializeDLL", exports_[E_INIT], {init_struct_}, &ok))
    return 0;
  uint8_t is_static = 0;
  sb_->Read(init_struct_ + guest::kInitIsStatic, &is_static, 1);
  mi->module_is_static = is_static != 0;
  return static_cast<char>(ok & 0xff);
}

void Module::ShutdownDLL() {
  if (!Alive())
    return;
  Run("ShutdownDLL", exports_[E_SHUTDOWN], {}, nullptr);
  if (Alive())
    sb_->Unload();
}

int Module::GetGOScriptID(const char *name, uint8_t isdoor) {
  if (!Alive())
    return -1;
  ArenaScope arena(*this);
  uint32_t n = PutStr(arena, name);
  uint32_t r = 0;
  if (!Run("GetGOScriptID", exports_[E_GETGO], {n, isdoor}, &r))
    return -1;
  return static_cast<int32_t>(r);
}

void *Module::CreateInstance(int id) {
  if (!Alive())
    return nullptr;
  uint32_t r = 0;
  if (!Run("CreateInstance", exports_[E_CREATE], {static_cast<uint32_t>(id)}, &r))
    return nullptr;
  return reinterpret_cast<void *>(static_cast<uintptr_t>(r));
}

void Module::DestroyInstance(int id, void *ptr) {
  if (!Alive())
    return;
  Run("DestroyInstance", exports_[E_DESTROY],
      {static_cast<uint32_t>(id), static_cast<uint32_t>(reinterpret_cast<uintptr_t>(ptr))}, nullptr);
}

int16_t Module::CallInstanceEvent(int id, void *ptr, int event, tOSIRISEventInfo *data) {
  if (!Alive())
    return CONTINUE_CHAIN | CONTINUE_DEFAULT;
  ArenaScope arena(*this);
  uint32_t gei = arena.Alloc(guest::kEventSize);
  if (!gei)
    return CONTINUE_CHAIN | CONTINUE_DEFAULT;
  uint32_t temp_file = 0;
  if (data && (event == EVT_SAVESTATE || event == EVT_RESTORESTATE) && data->evt_savestate.fileptr)
    temp_file = AddFile(static_cast<CFILE *>(data->evt_savestate.fileptr));
  FromHostEvent(data, event, gei, temp_file);
  uint32_t r = 0;
  bool ok = Run("CallInstanceEvent", exports_[E_EVENT],
                {static_cast<uint32_t>(id), static_cast<uint32_t>(reinterpret_cast<uintptr_t>(ptr)),
                 static_cast<uint32_t>(event), data ? gei : 0u},
                &r);
  if (temp_file)
    RemoveFile(temp_file);
  if (!ok)
    return CONTINUE_CHAIN | CONTINUE_DEFAULT;
  if (data)
    CopyEventBack(gei, event, data);
  return static_cast<int16_t>(r & 0xffff);
}

int Module::GetTriggerScriptID(int room, int face) {
  if (!Alive())
    return -1;
  uint32_t r = 0;
  if (!Run("GetTriggerScriptID", exports_[E_GETTRIG], {static_cast<uint32_t>(room), static_cast<uint32_t>(face)},
           &r))
    return -1;
  return static_cast<int32_t>(r);
}

int Module::GetCOScriptList(int **list, int **id_list) {
  *list = nullptr;
  *id_list = nullptr;
  if (!Alive())
    return 0;
  ArenaScope arena(*this);
  uint32_t out = arena.Alloc(8);
  uint32_t n = 0;
  if (!out || !Run("GetCOScriptList", exports_[E_GETCO], {out, out + 4}, &n))
    return 0;
  int32_t count = static_cast<int32_t>(n);
  uint32_t gl = 0, gi = 0;
  sb_->Read32(out, &gl);
  sb_->Read32(out + 4, &gi);
  if (count <= 0 || count > static_cast<int32_t>(kMaxList))
    return 0;
  co_list_.assign(count, 0);
  co_ids_.assign(count, 0);
  if (!sb_->Read(gl, co_list_.data(), 4u * count) || !sb_->Read(gi, co_ids_.data(), 4u * count)) {
    Fail("GetCOScriptList", Status::BadArgs);
    return 0;
  }
  *list = co_list_.data();
  *id_list = co_ids_.data();
  return count;
}

int Module::SaveRestoreState(void *file_ptr, uint8_t saving) {
  if (!Alive())
    return 0;
  uint32_t h = file_ptr ? AddFile(static_cast<CFILE *>(file_ptr)) : 0;
  uint32_t r = 0;
  bool ok = Run("SaveRestoreState", exports_[E_SAVERESTORE], {h, saving}, &r);
  if (h)
    RemoveFile(h);
  return ok ? static_cast<int32_t>(r) : 0;
}

void Module::CineCallback(int type) {
  if (Alive() && g_cine.guest_fn) {
    Status s = sb_->CallPointer(g_cine.guest_fn, {static_cast<uint32_t>(type)});
    if (s != Status::Ok)
      Fail("cinematic callback", s);
  }
}

void Module::SyncChunk(void *host, int size) {
  if (!sb_)
    return;
  for (const Mirror &m : mirrors_)
    if (m.host == host)
      sb_->Read(m.guest, host, std::min<uint32_t>(m.size, static_cast<uint32_t>(size)));
}

void Module::ForgetChunk(void *host) {
  for (size_t i = 0; i < mirrors_.size(); i++)
    if (mirrors_[i].host == host) {
      if (sb_)
        sb_->Free(mirrors_[i].guest);
      mirrors_.erase(mirrors_.begin() + i);
      return;
    }
}

// ---- API functions that need more than argument conversion ----

int32_t Module::CallEventApi(Args &a, bool trigger) {
  int32_t num = a.i32(), event = a.i32();
  uint32_t gei = a.u32();
  tOSIRISEventInfo ei;
  uint32_t outer_extra = pending_extra_info_;
  if (gei)
    ToHostEvent(gei, event, &ei);
  else
    memset(&ei, 0, sizeof(ei));
  bool r = trigger ? osipf_CallTriggerEvent(num, event, gei ? &ei : nullptr)
                   : osipf_CallObjectEvent(num, event, gei ? &ei : nullptr);
  if (gei)
    CopyEventBack(gei, event, &ei);
  pending_extra_info_ = outer_extra;
  return r ? 1 : 0;
}

int32_t Module::AddGoal(Args &a) {
  int objhandle = a.i32(), goal_type = a.i32(), level = a.i32();
  float influence = a.f32();
  int guid = a.i32(), flags = a.i32();
  int r = -1;
  switch (goal_type) {
  case AIG_GET_AWAY_FROM_OBJ:
  case AIG_GET_TO_OBJ:
  case AIG_GUARD_OBJ:
  case AIG_DODGE_OBJ:
  case AIG_MOVE_AROUND_OBJ:
  case AIG_MOVE_RELATIVE_OBJ:
  case AIG_GET_AROUND_OBJ:
  case AIG_FIRE_AT_OBJ:
  case AIG_SET_ANIM:
  case AIG_DO_MELEE_ANIM:
  case AIG_USE_MOVEMENT_TYPE:
  case AIG_SCRIPTED:
  case AIG_WANDER_AROUND: {
    int v = a.i32();
    r = osipf_AIGoalAdd(objhandle, goal_type, level, influence, guid, flags, v);
  } break;
  case AIG_FOLLOW_PATH: {
    int p = a.i32(), s = a.i32(), e = a.i32(), n = a.i32();
    r = osipf_AIGoalAdd(objhandle, goal_type, level, influence, guid, flags, p, s, e, n);
  } break;
  case AIG_ATTACH_TO_OBJ:
  case AIG_PLACE_OBJ_ON_OBJ: {
    int h = a.i32(), pap = a.i32(), cap = a.i32();
    double rad = a.f64();
    int aligned = a.i32(), sphere = a.i32();
    r = osipf_AIGoalAdd(objhandle, goal_type, level, influence, guid, flags, h, pap, cap, rad, aligned, sphere);
  } break;
  case AIG_MOVE_RELATIVE_OBJ_VEC:
  case AIG_HIDE_FROM_OBJ: {
    int h = a.i32(), v = a.i32();
    r = osipf_AIGoalAdd(objhandle, goal_type, level, influence, guid, flags, h, v);
  } break;
  case AIG_GUARD_AREA:
  case AIG_GET_TO_POS: {
    uint32_t pos = a.u32();
    int room = a.i32();
    GVec v(*sb_, pos, false);
    vector zero{};
    r = osipf_AIGoalAdd(objhandle, goal_type, level, influence, guid, flags, v.get() ? v.get() : &zero, room);
  } break;
  case AIG_FACE_DIR: {
    GVec v(*sb_, a.u32(), false);
    vector zero{};
    r = osipf_AIGoalAdd(objhandle, goal_type, level, influence, guid, flags, v.get() ? v.get() : &zero);
  } break;
  default:
    r = osipf_AIGoalAdd(objhandle, goal_type, level, influence, guid, flags);
    break;
  }
  // The scripts' header declares this function as returning float.
  sb_->ReturnDouble(static_cast<double>(r));
  return r;
}

int32_t Module::MonoPrintf(Args &a) {
  a.i32(); // window number
  std::string fmt = ReadStr(a.u32());
  std::string out;
  uint32_t arg = a.addr();
  for (size_t i = 0; i < fmt.size() && out.size() < 1024; i++) {
    if (fmt[i] != '%') {
      out.push_back(fmt[i]);
      continue;
    }
    size_t j = i + 1;
    while (j < fmt.size() && strchr("-+ #0123456789.lhI", fmt[j]))
      j++;
    if (j >= fmt.size())
      break;
    char conv = fmt[j];
    std::string spec = fmt.substr(i, j - i);
    spec.erase(std::remove_if(spec.begin(), spec.end(), [](char ch) { return ch == 'l' || ch == 'h' || ch == 'I'; }),
               spec.end());
    char buf[512] = {};
    uint32_t v = 0;
    switch (conv) {
    case 'd':
    case 'i':
    case 'u':
    case 'x':
    case 'X':
    case 'c':
      sb_->Read32(arg, &v);
      arg += 4;
      snprintf(buf, sizeof(buf), (spec + conv).c_str(), v);
      break;
    case 'f':
    case 'g':
    case 'e':
    case 'G':
    case 'E': {
      double d = 0;
      sb_->Read(arg, &d, 8);
      arg += 8;
      snprintf(buf, sizeof(buf), (spec + conv).c_str(), d);
    } break;
    case 's':
      sb_->Read32(arg, &v);
      arg += 4;
      snprintf(buf, sizeof(buf), "%.400s", ReadStr(v).c_str());
      break;
    case '%':
      buf[0] = '%';
      break;
    default:
      break;
    }
    out += buf;
    i = j;
  }
  LOG_DEBUG.printf("OSIRIS sandbox %s: %s", name_.c_str(), out.c_str());
  return 0;
}

int32_t Module::CineStart(Args &a) {
  uint32_t ginfo = a.u32(), gtext = a.u32();
  uint8_t g[guest::kCineSize];
  if (!ginfo || !sb_->Read(ginfo, g, sizeof(g))) {
    sb_->AbortGuest(Status::BadArgs);
    return 0;
  }
  tGameCinematic info{};
  memcpy(&info, g, guest::kCineOrient); // flags .. position
  uint32_t orient, callback;
  memcpy(&orient, g + guest::kCineOrient, 4);
  memcpy(&info.room, g + guest::kCineRoom, 4);
  memcpy(&info.max_time_play, g + guest::kCineMaxTime, 4);
  memcpy(&callback, g + guest::kCineCallback, 4);
  memcpy(&info.text_display, g + guest::kCineRanges, guest::kCineSize - guest::kCineRanges);
  if (orient) {
    if (!sb_->Read(orient, &g_cine_orient, sizeof(matrix))) {
      sb_->AbortGuest(Status::BadArgs);
      return 0;
    }
    info.orient = &g_cine_orient;
  }
  g_cine.module = callback ? this : nullptr;
  g_cine.guest_fn = callback;
  if (callback) {
    info.callback = [](int type) {
      if (g_cine.module)
        g_cine.module->CineCallback(type);
    };
  }
  cine_text_ = ReadStr(gtext);
  return Cinematic_Start(&info, cine_text_.data()) ? 1 : 0;
}

int32_t Module::CannedCine(Args &a) {
  uint32_t ginfo = a.u32();
  uint8_t g[guest::kCannedSize];
  if (!ginfo || !sb_->Read(ginfo, g, sizeof(g))) {
    sb_->AbortGuest(Status::BadArgs);
    return 0;
  }
  tCannedCinematicInfo info{};
  memcpy(&info, g, guest::kCannedText); // type, camera_pathid, target_pathid
  uint32_t text;
  memcpy(&text, g + guest::kCannedText, 4);
  memcpy(&info.target_objhandle, g + guest::kCannedTargetObj, guest::kCannedSize - guest::kCannedTargetObj);
  cine_text_ = ReadStr(text);
  info.text_to_display = text ? cine_text_.c_str() : nullptr;
  Cinematic_StartCannedScript(&info);
  return 0;
}

int32_t Module::FileOpen(Args &a) {
  std::string name = ReadStr(a.u32()), mode = ReadStr(a.u32());
  bool bare = !name.empty() && name.size() < 64 && name.find_first_of("/\\:") == std::string::npos &&
              name.find("..") == std::string::npos;
  bool read_only = mode == "rb" || mode == "rt";
  if (!bare || !read_only) {
    if (!warned_file_)
      LOG_WARNING.printf("OSIRIS sandbox: %s tried to open \"%s\" with mode \"%s\"; scripts may only read files "
                         "by bare name",
                         name_.c_str(), name.c_str(), mode.c_str());
    warned_file_ = true;
    return 0;
  }
  CFILE *f = cfopen(name, mode.c_str());
  return f ? static_cast<int32_t>(AddFile(f)) : 0;
}

// ---- The API dispatcher ----

int32_t Module::HostCall(Sandbox &sb, uint32_t index, uint32_t esp) {
  Args a(sb, esp);
  auto str = [&](uint32_t p) { return ReadStr(p); };
  switch (static_cast<Api>(index)) {
  case API_mprintf:
    return MonoPrintf(a);
  case API_MSafe_CallFunction:
  case API_MSafe_GetValue:
  case API_MSafe_DoPowerup: {
    int type = index == API_MSafe_DoPowerup ? 0 : a.i32();
    uint32_t gm = a.u32();
    msafe_struct m;
    if (!gm || !ToHostMsafe(gm, &m)) {
      sb.AbortGuest(Status::BadArgs);
      return 0;
    }
    if (index == API_MSafe_CallFunction)
      msafe_CallFunction(static_cast<uint8_t>(type), &m);
    else if (index == API_MSafe_GetValue)
      msafe_GetValue(type, &m);
    else
      msafe_DoPowerup(&m);
    FromHostMsafe(&m, gm);
    return 0;
  }
  case API_Obj_CallEvent:
    return CallEventApi(a, false);
  case API_Trgr_CallEvent:
    return CallEventApi(a, true);
  case API_Sound_TouchFile: {
    std::string s = str(a.u32());
    osipf_SoundTouch(s.data());
    return 0;
  }
  case API_Obj_FindID:
    return osipf_ObjectFindID(str(a.u32()).c_str());
  case API_Wpn_FindID:
    return osipf_WeaponFindID(str(a.u32()).c_str());
  case API_Obj_GetTimeLived:
    sb.ReturnDouble(osipf_ObjectGetTimeLived(a.i32()));
    return 0;
  case API_Obj_GetGunPos:
  case API_Obj_GetGroundPos: {
    int h = a.i32(), n = a.i32();
    GVec pnt(sb, a.u32(), true), normal(sb, a.u32(), true);
    vector dummy;
    if (index == API_Obj_GetGunPos)
      osipf_GetGunPos(h, n, pnt.get() ? pnt.get() : &dummy, normal.get());
    else
      osipf_GetGroundPos(h, n, pnt.get() ? pnt.get() : &dummy, normal.get() ? normal.get() : &dummy);
    return 0;
  }
  case API_Room_Value: {
    int room = a.i32();
    char op = a.c(), vh = a.c();
    uint32_t p = a.u32();
    int idx = a.i32();
    WithValue(p, [&](void *ptr) { osipf_RoomValue(room, op, vh, ptr, idx); });
    return 0;
  }
  case API_Room_IsValid:
    return osipf_IsRoomValid(a.i32());
  case API_Obj_GetAttachParent:
    return osipf_GetAttachParent(a.i32());
  case API_Obj_GetNumAttachSlots:
    return osipf_GetNumAttachSlots(a.i32());
  case API_Obj_GetAttachChildHandle: {
    int h = a.i32();
    return osipf_GetAttachChildHandle(h, a.c());
  }
  case API_Obj_AttachObjectAP: {
    int p = a.i32();
    char pap = a.c();
    int c = a.i32();
    char cap = a.c();
    return osipf_AttachObjectAP(p, pap, c, cap, a.u8());
  }
  case API_Obj_AttachObjectRad: {
    int p = a.i32();
    char pap = a.c();
    int c = a.i32();
    return osipf_AttachObjectRad(p, pap, c, a.f32());
  }
  case API_Obj_UnattachFromParent:
    osipf_UnattachFromParent(a.i32());
    return 0;
  case API_Obj_UnattachChild: {
    int h = a.i32();
    osipf_UnattachChild(h, a.c());
    return 0;
  }
  case API_Obj_UnattachChildren:
    osipf_UnattachChildren(a.i32());
    return 0;
  case API_FVI_RayCast: {
    int h = a.i32();
    GVec p0(sb, a.u32(), false), p1(sb, a.u32(), false);
    int room = a.i32();
    float rad = a.f32();
    int flags = a.i32();
    GuestObj<ray_info> ri(sb, a.u32(), true);
    if (!p0.get() || !p1.get() || !ri.get()) {
      sb.AbortGuest(Status::BadArgs);
      return 0;
    }
    return osipf_RayCast(h, p0.get(), p1.get(), room, rad, flags, ri.get());
  }
  case API_AI_GetPathID: {
    std::string s = str(a.u32());
    return osipf_AIGetPathID(s.data());
  }
  case API_AI_GoalFollowPathSimple: {
    int h = a.i32(), p = a.i32(), g = a.i32(), f = a.i32();
    return osipf_AIGoalFollowPathSimple(h, p, g, f, a.i32());
  }
  case API_AI_PowerSwitch: {
    int h = a.i32();
    return osipf_AIPowerSwitch(h, a.u8());
  }
  case API_AI_TurnTowardsVectors: {
    int h = a.i32();
    GVec f(sb, a.u32(), false), u(sb, a.u32(), false);
    if (!f.get() || !u.get()) {
      sb.AbortGuest(Status::BadArgs);
      return 0;
    }
    return osipf_AITurnTowardsVectors(h, f.get(), u.get());
  }
  case API_AI_SetType: {
    int h = a.i32();
    osipf_AISetType(h, a.i32());
    return 0;
  }
  case API_AI_FindHidePos: {
    // A 12-byte struct is returned through a hidden first argument (MSVC convention).
    uint32_t ret = a.u32();
    int hide = a.i32(), view = a.i32();
    float t = a.f32();
    GuestObj<int> room(sb, a.u32(), true);
    int dummy;
    vector v = osipf_AIFindHidePos(hide, view, t, room.get() ? room.get() : &dummy);
    if (!sb.Write(ret, &v, sizeof(v)))
      sb.AbortGuest(Status::BadArgs);
    return static_cast<int32_t>(ret);
  }
  case API_AI_GoalAddEnabler: {
    int h = a.i32(), gi = a.i32(), et = a.i32();
    float pct = a.f32(), iv = a.f32();
    uint32_t p = a.u32();
    int r = -1;
    WithValue(p, [&](void *ptr) { r = osipf_AIGoalAddEnabler(h, gi, et, pct, iv, ptr); });
    return r;
  }
  case API_AI_AddGoal:
    return AddGoal(a);
  case API_AI_ClearGoal: {
    int h = a.i32();
    osipf_AIGoalClear(h, a.i32());
    return 0;
  }
  case API_AI_Value: {
    int h = a.i32();
    char op = a.c(), vt = a.c();
    uint32_t p = a.u32();
    WithValue(p, [&](void *ptr) { osipf_AIValue(h, op, vt, ptr); });
    return 0;
  }
  case API_AI_FindObjOfType: {
    int h = a.i32(), t = a.i32(), id = a.i32();
    bool ig = a.b();
    return osipf_AIFindObjOfType(h, t, id, ig, a.i32());
  }
  case API_AI_GetRoomPathPoint: {
    uint32_t ret = a.u32();
    vector v = osipf_AIGetRoomPathPoint(a.i32());
    if (!sb.Write(ret, &v, sizeof(v)))
      sb.AbortGuest(Status::BadArgs);
    return static_cast<int32_t>(ret);
  }
  case API_AI_FindEnergyCenter:
    return osipf_AIFindEnergyCenter(a.i32());
  case API_AI_GetDistToObj: {
    int h = a.i32();
    sb.ReturnDouble(osipf_AIGetDistToObj(h, a.i32()));
    return 0;
  }
  case API_AI_SetGoalFlags: {
    int h = a.i32(), g = a.i32(), f = a.i32();
    return osipf_AISetGoalFlags(h, g, f, a.u8());
  }
  case API_AI_SetGoalCircleDist: {
    int h = a.i32(), g = a.i32();
    osipf_AISetGoalCircleDist(h, g, a.f32());
    return 0;
  }
  case API_File_ReadBytes: {
    uint32_t buf = a.u32();
    int count = a.i32();
    CFILE *f = File(a.u32());
    uint8_t *p = count > 0 ? sb.Ptr(buf, static_cast<uint32_t>(count)) : nullptr;
    if (!f || (count > 0 && !p) || count < 0) {
      sb.AbortGuest(Status::BadArgs);
      return 0;
    }
    return count ? osipf_CFReadBytes(p, count, f) : 0;
  }
  case API_File_ReadInt: {
    CFILE *f = File(a.u32());
    return f ? osipf_CFReadInt(f) : 0;
  }
  case API_File_ReadShort: {
    CFILE *f = File(a.u32());
    return f ? osipf_CFReadShort(f) : 0;
  }
  case API_File_ReadByte: {
    CFILE *f = File(a.u32());
    return f ? osipf_CFReadByte(f) : 0;
  }
  case API_File_ReadFloat: {
    CFILE *f = File(a.u32());
    sb.ReturnDouble(f ? osipf_CFReadFloat(f) : 0.0f);
    return 0;
  }
  case API_File_ReadDouble: {
    CFILE *f = File(a.u32());
    sb.ReturnDouble(f ? osipf_CFReadDouble(f) : 0.0);
    return 0;
  }
  case API_File_ReadString: {
    uint32_t buf = a.u32(), n = a.u32();
    CFILE *f = File(a.u32());
    char *p = n ? reinterpret_cast<char *>(sb.Ptr(buf, n)) : nullptr;
    if (!f || (n && !p)) {
      sb.AbortGuest(Status::BadArgs);
      return 0;
    }
    return n ? osipf_CFReadString(p, n, f) : 0;
  }
  case API_File_WriteBytes: {
    uint32_t buf = a.u32();
    int count = a.i32();
    CFILE *f = File(a.u32());
    const uint8_t *p = count > 0 ? sb.Ptr(buf, static_cast<uint32_t>(count)) : nullptr;
    if (!f || (count > 0 && !p) || count < 0) {
      sb.AbortGuest(Status::BadArgs);
      return 0;
    }
    return count ? osipf_CFWriteBytes(p, count, f) : 0;
  }
  case API_File_WriteString: {
    std::string s = str(a.u32());
    CFILE *f = File(a.u32());
    return f ? osipf_CFWriteString(s.c_str(), f) : 0;
  }
  case API_File_WriteInt: {
    int v = a.i32();
    CFILE *f = File(a.u32());
    if (f)
      osipf_CFWriteInt(v, f);
    return 0;
  }
  case API_File_WriteShort: {
    int16_t v = a.i16();
    CFILE *f = File(a.u32());
    if (f)
      osipf_CFWriteShort(v, f);
    return 0;
  }
  case API_File_WriteByte: {
    int8_t v = a.i8();
    CFILE *f = File(a.u32());
    if (f)
      osipf_CFWriteByte(v, f);
    return 0;
  }
  case API_File_WriteFloat: {
    float v = a.f32();
    CFILE *f = File(a.u32());
    if (f)
      osipf_CFWriteFloat(v, f);
    return 0;
  }
  case API_File_WriteDouble: {
    double v = a.f64();
    CFILE *f = File(a.u32());
    if (f)
      osipf_CFWriteDouble(v, f);
    return 0;
  }
  case API_Scrpt_MemAlloc: {
    GuestObj<tOSIRISMEMCHUNK> mc(sb, a.u32(), false);
    if (!mc.get() || mc.get()->size <= 0 || mc.get()->size > (8 << 20)) {
      sb.AbortGuest(Status::BadArgs);
      return 0;
    }
    uint32_t size = static_cast<uint32_t>(mc.get()->size);
    uint32_t g = sb.Alloc(size);
    if (!g)
      return 0;
    void *host = Osiris_AllocateMemory(mc.get());
    if (!host) {
      sb.Free(g);
      return 0;
    }
    mirrors_.push_back({g, host, size});
    return static_cast<int32_t>(g);
  }
  case API_Scrpt_MemFree: {
    uint32_t g = a.u32();
    for (size_t i = 0; i < mirrors_.size(); i++)
      if (mirrors_[i].guest == g) {
        void *host = mirrors_[i].host;
        mirrors_.erase(mirrors_.begin() + i);
        sb.Free(g);
        Osiris_FreeMemory(host);
        break;
      }
    return 0;
  }
  case API_Scrpt_CancelTimer:
    Osiris_CancelTimer(a.i32());
    return 0;
  case API_Scrpt_CreateTimer: {
    GuestObj<tOSIRISTIMER> t(sb, a.u32(), false);
    if (!t.get()) {
      sb.AbortGuest(Status::BadArgs);
      return 0;
    }
    return Osiris_CreateTimer(t.get());
  }
  case API_Obj_Create: {
    uint8_t type = a.u8();
    uint16_t id = a.u16();
    int room = a.i32();
    GVec pos(sb, a.u32(), false);
    GuestObj<matrix> orient(sb, a.u32(), false);
    int parent = a.i32();
    GVec vel(sb, a.u32(), false);
    if (!pos.get()) {
      sb.AbortGuest(Status::BadArgs);
      return 0;
    }
    return osipf_ObjCreate(type, id, room, pos.get(), orient.get(), parent, vel.get());
  }
  case API_Game_GetTime:
    sb.ReturnDouble(osipf_GameTime());
    return 0;
  case API_Game_GetFrameTime:
    sb.ReturnDouble(osipf_FrameTime());
    return 0;
  case API_Obj_WBValue: {
    int h = a.i32();
    int8_t wb = a.i8();
    char op = a.c(), vt = a.c();
    uint32_t p = a.u32();
    int8_t gi = a.i8();
    WithValue(p, [&](void *ptr) { osipf_ObjWBValue(h, wb, op, vt, ptr, gi); });
    return 0;
  }
  case API_Scrpt_TimerExists:
    return Osiris_TimerExists(a.i32());
  case API_Obj_Value: {
    int h = a.i32();
    char op = a.c(), vt = a.c();
    uint32_t p = a.u32();
    int idx = a.i32();
    WithValue(p, [&](void *ptr) { osipf_ObjectValue(h, op, vt, ptr, idx); });
    return 0;
  }
  case API_Matcen_Value: {
    int h = a.i32();
    char op = a.c(), vt = a.c();
    uint32_t p = a.u32();
    int idx = a.i32();
    WithValue(p, [&](void *ptr) { osipf_MatcenValue(h, op, vt, ptr, idx); });
    return 0;
  }
  case API_Matcen_Reset:
    osipf_MatcenReset(a.i32());
    return 0;
  case API_Matcen_Copy: {
    int d = a.i32();
    osipf_MatcenCopy(d, a.i32());
    return 0;
  }
  case API_Matcen_Create: {
    std::string s = str(a.u32());
    return osipf_MatcenCreate(s.data());
  }
  case API_Matcen_FindID: {
    std::string s = str(a.u32());
    return osipf_MatcenFindId(s.data());
  }
  case API_Msn_FlagSet: {
    int f = a.i32();
    osipf_MissionFlagSet(f, a.u8());
    return 0;
  }
  case API_Msn_FlagGet:
    return osipf_MissionFlagGet(a.i32());
  case API_Player_Value: {
    int h = a.i32();
    char op = a.c(), vh = a.c();
    uint32_t p = a.u32();
    int idx = a.i32();
    WithValue(p, [&](void *ptr) { osipf_PlayerValue(h, op, vh, ptr, idx); });
    return 0;
  }
  case API_Obj_SetCustomAnim: {
    int h = a.i32();
    float s = a.f32(), e = a.f32(), t = a.f32();
    char fl = a.c();
    int snd = a.i32();
    osipf_ObjectCustomAnim(h, s, e, t, fl, snd, a.c());
    return 0;
  }
  case API_Player_AddHudMessage: {
    int h = a.i32();
    osipf_PlayerAddHudMessage(h, str(a.u32()).c_str());
    return 0;
  }
  case API_Obj_Ghost: {
    int h = a.i32();
    osipf_ObjGhost(h, a.b());
    return 0;
  }
  case API_Obj_Burning: {
    int h = a.i32();
    float t = a.f32();
    osipf_ObjBurning(h, t, a.f32());
    return 0;
  }
  case API_Obj_IsEffect: {
    int h = a.i32();
    return osipf_ObjIsEffect(h, a.i32()) ? 1 : 0;
  }
  case API_File_Open:
    return FileOpen(a);
  case API_File_Close: {
    uint32_t h = a.u32();
    CFILE *f = File(h);
    if (f) {
      RemoveFile(h);
      osipf_CFclose(f);
    }
    return 0;
  }
  case API_File_Tell: {
    CFILE *f = File(a.u32());
    return f ? osipf_CFtell(f) : 0;
  }
  case API_File_eof: {
    CFILE *f = File(a.u32());
    return f ? osipf_CFeof(f) : 1;
  }
  case API_Sound_Stop: {
    int h = a.i32();
    osipf_SoundStop(h, a.b());
    return 0;
  }
  case API_Sound_Play2d:
  case API_Sound_Play3d: {
    int h = a.i32(), id = a.i32();
    float v = a.f32();
    return index == API_Sound_Play2d ? osipf_SoundPlay2d(h, id, v) : osipf_SoundPlay3d(h, id, v);
  }
  case API_Sound_FindId:
    return osipf_SoundFindId(str(a.u32()).c_str());
  case API_AI_IsObjFriend: {
    int h = a.i32();
    return osipf_AIIsObjFriend(h, a.i32()) ? 1 : 0;
  }
  case API_AI_IsObjEnemy: {
    int h = a.i32();
    return osipf_AIIsObjEnemy(h, a.i32()) ? 1 : 0;
  }
  case API_AI_GoalValue: {
    int h = a.i32();
    int8_t gi = a.i8();
    char op = a.c(), vt = a.c();
    uint32_t p = a.u32();
    int8_t idx = a.i8();
    if (vt == AIGV_I_SCRIPTED_DATA_PTR) {
      // The engine stores this value as a pointer; for the guest it is a 32-bit word.
      uint32_t gv = 0;
      if (op != VF_GET && (!p || !sb.Read32(p, &gv))) {
        sb.AbortGuest(Status::BadArgs);
        return 0;
      }
      void *hv = reinterpret_cast<void *>(static_cast<uintptr_t>(gv));
      osipf_AIGoalValue(h, gi, op, vt, &hv, idx);
      if (op == VF_GET && p)
        sb.Write32(p, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(hv)));
      return 0;
    }
    WithValue(p, [&](void *ptr) { osipf_AIGoalValue(h, gi, op, vt, ptr, idx); });
    return 0;
  }
  case API_AI_GetNearbyObjs: {
    GVec pos(sb, a.u32(), false);
    int room = a.i32();
    float rad = a.f32();
    uint32_t list = a.u32();
    int max = a.i32();
    bool b1 = a.b(), b2 = a.b(), b3 = a.b(), b4 = a.b();
    if (!pos.get() || max < 0 || max > static_cast<int>(kMaxList) ||
        (max > 0 && !sb.Ptr(list, 4u * static_cast<uint32_t>(max)))) {
      sb.AbortGuest(Status::BadArgs);
      return 0;
    }
    std::vector<int> out(std::max(max, 1));
    int n = osipf_AIGetNearbyObjs(pos.get(), room, rad, out.data(), max, b1, b2, b3, b4);
    n = std::clamp(n, 0, max);
    sb.Write(list, out.data(), 4u * static_cast<uint32_t>(n));
    return n;
  }
  case API_AI_GetCurGoalIndex:
    return osipf_AIGetCurGoalIndex(a.i32());
  case API_OMMS_Malloc: {
    uint32_t size = a.u32(), uid = a.u32();
    std::string script = str(a.u32());
    if (!warned_omms_)
      LOG_WARNING.printf("OSIRIS sandbox: %s uses OMMS memory, which is not shared with other modules or saved",
                         name_.c_str());
    warned_omms_ = true;
    for (size_t i = 0; i < omms_.size(); i++)
      if (!omms_[i].freed && omms_[i].uid == uid && omms_[i].script == script)
        return static_cast<int32_t>(kOmmsHandleBase + i);
    if (size == 0 || size > (8u << 20))
      return -1;
    uint32_t g = sb.Alloc(size);
    if (!g)
      return -1;
    omms_.push_back({uid, script, g, size, 0, false});
    return static_cast<int32_t>(kOmmsHandleBase + omms_.size() - 1);
  }
  case API_OMMS_Attach:
  case API_OMMS_Detach:
  case API_OMMS_Free: {
    uint32_t h = a.u32() - kOmmsHandleBase;
    if (h >= omms_.size() || omms_[h].freed)
      return 0;
    OmmsBlock &b = omms_[h];
    if (index == API_OMMS_Attach) {
      b.refs++;
      return static_cast<int32_t>(b.guest);
    }
    if (index == API_OMMS_Detach) {
      if (b.refs)
        b.refs--;
      return 0;
    }
    b.freed = true;
    sb.Free(b.guest);
    return 0;
  }
  case API_OMMS_Find: {
    uint32_t uid = a.u32();
    std::string script = str(a.u32());
    for (size_t i = 0; i < omms_.size(); i++)
      if (!omms_[i].freed && omms_[i].uid == uid && omms_[i].script == script)
        return static_cast<int32_t>(kOmmsHandleBase + i);
    return -1;
  }
  case API_OMMS_GetInfo: {
    uint32_t h = a.u32() - kOmmsHandleBase;
    uint32_t psize = a.u32(), puid = a.u32(), prefs = a.u32(), pfree = a.u32();
    if (h >= omms_.size())
      return 0;
    const OmmsBlock &b = omms_[h];
    uint16_t refs = b.refs;
    uint8_t freed = b.freed ? 1 : 0;
    if (psize)
      sb.Write32(psize, b.size);
    if (puid)
      sb.Write32(puid, b.uid);
    if (prefs)
      sb.Write(prefs, &refs, 2);
    if (pfree)
      sb.Write(pfree, &freed, 1);
    return 1;
  }
  case API_Cine_Start:
    return CineStart(a);
  case API_Cine_Stop:
    Cinematic_Stop();
    return 0;
  case API_Scrpt_FindSoundName:
    return osipf_FindSoundName(str(a.u32()).c_str());
  case API_Scrpt_FindRoomName:
    return osipf_FindRoomName(str(a.u32()).c_str());
  case API_Scrpt_FindTriggerName:
    return osipf_FindTriggerName(str(a.u32()).c_str());
  case API_Scrpt_FindObjectName:
    return osipf_FindObjectName(str(a.u32()).c_str());
  case API_Scrpt_GetTriggerRoom:
    return osipf_GetTriggerRoom(a.i32());
  case API_Scrpt_GetTriggerFace:
    return osipf_GetTriggerFace(a.i32());
  case API_Scrpt_FindDoorName:
    return osipf_FindDoorName(str(a.u32()).c_str());
  case API_Scrpt_FindTextureName:
    return osipf_FindTextureName(str(a.u32()).c_str());
  case API_Game_CreateRandomSparks: {
    int n = a.i32();
    GVec pos(sb, a.u32(), false);
    int room = a.i32(), which = a.i32();
    float force = a.f32();
    if (!pos.get()) {
      sb.AbortGuest(Status::BadArgs);
      return 0;
    }
    osipf_CreateRandomSparks(n, pos.get(), room, which, force);
    return 0;
  }
  case API_Scrpt_CancelTimerID:
    Osiris_CancelTimerID(a.i32());
    return 0;
  case API_Game_EnableShip: {
    std::string s = str(a.u32());
    osipf_EnableShip(s.data(), a.b());
    return 0;
  }
  case API_Game_IsShipEnabled: {
    std::string s = str(a.u32());
    return osipf_IsShipEnabled(s.data()) ? 1 : 0;
  }
  case API_Path_GetInformation: {
    int path = a.i32(), point = a.i32();
    GVec pos(sb, a.u32(), true);
    GuestObj<int> room(sb, a.u32(), true);
    GuestObj<matrix> orient(sb, a.u32(), true);
    return osipf_PathGetInformation(path, point, pos.get(), room.get(), orient.get()) ? 1 : 0;
  }
  case API_Cine_StartCanned:
    return CannedCine(a);
  case API_Scrpt_FindMatcenName:
    return osipf_FindMatcenName(str(a.u32()).c_str());
  case API_Scrpt_FindPathName:
    return osipf_FindPathName(str(a.u32()).c_str());
  case API_Scrpt_FindLevelGoalName:
    return osipf_FindLevelGoalName(str(a.u32()).c_str());
  case API_Obj_FindType:
    return osipf_ObjectFindType(str(a.u32()).c_str());
  case API_LGoal_Value: {
    char op = a.c(), vt = a.c();
    uint32_t p = a.u32();
    int gi = a.i32(), ii = a.i32();
    WithValue(p, [&](void *ptr) { osipf_LGoalValue(op, vt, ptr, gi, ii); });
    return 0;
  }
  case API_Obj_MakeListOfType: {
    int h = a.i32(), t = a.i32(), id = a.i32();
    bool ig = a.b();
    int parent = a.i32(), max = a.i32();
    uint32_t list = a.u32();
    if (max < 0 || max > static_cast<int>(kMaxList) || (max > 0 && !sb.Ptr(list, 4u * static_cast<uint32_t>(max)))) {
      sb.AbortGuest(Status::BadArgs);
      return 0;
    }
    std::vector<int> out(std::max(max, 1));
    int n = osipf_ObjMakeListOfType(h, t, id, ig, parent, max, out.data());
    n = std::clamp(n, 0, max);
    sb.Write(list, out.data(), 4u * static_cast<uint32_t>(n));
    return n;
  }
  case API_Obj_Kill: {
    int h = a.i32(), k = a.i32();
    float d = a.f32();
    int f = a.i32();
    float mn = a.f32(), mx = a.f32();
    osipf_ObjKill(h, k, d, f, mn, mx);
    return 0;
  }
  case API_AI_IsDestReachable: {
    int h = a.i32();
    return osipf_AIIsDestReachable(h, a.i32()) ? 1 : 0;
  }
  case API_AI_IsObjReachable: {
    int h = a.i32();
    return osipf_AIIsObjReachable(h, a.i32()) ? 1 : 0;
  }
  case API_Game_GetDiffLevel:
    return osipf_GameGetDiffLevel();
  case API_Game_GetLanguage:
    return osipf_GetLanguageSetting();
  case API_Path_Value: {
    int path = a.i32(), node = a.i32();
    char op = a.c();
    int changes = a.i32();
    uint32_t p = a.u32();
    WithValue(p, [&](void *ptr) { osipf_PathValue(path, node, op, changes, ptr); });
    return 0;
  }
  case API_COUNT:
    break;
  }
  sb.AbortGuest(Status::BadArgs);
  return 0;
}

// ---------------------------------------------------------------------------
// One set of trampolines per module slot: the engine stores plain function
// pointers, so each slot needs its own functions.
// ---------------------------------------------------------------------------
template <int N> struct Tramp {
  static Module *M() { return g_slots[N]; }
  static char DLLFUNCCALL InitializeDLL(tOSIRISModuleInit *mi) { return M() ? M()->InitializeDLL(mi) : 0; }
  static void DLLFUNCCALL ShutdownDLL() {
    if (M())
      M()->ShutdownDLL();
  }
  static int DLLFUNCCALL GetGOScriptID(const char *name, uint8_t isdoor) {
    return M() ? M()->GetGOScriptID(name, isdoor) : -1;
  }
  static void *DLLFUNCCALL CreateInstance(int id) { return M() ? M()->CreateInstance(id) : nullptr; }
  static void DLLFUNCCALL DestroyInstance(int id, void *ptr) {
    if (M())
      M()->DestroyInstance(id, ptr);
  }
  static int16_t DLLFUNCCALL CallInstanceEvent(int id, void *ptr, int event, tOSIRISEventInfo *data) {
    return M() ? M()->CallInstanceEvent(id, ptr, event, data) : static_cast<int16_t>(CONTINUE_CHAIN | CONTINUE_DEFAULT);
  }
  static int DLLFUNCCALL GetTriggerScriptID(int room, int face) { return M() ? M()->GetTriggerScriptID(room, face) : -1; }
  static int DLLFUNCCALL GetCOScriptList(int **list, int **ids) {
    if (M())
      return M()->GetCOScriptList(list, ids);
    *list = nullptr;
    *ids = nullptr;
    return 0;
  }
  static int DLLFUNCCALL SaveRestoreState(void *file, uint8_t saving) {
    return M() ? M()->SaveRestoreState(file, saving) : 0;
  }
  static constexpr OsirisModuleExports kExports = {InitializeDLL,   ShutdownDLL,       GetGOScriptID,
                                                   CreateInstance,  DestroyInstance,   CallInstanceEvent,
                                                   GetTriggerScriptID, GetCOScriptList, SaveRestoreState};
};

template <size_t... I>
constexpr std::array<OsirisModuleExports, sizeof...(I)> MakeTrampolines(std::index_sequence<I...>) {
  return {{Tramp<static_cast<int>(I)>::kExports...}};
}
constexpr auto kTrampolines = MakeTrampolines(std::make_index_sequence<OSIRIS_SANDBOX_MAX_SLOTS>());

} // namespace

bool Osiris_SandboxIsWin32Module(const std::filesystem::path &path) {
  std::ifstream f(path, std::ios::binary);
  uint8_t hdr[64];
  if (!f.read(reinterpret_cast<char *>(hdr), sizeof(hdr)) || hdr[0] != 'M' || hdr[1] != 'Z')
    return false;
  uint32_t pe = hdr[0x3c] | (hdr[0x3d] << 8) | (hdr[0x3e] << 16) | (static_cast<uint32_t>(hdr[0x3f]) << 24);
  uint8_t sig[6];
  f.seekg(pe);
  if (!f.read(reinterpret_cast<char *>(sig), sizeof(sig)))
    return false;
  return memcmp(sig, "PE\0\0", 4) == 0 && sig[4] == 0x4c && sig[5] == 0x01;
}

bool Osiris_SandboxLoadModule(int slot, const std::filesystem::path &path, OsirisModuleExports *exports,
                              std::string *error) {
  if (slot < 0 || slot >= OSIRIS_SANDBOX_MAX_SLOTS) {
    *error = "bad module slot";
    return false;
  }
  Osiris_SandboxUnloadModule(slot);
  std::ifstream f(path, std::ios::binary);
  std::vector<uint8_t> image((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
  if (image.empty()) {
    *error = "cannot read " + path.u8string();
    return false;
  }
  static bool logger_set = false;
  if (!logger_set) {
    Sandbox::SetLogger([](const std::string &m) { LOG_DEBUG << m; });
    logger_set = true;
  }
  auto m = std::make_unique<Module>(slot, path.filename().u8string());
  if (!m->Load(image, error))
    return false;
  g_slots[slot] = m.release();
  *exports = kTrampolines[slot];
  LOG_INFO.printf("OSIRIS: loaded Win32 script %s in the x86 sandbox", path.filename().u8string().c_str());
  return true;
}

void Osiris_SandboxUnloadModule(int slot) {
  if (slot < 0 || slot >= OSIRIS_SANDBOX_MAX_SLOTS || !g_slots[slot])
    return;
  delete g_slots[slot];
  g_slots[slot] = nullptr;
}

bool Osiris_SandboxIsLoaded(int slot) {
  return slot >= 0 && slot < OSIRIS_SANDBOX_MAX_SLOTS && g_slots[slot] != nullptr;
}

void Osiris_SandboxSyncMemoryChunk(void *host_memory, int size) {
  for (Module *m : g_slots)
    if (m)
      m->SyncChunk(host_memory, size);
}

void Osiris_SandboxForgetMemoryChunk(void *host_memory) {
  for (Module *m : g_slots)
    if (m)
      m->ForgetChunk(host_memory);
}
