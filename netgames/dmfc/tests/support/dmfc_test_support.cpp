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

#include <cstring>
#include <vector>

#include <gtest/gtest.h>

#include "dmfc_test_support.h"

namespace {

// In non-release builds DMFC's ASSERT() always calls through the game-provided DLLassert pointer. Route it to
// a gtest failure so that a violated DMFC assertion fails the test instead of crashing on a null pointer.
void TestAssert(int x, const char *expression, const char *file, int line) {
  if (!x)
    ADD_FAILURE_AT(file, line) << "DMFC ASSERT failed: " << expression;
}

class InstallAssertHandler : public ::testing::Environment {
public:
  void SetUp() override { DLLassert = TestAssert; }
};

const auto *const kInstallAssertHandler = ::testing::AddGlobalTestEnvironment(new InstallAssertHandler);

} // namespace

namespace dmfc_test {

bool HandleCommand(DMFCBase &dmfc, const char *text) {
  std::vector<char> buf(text, text + strlen(text) + 1);
  return dmfc.InputCommandHandle(buf.data());
}

} // namespace dmfc_test
