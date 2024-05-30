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

 * $Logfile: /DescentIII/Main/Dmfc/dmfcui.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:57:21 $
 * $Author: kevinb $
 *
 * UI Code for DMFC dialogs
 *
 * $Log: dmfcui.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:57:21  kevinb
 * initial 1.5 import
 *
 *
 * 26    7/09/99 2:54p Jeff
 * handle gametime better (pause it when needed) if the server is 'waiting
 * for players'
 *
 * 25    5/19/99 2:40a Jeff
 * use correct calls of NewUIGameWindow
 *
 * 24    5/18/99 7:14p Jeff
 * error checking
 *
 * 23    4/27/99 12:36p Jeff
 * fixed dialog due to new art
 *
 * 22    4/24/99 11:13p Jeff
 * added flags for creating a newui window for a title bar
 *
 * 21    4/09/99 3:28p Jeff
 * fixed some bugs in wait for players dlg
 *
 * 20    4/03/99 9:26p Jeff
 * changed dialogs that weren't using UID_OK and UID_CANCEL to use and
 * handle them properly
 *
 * 19    3/17/99 12:25p Jeff
 * converted DMFC to be COM interface
 *
 * 18    2/11/99 12:51a Jeff
 * changed names of exported variables
 *
 * 17    2/09/99 3:32p Jeff
 * table file parser takes quotes strings for force keywords
 *
 * 16    2/07/99 1:18a Jeff
 * peppered UI dialogs that were missing NEWUIRES_FORCEQUIT to handle it
 *
 * 15    2/05/99 8:24p Jeff
 * added table file parser macros
 *
 * 14    1/19/99 3:55a Jeff
 * all strings localized out
 *
 * 13    1/14/99 7:55p Jeff
 * changed icons for team selection dialog, added real powerball
 *
 * 12    1/12/99 2:55p Jeff
 * added/finished the waiting for player's to join dialog
 *
 * 11    1/11/99 6:54p Jeff
 * fixed bug with wait4player dialog
 *
 * 10    1/10/99 7:41p Jeff
 * added initial version of waitforplayers dialog
 *
 * 9     12/08/98 3:29p Jeff
 * updated the team control dialog so the server can determine if they
 * want to make the clients wait
 *
 * 8     12/08/98 12:17p Jeff
 * various changes that include an improved Team Control dialog (doesn't
 * switch teams until exit..) and spew/respawn players that change teams
 *
 * 7     11/19/98 5:56p Jeff
 * added slider exported and improved Hoard
 *
 * 6     11/17/98 6:29p Jeff
 * mod can specify whether or not to display the team setup dialog on team
 * game start.  Added a menu item to display team setup dialog in mid-game
 *
 * 5     11/17/98 12:35p Jeff
 * fixed bug where team placement window didn't update second time
 * displayed
 *
 * 4     11/16/98 5:35p Jeff
 * removed log functions, added support for changing team names, fixed ctf
 * to work better with different team names
 *
 * 3     11/11/98 7:19p Jeff
 * changes made so that a dedicated server's team is always -1 (team game
 * or not)
 *
 * 2     10/29/98 7:01p Jeff
 * creation of team placement dialog.  Moved TranslateEvent into DMFC
 *
 * $NoKeywords: $
 */

#include "gamedll_header.h"
#include "DMFC.h"
#include "dmfcinternal.h"

extern char **DMFCStringTable;
extern int DMFCStringTableSize;
extern const char *_DMFCErrorString;
extern DMFCBase *basethis;
const char *DMFCGetString(int d);

struct tTeamPlacementDialogInfo {
  void *lb[DLLMAX_TEAMS];
  void *text[DLLMAX_PLAYERS];
  int trans[DLLMAX_TEAMS][DLLMAX_PLAYERS];
  void (*old_callback)();
};
tTeamPlacementDialogInfo tpdi;

#define TS_NOTINGAME -2
int8_t TeamDlgInitialTeamSettings[DLLMAX_PLAYERS];
int8_t TeamDlgFinalTeamSettings[DLLMAX_PLAYERS];

// UpdateTeamPlacementDialog
//
//	Call to update the listboxes, etc in the team placement
void UpdateTeamPlacementDialog(void) {
  int selected_num[DLLMAX_TEAMS];
  int i;

  for (i = 0; i < DLLMAX_TEAMS; i++) {
    selected_num[i] = -1;

    // remove everything from the listboxes
    // save the selected names, and reselect them if they are on the same listbox
    if (tpdi.lb[i]) {
      int sel = DLLListGetSelectedIndex(tpdi.lb[i]);

      if (sel >= 0 && sel < DLLMAX_PLAYERS)
        selected_num[i] = tpdi.trans[i][sel];

      DLLListRemoveAll(tpdi.lb[i]);
    }
  }

  int curr_lb_count[DLLMAX_TEAMS];
  for (i = 0; i < DLLMAX_TEAMS; i++) {
    curr_lb_count[i] = 0;
    for (int p = 0; p < DLLMAX_PLAYERS; p++) {
      tpdi.trans[i][p] = -1;
    }
  }

  // now rebuild the listboxes with correct placement
  for (i = 0; i < DLLMAX_PLAYERS; i++) {

    if (tpdi.text[i]) {
      DLLRemoveUITextItem(tpdi.text[i]);
      tpdi.text[i] = NULL;
    }

    // as we go through all the players, we need to update the trans[][]
    if (basethis->CheckPlayerNum(i)) {
      // the player is in the game, get their team
      int team;

      if (TeamDlgFinalTeamSettings[i] != TS_NOTINGAME) {
        // ok this guy was in the game when the dialog first got displayed, so
        // use his current status, and not his real team
        team = TeamDlgFinalTeamSettings[i];
      } else {
        // he wasn't in the game to start and his team hasn't changed yet
        team = basethis->GetPlayerTeam(i);
      }

      // add them to a team
      if (tpdi.lb[team]) {
        tpdi.text[i] = DLLCreateNewUITextItem(basethis->Players[i].callsign, basethis->GetTeamColor(team), -1);
        if (tpdi.text[i]) {
          DLLListAddItem(tpdi.lb[team], tpdi.text[i]);
          tpdi.trans[team][curr_lb_count[team]] = i; // update the translator
          curr_lb_count[team]++;
        } else
          mprintf(0, "Couldn't create text item\n");
      } else {
        mprintf(0, "Listbox not valid!\n");
      }
    } else {
      TeamDlgInitialTeamSettings[i] = TeamDlgFinalTeamSettings[i] = TS_NOTINGAME;
    }
  }

  // go through all the listboxes, and reselect previously selected
  for (i = 0; i < DLLMAX_TEAMS; i++) {
    if (selected_num[i] != -1 && basethis->CheckPlayerNum(selected_num[i])) {

      if (((TeamDlgFinalTeamSettings[selected_num[i]] == TS_NOTINGAME) &&
           (basethis->GetPlayerTeam(selected_num[i]) == i)) ||
          (TeamDlgFinalTeamSettings[selected_num[i]] == i)) {
        // this player was on this team last update, is still on the team, is a valid player in the game and was
        // selected so select them again
        for (int j = 0; j < DLLMAX_PLAYERS; j++) {
          if (tpdi.trans[i][j] == selected_num[i]) {
            // we found em, reselect
            if (tpdi.lb[i]) {
              DLLListSetSelectedIndex(tpdi.lb[i], j);
            }
            break;
          }
        }
      }
    }
  }
}

static float Team_placement_lastcall = 0;
void TeamPlacementCallback(void) {
  float time = *basethis->Gametime;

  if (time > Team_placement_lastcall + 0.5f) {
    UpdateTeamPlacementDialog();
    Team_placement_lastcall = time;
  }

  if (tpdi.old_callback)
    (*tpdi.old_callback)();
}

void TeamPlacementChangeTeam(int old_team, int new_team) {
  int sel = DLLListGetSelectedIndex(tpdi.lb[old_team]);
  if (sel < 0)
    return;

  int pnum = tpdi.trans[old_team][sel];
  if (!basethis->CheckPlayerNum(pnum)) {
    TeamDlgInitialTeamSettings[pnum] = TeamDlgFinalTeamSettings[pnum] = TS_NOTINGAME;
    return;
  }

  TeamDlgFinalTeamSettings[pnum] = new_team;
}

#define REDTOBLUE 0xC0
#define REDTOGREEN 0xC1
#define REDTOYELLOW 0xC2
#define BLUETORED 0xC3
#define BLUETOGREEN 0xC4
#define BLUETOYELLOW 0xC5
#define GREENTORED 0xC6
#define GREENTOBLUE 0xC7
#define GREENTOYELLOW 0xC8
#define YELLOWTORED 0xC9
#define YELLOWTOBLUE 0xCA
#define YELLOWTOGREEN 0xCB

//	DMFCBase::DoDMFCUITeamPlacement
//
//	Displays the dialog for doing team placement in DMFC
void DMFCBase::DoDMFCUITeamPlacement(bool clients_wait, bool called_by_level_start) {
  if (GetLocalRole() != LR_SERVER)
    return;

  if (IAmDedicatedServer())
    return;

  Team_placement_lastcall = 0;

  // turn on client waiting
  if (clients_wait) {
    // Pause realgametime,
    ENABLE_FLAGS(m_iProtectedFlags, DMFC_PRF_PAUSETIME);
    SendControlMessageToPlayer(SP_ALL, CM_PAUSETIME);
    MakeClientsWait(true);
  }

  int i;

  // build the initial team list, so we know what players changed teams in the process
  for (i = 0; i < DLLMAX_PLAYERS; i++) {
    if (CheckPlayerNum(i)) {
      TeamDlgFinalTeamSettings[i] = TeamDlgInitialTeamSettings[i] = Players[i].team;
    } else {
      TeamDlgFinalTeamSettings[i] = TeamDlgInitialTeamSettings[i] = TS_NOTINGAME;
    }
  }

  bool exit_menu = false;
  void *list_red = NULL, *list_blue = NULL, *list_green = NULL, *list_yellow = NULL;
  void *icon_b_hs[DLLMAX_TEAMS][DLLMAX_TEAMS - 1];
  void *icon_hs[DLLMAX_TEAMS][DLLMAX_TEAMS - 1];
  void *team_text[DLLMAX_TEAMS];

  int count;
  int id_list[DLLMAX_TEAMS * (DLLMAX_TEAMS - 1)];

  int team_icons[DLLMAX_TEAMS];
  const char *team_icon_names[DLLMAX_TEAMS] = {"toredteamicon.ogf", "toblueteamicon.ogf", "togreenteamicon.ogf", "toyellowteamicon.ogf"};
  /*
  $$TABLE_GAMEFILE "toredteamicon.ogf"
  $$TABLE_GAMEFILE "toblueteamicon.ogf"
  $$TABLE_GAMEFILE "togreenteamicon.ogf"
  $$TABLE_GAMEFILE "toyellowteamicon.ogf"
  */
  for (i = 0; i < DLLMAX_TEAMS; i++) {
    team_icons[i] = DLLbm_AllocLoadFileBitmap(IGNORE_TABLE(team_icon_names[i]), 0, BITMAP_FORMAT_1555);
    if (team_icons[i] <= BAD_BITMAP_HANDLE)
      team_icons[i] = BAD_BITMAP_HANDLE;
  }

  for (i = 0; i < DLLMAX_TEAMS; i++) {
    count = 0;
    for (int j = 0; j < DLLMAX_TEAMS; j++) {
      if (i != j) {
        icon_b_hs[i][count] = DLLCreateNewUIBmpItem(team_icons[j], 255);
        icon_hs[i][count] = NULL;
        count++;
      }
    }
  }

  for (i = 0; i < DLLMAX_TEAMS * (DLLMAX_TEAMS - 1); i++) {
    id_list[i] = 0xC0 + i;
  }

  for (i = 0; i < DLLMAX_TEAMS; i++) {
    team_text[i] = NULL;
  }

  //////////////////////////////////////////////////
  // Text Items
  void *start_game_text_off =
      DLLCreateNewUITextItem((called_by_level_start) ? DTXT_UI_STARTGAME : DTXT_UI_RETURNTOGAME, UICOL_HOTSPOT_LO, -1);
  void *start_game_text_on =
      DLLCreateNewUITextItem((called_by_level_start) ? DTXT_UI_STARTGAME : DTXT_UI_RETURNTOGAME, UICOL_HOTSPOT_HI, -1);
  void *client_wait_txt = DLLCreateNewUITextItem(DTXT_UI_CLIENTSWAIT, UICOL_HOTSPOT_LO, -1);
  void *window_title = DLLCreateNewUITextItem(DTXT_UI_TEAMSETUP, UICOL_WINDOW_TITLE, -1);

  ///////////////////////////////////////////////////
  // Main Window
  void *main_wnd = DLLNewUIGameWindowCreate(0, 0, 512, 384, UIF_PROCESS_ALL | UIF_CENTER | NUWF_TITLEMED);

  ///////////////////////////////////////////////////
  // Item placement
  int cury;
  count = GetNumTeams();

  void *start_game_hs = DLLHotSpotCreate(main_wnd, UID_OK, K_ENTER, start_game_text_off, start_game_text_on, 40,
                                         384 - OKCANCEL_YOFFSET, 130, 15, UIF_CENTER);
  void *window_title_text = DLLTextCreate(main_wnd, window_title, 0, 7, UIF_CENTER | UIF_FIT);
  void *client_wait_cb;

  if (count > 2) {
    client_wait_cb = DLLCheckBoxCreate(main_wnd, 128, client_wait_txt, 256 - (DLLGetUIItemWidth(client_wait_txt) / 2),
                                       192 - (DLLGetUIItemHeight(client_wait_txt) / 2), 130, 15, UIF_FIT);
    cury = 60;
  } else {
    client_wait_cb = DLLCheckBoxCreate(main_wnd, 128, client_wait_txt, 30, 384 - OKCANCEL_YOFFSET, 130, 15, UIF_FIT);
    cury = 80;
  }

  DLLCheckBoxSetCheck(client_wait_cb, clients_wait);

  int left_x, right_x;
  int top_y, bottom_y;
  int icon_width = DLLbm_w(team_icons[0], 0);
  int icon_height = DLLbm_h(team_icons[0], 0);
  int curr_y;
  int *id_list_ptr;

  left_x = 168 + 10;
  right_x = 512 - 178 - icon_width;

  int total_height = (count - 1) * (icon_height + 3);
  top_y = cury + (64 - (total_height / 2));
  bottom_y = cury + 163 + (64 - (total_height / 2));

  switch (count) {
  case 4:
    list_yellow = DLLListCreate(main_wnd, 1, 512 - 168, cury + 163, 128, 128, 0);
    team_text[YELLOW_TEAM] = DLLEditCreate(main_wnd, -1, 512 - 168, cury + 131, 128, 32, 0);

    // put in the hotspots for yellow
    curr_y = bottom_y;
    id_list_ptr = &id_list[YELLOW_TEAM * (DLLMAX_TEAMS - 1)];
    for (i = 0; i < count - 1; i++) {
      icon_hs[YELLOW_TEAM][i] =
          DLLHotSpotCreate(main_wnd, id_list_ptr[i], -1, icon_b_hs[YELLOW_TEAM][i], icon_b_hs[YELLOW_TEAM][i], right_x,
                           curr_y, icon_width, icon_height, 0);
      curr_y += icon_height + 3;
    }
  case 3:
    list_green = DLLListCreate(main_wnd, 1, 40, cury + 163, 128, 128, 0);
    team_text[GREEN_TEAM] = DLLEditCreate(main_wnd, -1, 40, cury + 131, 128, 32, 0);

    list_red = DLLListCreate(main_wnd, 0, 40, cury, 128, 128, 0);
    team_text[RED_TEAM] = DLLEditCreate(main_wnd, -1, 40, cury - 32, 128, 32, 0);

    list_blue = DLLListCreate(main_wnd, 1, 512 - 168, cury, 128, 128, 0);
    team_text[BLUE_TEAM] = DLLEditCreate(main_wnd, -1, 512 - 168, cury - 32, 128, 32, 0);

    // put in the hotspots for green
    curr_y = bottom_y;
    id_list_ptr = &id_list[GREEN_TEAM * (DLLMAX_TEAMS - 1)];
    for (i = 0; i < count - 1; i++) {
      icon_hs[GREEN_TEAM][i] = DLLHotSpotCreate(main_wnd, id_list_ptr[i], -1, icon_b_hs[GREEN_TEAM][i],
                                                icon_b_hs[GREEN_TEAM][i], left_x, curr_y, icon_width, icon_height, 0);
      curr_y += icon_height + 3;
    }

    // put in the hotspots for blue
    curr_y = top_y;
    id_list_ptr = &id_list[BLUE_TEAM * (DLLMAX_TEAMS - 1)];
    for (i = 0; i < count - 1; i++) {
      icon_hs[BLUE_TEAM][i] = DLLHotSpotCreate(main_wnd, id_list_ptr[i], -1, icon_b_hs[BLUE_TEAM][i],
                                               icon_b_hs[BLUE_TEAM][i], right_x, curr_y, icon_width, icon_height, 0);
      curr_y += icon_height + 3;
    }

    // put in the hotspots for red
    curr_y = top_y;
    id_list_ptr = &id_list[RED_TEAM * (DLLMAX_TEAMS - 1)];
    for (i = 0; i < count - 1; i++) {
      icon_hs[RED_TEAM][i] = DLLHotSpotCreate(main_wnd, id_list_ptr[i], -1, icon_b_hs[RED_TEAM][i],
                                              icon_b_hs[RED_TEAM][i], left_x, curr_y, icon_width, icon_height, 0);
      curr_y += icon_height + 3;
    }

    break;
  case 2:
    list_red = DLLListCreate(main_wnd, 0, 40, cury, 128, 224, 0);
    team_text[RED_TEAM] = DLLEditCreate(main_wnd, -1, 40, cury - 32, 128, 32, 0);

    list_blue = DLLListCreate(main_wnd, 1, 512 - 168, cury, 128, 224, 0);
    team_text[BLUE_TEAM] = DLLEditCreate(main_wnd, -1, 512 - 168, cury - 32, 128, 32, 0);

    // recalc the top y value
    top_y = cury + (118 - (total_height / 2));

    // put in the hotspots for blue
    curr_y = top_y;
    id_list_ptr = &id_list[BLUE_TEAM * (DLLMAX_TEAMS - 1)];
    for (i = 0; i < count - 1; i++) {
      icon_hs[BLUE_TEAM][i] = DLLHotSpotCreate(main_wnd, id_list_ptr[i], -1, icon_b_hs[BLUE_TEAM][i],
                                               icon_b_hs[BLUE_TEAM][i], right_x, curr_y, icon_width, icon_height, 0);
      curr_y += icon_height + 3;
    }

    // put in the hotspots for red
    curr_y = top_y;
    id_list_ptr = &id_list[RED_TEAM * (DLLMAX_TEAMS - 1)];
    for (i = 0; i < count - 1; i++) {
      icon_hs[RED_TEAM][i] = DLLHotSpotCreate(main_wnd, id_list_ptr[i], -1, icon_b_hs[RED_TEAM][i],
                                              icon_b_hs[RED_TEAM][i], left_x, curr_y, icon_width, icon_height, 0);
      curr_y += icon_height + 3;
    }

    break;
  case 0:
  case 1:
    // the wait is over, let them in
    if (clients_wait) {
      // UnPause realgametime,
      DISABLE_FLAGS(m_iProtectedFlags, DMFC_PRF_PAUSETIME);
      SendControlMessageToPlayer(SP_ALL, CM_UNPAUSETIME);
      MakeClientsWait(false);
    }

    // Unpause realgametime,
    DISABLE_FLAGS(m_iProtectedFlags, DMFC_PRF_PAUSETIME);
    SendControlMessageToPlayer(SP_ALL, CM_UNPAUSETIME);
    return;
    break;
  }

  // Set information for the team name edit boxes
  for (i = 0; i < DLLMAX_TEAMS; i++) {
    if (team_text[i]) {
      DLLSetOldEditBufferLen(team_text[i], MAX_TEAMNAME_LEN - 1);
      DLLEditSetText(team_text[i], DMFC_team_names[i]);
    }
  }

  tpdi.lb[RED_TEAM] = list_red;
  tpdi.lb[BLUE_TEAM] = list_blue;
  tpdi.lb[GREEN_TEAM] = list_green;
  tpdi.lb[YELLOW_TEAM] = list_yellow;
  for (i = 0; i < DLLMAX_PLAYERS; i++) {
    tpdi.text[i] = NULL;
    for (int t = 0; t < DLLMAX_TEAMS; t++) {
      tpdi.trans[t][i] = -1;
    }
  }
  DLLGetUICallback((void **)&tpdi.old_callback);
  DLLSetUICallback(TeamPlacementCallback);

  //////////////////////////////////////////////////
  // Process all
  DLLNewUIGameWindowOpen(main_wnd);

  UpdateTeamPlacementDialog();

  bool canceled = false;

  while (!exit_menu) {
    int res;
    res = DLLPollUI();

    if (res == -1)
      continue;

    // handle all UI results.
    switch (res) {
    case REDTOBLUE:
      TeamPlacementChangeTeam(RED_TEAM, BLUE_TEAM);
      UpdateTeamPlacementDialog();
      break;
    case REDTOGREEN:
      TeamPlacementChangeTeam(RED_TEAM, GREEN_TEAM);
      UpdateTeamPlacementDialog();
      break;
    case REDTOYELLOW:
      TeamPlacementChangeTeam(RED_TEAM, YELLOW_TEAM);
      UpdateTeamPlacementDialog();
      break;
    case BLUETORED:
      TeamPlacementChangeTeam(BLUE_TEAM, RED_TEAM);
      UpdateTeamPlacementDialog();
      break;
    case BLUETOGREEN:
      TeamPlacementChangeTeam(BLUE_TEAM, GREEN_TEAM);
      UpdateTeamPlacementDialog();
      break;
    case BLUETOYELLOW:
      TeamPlacementChangeTeam(BLUE_TEAM, YELLOW_TEAM);
      UpdateTeamPlacementDialog();
      break;
    case GREENTORED:
      TeamPlacementChangeTeam(GREEN_TEAM, RED_TEAM);
      UpdateTeamPlacementDialog();
      break;
    case GREENTOBLUE:
      TeamPlacementChangeTeam(GREEN_TEAM, BLUE_TEAM);
      UpdateTeamPlacementDialog();
      break;
    case GREENTOYELLOW:
      TeamPlacementChangeTeam(GREEN_TEAM, YELLOW_TEAM);
      UpdateTeamPlacementDialog();
      break;
    case YELLOWTORED:
      TeamPlacementChangeTeam(YELLOW_TEAM, RED_TEAM);
      UpdateTeamPlacementDialog();
      break;
    case YELLOWTOBLUE:
      TeamPlacementChangeTeam(YELLOW_TEAM, BLUE_TEAM);
      UpdateTeamPlacementDialog();
      break;
    case YELLOWTOGREEN:
      TeamPlacementChangeTeam(YELLOW_TEAM, GREEN_TEAM);
      UpdateTeamPlacementDialog();
      break;
    case UID_OK: // start game
      // get the team names and save them
      for (i = 0; i < DLLMAX_TEAMS; i++) {
        if (team_text[i]) {
          char buff[MAX_TEAMNAME_LEN + 1];
          DLLEditGetText(team_text[i], buff, MAX_TEAMNAME_LEN);
          SetTeamName(i, buff, true);
        }
      }

      exit_menu = true;
      break;
    case NEWUIRES_FORCEQUIT:
      exit_menu = true;
      canceled = true;
      break;
    case 128: // make clients wait on/off
    {
      if (DLLCheckBoxIsChecked(client_wait_cb)) {
        // make the clients wait
        clients_wait = true;

        // Pause realgametime,
        ENABLE_FLAGS(m_iProtectedFlags, DMFC_PRF_PAUSETIME);
        SendControlMessageToPlayer(SP_ALL, CM_PAUSETIME);
      } else {
        // they no longer have to wait
        clients_wait = false;

        // UnPause realgametime,
        DISABLE_FLAGS(m_iProtectedFlags, DMFC_PRF_PAUSETIME);
        SendControlMessageToPlayer(SP_ALL, CM_UNPAUSETIME);
      }

      MakeClientsWait(clients_wait);
    } break;
    };
  }

  // Place all changed players!!!
  if (!canceled) {
    for (i = 0; i < DLLMAX_PLAYERS; i++) {
      if (CheckPlayerNum(i)) {
        // the player is in the game, update his team if needed
        if (TeamDlgInitialTeamSettings[i] == TS_NOTINGAME) {
          // this player was NOT in the game when we brought up the dialog

          if (TeamDlgFinalTeamSettings[i] != TS_NOTINGAME) {
            // we've changed his team though
            int new_team;
            new_team = TeamDlgFinalTeamSettings[i];
            RequestTeamChange(new_team, i, false); // no need to spew his stuff
          } else {
            // we haven't changed his team, so he's ok on the team he's on
          }
        } else {
          // this player was in the game when we brought up the dialog
          if (TeamDlgInitialTeamSettings[i] != TeamDlgFinalTeamSettings[i]) {
            // he has changed teams!!
            int new_team;
            if (TeamDlgFinalTeamSettings[i] != TS_NOTINGAME) {
              new_team = TeamDlgFinalTeamSettings[i];
            } else {
              // weird...they are out of sync?
              FatalError("Team Dialog Out Of Sync"); // remove later
            }
            RequestTeamChange(new_team, i, true); // spew his stuff also
          }
        }
      }
    }
  }

  // the wait is over, let them in
  if (clients_wait) {
    // UnPause realgametime,
    DISABLE_FLAGS(m_iProtectedFlags, DMFC_PRF_PAUSETIME);
    SendControlMessageToPlayer(SP_ALL, CM_UNPAUSETIME);
    MakeClientsWait(false);
  }

  DLLNewUIGameWindowClose(main_wnd);
  DLLNewUIGameWindowDestroy(main_wnd);

  ////////////////////////////////////////////
  // Remove text items
  DLLRemoveUITextItem(start_game_text_on);
  DLLRemoveUITextItem(start_game_text_off);
  DLLRemoveUITextItem(window_title);
  DLLRemoveUITextItem(client_wait_txt);
  for (i = 0; i < DLLMAX_PLAYERS; i++) {
    if (tpdi.text[i]) {
      DLLRemoveUITextItem(tpdi.text[i]);
      tpdi.text[i] = NULL;
    }
  }
  for (i = 0; i < DLLMAX_TEAMS; i++) {
    for (int j = 0; j < DLLMAX_TEAMS - 1; j++) {
      if (icon_b_hs[i][j])
        DLLRemoveUIBmpItem(icon_b_hs[i][j]);
      if (icon_hs[i][j])
        DLLDeleteUIItem(icon_hs[i][j]);
    }
  }

  ////////////////////////////////////////////
  // Remove all items
  if (list_red)
    DLLDeleteUIItem(list_red);
  if (list_blue)
    DLLDeleteUIItem(list_blue);
  if (list_green)
    DLLDeleteUIItem(list_green);
  if (list_yellow)
    DLLDeleteUIItem(list_yellow);
  DLLDeleteUIItem(main_wnd);
  DLLDeleteUIItem(start_game_hs);
  DLLDeleteUIItem(window_title_text);
  DLLDeleteUIItem(client_wait_cb);

  for (i = 0; i < DLLMAX_TEAMS; i++) {
    if (team_icons[i] > BAD_BITMAP_HANDLE)
      DLLbm_FreeBitmap(team_icons[i]);
  }

  for (i = 0; i < DLLMAX_TEAMS; i++) {
    if (team_text[i])
      DLLDeleteUIItem(team_text[i]);
  }

  DLLSetUICallback(tpdi.old_callback);
}

//============================================================================
//		Wait for Players Dialog
//============================================================================
struct tWaitForPlayers {
  float last_update;
  void (*old_callback)(void);
  void *listbox;
  void *text[DLLMAX_PLAYERS];
  int indexmap[DLLMAX_PLAYERS];
};
tWaitForPlayers wfpi;

void UpdateWaitForPlayersDialog(void) {
  int pnum = -1;

  int sel = DLLListGetSelectedIndex(wfpi.listbox);
  if (sel >= 0) {
    pnum = wfpi.indexmap[sel];
  }

  int index = 0;
  DLLListRemoveAll(wfpi.listbox);
  for (int i = 0; i < DLLMAX_PLAYERS; i++) {
    if (wfpi.text[i]) {
      DLLRemoveUITextItem(wfpi.text[i]);
      wfpi.text[i] = NULL;
    }

    if (basethis->CheckPlayerNum(i)) {
      wfpi.indexmap[index] = i;
      index++;
      wfpi.text[i] = DLLCreateNewUITextItem(basethis->Players[i].callsign, basethis->Player_colors[i], -1);
      DLLListAddItem(wfpi.listbox, wfpi.text[i]);
      if (pnum == i) {
        DLLListSetSelectedIndex(wfpi.listbox, i);
      }
    }
  }
}
void WaitForPlayersCallback(void) {
  float time = *basethis->Gametime;

  if (time > wfpi.last_update + 0.5f) {
    UpdateWaitForPlayersDialog();
    wfpi.last_update = time;
  }

  if (wfpi.old_callback)
    (*wfpi.old_callback)();
}

//	DMFCBase::DoDMFCUIWaitForPlayers
//
//	Displays the dialog for in the beginning of the level, waiting for all the players
void DMFCBase::DoDMFCUIWaitForPlayers(bool clients_wait) {
  if (GetLocalRole() != LR_SERVER)
    return;

  if (IAmDedicatedServer())
    return;

  wfpi.last_update = 0;
  int i;
  for (i = 0; i < DLLMAX_PLAYERS; i++)
    wfpi.text[i] = NULL;

  // turn on client waiting
  if (clients_wait) {
    // Pause realgametime,
    ENABLE_FLAGS(m_iProtectedFlags, DMFC_PRF_PAUSETIME);
    SendControlMessageToPlayer(SP_ALL, CM_PAUSETIME);
    MakeClientsWait(true);
  }

  for (i = 0; i < DLLMAX_PLAYERS; i++)
    wfpi.indexmap[i] = 0;

  ///////////////////////////////////////////////////
  // Text Items
  void *start_game_text_off = DLLCreateNewUITextItem(DTXT_UI_STARTGAME, UICOL_HOTSPOT_LO, -1);
  void *start_game_text_on = DLLCreateNewUITextItem(DTXT_UI_STARTGAME, UICOL_HOTSPOT_HI, -1);
  void *client_wait_txt = DLLCreateNewUITextItem(DTXT_UI_CLIENTSWAIT, UICOL_HOTSPOT_LO, -1);
  void *window_title = DLLCreateNewUITextItem(DTXT_UI_WAITFORPLAYERS, UICOL_WINDOW_TITLE, -1);
  void *kick_text_off = DLLCreateNewUITextItem(DTXT_UI_KICKPLAYER, UICOL_HOTSPOT_LO, -1);
  void *kick_text_on = DLLCreateNewUITextItem(DTXT_UI_KICKPLAYER, UICOL_HOTSPOT_HI, -1);
  void *ban_text_off = DLLCreateNewUITextItem(DTXT_UI_BANPLAYER, UICOL_HOTSPOT_LO, -1);
  void *ban_text_on = DLLCreateNewUITextItem(DTXT_UI_BANPLAYER, UICOL_HOTSPOT_HI, -1);

  ///////////////////////////////////////////////////
  // Main Window
  void *main_wnd = DLLNewUIGameWindowCreate(0, 0, 512, 384, UIF_PROCESS_ALL | UIF_CENTER | NUWF_TITLEMED);

  ///////////////////////////////////////////////////
  // Item placement
  int cury = 30;
  bool exit_menu = false;

  void *start_game_hs = DLLHotSpotCreate(main_wnd, UID_OK, K_ENTER, start_game_text_off, start_game_text_on, 40,
                                         384 - OKCANCEL_YOFFSET, 130, 15, UIF_CENTER);
  void *window_title_text = DLLTextCreate(main_wnd, window_title, 0, 7, UIF_CENTER | UIF_FIT);
  void *client_wait_cb =
      DLLCheckBoxCreate(main_wnd, 128, client_wait_txt, 30, 369 - OKCANCEL_YOFFSET, 130, 15, UIF_FIT | UIF_CENTER);
  wfpi.listbox = DLLListCreate(main_wnd, -1, 40, cury, 256, 288, UIF_CENTER);
  DLLCheckBoxSetCheck(client_wait_cb, clients_wait);

  int l_x, l_y, l_w, l_h;
  DLLGetUIItemPosition(wfpi.listbox, &l_x, &l_y, &l_w, &l_h);
  l_x = l_x + l_w + 10;
  l_y = (l_y + (l_h / 2)) - 15;
  void *kick_hs = DLLHotSpotCreate(main_wnd, 10, -1, kick_text_off, kick_text_on, l_x, l_y, 130, 15, UIF_FIT);
  l_y += 15;
  void *ban_hs = DLLHotSpotCreate(main_wnd, 20, -1, ban_text_off, ban_text_on, l_x, l_y, 130, 15, UIF_FIT);

  //////////////////////////////////////////////////
  // Process all
  DLLGetUICallback((void **)&wfpi.old_callback);
  DLLSetUICallback(WaitForPlayersCallback);

  DLLNewUIGameWindowOpen(main_wnd);

  UpdateWaitForPlayersDialog();

  mprintf(0, "1:Entering UI Loop\n");

  while (!exit_menu) {
    int res;
    res = DLLPollUI();

    if (res == -1) {
      continue;
    }
    switch (res) {
    case NEWUIRES_FORCEQUIT:
      mprintf(0, "1:Got a force quit\n");
      exit_menu = true;
      break;
    case UID_OK: // exit menu
      mprintf(0, "1:Got a regular quit\n");
      exit_menu = true;
      break;
    case 10: // kick player
    {
      int cursel = DLLListGetSelectedIndex(wfpi.listbox);
      int pnum = wfpi.indexmap[cursel];
      KickPlayer(pnum);
    } break;
    case 20: // ban player
    {
      int cursel = DLLListGetSelectedIndex(wfpi.listbox);
      int pnum = wfpi.indexmap[cursel];
      BanPlayer(pnum);
    } break;
    case 128: // make clients wait on/off
    {
      if (DLLCheckBoxIsChecked(client_wait_cb)) {
        // make the clients wait
        clients_wait = true;
        // Pause realgametime,
        ENABLE_FLAGS(m_iProtectedFlags, DMFC_PRF_PAUSETIME);
        SendControlMessageToPlayer(SP_ALL, CM_PAUSETIME);
      } else {
        // they no longer have to wait
        clients_wait = false;
        // UnPause realgametime,
        DISABLE_FLAGS(m_iProtectedFlags, DMFC_PRF_PAUSETIME);
        SendControlMessageToPlayer(SP_ALL, CM_UNPAUSETIME);
      }
      MakeClientsWait(clients_wait);
    } break;
    };
  }

  mprintf(0, "1:Out of UI Loop\n");

  // the wait is over, let them in
  if (clients_wait) {
    // Unpause realgametime,
    DISABLE_FLAGS(m_iProtectedFlags, DMFC_PRF_PAUSETIME);
    SendControlMessageToPlayer(SP_ALL, CM_UNPAUSETIME);
    MakeClientsWait(false);
  }

  DLLNewUIGameWindowClose(main_wnd);
  DLLNewUIGameWindowDestroy(main_wnd);

  DLLRemoveUITextItem(start_game_text_on);
  DLLRemoveUITextItem(start_game_text_off);
  DLLRemoveUITextItem(kick_text_off);
  DLLRemoveUITextItem(kick_text_on);
  DLLRemoveUITextItem(ban_text_off);
  DLLRemoveUITextItem(ban_text_on);
  DLLRemoveUITextItem(window_title);
  DLLRemoveUITextItem(client_wait_txt);
  for (i = 0; i < DLLMAX_PLAYERS; i++) {
    if (wfpi.text[i]) {
      DLLRemoveUITextItem(wfpi.text[i]);
      wfpi.text[i] = NULL;
    }
  }

  DLLDeleteUIItem(main_wnd);
  DLLDeleteUIItem(start_game_hs);
  DLLDeleteUIItem(window_title_text);
  DLLDeleteUIItem(client_wait_cb);
  DLLDeleteUIItem(wfpi.listbox);
  DLLDeleteUIItem(kick_hs);
  DLLDeleteUIItem(ban_hs);

  DLLSetUICallback(wfpi.old_callback);
}
