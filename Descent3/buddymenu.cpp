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

/*
 * $Logfile: /DescentIII/main/buddymenu.cpp $
 * $Revision: 31 $
 * $Date: 9/05/01 12:26p $
 * $Author: Matt $
 *
 * Guidebot menu system.
 *
 * $Log: /DescentIII/main/buddymenu.cpp $
 *
 * 31    9/05/01 12:26p Matt
 * Flush low-level mouse click queue before bringing up buddy, save, and
 * load menus since these menus would otherwise get clicks that occurred
 * before they existed.
 *
 * 30    4/30/99 7:49p Jason
 * setups urgent/non urgent reliable stuff better
 *
 * 29    4/30/99 1:29p Samir
 * fixed buddy menu a-z selections.
 *
 * 28    4/27/99 1:56p Jeff
 * audio taunts stuff in pilot menu, added stringtables
 *
 * 27    4/27/99 4:41a Jeff
 * made multiplayer friendly
 *
 * 26    4/24/99 5:41p Samir
 * keys should work properly for buddy menu.
 *
 * 25    4/22/99 10:25p Chris
 * Added a comment block
 *
 * $NoKeywords: $
 */

#include "help.h"
#include "mono.h"
#include "renderer.h"
#include "render.h"
#include "ddio.h"
#include "descent.h"
#include "game.h"
#include "CFILE.H"
#include "application.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "newui.h"
#include "grtext.h"
#include "gamefont.h"
#include "AIMain.h"
#include "robot.h"
#include "hud.h"
#include "stringtable.h"
#include "textaux.h"
#include "multi.h"

#include "osiris_share.h"
#include "multi.h"

#define GB_MENU_REQ_TEXT 1
#define GB_MENU_REQ_SELECT 2

#define IDH_QUIT 50
#define WND_HELP_W 320
#define WND_HELP_H 384
#define WND_HELP_X (Game_window_w - WND_HELP_W) / 2
#define WND_HELP_Y (Game_window_h - WND_HELP_H) / 2
#define BUDDYMENU_ITEM_X 10
#define BUDDYMENU_ITEM_W 226
#define UID_BUDDYCMD 0x100

gb_menu Guidebot_menu_data;
int Guidebot_data_download_status;

// downloads data for the buddybot
//	on return:	if 1, process data
//				if 0, abort
//				if -1, no guidebot
int BuddyBotDownloadData(void);
void BuddyProcessCommand(int res);

void BuddyDisplay(void) {
  newuiTiledWindow gbcommand_wnd;
  newuiSheet *sheet;
  bool exit_menu = false;
  int strs_to_print, index, height;

  if (Game_mode & GM_MULTI && (!(Netgame.flags & NF_ALLOWGUIDEBOT))) {
    // only in single player games, and multiplayer games that allow guidebot
    return;
  }

  strs_to_print = 0;

  if (BuddyBotDownloadData() != 1)
    return;

  gbcommand_wnd.Create(Guidebot_menu_data.title, 0, 0, WND_HELP_W, WND_HELP_H);
  sheet = gbcommand_wnd.GetSheet();

  // add commands
  sheet->NewGroup(NULL, 10, 10);
  gbcommand_wnd.AddAcceleratorKey(KEY_F4, UID_CANCEL);

  for (index = 0; index < Guidebot_menu_data.num_commands; index++) {
    char buf[256], buf2[256];
    if (index < 9) {
      snprintf(buf, sizeof(buf), "%d. %s", index + 1, Guidebot_menu_data.command_text[index]);
    } else {
      snprintf(buf, sizeof(buf), "%c. %s", 'A' + (index - 9), Guidebot_menu_data.command_text[index]);
    }

    textaux_WordWrap(buf, buf2, BUDDYMENU_ITEM_W, MONITOR9_NEWUI_FONT);
    grtext_SetFont(MONITOR9_NEWUI_FONT);
    height = grtext_GetTextHeight(buf2);

    sheet->AddHotspot(buf, BUDDYMENU_ITEM_W, height, index + UID_BUDDYCMD,
                      (index == 0 || index == (Guidebot_menu_data.num_commands - 1)) ? true : false);
    if (index < (N_WINDOW_ACCELS - 1)) {
      if (index < 9) {
        int key = ddio_AsciiToKey('1' + index);
        gbcommand_wnd.AddAcceleratorKey(key, index + UID_BUDDYCMD);
      } else {
        int key = ddio_AsciiToKey('a' + (index - 9));
        gbcommand_wnd.AddAcceleratorKey(key, index + UID_BUDDYCMD);
      }
    }
  }

  // Double space
  sheet->NewGroup(NULL, WND_HELP_W - 240, WND_HELP_H - 96, NEWUI_ALIGN_HORIZ);
  sheet->AddLongButton(TXT_PRESSESCRET, UID_CANCEL);
  gbcommand_wnd.AddAcceleratorKey(KEY_ESC, UID_OK);

  gbcommand_wnd.Open();

  // Mouse clicks from gameplay will be read by the dialog without this flush
  ddio_MouseQueueFlush();

  while (!exit_menu) {
    int res = gbcommand_wnd.DoUI();

    // handle all UI results.
    switch (res) {
    case UID_OK:
    case UID_CANCEL:
      // TODO: Save menu options to variables
      exit_menu = true;
      break;
    case NEWUIRES_FORCEQUIT:
      exit_menu = true;
      break;
    default:
      BuddyProcessCommand(res);
      exit_menu = true;
      break;
    }
  }

  gbcommand_wnd.Close();
  gbcommand_wnd.Destroy();
}

void MultiAskforGuidebotMenu() {
  int size_offset;
  int count = 0;
  ubyte data[MAX_GAME_DATA_SIZE];
  // Only process this if we are the client
  if (Netgame.local_role != LR_CLIENT) {
    Int3(); // We shouldn't be here... get Kevin
    return;
  }
  size_offset = START_DATA(MP_GUIDEBOTMENU_REQUEST, data, &count);
  MultiAddByte(GB_MENU_REQ_TEXT, data, &count);
  END_DATA(count, data, size_offset);
  nw_SendReliable(NetPlayers[Player_num].reliable_socket, data, count);
}

void MultiStuffGuidebotMenuData(ubyte *data, int *count, gb_menu *menu) {
  int i;
  // Get the menu data and stick it here
  // length of the title
  int slen = strlen(menu->title) + 1;
  MultiAddShort(slen, data, count);
  memcpy(data + *count, menu->title, slen);
  *count += slen;

  MultiAddShort(menu->num_commands, data, count);
  for (i = 0; i < menu->num_commands; i++) {
    slen = strlen(menu->command_text[i]) + 1;
    MultiAddShort(slen, data, count);
    memcpy(data + *count, menu->command_text[i], slen);
    *count += slen;

    MultiAddByte(menu->command_id[i], data, count);

    MultiAddByte(menu->command_type[i], data, count);

    slen = strlen(menu->dialog_text[i]) + 1;
    MultiAddShort(slen, data, count);
    memcpy(data + *count, menu->dialog_text[i], slen);
    *count += slen;
  }
}

// TODO -- Call this function when the client selects a menu item
void MultiSendGuidebotMenuSelection(gb_com *command) {
  int size_offset;
  int count = 0;
  ubyte data[MAX_GAME_DATA_SIZE];

  // Only process this if we are the client
  if (Netgame.local_role != LR_CLIENT) {
    Int3(); // We shouldn't be here... get Kevin
    return;
  }

  size_offset = START_DATA(MP_GUIDEBOTMENU_REQUEST, data, &count);
  MultiAddByte(GB_MENU_REQ_SELECT, data, &count);

  MultiAddInt(command->action, data, &count);
  MultiAddInt(command->index, data, &count);

  if (command->ptr) {
    int len = strlen((char *)command->ptr) + 1;
    MultiAddShort(len, data, &count);
    memcpy(data + count, command->ptr, len);
    count += len;
  } else {
    MultiAddShort(0, data, &count);
  }

  END_DATA(count, data, size_offset);
  nw_SendReliable(NetPlayers[Player_num].reliable_socket, data, count);
}

void MultiSendGuidebotMenuText(gb_menu *menu, int slot) {
  int size_offset;
  int count = 0;

  ubyte data[MAX_GAME_DATA_SIZE];
  // Only process this if we are the server
  if (Netgame.local_role != LR_SERVER) {
    Int3(); // We shouldn't be here... get Kevin
    return;
  }
  size_offset = START_DATA(MP_GUIDEBOTMENU_DATA, data, &count);

  MultiStuffGuidebotMenuData(data, &count, menu);

  END_DATA(count, data, size_offset);
  nw_SendReliable(NetPlayers[slot].reliable_socket, data, count, false);
}

void MultiReadGuidebotMenuData(ubyte *data, int *count, gb_menu *menu) {

  int i;
  // Get the menu data and stick it here
  // length of the title
  int slen = MultiGetShort(data, count);
  memcpy(menu->title, data + *count, slen);
  *count += slen;

  menu->num_commands = MultiGetShort(data, count);

  for (i = 0; i < menu->num_commands; i++) {
    slen = MultiGetShort(data, count);
    memcpy(menu->command_text[i], data + *count, slen);
    *count += slen;

    menu->command_id[i] = MultiGetByte(data, count);

    menu->command_type[i] = MultiGetByte(data, count);

    slen = MultiGetShort(data, count);
    memcpy(menu->dialog_text[i], data + *count, slen);
    *count += slen;
  }
}

void MultiDoGuidebotMenuRequest(ubyte *data, int slot) {

  int count = 0;
  SKIP_HEADER(data, &count);
  // Only process this if we are the server
  if (Netgame.local_role == LR_CLIENT) {
    Int3(); // We shouldn't be here... get Kevin
    return;
  }

  int command = MultiGetByte(data, &count);

  switch (command) {
  case GB_MENU_REQ_TEXT: {
    // Client wants text to put in his menu
    // Generate a menu struct and send it off
    object *b_obj = ObjGet(Buddy_handle[slot]);
    ASSERT(b_obj);

    gb_com command;
    gb_menu menu;

    command.action = COM_GET_MENU;
    command.ptr = (void *)&menu;

    AINotify(b_obj, AIN_USER_DEFINED, (void *)&command);

    MultiSendGuidebotMenuText(&menu, slot);
  } break;
  case GB_MENU_REQ_SELECT: {
    gb_com command;
    object *b_obj = ObjGet(Buddy_handle[slot]);
    ASSERT(b_obj);

    command.action = MultiGetInt(data, &count);
    command.index = MultiGetInt(data, &count);

    int len = MultiGetShort(data, &count);

    if (len > 0) {
      command.ptr = data + count;
      count += len;
    } else {
      command.ptr = NULL;
    }

    AINotify(b_obj, AIN_USER_DEFINED, (void *)&command);
  } break;
  default:
    Int3(); // We shouldn't be here either...get Kevin
  }
}

void MultiDoGuidebotMenuData(ubyte *data) {
  int count = 0;
  SKIP_HEADER(data, &count);
  if (Netgame.local_role != LR_CLIENT) {
    Int3();
    return;
  }
  // Server is telling us about the menu -- we should display it now
  MultiReadGuidebotMenuData(data, &count, &Guidebot_menu_data);

  Guidebot_data_download_status = 1;
  Multi_bail_ui_menu = true;
  mprintf((0, "Recieved Guidebot data from server\n"));
}

// downloads data for the buddybot
//	on return:	if 1, process data
//				if 0, abort
//				if -1, no guidebot
int BuddyBotDownloadData(void) {
  if ((!(Game_mode & GM_MULTI)) || (Netgame.local_role != LR_CLIENT)) {
    // no need to download data, we already have access to it
    // fill in Guidebot_menu_data and leave
    object *b_obj = ObjGet(Buddy_handle[Player_object->id]);
    ASSERT(b_obj);

    gb_com command;
    command.action = COM_GET_MENU;
    command.ptr = (void *)&Guidebot_menu_data;

    AINotify(b_obj, AIN_USER_DEFINED, (void *)&command);

    if (Guidebot_menu_data.num_commands == 0) {
      AddHUDMessage(TXT_NOGUIDEBOT);
      return -1;
    }

    return 1;
  }

  // we are a client in a multiplayer game, we must request from the server
  Guidebot_data_download_status = 0; // processesing
  MultiAskforGuidebotMenu();

  // now we must 'process' until Guidebot_data_download_status no longer is 0
  newuiTiledWindow hwnd;
  newuiSheet *sheet;
  hwnd.Create(TXT_GUIDEBOT, 0, 0, 288, 128);
  sheet = hwnd.GetSheet();
  sheet->NewGroup(NULL, 5, 10);
  sheet->AddText(TXT_ACCESSGUIDEBOT);
  sheet->NewGroup(NULL, 40, 30);
  sheet->AddText(TXT_ESCTOCANCEL);
  hwnd.AddAcceleratorKey(KEY_ESC, UID_CANCEL);

  hwnd.Open();
  while (Guidebot_data_download_status == 0) {
    int res = hwnd.DoUI();

    switch (res) {
    case UID_OK:
    case UID_CANCEL:
    case NEWUIRES_FORCEQUIT:
      if (Guidebot_data_download_status == 1) {
        // we recieved data
        Multi_bail_ui_menu = false;
      } else {
        Guidebot_data_download_status = -1;
      }
      break;
    default:
      break;
    }
  }
  hwnd.Close();
  hwnd.Destroy();

  if (Guidebot_data_download_status == 1) {
    if (Guidebot_menu_data.num_commands == 0) {
      AddHUDMessage(TXT_NOGUIDEBOT);
      return -1;
    }
    return 1;
  }
  return 0;
}

void BuddyProcessCommand(int res) {
  gb_com command;
  char answer[8];

  int index = res - UID_BUDDYCMD;

  command.action = COM_DO_ACTION;
  command.index = Guidebot_menu_data.command_id[index];
  if (Guidebot_menu_data.command_type[index] == GBCT_STRING_DIALOG) {
    answer[0] = '\0';

    DoEditDialog(Guidebot_menu_data.dialog_text[index], answer, sizeof(answer) - 1);
    command.ptr = (void *)answer;
  } else {
    command.ptr = NULL;
  }

  if ((!(Game_mode & GM_MULTI)) || (Netgame.local_role != LR_CLIENT)) {
    object *b_obj = ObjGet(Buddy_handle[Player_object->id]);
    ASSERT(b_obj);

    AINotify(b_obj, AIN_USER_DEFINED, (void *)&command);
  } else {
    MultiSendGuidebotMenuSelection(&command);
  }
}
