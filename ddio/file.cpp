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

#include <filesystem>

#include "ddio.h"
#include "mono.h"

int ddio_CheckLockFile(const std::filesystem::path& dir, int pid) {
  bool found_lock_file_in_dir;
  FILE *file;

  std::filesystem::path lockfile = dir / ".lock";
  mprintf(0, "LockFile: Checking [%s]...", lockfile.c_str());
  file = fopen(lockfile.c_str(), "rb");

  if (!file) {
    // File exists, but couldn't open it
    if (errno == EACCES) {
      return -2;
    }

    found_lock_file_in_dir = false;
  } else {
    found_lock_file_in_dir = true;

    // check the file, see if it is a lock file
    char c;
    c = fgetc(file);
    if (c != 'L') {
      fclose(file);
      return -2;
    }
    c = fgetc(file);
    if (c != 'O') {
      fclose(file);
      return -2;
    }
    c = fgetc(file);
    if (c != 'C') {
      fclose(file);
      return -2;
    }
    c = fgetc(file);
    if (c != 'K') {
      fclose(file);
      return -2;
    }

    // it appears to be a lock file, check the pid
    pid_t f_pid;
    fread(&f_pid, sizeof(pid_t), 1, file);
    fclose(file);

    // check the file id in the file, compared to our pid
    if (f_pid == pid) {
      // lock file already exists for the current process
      return 3;
    }

    if (ddio_CheckProcess(f_pid)) {
      // Process exists, lock still valid
      return 0;
    }

    // the process no longer exists, we can create a lock file if needed
    // we'll delete the useless one now
    ddio_DeleteFile(".lock");
  }

  return (found_lock_file_in_dir) ? 2 : 1;
}

int ddio_CreateLockFile(const std::filesystem::path& dir) {
  pid_t curr_pid = ddio_GetPID();

  int result = ddio_CheckLockFile(dir, curr_pid);
  switch (result) {
  case 0:
  case -1:
  case -2:
  case 3:
    return result;
  };

  std::filesystem::path lockfile = dir / ".lock";

  FILE *file;
  file = fopen(lockfile.c_str(), "wb");
  if (!file) {
    return -3;
  }

  fputc('L', file);
  fputc('O', file);
  fputc('C', file);
  fputc('K', file);
  fwrite(&curr_pid, sizeof(pid_t), 1, file);

  fclose(file);

  // at this point result will either be 1 or 2 from checking the lock file
  // either way, a lock file has been created
  return result;
}

int ddio_DeleteLockFile(const std::filesystem::path& dir) {
  pid_t curr_pid = ddio_GetPID();

  FILE *file;

  std::filesystem::path lockfile = dir / ".lock";

  file = fopen(lockfile.c_str(), "rb");

  if (!file)
    return 1;

  // check the file, see if it is a lock file
  char c;
  c = fgetc(file);
  if (c != 'L') {
    fclose(file);
    return -2;
  }
  c = fgetc(file);
  if (c != 'O') {
    fclose(file);
    return -2;
  }
  c = fgetc(file);
  if (c != 'C') {
    fclose(file);
    return -2;
  }
  c = fgetc(file);
  if (c != 'K') {
    fclose(file);
    return -2;
  }

  // it appears to be a lock file, check the pid
  pid_t f_pid;
  fread(&f_pid, sizeof(pid_t), 1, file);
  fclose(file);

  // check the file id in the file, compared to our pid
  if (f_pid != curr_pid) {
    // it doesn't belong to
    return 0;
  }

  std::error_code ec;
  // the lock file in the directory belongs to us!
  if (!std::filesystem::remove(lockfile, ec)) {
    return -3;
  }

  return 1;
}