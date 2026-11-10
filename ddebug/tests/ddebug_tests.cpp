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

#include <ostream>
#include <string>
#include <vector>

#include <SDL3/SDL_assert.h>
#include <gtest/gtest.h>

#include "debug.h"
#include "pserror.h"

#include "support/test_predicates.h"
#include "support/fake_message_boxes.h"

namespace {

class DdebugTest : public ::testing::Test {
protected:
  void SetUp() override {
    fake_dialogs::Reset();
    Debug_break = false;
    previous_boxes_ = Debug_SetMessageBoxHandlers({fake_dialogs::ErrorBox, fake_dialogs::MessageBox});
    SetDebugBreakHandlers(RecordStop, RecordResume);
  }
  void TearDown() override {
    SetDebugBreakHandlers(nullptr, nullptr);
    Debug_SetMessageBoxHandlers(previous_boxes_);
    // Messagebox_title is process-global; put back the startup default.
    SetMessageBoxTitle("Message");
    fake_dialogs::Reset();
  }

private:
  DebugMessageBoxHandlers previous_boxes_{};
};

TEST_F(DdebugTest, OutrageMessageBoxFormatsMessageAndReturnsUsersChoice) {
  fake_dialogs::response = IDNO;

  int answer = OutrageMessageBox(MBOX_YESNO, "Delete room %d from %s?", 7, "level1");

  EXPECT_EQ(answer, IDNO);
  ASSERT_EQ(fake_dialogs::shown.size(), 1u);
  EXPECT_EQ(fake_dialogs::shown[0].text, "Delete room 7 from level1?");
}

TEST_F(DdebugTest, SimpleOutrageMessageBoxShowsFormattedOkDialog) {
  OutrageMessageBox("%d scripts failed", 3);

  ASSERT_EQ(fake_dialogs::shown.size(), 1u);
  EXPECT_EQ(fake_dialogs::shown[0].type, OSMBOX_OK);
  EXPECT_EQ(fake_dialogs::shown[0].text, "3 scripts failed");
}

struct ButtonMapping {
  const char *name;
  int mbox_type;
  int expected_os_type;
};

void PrintTo(const ButtonMapping &mapping, std::ostream *os) { *os << mapping.name; }

class OutrageMessageBoxButtonsTest : public DdebugTest, public ::testing::WithParamInterface<ButtonMapping> {};

TEST_P(OutrageMessageBoxButtonsTest, RequestedButtonsReachTheDialog) {
  OutrageMessageBox(GetParam().mbox_type, "question");

  ASSERT_EQ(fake_dialogs::shown.size(), 1u);
  EXPECT_EQ(fake_dialogs::shown[0].type, GetParam().expected_os_type);
}

INSTANTIATE_TEST_SUITE_P(AllTypes, OutrageMessageBoxButtonsTest,
                         ::testing::Values(ButtonMapping{"Ok", MBOX_OK, OSMBOX_OK},
                                           ButtonMapping{"YesNo", MBOX_YESNO, OSMBOX_YESNO},
                                           ButtonMapping{"YesNoCancel", MBOX_YESNOCANCEL, OSMBOX_YESNOCANCEL},
                                           ButtonMapping{"AbortRetryIgnore", MBOX_ABORTRETRYIGNORE,
                                                         OSMBOX_ABORTRETRYIGNORE}),
                         [](const ::testing::TestParamInfo<ButtonMapping> &info) { return info.param.name; });

TEST_F(DdebugTest, OutrageMessageBoxTruncatesOverlongMessageAndWarnsFirst) {
  const std::string long_message(20000, 'x');
  fake_dialogs::response = IDYES;

  int answer = OutrageMessageBox(MBOX_YESNO, "%s", long_message.c_str());

  ASSERT_GE(fake_dialogs::shown.size(), 2u) << "expected an overflow warning before the real dialog";
  const fake_dialogs::Dialog &real = fake_dialogs::shown.back();
  EXPECT_EQ(real.type, OSMBOX_YESNO);
  EXPECT_FALSE(real.text.empty());
  EXPECT_LT(real.text.size(), long_message.size());
  EXPECT_EQ(long_message.compare(0, real.text.size(), real.text), 0) << "dialog text is not a prefix of the message";
  EXPECT_EQ(answer, IDYES);
}

TEST_F(DdebugTest, SimpleOutrageMessageBoxTruncatesOverlongMessageAndWarnsFirst) {
  const std::string long_message(20000, 'y');

  OutrageMessageBox("%s", long_message.c_str());

  ASSERT_GE(fake_dialogs::shown.size(), 2u) << "expected an overflow warning before the real dialog";
  const fake_dialogs::Dialog &real = fake_dialogs::shown.back();
  EXPECT_FALSE(real.text.empty());
  EXPECT_LT(real.text.size(), long_message.size());
  EXPECT_EQ(long_message.compare(0, real.text.size(), real.text), 0) << "dialog text is not a prefix of the message";
}

TEST_F(DdebugTest, SetMessageBoxTitleAppliesToLaterDialogs) {
  SetMessageBoxTitle("Descent 3 Message");
  OutrageMessageBox("first");
  // A shorter title must fully replace the previous one, not leave a tail.
  SetMessageBoxTitle("D3");
  OutrageMessageBox(MBOX_OK, "second");

  ASSERT_EQ(fake_dialogs::shown.size(), 2u);
  EXPECT_EQ(fake_dialogs::shown[0].title, "Descent 3 Message");
  EXPECT_EQ(fake_dialogs::shown[1].title, "D3");
}

// The game uses the debug-break "stop" callback to drop out of fullscreen
// graphics so the modal dialog is visible.

TEST_F(DdebugTest, AssertionFailedContinueBracketsDialogWithBreakCallbacks) {
#ifdef RELEASE
  GTEST_SKIP() << "AssertionFailed is a no-op in RELEASE builds";
#else
  fake_dialogs::response = IDYES;

  AssertionFailed("ptr != nullptr", "physics.cpp", 1234);

  EXPECT_EQ(fake_dialogs::events, (std::vector<std::string>{"stop", "dialog", "resume"}));
  ASSERT_EQ(fake_dialogs::shown.size(), 1u);
  EXPECT_EQ(fake_dialogs::shown[0].type, OSMBOX_YESNO);
  EXPECT_TRUE(Contains(fake_dialogs::shown[0].text, "ptr != nullptr"));
  EXPECT_TRUE(Contains(fake_dialogs::shown[0].text, "physics.cpp"));
  EXPECT_TRUE(Contains(fake_dialogs::shown[0].text, "1234"));
#endif
}

TEST_F(DdebugTest, AssertionFailedDecliningToContinueExitsWithFailure) {
#ifdef RELEASE
  GTEST_SKIP() << "AssertionFailed is a no-op in RELEASE builds";
#else
  fake_dialogs::response = IDNO;
  EXPECT_EXIT(AssertionFailed("x", "file.cpp", 1), NonZeroExitCode, "");
#endif
}

TEST_F(DdebugTest, Int3MessageBoxContinueBracketsDialogWithBreakCallbacks) {
#ifdef RELEASE
  GTEST_SKIP() << "Int3MessageBox is a no-op in RELEASE builds";
#else
  fake_dialogs::response = IDYES;

  Int3MessageBox("render.cpp", 567);

  EXPECT_EQ(fake_dialogs::events, (std::vector<std::string>{"stop", "dialog", "resume"}));
  ASSERT_EQ(fake_dialogs::shown.size(), 1u);
  EXPECT_EQ(fake_dialogs::shown[0].type, OSMBOX_YESNO);
  EXPECT_TRUE(Contains(fake_dialogs::shown[0].text, "render.cpp"));
  EXPECT_TRUE(Contains(fake_dialogs::shown[0].text, "567"));
#endif
}

TEST_F(DdebugTest, Int3MessageBoxDecliningToContinueExitsWithFailure) {
#ifdef RELEASE
  GTEST_SKIP() << "Int3MessageBox is a no-op in RELEASE builds";
#else
  fake_dialogs::response = IDNO;
  EXPECT_EXIT(Int3MessageBox("file.cpp", 1), NonZeroExitCode, "");
#endif
}

TEST_F(DdebugTest, ErrorTerminatesProgramWhenAcknowledged) {
  fake_dialogs::response = IDOK;
  EXPECT_EXIT(Error("Cannot load %s", "level.d3l"), AnyExitCode, "");
}

TEST_F(DdebugTest, ErrorCancelInDebugBuildContinuesExecution) {
#ifndef _DEBUG
  GTEST_SKIP() << "Error() can only be ignored in _DEBUG builds";
#else
  fake_dialogs::response = IDCANCEL;

  // If Error() exits instead of returning, the shared test main fails the run.
  Error("Cannot load %s (code %d)", "level.d3l", 42);

  EXPECT_EQ(fake_dialogs::events, (std::vector<std::string>{"stop", "dialog", "resume"}));
  ASSERT_EQ(fake_dialogs::shown.size(), 1u);
  EXPECT_TRUE(Contains(fake_dialogs::shown[0].text, "Cannot load level.d3l (code 42)"));
#endif
}

#if !defined(RELEASE) && SDL_ASSERT_LEVEL >= 2
#define DDEBUG_TEST_ASSERTS_ACTIVE 1
#else
#define DDEBUG_TEST_ASSERTS_ACTIVE 0
#endif

class AssertMacroTest : public DdebugTest {
protected:
  void SetUp() override {
    DdebugTest::SetUp();
    previous_handler_ = SDL_GetAssertionHandler(&previous_userdata_);
    SDL_SetAssertionHandler(fake_dialogs::CountAndIgnoreAssertion, &fired);
  }
  void TearDown() override {
    SDL_SetAssertionHandler(previous_handler_, previous_userdata_);
    SDL_ResetAssertionReport();
    DdebugTest::TearDown();
  }
  int fired = 0;

private:
  SDL_AssertionHandler previous_handler_ = nullptr;
  void *previous_userdata_ = nullptr;
};

TEST_F(AssertMacroTest, AssertReportsOnlyFalseConditions) {
#if !DDEBUG_TEST_ASSERTS_ACTIVE
  GTEST_SKIP() << "ASSERT is compiled out in this configuration";
#else
  int value = 3;
  ASSERT(value == 3);
  EXPECT_EQ(fired, 0);

  ASSERT(value == 4);
  EXPECT_EQ(fired, 1);
#endif
}

TEST_F(AssertMacroTest, DebugBreakBracketsBreakWithCallbacks) {
#if !DDEBUG_TEST_ASSERTS_ACTIVE
  GTEST_SKIP() << "DEBUG_BREAK is compiled out in this configuration";
#else
  DEBUG_BREAK();

  EXPECT_EQ(fake_dialogs::events, (std::vector<std::string>{"stop", "assert", "resume"}));
#endif
}

} // namespace
