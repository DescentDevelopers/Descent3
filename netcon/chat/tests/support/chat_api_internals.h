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

#include "networking.h"

// Globals and functions defined in chat_api.cpp that no header declares.

// The chat server socket.
extern SOCKET Chatsock;
// Nonzero once the socket is connected; most of the public API refuses to work without it.
extern int Socket_connected;
// Nonzero once the server has confirmed our nickname.
extern int Chat_server_connected;
// Our current nickname.
extern char Nick_name[33];
// The buffers GetChatUserList() and GetChannelList() return.
extern char *User_list;
extern char *Chan_list;

// Resets the chat module's connection and channel state.
void ChatInit();
