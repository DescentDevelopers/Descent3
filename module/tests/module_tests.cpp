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

#include <filesystem>
#include <string>

#include <gtest/gtest.h>

#include "module.h"

#include "support/module_test_support.h"

namespace {

namespace fs = std::filesystem;

// ModLastError is process-global; start every test from a clean slate.
class ModuleTest : public ::testing::Test {
protected:
  void SetUp() override { mod_GetLastError(); }
};

TEST(ModuleRealName, AppendsPlatformExtensionWhenMissing) {
  EXPECT_EQ(mod_GetRealModuleName("anarchy"), fs::path("anarchy" MODULE_EXT));
  EXPECT_EQ(mod_GetRealModuleName(fs::path("netgames") / "anarchy"), fs::path("netgames") / ("anarchy" MODULE_EXT));
}

TEST(ModuleRealName, DotInDirectoryNameIsNotAnExtension) {
  EXPECT_EQ(mod_GetRealModuleName(fs::path("scripts.v2") / "level1"), fs::path("scripts.v2") / ("level1" MODULE_EXT));
}

TEST(ModuleRealName, MapsAnyPlatformsModuleExtensionToThisPlatforms) {
  for (const char *ext : {".dll", ".so", ".dylib", ".msl"}) {
    SCOPED_TRACE(ext);
    EXPECT_EQ(mod_GetRealModuleName(fs::path("data") / (std::string("level1") + ext)),
              fs::path("data") / ("level1" MODULE_EXT));
  }
}

TEST(ModuleRealName, RecognisesModuleExtensionsCaseInsensitively) {
  for (const char *ext : {".DLL", ".So", ".DYLIB", ".MSL"}) {
    SCOPED_TRACE(ext);
    fs::path result = mod_GetRealModuleName(std::string("LEVEL1") + ext);
    // Whether a native extension keeps its original case is not part of the contract; the name and the
    // (case-folded) extension are.
    EXPECT_EQ(result.stem(), fs::path("LEVEL1"));
    EXPECT_EQ(ToLower(result.extension().u8string()), MODULE_EXT);
  }
}

TEST(ModuleRealName, LeavesUnrelatedExtensionsAlone) {
  EXPECT_EQ(mod_GetRealModuleName("readme.txt"), fs::path("readme.txt"));
  EXPECT_EQ(mod_GetRealModuleName(fs::path("dir") / "level1.d3m"), fs::path("dir") / "level1.d3m");
}

TEST_F(ModuleTest, LoadResolveCallAndFree) {
  ASSERT_TRUE(fs::exists(kFixture)) << kFixture;

  module_t mod{};
  ASSERT_TRUE(mod_LoadModule(&mod, kFixture));
  EXPECT_EQ(mod_GetLastError(), MODERR_NOERROR);

  // parmbytes is the stdcall argument size, passed the way the game does (e.g. 8 for two ints).
  auto add = reinterpret_cast<AddFn>(mod_GetSymbol(&mod, "ModuleTestAdd", 8));
  ASSERT_NE(add, nullptr);
  EXPECT_EQ(add(40, 2), 42);
  EXPECT_EQ(add(-7, 3), -4);

  // 255 means "look the name up exactly as given".
  auto magic = reinterpret_cast<MagicFn>(mod_GetSymbol(&mod, "ModuleTestMagic", 255));
  ASSERT_NE(magic, nullptr);
  EXPECT_EQ(magic(), 0x5eed);
  EXPECT_EQ(mod_GetLastError(), MODERR_NOERROR);

  EXPECT_TRUE(mod_FreeModule(&mod));
}

TEST_F(ModuleTest, LoadAddsPlatformExtensionWhenOmitted) {
  ASSERT_TRUE(fs::exists(kFixture)) << kFixture;
  fs::path without_ext = kFixture;
  without_ext.replace_extension();

  module_t mod{};
  ASSERT_TRUE(mod_LoadModule(&mod, without_ext)) << without_ext;
  EXPECT_NE(mod_GetSymbol(&mod, "ModuleTestMagic", 255), nullptr);
  EXPECT_TRUE(mod_FreeModule(&mod));
}

TEST_F(ModuleTest, MissingSymbolReturnsNullAndSetsError) {
  module_t mod{};
  ASSERT_TRUE(mod_LoadModule(&mod, kFixture));

  EXPECT_EQ(mod_GetSymbol(&mod, "NoSuchSymbolInModule", 0), nullptr);
  EXPECT_NE(mod_GetLastError(), MODERR_NOERROR);

  EXPECT_EQ(mod_GetSymbol(&mod, "NoSuchSymbolInModule", 255), nullptr);
  EXPECT_NE(mod_GetLastError(), MODERR_NOERROR);

  EXPECT_TRUE(mod_FreeModule(&mod));
}

TEST_F(ModuleTest, FreedModuleCannotBeUsedOrFreedAgain) {
  module_t mod{};
  ASSERT_TRUE(mod_LoadModule(&mod, kFixture));
  ASSERT_TRUE(mod_FreeModule(&mod));
  mod_GetLastError();

  EXPECT_EQ(mod_GetSymbol(&mod, "ModuleTestMagic", 255), nullptr);
  EXPECT_EQ(mod_GetLastError(), MODERR_NOMOD);

  EXPECT_FALSE(mod_FreeModule(&mod));
  EXPECT_NE(mod_GetLastError(), MODERR_NOERROR);
}

TEST_F(ModuleTest, GetLastErrorClearsTheError) {
  EXPECT_FALSE(mod_FreeModule(nullptr));
  EXPECT_EQ(mod_GetLastError(), MODERR_INVALIDHANDLE);
  EXPECT_EQ(mod_GetLastError(), MODERR_NOERROR);
}

TEST_F(ModuleTest, NullHandleIsRejected) {
  EXPECT_FALSE(mod_LoadModule(nullptr, kFixture));
  EXPECT_EQ(mod_GetLastError(), MODERR_INVALIDHANDLE);

  EXPECT_EQ(mod_GetSymbol(nullptr, "ModuleTestMagic", 255), nullptr);
  EXPECT_EQ(mod_GetLastError(), MODERR_INVALIDHANDLE);

  EXPECT_FALSE(mod_FreeModule(nullptr));
  EXPECT_EQ(mod_GetLastError(), MODERR_INVALIDHANDLE);
}

TEST_F(ModuleTest, NeverLoadedHandleIsRejected) {
  module_t mod{};
  EXPECT_EQ(mod_GetSymbol(&mod, "ModuleTestMagic", 255), nullptr);
  EXPECT_EQ(mod_GetLastError(), MODERR_NOMOD);
}

TEST_F(ModuleTest, EmptyFilenameFailsToLoad) {
  module_t mod{};
  EXPECT_FALSE(mod_LoadModule(&mod, fs::path()));
  EXPECT_NE(mod_GetLastError(), MODERR_NOERROR);
}

TEST_F(ModuleTest, MissingModuleReportsModNotFound) {
  fs::path missing = kFixture.parent_path() / ("module_test_does_not_exist" MODULE_EXT);
  ASSERT_FALSE(fs::exists(missing));

  module_t mod{};
  EXPECT_FALSE(mod_LoadModule(&mod, missing));
  EXPECT_EQ(mod_GetLastError(), MODERR_MODNOTFOUND);
}

} // namespace
