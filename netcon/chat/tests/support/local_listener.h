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

#include "networking.h"

namespace chat_test {

// Closes a socket the chat client or a test opened.
void CloseSocket(SOCKET s);

// A TCP socket listening on 127.0.0.1, on a port the system picks, standing in for a chat server.
class LocalListener {
public:
  LocalListener();
  ~LocalListener();
  LocalListener(const LocalListener &) = delete;
  LocalListener &operator=(const LocalListener &) = delete;

  bool listening() const { return socket_ != INVALID_SOCKET; }
  uint16_t port() const { return port_; }

  // Waits up to `timeout_ms` for a connection and accepts it; returns whether one arrived.
  bool Accept(int timeout_ms);

private:
  SOCKET socket_ = INVALID_SOCKET;
  SOCKET accepted_ = INVALID_SOCKET;
  uint16_t port_ = 0;
};

} // namespace chat_test
