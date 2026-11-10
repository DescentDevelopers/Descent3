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

#include "networking_test_packets.h"

#include <cstring>

std::vector<uint8_t> Bytes(const std::string &s) { return std::vector<uint8_t>(s.begin(), s.end()); }

network_address MakePeer(uint8_t last_octet, uint16_t port) {
  network_address addr;
  std::memset(&addr, 0, sizeof(addr));
  addr.address[0] = 10;
  addr.address[1] = 0;
  addr.address[2] = 0;
  addr.address[3] = last_octet;
  addr.port = port;
  addr.connection_type = NP_TCP;
  return addr;
}

std::vector<uint8_t> ReliablePacket(uint8_t type, uint16_t seq, const std::vector<uint8_t> &payload) {
  std::vector<uint8_t> p;
  p.push_back(type);
  p.push_back(0); // not compressed
  p.push_back(static_cast<uint8_t>(seq & 0xFF));
  p.push_back(static_cast<uint8_t>(seq >> 8));
  p.push_back(static_cast<uint8_t>(payload.size() & 0xFF));
  p.push_back(static_cast<uint8_t>(payload.size() >> 8));
  p.insert(p.end(), 4, 0); // send_time 0.0f
  p.insert(p.end(), payload.begin(), payload.end());
  return p;
}
