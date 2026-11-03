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
#include <vector>

#include "test_support.h"
#include "mtgametrack.h"

namespace mt_test {

game_packet_header MakePacket(int type, const void *data = nullptr, size_t data_len = 0);

void Deliver(game_packet_header p, int len);

void Deliver(game_packet_header p);

struct Sent {
  game_packet_header header;
  size_t len;
  network_address to;
};

std::vector<Sent> SentToTracker(int type);

game_list MakeGameList(uint32_t first_server, uint16_t first_port);

d3_net_game_data_tiny MakeTinyGame(const char *lobby);

} // namespace mt_test
