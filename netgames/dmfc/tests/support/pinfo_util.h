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

#ifndef DMFC_PINFO_UTIL_H
#define DMFC_PINFO_UTIL_H

#include <map>

#include "gamedll_header.h"
#include "DMFC.h"
#include "dmfcinternal.h"

namespace dmfc_test {

// The killer list as slot -> kills. Fails the test if a slot is listed twice.
std::map<int, int> Killers(PInfo &info);

// Records `kills` more kills of this player by `slot`.
void AddKills(PInfo &info, int slot, int kills);

} // namespace dmfc_test

#endif
