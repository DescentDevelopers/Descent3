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

// Networking is never initialised, so TCP_active stays false and every outgoing send (ACKs, heartbeats, disconnects)
// returns early without touching the network. Incoming datagrams are injected by calling the receive callbacks that
// nw_DoReceiveCallbacks() would normally dispatch to.

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "d3_test/game_clock.h"
#include "networking.h"
#include "support/networking_test_packets.h"
#include "support/networking_test_reliable.h"
#include "support/networking_test_unreliable.h"

namespace {

// The algorithm is Fletcher-16 (both running sums modulo 255), returned with the simple sum in the high byte and the
// sum-of-sums in the low byte, i.e. byte-swapped relative to the usual Fletcher-16 presentation. Expected values are
// the published Fletcher-16 test vectors ("abcde" -> 0xC8F0, "abcdef" -> 0x2057, "abcdefgh" -> 0x0627) with their
// two bytes swapped.
TEST(NetworkingChecksum, MatchesFletcher16ReferenceVectors) {
  auto checksum = [](std::string s) { return nw_CalculateChecksum(s.data(), static_cast<int>(s.size())); };
  EXPECT_EQ(checksum(""), 0x0000);
  EXPECT_EQ(checksum("abcde"), 0xF0C8);
  EXPECT_EQ(checksum("abcdef"), 0x5720);
  EXPECT_EQ(checksum("abcdefgh"), 0x2706);
}

class UnreliableQueueTest : public ::testing::Test {
protected:
  void SetUp() override {
    d3_test::EnsureChronoTimer();
    nw_psnet_buffer_init();
  }
  void TearDown() override { nw_psnet_buffer_init(); }
};

TEST_F(UnreliableQueueTest, EmptyQueueReceivesNothing) { EXPECT_EQ(Receive().len, 0); }

TEST_F(UnreliableQueueTest, DeliversPacketsInArrivalOrderWithSenderAndLength) {
  network_address alice = MakePeer(1, 2092);
  network_address bob = MakePeer(2, 3000);
  Arrive(Bytes("first"), alice);
  Arrive(Bytes("2"), bob);
  Arrive(Bytes("the third packet"), alice);

  Received r = Receive();
  EXPECT_EQ(r.len, 5);
  EXPECT_EQ(r.data, Bytes("first"));
  EXPECT_EQ(0, std::memcmp(&r.from, &alice, sizeof(alice)));

  r = Receive();
  EXPECT_EQ(r.len, 1);
  EXPECT_EQ(r.data, Bytes("2"));
  EXPECT_EQ(0, std::memcmp(&r.from, &bob, sizeof(bob)));

  r = Receive();
  EXPECT_EQ(r.len, 16);
  EXPECT_EQ(r.data, Bytes("the third packet"));
  EXPECT_EQ(0, std::memcmp(&r.from, &alice, sizeof(alice)));

  EXPECT_EQ(Receive().len, 0);
}

// Interleaving arrivals and reads makes later packets land in storage freed by earlier ones.
TEST_F(UnreliableQueueTest, InterleavedArrivalsAndReadsKeepArrivalOrder) {
  network_address peer = MakePeer(3, 2092);
  int next_sent = 0;
  int next_expected = 0;
  for (int round = 0; round < 20; round++) {
    for (int k = 0; k < 3; k++)
      Arrive(PacketPayload(next_sent++), peer);
    for (int k = 0; k < 2; k++) {
      Received r = Receive();
      ASSERT_GT(r.len, 0);
      EXPECT_EQ(r.data, PacketPayload(next_expected));
      next_expected++;
    }
  }
  for (Received r = Receive(); r.len > 0; r = Receive()) {
    EXPECT_EQ(r.data, PacketPayload(next_expected));
    next_expected++;
  }
  EXPECT_EQ(next_expected, next_sent);
}

TEST_F(UnreliableQueueTest, OverflowDropsPacketsButKeepsOrderAndRecovers) {
  network_address peer = MakePeer(4, 2092);
  const int flood = 1000;
  for (int n = 0; n < flood; n++)
    Arrive(PacketPayload(n), peer);

  int delivered = 0;
  int last = -1;
  for (Received r = Receive(); r.len > 0; r = Receive()) {
    int n = PacketNumber(r);
    ASSERT_GT(n, last) << "packets delivered out of order";
    ASSERT_LT(n, flood);
    EXPECT_EQ(r.data, PacketPayload(n));
    last = n;
    delivered++;
    ASSERT_LE(delivered, flood);
  }
  EXPECT_GT(delivered, 0);

  Arrive(PacketPayload(flood), peer);
  Received r = Receive();
  EXPECT_EQ(r.data, PacketPayload(flood));
  EXPECT_EQ(Receive().len, 0);
}

class ReliableServerTest : public ::testing::Test {
protected:
  void SetUp() override {
    d3_test::EnsureChronoTimer();
    CloseAllReliableSockets();
  }
  void TearDown() override { CloseAllReliableSockets(); }
};

TEST_F(ReliableServerTest, HandshakeYieldsOneConnectionToThePeer) {
  network_address peer = MakePeer(10, 4321);
  network_address from;

  EXPECT_TRUE(IsNoConnection(nw_CheckListenSocket(&from)));

  Deliver(peer, ReliablePacket(RNT_REQ_CONN, 0));
  EXPECT_TRUE(IsNoConnection(nw_CheckListenSocket(&from)));

  Deliver(peer, ReliablePacket(RNT_I_AM_HERE, 0));
  std::memset(&from, 0, sizeof(from));
  int id = nw_CheckListenSocket(&from);
  ASSERT_FALSE(IsNoConnection(id));
  ASSERT_NE(id, 0);
  EXPECT_TRUE(nw_CheckReliableSocket(id));
  EXPECT_EQ(from.connection_type, NP_TCP);
  EXPECT_EQ(from.port, peer.port);
  EXPECT_EQ(0, std::memcmp(from.address, peer.address, 4));

  EXPECT_TRUE(IsNoConnection(nw_CheckListenSocket(&from)));
}

TEST_F(ReliableServerTest, OutOfOrderDataIsDeliveredInSequence) {
  network_address peer = MakePeer(11, 4321);
  SOCKET sock = Connect(peer);

  SendData(peer, 2);
  SendData(peer, 1);
  EXPECT_TRUE(Receive(sock).empty()) << "nothing may be delivered while seq 0 is missing";

  SendData(peer, 0);
  EXPECT_EQ(Receive(sock), DataPayload(0));
  EXPECT_EQ(Receive(sock), DataPayload(1));
  EXPECT_EQ(Receive(sock), DataPayload(2));
  EXPECT_TRUE(Receive(sock).empty());
}

// A retransmission that gets stored anyway occupies one of the connection's MAXNETBUFFERS receive buffers that nothing
// ever frees, and once they are all taken, genuine packets are dropped (yet still ACKed, so they are lost for good).
// The tests below therefore send at least MAXNETBUFFERS retransmissions, then check that new data still gets through.
TEST_F(ReliableServerTest, RetransmissionsOfUnreadDataAreDiscarded) {
  network_address peer = MakePeer(12, 4321);
  SOCKET sock = Connect(peer);

  for (int i = 0; i < MAXNETBUFFERS; i++)
    SendData(peer, 1);
  EXPECT_TRUE(Receive(sock).empty()) << "nothing may be delivered while seq 0 is missing";

  SendData(peer, 0);
  EXPECT_EQ(Receive(sock), DataPayload(0));
  EXPECT_EQ(Receive(sock), DataPayload(1));
  EXPECT_TRUE(Receive(sock).empty()) << "seq 1 must be delivered only once";
}

TEST_F(ReliableServerTest, RetransmissionsOfAlreadyReadDataAreDiscarded) {
  network_address peer = MakePeer(18, 4321);
  SOCKET sock = Connect(peer);

  const int already_read = MAXNETBUFFERS;
  for (int seq = 0; seq < already_read; seq++) {
    SendData(peer, static_cast<uint16_t>(seq));
    ASSERT_EQ(Receive(sock), DataPayload(seq));
  }

  for (int round = 0; round < 2; round++)
    for (int seq = 0; seq < already_read; seq++)
      SendData(peer, static_cast<uint16_t>(seq));
  EXPECT_TRUE(Receive(sock).empty()) << "already-read data must not be delivered again";

  // A burst of new packets arriving in reverse order has to be buffered until the first of them shows up.
  const int burst = 10;
  for (int k = burst - 1; k >= 0; k--)
    SendData(peer, static_cast<uint16_t>(already_read + k));
  for (int k = 0; k < burst; k++)
    EXPECT_EQ(Receive(sock), DataPayload(already_read + k)) << "new packet " << k << " of the burst was lost";
  EXPECT_TRUE(Receive(sock).empty());
}

TEST_F(ReliableServerTest, DataIsDeliveredOnlyToTheSendersConnection) {
  network_address alice = MakePeer(13, 4321);
  network_address bob = MakePeer(14, 4321);
  SOCKET alice_sock = Connect(alice);
  SOCKET bob_sock = Connect(bob);
  ASSERT_NE(alice_sock, bob_sock);

  Deliver(alice, ReliablePacket(RNT_DATA, 0, Bytes("from alice")));
  Deliver(bob, ReliablePacket(RNT_DATA, 0, Bytes("from bob")));
  EXPECT_EQ(Receive(bob_sock), Bytes("from bob"));
  EXPECT_EQ(Receive(alice_sock), Bytes("from alice"));
}

} // namespace
