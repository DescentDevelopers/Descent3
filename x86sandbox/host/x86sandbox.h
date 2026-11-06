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
 * x86sandbox runs one 32-bit Windows DLL in an emulated i386 CPU (tiny386) that
 * is itself compiled to WebAssembly and converted to C with wasm2c. The DLL can
 * only touch its own guest memory and can only leave the emulator through:
 *  - the API stubs, which call HostCallHandler::HostCall, and
 *  - the Win32/CRT functions implemented inside the sandbox (x86sandbox/guest/win32.c),
 *    none of which reach host files, the network or other host resources.
 *
 * Guest addresses are 32-bit offsets into guest memory. Use Read/Write/Ptr to
 * reach guest memory from the host; they check every range.
 */

#ifndef X86SANDBOX_H
#define X86SANDBOX_H

#include <cstdint>
#include <functional>
#include <initializer_list>
#include <memory>
#include <string>
#include <vector>

struct w2c_env;
typedef struct x86sb_instance x86sb_instance;

namespace x86sandbox {

enum class Status : int {
  Ok = 0,
  NotInitialized = 1,
  NoMemory = 2,
  BadImage = 3,
  Exception = 4,
  Budget = 5,
  Aborted = 6,
  MemFault = 7,
  Dead = 8,
  Nesting = 9,
  MissingImport = 10,
  BadArgs = 11,
  Trap = 100, // the Wasm module itself trapped; the sandbox is unusable afterwards
};

const char *StatusName(Status s);

class Sandbox;

class HostCallHandler {
public:
  virtual ~HostCallHandler() = default;
  // The guest called API stub `index`. Arguments start at guest address esp + 4.
  // The return value goes to EAX. To return a float or double, call
  // Sandbox::ReturnDouble. To stop the guest, call Sandbox::AbortGuest.
  virtual int32_t HostCall(Sandbox &sb, uint32_t index, uint32_t esp) = 0;
};

class Sandbox {
public:
  static constexpr uint32_t kDefaultGuestMemory = 32u << 20;

  // Creates an empty sandbox. Returns nullptr on failure.
  static std::unique_ptr<Sandbox> Create(uint32_t guest_memory = kDefaultGuestMemory, std::string *error = nullptr);
  ~Sandbox();
  Sandbox(const Sandbox &) = delete;
  Sandbox &operator=(const Sandbox &) = delete;

  // Loads a PE32 i386 DLL and runs its DllMain. Can be called once.
  Status Load(const uint8_t *data, size_t len);

  // Address of an exported function, or 0.
  uint32_t FindExport(const char *name);

  // Calls a function inside the loaded image. Arguments are pushed as 32-bit words.
  Status Call(uint32_t fn, std::initializer_list<uint32_t> args, uint32_t *eax = nullptr);
  // Like Call, for a function pointer the guest gave the host (a callback).
  Status CallPointer(uint32_t fn, std::initializer_list<uint32_t> args, uint32_t *eax = nullptr);
  // Calls DLL_PROCESS_DETACH and atexit functions.
  Status Unload();

  uint32_t LastEDX();
  double LastST0();

  void SetHandler(HostCallHandler *handler) { handler_ = handler; }
  void SetInstructionBudget(uint32_t per_call);
  bool IsDead();
  const std::string &LastError() const { return last_error_; }

  // Imports the DLL binds to, as "dll!function", and whether each is implemented.
  std::vector<std::pair<std::string, bool>> Imports();

  // Guest memory
  uint32_t GuestSize() const { return guest_size_; }
  uint8_t *Ptr(uint32_t addr, uint32_t len); // nullptr if any part is out of range
  bool Read(uint32_t addr, void *dst, uint32_t len);
  bool Write(uint32_t addr, const void *src, uint32_t len);
  bool Read32(uint32_t addr, uint32_t *v) { return Read(addr, v, 4); }
  bool Write32(uint32_t addr, uint32_t v) { return Write(addr, &v, 4); }
  // Reads a NUL-terminated string of at most max_len characters. False if it is not terminated in range.
  bool ReadString(uint32_t addr, std::string &out, uint32_t max_len);
  uint32_t Alloc(uint32_t size); // guest heap; 0 on failure
  void Free(uint32_t addr);
  uint32_t ApiStub(uint32_t index);

  // For use inside HostCallHandler::HostCall.
  uint32_t Arg(uint32_t esp, int i); // 32-bit argument i; aborts the guest if unreadable
  void ReturnDouble(double v);
  void SetEDX(uint32_t v);
  void AbortGuest(Status code = Status::Aborted);

  // Message sink for the sandbox's diagnostics (default: none).
  static void SetLogger(std::function<void(const std::string &)> logger);

  // Used by the import glue.
  int32_t DispatchHostCall(uint32_t index, uint32_t esp);
  uint8_t *LinearPtr(uint32_t linear, uint64_t len);

private:
  Sandbox() = default;
  Status Finish(int trap, int32_t rc);
  Status CallImpl(uint32_t fn, std::initializer_list<uint32_t> args, uint32_t *eax, bool any);
  std::string ReadLinearString(uint32_t linear);

  x86sb_instance *inst_ = nullptr;
  std::unique_ptr<w2c_env> env_;
  HostCallHandler *handler_ = nullptr;
  uint32_t guest_base_ = 0;
  uint32_t guest_size_ = 0;
  bool trapped_ = false;
  std::string last_error_;
};

} // namespace x86sandbox

#endif
