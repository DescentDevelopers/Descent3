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

#include "lock_file.h"

#include "d3_test/byte_writer.h"
#include "d3_test/files.h"

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <tlhelp32.h>
#else
#include <unistd.h>
#endif

namespace fs = std::filesystem;

void WriteLockFile(const fs::path &dir, int32_t pid) {
  d3_test::WriteFile(dir / ".lock", d3_test::ByteWriter().Text("LOCK").I32(pid));
}

int ParentPid() {
#if defined(_WIN32)
  int parent = 0;
  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (snapshot == INVALID_HANDLE_VALUE) {
    return 0;
  }
  PROCESSENTRY32 entry{};
  entry.dwSize = sizeof(entry);
  for (BOOL ok = Process32First(snapshot, &entry); ok; ok = Process32Next(snapshot, &entry)) {
    if (entry.th32ProcessID == GetCurrentProcessId()) {
      parent = static_cast<int>(entry.th32ParentProcessID);
      break;
    }
  }
  CloseHandle(snapshot);
  return parent;
#else
  return static_cast<int>(getppid());
#endif
}
