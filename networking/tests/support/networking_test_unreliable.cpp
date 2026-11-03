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

#include "networking_test_unreliable.h"

#include <cstring>

void Arrive(std::vector<uint8_t> payload, network_address from) {
  nw_HandleUnreliableData(payload.data(), static_cast<int>(payload.size()), &from);
}

Received Receive() {
  Received r;
  // Generously sized: nw_Receive has no length parameter.
  std::vector<uint8_t> buf(4096, 0xCC);
  std::memset(&r.from, 0, sizeof(r.from));
  r.len = nw_Receive(buf.data(), &r.from);
  if (r.len > 0 && r.len <= static_cast<int>(buf.size()))
    r.data.assign(buf.begin(), buf.begin() + r.len);
  return r;
}

std::vector<uint8_t> PacketPayload(int n) {
  std::vector<uint8_t> v(4 + n % 13, static_cast<uint8_t>(0xA0 + n % 16));
  std::memcpy(v.data(), &n, sizeof(n));
  return v;
}

int PacketNumber(const Received &r) {
  int n = -1;
  if (r.data.size() >= sizeof(n))
    std::memcpy(&n, r.data.data(), sizeof(n));
  return n;
}
