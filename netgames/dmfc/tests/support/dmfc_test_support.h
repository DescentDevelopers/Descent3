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

#ifndef DMFC_TEST_SUPPORT_H
#define DMFC_TEST_SUPPORT_H

#include <memory>

#include <gtest/gtest.h>

#include "gamedll_header.h"
#include "DMFC.h"
#include "dmfcinternal.h"

extern DMFCBase *basethis;

// A DMFCBase that releases the input-command list it owns (normally done by GameClose(), which needs the game).
class TestDMFC : public DMFCBase {
public:
  ~TestDMFC() override { InputCommandFree(); }
};

// Nothing from the game is loaded (no LoadFunctions()), so only code paths that do not reach into the game work.
class DMFCFixture : public ::testing::Test {
protected:
  void SetUp() override {
    dmfc = std::make_unique<TestDMFC>();
    basethis = dmfc.get();
  }
  void TearDown() override {
    basethis = nullptr;
    dmfc.reset();
  }

  std::unique_ptr<TestDMFC> dmfc;
};

namespace dmfc_test {

// Passes a copy of `text` to dmfc.InputCommandHandle(), which modifies its argument, and returns its result.
bool HandleCommand(DMFCBase &dmfc, const char *text);

} // namespace dmfc_test

#endif
