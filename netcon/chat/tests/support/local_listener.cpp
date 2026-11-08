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

#include "local_listener.h"

#include <cstring>

#include "d3_test/sockets.h"

namespace chat_test {

void CloseSocket(SOCKET s) {
#ifdef WIN32
  closesocket(s);
#else
  close(s);
#endif
}

LocalListener::LocalListener() {
  d3_test::EnsureSockets();
  socket_ = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_ == INVALID_SOCKET)
    return;
  SOCKADDR_IN addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = 0;
#ifdef WIN32
  int len = sizeof(addr);
#else
  socklen_t len = sizeof(addr);
#endif
  if (bind(socket_, reinterpret_cast<SOCKADDR *>(&addr), sizeof(addr)) == SOCKET_ERROR ||
      listen(socket_, 1) == SOCKET_ERROR ||
      getsockname(socket_, reinterpret_cast<SOCKADDR *>(&addr), &len) == SOCKET_ERROR) {
    CloseSocket(socket_);
    socket_ = INVALID_SOCKET;
    return;
  }
  port_ = ntohs(addr.sin_port);
}

LocalListener::~LocalListener() {
  if (accepted_ != INVALID_SOCKET)
    CloseSocket(accepted_);
  if (socket_ != INVALID_SOCKET)
    CloseSocket(socket_);
}

bool LocalListener::Accept(int timeout_ms) {
  fd_set read_fds;
  FD_ZERO(&read_fds);
  FD_SET(socket_, &read_fds);
  timeval timeout;
  timeout.tv_sec = timeout_ms / 1000;
  timeout.tv_usec = (timeout_ms % 1000) * 1000;
  if (select(static_cast<int>(socket_) + 1, &read_fds, nullptr, nullptr, &timeout) <= 0)
    return false;
  accepted_ = accept(socket_, nullptr, nullptr);
  return accepted_ != INVALID_SOCKET;
}

} // namespace chat_test
