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

#include <gtest/gtest.h>

#include "pilottrack_packets.h"
#include "mt_net.h"

namespace mt_test {

udp_packet_header MakePacket(uint8_t type, uint32_t code, const void *data, size_t data_len) {
  udp_packet_header p;
  memset(&p, 0, sizeof(p));
  p.type = type;
  p.code = code;
  p.len = static_cast<uint16_t>(PACKED_HEADER_ONLY_SIZE + data_len);
  if (data)
    memcpy(p.data, data, data_len);
  return p;
}

udp_packet_header MakeStringPacket(uint8_t type, const std::string &s) {
  // String payloads are sent with their terminating NUL.
  return MakePacket(type, 0, s.c_str(), s.size() + 1);
}

void Deliver(udp_packet_header p) {
  network_address from{};
  HandlePilotData(reinterpret_cast<uint8_t *>(&p), p.len, &from);
}

std::vector<udp_packet_header> SentToTracker() {
  std::vector<udp_packet_header> out;
  for (const auto &s : mt_test::sent) {
    if (s.id != PXO_NETID_USER_TRACKER)
      continue;
    udp_packet_header p;
    memset(&p, 0, sizeof(p));
    memcpy(&p, s.bytes.data(), std::min(s.bytes.size(), sizeof(p)));
    out.push_back(p);
  }
  return out;
}

int CountSent(uint8_t type) {
  int n = 0;
  for (const auto &p : SentToTracker())
    if (p.type == type)
      n++;
  return n;
}

vmt_descent3_struct MakePilot(const char *name, const char *tracker_id) {
  vmt_descent3_struct v;
  memset(&v, 0, sizeof(v));
  strncpy(v.pilot_name, name, sizeof(v.pilot_name) - 1);
  strncpy(v.tracker_id, tracker_id, sizeof(v.tracker_id) - 1);
  return v;
}

void StartLogin(char *tracker_id) {
  validate_id_request req;
  memset(&req, 0, sizeof(req));
  strcpy(req.login, "player1");
  strcpy(req.password, "hunter2");
  ASSERT_EQ(ValidateUser(&req, tracker_id), 0);
}

} // namespace mt_test
