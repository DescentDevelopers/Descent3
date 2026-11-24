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
 * Runs the original level1 script (data/level1, built as a 32-bit Windows DLL)
 * through the OSIRIS sandbox bridge against the recording fake engine.
 *
 * The expected values come from the script's own source (data/level1/level1.cpp)
 * and message file (data/level1/level1.msg), not from the bridge: the names the
 * script looks up, the literals its DALLAS actions pass, and the layout of the
 * data it saves.
 */

#include <cstring>
#include <fstream>
#include <iterator>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Init.h>

#include "cfile.h"
#include "fake_engine.h"
#include "gamecinematics.h"
#include "osiris_sandbox.h"

namespace {

std::string Slurp(const char *path) {
  std::ifstream f(path, std::ios::binary);
  return std::string((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
}

// The quoted strings of `char *Name[NUM_...] = { ... };` in the script source.
std::vector<std::string> NameTable(const std::string &src, const std::string &name) {
  std::smatch m;
  std::vector<std::string> out;
  if (!std::regex_search(src, m, std::regex("char \\*" + name + "\\[[A-Z_]+\\]\\s*=\\s*\\{([^}]*)\\}")))
    return out;
  std::string body = m[1];
  std::regex quoted("\"([^\"]*)\"");
  for (auto it = std::sregex_iterator(body.begin(), body.end(), quoted); it != std::sregex_iterator(); ++it)
    out.push_back((*it)[1]);
  return out;
}

// The text of `Name=Text` in the message file.
std::string MessageText(const std::string &msg, const std::string &name) {
  std::istringstream in(msg);
  for (std::string line; std::getline(in, line);) {
    if (!line.empty() && line.back() == '\r')
      line.pop_back();
    if (line.rfind(name + "=", 0) == 0)
      return line.substr(name.size() + 1);
  }
  return "";
}

int CountPrefix(const std::string &prefix) {
  int n = 0;
  for (const std::string &c : g_calls)
    n += c.rfind(prefix, 0) == 0;
  return n;
}

bool HasCall(const std::string &call) {
  for (const std::string &c : g_calls)
    if (c == call)
      return true;
  return false;
}

std::string Quoted(const std::string &s) { return "\"" + s + "\""; }
std::string Str(long long v) { return std::to_string(v); }

class OsirisSandboxLevel1 : public ::testing::Test {
protected:
  static void SetUpTestSuite() {
    static plog::ConsoleAppender<plog::TxtFormatter> console;
    if (!plog::get())
      plog::init(plog::warning, &console);
#ifdef LEVEL1_DLL
    source_ = Slurp(LEVEL1_SOURCE);
    messages_ = Slurp(LEVEL1_MSG);
#endif
    fill_module_init(&Osiris_module_init);
  }

  void SetUp() override {
#ifndef LEVEL1_DLL
    GTEST_SKIP() << "level1.dll not built (needs clang, lld-link and llvm-dlltool)";
#else
    ASSERT_FALSE(source_.empty());
    ASSERT_FALSE(messages_.empty());
    fake_reset();
    g_files["level1.msg"] = messages_;
    std::string error;
    ASSERT_TRUE(Osiris_SandboxLoadModule(0, LEVEL1_DLL, &ex_, &error)) << error;

    tOSIRISModuleInit mi = Osiris_module_init;
    mi.string_table = nullptr;
    mi.string_count = 0;
    mi.module_identifier = 0;
    mi.module_is_static = false;
    static char ident[] = "level1";
    mi.script_identifier = ident;
    ASSERT_EQ(ex_.InitializeDLL(&mi), 1) << "InitializeDLL failed; the 32-bit game checksum may not match";
    init_calls_ = g_calls;
    instance_ = ex_.CreateInstance(0);
    ASSERT_NE(instance_, nullptr);
    g_calls.clear();
#endif
  }

  void TearDown() override {
    if (Osiris_SandboxIsLoaded(0)) {
      if (instance_)
        ex_.DestroyInstance(0, instance_);
      ex_.ShutdownDLL();
      Osiris_SandboxUnloadModule(0);
    }
  }

  int16_t Event(int event, tOSIRISEventInfo &ei) { return ex_.CallInstanceEvent(0, instance_, event, &ei); }

  void LevelStart() {
    tOSIRISEventInfo ei{};
    Event(EVT_LEVELSTART, ei);
  }

  std::vector<uint8_t> Save() {
    CFILE *f = fake_new_file({});
    tOSIRISEventInfo ei{};
    ei.evt_savestate.fileptr = f;
    Event(EVT_SAVESTATE, ei);
    std::vector<uint8_t> data = fake_file_data(f);
    fake_free_file(f);
    return data;
  }

  int Id(const char *kind, const char *table, size_t index) {
    return fake_id(kind, NameTable(source_, table).at(index).c_str());
  }

  static std::string source_, messages_;
  OsirisModuleExports ex_{};
  void *instance_ = nullptr;
  std::vector<std::string> init_calls_;
};

std::string OsirisSandboxLevel1::source_;
std::string OsirisSandboxLevel1::messages_;

TEST_F(OsirisSandboxLevel1, InitializeReadsTheMessageFileByBareName) {
  bool opened = false;
  for (const std::string &c : init_calls_)
    opened |= c == "cfopen(\"level1.msg\", \"rt\")";
  EXPECT_TRUE(opened);
}

TEST_F(OsirisSandboxLevel1, InitializeLooksUpEveryNameInSourceOrder) {
  const struct {
    const char *table, *fn;
  } lookups[] = {{"Door_names", "osipf_FindDoorName"},       {"Object_names", "osipf_FindObjectName"},
                 {"Room_names", "osipf_FindRoomName"},       {"Trigger_names", "osipf_FindTriggerName"},
                 {"Sound_names", "osipf_FindSoundName"},     {"Texture_names", "osipf_FindTextureName"},
                 {"Path_names", "osipf_FindPathName"},       {"Matcen_names", "osipf_FindMatcenName"},
                 {"Goal_names", "osipf_FindLevelGoalName"}};
  for (const auto &l : lookups) {
    std::vector<std::string> expected = NameTable(source_, l.table), got;
    std::string prefix = std::string(l.fn) + "(\"";
    for (const std::string &c : init_calls_)
      if (c.rfind(prefix, 0) == 0)
        got.push_back(c.substr(prefix.size(), c.size() - prefix.size() - 2));
    EXPECT_FALSE(expected.empty()) << l.table;
    EXPECT_EQ(got, expected) << l.table;
  }
}

TEST_F(OsirisSandboxLevel1, LevelStartPassesHandlesAndStructures) {
  LevelStart();
  // aObjHide(Object_handles[1]): the handle the engine returned for that name comes back.
  EXPECT_EQ(CountPrefix("msafe_CallFunction(" + Str(MSAFE_OBJECT_NO_RENDER) + ", " +
                        Str(Id("object", "Object_names", 1)) + ","),
            1);
  // aSetLevelTimer(5.0, 10) and aSetLevelTimer(15.0, 11)
  EXPECT_TRUE(HasCall("Osiris_CreateTimer(" + Str(OTF_LEVEL) + ", 10, " + Str(OTF_LEVEL) + ", 5, -1)"));
  EXPECT_TRUE(HasCall("Osiris_CreateTimer(" + Str(OTF_LEVEL) + ", 11, " + Str(OTF_LEVEL) + ", 15, -1)"));
  // aRoomSetFog(Room_indexes[7] and [8], 1.0, 0.7, 0.4, 6000.0)
  for (size_t room : {7, 8}) {
    int n = 0;
    std::string prefix = "msafe_CallFunction(" + Str(MSAFE_ROOM_FOG) + ", ";
    std::string fields = ", " + Str(Id("room", "Room_names", room)) + ", ";
    for (const std::string &c : g_calls)
      n += c.rfind(prefix, 0) == 0 && c.find(fields) != std::string::npos && c.find("1, 0.7, 0.4, 6000") != std::string::npos;
    EXPECT_EQ(n, 1) << "fog for Room_names[" << room << "]";
  }
  // Six aTurnOnSpew calls: mass, drag, lifetime, interval, longevity, size, speed = 0, 0, 1, 0.15, -1, 4, 20
  int spews = 0;
  for (const std::string &c : g_calls)
    spews += c.rfind("msafe_CallFunction(" + Str(MSAFE_OBJECT_START_SPEW) + ", ", 0) == 0 &&
             c.find("0, 0, 1, 0.15, -1, 4, 20") != std::string::npos;
  EXPECT_EQ(spews, 6);
}

TEST_F(OsirisSandboxLevel1, IntroCinematicCarriesTheMessageText) {
  LevelStart();
  // aCinematicIntro(Path_indexes[2], Message_strings[22], ..., Path_indexes[3], 12.0)
  std::string intro = MessageText(messages_, NameTable(source_, "Message_names").at(22));
  ASSERT_FALSE(intro.empty());
  EXPECT_EQ(CountPrefix("Cinematic_StartCannedScript(" + Str(CANNED_LEVEL_INTRO) + ", " +
                        Str(Id("path", "Path_names", 2)) + ", " + Str(Id("path", "Path_names", 3)) + ", " +
                        Quoted(intro) + ", "),
            1);
}

TEST_F(OsirisSandboxLevel1, TimerEventPassesFloatStackArguments) {
  LevelStart();
  g_calls.clear();
  // Timer 4 (Script 015): aObjPlayAnim(Object_handles[14], 7, 15, 3.0, 0) calls Obj_SetCustomAnim
  // with floats on the stack, aAddGameMessage(Message_strings[7], ...) and aSetLevelTimer(6.0, 8).
  tOSIRISEventInfo ei{};
  ei.evt_timer.id = 4;
  Event(EVT_TIMER, ei);
  EXPECT_TRUE(HasCall("osipf_ObjectCustomAnim(" + Str(Id("object", "Object_names", 14)) + ", 7, 15, 3, 0, -1, -1)"));
  std::string text = MessageText(messages_, NameTable(source_, "Message_names").at(7));
  ASSERT_FALSE(text.empty());
  EXPECT_EQ(CountPrefix("msafe_CallFunction(" + Str(MSAFE_MISC_GAME_MESSAGE) + ", "), 1);
  int with_text = 0;
  for (const std::string &c : g_calls)
    with_text += c.rfind("msafe_CallFunction(" + Str(MSAFE_MISC_GAME_MESSAGE) + ", ", 0) == 0 &&
                 c.find(Quoted(text)) != std::string::npos;
  EXPECT_EQ(with_text, 1);
  EXPECT_TRUE(HasCall("Osiris_CreateTimer(" + Str(OTF_LEVEL) + ", 8, " + Str(OTF_LEVEL) + ", 6, -1)"));
}

TEST_F(OsirisSandboxLevel1, SaveWritesTheScriptStateAndRestoresIt) {
  LevelStart();
  std::vector<uint8_t> saved = Save();

  // The save is SaveGlobalActionCtrs (one int per counter, in source order), then dfSave.
  std::vector<std::string> counters;
  size_t at = source_.find("void SaveGlobalActionCtrs(void *file_ptr)");
  ASSERT_NE(at, std::string::npos);
  std::string body = source_.substr(at, source_.find('}', at) - at);
  std::regex write_ctr("File_WriteInt\\((ScriptActionCtr_\\d+),");
  for (auto it = std::sregex_iterator(body.begin(), body.end(), write_ctr); it != std::sregex_iterator(); ++it)
    counters.push_back((*it)[1]);
  ASSERT_GT(counters.size(), 10u);

  auto int_at = [&](size_t i) {
    int v = 0;
    if (4 * i + 4 <= saved.size())
      memcpy(&v, &saved[4 * i], 4);
    return v;
  };
  auto counter = [&](const char *name) {
    for (size_t i = 0; i < counters.size(); i++)
      if (counters[i] == name)
        return int_at(i);
    return -1;
  };
  // Scripts 001, 002 and 011 ran once at level start; 000 did not.
  EXPECT_EQ(counter("ScriptActionCtr_000"), 0);
  EXPECT_EQ(counter("ScriptActionCtr_001"), 1);
  EXPECT_EQ(counter("ScriptActionCtr_002"), 1);
  EXPECT_EQ(counter("ScriptActionCtr_011"), 1);

  // dfSave: MAX_SPEW_HANDLES (50) spew handles, MAX_SOUND_HANDLES (10) sound handles, MAX_USER_VARS (25)
  // floats, MAX_SAVED_OBJECT_HANDLES (20) handles, then User_flags.
  const size_t df = counters.size();
  ASSERT_GE(saved.size(), 4 * (df + 50 + 10 + 25 + 20 + 1));
  // The spews used handle slots 0, 1, 3, 2, 4, 5 in that order; the fake engine numbers them 7000 upwards
  // through msafe_struct::id, which the bridge copies back to the script.
  EXPECT_EQ(int_at(df + 0), 7000);
  EXPECT_EQ(int_at(df + 1), 7001);
  EXPECT_EQ(int_at(df + 3), 7002);
  EXPECT_EQ(int_at(df + 2), 7003);
  EXPECT_EQ(int_at(df + 4), 7004);
  EXPECT_EQ(int_at(df + 5), 7005);
  // aUserFlagSet(0, true) and aUserFlagSet(1..3, false)
  EXPECT_EQ(int_at(df + 50 + 10 + 25 + 20), 1);

  CFILE *f = fake_new_file(saved);
  tOSIRISEventInfo ei{};
  ei.evt_restorestate.fileptr = f;
  Event(EVT_RESTORESTATE, ei);
  fake_free_file(f);
  EXPECT_EQ(Save(), saved);
}

TEST_F(OsirisSandboxLevel1, IntervalEventsRun) {
  LevelStart();
  for (int i = 0; i < 50; i++) {
    tOSIRISEventInfo ei{};
    ei.evt_interval.frame_time = 0.05f;
    ei.evt_interval.game_time = 100.0f + 0.05f * i;
    Event(EVT_INTERVAL, ei);
  }
  EXPECT_TRUE(Osiris_SandboxIsLoaded(0));
  // A disabled script makes no engine calls; a working one handles the level start again.
  g_calls.clear();
  LevelStart();
  EXPECT_FALSE(g_calls.empty());
}

TEST(OsirisSandboxTable, RefusesToLoadWhenTheTableOrderDoesNotMatch) {
#ifndef LEVEL1_DLL
  GTEST_SKIP() << "level1.dll not built";
#else
  fill_module_init(&Osiris_module_init);
  std::swap(Osiris_module_init.fp[5], Osiris_module_init.fp[6]);
  std::swap(Osiris_module_init.fp[20], Osiris_module_init.fp[61]);
  OsirisModuleExports ex{};
  std::string error;
  EXPECT_FALSE(Osiris_SandboxLoadModule(0, LEVEL1_DLL, &ex, &error));
  EXPECT_NE(error.find("function table"), std::string::npos) << error;
  EXPECT_FALSE(Osiris_SandboxIsLoaded(0));
  fill_module_init(&Osiris_module_init);
#endif
}

// The bridge probe script (support/guest/bridge_probe.cpp) passes structures with
// fields left unset. The bridge must pass a field on only where the engine reads
// it, and refuse a bad value there.
class OsirisSandboxProbe : public ::testing::Test {
protected:
  void SetUp() override {
#ifndef PROBE_DLL
    GTEST_SKIP() << "bridge_probe.dll not built (needs clang, lld-link and llvm-dlltool)";
#else
    fake_reset();
    fill_module_init(&Osiris_module_init);
    std::string error;
    ASSERT_TRUE(Osiris_SandboxLoadModule(0, PROBE_DLL, &ex_, &error)) << error;
    tOSIRISModuleInit mi = Osiris_module_init;
    static char ident[] = "bridge_probe";
    mi.script_identifier = ident;
    ASSERT_EQ(ex_.InitializeDLL(&mi), 1);
    instance_ = ex_.CreateInstance(0);
    ASSERT_NE(instance_, nullptr);
    g_calls.clear();
#endif
  }
  void TearDown() override {
    if (Osiris_SandboxIsLoaded(0)) {
      ex_.ShutdownDLL();
      Osiris_SandboxUnloadModule(0);
    }
  }
  void Probe(int which) {
    tOSIRISEventInfo ei{};
    ei.me_handle = which;
    ex_.CallInstanceEvent(0, instance_, EVT_INTERVAL, &ei);
  }
  // After a refused call the script is disabled: a valid call does nothing.
  bool Disabled() {
    g_calls.clear();
    Probe(1);
    return g_calls.empty();
  }
  OsirisModuleExports ex_{};
  void *instance_ = nullptr;
};

TEST_F(OsirisSandboxProbe, MsafeListIsIgnoredWhereTheEngineDoesNotReadIt) {
  Probe(1);
  ASSERT_EQ(g_calls.size(), 1u);
  EXPECT_EQ(g_calls[0].rfind("msafe_CallFunction(" + Str(MSAFE_OBJECT_NO_RENDER) + ", 1234, ", 0), 0u) << g_calls[0];
  EXPECT_NE(g_calls[0].find(", null, -7)"), std::string::npos) << g_calls[0];
  EXPECT_FALSE(Disabled());
}

TEST_F(OsirisSandboxProbe, MsafeListIsCheckedWhereTheEngineReadsIt) {
  Probe(2);
  EXPECT_TRUE(g_calls.empty()) << g_calls[0];
  EXPECT_TRUE(Disabled());
}

TEST_F(OsirisSandboxProbe, CinematicOrientIsIgnoredForACameraPath) {
  Probe(3);
  EXPECT_TRUE(HasCall("Cinematic_Start(" + Str(GCF_USEPATH) + ", \"probe\", null)"));
  EXPECT_FALSE(Disabled());
}

TEST_F(OsirisSandboxProbe, CinematicOrientIsCheckedForACameraPoint) {
  Probe(4);
  EXPECT_TRUE(g_calls.empty()) << g_calls[0];
  EXPECT_TRUE(Disabled());
}

TEST_F(OsirisSandboxProbe, CannedTextIsIgnoredWhereTheEngineDoesNotReadIt) {
  Probe(5);
  EXPECT_EQ(CountPrefix("Cinematic_StartCannedScript(" + Str(CANNED_MOVE_PLAYER_FADE) + ", 0, 0, null, 55, "), 1);
  EXPECT_FALSE(Disabled());
}

TEST_F(OsirisSandboxProbe, CannedTextIsCheckedWhereTheEngineReadsIt) {
  Probe(6);
  EXPECT_TRUE(g_calls.empty()) << g_calls[0];
  EXPECT_TRUE(Disabled());
}

} // namespace
