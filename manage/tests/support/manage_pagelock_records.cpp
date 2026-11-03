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

#include "manage_pagelock_records.h"

#include <gtest/gtest.h>

#include "cfile.h"

namespace manage_pagelock {

void WriteLockFile(const std::vector<mngs_Pagelock> &locks) {
  CFILE *out = cfopen(TableLockFilename, "wb");
  ASSERT_NE(out, nullptr);
  for (auto pl : locks)
    mng_WritePagelock(out, &pl);
  cfclose(out);
}

std::vector<LockRecord> ReadLockFile() {
  std::vector<LockRecord> result;
  CFILE *in = cfopen(TableLockFilename, "rb");
  EXPECT_NE(in, nullptr) << "lock file is missing";
  if (!in)
    return result;
  mngs_Pagelock pl;
  while (mng_ReadPagelock(in, &pl))
    result.push_back({pl.pagetype, pl.name, pl.holder});
  cfclose(in);
  return result;
}

} // namespace manage_pagelock
