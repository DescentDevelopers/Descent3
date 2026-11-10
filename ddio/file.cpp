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

#include <array>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <random>
#include <regex>
#include <string>
#include <vector>
#include <SDL3/SDL_filesystem.h>

#include "IOOps.h"
#include "d3_platform_path.h"
#include "ddio.h"
#include "mem.h"
#include "pserror.h"

const std::array<char, 4> LOCK_TAG = {'L', 'O', 'C', 'K'};

struct LockFileContent {
  std::array<char, 4> tag = LOCK_TAG;
  int32_t pid = -1;
};

std::ostream &operator<<(std::ostream &output, const LockFileContent &header) {
  std::copy(header.tag.begin(), header.tag.end(), std::ostream_iterator<char>(output));
  D3::bin_write(output, header.pid);
  return output;
}

std::istream &operator>>(std::istream &input, LockFileContent &header) {
  input.read(reinterpret_cast<char *>(header.tag.data()), 4);
  D3::bin_read(input, header.pid);
  return input;
}

bool ddio_CreateLockFile(const std::filesystem::path &dir) {
  std::filesystem::path lock_filename = dir / ".lock";

  if (!std::filesystem::is_directory(lock_filename.parent_path())) {
    return false;
  }

  LockFileContent lock_content;
  int32_t curr_pid = ddio_GetPID();

  if (std::filesystem::exists(lock_filename)) {
    try {
      std::ifstream lockfile(lock_filename, std::ios::binary);
      lockfile >> lock_content;
      lockfile.close();
    } catch (std::exception &e) {
      return false;
    }

    if (lock_content.tag != LOCK_TAG) {
      return false;
    }

    // it appears to be a lock file, check the pid
    ASSERT(lock_content.pid != -1);

    // check the file id in the file, compared to our pid
    if (lock_content.pid == curr_pid) {
      // lock file already exists for the current process
      return true;
    }

    if (ddio_CheckProcess(lock_content.pid)) {
      // Process exists, lock still valid
      return false;
    }

    // the process no longer exists, we can create a lock file if needed
    // we'll delete the useless one now
    // the lock file in the directory belongs to us!
    std::filesystem::remove(lock_filename);
  }

  lock_content.pid = curr_pid;

  try {
    std::ofstream lockfile(lock_filename, std::ios::binary);
    lockfile << lock_content;
    lockfile.close();
  } catch (std::exception &e) {
    return false;
  }

  // at this point result will either be 1 or 2 from checking the lock file
  // either way, a lock file has been created
  return true;
}

bool ddio_DeleteLockFile(const std::filesystem::path &dir) {
  int32_t curr_pid = ddio_GetPID();

  std::filesystem::path lock_filename = dir / ".lock";
  if (!std::filesystem::exists(lock_filename)) {
    return true;
  }

  LockFileContent lock_content;
  try {
    std::ifstream lockfile(lock_filename, std::ios::binary);
    lockfile >> lock_content;
    lockfile.close();
  } catch (std::exception &e) {
    return false;
  }

  if (lock_content.tag != LOCK_TAG) {
    return false;
  }

  ASSERT(lock_content.pid != -1);

  // check the file id in the file, compared to our pid
  if (lock_content.pid != curr_pid) {
    // it doesn't belong to
    return false;
  }

  std::error_code ec;
  // the lock file in the directory belongs to us!
  if (!std::filesystem::remove(lock_filename, ec)) {
    return false;
  }

  return true;
}

void ddio_DoForeachFile(const std::filesystem::path &search_path, const std::regex &regex,
                       const std::function<void(std::filesystem::path)> &func) {
  if (!std::filesystem::is_directory(search_path)) {
    return;
  }
  const std::filesystem::directory_iterator end;
  for (std::filesystem::directory_iterator iter{search_path}; iter != end; iter++) {
    const std::string filename = iter->path().filename().string();
    if (std::filesystem::is_regular_file(*iter)) {
      if (std::regex_match(filename, regex)) {
        func(iter->path());
      }
    }
  }
}

std::filesystem::path ddio_GetTmpFileName(const std::filesystem::path &basedir, const char *prefix) {
  static const char alphanum[] =
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";
  // size of random part
  const int len = 10;
  const char *ext = ".tmp";
  std::filesystem::path result;
  size_t len_result = strlen(PATH_TO_CSTR(basedir / prefix));
  char *random_name = (char *)mem_malloc(len_result + len + strlen(ext) + 1);
  strncpy(random_name, PATH_TO_CSTR(basedir / prefix), len_result);

  // we intentionally do NOT use the same RNG as the "game" logic. that RNG is seed-controlled for tests,
  // and using it here would introduce an uncontrollable source of randomness from potential tmpdir collisions
  thread_local std::minstd_rand gen{std::random_device{}()};
  std::uniform_int_distribution<size_t> pick(0, sizeof(alphanum) - 2);

  int tries = 20;
  while (tries > 0) {
    for (size_t i = len_result; i < len_result + len; i++) {
      random_name[i] = alphanum[pick(gen)];
    }
    random_name[len_result + len] = '\0';
    strcat(random_name, ext);
    if (!std::filesystem::exists(random_name)) {
      // Found unique name, break the loop
      result = random_name;
      break;
    }
    tries--;
  }
  mem_free(random_name);
  return result;
}

std::filesystem::path ddio_GetPrefPath() {
#if defined(__LINUX__)
  // SDL_GetPrefPath() uses $XDG_DATA_HOME and $HOME without checking that they are absolute, as the XDG Base
  // Directory spec requires (an empty value makes it read out of bounds). Unset unusable values so SDL falls
  // back the way the spec describes.
  for (const char *name : {"XDG_DATA_HOME", "HOME"}) {
    const char *value = SDL_getenv(name);
    if (value && !std::filesystem::path(value).is_absolute()) {
      SDL_UnsetEnvironmentVariable(SDL_GetEnvironment(), name);
    }
  }
#endif
  char *pref_path = SDL_GetPrefPath("", D3_SUBDIR);
  if (!pref_path) {
    LOG_ERROR << "Failed to get writable preference path!";
    return {};
  }
  std::filesystem::path result = std::filesystem::canonical(pref_path);
  SDL_free(pref_path);
  if (!result.has_filename()) {
    result = result.parent_path();
  }

  // Earlier versions used "Outrage Entertainment/Descent 3" next to this directory. SDL_GetPrefPath() has just
  // created this one if it was missing, so the old one is copied here while this one is still empty. It is copied,
  // not moved, so that those versions still find their files.
  result = ddio_CopyLegacyPath(result.parent_path() / "Outrage Entertainment" / "Descent 3", result);
  return std::filesystem::canonical(result);
}

bool ddio_CreateDirectories(const std::filesystem::path &dir, std::filesystem::perms perms) {
  std::error_code ec;
  if (std::filesystem::is_directory(dir, ec)) {
    return true;
  }
  std::filesystem::path parent = dir.parent_path();
  if (!parent.empty() && parent != dir && !ddio_CreateDirectories(parent, perms)) {
    return false;
  }
  bool created = std::filesystem::create_directory(dir, ec);
  if (ec) {
    LOG_ERROR << "Could not create directory " << dir << ": " << ec.message();
    return false;
  }
  // Not created but no error: another process made it, or dir ends in a separator and the parent call made it
  if (created) {
    std::filesystem::permissions(dir, perms, ec);
    if (ec) {
      LOG_WARNING << "Could not set permissions of directory " << dir << ": " << ec.message();
    }
  }
  return true;
}

std::filesystem::path ddio_CopyLegacyPath(const std::filesystem::path &from, const std::filesystem::path &to) {
  // 1. Decide whether to copy. A legacy path that has been copied before has a marker next to it. Only nothing, or an
  //    empty directory such as the one SDL_GetPrefPath() has just created, may be replaced at `to`.
  const std::filesystem::path marker = std::filesystem::path(from).concat(DDIO_ABANDONED_SUFFIX);
  try {
    if (from == to || !std::filesystem::exists(from) || std::filesystem::exists(marker)) {
      return to; // nothing to copy, or copied on an earlier run
    }
    const bool to_is_empty_directory = std::filesystem::is_directory(to) && std::filesystem::is_empty(to);
    if (std::filesystem::exists(to) && !to_is_empty_directory) {
      LOG_WARNING << "Not copying " << from << " to " << to << ", which already exists; " << from << " is not used";
      return to;
    }
  } catch (const std::filesystem::filesystem_error &e) {
    LOG_WARNING << "Not copying " << from << " to " << to << ": " << e.code().message();
    return to;
  }

  // 2. Copy next to the target and rename the copy into place, so a copy that fails halfway leaves nothing at the
  //    target and is tried again on the next start
  const std::filesystem::path partial = std::filesystem::path(to).concat(".partial");
  try {
    std::filesystem::remove_all(partial); // left over from an interrupted attempt
    std::filesystem::copy(from, partial,
                          std::filesystem::copy_options::recursive | std::filesystem::copy_options::copy_symlinks);
    std::filesystem::remove(to); // the empty directory, if there is one
    std::filesystem::rename(partial, to);
  } catch (const std::filesystem::filesystem_error &e) {
    LOG_WARNING << "Could not copy " << from << " to " << to << ", using the old location: " << e.code().message();
    std::error_code ignored;
    std::filesystem::remove_all(partial, ignored);
    return from;
  }
  LOG_INFO << "Copied " << from << " to " << to;

  // 3. Leave the marker, saying where the files went
  try {
    std::ofstream out(marker, std::ios::binary);
    out.exceptions(std::ios::failbit | std::ios::badbit); // throws right away if the open failed
    out << "Descent 3 has copied\n"
        << "\n"
        << "    " << from.lexically_normal().u8string() << "\n"
        << "\n"
        << "to\n"
        << "\n"
        << "    " << to.lexically_normal().u8string() << "\n"
        << "\n"
        << "and now uses it from there. Changes made in one place do not show up in the\n"
        << "other. The original has been left in place in case an earlier version of\n"
        << "Descent 3 still uses it. If not, you can delete it.\n";
    out.close();
  } catch (const std::exception &e) { // not std::ios_base::failure, which GCC's library may throw with another ABI
    LOG_WARNING << "Could not write " << marker << ": " << e.what();
  }
  return to;
}

std::filesystem::path ddio_GetBasePath() {
  const char *exe_path = SDL_GetBasePath();
  if (!exe_path) {
    LOG_ERROR << "Failed to get parent path of executable!";
    return {};
  }
  std::filesystem::path result = std::filesystem::canonical(exe_path);
  return result;
}

#if defined(__LINUX__)
/**
 * Gets one of the user directories from the XDG Base Directory spec: $<env_var>, falling back to
 * $HOME/<home_default>. Unset, empty or relative values are ignored, as the spec requires.
 * @return the directory, or an empty path if neither variable is usable
 */
static std::filesystem::path GetXDGUserDir(const char *env_var, const char *home_default) {
  const char *value = SDL_getenv(env_var);
  if (value && std::filesystem::path(value).is_absolute()) {
    return value;
  }
  const char *home = SDL_getenv("HOME");
  if (home && std::filesystem::path(home).is_absolute()) {
    return std::filesystem::path(home) / home_default;
  }
  return {};
}
#endif

std::filesystem::path ddio_GetConfigPath() {
#if defined(__LINUX__)
  std::filesystem::path config_home = GetXDGUserDir("XDG_CONFIG_HOME", ".config");
  if (!config_home.empty() && ddio_CreateDirectories(config_home / D3_SUBDIR)) {
    return config_home / D3_SUBDIR;
  }
#endif
  return ddio_GetPrefPath();
}

std::vector<std::filesystem::path> ddio_GetSystemDataPaths() {
  std::vector<std::filesystem::path> result;
#if defined(__LINUX__)
  // Unset or empty $XDG_DATA_DIRS means the spec's default; relative entries are ignored, as the spec requires
  const char *value = SDL_getenv("XDG_DATA_DIRS");
  std::string data_dirs = (value && *value) ? value : "/usr/local/share/:/usr/share/";
  size_t start = 0;
  while (start <= data_dirs.size()) {
    size_t end = data_dirs.find(':', start);
    if (end == std::string::npos) {
      end = data_dirs.size();
    }
    std::filesystem::path dir = data_dirs.substr(start, end - start);
    if (dir.is_absolute()) {
      result.push_back(dir / D3_SUBDIR);
    }
    start = end + 1;
  }
#endif
  return result;
}

std::filesystem::path ddio_GetStatePath() {
#if defined(__LINUX__)
  std::filesystem::path state_home = GetXDGUserDir("XDG_STATE_HOME", ".local/state");
  if (!state_home.empty() && ddio_CreateDirectories(state_home / D3_SUBDIR)) {
    return state_home / D3_SUBDIR;
  }
#endif
  std::error_code ec;
  return std::filesystem::current_path(ec);
}

std::filesystem::path ddio_GetTempPath() {
#if defined(__LINUX__)
  std::filesystem::path cache_home = GetXDGUserDir("XDG_CACHE_HOME", ".cache");
  if (!cache_home.empty()) {
    return cache_home / D3_SUBDIR;
  }
#endif

  std::error_code ec;
  std::filesystem::path temp_path = std::filesystem::temp_directory_path(ec);
  if (ec) {
    LOG_ERROR << "Could not find temporary directory: " << ec.message();
    return {};
  }
  return temp_path / "Descent3" / "cache";
}
