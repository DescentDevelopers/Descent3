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

#include "chat_module_fakes.h"

#include <cstdint>
#include <cstdlib>

#include "chat_api.h"
#include "networking.h"

typedef void *(*mem_malloc_fp)(int size);
typedef int (*nw_Asyncgethostbyname_fp)(uint32_t *ip, int command, const char *hostname);
typedef int (*PollUI_fp)();
typedef void (*Debug_ConsolePrintf_fp)(int n, const char *format, ...);
extern mem_malloc_fp DLLmem_malloc;
extern nw_Asyncgethostbyname_fp DLLnw_Asyncgethostbyname;
extern PollUI_fp DLLPollUI;
extern Debug_ConsolePrintf_fp DLLDebug_ConsolePrintf;

namespace {

void *TestMalloc(int size) { return std::malloc(size); }
void TestFree(void *p) { std::free(p); }

chat_test::FakeNameLookup *name_lookup = nullptr;

int FakeAsyncgethostbyname(uint32_t *ip, int command, const char *hostname) {
  name_lookup->calls.push_back({command, hostname ? hostname : ""});
  if (command != NW_AGHBN_READ)
    return 1;
  if (name_lookup->pending_reads > 0) {
    name_lookup->pending_reads--;
    return 0;
  }
  if (name_lookup->result == 1)
    *ip = name_lookup->ip;
  return name_lookup->result;
}

int FakePollUI() {
  name_lookup->poll_ui_calls++;
  return name_lookup->poll_ui_result;
}

} // namespace

namespace chat_test {

void InstallModuleApi() {
  DLLmem_malloc = TestMalloc;
  DLLmem_free = TestFree;
  DLLnw_Asyncgethostbyname = nullptr;
  DLLPollUI = nullptr;
  DLLDebug_ConsolePrintf = nullptr;
}

void InstallNameLookup(FakeNameLookup *fake) {
  name_lookup = fake;
  DLLnw_Asyncgethostbyname = fake ? FakeAsyncgethostbyname : nullptr;
  DLLPollUI = fake ? FakePollUI : nullptr;
}

} // namespace chat_test

// Each online module defines this with the texts from its own string table.
const char *GetChatModuleString(ChatString id) {
  switch (id) {
  case ChatString::ErrorNotOnline:
    return TEST_FMT_NOT_ONLINE;
  case ChatString::BadNick:
    return TEST_BAD_NICK;
  case ChatString::PrivMsgTo:
    return TEST_FMT_PRIVMSG_TO;
  case ChatString::PrivMsgFrom:
    return TEST_FMT_PRIVMSG_FROM;
  }
  return "";
}
