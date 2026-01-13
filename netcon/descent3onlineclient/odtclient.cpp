/*
 * Descent 3
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
 */

#include "ui.h"
#include "newui.h"
#include "grdefs.h"
#include "player.h"
#include "game.h"
#include "pilot.h"
#include "ddio_common.h"
#include "module.h"
#include "chat_api.h"
#include "odtstrings.h"

#define TXT_DLL_SAVESETTINGS TXT_ODT_SAVESETTINGS
#define TXT_DLL_LOADSETTINGS TXT_ODT_LOADSETTINGS

#define TXT_GEN_MPLYROPTIONS TXT_ODT_MPLYROPTIONS
#define TXT_GEN_TIMELIMIT TXT_ODT_TIMELIMIT
#define TXT_GEN_KILLGOAL TXT_ODT_KILLGOAL
#define TXT_GEN_PPS TXT_ODT_PPS
#define TXT_GEN_RESPAWNRATE TXT_ODT_RESPAWNRATE
#define TXT_GEN_MAXPLAYERS TXT_ODT_MAXPLAYERS
#define TXT_GEN_PREVMENU TXT_ODT_PREVMENU
#define TXT_GEN_CANCEL TXT_ODT_CANCEL
#define TXT_GEN_CFGALLOWEDSHIP TXT_ODT_CFGALLOWEDSHIP
#define TXT_GEN_USEROTVEL TXT_ODT_USEROTVEL
#define TXT_GEN_USEROTVEL TXT_ODT_USEROTVEL
#define TXT_GEN_USESMOOTHING TXT_ODT_USESMOOTHING
#define TXT_GEN_CLIENTSERVER TXT_ODT_CLIENTSERVER
#define TXT_GEN_PEERPEER TXT_ODT_PEERPEER
#define TXT_GEN_ACC_WEAP_COLL TXT_ODT_ACC_WEAP_COLL
#define TXT_GEN_BRIGHT_PLAYERS TXT_ODT_BRIGHT_PLAYERS

#define MULTI_USE_ALL_OPTIONS 1

#include "odtclient.h"
#include "dip_gametrack.h"

char Ourlobby[50] = "";
bool Auto_start = false;
int Bypass_chat = 0;
int ChatStarted = 0;

std::vector<apiServerEntry> knownDirectIpHosts;

#define MAX_GAMELIST_ITEMS 300
server_game_list Gamelist[MAX_GAMELIST_ITEMS];
int NextGameItemNo = 0;

int GetGameByHandle(uint32_t handle) {
  int j;
  for (j = 0; j < *DLLNum_network_games_known; j++) {
    if (DLLNetwork_games[j].handle == handle) {
      return j;
    }
  }
  return -1;
}

int GetGameServerItemByHandle(uint32_t handle) {
  for (int i = 0; i < MAX_GAMELIST_ITEMS; i++) {
    if (Gamelist[i].used) {
      if (Gamelist[i].handle == handle) {
        return i;
      }
    }
  }
  return -1;
}

int GetGameByLBNo(int selno) {
  for (int i = 0; i < MAX_GAMELIST_ITEMS; i++) {
    if (Gamelist[i].used) {
      if (Gamelist[i].lb_no == selno) {
        return GetGameByHandle(Gamelist[i].handle);
      }
    }
  }
  return -1;
}

void FormatServerLine(char *fmt, int servernum) {
  int k = servernum;
  char server_mode[20];
  char server_type[200];

  if (DLLNetwork_games[k].flags & NF_PEER_PEER) {
    strcpy(server_mode, "PP");
  } else if (DLLNetwork_games[k].flags & NF_PERMISSABLE) {
    strcpy(server_mode, "PS");
  } else {
    strcpy(server_mode, "CS");
  }

  if (DLLNetwork_games[k].flags & NF_ALLOW_MLOOK) {
    strcat(server_mode, "-ML");
  }

  snprintf(server_type, sizeof(server_type), "%s %s", server_mode, DLLNetwork_games[k].scriptname);

  int game_type_pos = strlen(server_type);

#define SERVER_TYPE_MAX_LEN 100
  while (DLLgrtext_GetTextLineWidth(server_type) > SERVER_TYPE_MAX_LEN) {
    server_type[game_type_pos] = 0;
    game_type_pos--;
  }

  sprintf(fmt, "%.20s\t\x02\x02b%s\x02\x45%.15s\x02\x63%d\x02\x6d%d/%d\x02\x7e%.3f", DLLNetwork_games[k].name,
          server_type, DLLNetwork_games[k].mission_name, DLLNetwork_games[k].level_num,
          DLLNetwork_games[k].curr_num_players, DLLNetwork_games[k].max_num_players,
          DLLNetwork_games[k].server_response_time);
}


void UpdateGamelist(void *lb) {
  int i, j;

  // Look for items that we need to remove
  for (i = 0; i < MAX_GAMELIST_ITEMS; i++) {
    bool found = false;
    if (Gamelist[i].used) {
      for (j = 0; j < *DLLNum_network_games_known; j++) {
        if (DLLNetwork_games[j].handle == Gamelist[i].handle) {
          found = true;
        }
      }
      if (!found) {
        // Now we need to fix up the lb_no items because we removed this one!
        int k = 0;
        for (k = 0; k < MAX_GAMELIST_ITEMS; k++) {
          if (!Gamelist[k].used)
            continue;

          // This is yucky, but it accounts for the shifting up of the game items in the list
          if (Gamelist[i].lb_no < Gamelist[k].lb_no)
            Gamelist[k].lb_no--;
        }
        NextGameItemNo--;
        Gamelist[i].used = false;
        Gamelist[i].handle = -1;
        DLLListRemoveItem(lb, Gamelist[i].ti);
        DLLRemoveUITextItem(Gamelist[i].ti);
      }
    }
  }

  // Look for items to add
  for (i = 0; i < *DLLNum_network_games_known; i++) {
    bool found = false;
    for (j = 0; j < MAX_GAMELIST_ITEMS; j++) {
      if (DLLNetwork_games[i].handle == Gamelist[j].handle) {
        found = true;
      }
    }
    if (!found) {
      // Add this item!
      // Update Gamelist[i].ti
      for (j = 0; j < MAX_GAMELIST_ITEMS; j++) {
        if (!Gamelist[j].used) {
          // Aha! an empty slot
          break;
        }
      }

      int k = i;
      if (j < MAX_GAMELIST_ITEMS) {
        char fmtline[200];

        Gamelist[j].lb_no = NextGameItemNo;
        NextGameItemNo++;
        Gamelist[j].handle = DLLNetwork_games[k].handle;
        Gamelist[j].used = true;
        DLLmprintf(0, "Adding %s\n", DLLNetwork_games[k].name);

        FormatServerLine(fmtline, k);

        Gamelist[j].ti = DLLCreateNewUITextItem(fmtline, GR_LIGHTGRAY);

        DLLListAddItem(lb, Gamelist[j].ti);
      } else {
        // DLLInt3();
        // Out of slots???
      }
    }
  }

  for (j = 0; j < MAX_GAMELIST_ITEMS; j++) {
    if (Gamelist[j].used && (Gamelist[j].handle != -1)) {
      int gameid = GetGameByHandle(Gamelist[j].handle);

      if (gameid != -1) {
        int k = gameid;

        char fmtline[200];
        FormatServerLine(fmtline, k);

        DLLSetUITextItemText(Gamelist[j].ti, fmtline, GR_LIGHTGRAY);
      }
    }
  }
}

// These next two function prototypes MUST appear in the extern "C" block if called
// from a CPP file.
extern "C" {
DLLEXPORT void DLLFUNCCALL DLLMultiInit(int *api_func);
DLLEXPORT void DLLFUNCCALL DLLMultiCall(int eventnum);
DLLEXPORT void DLLFUNCCALL DLLMultiClose();
}

bool All_ok = true;
bool MT_Sock_inited = false;

// Initializes the game function pointers
void DLLFUNCCALL DLLMultiInit(int *api_func) {
  Use_netgame_flags = true;
  CommonDLLInit(api_func);

  DLLmprintf(0, "Inside DLLMultiInit...\n");
  *DLLUse_DirectPlay = false;
  Auto_start = false;

  DLLmprintf(0, "About to create string table...\n");
  DLLCreateStringTable("d3online.str", &StringTable, &StringTableSize);
  DLLmprintf(0, "%d strings loaded from string table\n", StringTableSize);
  if (!StringTableSize) {
    All_ok = false;
    return;
  }

  memset(Gamelist, 0, sizeof(Gamelist));
}

// Called when the DLL is shutdown
void DLLFUNCCALL DLLMultiClose() {
  DLLmprintf(0, "Closing down Online Direct TCP-IP DLL\n");
  DLLDestroyStringTable(StringTable, StringTableSize);
}

// The main entry point where the game calls the dll
void DLLFUNCCALL DLLMultiCall(int eventnum) {
  // We don't need eventnum right now.
  DLLmprintf(0, "DLLMultiCall:%d!\n", eventnum);

  switch (eventnum) {
  case MT_EVT_GET_HELP:
    strcpy(DLLHelpText1, TXT_ODT_HELP1);
    strcpy(DLLHelpText2, TXT_ODT_HELP2);
    strcpy(DLLHelpText3, TXT_ODT_HELP3);
    break;
  case MT_EVT_LOGIN:
	//onload
    DLLDatabaseReadInt(BYPASSCHATDBKEY, &Bypass_chat);
    if (!DLLTCP_active) {
      DLLDoMessageBox(TXT_ODT_ERROR, TXT_ODT_NO_TCPIP, MSGBOX_OK, UICOL_WINDOW_TITLE, UICOL_TEXT_NORMAL);
      *DLLMultiGameStarting = 0;
      break;
    }

    if (!All_ok) {
      *DLLMultiGameStarting = 0;
      return;
    }
    {
      //  menu.... start a game, or join a game
      if (MainMultiplayerMenu()) {
       *DLLMultiGameStarting = 1;
      } else {
        *DLLMultiGameStarting = 0;
      }
    }
    break;
  case MT_EVT_FRAME:
    break;
  case MT_EVT_FIRST_FRAME:
    // ingame first frame
    // TODO, send to master via gamespy gspy_DoHeartbeat
    break;
  case MT_EVT_GAME_OVER:
	//on game end
    break;
  case MT_AUTO_LOGIN:
    if (!DLLTCP_active) {
      DLLDoMessageBox(TXT_ODT_ERROR, TXT_ODT_NO_TCPIP, MSGBOX_OK, UICOL_WINDOW_TITLE, UICOL_TEXT_NORMAL);
      *DLLMultiGameStarting = 0;
      break;
    }
    if (!All_ok)
      *DLLMultiGameStarting = 0;
    else
      *DLLMultiGameStarting = 1;

    break;
  case MT_AUTO_START:
    if (!All_ok)
      *DLLMultiGameStarting = 0;
    else
      *DLLMultiGameStarting = 1;

    break;
  case MT_RETURN_TO_GAME_LIST:
    Bypass_chat = 1;
    // menu.... start a game, or join a game
    if (MainMultiplayerMenu()) {
      *DLLMultiGameStarting = 1;
    } else {
      *DLLMultiGameStarting = 0;
    }
    break;
  }
}

#define CONNECT_IRC_TIMEOUT 30.0
#define MAX_CHAT_SEND_LEN 200
#define MAX_CHAT_CHANNELS 50
#define CHAT_INFO_LEN 200
#define CHAT_COUNT_LEN 10
#define CHAT_MAX_USERLIST 200
#define CHAT_USER_REFRESH_TIME 5
#define CHAT_LIST_REFRESH_TIME 10

struct {
  char name[CHAT_INFO_LEN];
  char origname[CHAT_INFO_LEN];
  char topic[CHAT_INFO_LEN];
  char count[CHAT_COUNT_LEN];
  char games[CHAT_COUNT_LEN];
} chan_info[MAX_CHAT_CHANNELS];

int MainMultiplayerMenu() {
  knownDirectIpHosts.clear();
  DLLmprintf(0, "MainMultiplayerMenu.\n");
  DLLmprintf(0, "%d", DESCENT3_BLOCK_SIZE);

  DLLmprintf(0, "Inside MainMultiplayerMenu()\n");
  if (Bypass_chat && !ChatStarted) {
    int rcode1 = SearchMasterTrackerGameMenu();
    if (rcode1 == -1)
      rcode1 = 0;
    return rcode1;
  }

  DLLToggleUICallback(0);
  ChatStarted = 1;
  auto title_text = DLLCreateNewUITextItem(TXT_ODT_CONNECTING, UICOL_WINDOW_TITLE);

  auto cancel_on_text = DLLCreateNewUITextItem(TXT_ODT_CANCEL, UICOL_HOTSPOT_HI);
  auto cancel_off_text = DLLCreateNewUITextItem(TXT_ODT_CANCEL, UICOL_HOTSPOT_LO);

  auto exit_on_text = DLLCreateNewUITextItem(TXT_ODT_EXIT, UICOL_HOTSPOT_HI);
  auto exit_off_text = DLLCreateNewUITextItem(TXT_ODT_EXIT, UICOL_HOTSPOT_LO);

  auto priv_msg_on_text = DLLCreateNewUITextItem(TXT_ODT_SENDPRIV, UICOL_HOTSPOT_HI);
  auto priv_msg_off_text = DLLCreateNewUITextItem(TXT_ODT_SENDPRIV, UICOL_HOTSPOT_LO);

  auto join_lobby_on_text = DLLCreateNewUITextItem(TXT_ODT_JOINPRIV, UICOL_HOTSPOT_HI);
  auto join_lobby_off_text = DLLCreateNewUITextItem(TXT_ODT_JOINPRIV, UICOL_HOTSPOT_LO);

  auto join_chan_on_text = DLLCreateNewUITextItem(TXT_ODT_JOINCHAN, UICOL_HOTSPOT_HI);
  auto join_chan_off_text = DLLCreateNewUITextItem(TXT_ODT_JOINCHAN, UICOL_HOTSPOT_LO);

  auto find_pilot_on_text = DLLCreateNewUITextItem(TXT_ODT_FINDPILOT, UICOL_HOTSPOT_HI);
  auto find_pilot_off_text = DLLCreateNewUITextItem(TXT_ODT_FINDPILOT, UICOL_HOTSPOT_LO);

  auto game_on_text = DLLCreateNewUITextItem(TXT_ODT_JOINSTARTGAME, UICOL_HOTSPOT_HI);
  auto game_off_text = DLLCreateNewUITextItem(TXT_ODT_JOINSTARTGAME, UICOL_HOTSPOT_LO);

  auto send_on_text = DLLCreateNewUITextItem(TXT_ODT_SEND, UICOL_HOTSPOT_HI);
  auto send_off_text = DLLCreateNewUITextItem(TXT_ODT_SEND, UICOL_HOTSPOT_LO);

  char fmtlobbytext[200];
  snprintf(fmtlobbytext, sizeof(fmtlobbytext), TXT_ODT_YOUAREINLOBBY, Ourlobby);
  auto lobby_text = DLLCreateNewUITextItem(fmtlobbytext, UICOL_TEXT_AUX);

  auto blank_text = DLLCreateNewUITextItem(TXT_ODT_BLANK, UICOL_TEXT_AUX);
  char sendline[MAX_CHAT_SEND_LEN];
  int exit_menu = 0;
  int ret = 0;
  int res;
  const char *p;
  int i;
  float lastlisttime = 0;
  float lastchanlisttime = 0;
  void *chan_ti[MAX_CHAT_CHANNELS];
  void *user_ti[CHAT_MAX_USERLIST];
  char selpilot[MAX_CHAT_SEND_LEN];
  char oldselchan[200];
  for (i = 0; i < MAX_CHAT_CHANNELS; i++)
    chan_ti[i] = nullptr;
  for (i = 0; i < CHAT_MAX_USERLIST; i++)
    user_ti[i] = nullptr;

  uint8_t oldalpha = *DLLNewUIWindow_alpha;

  int colx1 = 0;
  int colx2 = 148;
  DLLSetScreenMode(SM_MENU);
  *DLLNewUIWindow_alpha = 255;

  for (i = 0; i < MAX_CHAT_CHANNELS; i++) {
    chan_info[i].origname[0] = '\0';
  }

  // Create our buttons
  auto main_wnd = DLLNewUIWindowCreate(0, 0, 640, 480, UIF_PROCESS_ALL);

  // void HotSpotCreate(int item,int parentitem, int id, int key, int txtitemoff, int txtitemon, int x, int y, int w,
  // int h, int flags,int winnum)
  /* auto exit_hs = */ DLLHotSpotCreate(main_wnd, 5, KEY_ESC, exit_off_text, exit_on_text, 33, 427, 70, 15, 0);

  auto priv_hs = DLLHotSpotCreate(main_wnd, 6, 0, priv_msg_off_text, priv_msg_on_text, 328, 403, 170, 15, 0);

#define JOIN_ROW 131
  auto join_lobby_hs =
      DLLHotSpotCreate(main_wnd, 7, 0, join_lobby_off_text, join_lobby_on_text, 400, JOIN_ROW, 170, 15, 0);
  auto join_chan_hs =
      DLLHotSpotCreate(main_wnd, 14, 0, join_chan_off_text, join_chan_on_text, 178, JOIN_ROW, 170, 15, 0);

  auto find_hs = DLLHotSpotCreate(main_wnd, 9, 0, find_pilot_off_text, find_pilot_on_text, 156, 403, 140, 15, 0);

  auto game_hs = DLLHotSpotCreate(main_wnd, 11, 0, game_off_text, game_on_text, 505, 431, 135, 15, 0);

  auto enter_hs = DLLHotSpotCreate(main_wnd, 15, KEY_ENTER, send_off_text, send_on_text, 550, 370, 70, 15, 0);

  // lobby_text
  auto lobby_txt_gadget = DLLTextCreate(main_wnd, lobby_text, 200, 450, UIF_CENTER);

  // User list box, id #10
  auto user_list = DLLOldListCreate(main_wnd, 10, colx1 + 20, 100, 108, 265, 0);
  // Channel list box, id #14
  auto chan_list = DLLOldListCreate(main_wnd, 14, colx2, 25, 470, 96, 0);

  // Edit box for typing in chat, id # 15
  auto send_edit = DLLOldEditCreate(main_wnd, 15, colx2 + 10, 372, 390, 20, UIED_AUTOSELECT);

  DLLNewUIWindowLoadBackgroundImage(main_wnd, "d3online_main.ogf");

  DLLSetOldEditBufferLen(send_edit, 100);
  auto console_item = DLLUIConsoleGadgetCreate(main_wnd, 16, colx2, 165, 0, 51, 16, 0);
  pconsole = console_item;

  DLLNewUIWindowOpen(main_wnd);

  // Menu loop
  Chat_command *cmd;
  int gotcancel = 0;
  int chat_connected = 0;
  float chat_conn_time = 0;
  char oldseluser[100];
  char chat_whois_info[PILOT_NAME_LEN + TRACKER_ID_LEN + 4];

  char pilot_name[PILOT_STRING_SIZE];
  CurrentPilotName(pilot_name);

  snprintf(chat_whois_info, sizeof(chat_whois_info), "%s %s", sztrackerid, pilot_name);
  chat_conn_time = DLLtimer_GetTime();

  DLLmprintf(0, "About to connect...\n");
  do {
    chat_connected = ConnectToChatServer(CHATTRACKERNAME, CHATPORT, pilot_name, chat_whois_info);
    p = GetChatText();
    if (p) {
      DLLUIConsoleGadgetputs(console_item, p);
      DLLUIConsoleGadgetputs(console_item, "\x0a\x0d");
    }
    if ((chat_connected == -1) || ((DLLtimer_GetTime() - chat_conn_time) > CONNECT_IRC_TIMEOUT)) {
      DLLDoMessageBox(TXT_ODT_MASTERTRACKER, TXT_ODT_CANTCONNECT, MSGBOX_OK, UICOL_WINDOW_TITLE, UICOL_TEXT_NORMAL);
      // Go ahead to the game screen.
      int rcode1 = SearchMasterTrackerGameMenu();
      if (rcode1 == -1)
        rcode1 = 0;
      ret = rcode1;
      exit_menu = 1;
      DLLCloseSplashScreen();
      DLLNewUIWindowClose(main_wnd);
      goto shutdownodip;
      break;
    }
    res = DLLPollUI();
    if (res == 99) {
      SearchMasterTrackerGameMenu();
      gotcancel = 1;
    }

  } while ((!gotcancel) && (chat_connected == 0));
  DLLCloseSplashScreen();

  if (gotcancel) {
    ret = 0;
    exit_menu = 1;
    DLLNewUIWindowClose(main_wnd);
    goto shutdownodip;
  }
  DLLmprintf(0, "Connected to chat server!\n");
  if (!JoinNewLobby("#descent")) {
    ret = 0;
    exit_menu = 1;
  }
  SendChatString("/list");

  while (!exit_menu) {

    // Handle commands...
    cmd = GetChatCommandFromQueue();
    if (cmd) {
      switch (cmd->command) {
      case CC_USER_JOINING:
      case CC_USER_LEAVING:
      case CC_KICKED:
      case CC_NICKCHANGED:
        // We will ignore all these things for now
        break;
      case CC_YOURCHANNEL: {
        strcpy(Ourlobby, cmd->data);
        char fmtlobbytext[200];
        char cleanlobby[50];
        memset(cleanlobby, 0, 29);
        strcpy(cleanlobby, Ourlobby + 1);
        for (int l = 0; l < 18; l++) {
          if (cleanlobby[l] == '_')
            cleanlobby[l] = ' ';
        }
        snprintf(fmtlobbytext, sizeof(fmtlobbytext), TXT_ODT_YOUAREINLOBBY, cleanlobby);

        DLLRemoveUITextItem(lobby_text);
        DLLGadgetDestroy(lobby_txt_gadget);
        DLLDeleteUIItem(lobby_txt_gadget);
        lobby_text = DLLCreateNewUITextItem(fmtlobbytext, UICOL_TEXT_AUX);
        lobby_txt_gadget = DLLTextCreate(main_wnd, lobby_text, 0, 450, UIF_CENTER);
        // Now update the text on the bottom of the screen
      } break;
      case CC_DISCONNECTED:
        // Display a message box letting the user know we are disconnected.
        DLLDoMessageBox(TXT_ODT_MASTERTRACKER, TXT_ODT_CHATDISCONNECTED, MSGBOX_OK, UICOL_WINDOW_TITLE,
                        UICOL_TEXT_NORMAL);
        DLLNewUIWindowClose(main_wnd);
        exit_menu = 1;
        break;
      default:
        DLLmprintf(0, "Warning: Received an unknown chat command!\n");
        break;
      }
    }
    if (exit_menu)
      break;
    p = GetChatText();
    if (p) {
      DLLUIConsoleGadgetputs(console_item, p);
      DLLUIConsoleGadgetputs(console_item, "\x0a\x0d");
    }

    // refresh the userlist
    if ((DLLtimer_GetTime() - lastlisttime) > CHAT_USER_REFRESH_TIME) {
      lastlisttime = DLLtimer_GetTime();
      p = GetChatUserList();
      if (p) {

        char *oldsel = DLLOldListGetItem(user_list, DLLOldListGetSelectedIndex(user_list));
        oldseluser[0] = 0;
        if (oldsel)
          strcpy(oldseluser, oldsel);
        void *old_ti = nullptr; 

        DLLOldListRemoveAll(user_list);
        for (i = 0; i < CHAT_MAX_USERLIST; i++) {
          if (user_ti[i] != nullptr)
            DLLRemoveUITextItem(user_ti[i]);
          user_ti[i] = nullptr;
        }
        char seps[] = " ";
        char *tokp;
        tokp = strtok((char *)p, seps);
        if (tokp) {
          for (i = 0; i < CHAT_MAX_USERLIST; i++) {
            user_ti[i] = DLLCreateNewUITextItem(tokp, GR_WHITE);
            DLLOldListAddItem(user_list, user_ti[i]);
            if (strcmp(tokp, oldseluser) == 0)
              old_ti = user_ti[i];
            tokp = strtok(nullptr, seps);
            if (!tokp)
              break;
          }
          if (old_ti)
            DLLOldListSelectItem(user_list, old_ti);
        }
      }
    }

    // refresh the channellist
    if ((DLLtimer_GetTime() - lastchanlisttime) > CHAT_LIST_REFRESH_TIME) {
      lastchanlisttime = DLLtimer_GetTime();
      SendChatString("/list");
    }
    p = GetChannelList();
    if (p) {
      char *pchanlist;
      pchanlist = (char *)DLLmem_malloc(strlen(p) + 1);
      memset(pchanlist, 0, strlen(p));
      strcpy(pchanlist, p);
      char *tokp = nullptr;
      char *nexttok;

      nexttok = strchr(pchanlist, '$');
      if (nexttok) {
        *nexttok = '\0';
        nexttok++;
        tokp = nexttok;
        if (nexttok) {
          nexttok = strchr(nexttok, '$');
          if (nexttok) {
            *nexttok = '\0';
            nexttok++;
          }
        }
      }

      if (tokp) {
        // Remove all previous items
        DLLmprintf(0, "Refreshing channel list\n");

        int selitem = DLLOldListGetSelectedIndex(chan_list);
        char *oldsel = nullptr;
        oldselchan[0] = 0;

        if ((selitem > 0) && (selitem < MAX_CHAT_CHANNELS)) {
          oldsel = chan_info[selitem].origname;
          strcpy(oldselchan, oldsel);
        }

        DLLOldListRemoveAll(chan_list);
        for (i = 0; i < MAX_CHAT_CHANNELS; i++) {
          if (chan_ti[i] != nullptr)
            DLLRemoveUITextItem(chan_ti[i]);
          chan_ti[i] = nullptr;
        }
        for (i = 0; i < MAX_CHAT_CHANNELS; i++) {
          char fmtchan[500];
          char *pcount = strchr(tokp, ' ');
          // if(pcount)
          {
            if (!pcount) {
              pcount = (char *)"";
            } else {
              // pcount++;
              *pcount = '\0';
            }

            // 17 is the magic number we want all channel names to align with
            memset(chan_info[i].name, 0, 19);
            strcpy(chan_info[i].origname, tokp);
            strcpy(chan_info[i].name, tokp + 1); // skip the #
            for (int l = 0; l < 18; l++) {
              if (chan_info[i].name[l] == '_')
                chan_info[i].name[l] = ' ';
              if (chan_info[i].name[l] == '\0')
                chan_info[i].name[l] = ' ';
            }
            pcount++;
            char *ptopic = pcount;
            while (isdigit(*ptopic)) {
              ptopic++;
            }
            *ptopic = '\0';
            ptopic++;
            // count
            strcpy(chan_info[i].count, pcount);
            // topic
            strcpy(chan_info[i].topic, ptopic);
            // Now we want it formatted nicely.....
            char chan_name[100];
            memset(chan_name, ' ', 99);
            chan_name[99] = '\0';
            char count_list[10];
            memset(count_list, ' ', 9);
            count_list[9] = '\0';

            memset(fmtchan, 0, 500);

            int charpos = 0;
            int endpos;
#define LIST_ROW1 90
#define LIST_ROW2 130

            strcpy(chan_name, chan_info[i].name);
            endpos = strlen(chan_info[i].name);
            chan_name[endpos] = ' ';

            strcpy(count_list, chan_info[i].count);
            endpos = strlen(chan_info[i].count);
            count_list[endpos] = ' ';

            while (DLLgrtext_GetTextLineWidth(fmtchan) < LIST_ROW1) {
              endpos = strlen(fmtchan);
              fmtchan[endpos] = chan_name[charpos];
              fmtchan[endpos + 1] = NULL;
              charpos++;
            }
            strcat(fmtchan, "\t\0");
            charpos = 0;
            while (DLLgrtext_GetTextLineWidth(fmtchan) < LIST_ROW2) {
              endpos = strlen(fmtchan);
              fmtchan[endpos] = count_list[charpos];
              fmtchan[endpos + 1] = NULL;
              charpos++;
            }
            strcat(fmtchan, "\t\0");

            strcat(fmtchan, chan_info[i].topic);
            chan_ti[i] = DLLCreateNewUITextItem(fmtchan, GR_WHITE);
            DLLOldListAddItem(chan_list, chan_ti[i]);
            if (strcmp(oldselchan, chan_info[i].origname) == 0) {
              DLLOldListSelectItem(chan_list, chan_ti[i]);
            }
          }
          tokp = nexttok;
          if (nexttok) {
            nexttok = strchr(nexttok, '$');
            if (nexttok) {
              *nexttok = '\0';
              nexttok++;
            }
          }
          if (!tokp)
            break;
        }
      }
      DLLmem_free(pchanlist);
    }

    res = DLLPollUI();
    // handle all UI results.
    switch (res) {
    case -1:
      break;
    case 3:
      // Join Mastertracker game
      {
        DLLmprintf(0, "Sending Mastertracker game list request.\n");
        RequestDIPGameList();
        DLLmprintf(0, "Calling SearchMasterTrackerGameMenu().\n");
        DLLNewUIWindowClose(main_wnd);
        *DLLGame_is_master_tracker_game = 0;
        int gamestart = SearchMasterTrackerGameMenu();
        if (gamestart == 1) {
          exit_menu = 1;
          ret = 1;
        } else if (gamestart == 0) {
          DLLNewUIWindowOpen(main_wnd);
        } else if (gamestart == -1) {
          exit_menu = 1;
          ret = 0;
        }
        break;
      }

    case 4:
      // Start Mastertracker game
      //  Start a netgame
      DLLNewUIWindowClose(main_wnd);
      *DLLGame_is_master_tracker_game = 0;
      if (StartMultiplayerGameMenu()) {
        exit_menu = 1;
        ret = 1;
      } else {
        DLLNewUIWindowOpen(main_wnd);
      }
      break;
    case 5:
      DLLNewUIWindowClose(main_wnd);
      exit_menu = 1;
      ret = 0;
      break;
    case 6:
      // Send a private message
      // Create new dialog box, prompt for user and message, then send message
      DLLmprintf(0, "Sending private Message\n");
      // Get the currently selected pilot name
      strcpy(selpilot, DLLOldListGetItem(user_list, DLLOldListGetSelectedIndex(user_list)));
      SendWhisper(selpilot);
      break;
    case 7:
      // Join a private channel
      if (!JoinPrivateLobby()) {
        ret = 0;
        exit_menu = 0;
      }
      break;
    case 8:
      // Join a channel
      // unused
      break;
    case 9:
      // Find a pilot
      DLLmprintf(0, "Finding a pilot\n");
      FindPilot();
      break;
    case 11: {
      DLLmprintf(0, "Calling SearchMasterTrackerGameMenu().\n");
      DLLNewUIWindowClose(main_wnd);
      *DLLGame_is_master_tracker_game = 0;
      int gamestart = SearchMasterTrackerGameMenu();
      if (gamestart == 1) {
        exit_menu = 1;
        ret = 1;
      } else if (gamestart == 0) {
        DLLNewUIWindowOpen(main_wnd);
      } else if (gamestart == -1) {
        exit_menu = 1;
        ret = 0;
      }
      break;
    } break;
    case 14:
      // Channel list box
      DLLmprintf(0, "Joining new public channel\n");
      {
        int selitem = 0;
        DLLmprintf(0, "Joining new public channel\n");
        selitem = DLLOldListGetSelectedIndex(chan_list);
        if (!JoinNewLobby(chan_info[selitem].origname)) {
          ret = 0;
          exit_menu = 0;
        } else {
          char txtmessage[200];
          char cleanlobby[50];
          memset(cleanlobby, 0, 29);
          strcpy(cleanlobby, chan_info[selitem].origname + 1);
          for (int l = 0; l < 18; l++) {
            if (cleanlobby[l] == '_')
              cleanlobby[l] = ' ';
          }
          snprintf(txtmessage, sizeof(txtmessage), TXT_ODT_INNEWLOBBY, cleanlobby);
          DLLDoMessageBox(TXT_ODT_MASTERTRACKER, txtmessage, MSGBOX_OK, UICOL_WINDOW_TITLE, UICOL_TEXT_NORMAL);
        }
      }

      break;
    case 15:
      // Send a message
      DLLOldEditGetText(send_edit, sendline, MAX_CHAT_SEND_LEN - 1);
      if (*sendline == '\0') {
        break;
      }
      DLLOldEditSetText(send_edit, "");
      p = SendChatString(sendline);
      if (p) {
        DLLUIConsoleGadgetputs(console_item, p);
        DLLUIConsoleGadgetputs(console_item, "\x0a\x0d");
      }
      DLLNewUIWindowSetFocusOnEditGadget(send_edit, main_wnd);
      break;
    }
  }
shutdownodip:

  DLLNewUIWindowDestroy(main_wnd);
  DLLOldListRemoveAll(chan_list);
  DLLOldListRemoveAll(user_list);
  for (i = 0; i < MAX_CHAT_CHANNELS; i++) {
    if (chan_ti[i] != nullptr)
      DLLRemoveUITextItem(chan_ti[i]);
    chan_ti[i] = nullptr;
  }
  for (i = 0; i < CHAT_MAX_USERLIST; i++) {
    if (user_ti[i] != nullptr)
      DLLRemoveUITextItem(user_ti[i]);
    user_ti[i] = nullptr;
  }
  *DLLNewUIWindow_alpha = oldalpha;
  DLLmprintf(0, "Disconnecting from Online Direct TCP-IP IRC.\n");
  DisconnectFromChatServer();
  ChatStarted = 0;
  DLLmprintf(0, "Disconnected.\n");
  DLLToggleUICallback(1);

  // Delete all those items we created
  DLLDeleteUIItem(main_wnd);
  DLLDeleteUIItem(priv_hs);
  DLLDeleteUIItem(join_lobby_hs);
  DLLDeleteUIItem(join_chan_hs);
  DLLDeleteUIItem(find_hs);

  DLLDeleteUIItem(game_hs);
  DLLDeleteUIItem(enter_hs);
  DLLDeleteUIItem(user_list);
  DLLDeleteUIItem(chan_list);
  DLLDeleteUIItem(send_edit);
  DLLDeleteUIItem(console_item);
  DLLDeleteUIItem(lobby_txt_gadget);

  DLLRemoveUITextItem(title_text);
  DLLRemoveUITextItem(lobby_text);
  DLLRemoveUITextItem(cancel_on_text);
  DLLRemoveUITextItem(cancel_off_text);
  DLLRemoveUITextItem(exit_on_text);
  DLLRemoveUITextItem(exit_off_text);
  DLLRemoveUITextItem(priv_msg_on_text);
  DLLRemoveUITextItem(priv_msg_off_text);
  DLLRemoveUITextItem(join_lobby_on_text);
  DLLRemoveUITextItem(join_lobby_off_text);
  DLLRemoveUITextItem(join_chan_on_text);
  DLLRemoveUITextItem(join_chan_off_text);
  DLLRemoveUITextItem(find_pilot_on_text);
  DLLRemoveUITextItem(find_pilot_off_text);
  DLLRemoveUITextItem(game_on_text);
  DLLRemoveUITextItem(game_off_text);
  DLLRemoveUITextItem(blank_text);
  DLLRemoveUITextItem(send_on_text);
  DLLRemoveUITextItem(send_off_text);
  return ret;
}

#define SORT_GAMENAME 30
#define SORT_GAMETYPE 31
#define SORT_MISSION 32
#define SORT_LEVEL 33
#define SORT_PLAYERS 34
#define SORT_PING 35

int sort_type;

bool invert_sort_gamename = true;
bool invert_sort_gametype = true;
bool invert_sort_mission = true;
bool invert_sort_levels = true;
bool invert_sort_players = true;
bool invert_sort_ping = true;

int net_game_compare(const void *arg1, const void *arg2) {

  network_game *net1 = (network_game *)arg1;
  network_game *net2 = (network_game *)arg2;

  if ((!arg1) || (!arg2))
    return 0;

  switch (sort_type) {
  case SORT_GAMENAME: {
    return stricmp(net1->name, net2->name);
  } break;
  case SORT_GAMETYPE: {
    return stricmp(net1->scriptname, net2->scriptname);
  } break;
  case SORT_MISSION: {
    return stricmp(net1->mission_name, net2->mission_name);
  } break;
  case SORT_LEVEL: {
    if (net1->level_num == net2->level_num)
      return 0;
    else
      return (net1->level_num - net2->level_num);
  } break;
  case SORT_PLAYERS: {
    if (net1->curr_num_players == net2->curr_num_players)
      return 0;
    else
      return (net1->curr_num_players - net2->curr_num_players);
  } break;
  case SORT_PING: {
    if (net1->server_response_time == net2->server_response_time)
      return 0;
    else
      return (net1->server_response_time < net2->server_response_time) ? 1 : -1;
  } break;
  }
  return 0;
}

void ResortGameList(void *lb, int type, bool invert) {

  if (!*DLLNum_network_games_known) {
    return;
  }
  // Make a copy of the games list
  network_game *net_game_copy;
  net_game_copy = (network_game *)DLLmem_malloc(sizeof(network_game) * (*DLLNum_network_games_known));
  memcpy(net_game_copy, &DLLNetwork_games[0], sizeof(network_game) * (*DLLNum_network_games_known));

  // Set the sort type
  sort_type = type;

  // Do the actual sort
  qsort(net_game_copy, *DLLNum_network_games_known, sizeof(network_game), net_game_compare);

  // Now populate the list in the order sorted...
  NextGameItemNo = 0;
  DLLListRemoveAll(lb);
  int i;
  if (!invert) {
    for (i = 0; i < *DLLNum_network_games_known; i++) {
      int gameserver_item = GetGameServerItemByHandle(net_game_copy[i].handle);
      if (gameserver_item != -1) {
        Gamelist[gameserver_item].lb_no = NextGameItemNo;
        NextGameItemNo++;
        DLLListAddItem(lb, Gamelist[gameserver_item].ti);
      }
    }
  } else {
    for (i = *DLLNum_network_games_known; i >= 0; i--) {
      int gameserver_item = GetGameServerItemByHandle(net_game_copy[i].handle);
      if (gameserver_item != -1) {
        Gamelist[gameserver_item].lb_no = NextGameItemNo;
        NextGameItemNo++;
        DLLListAddItem(lb, Gamelist[gameserver_item].ti);
      }
    }
  }

  DLLmem_free(net_game_copy);
}

#define REQ_GAME_MASTER_INTERVAL 30
#define REQ_GAME_LIST_INTERVAL 8

#define GET_INFO_ID 50

int SearchMasterTrackerGameMenu() {
  DLLmprintf(0, "SearchMasterTrackerGameMenu().\n");
  int exit_menu = 0;
  int cury = 40;
  int ret = 0;
  int res;

  int i = 0;
  float last_req_time;
  float last_master_req_time;
  char selgame[200];
  auto return_text_on = DLLCreateNewUITextItem(TXT_ODT_RETURNTOCHAT, UICOL_HOTSPOT_HI);
  auto return_text_off = DLLCreateNewUITextItem(TXT_ODT_RETURNTOCHAT, UICOL_HOTSPOT_LO);
  auto game_head_text = DLLCreateNewUITextItem(TXT_ODT_GAMELISTHDR, UICOL_TEXT_NORMAL);
  auto exit_on_text = DLLCreateNewUITextItem(TXT_ODT_EXIT, UICOL_HOTSPOT_HI);
  auto exit_off_text = DLLCreateNewUITextItem(TXT_ODT_EXIT, UICOL_HOTSPOT_LO);
  auto join_on_text = DLLCreateNewUITextItem(TXT_ODT_JOINSEL, UICOL_HOTSPOT_HI);
  auto join_off_text = DLLCreateNewUITextItem(TXT_ODT_JOINSEL, UICOL_HOTSPOT_LO);
  auto start_on_text = DLLCreateNewUITextItem(TXT_ODT_STARTNEW, UICOL_HOTSPOT_HI);
  auto start_off_text = DLLCreateNewUITextItem(TXT_ODT_STARTNEW, UICOL_HOTSPOT_LO);

  auto gname_on_text = DLLCreateNewUITextItem(TXT_ODT_GAMENAME2, UICOL_HOTSPOT_HI);
  auto gname_off_text = DLLCreateNewUITextItem(TXT_ODT_GAMENAME2, UICOL_HOTSPOT_LO);
  auto gtype_on_text = DLLCreateNewUITextItem(TXT_ODT_GAMETYPE, UICOL_HOTSPOT_HI);
  auto gtype_off_text = DLLCreateNewUITextItem(TXT_ODT_GAMETYPE, UICOL_HOTSPOT_LO);
  auto msn_on_text = DLLCreateNewUITextItem(TXT_ODT_MISSION, UICOL_HOTSPOT_HI);
  auto msn_off_text = DLLCreateNewUITextItem(TXT_ODT_MISSION, UICOL_HOTSPOT_LO);
  auto lvl_on_text = DLLCreateNewUITextItem(TXT_ODT_LEVEL, UICOL_HOTSPOT_HI);
  auto lvl_off_text = DLLCreateNewUITextItem(TXT_ODT_LEVEL, UICOL_HOTSPOT_LO);
  auto plrs_on_text = DLLCreateNewUITextItem(TXT_ODT_PLAYERS, UICOL_HOTSPOT_HI);
  auto plrs_off_text = DLLCreateNewUITextItem(TXT_ODT_PLAYERS, UICOL_HOTSPOT_LO);
  auto ping_on_text = DLLCreateNewUITextItem(TXT_ODT_PING, UICOL_HOTSPOT_HI);
  auto ping_off_text = DLLCreateNewUITextItem(TXT_ODT_PING, UICOL_HOTSPOT_LO);

  char fmt_textstr[200];
  snprintf(fmt_textstr, sizeof(fmt_textstr), TXT_ODT_MAKEGAMEDEFAULT, 28);
  auto default_on_text = DLLCreateNewUITextItem(fmt_textstr, UICOL_HOTSPOT_HI);
  snprintf(fmt_textstr, sizeof(fmt_textstr), TXT_ODT_MAKEGAMEDEFAULT, 28);

  auto default_off_text = DLLCreateNewUITextItem(fmt_textstr, UICOL_HOTSPOT_LO);
  auto game_hdr_text = DLLCreateNewUITextItem(TXT_ODT_GAME_HDR, UICOL_WINDOW_TITLE);

  auto info_on_text = DLLCreateNewUITextItem("", UICOL_HOTSPOT_HI);
  auto info_off_text = DLLCreateNewUITextItem("", UICOL_HOTSPOT_LO);

  memset(Gamelist, 0, sizeof(Gamelist));
  NextGameItemNo = 0;

  UIObject *set_dft_hs;

  void *net_game_txt_items[MAX_NET_GAMES];
  int a;
  bool hardexit = false;
  for (a = 0; a < MAX_NET_GAMES; a++)
    net_game_txt_items[a] = nullptr;
  uint8_t oldalpha = *DLLNewUIWindow_alpha;

  DLLSetScreenMode(SM_MENU);
  *DLLNewUIWindow_alpha = 255;

  auto main_wnd = DLLNewUIWindowCreate(0, 0, 640, 480, UIF_PROCESS_ALL);

  auto screen_header = DLLTextCreate(main_wnd, game_hdr_text, 5, 15, UIF_CENTER);
  cury += 30;
  auto start_hs = DLLHotSpotCreate(main_wnd, 7, KEY_S, start_off_text, start_on_text, 320, cury, 150, 15, UIF_CENTER);
  cury += 30;

  int setdfty;

  auto return_hs = DLLHotSpotCreate(main_wnd, 8, KEY_R, return_text_off, return_text_on, 490, cury, 250, 15, UIF_CENTER);
  cury += 20;

  if (Bypass_chat) {

    snprintf(fmt_textstr, sizeof(fmt_textstr), TXT_ODT_MAKEGAMEDEFAULT, 29);
    default_on_text = DLLCreateNewUITextItem(fmt_textstr, UICOL_HOTSPOT_HI);
    snprintf(fmt_textstr, sizeof(fmt_textstr), TXT_ODT_MAKEGAMEDEFAULT, 29);
    default_off_text = DLLCreateNewUITextItem(fmt_textstr, UICOL_HOTSPOT_LO);
    setdfty = cury;
    set_dft_hs = DLLHotSpotCreate(main_wnd, 9, 0, default_off_text, default_on_text, 490, cury, 300, 15, UIF_CENTER);
    cury += 20;
  } else {
    snprintf(fmt_textstr, sizeof(fmt_textstr), TXT_ODT_MAKEGAMEDEFAULT, 28);
    default_on_text = DLLCreateNewUITextItem(fmt_textstr, UICOL_HOTSPOT_HI);
    snprintf(fmt_textstr, sizeof(fmt_textstr), TXT_ODT_MAKEGAMEDEFAULT, 28);
    default_off_text = DLLCreateNewUITextItem(fmt_textstr, UICOL_HOTSPOT_LO);
    setdfty = cury;
    set_dft_hs = DLLHotSpotCreate(main_wnd, 9, 0, default_off_text, default_on_text, 490, cury, 300, 15, UIF_CENTER);
    cury += 30;
  }

  auto gname_hs = DLLHotSpotCreate(main_wnd, SORT_GAMENAME, 0, gname_off_text, gname_on_text, 44, cury,
                                   DLLgrtext_GetTextLineWidth(TXT_ODT_GAMENAME2) + 1, 15, 0);
  auto gtype_hs = DLLHotSpotCreate(main_wnd, SORT_GAMETYPE, 0, gtype_off_text, gtype_on_text, 215, cury,
                                   DLLgrtext_GetTextLineWidth(TXT_ODT_GAMETYPE) + 1, 15, 0);
  auto msn_hs = DLLHotSpotCreate(main_wnd, SORT_MISSION, 0, msn_off_text, msn_on_text, 320, cury,
                                 DLLgrtext_GetTextLineWidth(TXT_ODT_MISSION) + 1, 15, 0);
  auto lvl_hs = DLLHotSpotCreate(main_wnd, SORT_LEVEL, 0, lvl_off_text, lvl_on_text, 428, cury,
                                 DLLgrtext_GetTextLineWidth(TXT_ODT_LEVEL) + 1, 15, 0);
  auto players_hs = DLLHotSpotCreate(main_wnd, SORT_PLAYERS, 0, plrs_off_text, plrs_on_text, 479, cury,
                                     DLLgrtext_GetTextLineWidth(TXT_ODT_PLAYERS) + 1, 15, 0);
  auto ping_hs = DLLHotSpotCreate(main_wnd, SORT_PING, 0, ping_off_text, ping_on_text, 549, cury,
                                  DLLgrtext_GetTextLineWidth(TXT_ODT_PING) + 1, 15, 0);

  cury += 15;
  auto game_list = DLLListCreate(main_wnd, 6, 10, cury, 600, 230, UIF_CENTER | UILB_NOSORT);
  cury += 260;
  int curyButtomMenu = cury;

  auto join_hs =
      DLLHotSpotCreate(main_wnd, 6, KEY_ENTER, join_off_text, join_on_text, 100, curyButtomMenu, 130, 15, 0);
  auto exit_hs = DLLHotSpotCreate(main_wnd, 5, KEY_ESC, exit_off_text, exit_on_text, 400, curyButtomMenu, 70, 15, 0);

  auto info_hs = DLLHotSpotCreate(main_wnd, GET_INFO_ID, KEY_I, info_off_text, info_on_text, 1, 1, 1, 1, 0);

  DLLNewUIWindowLoadBackgroundImage(main_wnd, "d3online_game.ogf");

  DLLNewUIWindowOpen(main_wnd);
  *DLLNum_network_games_known = 0;

  int itemp;
  last_req_time = DLLtimer_GetTime();
  last_master_req_time = DLLtimer_GetTime();

  // request master
  RequestDIPGameList();
  *DLLNum_network_games_known = 0;
  DLLmprintf(0, "Waiting for Mastertracker response.\n");

  auto ui_directip_counter = DLLCreateNewUITextItem("0 Server", GR_LIGHTGRAY);
  auto directip_counter = DLLTextCreate(main_wnd, ui_directip_counter, 290, curyButtomMenu, 130);

  int currentNumServers = 0;
  // Menu loop
  while (!exit_menu) {
    if (ChatStarted)
      GetChatText();

    DLLDescentDefer();

    res = DLLPollUI();

    std::queue<apiServerEntry> dIpHostList = GetDIpGameList();
    while (!dIpHostList.empty()) {
      apiServerEntry entry = dIpHostList.front();
      dIpHostList.pop();

      bool isKnown = false;
      for (apiServerEntry &entryKnown : knownDirectIpHosts) {
        if (entryKnown.ipv4adr == entry.ipv4adr && entryKnown.port == entry.port) {
          isKnown = true;
          break;
        }
      }

      if (!isKnown) {
        knownDirectIpHosts.push_back(entry);

        // update new server
        DLLSearchForLocalGamesTCP(entry.ipv4adr, entry.port);
      }
    }

    // count active server
    int activeDirectIp = 0;
    int j;
    for (j = 0; j < MAX_GAMELIST_ITEMS; j++) {
      if (Gamelist[j].used && (Gamelist[j].handle != -1)) {
        int gameid = GetGameByHandle(Gamelist[j].handle);
        if (gameid != -1) {
          activeDirectIp++;
        }
      }
    }

    // print num active server
    if (currentNumServers != activeDirectIp) {
      char numDirectIpServers[500];
      sprintf(numDirectIpServers, "%d Server", activeDirectIp);

      DLLRemoveUITextItem(ui_directip_counter);
      DLLGadgetDestroy(directip_counter);
      DLLDeleteUIItem(directip_counter);

      ui_directip_counter = DLLCreateNewUITextItem(numDirectIpServers, GR_LIGHTGRAY);
      directip_counter = DLLTextCreate(main_wnd, ui_directip_counter, 290, curyButtomMenu, 130);

      currentNumServers = activeDirectIp;
    }

    // request master
    if ((DLLtimer_GetTime() - last_master_req_time) > REQ_GAME_MASTER_INTERVAL) {
      last_master_req_time = DLLtimer_GetTime();
      DLLmprintf(0, "request master\n");

      // re request master
      RequestDIPGameList();
    }

    if ((DLLtimer_GetTime() - last_req_time) > REQ_GAME_LIST_INTERVAL) {
      last_req_time = DLLtimer_GetTime();

      // update existing servers
      for (apiServerEntry &directIpHost : knownDirectIpHosts) {
        DLLmprintf(0, "send directip query to: %d %d\n", directIpHost.ipv4adr, directIpHost.port);
        DLLSearchForLocalGamesTCP(directIpHost.ipv4adr, directIpHost.port);
      }

      *DLLMulti_Gamelist_changed = true;
      int selno = DLLListGetSelectedIndex(game_list);
      if (selno >= 0) {
        strcpy(selgame, DLLNetwork_games[selno].name);
      } else {
        selgame[0] = '\0';
      }
    }
    DLLUpdateAndPackGameList();

    if ((itemp = DLLSearchForLocalGamesTCP(0, 0)) || *DLLMulti_Gamelist_changed) {
      UpdateGamelist(game_list);
    }

    if (res == -1) {
      continue;
    }
    // handle all UI results.
    switch (res) {

    case 5:
      // Hard exit. Don't return to chat if in bypass chat mode
      hardexit = true;
      DLLNewUIWindowClose(main_wnd);
      exit_menu = 1;
      break;
    case 6:
      // Double click on listbox, or join selected hit.
      if (*DLLNum_network_games_known) {
        // Get the appropriate game address
        int gameno = DLLListGetSelectedIndex(game_list);
        int gameid = GetGameByLBNo(gameno);

        if (gameid == -1)
          break;
        gameno = gameid;

        DLLmprintf(0, "Selected item is %s\n", DLLNetwork_games[gameno].name);

        network_address s_address;
        s_address.connection_type = NP_TCP;
        memcpy(&s_address.address, &DLLNetwork_games[gameno].addr, sizeof(network_address));
        s_address.port = DLLNetwork_games[gameno].addr.port;
        *DLLGame_is_master_tracker_game = 0;
        DLLMultiStartClient(nullptr);

        if (DLLDoPlayerMouselookCheck(DLLNetwork_games[gameno].flags)) {
          // char script_file[500];
          // snprintf(script_file, sizeof(script_file), "%s.d3m", DLLNetwork_games[gameno].scriptname);
          // if (DLLCheckGetD3M(script_file)) {
          //  Check to see if this mission exists!
          if (DLLmsn_CheckGetMission(&s_address, DLLNetwork_games[gameno].mission)) {
            if ((DLLTryToJoinServer(&s_address))) {
              DLLmprintf(0, "Menu: Game joined!\n");
              DLLNewUIWindowClose(main_wnd);
              exit_menu = 1;
              ret = 1;
            } else {
              DLLNewUIWindowClose(main_wnd);
              DLLNewUIWindowOpen(main_wnd);
            }
          }
          //}
        }
      } else {
        DLLListRemoveAll(game_list);
        last_req_time = DLLtimer_GetTime() - (REQ_GAME_LIST_INTERVAL * 2);
        DLLDoMessageBox(TXT_ODT_ERROR, TXT_ODT_NO_GAMES, MSGBOX_OK, UICOL_WINDOW_TITLE, UICOL_TEXT_NORMAL);
      }
      break;
    case 7:
      // Start a new game
      DLLNewUIWindowClose(main_wnd);
      *DLLGame_is_master_tracker_game = 0;
      if (StartMultiplayerGameMenu()) {
        exit_menu = 1;
        ret = 1;
      } else {
        DLLNewUIWindowOpen(main_wnd);
      }
      break;
    case 8:
      // We just want to return to chat, not totally exit
      DLLNewUIWindowClose(main_wnd);
      exit_menu = 1;
      ret = 0;
      break;
    case 9:
      //	Change the current selection
      {
        int bypass_chat_curr_val;
        DLLDatabaseReadInt(BYPASSCHATDBKEY, &bypass_chat_curr_val);

        DLLRemoveUITextItem(default_on_text);
        DLLRemoveUITextItem(default_off_text);
        DLLGadgetDestroy(set_dft_hs);
        DLLDeleteUIItem(set_dft_hs);
        if (bypass_chat_curr_val) {
          bypass_chat_curr_val = 0;
          DLLDatabaseWriteInt(BYPASSCHATDBKEY, bypass_chat_curr_val);
          snprintf(fmt_textstr, sizeof(fmt_textstr), TXT_ODT_MAKEGAMEDEFAULT, 28);
          default_on_text = DLLCreateNewUITextItem(fmt_textstr, UICOL_HOTSPOT_HI);
          snprintf(fmt_textstr, sizeof(fmt_textstr), TXT_ODT_MAKEGAMEDEFAULT, 28);
          default_off_text = DLLCreateNewUITextItem(fmt_textstr, UICOL_HOTSPOT_LO);
          set_dft_hs =
              DLLHotSpotCreate(main_wnd, 9, 0, default_off_text, default_on_text, 490, setdfty, 300, 15, UIF_CENTER);
        } else {
          bypass_chat_curr_val = 1;
          DLLDatabaseWriteInt(BYPASSCHATDBKEY, bypass_chat_curr_val);
          snprintf(fmt_textstr, sizeof(fmt_textstr), TXT_ODT_MAKEGAMEDEFAULT, 29);
          default_on_text = DLLCreateNewUITextItem(fmt_textstr, UICOL_HOTSPOT_HI);
          snprintf(fmt_textstr, sizeof(fmt_textstr), TXT_ODT_MAKEGAMEDEFAULT, 29);
          default_off_text = DLLCreateNewUITextItem(fmt_textstr, UICOL_HOTSPOT_LO);
          set_dft_hs =
              DLLHotSpotCreate(main_wnd, 9, 0, default_off_text, default_on_text, 490, setdfty, 300, 15, UIF_CENTER);
        }
      }
      break;
    case GET_INFO_ID: {
      if (*DLLNum_network_games_known) {
        int gameno;
        gameno = DLLListGetSelectedIndex(game_list);
        int gameid = GetGameByLBNo(gameno);
        gameno = gameid;
        if (gameid == -1)
          break;
        DLLmprintf(0, "Selected item is %s\n", DLLNetwork_games[gameno].name);
        DLLShowNetgameInfo(&DLLNetwork_games[gameno]);
      }
    } break;

    case SORT_GAMENAME: {
      DLLmprintf(0, "Sorting by game name.\n");
      invert_sort_gamename = !invert_sort_gamename;
      ResortGameList(game_list, res, invert_sort_gamename);
    } break;
    case SORT_GAMETYPE: {
      DLLmprintf(0, "Sorting by game type.\n");
      invert_sort_gametype = !invert_sort_gametype;
      ResortGameList(game_list, res, invert_sort_gametype);
    } break;
    case SORT_MISSION: {
      DLLmprintf(0, "Sorting by mission.\n");
      invert_sort_mission = !invert_sort_mission;
      ResortGameList(game_list, res, invert_sort_mission);
    } break;
    case SORT_LEVEL: {
      DLLmprintf(0, "Sorting by level.\n");
      invert_sort_levels = !invert_sort_levels;
      ResortGameList(game_list, res, invert_sort_levels);
    } break;
    case SORT_PLAYERS: {
      DLLmprintf(0, "Sorting by players.\n");
      invert_sort_players = !invert_sort_players;
      ResortGameList(game_list, res, invert_sort_players);
    } break;
    case SORT_PING: {
      DLLmprintf(0, "Sorting by ping.\n");
      invert_sort_ping = !invert_sort_ping;
      ResortGameList(game_list, res, invert_sort_ping);
    } break;
    }
  }
  DLLNewUIWindowDestroy(main_wnd);

  *DLLNewUIWindow_alpha = oldalpha;

  // Look for items that we need to remove
  for (i = 0; i < MAX_GAMELIST_ITEMS; i++) {
    if (Gamelist[i].used) {
      DLLRemoveUITextItem(Gamelist[i].ti);
      Gamelist[i].used = false;
    }
  }

  // Clean up
  DLLDeleteUIItem(main_wnd);
  DLLDeleteUIItem(game_list);
  DLLDeleteUIItem(exit_hs);
  DLLDeleteUIItem(join_hs);
  DLLDeleteUIItem(start_hs);
  DLLDeleteUIItem(screen_header);
  DLLDeleteUIItem(set_dft_hs);
  DLLDeleteUIItem(return_hs);
  DLLDeleteUIItem(info_hs);
  DLLDeleteUIItem(gname_hs);
  DLLDeleteUIItem(gtype_hs);
  DLLDeleteUIItem(msn_hs);
  DLLDeleteUIItem(lvl_hs);
  DLLDeleteUIItem(players_hs);
  DLLDeleteUIItem(ping_hs);

  DLLRemoveUITextItem(info_on_text);
  DLLRemoveUITextItem(info_off_text);
  DLLRemoveUITextItem(game_hdr_text);
  DLLRemoveUITextItem(return_text_on);
  DLLRemoveUITextItem(return_text_off);
  DLLRemoveUITextItem(game_head_text);
  DLLRemoveUITextItem(exit_on_text);
  DLLRemoveUITextItem(exit_off_text);
  DLLRemoveUITextItem(join_on_text);
  DLLRemoveUITextItem(join_off_text);
  DLLRemoveUITextItem(start_on_text);
  DLLRemoveUITextItem(start_off_text);
  DLLRemoveUITextItem(default_on_text);
  DLLRemoveUITextItem(default_off_text);

  DLLDeleteUIItem(directip_counter);
  DLLRemoveUITextItem(ui_directip_counter);

  DLLRemoveUITextItem(gname_on_text);
  DLLRemoveUITextItem(gname_off_text);
  DLLRemoveUITextItem(gtype_on_text);
  DLLRemoveUITextItem(gtype_off_text);
  DLLRemoveUITextItem(msn_on_text);
  DLLRemoveUITextItem(msn_off_text);
  DLLRemoveUITextItem(lvl_on_text);
  DLLRemoveUITextItem(lvl_off_text);
  DLLRemoveUITextItem(plrs_on_text);
  DLLRemoveUITextItem(plrs_off_text);
  DLLRemoveUITextItem(ping_on_text);
  DLLRemoveUITextItem(ping_off_text);

  if (hardexit)
    return -1;

  if (!ret && !ChatStarted && Bypass_chat) {
    ChatStarted = 1;
    ret = MainMultiplayerMenu();
  }

  return ret;
}

int JoinNewLobby(const char *lobby) {
  int rcode;
  const char *p;
  DLLmprintf(0, "Entering new lobby");
  auto title_text = DLLCreateNewUITextItem(TXT_ODT_ENTERINGLOBBY, UICOL_WINDOW_TITLE);

  auto cancel_on_text = DLLCreateNewUITextItem(TXT_ODT_CANCEL, UICOL_HOTSPOT_HI);
  auto cancel_off_text = DLLCreateNewUITextItem(TXT_ODT_CANCEL, UICOL_HOTSPOT_LO);

  auto main_wnd = DLLNewUIGameWindowCreate(0, 256, 128, 128, UIF_CENTER | UIF_PROCESS_ALL | NUWF_TITLEMED);
  auto title = DLLTextCreate(main_wnd, title_text, 0, 7, UIF_CENTER);
  auto cancel_hs =
      DLLHotSpotCreate(main_wnd, 5, KEY_ESC, cancel_off_text, cancel_on_text, 20, 100, 70, 15, UIF_CENTER);
  DLLNewUIGameWindowOpen(main_wnd);

  do {
    rcode = SetNewChatChannel(lobby);
    p = GetChatText();
    if (p && pconsole) {
      DLLUIConsoleGadgetputs(pconsole, p);
      DLLUIConsoleGadgetputs(pconsole, "\x0a\x0d");
    }
  } while (rcode == 0);

  DLLNewUIGameWindowClose(main_wnd);
  DLLNewUIGameWindowDestroy(main_wnd);

  if (rcode == 1) {
    return 1;
  } else {
    DLLmprintf(0, "Unable to join lobby: %d\n", rcode);
    char txtmessage[200];
    char cleanlobby[50];
    memset(cleanlobby, 0, 29);
    strcpy(cleanlobby, lobby + 1);
    for (int l = 0; l < 18; l++) {
      if (cleanlobby[l] == '_')
        cleanlobby[l] = ' ';
    }
    snprintf(txtmessage, sizeof(txtmessage), TXT_ODT_CANTJOINLOBBY, cleanlobby);
    DLLDoMessageBox(TXT_ODT_MASTERTRACKER, txtmessage, MSGBOX_OK, UICOL_WINDOW_TITLE, UICOL_TEXT_NORMAL);

    return 0;
  }

  DLLDeleteUIItem(main_wnd);
  DLLDeleteUIItem(title);
  DLLDeleteUIItem(cancel_hs);

  DLLRemoveUITextItem(title_text);
  DLLRemoveUITextItem(cancel_on_text);
  DLLRemoveUITextItem(cancel_off_text);
}

const char *SendWhisper(const char *name) {

  int exit_menu = 0;
  const char *p;
  char message[MAX_CHAT_SEND_LEN];
  char pilot_name[MAX_CHAT_SEND_LEN];
  static char fmt_msg[MAX_CHAT_SEND_LEN * 2];
  auto title_text = DLLCreateNewUITextItem(TXT_ODT_PRIVATEMESSAGE, UICOL_WINDOW_TITLE);

  auto cancel_on_text = DLLCreateNewUITextItem(TXT_ODT_CANCEL, UICOL_HOTSPOT_HI);
  auto cancel_off_text = DLLCreateNewUITextItem(TXT_ODT_CANCEL, UICOL_HOTSPOT_LO);
  auto send_on_text = DLLCreateNewUITextItem(TXT_ODT_SEND, UICOL_HOTSPOT_HI);
  auto send_off_text = DLLCreateNewUITextItem(TXT_ODT_SEND, UICOL_HOTSPOT_LO);

  auto pilot_text = DLLCreateNewUITextItem(TXT_ODT_PILOTNAME, UICOL_TEXT_NORMAL);
  auto message_text = DLLCreateNewUITextItem(TXT_ODT_MESSAGE, UICOL_TEXT_NORMAL);

  auto main_wnd = DLLNewUIGameWindowCreate(0, 0, 384, 256, UIF_PROCESS_ALL | UIF_CENTER | NUWF_TITLEMED);

  auto title = DLLTextCreate(main_wnd, title_text, 0, 7, UIF_CENTER);
  auto pilot = DLLTextCreate(main_wnd, pilot_text, 50, 55, 0);
  auto message_t = DLLTextCreate(main_wnd, message_text, 50, 110, 0);
  auto user_edit = DLLEditCreate(main_wnd, 10, 50, 75, 130, 15, 0);
  auto msg_edit = DLLEditCreate(main_wnd, 3, 50, 130, 200, 15, 0);

  // HotSpotCreate(int item,int parentitem, int id, int key, int txtitemoff, int txtitemon, int x, int y, int w, int h,
  // int flags,int winnum)
  auto cancel_hs =
      DLLHotSpotCreate(main_wnd, UID_CANCEL, KEY_ESC, cancel_on_text, cancel_off_text, 130, 256 - 80, 60, 20, 0);
  auto send_hs = DLLHotSpotCreate(main_wnd, UID_OK, KEY_ENTER, send_on_text, send_off_text, 60, 256 - 80, 60, 20, 0);
  DLLNewUIGameWindowOpen(main_wnd);

  DLLEditSetText(user_edit, name);
  while (!exit_menu) {
    int res;

    res = DLLDoUI();

    // handle all UI results.
    switch (res) {
    case 3:
    case UID_OK:
      DLLEditGetText(user_edit, pilot_name, MAX_CHAT_SEND_LEN);
      DLLEditGetText(msg_edit, message, MAX_CHAT_SEND_LEN);
      snprintf(fmt_msg, sizeof(fmt_msg), "/msg %s %s", pilot_name, message);
      p = SendChatString(fmt_msg);
      if (p && pconsole) {
        DLLUIConsoleGadgetputs(pconsole, p);
        DLLUIConsoleGadgetputs(pconsole, "\x0a\x0d");
      }
      exit_menu = 1;
      break;
    case UID_CANCEL:
      exit_menu = 1;
      break;
    }
  }

  DLLNewUIGameWindowClose(main_wnd);
  DLLNewUIGameWindowDestroy(main_wnd);

  // Clean up
  DLLRemoveUITextItem(title_text);
  DLLRemoveUITextItem(cancel_on_text);
  DLLRemoveUITextItem(cancel_off_text);
  DLLRemoveUITextItem(send_on_text);
  DLLRemoveUITextItem(send_off_text);
  DLLRemoveUITextItem(pilot_text);
  DLLRemoveUITextItem(message_text);

  DLLDeleteUIItem(main_wnd);
  DLLDeleteUIItem(title);
  DLLDeleteUIItem(pilot);
  DLLDeleteUIItem(message_t);
  DLLDeleteUIItem(user_edit);
  DLLDeleteUIItem(msg_edit);
  DLLDeleteUIItem(cancel_hs);
  DLLDeleteUIItem(send_hs);

  return "";
}

int JoinPrivateLobby() {
  int exit_menu = 0;
  int ret = 0;
  char message[MAX_CHAT_SEND_LEN];
  char priv_channel[MAX_CHAT_SEND_LEN];

  auto title_text = DLLCreateNewUITextItem(TXT_ODT_JOINPRIV, UICOL_WINDOW_TITLE);

  auto cancel_on_text = DLLCreateNewUITextItem(TXT_ODT_CANCEL, UICOL_HOTSPOT_HI);
  auto cancel_off_text = DLLCreateNewUITextItem(TXT_ODT_CANCEL, UICOL_HOTSPOT_LO);
  auto join_on_text = DLLCreateNewUITextItem(TXT_ODT_JOIN, UICOL_HOTSPOT_HI);
  auto join_off_text = DLLCreateNewUITextItem(TXT_ODT_JOIN, UICOL_HOTSPOT_LO);

  auto channel_text = DLLCreateNewUITextItem(TXT_ODT_CHANNELNAME, UICOL_TEXT_NORMAL);

  auto main_wnd = DLLNewUIGameWindowCreate(0, 0, 256, 256, UIF_PROCESS_ALL | UIF_CENTER | NUWF_TITLELARGE);

  auto title = DLLTextCreate(main_wnd, title_text, 0, 7, UIF_CENTER);
  auto channel_t = DLLTextCreate(main_wnd, channel_text, 50, 95, UIF_CENTER);
  auto chan_edit = DLLEditCreate(main_wnd, 3, 50, 115, 130, 15, UIF_CENTER);

  // DLLNewUIWindowSetFocusOnEditGadget(chan_edit,main_wnd);
  // HotSpotCreate(int item,int parentitem, int id, int key, int txtitemoff, int txtitemon, int x, int y, int w, int h,
  // int flags,int winnum)
  auto cancel_hs =
      DLLHotSpotCreate(main_wnd, UID_CANCEL, KEY_ESC, cancel_on_text, cancel_off_text, 130, 256 - 80, 60, 20, 0);
  auto join_hs = DLLHotSpotCreate(main_wnd, UID_OK, KEY_ENTER, join_on_text, join_off_text, 60, 256 - 80, 60, 20, 0);
  DLLNewUIGameWindowOpen(main_wnd);
  while (!exit_menu) {
    int res;

    res = DLLDoUI();

    // handle all UI results.
    switch (res) {
    case 3:
    case UID_OK: {
      DLLEditGetText(chan_edit, priv_channel, MAX_CHAT_SEND_LEN);
      for (uint32_t i = 0; i < strlen(priv_channel); i++) {
        if (priv_channel[i] == ' ') {
          priv_channel[i] = '_';
        }
      }

      snprintf(message, sizeof(message), "#%s", priv_channel);
      ret = JoinNewLobby(message);

      exit_menu = 1;
      if (ret) {
        char txtmessage[200];
        char cleanlobby[50];
        memset(cleanlobby, 0, 29);
        strcpy(cleanlobby, message + 1);
        for (int l = 0; l < 18; l++) {
          if (cleanlobby[l] == '_')
            cleanlobby[l] = ' ';
        }
        snprintf(txtmessage, sizeof(txtmessage), TXT_ODT_INNEWLOBBY, cleanlobby);
        DLLDoMessageBox(TXT_ODT_MASTERTRACKER, txtmessage, MSGBOX_OK, UICOL_WINDOW_TITLE, UICOL_TEXT_NORMAL);
        *DLLNum_network_games_known = 0;
      }
    } break;
    case UID_CANCEL:
      exit_menu = 1;
      break;
    }
  }

  DLLNewUIGameWindowClose(main_wnd);
  DLLNewUIGameWindowDestroy(main_wnd);

  DLLRemoveUITextItem(title_text);
  DLLRemoveUITextItem(cancel_on_text);
  DLLRemoveUITextItem(cancel_off_text);
  DLLRemoveUITextItem(join_on_text);
  DLLRemoveUITextItem(join_off_text);
  DLLRemoveUITextItem(channel_text);

  DLLDeleteUIItem(main_wnd);
  DLLDeleteUIItem(title);
  DLLDeleteUIItem(channel_t);
  DLLDeleteUIItem(chan_edit);
  DLLDeleteUIItem(cancel_hs);
  DLLDeleteUIItem(join_hs);
  return ret;
}

int FindPilot() {
  int exit_menu = 0;
  const char *p;
  char message[MAX_CHAT_SEND_LEN];
  char pilot_name[MAX_CHAT_SEND_LEN];
  static char fmt_msg[MAX_CHAT_SEND_LEN * 2];
  auto title_text = DLLCreateNewUITextItem(TXT_ODT_FINDPILOT, UICOL_WINDOW_TITLE);

  auto cancel_on_text = DLLCreateNewUITextItem(TXT_ODT_CANCEL, UICOL_HOTSPOT_HI);
  auto cancel_off_text = DLLCreateNewUITextItem(TXT_ODT_CANCEL, UICOL_HOTSPOT_LO);
  auto search_on_text = DLLCreateNewUITextItem(TXT_ODT_SEARCH, UICOL_HOTSPOT_HI);
  auto search_off_text = DLLCreateNewUITextItem(TXT_ODT_SEARCH, UICOL_HOTSPOT_LO);

  auto pilot_text = DLLCreateNewUITextItem(TXT_ODT_PILOTNAME, UICOL_TEXT_NORMAL);

  auto main_wnd = DLLNewUIGameWindowCreate(0, 0, 256, 256, UIF_PROCESS_ALL | UIF_CENTER | NUWF_TITLEMED);

  auto title_t = DLLTextCreate(main_wnd, title_text, 0, 7, UIF_CENTER);
  auto pilot_t = DLLTextCreate(main_wnd, pilot_text, 50, 95, UIF_CENTER);
  auto pilot_edit = DLLEditCreate(main_wnd, 3, 50, 115, 130, 15, UIF_CENTER);

  // DLLNewUIWindowSetFocusOnEditGadget(pilot_edit,main_wnd);
  // HotSpotCreate(int item,int parentitem, int id, int key, int txtitemoff, int txtitemon, int x, int y, int w, int h,
  // int flags,int winnum)
  auto cancel_hs =
      DLLHotSpotCreate(main_wnd, UID_CANCEL, KEY_ESC, cancel_on_text, cancel_off_text, 130, 256 - 80, 60, 20, 0);
  auto search_hs =
      DLLHotSpotCreate(main_wnd, UID_OK, KEY_ENTER, search_on_text, search_off_text, 60, 256 - 80, 60, 20, 0);
  DLLNewUIGameWindowOpen(main_wnd);
  while (!exit_menu) {
    int res;

    res = DLLDoUI();

    // handle all UI results.
    switch (res) {
    case 3:
    case UID_OK: {
      DLLEditGetText(pilot_edit, pilot_name, MAX_CHAT_SEND_LEN);
      DLLCreateSplashScreen(TXT_ODT_SEARCHINGPILOT, 1);
      p = GetChannelByUser(pilot_name);
      while (p == nullptr) {
        int sres = DLLPollUI();
        // Detect if cancel is hit
        if (sres == 99) {
          p = nullptr;
          // Cancel the lookup
          GetChannelByUser((char *)-1);
          break;
        }
        DLLDescentDefer();
        p = GetChatText();
        if (p && pconsole) {
          DLLUIConsoleGadgetputs(pconsole, p);
          DLLUIConsoleGadgetputs(pconsole, "\x0a\x0d");
        }
        p = GetChannelByUser(pilot_name);
      }
      if ((p == nullptr) || (p == (char *)-1)) {
        exit_menu = 1;
        DLLCloseSplashScreen();
        break;
      }
      strcpy(message, p + 1);
      for (uint32_t i = 0; i < strlen(message); i++)
        if (message[i] == '_')
          message[i] = ' ';
      snprintf(fmt_msg, sizeof(fmt_msg), TXT_ODT_CANBEFOUNDIN, pilot_name, message);
      if (pconsole) {
        DLLUIConsoleGadgetputs(pconsole, fmt_msg);
        DLLUIConsoleGadgetputs(pconsole, "\x0a\x0d");
      }
      exit_menu = 1;
      DLLCloseSplashScreen();
      break;
    }
    case UID_CANCEL:
      exit_menu = 1;
      break;
    }
  }

  DLLNewUIGameWindowClose(main_wnd);
  DLLNewUIGameWindowDestroy(main_wnd);

  DLLRemoveUITextItem(title_text);
  DLLRemoveUITextItem(cancel_on_text);
  DLLRemoveUITextItem(cancel_off_text);
  DLLRemoveUITextItem(search_on_text);
  DLLRemoveUITextItem(search_off_text);
  DLLRemoveUITextItem(pilot_text);

  DLLDeleteUIItem(main_wnd);
  DLLDeleteUIItem(title_t);
  DLLDeleteUIItem(pilot_t);
  DLLDeleteUIItem(pilot_edit);
  DLLDeleteUIItem(cancel_hs);
  DLLDeleteUIItem(search_hs);

  return 1;
}
