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

#include <algorithm>
#include <cstring>

#include "gametrack_packets.h"
#include "mt_net.h"

namespace mt_test {

game_packet_header MakePacket(int type, const void *data, size_t data_len) {
  game_packet_header p;
  memset(&p, 0, sizeof(p));
  p.game_type = GT_D3TNG;
  p.type = type;
  p.len = static_cast<uint32_t>(GAME_HEADER_ONLY_SIZE + data_len);
  if (data)
    memcpy(p.data, data, data_len);
  return p;
}

void Deliver(game_packet_header p, int len) {
  network_address from{};
  HandleGamePacket(reinterpret_cast<uint8_t *>(&p), len, &from);
}

void Deliver(game_packet_header p) { Deliver(p, static_cast<int>(p.len)); }

std::vector<Sent> SentToTracker(int type) {
  std::vector<Sent> out;
  for (const auto &s : mt_test::sent) {
    if (s.id != PXO_NETID_GAME_TRACKER)
      continue;
    Sent p;
    memset(&p.header, 0, sizeof(p.header));
    memcpy(&p.header, s.bytes.data(), std::min(s.bytes.size(), sizeof(p.header)));
    p.len = s.bytes.size();
    p.to = s.to;
    if (p.header.type == type)
      out.push_back(p);
  }
  return out;
}

game_list MakeGameList(uint32_t first_server, uint16_t first_port) {
  game_list gl;
  memset(&gl, 0, sizeof(gl));
  gl.game_type = GT_D3TNG;
  gl.game_server[0] = first_server;
  gl.game_port[0] = first_port;
  return gl;
}

d3_net_game_data_tiny MakeTinyGame(const char *lobby) {
  d3_net_game_data_tiny g;
  memset(&g, 0, sizeof(g));
  strncpy(g.lobby, lobby, sizeof(g.lobby) - 1);
  return g;
}

} // namespace mt_test
