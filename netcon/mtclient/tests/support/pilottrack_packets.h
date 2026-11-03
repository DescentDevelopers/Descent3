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

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "test_support.h"
#include "mtpilottrack.h"

namespace mt_test {

udp_packet_header MakePacket(uint8_t type, uint32_t code = 0, const void *data = nullptr, size_t data_len = 0);

udp_packet_header MakeStringPacket(uint8_t type, const std::string &s);

void Deliver(udp_packet_header p);

std::vector<udp_packet_header> SentToTracker();

int CountSent(uint8_t type);

vmt_descent3_struct MakePilot(const char *name, const char *tracker_id);

// Starts validating player1/hunter2; the tracker id is written to `tracker_id` once the login is authenticated.
void StartLogin(char *tracker_id);

} // namespace mt_test
