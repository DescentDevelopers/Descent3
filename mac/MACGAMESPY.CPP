/*
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
#include "pstypes.h"
#include "pserror.h"
#include "player.h"
#include "multi.h"
#include "networking.h"
#include "descent.h"
#include "ddio.h"
#include "args.h"
#include "CFILE.H"
#include "program.h"
#include <stdlib.h>
#include <memory.h>
#include "gamespyutils.h"
#include "gamespy.h"
#ifdef FIXED
extern short Multi_kills[MAX_NET_PLAYERS];
extern short Multi_deaths[MAX_NET_PLAYERS];
// Secret code... encrypted using some really high tech method...
char gspy_d3_secret[10]; // = "feWh2G";
const char origstring[] = {(const char)0x50, (const char)0xf8, (const char)0xa4,
                           (const char)0xba, (const char)0xc7, (const char)0x7c};
char gspy_cfgfilename[_MAX_PATH];
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
SOCKET gspy_socket;
SOCKADDR_IN gspy_server[MAX_GAMESPY_SERVERS];
extern ushort Gameport;
int gspy_region = 0;
char gspy_outgoingbuffer[MAX_GAMESPY_BUFFER] = "";
float gspy_last_heartbeat;
bool gspy_game_running = false;
int gspy_packetnumber = 0;
int gspy_queryid = 0;
char gspy_validate[MAX_GAMESPY_BUFFER] = "";
unsigned short gspy_listenport;
#endif // FIXED
// Register a game with this library so we will tell the servers about it...
void gspy_StartGame(char *name) {
#ifdef FIXED
  gspy_last_heartbeat = timer_GetTime() - GSPY_HEARBEAT_INTERVAL;
  gspy_game_running = true;
#endif // FIXED
}
// Let the servers know that the game is over
void gspy_EndGame() {
#ifdef FIXED
  gspy_game_running = false;
#endif // FIXED
}
// Initialize gamespy with the info we need to talk to the servers
int gspy_Init(void) {
#ifdef FIXED
#ifndef OEM
  char cfgpath[_MAX_PATH * 2];
  int argnum = FindArg("-gspyfile");
  if (argnum) {
    strcpy(gspy_cfgfilename, GameArgs[argnum + 1]);
  } else {
    strcpy(gspy_cfgfilename, "gamespy.cfg");
  }

  for (int a = 0; a < MAX_GAMESPY_SERVERS; a++) {
    // gspy_server[a].sin_addr.S_un.S_addr = INADDR_NONE;
    INADDR_SET_SUN_SADDR(&gspy_server[a].sin_addr, INADDR_NONE);
    gspy_server[a].sin_port = htons(GAMESPY_PORT);
    gspy_server[a].sin_family = AF_INET;
  }
  unsigned char keychars[] = {0x36, 0x9d, 0xf3, 0xd2, 0xf5, 0x3b, 0x42, 0xcc, 0x58};
  for (int i = 0; i < 6; i++) {
    gspy_d3_secret[i] = (char)(origstring[i] ^ keychars[i]);
  }
  gspy_d3_secret[6] = NULL;
  gspy_d3_secret[7] = NULL;

  // strcpy(gspy_d3_secret,"feWh2G\0\0");
  // Read the config, resolve the name if needed and setup the server addresses
  ddio_MakePath(cfgpath, Base_directory, gspy_cfgfilename, NULL);

  gspy_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  ;
  if (SOCKET_ERROR == gspy_socket) {
    int lerror = WSAGetLastError();
    mprintf((0, "Unable to init gamespy socket! (%d)\n", lerror));
    return 0;
  }
  SOCKADDR_IN sock_addr;
  memset(&sock_addr, 0, sizeof(SOCKADDR_IN));
  sock_addr.sin_family = AF_INET;

  unsigned int my_ip;

  my_ip = nw_GetThisIP();
  memcpy(&sock_addr.sin_addr.s_addr, &my_ip, sizeof(uint));
  int portarg = FindArg("-gamespyport");
  if (portarg) {
    gspy_listenport = htons(atoi(GameArgs[portarg + 1]));
  } else {
    gspy_listenport = htons(GAMESPY_LISTENPORT);
  }
  mprintf((0, "Using port %d for gamespy requests.\n", GAMESPY_LISTENPORT));
  sock_addr.sin_port = gspy_listenport;
  if (bind(gspy_socket, (SOCKADDR *)&sock_addr, sizeof(sock_addr)) == SOCKET_ERROR) {
    mprintf((0, "Couldn't bind gamespy socket (%d)!\n", WSAGetLastError()));
    return 0;
  }
  int error;
  unsigned long arg;
  arg = TRUE;
  // make the socket non blocking
#ifdef WIN32
  error = ioctlsocket(gspy_socket, FIONBIO, &arg);
#elif defined(__LINUX__)
  error = ioctl(gspy_socket, FIONBIO, &arg);
#endif
  CFILE *cfp = cfopen(cfgpath, "rt");
  if (cfp) {
    mprintf((0, "Found a gamespy config file!\n"));
    char hostn[MAX_HOSTNAMELEN];

    for (int i = 0; i < MAX_GAMESPY_SERVERS; i++) {

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
          *port = NULL;
          // Increment to the first character of the port name
          port++;
          // get the port number
          gspy_server[i].sin_port = htons(atoi(port));
        }
        if (INADDR_NONE == inet_addr(hostn)) {
          // This is a name we must resolve
          HOSTENT *he;
          mprintf((0, "Resolving hostname for gamespy: %s\n", hostn));
          he = gethostbyname(hostn);
          if (!he) {
            mprintf((0, "Unable to resolve %s\n", hostn));
            // gspy_server[i].sin_addr.S_un.S_addr = INADDR_NONE;
            INADDR_SET_SUN_SADDR(&gspy_server[i].sin_addr, INADDR_NONE);
          } else {
            // memcpy(&gspy_server[i].sin_addr.S_un.S_addr,he->h_addr_list[0],sizeof(unsigned int));
            memcpy(&gspy_server[i].sin_addr, he->h_addr_list[0], sizeof(unsigned int));
          }
        } else {
          // This is just a number
          // gspy_server[i].sin_addr.S_un.S_addr = inet_addr(hostn);
          INADDR_SET_SUN_SADDR(&gspy_server[i].sin_addr, inet_addr(hostn));
          // break;
        }
      }
#if defined(WIN32)
      if (gspy_server[i].sin_addr.S_un.S_addr != INADDR_NONE) {
        mprintf((0, "Sending gamespy heartbeats to %s:%d\n", inet_ntoa(gspy_server[i].sin_addr),
                 htons(gspy_server[i].sin_port)));
      }
#elif defined(__LINUX__)
      if (gspy_server[i].sin_addr.s_addr != INADDR_NONE) {
        mprintf((0, "Sending gamespy heartbeats to %s:%d\n", inet_ntoa(gspy_server[i].sin_addr),
                 htons(gspy_server[i].sin_port)));
      }
#endif
    }
  }
#endif
#endif // FIXED
  return 1;
}
// Takes a gspy response and puts the appropriate validation code to the end
// Of the string. If crypt is something besides NULL, create and tack the proper
// response to the end
#define VALIDATE_SIZE 6
bool gpsy_ValidateString(char *str, char *crypt) {
#ifdef FIXED

  char keyvalue[80];
  unsigned char encrypted_val[VALIDATE_SIZE]; // don't need to num terminate
  unsigned char encoded_val[(VALIDATE_SIZE * 4) / 3 + 1];

  if (crypt) {
    strcpy((char *)encrypted_val, crypt);
    gspy_encrypt(encrypted_val, VALIDATE_SIZE, (unsigned char *)gspy_d3_secret);
    gspy_encode(encrypted_val, VALIDATE_SIZE, (unsigned char *)encoded_val);
    sprintf(keyvalue, "\\validate\\%s", encoded_val);
    strcat(str, keyvalue);
  }
  sprintf(keyvalue, "\\final\\");
  strcat(str, keyvalue);
#endif // FIXED
  return false;
}
// Check the socket for data, and respond properly if needed
// Also send heartbeat when needed
void gspy_DoFrame() {
#ifdef FIXED
#ifndef OEM
  SOCKADDR_IN fromaddr;
  int bytesin;
  int fromsize = sizeof(SOCKADDR_IN);
  char inbuffer[MAX_GAMESPY_BUFFER];

  if (!gspy_game_running)
    return;
  // If it's time, send the heartbeat
  if ((timer_GetTime() - gspy_last_heartbeat) > GSPY_HEARBEAT_INTERVAL) {
    for (int a = 0; a < MAX_GAMESPY_SERVERS; a++) {
#if defined(WIN32)
      if (INADDR_NONE != gspy_server[a].sin_addr.S_un.S_addr) {
        mprintf(
            (0, "Sending heartbeat to %s:%d\n", inet_ntoa(gspy_server[a].sin_addr), htons(gspy_server[a].sin_port)));
        gspy_DoHeartbeat(&gspy_server[a]);
      }
#elif defined(__LINUX__)
      if (INADDR_NONE != gspy_server[a].sin_addr.s_addr) {
        mprintf(
            (0, "Sending heartbeat to %s:%d\n", inet_ntoa(gspy_server[a].sin_addr), htons(gspy_server[a].sin_port)));
        gspy_DoHeartbeat(&gspy_server[a]);
      }
#endif
    }
    gspy_last_heartbeat = timer_GetTime();
  }
  // Look for incoming network data
  do {
    bytesin = recvfrom(gspy_socket, inbuffer, MAX_GAMESPY_BUFFER, 0, (SOCKADDR *)&fromaddr, &fromsize);
    if (bytesin > 0) {
      *(inbuffer + bytesin) = NULL;
      mprintf((0, "Got a gamespy request:\n%s\n", inbuffer));
      gspy_ParseReq(inbuffer, &fromaddr);
    } else if (bytesin == SOCKET_ERROR) {
      int lerror = WSAGetLastError();
      if (lerror != WSAEWOULDBLOCK) {
        mprintf((0, "Warning: recvfrom failed for gamespy! (%d)\n", lerror));
      }
    }
  } while (bytesin > 0);
#endif
#endif // FIXED
}
// Sends the packet out to whoever it is that we are sending to
int gspy_SendPacket(SOCKADDR_IN *addr) {
#ifdef FIXED
  gspy_packetnumber++; // packet numbers start at 1
  char keyvalue[80];
  if (!*gspy_outgoingbuffer) {
    // It's an empty buffer, so don't send anything!!
    return 0;
  }
  gpsy_ValidateString(gspy_outgoingbuffer, *gspy_validate ? gspy_validate : NULL);
  sprintf(keyvalue, "\\queryid\\%d.%d", gspy_queryid, gspy_packetnumber);
  strcat(gspy_outgoingbuffer, keyvalue);

  mprintf((0, "GSPYOUT:%s\n", gspy_outgoingbuffer));
  sendto(gspy_socket, gspy_outgoingbuffer, strlen(gspy_outgoingbuffer) + 1, 0, (SOCKADDR *)addr, sizeof(SOCKADDR_IN));
  *gspy_outgoingbuffer = NULL;
#endif // FIXED
  return 0;
}
// Adds some values\keys to the send buffer and sends the packet if it overflows
int gspy_AddToBuffer(SOCKADDR_IN *addr, char *addstr) {
#ifdef FIXED
  if (strlen(gspy_outgoingbuffer) + strlen(addstr) + 50 >= MAX_GAMESPY_BUFFER + 1) {
    // package up this response and send this packet
    gspy_SendPacket(addr);

  } else {
    strcat(gspy_outgoingbuffer, addstr);
  }
#endif // FIXED
  return 1;
}
// Looks for the secure key in the request and returns it if there is. If there isn't, it returns a NULL
char *gspy_GetSecure(char *req) {
#ifdef FIXED
  char *tokp;
  char str[MAX_GAMESPY_BUFFER];
  strcpy(str, req);
  tokp = strtok(str, "\\");
  if (tokp) {
    while (tokp) {
      if (strcmpi(tokp, "secure") == 0) {
        tokp = strtok(NULL, "\\");
        return tokp;
      }
      tokp = strtok(NULL, "\\");
    };
    return NULL;
  } else {
    return NULL;
  }
#endif // FIXED
  return NULL;
}
int gspy_ContainsKey(char *buffer, char *key) {
#ifdef FIXED
  char str[MAX_GAMESPY_BUFFER];
  char lowkey[MAX_GAMESPY_BUFFER];
  strcpy(str, buffer);
  int len = strlen(str);
  int i;
  // If it's an empty string return 0
  if (*buffer == '\0')
    return 0;
  for (i = 0; i < len; i++)
    tolower(str[i]);
  strcpy(lowkey, key);
  len = strlen(str);
  for (i = 0; i < len; i++)
    tolower(lowkey[i]);
  if (strstr(str, lowkey)) {
    return 1;
  } else {
    return 0;
  }
#endif // FIXED
}
int gspy_ParseReq(char *buffer, SOCKADDR_IN *addr) {
#ifdef FIXED
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
#endif // FIXED
  return 0;
}
int gspy_DoEcho(SOCKADDR_IN *addr, char *msg) {
#ifdef FIXED
  char buf[MAX_GAMESPY_BUFFER];

  // All this is needed in case an echo packet was embedded with other stuff
  strcpy(buf, msg);
  char *p = strstr(buf, "\\echo\\");
  if (!p) {
    mprintf((0, "Couldn't find echo keyword in gamespy query, this is a wacky bug that should never happen!\n"));
    Int3();
    return 0;
  }

  // send back the string!
  gspy_AddToBuffer(addr, p);
#endif // FIXED
  return 0;
}
int gspy_DoBasic(SOCKADDR_IN *addr) {
#ifdef FIXED
  char buf[MAX_GAMESPY_BUFFER];

  sprintf(buf, "\\gamename\\%s", THISGAMENAME);
  gspy_AddToBuffer(addr, buf);
  // sprintf(buf,"\\gamever\\%d.%d",Program_version.major,Program_version.minor);
  sprintf(buf, "\\gamever\\%s %.1d.%.1d.%.1d", THISGAMEVER, Program_version.major, Program_version.minor,
          Program_version.build);
  gspy_AddToBuffer(addr, buf);
  sprintf(buf, "\\location\\%d", gspy_region);
  gspy_AddToBuffer(addr, buf);

#endif // FIXED
  return 0;
}
int gspy_DoStatus(SOCKADDR_IN *addr) {
#ifdef FIXED
  gspy_DoBasic(addr);
  gspy_DoGameInfo(addr);
  gspy_DoRules(addr);
  gspy_DoPlayers(addr);
#endif // FIXED
  return 0;
}
int gspy_DoRules(SOCKADDR_IN *addr) {
#ifdef FIXED
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
#endif // FIXED
  return 0;
}
// Send the player list to whoever wants it.
int gspy_DoPlayers(SOCKADDR_IN *addr) {
#ifdef FIXED
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
#endif // FIXED
  return 0;
}
int gspy_DoGameInfo(SOCKADDR_IN *addr) {
#ifdef FIXED
  char buf[MAX_GAMESPY_BUFFER];
  int curplayers = 0;
  for (int i = 0; i < MAX_NET_PLAYERS; i++) {
    if (NetPlayers[i].flags & NPF_CONNECTED)
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
#endif // FIXED
  return 0;
}
int gspy_DoHeartbeat(SOCKADDR_IN *addr) {
#ifdef FIXED
  char buf[MAX_GAMESPY_BUFFER];
  sprintf(buf, "\\heartbeat\\%d\\gamename\\%s", htons(gspy_listenport), THISGAMENAME);
  mprintf((0, "GSPYOUT:%s\n", buf));
  sendto(gspy_socket, buf, strlen(buf) + 1, 0, (SOCKADDR *)addr, sizeof(SOCKADDR_IN));
#endif // FIXED
  return 0;
}