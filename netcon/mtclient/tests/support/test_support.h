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

namespace mt_test {

struct SentPacket {
  uint8_t id;
  std::vector<uint8_t> bytes; // exactly the `len` bytes handed to nw_SendWithID
  network_address to;
};

extern float now;
// Value returned by DLLPollUI(); 99 means "the user cancelled".
extern int poll_ui_result;
extern std::vector<SentPacket> sent;

// Resets the fakes and points the module API at them.
void Reset();

// Copies a value out of a #pragma pack(1) struct. GCC refuses to bind packed
// fields to the const references that EXPECT_EQ takes.
template <typename T> T Unpacked(T v) { return v; }

// Advances `now` by `seconds` in quarter-second steps, calling poll() after each step.
template <typename PollFn> void AdvanceAndPoll(float seconds, PollFn poll) {
  const float step = 0.25f;
  for (float t = 0; t < seconds; t += step) {
    now += step;
    poll();
  }
}

} // namespace mt_test

// The pilot tracker header needs these definitions (and vmt_descent3_struct)
// before it is included; the production .cpp files define them the same way.
#define LOGIN_LEN 33
#define REAL_NAME_LEN 66
#define PASSWORD_LEN 17
#define EMAIL_LEN 100
#define TRACKER_ID_LEN 10
#define PILOT_NAME_LEN 20

#ifdef WIN32
#pragma pack(push, pxo)
#endif
#pragma pack(1)
struct vmt_descent3_struct {
  char tracker_id[TRACKER_ID_LEN];
  char pilot_name[PILOT_NAME_LEN];
  int rank;

  int kills;
  int deaths;
  int suicides;
  int online_time;
  int games_played;
  uint32_t security;
  uint8_t virgin_pilot;
  uint32_t lateral_thrust;
  uint32_t rotational_thrust;
  uint32_t sliding_pct;
  uint32_t checksum;
  uint32_t pad;
};
#define DESCENT3_BLOCK_SIZE (sizeof(vmt_descent3_struct) - 4)
#ifdef WIN32
#pragma pack(pop, pxo)
#else
#pragma pack()
#endif
