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

// One datagram read back from the unreliable queue by Receive().
struct Received {
  int len;
  std::vector<uint8_t> data;
  network_address from;
};

// Injects payload into the unreliable queue as if it had arrived from `from`.
void Arrive(std::vector<uint8_t> payload, network_address from);

// Reads the next datagram from the unreliable queue; len is 0 when the queue is empty.
Received Receive();

// Builds the payload of unreliable packet n: n itself followed by filler whose length and value depend on n.
std::vector<uint8_t> PacketPayload(int n);

// Returns the packet number PacketPayload() stored in r, or -1 if r is too short to hold one.
int PacketNumber(const Received &r);
