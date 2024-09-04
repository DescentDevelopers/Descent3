/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 * Copyright (C) 2024 Descent Developers
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

--- HISTORICAL COMMENTS FOLLOW ---

 * $Logfile: /DescentIII/Main/mac/MACGAMESPY.CPP $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:58:15 $
 * $Author: kevinb $
 *
 * Gamespy client code
 * This library knows about the game and is responsible for telling the
 * gamespy client and server about everything it wants to know
 *
 *
 * $Log: MACGAMESPY.CPP,v $
 * Revision 1.1.1.1  2003/08/26 03:58:15  kevinb
 * initial 1.5 import
 *
 *
 * 2     10/21/99 1:55p Kevin
 * Mac Merge!
 *
 * 1     7/28/99 2:31p Kevin
 * Mac only stuff
 *
 *
 *
 * $NoKeywords: $
 */

#include <cstdlib>
#include <memory.h>

#include "args.h"
#include "cfile.h"
#include "crossplat.h"
#include "ddio.h"
#include "descent.h"
#include "gamespy.h"
#include "gamespyutils.h"
#include "mono.h"
#include "multi.h"
#include "networking.h"
#include "player.h"
#include "pserror.h"
#include "program.h"

#ifdef WIN32
typedef int socklen_t;
#endif

#define MAX_GAMESPY_SERVERS 5
#define MAX_GAMESPY_BUFFER 1400
#define MAX_HOSTNAMELEN 300
#define GSPY_HEARBEAT_INTERVAL 300 // Seconds between heartbeats.
#define GAMESPY_PORT 27900
#define GAMESPY_LISTENPORT 20142
#define THISGAMENAME "descent3"
#ifdef DEMO
#define THISGAMEVER "Demo2"
#elif defined(OEM)
#define THISGAMEVER "OEM"
#else
#define THISGAMEVER "Retail"
#endif
#define VALIDATE_SIZE 6


extern short Multi_kills[MAX_NET_PLAYERS];
extern short Multi_deaths[MAX_NET_PLAYERS];
// Secret code... no more so secret
const uint8_t gspy_d3_secret[VALIDATE_SIZE + 1] = "feWh2G";

SOCKET gspy_socket;
SOCKADDR_IN gspy_server[MAX_GAMESPY_SERVERS];
extern uint16_t Gameport;
int gspy_region = 0;
char gspy_outgoingbuffer[MAX_GAMESPY_BUFFER] = "";
float gspy_last_heartbeat;
bool gspy_game_running = false;
int gspy_packetnumber = 0;
int gspy_queryid = 0;
char gspy_validate[MAX_GAMESPY_BUFFER] = "";
uint16_t gspy_listenport;

void gspy_StartGame(char *name) {
  gspy_last_heartbeat = timer_GetTime() - GSPY_HEARBEAT_INTERVAL;
  gspy_game_running = true;
}

void gspy_EndGame() { gspy_game_running = false; }

int gspy_Init() {
#ifndef OEM
  std::filesystem::path cfgpath;
  std::filesystem::path gspy_cfgfilename;
  int argnum = FindArg("-gspyfile");
  if (argnum) {
    gspy_cfgfilename = GameArgs[argnum + 1];
  } else {
    gspy_cfgfilename = "gamespy.cfg";
  }

  for (auto &a : gspy_server) {
    INADDR_SET_SUN_SADDR(&a.sin_addr, INADDR_NONE);
    a.sin_port = htons(GAMESPY_PORT);
    a.sin_family = AF_INET;
  }

  // Read the config, resolve the name if needed and setup the server addresses
  cfgpath = std::filesystem::path(Base_directory) / gspy_cfgfilename;

  gspy_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if (SOCKET_ERROR == gspy_socket) {
    int lerror = WSAGetLastError();
    mprintf(0, "Unable to init gamespy socket! (%d)\n", lerror);
    return 0;
  }
  SOCKADDR_IN sock_addr{};
  memset(&sock_addr, 0, sizeof(SOCKADDR_IN));
  sock_addr.sin_family = AF_INET;

  unsigned int my_ip;

  my_ip = nw_GetThisIP();
  memcpy(&sock_addr.sin_addr.s_addr, &my_ip, sizeof(uint32_t));
  int portarg = FindArg("-gamespyport");
  if (portarg) {
    gspy_listenport = atoi(GameArgs[portarg + 1]);
  } else {
    gspy_listenport = GAMESPY_LISTENPORT;
  }
  mprintf(0, "Using port %d for gamespy requests.\n", gspy_listenport);
  sock_addr.sin_port = htons(gspy_listenport);
  if (bind(gspy_socket, (SOCKADDR *)&sock_addr, sizeof(sock_addr)) == SOCKET_ERROR) {
    mprintf(0, "Couldn't bind gamespy socket (%d)!\n", WSAGetLastError());
    return 0;
  }
  // make the socket non-blocking
  make_nonblocking(gspy_socket);
  CFILE *cfp = cfopen(cfgpath, "rt");
  if (cfp) {
    mprintf(0, "Found a gamespy config file!\n");
    char hostn[MAX_HOSTNAMELEN];

    for (auto &server : gspy_server) {

      // First in the config file is the region, which is a number from 0-12 (currently)
      if (cf_ReadString(hostn, MAX_HOSTNAMELEN - 1, cfp)) {
        gspy_region = atoi(hostn);
      }
      // next in the config file are the servers
      // Each gamespy server should appear in the file with the hostname:port
      // Or optionally, just the hostname
      // Examples:
      // 192.168.1.100:27900
      // master01.gamespy.com:27900
      // master02.gamespy.com
      // 192.168.1.100

      if (cf_ReadString(hostn, MAX_HOSTNAMELEN - 1, cfp)) {
        char *port = strstr(hostn, ":");
        if (port) {
          // terminate the hostname
          *port = '\0';
          // Increment to the first character of the port name
          port++;
          // get the port number
          server.sin_port = htons(atoi(port));
        }
        if (INADDR_NONE == inet_addr(hostn)) {
          // This is a name we must resolve
          HOSTENT *he;
          mprintf(0, "Resolving hostname for gamespy: %s\n", hostn);
          he = gethostbyname(hostn);
          if (!he) {
            mprintf(0, "Unable to resolve %s\n", hostn);
            // gspy_server[i].sin_addr.S_un.S_addr = INADDR_NONE;
            INADDR_SET_SUN_SADDR(&server.sin_addr, INADDR_NONE);
          } else {
            memcpy(&server.sin_addr, he->h_addr_list[0], sizeof(unsigned int));
          }
        } else {
          // This is just a number
          INADDR_SET_SUN_SADDR(&server.sin_addr, inet_addr(hostn));
        }
      }

      uint32_t resolved_addr;
      INADDR_GET_SUN_SADDR(&server.sin_addr, &resolved_addr);
      if (resolved_addr != INADDR_NONE) {
        mprintf(0, "Sending gamespy heartbeats to %s:%d\n", inet_ntoa(server.sin_addr),
                htons(server.sin_port));
      }
    }

    cfclose(cfp);
  }
#endif // #ifndef OEM
  return 1;
}

// Takes a gspy response and puts the appropriate validation code to the end
// Of the string. If crypt is something besides NULL, create and tack the proper
// response to the end
bool gpsy_ValidateString(char *str, char *crypt) {
  char keyvalue[80];
  unsigned char encrypted_val[VALIDATE_SIZE]; // don't need to num terminate
  unsigned char encoded_val[(VALIDATE_SIZE * 4) / 3 + 1];

  if (crypt) {
    strcpy((char *)encrypted_val, crypt);
    gs_encrypt(gspy_d3_secret, encrypted_val, VALIDATE_SIZE);
    gs_encode(encrypted_val, VALIDATE_SIZE, (unsigned char *)encoded_val);
    sprintf(keyvalue, "\\validate\\%s", encoded_val);
    strcat(str, keyvalue);
  }
  sprintf(keyvalue, "\\final");
  strcat(str, keyvalue);
  return false;
}

void gspy_DoFrame() {
#ifndef OEM
  SOCKADDR_IN fromaddr{};
  int bytesin;
  socklen_t fromsize = sizeof(SOCKADDR_IN);
  char inbuffer[MAX_GAMESPY_BUFFER];

  if (!gspy_game_running)
    return;
  // If it's time, send the heartbeat
  if ((timer_GetTime() - gspy_last_heartbeat) > GSPY_HEARBEAT_INTERVAL) {
    for (auto &server : gspy_server) {
      uint32_t resolved_addr;
      INADDR_GET_SUN_SADDR(&server.sin_addr, &resolved_addr);
      if (resolved_addr != INADDR_NONE) {
        mprintf(0, "Sending heartbeat to %s:%d\n", inet_ntoa(server.sin_addr), htons(server.sin_port));
        gspy_DoHeartbeat(&server);
      }
    }
    gspy_last_heartbeat = timer_GetTime();
  }
  // Look for incoming network data
  do {
    bytesin = (int)recvfrom(gspy_socket, inbuffer, MAX_GAMESPY_BUFFER, 0, (SOCKADDR *)&fromaddr, &fromsize);
    if (bytesin > 0) {
      *(inbuffer + bytesin) = '\0';
      mprintf(0, "GSPYIN: %s\n", inbuffer);
      gspy_ParseReq(inbuffer, &fromaddr);
    } else if (bytesin == SOCKET_ERROR) {
      int lerror = WSAGetLastError();
      if (lerror != WSAEWOULDBLOCK) {
        mprintf(0, "Warning: recvfrom failed for gamespy! (%d)\n", lerror);
      }
    }
  } while (bytesin > 0);
#endif
}

// Sends the packet out to whoever it is that we are sending to
int gspy_SendPacket(SOCKADDR_IN *addr) {
  gspy_packetnumber++; // packet numbers start at 1
  char keyvalue[80];

  /* FIXME: WH: this prevents to validate 'secure' requests
  if (!*gspy_outgoingbuffer) {
    // It's an empty buffer, so don't send anything!!
    return 0;
  }
  */
  gpsy_ValidateString(gspy_outgoingbuffer, *gspy_validate ? gspy_validate : nullptr);
  sprintf(keyvalue, "\\queryid\\%d.%d", gspy_queryid, gspy_packetnumber);
  strcat(gspy_outgoingbuffer, keyvalue);

  mprintf(0, "GSPYOUT: %s\n", gspy_outgoingbuffer);
  sendto(gspy_socket, gspy_outgoingbuffer, strlen(gspy_outgoingbuffer) + 1, 0, (SOCKADDR *)addr, sizeof(SOCKADDR_IN));
  *gspy_outgoingbuffer = '\0';
  return 0;
}

// Adds some values\keys to the send buffer and sends the packet if it overflows
int gspy_AddToBuffer(SOCKADDR_IN *addr, char *addstr) {
  if (strlen(gspy_outgoingbuffer) + strlen(addstr) + 50 >= MAX_GAMESPY_BUFFER + 1) {
    // package up this response and send this packet
    gspy_SendPacket(addr);

  } else {
    strcat(gspy_outgoingbuffer, addstr);
  }
  return 1;
}

// Looks for the secure key in the request and returns it if there is. If there isn't, it returns a NULL
char *gspy_GetSecure(char *req) {
  char *tokp;
  char str[MAX_GAMESPY_BUFFER];
  strcpy(str, req);
  tokp = strtok(str, "\\");
  if (tokp) {
    while (tokp) {
      if (stricmp(tokp, "secure") == 0) {
        tokp = strtok(nullptr, "\\");
        return tokp;
      }
      tokp = strtok(nullptr, "\\");
    };
    return nullptr;
  }
  return nullptr;
}

int gspy_ContainsKey(char *buffer, const char *key) {
  // If it's an empty string return 0
  if (*buffer == '\0')
    return 0;

  char str[MAX_GAMESPY_BUFFER];
  char lowkey[MAX_GAMESPY_BUFFER];
  strcpy(str, buffer);
  size_t len = strlen(str);

  for (size_t i = 0; i < len; i++) {
    str[i] = (char)tolower(str[i]);
  }
  strcpy(lowkey, key);
  len = strlen(lowkey);
  for (size_t i = 0; i < len; i++) {
    lowkey[i] = (char)tolower(lowkey[i]);
  }
  return strstr(str, lowkey) ? 1 : 0;
}

int gspy_ParseReq(char *buffer, SOCKADDR_IN *addr) {
  gspy_packetnumber = 0;
  gspy_queryid++;
  char *validate = gspy_GetSecure(buffer);
  if (validate) {
    strcpy(gspy_validate, validate);
  } else {
    *gspy_validate = 0;
  }
  if (gspy_ContainsKey(buffer, "basic")) {
    // Send basic
    gspy_DoBasic(addr);
  }
  if (gspy_ContainsKey(buffer, "info")) {
    // Send info
    gspy_DoGameInfo(addr);
  }
  if (gspy_ContainsKey(buffer, "rules")) {
    // Send rules
    gspy_DoRules(addr);
  }
  if (gspy_ContainsKey(buffer, "players")) {
    // Send players
    gspy_DoPlayers(addr);
  }
  if (gspy_ContainsKey(buffer, "status")) {
    // Send status
    gspy_DoStatus(addr);
  }
  if (gspy_ContainsKey(buffer, "echo")) {
    // Send echo
    gspy_DoEcho(addr, buffer);
  }

  gspy_SendPacket(addr);
  return 0;
}

int gspy_DoEcho(SOCKADDR_IN *addr, char *msg) {
  char buf[MAX_GAMESPY_BUFFER];

  // All this is needed in case an echo packet was embedded with other stuff
  strcpy(buf, msg);
  char *p = strstr(buf, "\\echo\\");
  if (!p) {
    mprintf(0, "Couldn't find echo keyword in gamespy query, this is a wacky bug that should never happen!\n");
    Int3();
    return 0;
  }

  // send back the string!
  gspy_AddToBuffer(addr, p);
  return 0;
}

int gspy_DoBasic(SOCKADDR_IN *addr) {
  char buf[MAX_GAMESPY_BUFFER];

  sprintf(buf, "\\gamename\\%s", THISGAMENAME);
  mprintf(0, "Sending to gamespy: %s\n", buf);
  gspy_AddToBuffer(addr, buf);

  // sprintf(buf,"\\gamever\\%d.%d",Program_version.major,Program_version.minor);
  sprintf(buf, "\\gamever\\%s %.1d.%.1d.%.1d", THISGAMEVER, Program_version.major, Program_version.minor,
          Program_version.build);
  mprintf(0, "Sending to gamespy: %s\n", buf);
  gspy_AddToBuffer(addr, buf);

  sprintf(buf, "\\location\\%d", gspy_region);
  mprintf(0, "Sending to gamespy: %s\n", buf);
  gspy_AddToBuffer(addr, buf);

  return 0;
}

int gspy_DoStatus(SOCKADDR_IN *addr) {
  gspy_DoBasic(addr);
  gspy_DoGameInfo(addr);
  gspy_DoRules(addr);
  gspy_DoPlayers(addr);

  return 0;
}

int gspy_DoRules(SOCKADDR_IN *addr) {
  char buf[MAX_GAMESPY_BUFFER];

  sprintf(buf, "\\teamplay\\%d", Num_teams);
  gspy_AddToBuffer(addr, buf);

  sprintf(buf, "\\timelimit\\%d", (Netgame.flags & NF_TIMER) ? 0 : Netgame.timelimit);
  gspy_AddToBuffer(addr, buf);
  sprintf(buf, "\\fraglimit\\%d", (Netgame.flags & NF_KILLGOAL) ? 0 : Netgame.killgoal);
  gspy_AddToBuffer(addr, buf);
  sprintf(buf, "\\cl_pxotrack\\%d", Game_is_master_tracker_game);
  gspy_AddToBuffer(addr, buf);
  sprintf(buf, "\\mouselook\\%d", (Netgame.flags & NF_ALLOW_MLOOK) ? 1 : 0);
  gspy_AddToBuffer(addr, buf);
  sprintf(buf, "\\permissable\\%d", (Netgame.flags & NF_PERMISSABLE) ? 1 : 0);
  gspy_AddToBuffer(addr, buf);
  sprintf(buf, "\\brightships\\%d", (Netgame.flags & NF_BRIGHT_PLAYERS) ? 1 : 0);
  gspy_AddToBuffer(addr, buf);
  sprintf(buf, "\\acccollisions\\%d", (Netgame.flags & NF_USE_ACC_WEAP) ? 1 : 0);
  gspy_AddToBuffer(addr, buf);

  sprintf(buf, "\\randpowerup\\%d", (Netgame.flags & NF_RANDOMIZE_RESPAWN) ? 1 : 0);
  gspy_AddToBuffer(addr, buf);

  return 0;
}

int gspy_DoPlayers(SOCKADDR_IN *addr) {
  char buf[MAX_GAMESPY_BUFFER];
  int player_count = 0;
  for (int i = 0; i < MAX_NET_PLAYERS; i++) {
    if (NetPlayers[i].flags & NPF_CONNECTED) {
      sprintf(buf, "\\player_%d\\%s", player_count, Players[i].callsign);
      gspy_AddToBuffer(addr, buf);
      sprintf(buf, "\\frags_%d\\%d", player_count, Multi_kills[i]);
      gspy_AddToBuffer(addr, buf);
      sprintf(buf, "\\deaths_%d\\%d", player_count, Multi_deaths[i]);
      gspy_AddToBuffer(addr, buf);
      sprintf(buf, "\\team_%d\\%d", player_count, Players[i].team);
      gspy_AddToBuffer(addr, buf);
      sprintf(buf, "\\ping_%d\\%.0f", player_count, (NetPlayers[i].ping_time * 1000.0));
      gspy_AddToBuffer(addr, buf);

      player_count++;
    }
  }

  return 0;
}

int gspy_DoGameInfo(SOCKADDR_IN *addr) {
  char buf[MAX_GAMESPY_BUFFER];
  int curplayers = 0;
  for (auto &NetPlayer : NetPlayers) {
    if (NetPlayer.flags & NPF_CONNECTED)
      curplayers++;
  }

  sprintf(buf, "\\hostname\\%s", Netgame.name);
  gspy_AddToBuffer(addr, buf);
  sprintf(buf, "\\hostport\\%d", Gameport);
  gspy_AddToBuffer(addr, buf);
  sprintf(buf, "\\mapname\\%s", Netgame.mission);
  gspy_AddToBuffer(addr, buf);
  sprintf(buf, "\\gametype\\%s", Netgame.scriptname);
  gspy_AddToBuffer(addr, buf);
  sprintf(buf, "\\numplayers\\%d", curplayers);
  gspy_AddToBuffer(addr, buf);
  sprintf(buf, "\\maxplayers\\%d", Netgame.max_players);
  gspy_AddToBuffer(addr, buf);
  sprintf(buf, "\\gamemode\\%s", "openplaying");
  gspy_AddToBuffer(addr, buf);

  return 0;
}

int gspy_DoHeartbeat(SOCKADDR_IN *addr) {
  char buf[MAX_GAMESPY_BUFFER];
  sprintf(buf, "\\heartbeat\\%d\\gamename\\%s", gspy_listenport, THISGAMENAME);
  mprintf(0, "GSPYOUT: %s\n", buf);
  sendto(gspy_socket, buf, strlen(buf) + 1, 0, (SOCKADDR *)addr, sizeof(SOCKADDR_IN));

  return 0;
}
