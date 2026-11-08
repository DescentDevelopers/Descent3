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

#ifdef WIN32
#include <winsock.h>
#endif

namespace d3_test {

// Starts the platform's sockets the first time it is called in a process, as nw_InitNetworking() does in the game.
// Windows needs this (WSAStartup) before any socket or name lookup; elsewhere it does nothing.
inline void EnsureSockets() {
#ifdef WIN32
  static const bool started = [] {
    WSADATA data;
    return WSAStartup(MAKEWORD(1, 1), &data) == 0;
  }();
  (void)started;
#endif
}

} // namespace d3_test
