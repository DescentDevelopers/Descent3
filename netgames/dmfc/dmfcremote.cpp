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

--- HISTORICAL COMMENTS FOLLOW ---

 * $Logfile: /DescentIII/Main/Dmfc/dmfcremote.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:57:21 $
 * $Author: kevinb $
 *
 * Remote administration
 *
 * $Log: dmfcremote.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:57:21  kevinb
 * initial 1.5 import
 *
 *
 * 6     7/14/99 11:48a Jeff
 * localized text for patch fixes
 *
 * 5     7/09/99 6:18p Jeff
 * added $remoteadminlogout and $wait commands
 *
 * 4     7/08/99 6:25p Jeff
 * remote admin in and working
 *
 * 3     7/08/99 2:38a Jeff
 * rough implementation of remote administration checked in.  Still needs
 * some polishing, but should work basically.
 *
 * 2     7/07/99 7:07p Jeff
 * created dmfcremote.cpp
 *
 * $NoKeywords: $
 */

#include "gamedll_header.h"
#include "DMFC.h"
#include "dmfcinternal.h"
#include "encryption.h"

#include <stdlib.h>
#include <memory.h>

extern char **DMFCStringTable;
extern int DMFCStringTableSize;
extern const char *_DMFCErrorString;
extern DMFCBase *basethis;
const char *DMFCGetString(int d);

struct tRemotePlayerData {
  bool authorized;   // whether the player is authorized
  uint8_t curr_key[8]; // current encryption key
};

static tRemotePlayerData Authorized_players[MAX_PLAYER_RECORDS];
static char Remote_Admin_pass[64];
bool Use_remote_admin = false;
static bool IAmAnAdmin = false;

static void Remote_encrypt(uint8_t key[8], int size_buffer, uint8_t *buffer, int *new_size, uint8_t **new_buffer);
static void Remote_decrypt(uint8_t key[8], int size_buffer, uint8_t *buffer, int *new_size, uint8_t **new_buffer);

// Sends a string to the server (function encrypts it before sending)
void Remote_SendStringToServer(const char *string);
// Recieves a string from the server (function decrypts it)
void Remote_RecieveStringFromServer(uint8_t *data, char *buffer, int size);
// Sends a string to a client (function encrypts it before sending)
void Remote_SendStringToClient(int client, const char *string);
// Recieves a string from a client (function decrypts it)
int Remote_RecieveStringFromClient(uint8_t *data, char *buffer, int size);
// Handles remote admin packets from a client
void Remote_HandleClientPacket(uint8_t *data);
// Handles remote admin packets from a client
void Remote_HandleServerPacket(uint8_t *data);

// Sends a message from the server to a client
void Remote_SendMessage(int client, const char *message);
// Receives a message from the server
void Remote_GetMessage(uint8_t *data);

// Logs a player out from being an administrator
void Remote_Logout(int precnum);

// filters any remote commands not allowed to be executed remotely
// -1 : not found
// 0 : can't call
// 1 : can call
int FilterNonRemoteCommands(char *command);

// Initializes the remote administration system
void Remote_Initialize(void) {
  // initialize encryption keys
  srand((uint32_t)time(0));

  int i, j;
  for (i = 0; i < MAX_PLAYER_RECORDS; i++) {
    Authorized_players[i].authorized = false;

    for (j = 0; j < 8; j++) {
      Authorized_players[i].curr_key[j] = rand() % 255;
    }
  }

  memset(Remote_Admin_pass, 0, 64);

  // setup packet recievers
#ifdef WIN32
  basethis->RegisterPacketReceiver(SPID_REMOTETOSERVER, Remote_HandleClientPacket);
  basethis->RegisterPacketReceiver(SPID_REMOTETOCLIENT, Remote_HandleServerPacket);
  basethis->RegisterPacketReceiver(SPID_SERVERREMOTEMSG, Remote_GetMessage);
#else
  basethis->RegisterPacketReceiver(SPID_REMOTETOSERVER, &Remote_HandleClientPacket);
  basethis->RegisterPacketReceiver(SPID_REMOTETOCLIENT, &Remote_HandleServerPacket);
  basethis->RegisterPacketReceiver(SPID_SERVERREMOTEMSG, &Remote_GetMessage);
#endif

  if (basethis->GetLocalRole() == LR_SERVER) {
    bool bTemp;
    int pass_len = 64;
    DatabaseRead1("DMFCRemoteAdminPass", Remote_Admin_pass, &pass_len);
    if (DatabaseRead3("DMFCRemoteAdmin", &bTemp)) {
      Remote_Enable(bTemp);
    }
  }
}

// Processes remote administration for the frame
void Remote_ProcessFrame(void) {
  if (basethis->GetLocalRole() != LR_SERVER)
    return;

  // go through all the players, and update their state (whether
  // they are in the game or not)
  int p;
  for (p = 0; p < MAX_PLAYER_RECORDS; p++) {
    player_record *pr = PRec_GetPRecord(p);

    if (Authorized_players[p].authorized) {
      // this is an authorized player, update them

      // make sure they are in the game
      if (!pr || pr->state != STATE_INGAME) {
        // the player is no longer in the game
        mprintf(0, "REMOTE: Removing authorization for %s\n", (pr->callsign) ? (pr->callsign) : "<UNKNOWN>");
        Remote_Logout(p);

        // change the key for the slot
        int j;
        for (j = 0; j < 8; j++) {
          Authorized_players[p].curr_key[j] = rand() % 255;
        }

      } else {
        // the player is authorized and in the game
      }
    }
  }
}

// Gets the key for the given player record
uint8_t *Remote_GetKey(int prec) {
  ASSERT(prec >= 0 && prec < MAX_PLAYER_RECORDS);
  return Authorized_players[prec].curr_key;
}

// Enables/Disables remote administration
void Remote_Enable(bool enable) {
  if (basethis->GetLocalRole() != LR_SERVER)
    return;

  Use_remote_admin = enable;

  if (*Remote_Admin_pass == '\0' && enable) {
    Use_remote_admin = false;
  }

  mprintf(0, "Remote Administration: %s\n", (Use_remote_admin) ? "On" : "Off");

  if (!Use_remote_admin) {
    // log out all the logged in players
    int p;
    for (p = 0; p < MAX_PLAYER_RECORDS; p++) {
      if (Authorized_players[p].authorized) {
        // log the player out
        Remote_Logout(p);
      }
    }
  }

  DatabaseWrite2("DMFCRemoteAdmin", Use_remote_admin);
}

// sets the administration password for the server
// pass in an unencrypted password
void Remote_SetPassword(char *password) {
  if (basethis->GetLocalRole() != LR_SERVER)
    return;
  strncpy(Remote_Admin_pass, password, 63);
  Remote_Admin_pass[63] = '\0';
  DatabaseWrite1("DMFCRemoteAdminPass", Remote_Admin_pass, strlen(Remote_Admin_pass));
}

// Logs a player record in as an administrator
bool Remote_Login(int precnum, char *pass) {
  if (precnum < 0 || precnum >= MAX_PLAYER_RECORDS)
    return false;

  player_record *pr = PRec_GetPRecord(precnum);
  if (!pr || pr->state != STATE_INGAME) {
    return false;
  }

  if (Authorized_players[precnum].authorized)
    return true;

  if (strcmp(pass, Remote_Admin_pass) == 0) {
    Authorized_players[precnum].authorized = true;
  }

  return Authorized_players[precnum].authorized;
}

// Logs a player out from being an administrator
void Remote_Logout(int precnum) {
  if (precnum < 0 || precnum >= MAX_PLAYER_RECORDS)
    return;

  if (!Authorized_players[precnum].authorized)
    return;

  player_record *pr = PRec_GetPRecord(precnum);
  if (!pr)
    return;
  if (pr->state == STATE_INGAME) {
    Remote_SendStringToClient(pr->pnum, "LOGOUTSUCCESS");
    Remote_SendMessage(pr->pnum, DTXT_REMOTELOGGEDOUT);
  }
  Authorized_players[precnum].authorized = false;

  DLLAddColoredHUDMessage(GR_RGB(200, 200, 40), DTXT_HUDMSGLOGGEDOUT, pr->callsign);
}

// Lists the players logged in
void Remote_ListLogins(void) {
  int prec;
  player_record *pr;

  char buffer[512];
  char temp[64];
  int len, curlen;

  curlen = 0;
  *buffer = '\0';

  for (prec = 0; prec < MAX_PLAYER_RECORDS; prec++) {
    if (Authorized_players[prec].authorized) {
      pr = PRec_GetPRecord(prec);
      if (pr && pr->state != STATE_EMPTY) {
        len = strlen(pr->callsign);
        if (curlen + len > 500) {
          // output the hudmessage
          DLLAddColoredHUDMessage(GR_RGB(200, 200, 40), buffer);
          *buffer = '\0';
          curlen = 0;
        }
        if (*buffer != '\0') {
          strncat(buffer, ":", sizeof(buffer) - strlen(buffer) - 1);
        }
        snprintf(temp, sizeof(temp), "%s[%d]", pr->callsign, prec);
        strncat(buffer, temp, sizeof(buffer) - strlen(buffer) - 1);
        curlen = strlen(buffer);
      }
    }
  }

  if (*buffer != '\0') {
    DLLAddColoredHUDMessage(GR_RGB(200, 200, 40), buffer);
  }
}

extern player_record Player_records[MAX_PLAYER_RECORDS];
int translate_precptr_to_index(player_record *pr) {
  int index = (pr - Player_records);
  if (index < 0 || index >= MAX_PLAYER_RECORDS)
    index = -1;
  return index;
}

// Sets a clients key
void Remote_SetMyKey(uint8_t key[8]) {
  player_record *pr;
  pr = PRec_GetPRecordByPnum(basethis->GetPlayerNum());
  int prec = translate_precptr_to_index(pr);
  if (prec == -1) {
    mprintf(0, "INVALID PREC!\n");
    Int3();
    return;
  }

  memcpy(Authorized_players[prec].curr_key, key, 8);
}

// handles a remote command (client side)
void Remote_ClientProcess(const char *command) {
  if (basethis->GetLocalRole() == LR_SERVER)
    return;
  if (!IAmAnAdmin) {
    // only let "login" go through
    if (!strnicmp(command, "login", strlen("login"))) {
      mprintf(0, "Sending login attempt for remote administration\n");
    } else {
      return;
    }
  }

  const char *ptr = command;
  while (*ptr == ' ')
    ptr++;
  if (*ptr) {
    Remote_SendStringToServer(ptr);
  }
}

// handles a remote command (server side)
void Remote_ServerProcess(int precnum, char *command) {
  if (basethis->GetLocalRole() != LR_SERVER)
    return;
  if (precnum < 0 || precnum >= MAX_PLAYER_RECORDS)
    return;
  if (!Use_remote_admin)
    return;

  if (!Authorized_players[precnum].authorized) {
    // check to see if the player is trying to login
    int loginlen = strlen("login ");
    if (!strnicmp(command, "login ", loginlen)) {
      char *ptr = &command[loginlen - 1];
      while (*ptr == ' ')
        ptr++;
      if (Remote_Login(precnum, ptr)) {
        // successful login
        player_record *pr = PRec_GetPRecord(precnum);
        DLLAddColoredHUDMessage(GR_RGB(200, 200, 40), DTXT_HUDMSGLOGGEDIN, pr->callsign);
        Remote_SendStringToClient(pr->pnum, "LOGINSUCCESS");
        Remote_SendMessage(pr->pnum, DTXT_REMOTELOGGEDIN);
      }
      return;
    }
  }

  player_record *pr = PRec_GetPRecord(precnum);

  // check to see if they are logging out
  if (!stricmp(command, "logout")) {
    // log the player out
    Remote_Logout(precnum);
    return;
  }

  // process the command
  if (command[0] == '$') {
    switch (FilterNonRemoteCommands(command)) {
    case 1: {
      DLLAddColoredHUDMessage(GR_RGB(200, 200, 40), DTXT_REMOTEEXECUTE, pr->callsign, command);

      if (basethis->InputCommandHandle(command)) {
        // command handled
        Remote_SendMessage(pr->pnum, DTXT_REMOTESUCCESS);
      } else {
        // command not handled
        Remote_SendMessage(pr->pnum, DTXT_REMOTENOTSUCCESS);
      }
    } break;
    case 0: {
      // command not handled
      Remote_SendMessage(pr->pnum, DTXT_REMOTENOTALLOWED);
    } break;
    default: {
      // command not handled
      Remote_SendMessage(pr->pnum, DTXT_REMOTENOTFOUND);
    } break;
    }
  } else {
    // command not handled
    Remote_SendMessage(pr->pnum, DTXT_REMOTENOTFOUND);
  }
}

// filters any remote commands not allowed to be executed remotely
int FilterNonRemoteCommands(char *command) {
  // cut off whats after the first word (the command)
  char cmd[64];
  strncpy(cmd, command, 63);
  cmd[63] = '\0';
  char *ptr = cmd;
  while (*ptr != ' ' && *ptr != '\0')
    ptr++;
  *ptr = '\0';

  return basethis->CanInputCommandBeUsedRemotely((*cmd == '$') ? cmd + 1 : cmd);
}

// Sends a message from the server to a client
void Remote_SendMessage(int client, const char *message) {
  if (basethis->GetLocalRole() != LR_SERVER)
    return;

  char *allocated = NULL;
  int len = strlen(message);
  const int maxlen = ((MAX_GAME_DATA_SIZE - 5) < 511) ? (MAX_GAME_DATA_SIZE - 5) : 511;
  if (len > maxlen) {
    allocated = new char[maxlen + 1];
    memcpy(allocated, message, maxlen);
    allocated[maxlen] = '\0';
    message = allocated;
    len = maxlen;
  }

  int count = 0;
  uint8_t data[MAX_GAME_DATA_SIZE];
  basethis->StartPacket(data, SPID_SERVERREMOTEMSG, &count);

  MultiAddString(message, data, &count);

  basethis->SendPacket(data, count, client);
  delete[] allocated;
}

// Receives a message from the server
void Remote_GetMessage(uint8_t *data) {
  int count = 0;
  char buffer[512];

  MultiGetString(buffer, data, &count);

  DLLAddColoredHUDMessage(GR_RGB(200, 200, 40), "==%s==", buffer);
}

// Sends a string to the server (function encrypts it before sending)
void Remote_SendStringToServer(const char *string) {
  if (basethis->GetLocalRole() == LR_SERVER) {
    Int3();
    return;
  }

  int slen;
  int new_strlen;
  uint8_t *packet_data;

  slen = strlen(string);
  player_record *pr;
  pr = PRec_GetPRecordByPnum(basethis->GetPlayerNum());
  int prec = translate_precptr_to_index(pr);
  if (prec == -1) {
    mprintf(0, "INVALID PREC!\n");
    Int3();
    return;
  }

  Remote_encrypt(Authorized_players[prec].curr_key, slen, (uint8_t *)string, &new_strlen, &packet_data);

  if (new_strlen == 0) {
    mprintf(0, "COULDN'T ENCRYPT\n");
    Int3();
    return;
  }

  // now we can send off the packet
  int count = 0;
  uint8_t data[MAX_GAME_DATA_SIZE];
  basethis->StartPacket(data, SPID_REMOTETOSERVER, &count);

  MultiAddByte(prec, data, &count);
  MultiAddInt(new_strlen, data, &count);
  memcpy(&data[count], packet_data, new_strlen);
  count += new_strlen;

  basethis->SendPacket(data, count, SP_SERVER);
  free(packet_data);
}

// Handles remote admin packets from a client
void Remote_HandleServerPacket(uint8_t *data) {
  char buffer[512];
  Remote_RecieveStringFromServer(data, buffer, 512);

  if (!strcmp(buffer, "LOGOUTSUCCESS")) {
    // we logged out
    IAmAnAdmin = false;
  } else if (!strcmp(buffer, "LOGINSUCCESS")) {
    // we logged in
    IAmAnAdmin = true;
  }
}

// Recieves a string from the server (function decrypts it)
void Remote_RecieveStringFromServer(uint8_t *data, char *buffer, int size) {
  int count = 0;
  int len = MultiGetInt(data, &count);

  player_record *pr;
  pr = PRec_GetPRecordByPnum(basethis->GetPlayerNum());
  int prec = translate_precptr_to_index(pr);
  if (prec == -1) {
    mprintf(0, "INVALID PREC!\n");
    Int3();
    return;
  }

  ASSERT(len > 0);
  if (len <= 0)
    return;

  uint8_t *packet;
  packet = (uint8_t *)malloc(len);
  memcpy(packet, &data[count], len);
  count += len;

  int new_size;
  uint8_t *new_buffer;
  Remote_decrypt(Authorized_players[prec].curr_key, len, packet, &new_size, &new_buffer);
  if (new_buffer) {
    strncpy(buffer, (char *)new_buffer, size - 1);
    buffer[size - 1] = '\0';
    free(new_buffer);
  } else {
    *buffer = '\0';
  }

  free(packet);
}

// Sends a string to a client (function encrypts it before sending)
void Remote_SendStringToClient(int client, const char *string) {
  if (basethis->GetLocalRole() != LR_SERVER) {
    Int3();
    return;
  }

  int slen;
  int new_strlen;
  uint8_t *packet_data;

  slen = strlen(string);
  player_record *pr;
  pr = PRec_GetPRecordByPnum(client);
  int prec = translate_precptr_to_index(pr);
  if (prec == -1) {
    mprintf(0, "INVALID PREC!\n");
    Int3();
    return;
  }

  // check to make sure the player is authorized before sending
  if (!Authorized_players[prec].authorized) {
    Int3();
    return;
  }

  Remote_encrypt(Authorized_players[prec].curr_key, slen, (uint8_t *)string, &new_strlen, &packet_data);

  if (new_strlen == 0) {
    mprintf(0, "COULDN'T ENCRYPT\n");
    Int3();
    return;
  }

  // now we can send off the packet
  int count = 0;
  uint8_t data[MAX_GAME_DATA_SIZE];
  basethis->StartPacket(data, SPID_REMOTETOCLIENT, &count);

  MultiAddInt(new_strlen, data, &count);
  memcpy(&data[count], packet_data, new_strlen);
  count += new_strlen;

  basethis->SendPacket(data, count, client);
  free(packet_data);
}

// Handles remote admin packets from a client
void Remote_HandleClientPacket(uint8_t *data) {
  char buffer[512];
  int prec = Remote_RecieveStringFromClient(data, buffer, 512);
  if (prec == -1)
    return;

  Remote_ServerProcess(prec, buffer);
}

// Recieves a string from a client (function decrypts it)
int Remote_RecieveStringFromClient(uint8_t *data, char *buffer, int size) {
  int count = 0;
  int prec = MultiGetByte(data, &count);
  int len = MultiGetInt(data, &count);

  ASSERT(len > 0);
  ASSERT(prec >= 0 && prec < MAX_PLAYER_RECORDS);
  if (prec < 0 || prec >= MAX_PLAYER_RECORDS)
    return -1;
  if (len <= 0)
    return -1;

  uint8_t *packet;
  packet = (uint8_t *)malloc(len);
  memcpy(packet, &data[count], len);
  count += len;

  int new_size;
  uint8_t *new_buffer;
  Remote_decrypt(Authorized_players[prec].curr_key, len, packet, &new_size, &new_buffer);
  if (new_buffer) {
    size = (len < size) ? len + 1 : size;
    strncpy(buffer, (char *)new_buffer, size - 1);
    buffer[size - 1] = '\0';
    free(new_buffer);
  } else {
    *buffer = '\0';
  }

  free(packet);
  return prec;
}

void Remote_pad_buffer(int orig_size, const uint8_t *orig_buffer, int *new_size, uint8_t **new_buffer) {
  // bump buffer up to size of mod 8
  *new_size = orig_size;
  if (orig_size < 8 || (orig_size % 8) != 0) {
    // we need to pad up
    int chunks = orig_size / 8;
    *new_size = (chunks + 1) * 8;
  }

  *new_buffer = (uint8_t *)malloc(*new_size);
  if (!(*new_buffer)) {
    *new_size = 0;
    return;
  }
  memset(*new_buffer, 0, *new_size);
  memcpy(*new_buffer, orig_buffer, orig_size);
}

static void Remote_encrypt(uint8_t key[8], int size_buffer, uint8_t *buffer, int *new_size, uint8_t **new_buffer) {
  *new_buffer = NULL;

  uint8_t *buff;
  Remote_pad_buffer(size_buffer, buffer, new_size, &buff);
  if (*new_size == 0)
    return;

  IceKey ice(0);

  ice.set((uint8_t *)key);

  int i, num_chunks = 0;
  int chunk_offset = 0;
  uint8_t *cipher;
  cipher = (uint8_t *)malloc(*new_size);
  if (!cipher)
    return;

  num_chunks = (*new_size) / 8;
  for (i = 0; i < num_chunks; i++) {
    ice.encrypt(&buff[chunk_offset], &cipher[chunk_offset]);
    chunk_offset += 8;
  }

  free(buff);
  *new_buffer = cipher;
}

static void Remote_decrypt(uint8_t key[8], int size_buffer, uint8_t *buffer, int *new_size, uint8_t **new_buffer) {
  *new_buffer = NULL;

  uint8_t *buff;
  Remote_pad_buffer(size_buffer, buffer, new_size, &buff);
  if (*new_size == 0)
    return;

  IceKey ice(0);

  ice.set((uint8_t *)key);

  int i, num_chunks = 0;
  int chunk_offset = 0;
  uint8_t *cipher;
  cipher = (uint8_t *)malloc(*new_size);
  if (!cipher)
    return;

  num_chunks = (*new_size) / 8;
  for (i = 0; i < num_chunks; i++) {
    ice.decrypt(&buff[chunk_offset], &cipher[chunk_offset]);
    chunk_offset += 8;
  }

  free(buff);
  *new_buffer = cipher;
}
