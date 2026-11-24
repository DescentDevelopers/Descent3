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

#include "x86sandbox.h"

#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "x86sandbox_rt.h"
#include "x86sandbox_wasm.h"

struct w2c_env {
  x86sandbox::Sandbox *sb;
};

namespace x86sandbox {

static std::function<void(const std::string &)> g_logger;

void Sandbox::SetLogger(std::function<void(const std::string &)> logger) { g_logger = std::move(logger); }

static void Log(const std::string &msg) {
  if (g_logger)
    g_logger(msg);
}

const char *StatusName(Status s) {
  switch (s) {
  case Status::Ok:
    return "ok";
  case Status::NotInitialized:
    return "not initialized";
  case Status::NoMemory:
    return "out of memory";
  case Status::BadImage:
    return "bad image";
  case Status::Exception:
    return "CPU exception";
  case Status::Budget:
    return "instruction budget exhausted";
  case Status::Aborted:
    return "aborted";
  case Status::MemFault:
    return "memory fault";
  case Status::Dead:
    return "module failed earlier";
  case Status::Nesting:
    return "calls nested too deeply";
  case Status::MissingImport:
    return "unsupported import called";
  case Status::BadArgs:
    return "bad arguments";
  case Status::Trap:
    return "sandbox trapped";
  }
  return "unknown";
}

std::unique_ptr<Sandbox> Sandbox::Create(uint32_t guest_memory, std::string *error) {
  std::unique_ptr<Sandbox> sb(new Sandbox());
  sb->env_ = std::make_unique<w2c_env>();
  sb->env_->sb = sb.get();
  sb->inst_ = x86sb_new(sb->env_.get());
  if (!sb->inst_) {
    if (error)
      *error = "could not instantiate the sandbox module";
    return nullptr;
  }
  int32_t rc = 0;
  int trap = x86sb_init(sb->inst_, guest_memory, &rc);
  if (trap || rc != 0) {
    if (error)
      *error = std::string("sandbox initialization failed: ") +
               (trap ? "trap" : StatusName(static_cast<Status>(rc)));
    return nullptr;
  }
  sb->guest_base_ = x86sb_guest_base(sb->inst_);
  sb->guest_size_ = x86sb_guest_size(sb->inst_);
  uint64_t mem_size = 0;
  x86sb_memory(sb->inst_, &mem_size);
  if (static_cast<uint64_t>(sb->guest_base_) + sb->guest_size_ > mem_size) {
    if (error)
      *error = "guest memory outside the sandbox memory";
    return nullptr;
  }
  return sb;
}

Sandbox::~Sandbox() { x86sb_delete(inst_); }

uint8_t *Sandbox::LinearPtr(uint32_t linear, uint64_t len) {
  uint64_t size = 0;
  uint8_t *data = x86sb_memory(inst_, &size);
  if (!data || static_cast<uint64_t>(linear) > size || len > size - linear)
    return nullptr;
  return data + linear;
}

std::string Sandbox::ReadLinearString(uint32_t linear) {
  std::string out;
  for (uint32_t i = 0; i < 1024; i++) {
    uint8_t *p = LinearPtr(linear + i, 1);
    if (!p || !*p)
      break;
    out.push_back(static_cast<char>(*p));
  }
  return out;
}

Status Sandbox::Finish(int trap, int32_t rc) {
  if (trap) {
    trapped_ = true;
    last_error_ = "the sandbox trapped (wasm2c trap " + std::to_string(trap) + ")";
    Log("x86sandbox: " + last_error_);
    return Status::Trap;
  }
  if (rc != 0) {
    // The status is returned separately; the message says what happened.
    last_error_ = ReadLinearString(x86sb_error(inst_));
    if (last_error_.empty())
      last_error_ = StatusName(static_cast<Status>(rc));
    return static_cast<Status>(rc);
  }
  return Status::Ok;
}

Status Sandbox::Load(const uint8_t *data, size_t len) {
  if (trapped_)
    return Status::Trap;
  if (len == 0 || len > (16u << 20))
    return Status::BadArgs;
  uint32_t staging = x86sb_staging(inst_, static_cast<uint32_t>(len));
  uint8_t *dst = staging ? LinearPtr(staging, len) : nullptr;
  if (!dst)
    return Status::NoMemory;
  memcpy(dst, data, len);
  int32_t rc = 0;
  int trap = x86sb_load(inst_, static_cast<uint32_t>(len), &rc);
  return Finish(trap, rc);
}

uint32_t Sandbox::FindExport(const char *name) {
  if (trapped_)
    return 0;
  size_t n = strlen(name);
  uint8_t *buf = LinearPtr(x86sb_namebuf(inst_), 256);
  if (!buf || n > 255)
    return 0;
  memcpy(buf, name, n + 1);
  uint32_t addr = 0;
  if (x86sb_find_export(inst_, &addr) != 0) {
    trapped_ = true;
    return 0;
  }
  return addr;
}

Status Sandbox::CallImpl(uint32_t fn, std::initializer_list<uint32_t> args, uint32_t *eax, bool any) {
  if (trapped_)
    return Status::Trap;
  if (args.size() > 32)
    return Status::BadArgs;
  uint8_t *buf = LinearPtr(x86sb_argbuf(inst_), 32 * 4);
  if (!buf)
    return Status::NoMemory;
  /* Writing the argument buffer of a nested call does not disturb an outer call:
     the outer call's arguments were already pushed onto the guest stack. */
  uint32_t i = 0;
  for (uint32_t a : args)
    memcpy(buf + 4 * i++, &a, 4);
  int32_t rc = 0;
  int trap = x86sb_call(inst_, fn, static_cast<int32_t>(args.size()), any ? 1 : 0, &rc);
  Status s = Finish(trap, rc);
  if (s == Status::Ok && eax)
    *eax = x86sb_eax(inst_);
  return s;
}

Status Sandbox::Call(uint32_t fn, std::initializer_list<uint32_t> args, uint32_t *eax) {
  return CallImpl(fn, args, eax, false);
}

Status Sandbox::CallPointer(uint32_t fn, std::initializer_list<uint32_t> args, uint32_t *eax) {
  return CallImpl(fn, args, eax, true);
}

Status Sandbox::Unload() {
  if (trapped_)
    return Status::Trap;
  int32_t rc = 0;
  int trap = x86sb_unload(inst_, &rc);
  return Finish(trap, rc);
}

uint32_t Sandbox::LastEDX() { return trapped_ ? 0 : x86sb_edx(inst_); }
double Sandbox::LastST0() { return trapped_ ? 0.0 : x86sb_st0(inst_); }
void Sandbox::SetInstructionBudget(uint32_t per_call) { x86sb_set_budget(inst_, per_call); }
bool Sandbox::IsDead() { return trapped_ || x86sb_is_dead(inst_); }

std::vector<std::pair<std::string, bool>> Sandbox::Imports() {
  std::vector<std::pair<std::string, bool>> out;
  if (trapped_)
    return out;
  uint32_t n = x86sb_import_count(inst_);
  for (uint32_t i = 0; i < n; i++)
    out.emplace_back(ReadLinearString(x86sb_import_name(inst_, i)), x86sb_import_supported(inst_, i) != 0);
  return out;
}

uint8_t *Sandbox::Ptr(uint32_t addr, uint32_t len) {
  if (static_cast<uint64_t>(addr) + len > guest_size_)
    return nullptr;
  return LinearPtr(guest_base_ + addr, len);
}

bool Sandbox::Read(uint32_t addr, void *dst, uint32_t len) {
  uint8_t *p = Ptr(addr, len);
  if (!p)
    return false;
  memcpy(dst, p, len);
  return true;
}

bool Sandbox::Write(uint32_t addr, const void *src, uint32_t len) {
  uint8_t *p = Ptr(addr, len);
  if (!p)
    return false;
  memcpy(p, src, len);
  return true;
}

bool Sandbox::ReadString(uint32_t addr, std::string &out, uint32_t max_len) {
  out.clear();
  for (uint32_t i = 0; i <= max_len; i++) {
    uint8_t *p = Ptr(addr + i, 1);
    if (!p || addr + i < addr)
      return false;
    if (*p == 0)
      return true;
    if (i == max_len)
      return false;
    out.push_back(static_cast<char>(*p));
  }
  return false;
}

uint32_t Sandbox::Alloc(uint32_t size) {
  uint32_t addr = 0;
  if (trapped_ || x86sb_alloc(inst_, size, &addr) != 0)
    return 0;
  return addr;
}

void Sandbox::Free(uint32_t addr) {
  if (!trapped_ && x86sb_free(inst_, addr) != 0)
    trapped_ = true;
}

uint32_t Sandbox::ApiStub(uint32_t index) { return x86sb_api_stub(inst_, index); }

uint32_t Sandbox::Arg(uint32_t esp, int i) {
  uint32_t v = 0;
  if (!Read32(esp + 4 + 4u * static_cast<uint32_t>(i), &v))
    AbortGuest(Status::BadArgs);
  return v;
}

void Sandbox::ReturnDouble(double v) { x86sb_push_f64(inst_, v); }
void Sandbox::SetEDX(uint32_t v) { x86sb_set_edx(inst_, v); }
void Sandbox::AbortGuest(Status code) { x86sb_host_abort(inst_, static_cast<int32_t>(code)); }

int32_t Sandbox::DispatchHostCall(uint32_t index, uint32_t esp) {
  if (!handler_) {
    AbortGuest(Status::Aborted);
    return 0;
  }
  return handler_->HostCall(*this, index, esp);
}

} // namespace x86sandbox

/* ---- Functions the Wasm module imports from the host ---- */

using x86sandbox::Sandbox;

extern "C" {

u32 w2c_env_host_call(struct w2c_env *env, u32 index, u32 esp) {
  return static_cast<u32>(env->sb->DispatchHostCall(index, esp));
}

void w2c_env_host_log(struct w2c_env *env, u32 msg, u32 len) {
  if (len > 4096)
    len = 4096;
  uint8_t *p = env->sb->LinearPtr(msg, len);
  if (p)
    x86sandbox::Log("x86sandbox: " + std::string(reinterpret_cast<const char *>(p), len));
}

u32 w2c_env_host_time_ms(struct w2c_env *env) {
  (void)env;
  static const auto start = std::chrono::steady_clock::now();
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
  return static_cast<u32>(ms.count());
}

u32 w2c_env_fmt_double(struct w2c_env *env, f64 v, u32 conv, u32 prec, u32 flags, u32 width, u32 out, u32 cap) {
  if (cap == 0)
    return 0;
  if (!strchr("fFeEgG", static_cast<int>(conv)) || conv == 0)
    conv = 'f';
  if (prec > 100)
    prec = 100;
  if (width > 200)
    width = 200;
  char fmt[32];
  int n = 0;
  fmt[n++] = '%';
  if (flags & 1)
    fmt[n++] = '-';
  if (flags & 2)
    fmt[n++] = '+';
  if (flags & 4)
    fmt[n++] = ' ';
  if (flags & 8)
    fmt[n++] = '#';
  if (flags & 16)
    fmt[n++] = '0';
  n += snprintf(fmt + n, sizeof(fmt) - n, "%u.%u%c", width, prec, static_cast<char>(conv));
  char buf[512];
  int len = snprintf(buf, sizeof(buf), fmt, v);
  if (len < 0)
    return 0;
  if (len > static_cast<int>(sizeof(buf)) - 1)
    len = sizeof(buf) - 1;
  if (static_cast<u32>(len) > cap - 1)
    len = static_cast<int>(cap - 1);
  uint8_t *dst = env->sb->LinearPtr(out, static_cast<uint64_t>(len) + 1);
  if (!dst)
    return 0;
  memcpy(dst, buf, len);
  dst[len] = 0;
  return static_cast<u32>(len);
}

f64 w2c_env_parse_double(struct w2c_env *env, u32 s, u32 len, u32 consumed) {
  if (len > 512)
    len = 512;
  uint8_t *src = env->sb->LinearPtr(s, len);
  uint8_t *out = env->sb->LinearPtr(consumed, 4);
  if (!src || !out)
    return 0.0;
  std::string str(reinterpret_cast<const char *>(src), len);
  char *end = nullptr;
  double d = strtod(str.c_str(), &end);
  int32_t used = static_cast<int32_t>(end - str.c_str());
  memcpy(out, &used, 4);
  return d;
}

f64 w2c_env_m_sin(struct w2c_env *, f64 x) { return std::sin(x); }
f64 w2c_env_m_cos(struct w2c_env *, f64 x) { return std::cos(x); }
f64 w2c_env_m_tan(struct w2c_env *, f64 x) { return std::tan(x); }
f64 w2c_env_m_asin(struct w2c_env *, f64 x) { return std::asin(x); }
f64 w2c_env_m_acos(struct w2c_env *, f64 x) { return std::acos(x); }
f64 w2c_env_m_atan(struct w2c_env *, f64 x) { return std::atan(x); }
f64 w2c_env_m_atan2(struct w2c_env *, f64 y, f64 x) { return std::atan2(y, x); }
f64 w2c_env_m_pow(struct w2c_env *, f64 x, f64 y) { return std::pow(x, y); }
f64 w2c_env_m_exp(struct w2c_env *, f64 x) { return std::exp(x); }
f64 w2c_env_m_log(struct w2c_env *, f64 x) { return std::log(x); }
f64 w2c_env_m_log10(struct w2c_env *, f64 x) { return std::log10(x); }
f64 w2c_env_m_log2(struct w2c_env *, f64 x) { return std::log2(x); }
f64 w2c_env_m_fmod(struct w2c_env *, f64 x, f64 y) { return std::fmod(x, y); }

} // extern "C"
