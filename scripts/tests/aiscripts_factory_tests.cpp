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

// This file is compiled into one test executable per module; the module under test is
// whichever aiscripts_<module>_module.cpp it is linked with.

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "support/aiscripts_test_support.h"

namespace {

class ScriptFactoryTest : public ::testing::Test {
protected:
  void SetUp() override {
    aiscripts_test::InstallEngineImports();
    names_ = aiscripts_test::RegisteredScriptNames();
    ASSERT_FALSE(names_.empty());
  }

  std::vector<std::string> names_;
};

TEST_F(ScriptFactoryTest, EveryRegisteredNameCreatesAnInstance) {
  for (const std::string &name : names_) {
    SCOPED_TRACE(name);
    const int id = aiscripts_test::ScriptIdForName(name.c_str());
    ASSERT_NE(id, -1) << "registered name is not recognised";

    void *instance = aiscripts_test::CreateScript(id);
    EXPECT_NE(instance, nullptr) << "no instance for ID " << id;
    if (instance)
      aiscripts_test::DestroyScript(id, instance);
  }
}

TEST_F(ScriptFactoryTest, RegisteredNamesResolveToDistinctIds) {
  std::map<int, std::string> owner_of_id;
  for (const std::string &name : names_) {
    const int id = aiscripts_test::ScriptIdForName(name.c_str());
    auto [it, inserted] = owner_of_id.emplace(id, name);
    EXPECT_TRUE(inserted) << "'" << name << "' and '" << it->second << "' both resolve to ID " << id;
  }
}

TEST_F(ScriptFactoryTest, UnknownNamesAreRejected) {
  EXPECT_EQ(aiscripts_test::ScriptIdForName("No Such Script In This Module"), -1);
  EXPECT_EQ(aiscripts_test::ScriptIdForName(""), -1);
}

TEST_F(ScriptFactoryTest, IdsNotIssuedByTheModuleCreateNothing) {
  int max_id = -1;
  for (const std::string &name : names_)
    max_id = std::max(max_id, aiscripts_test::ScriptIdForName(name.c_str()));

  EXPECT_EQ(aiscripts_test::CreateScript(-1), nullptr);
  EXPECT_EQ(aiscripts_test::CreateScript(max_id + 1), nullptr);
}

} // namespace
