/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
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

#ifndef _ODTCLIENT_HEADER
#define _ODTCLIENT_HEADER

#include "ui.h"

// Chat
#define CHATTRACKERNAME "irc.Descentforum.net"
#define CHATPORT 6667

#define BYPASSCHATDBKEY "BypassChatOTCP"

#define TRACKER_MENU_W 256
#define TRACKER_MENU_H 256
#define TRACKER_MENU_X (320 - (TRACKER_MENU_W / 2))
#define TRACKER_MENU_Y (240 - (TRACKER_MENU_H / 2))

#define MAXTEXTITEMS 100
#define MAXNEWWINDOWS 5
#define MAXNEWGAMEWINDOWS 5
#define MAXUIBUTTONS 20
#define MAXUITEXTS 20
#define MAXEDITS 20
#define MAXLISTS 20

#include "con_dll.h"
/*
#define MAX_GAME_NAME_LEN 100
#define MAX_MISSION_NAME_LEN 100
#define MAX_MISSION_FILE_LEN 40
#define MAX_SCRIPT_LEN 40
#define MAX_FORMAT_STRING 300*/

typedef struct s_server_game_list {
  /*
  char name[MAX_GAME_NAME_LEN];
  network_address address;
  char mission_name[MAX_MISSION_NAME_LEN];
  char mission_file[MAX_MISSION_FILE_LEN];
  char script[MAX_SCRIPT_LEN];
  int16_t level;
  int16_t curr_players;
  int16_t max_players;
  float ping;
  uint32_t flags;
  char format_string[MAX_FORMAT_STRING];

  bool dedicated;
  */
  bool used;
  uint32_t handle;
  void *ti;
  int lb_no;
} server_game_list;

int MainMultiplayerMenu();
int SearchMasterTrackerGameMenu();
int StartMultiplayerGameMenu();
int JoinNewLobby(const char *lobby);
const char *SendWhisper(const char *name);
int JoinPrivateLobby();
int FindPilot();
#endif
