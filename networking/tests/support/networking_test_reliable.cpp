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

#include "networking_test_reliable.h"

#include <gtest/gtest.h>

#include "networking_test_packets.h"

void CloseAllReliableSockets() {
  for (int i = 0; i < MAXRELIABLESOCKETS; i++) {
    if (nw_CheckReliableSocket(i)) {
      SOCKET s = i;
      nw_CloseSocket(&s);
    }
  }
}

void Deliver(network_address from, std::vector<uint8_t> packet) {
  nw_WorkReliable(packet.data(), static_cast<int>(packet.size()), &from);
}

bool IsNoConnection(int id) { return static_cast<SOCKET>(id) == INVALID_SOCKET; }

SOCKET Connect(network_address peer) {
  Deliver(peer, ReliablePacket(RNT_REQ_CONN, 0));
  Deliver(peer, ReliablePacket(RNT_I_AM_HERE, 0));
  network_address from;
  int id = nw_CheckListenSocket(&from);
  EXPECT_FALSE(IsNoConnection(id));
  EXPECT_NE(id, 0);
  return static_cast<SOCKET>(id);
}

std::vector<uint8_t> DataPayload(int seq) {
  std::vector<uint8_t> v(3 + seq % 7, static_cast<uint8_t>(seq * 31));
  v[0] = static_cast<uint8_t>(seq & 0xFF);
  v[1] = static_cast<uint8_t>((seq >> 8) & 0xFF);
  return v;
}

void SendData(network_address peer, uint16_t seq) { Deliver(peer, ReliablePacket(RNT_DATA, seq, DataPayload(seq))); }

std::vector<uint8_t> Receive(SOCKET sock) {
  std::vector<uint8_t> buf(NETBUFFERSIZE);
  int len = nw_ReceiveReliable(sock, buf.data(), static_cast<int>(buf.size()));
  if (len <= 0)
    return {};
  buf.resize(len);
  return buf;
}
