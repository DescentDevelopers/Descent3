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


// The main() of every test executable. Besides running the tests, it keeps the game's own failure paths from making
// a test look like it passed, or from stalling an unattended run:
// - Error() ends the program with exit(0), which CTest would count as a pass. Exiting before the tests have finished
//   fails the run instead. Death-test children are exempt, since exiting is what they are for.
// - ASSERT() and Int3() go through SDL_assert, whose default handler shows a dialog in debug builds. A failed game
//   assertion fails the current test instead (see ExpectGameAssertions).
// - Error(), Int3MessageBox(), AssertionFailed() and OutrageMessageBox() show their dialogs through Debug_ErrorBox and
//   Debug_MessageBox. Any of those dialogs fails the current test and is answered without being shown: "Yes" where
//   the dialog offers to continue, otherwise the answer that ends the program, which the exit check then reports.

#include <cstdio>
#include <cstdlib>
#include <string>

#include <SDL3/SDL_assert.h>
#include <gtest/gtest.h>

#include "debug.h"

namespace {

bool tests_finished = false;

void FailIfExitingEarly() {
  if (tests_finished || ::testing::internal::InDeathTestChild())
    return;
  std::string during;
  if (const ::testing::TestInfo *test = ::testing::UnitTest::GetInstance()->current_test_info())
    during = std::string(" during ") + test->test_suite_name() + "." + test->name();
  std::fprintf(stderr, "\nFAILURE: the program exited%s before its tests finished.\n", during.c_str());
  std::fflush(stderr);
  std::_Exit(1);
}

SDL_AssertState SDLCALL FailOnGameAssertion(const SDL_AssertData *data, void *) {
  ADD_FAILURE_AT(data->filename, data->linenum) << "Game assertion failed: " << data->condition;
  return SDL_ASSERTION_IGNORE;
}

int AnswerDialog(int type, const std::string &text) {
  ADD_FAILURE() << "The game showed a dialog: " << text;
  switch (type) {
  case OSMBOX_YESNO:
  case OSMBOX_YESNOCANCEL:
    return IDYES;
  case OSMBOX_ABORTRETRYIGNORE:
    return IDABORT;
  default:
    return IDOK;
  }
}

std::string Join(const char *a, const char *b, const char *c = nullptr) {
  std::string text;
  for (const char *s : {a, b, c}) {
    if (s && *s)
      text += (text.empty() ? "" : " | ") + std::string(s);
  }
  return text;
}

int FailOnErrorBox(int type, const char *topstring, const char *title, const char *bottomstring) {
  return AnswerDialog(type, Join(topstring, title, bottomstring));
}

int FailOnMessageBox(int type, const char *title, const char *str) { return AnswerDialog(type, Join(title, str)); }

} // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  std::atexit(FailIfExitingEarly);
  SDL_SetAssertionHandler(FailOnGameAssertion, nullptr);
  Debug_SetMessageBoxHandlers({FailOnErrorBox, FailOnMessageBox});
  int result = RUN_ALL_TESTS();
  tests_finished = true;
  return result;
}
