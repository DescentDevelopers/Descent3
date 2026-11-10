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

#include <cstdlib>
#include <cstring>

#include "dmfc_builders.h"

namespace dmfc_test {

network_address MakeAddress(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint16_t port) {
  network_address addr{};
  addr.address[0] = a;
  addr.address[1] = b;
  addr.address[2] = c;
  addr.address[3] = d;
  addr.port = port;
  addr.connection_type = NP_TCP;
  return addr;
}

char *HeapString(const char *s) {
  char *p = static_cast<char *>(malloc(strlen(s) + 1));
  strcpy(p, s);
  return p;
}

player_record *OccupyRecord(int slot, slot_state state, const char *callsign) {
  player_record *pr = PRec_GetPRecord(slot);
  pr->state = state;
  strcpy(pr->callsign, callsign);
  return pr;
}

} // namespace dmfc_test
