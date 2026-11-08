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

#include "pinfo_util.h"

#include <gtest/gtest.h>

namespace dmfc_test {

std::map<int, int> Killers(PInfo &info) {
  std::map<int, int> result;
  for (tPKillerInfo *k = info.GetFirstKiller(); k; k = info.GetNextKiller()) {
    EXPECT_EQ(result.count(k->slot), 0u) << "slot " << k->slot << " listed twice";
    result[k->slot] = k->kills;
  }
  return result;
}

void AddKills(PInfo &info, int slot, int kills) {
  tPKillerInfo amount{};
  amount.kills = kills;
  info.Update(slot, &amount);
}

} // namespace dmfc_test
