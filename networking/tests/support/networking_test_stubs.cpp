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

// Link stubs for symbols networking.cpp needs but which are defined outside the networking library. None of the code
// paths exercised by the tests reach these. The Descent3 executable's symbols come from d3_test_game.

#ifdef WIN32
#include "directplay.h"

// Normally defined in directplay.cpp. Keeping DirectPlay off routes networking.cpp through its UDP code paths.
// These stubs must cover every directplay.cpp symbol that networking.cpp references: that keeps the linker from pulling
// directplay.obj (which needs the game's player tables) out of the networking library. If networking.cpp starts using
// another directplay.cpp symbol, the link fails with duplicate or unresolved symbols; add a stub for it here.
bool Use_DirectPlay = false;
uint32_t Pending_dp_conn[MAX_PENDING_NEW_CONNECTIONS];

bool dp_DidLobbyLaunchGame() { return false; }
void dp_DirectPlayDestroyPlayer(DPID) {}
void dp_DirectPlayDispatch() {}
int dp_DirectPlaySend(network_address *, uint8_t *, int, bool) { return 0; }
void dp_ShutdownDirectPlay() {}
#endif
