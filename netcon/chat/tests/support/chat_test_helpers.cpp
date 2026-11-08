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

#include <cstring>

#include "networking.h"
#include "chat_api.h"
#include "chat_test_helpers.h"
#include "chat_module_fakes.h"

namespace chat_test {

void ResetChat() {
  InstallModuleApi();
  if (User_list) {
    DLLmem_free(User_list);
    User_list = nullptr;
  }
  if (Chan_list) {
    DLLmem_free(Chan_list);
    Chan_list = nullptr;
  }
  RemoveAllChatUsers();
  FlushChannelList();
  FlushChatCommandQueue();
  ChatInit();
  Chatsock = INVALID_SOCKET;
}

const char *Feed(const std::string &line) {
  static std::string buf;
  buf = line;
  return ParseIRCMessage(buf.data(), MSG_REMOTE);
}

std::string Str(const char *p) { return p ? std::string(p) : std::string("<null>"); }

void Welcome(const char *nick) {
  Feed(std::string(":irc.test 376 ") + nick + " :End of /MOTD command.");
  FlushChatCommandQueue();
}

void Names(const char *names) {
  Feed(std::string(":irc.test 353 Pilot = #descent :") + names);
  FlushChatCommandQueue();
}

std::vector<Command> DrainCommands() {
  std::vector<Command> out;
  while (Chat_command *c = GetChatCommandFromQueue()) {
    std::string data;
    if (c->command != CC_KICKED && c->command != CC_DISCONNECTED)
      data.assign(c->data, strnlen(c->data, sizeof(c->data)));
    out.emplace_back(c->command, data);
  }
  return out;
}

std::vector<std::string> Users() {
  ConnectedScope connected;
  std::vector<std::string> out;
  const char *list = GetChatUserList();
  if (!list)
    return out;
  std::string s(list);
  size_t pos = 0;
  while (pos < s.size()) {
    size_t end = s.find(' ', pos);
    if (end == std::string::npos)
      end = s.size();
    if (end > pos)
      out.push_back(s.substr(pos, end - pos));
    pos = end + 1;
  }
  return out;
}

std::vector<ChannelEntry> ParseChannelList(const char *list) {
  std::vector<ChannelEntry> out;
  std::string s(list);
  size_t pos = s.find('$');
  while (pos != std::string::npos) {
    size_t next = s.find('$', pos + 1);
    std::string entry = s.substr(pos + 1, next == std::string::npos ? std::string::npos : next - pos - 1);
    size_t sp1 = entry.find(' ');
    size_t sp2 = entry.find(' ', sp1 + 1);
    std::string topic = entry.substr(sp2 + 1);
    while (!topic.empty() && topic.back() == ' ')
      topic.pop_back();
    out.emplace_back(entry.substr(0, sp1), std::stoi(entry.substr(sp1 + 1, sp2 - sp1 - 1)), topic);
    pos = next;
  }
  return out;
}

} // namespace chat_test
