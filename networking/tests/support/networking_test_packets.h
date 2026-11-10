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
#include <string>
#include <vector>

#include "networking.h"

// Returns the bytes of s, without a terminator.
std::vector<uint8_t> Bytes(const std::string &s);

network_address MakePeer(uint8_t last_octet, uint16_t port);

// Builds a reliable-UDP datagram as a peer puts it on the wire (after the NWT_RELIABLE id byte, which
// nw_DoReceiveCallbacks strips): a packed little-endian header of
//   uint8 type, uint8 compressed, uint16 seq, uint16 data_len, float send_time
// followed by data_len bytes of payload.
std::vector<uint8_t> ReliablePacket(uint8_t type, uint16_t seq, const std::vector<uint8_t> &payload = {});
