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

// The module API (con_dll.h) as the game would hand it to the Parallax Online module, pointed at fakes the tests
// control. In the netcon test executable the API globals themselves are defined once, by con_dll.h (through
// lanclient.cpp), so Reset() assigns them before every test.

#include <cstdint>
#include <cstdlib>

#include "test_support.h"

typedef void *(*mem_malloc_fp)(int size);
typedef void (*mem_free_fp)(void *memblock);
typedef float (*timer_GetTime_fp)();
typedef int (*PollUI_fp)();
typedef int (*nw_Asyncgethostbyname_fp)(uint32_t *ip, int command, const char *hostname);
typedef int (*nw_SendWithID_fp)(uint8_t id, uint8_t *data, int len, network_address *who_to);
typedef void (*PacketHandler_fp)(uint8_t *data, int len, network_address *from);
typedef int (*nw_RegisterCallback_fp)(PacketHandler_fp, uint8_t id);
typedef int (*nw_DoReceiveCallbacks_fp)();

extern mem_malloc_fp DLLmem_malloc;
extern mem_free_fp DLLmem_free;
extern timer_GetTime_fp DLLtimer_GetTime;
extern PollUI_fp DLLPollUI;
extern bool *DLLDedicated_server;
extern nw_Asyncgethostbyname_fp DLLnw_Asyncgethostbyname;
extern nw_SendWithID_fp DLLnw_SendWithID;
extern nw_RegisterCallback_fp DLLnw_RegisterCallback;
extern nw_DoReceiveCallbacks_fp DLLnw_DoReceiveCallbacks;
extern char *DLLPXO_hosted_lobby_name;

// mtclient.cpp, which these tests do not compile, defines the port.
uint16_t DLLPXOPort = 0;

namespace {

void *StubMalloc(int size) { return std::malloc(size); }
void StubFree(void *p) { std::free(p); }
float StubGetTime() { return mt_test::now; }
int StubPollUI() { return mt_test::poll_ui_result; }
int StubAsyncgethostbyname(uint32_t *, int, const char *) { return -1; }

int StubSendWithID(uint8_t id, uint8_t *data, int len, network_address *who_to) {
  mt_test::SentPacket p;
  p.id = id;
  p.bytes.assign(data, data + len);
  p.to = *who_to;
  mt_test::sent.push_back(p);
  return len;
}

int StubRegisterCallback(PacketHandler_fp, uint8_t) { return 1; }
int StubDoReceiveCallbacks() { return 0; }

bool dedicated_server = true;
char hosted_lobby_name[] = "#test_lobby";

} // namespace

namespace mt_test {
float now = 0.0f;
int poll_ui_result = 0;
std::vector<SentPacket> sent;

void Reset() {
  now = 1000.0f;
  poll_ui_result = 0;
  sent.clear();

  DLLmem_malloc = StubMalloc;
  DLLmem_free = StubFree;
  DLLtimer_GetTime = StubGetTime;
  DLLPollUI = StubPollUI;
  DLLDedicated_server = &dedicated_server;
  DLLnw_Asyncgethostbyname = StubAsyncgethostbyname;
  DLLnw_SendWithID = StubSendWithID;
  DLLnw_RegisterCallback = StubRegisterCallback;
  DLLnw_DoReceiveCallbacks = StubDoReceiveCallbacks;
  DLLPXO_hosted_lobby_name = hosted_lobby_name;
}
} // namespace mt_test
