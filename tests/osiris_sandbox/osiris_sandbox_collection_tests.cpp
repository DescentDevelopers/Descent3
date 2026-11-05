/*
 * Descent 3
 * Copyright (C) 2026 Parallax Software
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
 * Runs every Win32 OSIRIS script DLL found under a directory through the
 * sandbox bridge against the fake engine, and reports which ones fail.
 *
 * The scripts are third-party files that are not part of the repository, so the
 * test runs only when D3_SCRIPT_COLLECTION names a directory. Each DLL is run
 * with the other files in its own directory available to cfopen (that is where
 * the .msg files are when the directory is an unpacked .mn3). For each one the
 * test calls InitializeDLL, and for a level script also creates the level
 * instance and every custom object script it lists, sends them EVT_CREATED and a
 * few EVT_INTERVAL events, saves and restores the script state, destroys the
 * custom object instances and calls ShutdownDLL.
 *
 * Game type modules (.d3m files, and DLLs that export DLLGameInit) need the DMFC
 * library and are counted but not run.
 *
 * Set D3_SCRIPT_SKIP to a comma-separated list of stage names (EVT_CREATED,
 * EVT_INTERVAL, save and restore) to leave those stages out, for narrowing down
 * a failure.
 *
 * Set D3_SCRIPT_VERBOSE to print the engine's debug log, which includes what
 * scripts write with OutputDebugStringA and MessageBoxA.
 *
 * Set D3_SCRIPT_REPORT to a file name to also write the results there as
 * tab-separated lines (path, result, seconds, engine calls, detail), each as
 * soon as its DLL has run.
 */

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <gtest/gtest.h>
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Init.h>

#include "cfile.h"
#include "fake_engine.h"
#include "osiris_sandbox.h"

namespace fs = std::filesystem;

namespace {

// Collects the bridge's warnings and errors for the module being run. An error
// means the bridge disabled the script; a warning is only noted.
class Capture : public plog::IAppender {
public:
  void write(const plog::Record &record) override {
    if (record.getSeverity() <= plog::error)
      errors.emplace_back(record.getMessage());
    else if (record.getSeverity() == plog::warning)
      warnings.emplace_back(record.getMessage());
  }
  void clear() {
    errors.clear();
    warnings.clear();
  }
  std::vector<std::string> errors, warnings;
};

std::string Lower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
  return s;
}

std::string Slurp(const fs::path &path) {
  std::ifstream f(path, std::ios::binary);
  return std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
}

struct Result {
  std::string path;
  std::string result; // ok, skipped or a failure stage
  std::string detail;
  double seconds = 0;
  size_t engine_calls = 0;
};

// The names after "imports " in the bridge's load-time warnings.
std::set<std::string> UnresolvedImports(const std::vector<std::string> &lines) {
  std::set<std::string> out;
  const std::string key = " imports ";
  for (const std::string &l : lines) {
    size_t a = l.find(key);
    if (a == std::string::npos)
      continue;
    a += key.size();
    size_t b = l.find(',', a);
    out.insert(l.substr(a, b == std::string::npos ? std::string::npos : b - a));
  }
  return out;
}

std::string Join(const std::vector<std::string> &lines) {
  std::string s;
  for (const std::string &l : lines) {
    if (l.find(" imports ") != std::string::npos)
      continue; // reported separately
    if (!s.empty())
      s += " | ";
    s += l;
  }
  return s;
}

Result RunOne(const fs::path &dll, Capture &capture) {
  Result r{dll.u8string(), "ok", ""};
  fake_reset();
  g_files.clear();
  for (const auto &e : fs::directory_iterator(dll.parent_path()))
    if (e.is_regular_file() && e.file_size() < (8u << 20))
      g_files[Lower(e.path().filename().u8string())] = Slurp(e.path());
  capture.clear();

  OsirisModuleExports ex{};
  std::string error;
  auto load_start = std::chrono::steady_clock::now();
  bool loaded = Osiris_SandboxLoadModule(0, dll, &ex, &error);
  double load_time = std::chrono::duration<double>(std::chrono::steady_clock::now() - load_start).count();
  if (!loaded) {
    r.result = "load";
    r.detail = error;
    return r;
  }

  tOSIRISModuleInit mi = Osiris_module_init;
  mi.string_table = nullptr;
  mi.string_count = 0;
  mi.module_identifier = 0;
  mi.module_is_static = false;
  std::string ident = dll.stem().u8string();
  mi.script_identifier = ident.data();

  auto failed = [&] { return !Osiris_SandboxIsLoaded(0) || !capture.errors.empty(); };
  std::vector<std::pair<int, void *>> instances;
  size_t instances_created = 0;
  void *level_instance = nullptr; // CreateInstance(0), the level script
  // Each stage runs only if the earlier ones succeeded; `stage` names the one that failed.
  const char *stage = nullptr;
  std::string timing;
  if (load_time >= 1.0)
    timing = "load " + std::to_string(static_cast<int>(load_time)) + "s";
  const char *skip_env = std::getenv("D3_SCRIPT_SKIP");
  std::string skip = std::string(",") + (skip_env ? skip_env : "") + ",";
  auto run = [&](const char *name, const std::function<bool()> &fn) {
    if (stage || skip.find(std::string(",") + name + ",") != std::string::npos)
      return;
    auto start = std::chrono::steady_clock::now();
    bool ok = fn();
    double t = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
    if (t >= 1.0)
      timing += std::string(timing.empty() ? "" : ", ") + name + " " + std::to_string(static_cast<int>(t)) + "s";
    if (!ok || failed())
      stage = name;
  };
  bool init_ok = true;
  run("InitializeDLL", [&] { return init_ok = ex.InitializeDLL(&mi) != 0; });
  if (ex.GetCOScriptList) {
    run("CreateInstance", [&] {
      int *list = nullptr, *ids = nullptr;
      int n = ex.GetCOScriptList(&list, &ids);
      std::vector<int> script_ids = {0};
      for (int i = 0; i < n && ids; i++)
        script_ids.push_back(ids[i]);
      for (int id : script_ids)
        if (void *p = ex.CreateInstance(id)) {
          instances.emplace_back(id, p);
          if (id == 0 && !level_instance)
            level_instance = p;
          if (std::getenv("D3_SCRIPT_VERBOSE"))
            printf("CreateInstance(%d) = %#x\n", id, static_cast<unsigned>(reinterpret_cast<uintptr_t>(p)));
        }
      instances_created = instances.size();
      // Each instance is a separate allocation; the same pointer twice means the
      // guest heap handed out overlapping memory.
      std::set<void *> distinct;
      for (auto &[id, p] : instances)
        if (!distinct.insert(p).second) {
          capture.errors.push_back("CreateInstance(" + std::to_string(id) + ") returned " +
                                   std::to_string(reinterpret_cast<uintptr_t>(p)) + " again");
          return false;
        }
      return true;
    });
    run("EVT_CREATED", [&] {
      tOSIRISEventInfo ei{};
      for (auto &[id, p] : instances)
        ex.CallInstanceEvent(id, p, EVT_CREATED, &ei);
      return true;
    });
    run("EVT_INTERVAL", [&] {
      for (int frame = 1; frame <= 3; frame++) {
        tOSIRISEventInfo ei{};
        ei.evt_interval.frame_time = 0.05f;
        ei.evt_interval.game_time = 0.05f * frame;
        for (auto &[id, p] : instances)
          ex.CallInstanceEvent(id, p, EVT_INTERVAL, &ei);
      }
      return true;
    });
    run("save and restore", [&] {
      CFILE *f = fake_new_file({});
      ex.SaveRestoreState(f, 1);
      CFILE *g = fake_new_file(fake_file_data(f));
      ex.SaveRestoreState(g, 0);
      fake_free_file(f);
      fake_free_file(g);
      return true;
    });
  }
  bool verbose = std::getenv("D3_SCRIPT_VERBOSE") != nullptr;
  run("DestroyInstance", [&] {
    for (auto &[id, p] : instances) {
      // Like the engine (Osiris_UnloadModule), leave the level instance alone:
      // some DALLAS-generated scripts delete it twice in DestroyInstance.
      if (p == level_instance)
        continue;
      if (verbose)
        printf("DestroyInstance(%d, %#x)\n", id, static_cast<unsigned>(reinterpret_cast<uintptr_t>(p)));
      ex.DestroyInstance(id, p);
    }
    instances.clear();
    return true;
  });
  run("ShutdownDLL", [&] {
    ex.ShutdownDLL();
    return true;
  });
  bool ok = !stage;
  if (!ok) {
    r.result = stage;
    r.detail = init_ok ? Join(capture.errors) : "InitializeDLL returned 0 " + Join(capture.errors);
  } else {
    // Notes for the report: imports the script did not call, and other warnings.
    std::set<std::string> missing = UnresolvedImports(capture.warnings);
    if (!missing.empty()) {
      r.detail = "unresolved, not called:";
      for (const std::string &m : missing)
        r.detail += " " + m;
    }
    std::string w = Join(capture.warnings);
    if (!w.empty())
      r.detail += (r.detail.empty() ? "" : "; ") + w;
  }
  r.engine_calls = g_calls.size();
  // After a failure the remaining instances are not destroyed and ShutdownDLL is
  // not called; unloading frees the whole sandbox.
  auto unload_start = std::chrono::steady_clock::now();
  Osiris_SandboxUnloadModule(0);
  double unload_time = std::chrono::duration<double>(std::chrono::steady_clock::now() - unload_start).count();
  if (unload_time >= 1.0)
    timing += std::string(timing.empty() ? "" : ", ") + "unload " + std::to_string(static_cast<int>(unload_time)) + "s";
  if (!timing.empty())
    r.detail += (r.detail.empty() ? "" : "; ") + std::to_string(instances_created) + " instances; " + timing;
  return r;
}

TEST(OsirisSandboxCollection, EveryScriptRuns) {
  const char *dir = std::getenv("D3_SCRIPT_COLLECTION");
  if (!dir || !*dir)
    GTEST_SKIP() << "set D3_SCRIPT_COLLECTION to a directory of unpacked missions";
  ASSERT_TRUE(fs::is_directory(dir)) << dir;

  static plog::ConsoleAppender<plog::TxtFormatter> console;
  static Capture capture;
  if (!plog::get())
    plog::init(plog::fatal, &console);
  plog::get()->setMaxSeverity(std::getenv("D3_SCRIPT_VERBOSE") ? plog::debug : plog::warning);
  plog::get()->addAppender(&capture);
  fill_module_init(&Osiris_module_init);

  std::vector<fs::path> dlls;
  int game_types = 0;
  for (const auto &e : fs::recursive_directory_iterator(dir)) {
    if (!e.is_regular_file())
      continue;
    std::string ext = Lower(e.path().extension().u8string());
    if (ext == ".d3m")
      game_types++;
    if (ext == ".dll" && Osiris_SandboxIsWin32Module(e.path()))
      dlls.push_back(e.path());
  }
  std::sort(dlls.begin(), dlls.end());

  std::ofstream report;
  if (const char *path = std::getenv("D3_SCRIPT_REPORT"))
    report.open(path);
  std::vector<Result> results;
  std::set<size_t> seen;
  for (const fs::path &dll : dlls) {
    std::string image = Slurp(dll);
    if (!seen.insert(std::hash<std::string>()(image)).second)
      continue; // the same DLL in another mission
    if (image.find("DLLGameInit") != std::string::npos) {
      results.push_back({dll.u8string(), "skipped", "game type module (needs DMFC)"});
      if (report.is_open())
        report << dll.u8string() << "\tskipped\t0\t0\tgame type module (needs DMFC)" << std::endl;
      continue;
    }
    auto start = std::chrono::steady_clock::now();
    Result r = RunOne(dll, capture);
    r.seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
    results.push_back(r);
    if (report.is_open())
      report << r.path << '\t' << r.result << '\t' << r.seconds << '\t' << r.engine_calls << '\t' << r.detail
             << std::endl;
  }
  plog::get()->setMaxSeverity(plog::fatal);

  int ok = 0, skipped = 0;
  std::map<std::string, int> by_stage;
  for (const Result &r : results) {
    if (r.result == "ok")
      ok++;
    else if (r.result == "skipped")
      skipped++;
    else {
      by_stage[r.result]++;
      ADD_FAILURE() << r.path << ": " << r.result << ": " << r.detail;
    }
  }
  printf("%zu distinct DLLs: %d ran, %d game type modules not run, %zu failed (plus %d .d3m files)\n",
         results.size(), ok, skipped, results.size() - ok - skipped, game_types);
  for (auto &[stage, n] : by_stage)
    printf("  failed in %s: %d\n", stage.c_str(), n);

}

} // namespace
