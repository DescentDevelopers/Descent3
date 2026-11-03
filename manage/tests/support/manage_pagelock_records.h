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

#pragma once

#include <cstdint>
#include <cstring>
#include <ostream>
#include <string>
#include <vector>

#include "manage.h"

namespace manage_pagelock {

inline mngs_Pagelock MakeLock(uint8_t pagetype, const char *name, const char *holder) {
  mngs_Pagelock pl{};
  pl.pagetype = pagetype;
  strncpy(pl.name, name, sizeof(pl.name) - 1);
  strncpy(pl.holder, holder, sizeof(pl.holder) - 1);
  return pl;
}

struct LockRecord {
  int pagetype;
  std::string name;
  std::string holder;
  bool operator==(const LockRecord &o) const {
    return pagetype == o.pagetype && name == o.name && holder == o.holder;
  }
};

inline std::ostream &operator<<(std::ostream &os, const LockRecord &r) {
  return os << "{" << r.pagetype << ", \"" << r.name << "\", \"" << r.holder << "\"}";
}

// Replaces the lock file at TableLockFilename with `locks`.
void WriteLockFile(const std::vector<mngs_Pagelock> &locks);

// Every record in the lock file at TableLockFilename, in file order.
std::vector<LockRecord> ReadLockFile();

} // namespace manage_pagelock
