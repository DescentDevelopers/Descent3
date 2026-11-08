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

#if defined(__LINUX__)
// Saves the environment variables used by the XDG lookups before each test and restores them afterwards, and
// provides an empty scratch directory to point them at
class DDIO_XDG : public ::testing::Test {
protected:
  void SetUp() override {
    for (const char *name : kVariables) {
      const char *value = SDL_getenv(name);
      saved_.emplace_back(name, value ? std::optional<std::string>(value) : std::nullopt);
      SDL_UnsetEnvironmentVariable(SDL_GetEnvironment(), name);
    }
    base_ = std::filesystem::temp_directory_path() / "ddio_tests_xdg";
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
#endif
