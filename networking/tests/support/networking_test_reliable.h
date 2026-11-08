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
#include <vector>

#include "networking.h"

// Closes every open reliable socket.
void CloseAllReliableSockets();

// Hands a reliable-UDP datagram from `from` to the reliable layer.
void Deliver(network_address from, std::vector<uint8_t> packet);

// True when a socket id returned by nw_CheckListenSocket means no connection is waiting.
bool IsNoConnection(int id);

// Completes the connection handshake for peer and returns the accepted socket.
SOCKET Connect(network_address peer);

// Builds the payload of reliable data packet seq: seq in its first two bytes, then filler that depends on seq.
std::vector<uint8_t> DataPayload(int seq);

// Delivers DataPayload(seq) from peer as reliable data packet seq.
void SendData(network_address peer, uint16_t seq);

// Reads the next in-sequence data from sock; empty when none is ready.
std::vector<uint8_t> Receive(SOCKET sock);
