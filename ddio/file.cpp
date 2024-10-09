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
#include <regex>

#include "IOOps.h"
#include "chrono_timer.h"
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
  size_t len_result = strlen((basedir / prefix).u8string().c_str());
  char *random_name = (char *)mem_malloc(len_result + len + strlen(ext) + 1);
  strncpy(random_name, (basedir / prefix).u8string().c_str(), len_result);

  srand(D3::ChronoTimer::GetTimeMS());

  int tries = 20;
  while (tries > 0) {
    for (size_t i = len_result; i < len_result + len; i++) {
      random_name[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
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

std::filesystem::path ddio_GetTempPath() {
  std::filesystem::path result;

#if defined(POSIX)
  char *envr = SDL_getenv("XDG_CACHE_HOME");
  if (envr) {
    result = std::filesystem::path(envr) / "Descent3";
  } else {
    envr = SDL_getenv("HOME");
    if (envr) {
      result = std::filesystem::path(envr) / ".cache" / "Descent3";
    } else {
#endif
      std::error_code ec;
      std::filesystem::path tempPath = std::filesystem::temp_directory_path(ec);
      if (ec) {
        Error("Could not find temporary directory: \"%s\"", ec.message().c_str() );
        exit(1);
      }
      result = tempPath / "Descent3" / "cache";
#if defined(POSIX)
    }
  }
#endif
  return result;
}
