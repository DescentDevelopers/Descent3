/*
 * Descent 3
 * Copyright (C) 2024 Descent Developers
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

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include <gtest/gtest.h>
#include <SDL3/SDL_stdinc.h>

#include "ddio.h"

TEST(D3, DDIO_GetTmpFileName) {
  std::filesystem::path temp_dir = std::filesystem::temp_directory_path();
  std::filesystem::path result = ddio_GetTmpFileName(temp_dir, "prefix_");
  EXPECT_FALSE(result.empty());
  EXPECT_EQ(result.extension(), ".tmp");
  EXPECT_TRUE(canonical(result.parent_path()) == canonical(temp_dir));
}

  // we intentionally do NOT use the same RNG as the "game" logic. that RNG is seed-controlled for tests,
  // and using it here would introduce an uncontrollable source of randomness from potential tmpdir collisions
TEST(D3, DDIO_GetTmpFileNameDoesNotUseRand) {
  std::filesystem::path temp_dir = std::filesystem::temp_directory_path();
  srand(1234);
  int expected = rand();
  srand(1234);
  ddio_GetTmpFileName(temp_dir, "prefix_");
  EXPECT_EQ(rand(), expected);
}

TEST(D3, DDIO_CreateDirectoriesCreatesMissingParents) {
  std::filesystem::path base = std::filesystem::temp_directory_path() / "ddio_tests_create_parents";
  std::filesystem::remove_all(base);
  std::filesystem::path dir = base / "a" / "b";

  EXPECT_TRUE(ddio_CreateDirectories(dir));
  EXPECT_TRUE(std::filesystem::is_directory(dir));
#ifndef WIN32
  EXPECT_EQ(std::filesystem::status(base).permissions(), std::filesystem::perms::owner_all);
  EXPECT_EQ(std::filesystem::status(base / "a").permissions(), std::filesystem::perms::owner_all);
  EXPECT_EQ(std::filesystem::status(dir).permissions(), std::filesystem::perms::owner_all);
#endif

  std::filesystem::remove_all(base);
}

TEST(D3, DDIO_CreateDirectoriesUsesGivenPermissions) {
  std::filesystem::path base = std::filesystem::temp_directory_path() / "ddio_tests_given_permissions";
  std::filesystem::remove_all(base);
  std::filesystem::path dir = base / "a";
  const auto perms = std::filesystem::perms::owner_all | std::filesystem::perms::group_read |
                     std::filesystem::perms::group_exec;

  EXPECT_TRUE(ddio_CreateDirectories(dir, perms));
  EXPECT_TRUE(std::filesystem::is_directory(dir));
#ifndef WIN32
  EXPECT_EQ(std::filesystem::status(base).permissions(), perms);
  EXPECT_EQ(std::filesystem::status(dir).permissions(), perms);
#endif

  std::filesystem::remove_all(base);
}

TEST(D3, DDIO_CreateDirectoriesLeavesExistingDirectoriesAlone) {
  std::filesystem::path dir = std::filesystem::temp_directory_path() / "ddio_tests_existing";
  std::filesystem::remove_all(dir);
  std::filesystem::create_directory(dir);
  auto perms_before = std::filesystem::status(dir).permissions();

  EXPECT_TRUE(ddio_CreateDirectories(dir));
  EXPECT_EQ(std::filesystem::status(dir).permissions(), perms_before);

  std::filesystem::remove_all(dir);
}

TEST(D3, DDIO_CreateDirectoriesFailsWhenFileIsInTheWay) {
  std::filesystem::path base = std::filesystem::temp_directory_path() / "ddio_tests_file_in_the_way";
  std::filesystem::remove_all(base);
  std::filesystem::create_directory(base);
  std::ofstream(base / "file").close();

  EXPECT_FALSE(ddio_CreateDirectories(base / "file"));
  EXPECT_FALSE(ddio_CreateDirectories(base / "file" / "sub"));

  std::filesystem::remove_all(base);
}

TEST(D3, DDIO_CopyLegacyPathCopiesFile) {
  std::filesystem::path base = std::filesystem::temp_directory_path() / "ddio_tests_copy_file";
  std::filesystem::remove_all(base);
  std::filesystem::create_directories(base / "old");
  std::filesystem::create_directories(base / "new");
  std::ofstream(base / "old" / "settings").close();

  EXPECT_EQ(ddio_CopyLegacyPath(base / "old" / "settings", base / "new" / "settings"), base / "new" / "settings");
  EXPECT_TRUE(std::filesystem::exists(base / "new" / "settings"));
  EXPECT_TRUE(std::filesystem::exists(base / "old" / "settings"));
  EXPECT_TRUE(std::filesystem::exists(base / "old" / (std::string("settings") + DDIO_ABANDONED_SUFFIX)));

  std::filesystem::remove_all(base);
}

TEST(D3, DDIO_CopyLegacyPathDoesNotCopyAbandonedFileAgain) {
  std::filesystem::path base = std::filesystem::temp_directory_path() / "ddio_tests_copy_abandoned_file";
  std::filesystem::remove_all(base);
  std::filesystem::create_directories(base / "old");
  std::filesystem::create_directories(base / "new");
  std::ofstream(base / "old" / "settings").close();
  std::ofstream(base / "old" / (std::string("settings") + DDIO_ABANDONED_SUFFIX)).close();

  // The copy is gone again, e.g. because the user deleted it
  EXPECT_EQ(ddio_CopyLegacyPath(base / "old" / "settings", base / "new" / "settings"), base / "new" / "settings");
  EXPECT_FALSE(std::filesystem::exists(base / "new" / "settings"));
  EXPECT_TRUE(std::filesystem::exists(base / "old" / "settings"));

  std::filesystem::remove_all(base);
}

TEST(D3, DDIO_CopyLegacyPathCopiesDirectoryOverEmptyDirectory) {
  std::filesystem::path base = std::filesystem::temp_directory_path() / "ddio_tests_copy_dir";
  std::filesystem::remove_all(base);
  std::filesystem::create_directories(base / "old" / "savegame");
  std::filesystem::create_directories(base / "new");
  std::ofstream(base / "old" / "pilot.plt").close();
  std::ofstream(base / "old" / "savegame" / "save0").close();

  EXPECT_EQ(ddio_CopyLegacyPath(base / "old", base / "new"), base / "new");
  EXPECT_TRUE(std::filesystem::exists(base / "new" / "pilot.plt"));
  EXPECT_TRUE(std::filesystem::exists(base / "new" / "savegame" / "save0"));
  EXPECT_TRUE(std::filesystem::exists(base / "old" / "pilot.plt"));
  EXPECT_TRUE(std::filesystem::exists(base / "old" / "savegame" / "save0"));
  EXPECT_FALSE(std::filesystem::exists(base / "new.partial"));
  // The marker is next to the old directory, which is left untouched
  auto old_entries = std::distance(std::filesystem::directory_iterator(base / "old"), {});
  EXPECT_EQ(old_entries, 2);

  std::ifstream readme(base / (std::string("old") + DDIO_ABANDONED_SUFFIX), std::ios::binary);
  ASSERT_TRUE(readme.is_open());
  std::string text{std::istreambuf_iterator<char>(readme), std::istreambuf_iterator<char>()};
  EXPECT_NE(text.find((base / "old").lexically_normal().u8string()), std::string::npos);
  EXPECT_NE(text.find((base / "new").lexically_normal().u8string()), std::string::npos);
  readme.close();

  std::filesystem::remove_all(base);
}

TEST(D3, DDIO_CopyLegacyPathKeepsExistingTarget) {
  std::filesystem::path base = std::filesystem::temp_directory_path() / "ddio_tests_copy_existing";
  std::filesystem::remove_all(base);
  std::filesystem::create_directories(base / "old");
  std::filesystem::create_directories(base / "new");
  std::ofstream(base / "old" / "old.plt").close();
  std::ofstream(base / "new" / "new.plt").close();

  EXPECT_EQ(ddio_CopyLegacyPath(base / "old", base / "new"), base / "new");
  EXPECT_TRUE(std::filesystem::exists(base / "new" / "new.plt"));
  EXPECT_FALSE(std::filesystem::exists(base / "new" / "old.plt"));
  EXPECT_TRUE(std::filesystem::exists(base / "old" / "old.plt"));
  EXPECT_FALSE(std::filesystem::exists(base / (std::string("old") + DDIO_ABANDONED_SUFFIX)));

  std::filesystem::remove_all(base);
}

TEST(D3, DDIO_CopyLegacyPathDoesNotCopyAbandonedDirectoryAgain) {
  std::filesystem::path base = std::filesystem::temp_directory_path() / "ddio_tests_copy_abandoned";
  std::filesystem::remove_all(base);
  std::filesystem::create_directories(base / "old");
  std::filesystem::create_directories(base / "new");
  std::ofstream(base / "old" / "pilot.plt").close();
  std::ofstream(base / (std::string("old") + DDIO_ABANDONED_SUFFIX)).close();

  // The new location is empty again, e.g. because the user deleted its files
  EXPECT_EQ(ddio_CopyLegacyPath(base / "old", base / "new"), base / "new");
  EXPECT_TRUE(std::filesystem::is_empty(base / "new"));
  EXPECT_TRUE(std::filesystem::exists(base / "old" / "pilot.plt"));

  std::filesystem::remove_all(base);
}

TEST(D3, DDIO_CopyLegacyPathWithNothingToCopy) {
  std::filesystem::path base = std::filesystem::temp_directory_path() / "ddio_tests_copy_nothing";
  std::filesystem::remove_all(base);
  std::filesystem::create_directories(base);
  std::ofstream(base / "settings").close();

  EXPECT_EQ(ddio_CopyLegacyPath(base / "missing", base / "new"), base / "new");
  EXPECT_FALSE(std::filesystem::exists(base / "new"));
  EXPECT_EQ(ddio_CopyLegacyPath(base / "settings", base / "settings"), base / "settings");
  EXPECT_TRUE(std::filesystem::exists(base / "settings"));

  std::filesystem::remove_all(base);
}

TEST(D3, DDIO_CopyLegacyPathReplacesLeftoverPartialCopy) {
  std::filesystem::path base = std::filesystem::temp_directory_path() / "ddio_tests_copy_leftover";
  std::filesystem::remove_all(base);
  std::filesystem::create_directories(base / "old");
  std::filesystem::create_directories(base / "new.partial");
  std::ofstream(base / "old" / "pilot.plt").close();
  std::ofstream(base / "new.partial" / "stale").close();

  EXPECT_EQ(ddio_CopyLegacyPath(base / "old", base / "new"), base / "new");
  EXPECT_TRUE(std::filesystem::exists(base / "new" / "pilot.plt"));
  EXPECT_FALSE(std::filesystem::exists(base / "new" / "stale"));
  EXPECT_FALSE(std::filesystem::exists(base / "new.partial"));

  std::filesystem::remove_all(base);
}

#ifndef WIN32
TEST(D3, DDIO_CopyLegacyPathSkipsUnreadableSource) {
  std::filesystem::path base = std::filesystem::temp_directory_path() / "ddio_tests_copy_unreadable";
  std::filesystem::remove_all(base);
  std::filesystem::create_directories(base / "locked" / "old");
  std::ofstream(base / "locked" / "old" / "pilot.plt").close();
  std::filesystem::permissions(base / "locked", std::filesystem::perms::none);

  std::error_code ec;
  if (std::filesystem::exists(base / "locked" / "old", ec)) {
    std::filesystem::permissions(base / "locked", std::filesystem::perms::owner_all);
    std::filesystem::remove_all(base);
    GTEST_SKIP() << "permissions are not enforced for this user";
  }
  EXPECT_EQ(ddio_CopyLegacyPath(base / "locked" / "old", base / "new"), base / "new");
  EXPECT_FALSE(std::filesystem::exists(base / "new"));

  std::filesystem::permissions(base / "locked", std::filesystem::perms::owner_all);
  std::filesystem::remove_all(base);
}

TEST(D3, DDIO_CopyLegacyPathUsesCopyWhenMarkerCannotBeWritten) {
  std::filesystem::path base = std::filesystem::temp_directory_path() / "ddio_tests_copy_no_marker";
  std::filesystem::remove_all(base);
  std::filesystem::create_directories(base / "readonly" / "old");
  std::ofstream(base / "readonly" / "old" / "pilot.plt").close();
  std::filesystem::permissions(base / "readonly",
                               std::filesystem::perms::owner_read | std::filesystem::perms::owner_exec);

  if (std::ofstream(base / "readonly" / "probe")) {
    std::filesystem::permissions(base / "readonly", std::filesystem::perms::owner_all);
    std::filesystem::remove_all(base);
    GTEST_SKIP() << "permissions are not enforced for this user";
  }
  EXPECT_EQ(ddio_CopyLegacyPath(base / "readonly" / "old", base / "new"), base / "new");
  EXPECT_TRUE(std::filesystem::exists(base / "new" / "pilot.plt"));
  EXPECT_FALSE(std::filesystem::exists(base / "readonly" / (std::string("old") + DDIO_ABANDONED_SUFFIX)));

  std::filesystem::permissions(base / "readonly", std::filesystem::perms::owner_all);
  std::filesystem::remove_all(base);
}
#endif

TEST(D3, DDIO_CopyLegacyPathKeepsOldLocationWhenCopyFails) {
  std::filesystem::path base = std::filesystem::temp_directory_path() / "ddio_tests_copy_fails";
  std::filesystem::remove_all(base);
  std::filesystem::create_directories(base / "old");
  std::ofstream(base / "old" / "pilot.plt").close();

  // The target's parent directory does not exist, so the copy fails
  EXPECT_EQ(ddio_CopyLegacyPath(base / "old", base / "missing" / "new"), base / "old");
  EXPECT_TRUE(std::filesystem::exists(base / "old" / "pilot.plt"));
  EXPECT_FALSE(std::filesystem::exists(base / "missing"));
  EXPECT_FALSE(std::filesystem::exists(base / (std::string("old") + DDIO_ABANDONED_SUFFIX)));

  std::filesystem::remove_all(base);
}

#if defined(__LINUX__)
// Saves the environment variables used by the XDG lookups before each test and restores them afterwards, and
// provides an empty scratch directory to point them at. Each test gets its own directory, since ctest -j runs the
// tests in parallel processes.
class DDIO_XDG : public ::testing::Test {
protected:
  void SetUp() override {
    for (const char *name : kVariables) {
      const char *value = SDL_getenv(name);
      saved_.emplace_back(name, value ? std::optional<std::string>(value) : std::nullopt);
      SDL_UnsetEnvironmentVariable(SDL_GetEnvironment(), name);
    }
    base_ = std::filesystem::temp_directory_path() /
            ("ddio_tests_xdg_" + std::string(::testing::UnitTest::GetInstance()->current_test_info()->name()));
    std::filesystem::remove_all(base_);
    std::filesystem::create_directory(base_);
  }

  void TearDown() override {
    for (const auto &[name, value] : saved_) {
      if (value) {
        SDL_SetEnvironmentVariable(SDL_GetEnvironment(), name, value->c_str(), true);
      } else {
        SDL_UnsetEnvironmentVariable(SDL_GetEnvironment(), name);
      }
    }
    std::filesystem::remove_all(base_);
  }

  static constexpr const char *kVariables[] = {"HOME",           "XDG_CACHE_HOME", "XDG_CONFIG_HOME",
                                               "XDG_STATE_HOME", "XDG_DATA_HOME",  "XDG_DATA_DIRS"};
  std::vector<std::pair<const char *, std::optional<std::string>>> saved_;
  std::filesystem::path base_;
};

TEST_F(DDIO_XDG, TempPathUsesXDGCacheHome) {
  SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "XDG_CACHE_HOME", (base_ / "cache").c_str(), true);
  SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "HOME", (base_ / "home").c_str(), true);
  EXPECT_EQ(ddio_GetTempPath(), base_ / "cache" / "descent3");
}

TEST_F(DDIO_XDG, TempPathIgnoresEmptyXDGCacheHome) {
  SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "XDG_CACHE_HOME", "", true);
  SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "HOME", base_.c_str(), true);
  EXPECT_EQ(ddio_GetTempPath(), base_ / ".cache" / "descent3");
}

TEST_F(DDIO_XDG, TempPathIgnoresRelativeXDGCacheHome) {
  SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "XDG_CACHE_HOME", "relative/cache", true);
  SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "HOME", base_.c_str(), true);
  EXPECT_EQ(ddio_GetTempPath(), base_ / ".cache" / "descent3");
}

TEST_F(DDIO_XDG, TempPathFallsBackToSystemTempDirectory) {
  SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "HOME", "", true);
  EXPECT_EQ(ddio_GetTempPath(), std::filesystem::temp_directory_path() / "Descent3" / "cache");
}

TEST_F(DDIO_XDG, ConfigPathUsesXDGConfigHome) {
  SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "XDG_CONFIG_HOME", (base_ / "config").c_str(), true);
  SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "HOME", (base_ / "home").c_str(), true);
  std::filesystem::path expected = base_ / "config" / "descent3";
  EXPECT_EQ(ddio_GetConfigPath(), expected);
  EXPECT_TRUE(std::filesystem::is_directory(expected));
  EXPECT_EQ(std::filesystem::status(expected).permissions(), std::filesystem::perms::owner_all);
}

TEST_F(DDIO_XDG, ConfigPathFallsBackToHome) {
  SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "XDG_CONFIG_HOME", "", true);
  SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "HOME", base_.c_str(), true);
  EXPECT_EQ(ddio_GetConfigPath(), base_ / ".config" / "descent3");
}

TEST_F(DDIO_XDG, ConfigPathFallsBackToPrefPath) {
  SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "HOME", "", true);
  SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "XDG_DATA_HOME", (base_ / "data").c_str(), true);
  EXPECT_EQ(ddio_GetConfigPath(), base_ / "data" / "descent3");
}

TEST_F(DDIO_XDG, PrefPathUsesXDGDataHome) {
  SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "XDG_DATA_HOME", (base_ / "data").c_str(), true);
  SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "HOME", (base_ / "home").c_str(), true);
  std::filesystem::path expected = base_ / "data" / "descent3";
  EXPECT_EQ(ddio_GetPrefPath(), expected);
  EXPECT_EQ(std::filesystem::status(expected).permissions(), std::filesystem::perms::owner_all);
}

TEST_F(DDIO_XDG, PrefPathIgnoresEmptyXDGDataHome) {
  SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "XDG_DATA_HOME", "", true);
  SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "HOME", base_.c_str(), true);
  EXPECT_EQ(ddio_GetPrefPath(), base_ / ".local" / "share" / "descent3");
}

TEST_F(DDIO_XDG, PrefPathFailsWithoutUsableDirectory) {
  SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "XDG_DATA_HOME", "relative/data", true);
  SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "HOME", "", true);
  EXPECT_TRUE(ddio_GetPrefPath().empty());
}

TEST_F(DDIO_XDG, PrefPathCopiesLegacyDirectory) {
  SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "XDG_DATA_HOME", (base_ / "data").c_str(), true);
  std::filesystem::path legacy = base_ / "data" / "Outrage Entertainment" / "Descent 3";
  std::filesystem::create_directories(legacy / "savegame");
  std::ofstream(legacy / "pilot.plt").close();

  std::filesystem::path expected = base_ / "data" / "descent3";
  EXPECT_EQ(ddio_GetPrefPath(), expected);
  EXPECT_TRUE(std::filesystem::exists(expected / "pilot.plt"));
  EXPECT_TRUE(std::filesystem::is_directory(expected / "savegame"));
  EXPECT_TRUE(std::filesystem::exists(legacy / "pilot.plt"));
  EXPECT_TRUE(std::filesystem::is_directory(legacy / "savegame"));
  EXPECT_TRUE(std::filesystem::exists(base_ / "data" / "Outrage Entertainment" / "Descent 3.ABANDONED.readme"));
}

TEST_F(DDIO_XDG, PrefPathDoesNotCopyLegacyDirectoryOverExistingFiles) {
  SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "XDG_DATA_HOME", (base_ / "data").c_str(), true);
  std::filesystem::path legacy = base_ / "data" / "Outrage Entertainment" / "Descent 3";
  std::filesystem::path current = base_ / "data" / "descent3";
  std::filesystem::create_directories(legacy);
  std::filesystem::create_directories(current);
  std::ofstream(legacy / "old.plt").close();
  std::ofstream(current / "new.plt").close();

  EXPECT_EQ(ddio_GetPrefPath(), current);
  EXPECT_TRUE(std::filesystem::exists(current / "new.plt"));
  EXPECT_FALSE(std::filesystem::exists(current / "old.plt"));
  EXPECT_TRUE(std::filesystem::exists(legacy / "old.plt"));
}

TEST_F(DDIO_XDG, SystemDataPathsDefault) {
  std::vector<std::filesystem::path> expected = {"/usr/local/share/descent3", "/usr/share/descent3"};
  EXPECT_EQ(ddio_GetSystemDataPaths(), expected);
  SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "XDG_DATA_DIRS", "", true);
  EXPECT_EQ(ddio_GetSystemDataPaths(), expected);
}

TEST_F(DDIO_XDG, SystemDataPathsFollowXDGDataDirs) {
  SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "XDG_DATA_DIRS", "/app/share:relative/share::/opt/share/", true);
  std::vector<std::filesystem::path> expected = {"/app/share/descent3", "/opt/share/descent3"};
  EXPECT_EQ(ddio_GetSystemDataPaths(), expected);
}

TEST_F(DDIO_XDG, StatePathUsesXDGStateHome) {
  SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "XDG_STATE_HOME", (base_ / "state").c_str(), true);
  SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "HOME", (base_ / "home").c_str(), true);
  std::filesystem::path expected = base_ / "state" / "descent3";
  EXPECT_EQ(ddio_GetStatePath(), expected);
  EXPECT_TRUE(std::filesystem::is_directory(expected));
  EXPECT_EQ(std::filesystem::status(expected).permissions(), std::filesystem::perms::owner_all);
}

TEST_F(DDIO_XDG, StatePathFallsBackToHome) {
  SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "XDG_STATE_HOME", "relative/state", true);
  SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "HOME", base_.c_str(), true);
  EXPECT_EQ(ddio_GetStatePath(), base_ / ".local" / "state" / "descent3");
}

TEST_F(DDIO_XDG, StatePathFallsBackToWorkingDirectory) {
  SDL_SetEnvironmentVariable(SDL_GetEnvironment(), "HOME", "", true);
  EXPECT_EQ(ddio_GetStatePath(), std::filesystem::current_path());
}
#else
TEST(D3, DDIO_GetStatePathIsWorkingDirectory) { EXPECT_EQ(ddio_GetStatePath(), std::filesystem::current_path()); }

TEST(D3, DDIO_GetSystemDataPathsIsEmpty) { EXPECT_TRUE(ddio_GetSystemDataPaths().empty()); }
#endif
