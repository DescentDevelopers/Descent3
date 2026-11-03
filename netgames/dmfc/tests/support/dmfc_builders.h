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

#ifndef DMFC_BUILDERS_H
#define DMFC_BUILDERS_H

#include <cstdint>

#include "networking.h"
#include "gamedll_header.h"
#include "DMFC.h"
#include "dmfcinternal.h"

namespace dmfc_test {

network_address MakeAddress(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint16_t port);

char *HeapString(const char *s);

// Puts player record `slot` in `state` with `callsign`, and returns it.
player_record *OccupyRecord(int slot, slot_state state, const char *callsign);

} // namespace dmfc_test

#endif
