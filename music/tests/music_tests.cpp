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

// The tests run without a low-level sound system, so every AudioStream::Open()
// fails. The sequencer is built to tolerate bad streams: it records the error
// and keeps running the theme script, so it still reports which loop it
// "played", with which loop count, from which region, and emits
// OMS_EVT_LOOPENDING each time the script reaches a `bnif`.

#include <cstdint>
#include <cstring>
#include <filesystem>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "d3_test/files.h"
#include "d3_test/game_assertions.h"
#include "music.h"
#include "support/music_seq_harness.h"

TEST(OmsTrackList, GetReturnsFilenameRegisteredForSymbol) {
  oms_tracklist tl;
  tl.init(4);
  ASSERT_TRUE(tl.add("first.osf", "First"));
  ASSERT_TRUE(tl.add("second.osf", "Second"));

  EXPECT_STREQ(tl.get("First"), "first.osf");
  EXPECT_STREQ(tl.get("Second"), "second.osf");
}

TEST(OmsTrackList, GetUnknownOrNullSymbolReturnsNull) {
  oms_tracklist tl;
  tl.init(4);
  ASSERT_TRUE(tl.add("first.osf", "First"));

  EXPECT_EQ(tl.get("Missing"), nullptr);
  EXPECT_EQ(tl.get(nullptr), nullptr);
  EXPECT_EQ(tl.get("first.osf"), nullptr);
}

TEST(OmsTrackList, AddCopiesItsArguments) {
  oms_tracklist tl;
  tl.init(4);
  char fname[32] = "first.osf";
  char sym[32] = "First";
  ASSERT_TRUE(tl.add(fname, sym));
  strcpy(fname, "clobbered.osf");
  strcpy(sym, "Clobbered");

  EXPECT_STREQ(tl.get("First"), "first.osf");
  EXPECT_EQ(tl.get("Clobbered"), nullptr);
}

TEST(OmsTrackList, AddFailsBeforeInit) {
  oms_tracklist tl;
  EXPECT_FALSE(tl.add("first.osf", "First"));
  EXPECT_EQ(tl.get("First"), nullptr);
}

TEST(OmsTrackList, AddFailsWhenFullAndKeepsExistingEntries) {
  oms_tracklist tl;
  tl.init(2);
  ASSERT_TRUE(tl.add("a.osf", "A"));
  ASSERT_TRUE(tl.add("b.osf", "B"));

  {
    // A full list Int3()s before refusing the add.
    d3_test::ExpectGameAssertions expected_assertions;
    EXPECT_FALSE(tl.add("c.osf", "C"));
  }
  EXPECT_EQ(tl.get("C"), nullptr);
  EXPECT_STREQ(tl.get("A"), "a.osf");
  EXPECT_STREQ(tl.get("B"), "b.osf");
}

TEST(OmsTrackList, ResetEmptiesListAndRestoresFullCapacity) {
  oms_tracklist tl;
  tl.init(2);
  ASSERT_TRUE(tl.add("a.osf", "A"));
  ASSERT_TRUE(tl.add("b.osf", "B"));

  tl.reset();
  EXPECT_EQ(tl.get("A"), nullptr);
  EXPECT_EQ(tl.get("B"), nullptr);

  EXPECT_TRUE(tl.add("c.osf", "C"));
  EXPECT_TRUE(tl.add("d.osf", "D"));
  EXPECT_STREQ(tl.get("C"), "c.osf");
  EXPECT_STREQ(tl.get("D"), "d.osf");
}

TEST(OmsTrackList, CanBeReinitializedAfterFree) {
  oms_tracklist tl;
  tl.init(1);
  ASSERT_TRUE(tl.add("a.osf", "A"));
  tl.free();
  EXPECT_FALSE(tl.add("b.osf", "B"));

  tl.init(1);
  EXPECT_EQ(tl.get("A"), nullptr);
  EXPECT_TRUE(tl.add("b.osf", "B"));
  EXPECT_STREQ(tl.get("B"), "b.osf");
}

using musictest::Append;
using musictest::CountEvents;
using musictest::LoadAndStart;
using musictest::LoopSample;
using musictest::RunUntilLoops;
using musictest::SimpleSection;
using musictest::WriteTheme;

namespace {

class MusicSeqTest : public ::testing::Test {
protected:
  void TearDown() override {
    m_seq.Shutdown();
  }

  d3_test::TempDir m_themes;
  OutrageMusicSeq m_seq;
};

} // namespace

TEST_F(MusicSeqTest, InitFailsForEmptyFilename) { EXPECT_FALSE(m_seq.Init("")); }

TEST_F(MusicSeqTest, InitFailsForMissingFile) {
  EXPECT_FALSE(m_seq.Init((m_themes.path() / "does_not_exist.omf").string().c_str()));
}

TEST_F(MusicSeqTest, InitFailsWithoutThemeFileTag) {
  std::filesystem::path path = m_themes.Write("untagged.omf", "stream=a.osf$A\n");
  EXPECT_FALSE(m_seq.Init(path.string().c_str()));
}

TEST_F(MusicSeqTest, NothingIsPlayingBeforeASongStarts) {
  std::vector<std::string> theme = {"stream=a.osf$A", "region Only"};
  Append(theme, SimpleSection("idle", "A", 1));
  theme.push_back("endregion");
  LoadAndStart(m_seq, m_themes, theme);

  int count = -1;
  EXPECT_EQ(m_seq.GetCurrentLoopName(&count), nullptr);
  EXPECT_EQ(count, 0);
  EXPECT_EQ(m_seq.GetPlayingRegion(), -1);
}

TEST_F(MusicSeqTest, PlayResolvesStreamSymbolAndLoopCount) {
  std::vector<std::string> theme = {
      "@ comment lines are ignored",
      "stream=first.osf$First",
      "stream=second.osf$Second",
      "region Only",
  };
  Append(theme, SimpleSection("combat", "Second", 3));
  theme.push_back("endregion");
  LoadAndStart(m_seq, m_themes, theme);

  m_seq.SetCurrentRegion(0);
  m_seq.StartSong(OMS_THEME_TYPE_COMBAT, false);
  auto loops = RunUntilLoops(m_seq, 1);

  ASSERT_EQ(loops.size(), 1u);
  EXPECT_EQ(loops[0], (LoopSample{"second.osf", 3, OMS_THEME_TYPE_COMBAT, 0}));
}

struct SectionCase {
  const char *section;
  int theme;
};

std::ostream &operator<<(std::ostream &os, const SectionCase &c) {
  return os << "section " << c.section << " -> theme " << c.theme;
}

const SectionCase kSectionCases[] = {{"intro", OMS_THEME_TYPE_INTRO},
                                     {"idle", OMS_THEME_TYPE_IDLE},
                                     {"combat", OMS_THEME_TYPE_COMBAT},
                                     {"death", OMS_THEME_TYPE_DEATH},
                                     {"idle_combat", OMS_THEME_TYPE_IDLE_TO_COMBAT},
                                     {"combat_idle", OMS_THEME_TYPE_COMBAT_TO_IDLE}};

class MusicSeqSectionTest : public MusicSeqTest, public ::testing::WithParamInterface<SectionCase> {};

TEST_P(MusicSeqSectionTest, StartSongPicksSectionByThemeType) {
  std::vector<std::string> theme;
  for (const auto &c : kSectionCases)
    theme.push_back(std::string("stream=") + c.section + ".osf$Sym_" + c.section);
  theme.push_back("region Only");
  for (const auto &c : kSectionCases)
    Append(theme, SimpleSection(c.section, std::string("Sym_") + c.section, 1));
  theme.push_back("endregion");
  LoadAndStart(m_seq, m_themes, theme);

  const SectionCase &want = GetParam();
  m_seq.SetCurrentRegion(0);
  m_seq.StartSong(want.theme, false);
  auto loops = RunUntilLoops(m_seq, 1);

  ASSERT_EQ(loops.size(), 1u);
  EXPECT_EQ(loops[0], (LoopSample{std::string(want.section) + ".osf", 1, want.theme, 0}));
}

INSTANTIATE_TEST_SUITE_P(AllThemes, MusicSeqSectionTest, ::testing::ValuesIn(kSectionCases),
                         [](const ::testing::TestParamInfo<SectionCase> &info) { return info.param.section; });

TEST_F(MusicSeqTest, RegionsAreNumberedInFileOrder) {
  std::vector<std::string> theme = {"stream=r0.osf$R0", "stream=r1.osf$R1", "stream=r2.osf$R2"};
  for (int r = 0; r < 3; r++) {
    theme.push_back("region Region" + std::to_string(r));
    Append(theme, SimpleSection("idle", "R" + std::to_string(r), 1));
    theme.push_back("endregion");
  }
  LoadAndStart(m_seq, m_themes, theme);

  for (int16_t r : {2, 0, 1}) {
    m_seq.SetCurrentRegion(r);
    m_seq.StartSong(OMS_THEME_TYPE_IDLE, false);
    auto loops = RunUntilLoops(m_seq, 1);
    ASSERT_EQ(loops.size(), 1u) << "region " << r;
    EXPECT_EQ(loops[0], (LoopSample{"r" + std::to_string(r) + ".osf", 1, OMS_THEME_TYPE_IDLE, r}));
  }
}

TEST_F(MusicSeqTest, StartSongWithoutMatchingSongPlaysNothing) {
  std::vector<std::string> theme = {"stream=a.osf$A", "region Only"};
  Append(theme, SimpleSection("idle", "A", 1));
  theme.push_back("endregion");
  LoadAndStart(m_seq, m_themes, theme);

  m_seq.StartSong(OMS_THEME_TYPE_IDLE, false);
  EXPECT_EQ(CountEvents(m_seq, 20), 0);

  m_seq.SetCurrentRegion(0);
  m_seq.StartSong(OMS_THEME_TYPE_COMBAT, false);
  EXPECT_EQ(CountEvents(m_seq, 20), 0);

  m_seq.SetCurrentRegion(5);
  m_seq.StartSong(OMS_THEME_TYPE_IDLE, false);
  EXPECT_EQ(CountEvents(m_seq, 20), 0);

  int count = -1;
  EXPECT_EQ(m_seq.GetCurrentLoopName(&count), nullptr);
  EXPECT_EQ(m_seq.GetPlayingRegion(), -1);
}

TEST_F(MusicSeqTest, FrameDoesNothingUntilSequencerIsStarted) {
  std::vector<std::string> theme = {"stream=a.osf$A", "region Only"};
  Append(theme, SimpleSection("idle", "A", 2));
  theme.push_back("endregion");
  ASSERT_TRUE(m_seq.Init(WriteTheme(m_themes, theme).c_str()));
  EXPECT_FALSE(m_seq.IsSequencerRunning());

  m_seq.SetCurrentRegion(0);
  m_seq.StartSong(OMS_THEME_TYPE_IDLE, false);
  EXPECT_EQ(CountEvents(m_seq, 20), 0);
  int count = -1;
  EXPECT_EQ(m_seq.GetCurrentLoopName(&count), nullptr);

  m_seq.Start();
  auto loops = RunUntilLoops(m_seq, 1);
  ASSERT_EQ(loops.size(), 1u);
  EXPECT_EQ(loops[0], (LoopSample{"a.osf", 2, OMS_THEME_TYPE_IDLE, 0}));
}

// Every region of a real theme file reuses the same label names.
TEST_F(MusicSeqTest, LabelsResolveWithinTheirOwnRegion) {
  LoadAndStart(m_seq, m_themes, {
      "stream=right.osf$Right",
      "stream=wrong.osf$Wrong",
      "stream=r0.osf$R0",
      "region First",
      "section idle",
      "llpt R0",       // 0
      "lplc 1",        // 1
      "goto pick",     // 2
      "label pick",    //   -> 3
      "play",          // 3, 4
      "label w0",      //
      "bnif w0",       // 5
      "endsection",    // 6
      "endregion",
      "region Second",
      "section idle",
      "goto pick",     // 0
      "lplc 1",        // 1
      "lplc 1",        // 2
      "llpt Wrong",    // 3: where region First's "pick" would land
      "play",          // 4, 5
      "label w1a",     //
      "bnif w1a",      // 6
      "label pick",    //   -> 7
      "llpt Right",    // 7
      "lplc 1",        // 8
      "play",          // 9, 10
      "label w1b",     //
      "bnif w1b",      // 11
      "endsection",    // 12
      "endregion",
  });

  m_seq.SetCurrentRegion(1);
  m_seq.StartSong(OMS_THEME_TYPE_IDLE, false);
  auto loops = RunUntilLoops(m_seq, 1);

  ASSERT_EQ(loops.size(), 1u);
  EXPECT_EQ(loops[0].name, "right.osf");
  EXPECT_EQ(loops[0].region, 1);
}

// `lcmp N` + `compare rX` set up a comparison of game register X against N;
// blt/bgt/beq branch on register < / > / == N.
struct CompareCase {
  const char *name;
  int reg_value;
  const char *expected_loop;
};

std::ostream &operator<<(std::ostream &os, const CompareCase &c) {
  return os << "r2=" << c.reg_value << " -> " << c.expected_loop;
}

class MusicSeqCompareTest : public MusicSeqTest, public ::testing::WithParamInterface<CompareCase> {};

TEST_P(MusicSeqCompareTest, BranchesOnRegisterComparedToConstant) {
  LoadAndStart(m_seq, m_themes, {
      "stream=less.osf$Less",
      "stream=equal.osf$Equal",
      "stream=greater.osf$Greater",
      "region Only",
      "section idle",
      "lcmp 5",
      "compare r2",
      "blt less",
      "bgt greater",
      "llpt Equal",
      "lplc 1",
      "goto go",
      "label less",
      "llpt Less",
      "lplc 1",
      "goto go",
      "label greater",
      "llpt Greater",
      "lplc 1",
      "label go",
      "play",
      "label w",
      "bnif w",
      "endsection",
      "endregion",
  });

  // Other registers hold values that would pick a different branch, so the
  // test also checks that `compare r2` reads register 2.
  m_seq.SetRegister(0, 100);
  m_seq.SetRegister(1, -100);
  m_seq.SetRegister(3, 100);
  m_seq.SetRegister(2, (tMusicVal)GetParam().reg_value);

  m_seq.SetCurrentRegion(0);
  m_seq.StartSong(OMS_THEME_TYPE_IDLE, false);
  auto loops = RunUntilLoops(m_seq, 1);

  ASSERT_EQ(loops.size(), 1u);
  EXPECT_EQ(loops[0].name, GetParam().expected_loop);
}

INSTANTIATE_TEST_SUITE_P(Values, MusicSeqCompareTest,
                         ::testing::Values(CompareCase{"Negative", -3, "less.osf"},
                                           CompareCase{"JustBelow", 4, "less.osf"},
                                           CompareCase{"Equal", 5, "equal.osf"},
                                           CompareCase{"JustAbove", 6, "greater.osf"},
                                           CompareCase{"FarAbove", 300, "greater.osf"}),
                         [](const ::testing::TestParamInfo<CompareCase> &info) { return info.param.name; });

// The playlist idiom used by the shipped theme files: an `ifi` counter selects
// which loop to play on each pass, `inci` advances it and `ifi N; seti 0`
// wraps it around. Several commands may share a line, separated by ';'.
TEST_F(MusicSeqTest, IfiCounterCyclesThroughPlaylist) {
  LoadAndStart(m_seq, m_themes, {
      "stream=a.osf$A",
      "stream=b.osf$B",
      "stream=c.osf$C",
      "region Only",
      "section idle",
      "seti 0",
      "label start",
      "ifi 0;llpt A;lplc 1",
      "ifi 1;llpt B;lplc 10",
      "ifi 2;llpt C;lplc 2",
      "play",
      "label handler",
      "bnif handler",
      "inci",
      "ifi 3; seti 0",
      "goto start",
      "endsection",
      "endregion",
  });

  m_seq.SetCurrentRegion(0);
  m_seq.StartSong(OMS_THEME_TYPE_IDLE, false);
  auto loops = RunUntilLoops(m_seq, 7);

  std::vector<std::pair<std::string, int>> got;
  for (const auto &s : loops)
    got.emplace_back(s.name, s.count);
  const std::vector<std::pair<std::string, int>> want = {{"a.osf", 1}, {"b.osf", 10}, {"c.osf", 2}, {"a.osf", 1},
                                                         {"b.osf", 10}, {"c.osf", 2}, {"a.osf", 1}};
  EXPECT_EQ(got, want);
}

TEST_F(MusicSeqTest, ImmediateSwitchReplacesActiveSong) {
  std::vector<std::string> theme = {"stream=r0.osf$R0", "stream=r1.osf$R1"};
  for (int r = 0; r < 2; r++) {
    theme.push_back("region Region" + std::to_string(r));
    theme.push_back("section idle");
    theme.push_back("label top");
    theme.push_back("llpt R" + std::to_string(r));
    theme.push_back("lplc 1");
    theme.push_back("play");
    theme.push_back("label wait");
    theme.push_back("bnif wait");
    theme.push_back("goto top");
    theme.push_back("endsection");
    theme.push_back("endregion");
  }
  LoadAndStart(m_seq, m_themes, theme);

  m_seq.SetCurrentRegion(0);
  m_seq.StartSong(OMS_THEME_TYPE_IDLE, false);
  auto before = RunUntilLoops(m_seq, 2);
  ASSERT_EQ(before.size(), 2u);
  EXPECT_EQ(before[1], (LoopSample{"r0.osf", 1, OMS_THEME_TYPE_IDLE, 0}));

  m_seq.SetCurrentRegion(1);
  m_seq.StartSong(OMS_THEME_TYPE_IDLE, false);
  auto after = RunUntilLoops(m_seq, 2);
  ASSERT_EQ(after.size(), 2u);
  EXPECT_EQ(after[0], (LoopSample{"r1.osf", 1, OMS_THEME_TYPE_IDLE, 1}));
  EXPECT_EQ(after[1], (LoopSample{"r1.osf", 1, OMS_THEME_TYPE_IDLE, 1}));
}

TEST_F(MusicSeqTest, StopSongSilencesActiveSong) {
  LoadAndStart(m_seq, m_themes, {
      "stream=a.osf$A",
      "region Only",
      "section idle",
      "label top",
      "llpt A",
      "lplc 1",
      "play",
      "label wait",
      "bnif wait",
      "goto top",
      "endsection",
      "endregion",
  });

  m_seq.SetCurrentRegion(0);
  m_seq.StartSong(OMS_THEME_TYPE_IDLE, false);
  ASSERT_EQ(RunUntilLoops(m_seq, 1).size(), 1u);

  m_seq.StopSong();
  int count = -1;
  EXPECT_EQ(m_seq.GetCurrentLoopName(&count), nullptr);
  EXPECT_EQ(m_seq.GetPlayingRegion(), -1);
  EXPECT_EQ(CountEvents(m_seq, 20), 0);
}
