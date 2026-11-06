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
 * Runs 32-bit Windows OSIRIS script DLLs (the level scripts shipped inside
 * third-party .mn3 missions) in the x86 sandbox, on every platform.
 *
 * A sandboxed module is used through the same nine entry points as a native
 * module. Osiris_SandboxLoadModule fills an OsirisModuleExports with
 * trampolines that forward to the sandboxed DLL, so the rest of the OSIRIS
 * loader does not need to know which kind of module it is calling.
 */

#ifndef OSIRIS_SANDBOX_H
#define OSIRIS_SANDBOX_H

#include <cstdint>
#include <filesystem>
#include <string>

#include "module.h"
#include "osiris_share.h"

// The exported DLL function call prototypes
#if defined(POSIX)
typedef char DLLFUNCCALL (*InitializeDLL_fp)(tOSIRISModuleInit *function_list);
typedef void DLLFUNCCALL (*ShutdownDLL_fp)(void);
typedef int DLLFUNCCALL (*GetGOScriptID_fp)(const char *name, uint8_t isdoor);
typedef void DLLFUNCCALL *(*CreateInstance_fp)(int id);
typedef void DLLFUNCCALL (*DestroyInstance_fp)(int id, void *ptr);
typedef int16_t DLLFUNCCALL (*CallInstanceEvent_fp)(int id, void *ptr, int event, tOSIRISEventInfo *data);
typedef int DLLFUNCCALL (*GetTriggerScriptID_fp)(int trigger_room, int trigger_face);
typedef int DLLFUNCCALL (*GetCOScriptList_fp)(int **list, int **id_list);
typedef int DLLFUNCCALL (*SaveRestoreState_fp)(void *file_ptr, uint8_t saving_state);
#else
typedef char(DLLFUNCCALL *InitializeDLL_fp)(tOSIRISModuleInit *function_list);
typedef void(DLLFUNCCALL *ShutdownDLL_fp)(void);
typedef int(DLLFUNCCALL *GetGOScriptID_fp)(const char *name, uint8_t isdoor);
typedef void *(DLLFUNCCALL *CreateInstance_fp)(int id);
typedef void(DLLFUNCCALL *DestroyInstance_fp)(int id, void *ptr);
typedef int16_t(DLLFUNCCALL *CallInstanceEvent_fp)(int id, void *ptr, int event, tOSIRISEventInfo *data);
typedef int(DLLFUNCCALL *GetTriggerScriptID_fp)(int trigger_room, int trigger_face);
typedef int(DLLFUNCCALL *GetCOScriptList_fp)(int **list, int **id_list);
typedef int(DLLFUNCCALL *SaveRestoreState_fp)(void *file_ptr, uint8_t saving_state);
#endif

struct OsirisModuleExports {
  InitializeDLL_fp InitializeDLL;
  ShutdownDLL_fp ShutdownDLL;
  GetGOScriptID_fp GetGOScriptID;
  CreateInstance_fp CreateInstance;
  DestroyInstance_fp DestroyInstance;
  CallInstanceEvent_fp CallInstanceEvent;
  GetTriggerScriptID_fp GetTriggerScriptID;
  GetCOScriptList_fp GetCOScriptList;
  SaveRestoreState_fp SaveRestoreState;
};

// Number of module slots; must match MAX_LOADED_MODULES in OsirisLoadandBind.cpp.
constexpr int OSIRIS_SANDBOX_MAX_SLOTS = 64;

// The game checksum a 32-bit script was built against (see CHECKSUM in CMakeLists.txt).
constexpr uint32_t OSIRIS_SANDBOX_GUEST_CHECKSUM = 2273873307u;

// True if the file starts with a PE32 i386 header.
bool Osiris_SandboxIsWin32Module(const std::filesystem::path &path);

// Loads the Win32 DLL at `path` into a new sandbox bound to module slot `slot`.
// On success fills `exports` and returns true. GetTriggerScriptID and
// GetCOScriptList are null if the DLL does not export them; only level scripts
// need them, as with native modules.
bool Osiris_SandboxLoadModule(int slot, const std::filesystem::path &path, OsirisModuleExports *exports,
                              std::string *error);

// Destroys the sandbox in `slot`. Call after ShutdownDLL.
void Osiris_SandboxUnloadModule(int slot);

bool Osiris_SandboxIsLoaded(int slot);

// Memory chunks a sandboxed script allocates with Scrpt_MemAlloc live in guest
// memory and are mirrored into the engine's chunk (which is what save games
// contain). Called by the engine before it saves chunks, and when it frees one.
void Osiris_SandboxSyncMemoryChunk(void *host_memory, int size);
void Osiris_SandboxForgetMemoryChunk(void *host_memory);

#endif
