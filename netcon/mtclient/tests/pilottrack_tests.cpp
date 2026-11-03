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
#include <string>

#include <gtest/gtest.h>

#include "support/pilottrack_packets.h"
#include "support/test_support.h"
#include "mt_net.h"
#include "mtpilottrack.h"

namespace {

using namespace mt_test;

class PilotTracker : public ::testing::Test {
protected:
  void SetUp() override {
    mt_test::Reset();
    InitPilotTrackerClient();
    GetD3MOTDCancel(); // MOTD state is not reset by InitPilotTrackerClient
    mt_test::sent.clear();
  }
};

TEST_F(PilotTracker, MotdIsDeliveredIntoCallersBuffer) {
  char motd[128];
  memset(motd, 'x', sizeof(motd));
  EXPECT_EQ(GetD3MOTD(motd, sizeof(motd)), 0);
  EXPECT_EQ(GetD3MOTD(nullptr, 0), 0) << "still waiting";

  Deliver(MakeStringPacket(UNT_MOTD_RESPONSE, "Welcome to PXO!"));

  EXPECT_EQ(GetD3MOTD(nullptr, 0), 1);
  EXPECT_STREQ(motd, "Welcome to PXO!");
  EXPECT_EQ(GetD3MOTD(nullptr, 0), -3) << "result is reported once";
}

TEST_F(PilotTracker, LongMotdIsTruncatedToFitTheBuffer) {
  struct {
    char motd[16];
    char guard[16];
  } buf;
  memset(&buf, 'x', sizeof(buf));
  const std::string message = "This message of the day is much longer than sixteen characters";

  GetD3MOTD(buf.motd, sizeof(buf.motd));
  Deliver(MakeStringPacket(UNT_MOTD_RESPONSE, message));
  ASSERT_EQ(GetD3MOTD(nullptr, 0), 1);

  size_t len = strnlen(buf.motd, sizeof(buf.motd));
  ASSERT_LT(len, sizeof(buf.motd)) << "MOTD must be NUL-terminated inside the buffer";
  EXPECT_EQ(message.compare(0, len, buf.motd), 0) << "truncated MOTD should be a prefix of the message";
  EXPECT_GT(len, 0u);
  for (char c : buf.guard)
    EXPECT_EQ(c, 'x') << "wrote past the end of the MOTD buffer";
}

TEST_F(PilotTracker, MotdRequestIsRetransmittedUntilAnswered) {
  char motd[64];
  GetD3MOTD(motd, sizeof(motd));
  AdvanceAndPoll(PILOT_REQ_RESEND_TIME * 3 + 0.5f, [] { GetD3MOTD(nullptr, 0); });
  EXPECT_GE(CountSent(UNT_MOTD_REQUEST), 2);

  Deliver(MakeStringPacket(UNT_MOTD_RESPONSE, "hi"));
  GetD3MOTD(nullptr, 0);
  mt_test::sent.clear();
  AdvanceAndPoll(PILOT_REQ_RESEND_TIME * 3, [] { GetD3MOTD(nullptr, 0); });
  EXPECT_EQ(CountSent(UNT_MOTD_REQUEST), 0) << "no retransmits once answered";
}

TEST_F(PilotTracker, MotdTimesOut) {
  char motd[64];
  GetD3MOTD(motd, sizeof(motd));
  int result = 0;
  AdvanceAndPoll(PILOT_REQ_TIMEOUT + 1, [&] {
    if (result == 0)
      result = GetD3MOTD(nullptr, 0);
  });
  EXPECT_EQ(result, -1);
  EXPECT_EQ(GetD3MOTD(nullptr, 0), -3) << "back to idle after reporting the timeout";
}

TEST_F(PilotTracker, OnlyOneMotdRequestAtATime) {
  char a[32], b[32];
  EXPECT_EQ(GetD3MOTD(a, sizeof(a)), 0);
  EXPECT_EQ(GetD3MOTD(b, sizeof(b)), -2);
  EXPECT_EQ(GetD3MOTDCancel(), 2);
  EXPECT_EQ(GetD3MOTDCancel(), -3);
  EXPECT_EQ(GetD3MOTD(b, sizeof(b)), 0) << "a new request is accepted after cancelling";
}

TEST_F(PilotTracker, MotdArrivingAfterCancelIsIgnored) {
  char motd[32] = "untouched";
  GetD3MOTD(motd, sizeof(motd));
  GetD3MOTDCancel();
  Deliver(MakeStringPacket(UNT_MOTD_RESPONSE, "late reply"));
  EXPECT_STREQ(motd, "untouched");
  EXPECT_EQ(GetD3MOTD(nullptr, 0), -3);
}

TEST_F(PilotTracker, PilotDataIsReadForTheRequestedPilot) {
  vmt_descent3_struct out;
  memset(&out, 0, sizeof(out));
  char name[] = "Hunter";
  char tid[] = "1234567";
  EXPECT_EQ(GetD3PilotData(&out, name, tid), 0);
  EXPECT_EQ(GetD3PilotData(nullptr, nullptr, nullptr), 0);

  // The tracker may return the name in a different case.
  vmt_descent3_struct reply = MakePilot("hunter", "1234567");
  reply.kills = 42;
  reply.deaths = 7;
  Deliver(MakePacket(UNT_PILOT_DATA_RESPONSE, CMD_GAME_DESCENT3, &reply, DESCENT3_BLOCK_SIZE));

  EXPECT_EQ(GetD3PilotData(nullptr, nullptr, nullptr), 1);
  EXPECT_EQ(mt_test::Unpacked(out.kills), 42);
  EXPECT_EQ(mt_test::Unpacked(out.deaths), 7);
  EXPECT_STREQ(out.tracker_id, "1234567");
  EXPECT_EQ(GetD3PilotData(nullptr, nullptr, nullptr), -3) << "back to idle";
}

TEST_F(PilotTracker, PilotDataForAnotherPilotIsIgnored) {
  vmt_descent3_struct out;
  memset(&out, 0, sizeof(out));
  char name[] = "Hunter";
  char tid[] = "1234567";
  GetD3PilotData(&out, name, tid);

  vmt_descent3_struct reply = MakePilot("SomeoneElse", "7654321");
  reply.kills = 99;
  Deliver(MakePacket(UNT_PILOT_DATA_RESPONSE, CMD_GAME_DESCENT3, &reply, DESCENT3_BLOCK_SIZE));

  EXPECT_EQ(GetD3PilotData(nullptr, nullptr, nullptr), 0) << "still waiting for our pilot";
  EXPECT_EQ(mt_test::Unpacked(out.kills), 0);
}

TEST_F(PilotTracker, PilotReadRequestIsSentWithNameAndTrackerId) {
  vmt_descent3_struct out;
  char name[] = "Hunter";
  char tid[] = "1234567";
  GetD3PilotData(&out, name, tid);
  AdvanceAndPoll(PILOT_REQ_RESEND_TIME + 0.5f, [] { GetD3PilotData(nullptr, nullptr, nullptr); });

  bool found = false;
  for (const auto &p : SentToTracker()) {
    if (p.type != UNT_PILOT_DATA_READ)
      continue;
    found = true;
    pilot_request req;
    memcpy(&req, p.data, sizeof(req));
    EXPECT_STREQ(req.pilot_name, "Hunter");
    EXPECT_STREQ(req.tracker_id, "1234567");
  }
  EXPECT_TRUE(found);
}

TEST_F(PilotTracker, PilotNotFound) {
  vmt_descent3_struct out;
  char name[] = "Nobody";
  char tid[] = "0000000";
  GetD3PilotData(&out, name, tid);
  Deliver(MakePacket(UNT_PILOT_READ_FAILED, CMD_GAME_DESCENT3));
  EXPECT_EQ(GetD3PilotData(nullptr, nullptr, nullptr), 3);
  EXPECT_EQ(GetD3PilotData(nullptr, nullptr, nullptr), -3);
}

TEST_F(PilotTracker, PilotReadTimesOut) {
  vmt_descent3_struct out;
  char name[] = "Hunter";
  char tid[] = "1234567";
  GetD3PilotData(&out, name, tid);
  int result = 0;
  AdvanceAndPoll(PILOT_REQ_TIMEOUT + 1, [&] {
    if (result == 0)
      result = GetD3PilotData(nullptr, nullptr, nullptr);
  });
  EXPECT_EQ(result, -1);
}

TEST_F(PilotTracker, PilotReadBusyAndCancel) {
  vmt_descent3_struct a, b;
  char name[] = "Hunter";
  char tid[] = "1234567";
  EXPECT_EQ(GetD3PilotDataCancel(), -3);
  EXPECT_EQ(GetD3PilotData(&a, name, tid), 0);
  EXPECT_EQ(GetD3PilotData(&b, name, tid), -2);
  EXPECT_EQ(GetD3PilotDataCancel(), 2);

  memset(&a, 0, sizeof(a));
  vmt_descent3_struct reply = MakePilot("Hunter", "1234567");
  reply.kills = 5;
  Deliver(MakePacket(UNT_PILOT_DATA_RESPONSE, CMD_GAME_DESCENT3, &reply, DESCENT3_BLOCK_SIZE));
  EXPECT_EQ(mt_test::Unpacked(a.kills), 0);
  EXPECT_EQ(GetD3PilotData(nullptr, nullptr, nullptr), -3);
}

TEST_F(PilotTracker, PilotWriteSucceeds) {
  vmt_descent3_struct p = MakePilot("Hunter", "1234567");
  EXPECT_EQ(SendD3PilotData(nullptr), -3);
  EXPECT_EQ(SendD3PilotData(&p), 0);
  EXPECT_EQ(SendD3PilotData(nullptr), 0);
  vmt_descent3_struct q = MakePilot("Other", "7654321");
  EXPECT_EQ(SendD3PilotData(&q), -2) << "only one write at a time";

  Deliver(MakePacket(UNT_PILOT_WRITE_SUCCESS, CMD_GAME_DESCENT3));
  EXPECT_EQ(SendD3PilotData(nullptr), 1);
  EXPECT_EQ(SendD3PilotData(nullptr), -3);
}

TEST_F(PilotTracker, PilotWriteRejectedByTracker) {
  vmt_descent3_struct p = MakePilot("Hunter", "1234567");
  SendD3PilotData(&p);
  Deliver(MakePacket(UNT_PILOT_WRITE_FAILED, CMD_GAME_DESCENT3));
  EXPECT_EQ(SendD3PilotData(nullptr), 3);
}

TEST_F(PilotTracker, PilotWriteTimesOut) {
  vmt_descent3_struct p = MakePilot("Hunter", "1234567");
  SendD3PilotData(&p);
  int result = 0;
  AdvanceAndPoll(PILOT_REQ_TIMEOUT + 1, [&] {
    if (result == 0)
      result = SendD3PilotData(nullptr);
  });
  EXPECT_EQ(result, -1);
}

TEST_F(PilotTracker, PilotWritePacketCarriesThePilotsStats) {
  vmt_descent3_struct p = MakePilot("Hunter", "1234567");
  p.kills = 12;
  p.deaths = 3;
  p.suicides = 1;
  SendD3PilotData(&p);
  AdvanceAndPoll(PILOT_REQ_RESEND_TIME + 0.5f, [] { SendD3PilotData(nullptr); });

  bool found = false;
  for (const auto &s : mt_test::sent) {
    udp_packet_header h;
    memset(&h, 0, sizeof(h));
    memcpy(&h, s.bytes.data(), std::min(s.bytes.size(), sizeof(h)));
    if (h.type != UNT_PILOT_DATA_WRITE)
      continue;
    found = true;
    EXPECT_GE(s.bytes.size(), PACKED_HEADER_ONLY_SIZE + DESCENT3_BLOCK_SIZE) << "stats must be inside the sent length";
    vmt_descent3_struct w;
    memset(&w, 0, sizeof(w));
    memcpy(&w, h.data, DESCENT3_BLOCK_SIZE);
    EXPECT_STREQ(w.pilot_name, "Hunter");
    EXPECT_STREQ(w.tracker_id, "1234567");
    EXPECT_EQ(mt_test::Unpacked(w.kills), 12);
    EXPECT_EQ(mt_test::Unpacked(w.deaths), 3);
    EXPECT_EQ(mt_test::Unpacked(w.suicides), 1);
  }
  EXPECT_TRUE(found);
}

class PilotTrackerLogin : public PilotTracker {
protected:
  char tracker_id[64] = "";
};

TEST_F(PilotTrackerLogin, LoginRequestIsSentImmediatelyWithCredentials) {
  StartLogin(tracker_id);
  bool found = false;
  for (const auto &p : SentToTracker()) {
    if (p.type != UNT_LOGIN_AUTH_REQUEST)
      continue;
    found = true;
    validate_id_request req;
    memcpy(&req, p.data, sizeof(req));
    EXPECT_STREQ(req.login, "player1");
    EXPECT_STREQ(req.password, "hunter2");
  }
  EXPECT_TRUE(found);
}

TEST_F(PilotTrackerLogin, AuthenticatedLoginReturnsTrackerId) {
  StartLogin(tracker_id);
  EXPECT_EQ(ValidateUser(nullptr, nullptr), 0);
  Deliver(MakeStringPacket(UNT_LOGIN_AUTHENTICATED, "8675309"));
  EXPECT_EQ(ValidateUser(nullptr, nullptr), 1);
  EXPECT_STREQ(tracker_id, "8675309");
  EXPECT_EQ(ValidateUser(nullptr, nullptr), 0) << "back to idle";
}

TEST_F(PilotTrackerLogin, RejectedLogin) {
  StartLogin(tracker_id);
  Deliver(MakePacket(UNT_LOGIN_NO_AUTH));
  EXPECT_EQ(ValidateUser(nullptr, nullptr), -1);
}

TEST_F(PilotTrackerLogin, ValidatedAckFromControlPacket) {
  StartLogin(tracker_id);
  Deliver(MakePacket(UNT_CONTROL, CMD_VALIDATED_USER_ACK));
  EXPECT_EQ(ValidateUser(nullptr, nullptr), 1);
}

TEST_F(PilotTrackerLogin, ValidatedNakFromControlPacket) {
  StartLogin(tracker_id);
  Deliver(MakePacket(UNT_CONTROL, CMD_VALIDATED_USER_NAK));
  EXPECT_EQ(ValidateUser(nullptr, nullptr), -1);
}

TEST_F(PilotTrackerLogin, LoginTimesOut) {
  StartLogin(tracker_id);
  int result = 0;
  AdvanceAndPoll(PILOT_REQ_TIMEOUT + 1, [&] {
    if (result == 0)
      result = ValidateUser(nullptr, nullptr);
  });
  EXPECT_EQ(result, -2);
}

TEST_F(PilotTrackerLogin, LoginAuthenticationWhenNotWaitingIsIgnored) {
  Deliver(MakeStringPacket(UNT_LOGIN_AUTHENTICATED, "8675309"));
  EXPECT_EQ(ValidateUser(nullptr, nullptr), 0);
}

TEST_F(PilotTracker, VersionCheckReceivesUpdateUrl) {
  char url[200] = "";
  EXPECT_EQ(MTAVersionCheck(5, url), 0);
  EXPECT_EQ(MTAVersionCheck(6, url), -3) << "already waiting";

  bool sent_req = false;
  for (const auto &p : SentToTracker())
    if (p.type == UNT_TRACKER_VERSION_REQ) {
      sent_req = true;
      EXPECT_EQ(mt_test::Unpacked(p.code), 5u) << "request carries our current version";
    }
  EXPECT_TRUE(sent_req);

  Deliver(MakeStringPacket(UNT_TRACKER_VERSION_RSP, "http://example.invalid/update.zip"));
  EXPECT_EQ(MTAVersionCheck(0, url), 1);
  EXPECT_STREQ(url, "http://example.invalid/update.zip");
  EXPECT_EQ(MTAVersionCheck(0, url), 0) << "back to idle";
}

TEST_F(PilotTracker, VersionCheckAbortedByUser) {
  char url[200] = "";
  MTAVersionCheck(5, url);
  mt_test::poll_ui_result = 99;
  EXPECT_EQ(MTAVersionCheck(0, url), -2);
}

TEST_F(PilotTracker, VersionCheckTimesOut) {
  char url[200] = "";
  MTAVersionCheck(5, url);
  int result = 0;
  AdvanceAndPoll(PILOT_REQ_TIMEOUT + 1, [&] {
    if (result == 0)
      result = MTAVersionCheck(0, url);
  });
  EXPECT_EQ(result, -2);
}

TEST_F(PilotTracker, EveryTrackerPacketIsAcknowledgedWithItsSignature) {
  udp_packet_header p = MakePacket(UNT_PILOT_WRITE_SUCCESS, CMD_GAME_DESCENT3);
  p.sig = 0x5A17C0DE;
  Deliver(p);

  auto sent = SentToTracker();
  ASSERT_EQ(sent.size(), 1u);
  EXPECT_EQ(mt_test::Unpacked(sent[0].type), UNT_CONTROL);
  EXPECT_EQ(mt_test::Unpacked(sent[0].code), static_cast<uint32_t>(CMD_CLIENT_RECEIVED));
  EXPECT_EQ(mt_test::Unpacked(sent[0].sig), 0x5A17C0DEu);
}

} // namespace
