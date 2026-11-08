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

#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "chat_api_internals.h"

namespace chat_test {

// Resets the chat module to its initial state: no users, channels, queued commands or socket.
void ResetChat();

// Feeds one line from the server to ParseIRCMessage and returns what it returned.
const char *Feed(const std::string &line);

// The string p points to, or "<null>".
std::string Str(const char *p);

// Has the server confirm our nickname in the end-of-MOTD line, discarding the queued commands.
void Welcome(const char *nick);

// Has the server send a #descent names reply listing names, discarding the queued commands.
void Names(const char *names);

using Command = std::pair<int, std::string>;

std::vector<Command> DrainCommands();

// Most of the public API refuses to do anything unless the socket is "connected".
class ConnectedScope {
public:
  ConnectedScope() : saved_(Socket_connected) { Socket_connected = 1; }
  ~ConnectedScope() { Socket_connected = saved_; }

private:
  int saved_;
};

std::vector<std::string> Users();

using ChannelEntry = std::tuple<std::string, int, std::string>;

std::vector<ChannelEntry> ParseChannelList(const char *list);

} // namespace chat_test
