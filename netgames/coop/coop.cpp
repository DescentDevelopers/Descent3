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

 * $Logfile: /DescentIII/Main/coop/coop.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:56:42 $
 * $Author: kevinb $
 *
 * Co-Op Play
 *
 * $Log: coop.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:56:42  kevinb
 * initial 1.5 import
 *
 *
 * 23    10/21/99 9:27p Jeff
 * B.A. Macintosh code merge
 *
 * 22    7/12/99 1:17p Jeff
 * added netgame flag NF_COOP
 *
 * 21    5/23/99 3:04a Jason
 * fixed bug with player rankings not being updated correctly
 *
 * 20    5/19/99 3:53p Jeff
 * fixed possible (???) bug with displaying hud names
 *
 * 19    5/12/99 11:04p Jeff
 * dmfc and multiplayer games now have endian friendly packets (*whew*)
 *
 * 18    5/09/99 6:20a Jeff
 * improved Entropy (added sounds, max virii per room).  Fixed rendering
 * bugs for other multiplayer dlls.
 *
 * 17    5/07/99 12:52p Jeff
 * audio taunt icon is ppic if available.  coop has hard max team set of 4
 *
 * 16    5/02/99 8:39a Jeff
 * made quick & dirty PLR for coop
 *
 * 15    4/23/99 6:15p Jeff
 * fixed double calls to GameClose
 *
 * 14    4/23/99 12:43p Jeff
 * forgot to call CloseGame
 *
 * 13    4/21/99 5:34p Jeff
 * added requirements
 *
 * 12    4/19/99 6:07p Jeff
 * compile for Linux
 *
 * 11    3/27/99 4:53p Jeff
 * player rankings in multiplayer games implemented.  Fixed join message
 * so it doesn't get cut off
 *
 * 10    3/19/99 12:54p Jeff
 * base support for requesting the number of teams for a multiplayer game
 *
 * 9     3/18/99 8:40p Jeff
 * hi res font fixes
 *
 * 8     3/17/99 12:23p Jeff
 * converted DMFC to be COM interface
 *
 * 7     3/01/99 4:17p Jeff
 * added net flag whether buddy bot allowed
 *
 * 6     2/11/99 3:09p Jeff
 * localized
 *
 * 5     2/11/99 12:49a Jeff
 * changed names of exported variables
 *
 * 3     2/07/99 2:06a Jeff
 * updated coop...fixed bug when getting countermeasure owner, if owner is
 * observer
 *
 * 2     2/03/99 7:22p Jeff
 *
 * 1     2/03/99 7:19p Jeff
 *
 * $NoKeywords: $
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "idmfc.h"
#include "coop.h"
#include "coopstr.h"

#include <algorithm>

IDMFC *DMFCBase = NULL;
static IDmfcStats *dstat = NULL;
static player *dPlayers;

struct tPlayerStat {
  int Score[2];
};

static int pack_pstat(tPlayerStat *user_info, uint8_t *data);
static int unpack_pstat(tPlayerStat *user_info, uint8_t *data);

int pack_pstat(tPlayerStat *user_info, uint8_t *data) {
  int count = 0;
  MultiAddInt(user_info->Score[0], data, &count);
  MultiAddInt(user_info->Score[1], data, &count);
  return count;
}

int unpack_pstat(tPlayerStat *user_info, uint8_t *data) {
  int count = 0;
  user_info->Score[0] = MultiGetInt(data, &count);
  user_info->Score[1] = MultiGetInt(data, &count);
  return count;
}

///////////////////////////////////////////////
// localization info
static char **StringTable;
static int StringTableSize = 0;
static const char *_ErrorString = "Missing String";
const char *GetStringFromTable(int d) {
  if ((d < 0) || (d >= StringTableSize))
    return _ErrorString;
  else
    return StringTable[d];
}
///////////////////////////////////////////////
static int SortedPlayers[MAX_PLAYER_RECORDS];
static bool DisplayScoreScreen = false;
static bool FirstFrame = false;
static bool display_my_welcome = false;

static void DisplayWelcomeMessage(int player_num);
static void DisplayHUDScores(struct tHUDItem *hitem);
static void DetermineScore(int precord_num, int column_num, char *buffer, int buffer_size);
static bool compare_slots(int a, int b);
static int Highlight_bmp = -1;

static void OnPLRInterval(void);
static void OnPLRInit(void);

void DetermineScore(int precord_num, int column_num, char *buffer, int buffer_size) {
  player_record *pr = DMFCBase->GetPlayerRecord(precord_num);
  if (!pr || pr->state == STATE_EMPTY) {
    buffer[0] = '\0';
    return;
  }
  tPlayerStat *stat = (tPlayerStat *)pr->user_info;

  snprintf(buffer, buffer_size, "%d", stat->Score[DSTAT_LEVEL]);
}

// This function gets called by the game when it wants to learn some info about the game
void DLLFUNCCALL DLLGetGameInfo(tDLLOptions *options) {
  options->flags = DOF_MAXTEAMS;
  options->max_teams = 1;
  strcpy(options->game_name, TXT_COOP);
  strcpy(options->requirements, "COOP");
}

// Initializes the game function pointers
void DLLFUNCCALL DLLGameInit(int *api_func, uint8_t *all_ok, int num_teams_to_use) {
  *all_ok = 1;
  DMFCBase = CreateDMFC();
  if (!DMFCBase) {
    *all_ok = 0;
    return;
  }

  dstat = CreateDmfcStats();
  if (!dstat) {
    *all_ok = 0;
    return;
  }

  DMFCBase->LoadFunctions(api_func);

  // Setup event handlers
  DMFCBase->Set_OnHUDInterval(OnHUDInterval);
  DMFCBase->Set_OnInterval(OnInterval);
  DMFCBase->Set_OnKeypress(OnKeypress);
  DMFCBase->Set_OnClientPlayerEntersGame(OnClientPlayerEntersGame);
  DMFCBase->Set_OnServerObjectKilled(OnServerObjectKilled);
  DMFCBase->Set_OnClientObjectKilled(OnClientObjectKilled);
  DMFCBase->Set_OnPlayerConnect(OnPlayerConnect);
  DMFCBase->Set_OnServerGameCreated(OnServerGameCreated);
  DMFCBase->Set_OnClientLevelStart(OnClientLevelStart);
  DMFCBase->Set_OnGameStateRequest(OnGameStateRequest);
  DMFCBase->Set_OnSaveStatsToFile(OnSaveStatsToFile);
  DMFCBase->Set_OnLevelEndSaveStatsToFile(OnLevelEndSaveStatsToFile);
  DMFCBase->Set_OnDisconnectSaveStatsToFile(OnDisconnectSaveStatsToFile);
  DMFCBase->Set_OnPrintScores(OnPrintScores);
  DMFCBase->Set_OnPLRInterval(OnPLRInterval);
  DMFCBase->Set_OnPLRInit(OnPLRInit);

  dPlayers = DMFCBase->GetPlayers();

  DLLCreateStringTable("Coop.str", &StringTable, &StringTableSize);
  mprintf(0, "%d strings loaded from string table\n", StringTableSize);
  if (!StringTableSize) {
    *all_ok = 0;
    return;
  }

  DMFCBase->GameInit(1);

  Highlight_bmp = DLLbm_AllocBitmap(32, 32, 0);
  if (Highlight_bmp > BAD_BITMAP_HANDLE) {
    uint16_t *data = DLLbm_data(Highlight_bmp, 0);
    if (!data) {
      // bail on out of here
      *all_ok = 0;
      return;
    }
    for (int x = 0; x < 32 * 32; x++) {
      data[x] = GR_RGB16(50, 50, 50) | OPAQUE_FLAG;
    }
  }

  DMFCBase->AddHUDItemCallback(HI_TEXT, DisplayHUDScores);
  DMFCBase->SetupPlayerRecord(sizeof(tPlayerStat), (int (*)(void *, uint8_t *))pack_pstat,
                              (int (*)(void *, uint8_t *))unpack_pstat);
  DMFCBase->SetNumberOfTeams(1);

  netgame_info *netgameinfo = DMFCBase->GetNetgameInfo();
  netgameinfo->flags |= (NF_USE_ROBOTS | NF_RESPAWN_WAYPOINT | NF_ALLOWGUIDEBOT | NF_COOP);
  netgameinfo->max_players = std::min(4, netgameinfo->max_players);
  DMFCBase->SetMaxPlayerHardLimit(4);

  // Initialize the Stats Manager
  // ----------------------------
  tDmfcStatsInit tsi;
  tDmfcStatsColumnInfo pl_col[5];
  char gname[20];
  strcpy(gname, TXT_STATGAMENAME);

  tsi.flags = DSIF_SHOW_PIC | DSIF_SHOW_OBSERVERICON | DSIF_NOLASTKILLER | DSIF_NOLASTVICTIM | DSIF_NODETAILEDINFO |
              DSIF_SHOWPLAYERVIEW;
  tsi.cColumnCountDetailed = 0;
  tsi.cColumnCountPlayerList = 5;
  tsi.clbDetailedColumn = NULL;
  tsi.clbDetailedColumnBMP = NULL;
  tsi.clbPlayerColumn = DetermineScore;
  tsi.clbPlayerColumnBMP = NULL;
  tsi.DetailedColumns = NULL;
  tsi.GameName = gname;
  tsi.MaxNumberDisplayed = NULL;
  tsi.PlayerListColumns = pl_col;
  tsi.SortedPlayerRecords = SortedPlayers;

  pl_col[0].color_type = DSCOLOR_SHIPCOLOR;
  strcpy(pl_col[0].title, TXT_PILOT);
  pl_col[0].type = DSCOL_PILOT_NAME;
  pl_col[0].width = 84;

  pl_col[1].color_type = DSCOLOR_SHIPCOLOR;
  strcpy(pl_col[1].title, TXT_KILLS);
  pl_col[1].type = DSCOL_CUSTOM;
  pl_col[1].width = 51;

  pl_col[2].color_type = DSCOLOR_SHIPCOLOR;
  strcpy(pl_col[2].title, TXT_DEATHS);
  pl_col[2].type = DSCOL_DEATHS_LEVEL;
  pl_col[2].width = 64;

  pl_col[3].color_type = DSCOLOR_SHIPCOLOR;
  strcpy(pl_col[3].title, TXT_SUICIDES);
  pl_col[3].type = DSCOL_SUICIDES_LEVEL;
  pl_col[3].width = 69;

  pl_col[4].color_type = DSCOLOR_SHIPCOLOR;
  strcpy(pl_col[4].title, TXT_PING);
  pl_col[4].type = DSCOL_PING;
  pl_col[4].width = 40;

  dstat->Initialize(&tsi);
}
// Called when the DLL is shutdown
void DLLFUNCCALL DLLGameClose() {
  if (Highlight_bmp > BAD_BITMAP_HANDLE)
    DLLbm_FreeBitmap(Highlight_bmp);

  DLLDestroyStringTable(StringTable, StringTableSize);

  if (dstat) {
    dstat->DestroyPointer();
    dstat = NULL;
  }

  if (DMFCBase) {
    DMFCBase->GameClose();
    DMFCBase->DestroyPointer();
    DMFCBase = NULL;
  }
}

void OnHUDInterval(void) {
  dstat->DoFrame();
  DMFCBase->DisplayOutrageLogo();
  DMFCBase->OnHUDInterval();
}

bool compare_slots(int a, int b) {
  int ascore, bscore;
  player_record *apr, *bpr;
  tPlayerStat *astat, *bstat;

  apr = DMFCBase->GetPlayerRecord(a);
  bpr = DMFCBase->GetPlayerRecord(b);
  if (!apr)
    return true;
  if (!bpr)
    return false;
  if (apr->state == STATE_EMPTY)
    return true;
  if (bpr->state == STATE_EMPTY)
    return false;
  astat = (tPlayerStat *)apr->user_info;
  bstat = (tPlayerStat *)bpr->user_info;

  if ((apr->state == STATE_INGAME) && (bpr->state == STATE_INGAME)) {
    // both players were in the game
    ascore = (astat) ? astat->Score[DSTAT_LEVEL] : 0;
    bscore = (bstat) ? bstat->Score[DSTAT_LEVEL] : 0;
    return (ascore < bscore);
  }
  if ((apr->state == STATE_INGAME) && (bpr->state == STATE_DISCONNECTED)) {
    // apr gets priority since he was in the game on exit
    return false;
  }
  if ((apr->state == STATE_DISCONNECTED) && (bpr->state == STATE_INGAME)) {
    // bpr gets priority since he was in the game on exit
    return true;
  }
  // if we got here then both players were disconnected
  ascore = (astat) ? astat->Score[DSTAT_LEVEL] : 0;
  bscore = (bstat) ? bstat->Score[DSTAT_LEVEL] : 0;
  return (ascore < bscore);
}

void OnInterval(void) {
  // sort players
  int i, t, j;

  for (i = 0; i < MAX_PLAYER_RECORDS; i++) {
    SortedPlayers[i] = i;
  }

  for (i = 1; i <= MAX_PLAYER_RECORDS - 1; i++) {
    t = SortedPlayers[i];
    // Shift elements down until
    // insertion point found.
    for (j = i - 1; j >= 0 && compare_slots(SortedPlayers[j], t); j--) {
      SortedPlayers[j + 1] = SortedPlayers[j];
    }
    // insert
    SortedPlayers[j + 1] = t;
  }

  FirstFrame = true;
  DMFCBase->OnInterval();
}

void OnKeypress(int key) {
  dllinfo *Data = DMFCBase->GetDLLInfoCallData();

  switch (key) {
  case K_F7:
    DisplayScoreScreen = !DisplayScoreScreen;
    DMFCBase->EnableOnScreenMenu(false);
    dstat->Enable(DisplayScoreScreen);
    break;
  case K_PAGEDOWN:
    if (DisplayScoreScreen) {
      dstat->ScrollDown();
      Data->iRet = 1;
    }
    break;
  case K_PAGEUP:
    if (DisplayScoreScreen) {
      dstat->ScrollUp();
      Data->iRet = 1;
    }
    break;
  case K_F6:
    DisplayScoreScreen = false;
    dstat->Enable(false);
    break;
  case K_ESC:
    if (DisplayScoreScreen) {
      dstat->Enable(false);
      DisplayScoreScreen = false;
      Data->iRet = 1;
    }
    break;
  }

  DMFCBase->OnKeypress(key);
}

void OnClientPlayerEntersGame(int player_num) {
  if (player_num == DMFCBase->GetPlayerNum()) {
    FirstFrame = false;
    DisplayScoreScreen = false;
  }

  DMFCBase->OnClientPlayerEntersGame(player_num);

  if (player_num != DMFCBase->GetPlayerNum())
    DisplayWelcomeMessage(player_num);
  else
    display_my_welcome = true;
}

// A new player has entered the game, zero there stats out
void OnPlayerConnect(int player_num) {
  DMFCBase->OnPlayerConnect(player_num);

  tPlayerStat *stat = (tPlayerStat *)DMFCBase->GetPlayerRecordData(player_num);
  if (stat) {
    stat->Score[DSTAT_LEVEL] = 0;
    stat->Score[DSTAT_OVERALL] = 0;
  }
}

// The server has just started, so clear out all the stats and game info
void OnServerGameCreated(void) {
  DMFCBase->OnServerGameCreated();
  player_record *pr;
  tPlayerStat *stat;
  for (int i = 0; i < MAX_PLAYER_RECORDS; i++) {
    pr = DMFCBase->GetPlayerRecord(i);
    if (pr)
      stat = (tPlayerStat *)pr->user_info;
    else
      stat = NULL;
    if (stat) {
      stat->Score[DSTAT_LEVEL] = 0;
      stat->Score[DSTAT_OVERALL] = 0;
    }
  }
}

// The server has started a new level, so clear out level scores
void OnClientLevelStart(void) {
  DMFCBase->OnClientLevelStart();
  player_record *pr;
  tPlayerStat *stat;
  for (int i = 0; i < MAX_PLAYER_RECORDS; i++) {
    pr = DMFCBase->GetPlayerRecord(i);
    if (pr)
      stat = (tPlayerStat *)pr->user_info;
    else
      stat = NULL;

    if (stat) {
      stat->Score[DSTAT_LEVEL] = 0;
    }
  }
  DMFCBase->RequestGameState();
}

// We need to send all the inventory info out to the new player
void OnGameStateRequest(int player_num) { DMFCBase->OnGameStateRequest(player_num); }

void DisplayWelcomeMessage(int player_num) {
  char name_buffer[64];
  strcpy(name_buffer, (DMFCBase->GetPlayers())[player_num].callsign);

  if (player_num == DMFCBase->GetPlayerNum()) {
    DLLAddHUDMessage(TXT_WELCOME, name_buffer);
  } else {
    DLLAddHUDMessage(TXT_JOINED, name_buffer);
  }
}

void OnServerObjectKilled(object *obj, object *killer) {
  if (!killer)
    return; // can't handle this

  if (obj->type == OBJ_ROBOT || (obj->type == OBJ_BUILDING && obj->ai_info)) {

    // check to see if the killer is a player
    bool ok_to_call = false;

    if (killer->type == OBJ_PLAYER) {
      ok_to_call = true;
    } else if (killer->type == OBJ_ROBOT) {
      if (DMFCBase->GetCounterMeasureOwner(killer) != -1) {
        ok_to_call = true;
      }
    }

    if (ok_to_call) {
      DMFCBase->CallClientEvent(EVT_CLIENT_GAMEOBJKILLED, DMFCBase->GetMeObjNum(), DMFCBase->GetItObjNum(), -1);
      DMFCBase->CallOnClientObjectKilled(obj, killer);
    }
  }

  DMFCBase->OnServerObjectKilled(obj, killer);
}

void OnClientObjectKilled(object *obj, object *killer) {
  object *parent;
  DLLGetUltimateParentForObject(&parent, killer);

  if ((parent->type != OBJ_PLAYER) && (parent->type != OBJ_OBSERVER)) {
    mprintf(0, "Robot killed wasn't by a OBJ_PLAYER or OBJ_OBSERVER (%d)\n", parent->type);
    return;
  }

  int killer_pnum = parent->id;

  player_record *pr = DMFCBase->GetPlayerRecordByPnum(killer_pnum);
  if (!pr) {
    mprintf(0, "Invalid player record!\n");
    Int3();
    return;
  }

  tPlayerStat *stat = (tPlayerStat *)pr->user_info;

  stat->Score[DSTAT_LEVEL]++;
  stat->Score[DSTAT_OVERALL]++;

  mprintf(0, "%s's score is now %d\n", dPlayers[killer_pnum].callsign, stat->Score[DSTAT_LEVEL]);

  DMFCBase->OnClientObjectKilled(obj, killer);
}

void DisplayHUDScores(struct tHUDItem *hitem) {
  if (!FirstFrame)
    return;

  if (display_my_welcome) {
    DisplayWelcomeMessage(DMFCBase->GetPlayerNum());
    display_my_welcome = false;
  }

  if (DisplayScoreScreen)
    return;

  if (DMFCBase->IAmDedicatedServer())
    return;

  int old_font = DLLgrtext_GetFont();
  int start_y;
  int i;

  DLLgrtext_SetFont((DMFCBase->GetGameFontTranslateArray())[HUD_FONT_INDEX]);
  int height = DLLRenderHUDGetTextHeight("X") + 1;
  int players_in_game = 0;

  for (i = 0; i < DLLMAX_PLAYERS; i++) {
    if (DMFCBase->CheckPlayerNum(i) && !DMFCBase->IsPlayerDedicatedServer(i)) {
      players_in_game++;
    }
  }

  if (players_in_game <= 0)
    return;

  players_in_game = std::min(players_in_game, 8);

  start_y = (DMFCBase->GetGameWindowH() / 2) - ((players_in_game * 5) / 2);
  // determine coordinates to use here
  // we'll use a virtual width of 85 pixels on a 640x480 screen
  // so first determine the new width
  int name_width = 85.0f * DMFCBase->GetHudAspectX();
  int name_x = DMFCBase->GetGameWindowW() - name_width - 10;

  int pnum, index = 0;
  uint8_t alpha = DMFCBase->ConvertHUDAlpha((uint8_t)((DisplayScoreScreen) ? 128 : 255));

  DLLgrtext_SetAlpha(alpha);
  char name[64];

  // Display only the players in the game
  while ((players_in_game > 0) && (index < MAX_PLAYER_RECORDS)) {
    pnum = SortedPlayers[index];

    if (DMFCBase->CheckPlayerNum(pnum) && !DMFCBase->IsPlayerDedicatedServer(pnum)) {
      // valid player
      if (pnum == DMFCBase->GetPlayerNum()) {
        if (Highlight_bmp > BAD_BITMAP_HANDLE) {
          // draw the highlite bar in the background
          DLLrend_SetAlphaValue(alpha * 0.50f);
          DLLrend_SetZBufferState(0);
          DLLrend_SetTextureType(TT_LINEAR);
          DLLrend_SetLighting(LS_NONE);
          DLLrend_SetAlphaType(AT_CONSTANT_TEXTURE);

          DLLrend_DrawScaledBitmap(name_x - 2, start_y - 2, DMFCBase->GetGameWindowW() - 8, start_y + height + 1,
                                   Highlight_bmp, 0, 0, 1, 1, 1.0, -1, NULL);
          DLLrend_SetZBufferState(1);
        }
      }

      strcpy(name, dPlayers[pnum].callsign);
      DMFCBase->ClipString(name_width, name, true);

      DLLgrtext_SetColor((DMFCBase->GetPlayerColors())[pnum]);
      DLLgrtext_Printf(name_x, start_y, name);

      start_y += height;
      players_in_game--;
    }
    index++;
  }

  DLLgrtext_SetFont(old_font);
}

void SaveStatsToFile(char *filename) {
  /*
  CFILE *file;
  DLLOpenCFILE(&file,filename,"wt");
  if(!file){
          mprintf(0,"Unable to open output file\n");
          return;
  }

  //write out game stats
  #define BUFSIZE	150
  char buffer[BUFSIZE];
  char tempbuffer[25];
  int sortedslots[MAX_PLAYER_RECORDS];
  player_record *pr,*dpr;
  tPInfoStat stat;
  int count,length,p;

  count = 1;

  sprintf(buffer,TXT_SAVE_HEADER,Netgame->name,Current_mission->cur_level);
  DLLcf_WriteString(file,buffer);

  sprintf(buffer,TXT_SAVE_HEADERB);
  DLLcf_WriteString(file,buffer);
  strcpy(buffer,"-----------------------------------------------------------------------------------");
  DLLcf_WriteString(file,buffer);


  tPlayerStat *st;
  for(int s=0;s<MAX_PLAYER_RECORDS;s++){
          p = SortedPlayers[s];
          pr = GetPlayerRecord(p);
          if( pr && pr->state!=STATE_EMPTY) {

                  if(IsPlayerDedicatedServer(pr))
                          continue;	//skip dedicated server

                  st = (tPlayerStat *)pr->user_info;

                  memset(buffer,' ',BUFSIZE);

                  sprintf(tempbuffer,"%d)",count);
                  memcpy(&buffer[0],tempbuffer,strlen(tempbuffer));

                  sprintf(tempbuffer,"%s%s",(pr->state==STATE_INGAME)?"":"*",pr->callsign);
                  memcpy(&buffer[7],tempbuffer,strlen(tempbuffer));

                  sprintf(tempbuffer,"%d[%d]",st->Score[DSTAT_LEVEL],st->Score[DSTAT_OVERALL]);
                  memcpy(&buffer[36],tempbuffer,strlen(tempbuffer));

                  sprintf(tempbuffer,"%d[%d]",pr->dstats.deaths[DSTAT_LEVEL],pr->dstats.deaths[DSTAT_OVERALL]);
                  memcpy(&buffer[48],tempbuffer,strlen(tempbuffer));

                  sprintf(tempbuffer,"%d[%d]",pr->dstats.suicides[DSTAT_LEVEL],pr->dstats.suicides[DSTAT_OVERALL]);
                  memcpy(&buffer[60],tempbuffer,strlen(tempbuffer));

                  sprintf(tempbuffer,"%s",GetTimeString(GetTimeInGame(p)));
                  memcpy(&buffer[71],tempbuffer,strlen(tempbuffer));

                  int pos;
                  pos = 71 + strlen(tempbuffer) + 1;
                  if(pos<BUFSIZE)
                          buffer[pos] = '\0';

                  buffer[BUFSIZE-1] = '\0';
                  DLLcf_WriteString(file,buffer);
                  count++;
          }
  }

  //done writing stats
  DLLcfclose(file);
  DLLAddHUDMessage(TXT_MSG_SAVESTATS);
  */
}

#define ROOTFILENAME "D3Coop"
void OnSaveStatsToFile(void) {
  char filename[256];
  DMFCBase->GenerateStatFilename(filename, ROOTFILENAME, false);
  SaveStatsToFile(filename);
}

void OnLevelEndSaveStatsToFile(void) {
  char filename[256];
  DMFCBase->GenerateStatFilename(filename, ROOTFILENAME, true);
  SaveStatsToFile(filename);
}

void OnDisconnectSaveStatsToFile(void) {
  char filename[256];
  DMFCBase->GenerateStatFilename(filename, ROOTFILENAME, false);
  SaveStatsToFile(filename);
}

void OnPrintScores(int level) {
  /*
  char buffer[256];
  char name[50];
  memset(buffer,' ',256);

  int t;
  int pos[5];
  int len[5];
  pos[0] = 0;					t = len[0] = 20;	//give ample room for pilot name
  pos[1] = pos[0] + t + 1;	t = len[1] = strlen(TXT_KILLS_SHORT);
  pos[2] = pos[1] + t + 1;	t = len[2] = strlen(TXT_DEATHS_SHORT);
  pos[3] = pos[2] + t + 1;	t = len[3] = strlen(TXT_SUICIDES_SHORT);
  pos[4] = pos[3] + t + 1;	t = len[4] = strlen(TXT_PING);

  memcpy(&buffer[pos[0]],TXT_PILOT,strlen(TXT_PILOT));
  memcpy(&buffer[pos[1]],TXT_KILLS_SHORT,len[1]);
  memcpy(&buffer[pos[2]],TXT_DEATHS_SHORT,len[2]);
  memcpy(&buffer[pos[3]],TXT_SUICIDES_SHORT,len[3]);
  memcpy(&buffer[pos[4]],TXT_PING,len[4]);
  buffer[pos[4]+len[4]+1] = '\n';
  buffer[pos[4]+len[4]+2] = '\0';
  DPrintf(buffer);

  int slot;
  player_record *pr;
  int pcount;

  if(level<0 || level>=MAX_PLAYER_RECORDS)
          pcount = MAX_PLAYER_RECORDS;
  else
          pcount = level;

  tPlayerStat *st;

  for(int i=0;i<pcount;i++){
          slot = SortedPlayers[i];
          pr = GetPlayerRecord(slot);
          if((pr)&&(pr->state!=STATE_EMPTY)){

                  if(IsPlayerDedicatedServer(pr))
                          continue;	//skip dedicated server

                  st = (tPlayerStat *)pr->user_info;

                  sprintf(name,"%s%s:",(pr->state==STATE_DISCONNECTED)?"*":"",pr->callsign);
                  name[19] = '\0';

                  memset(buffer,' ',256);
                  t = strlen(name); memcpy(&buffer[pos[0]],name,(t<len[0])?t:len[0]);
                  sprintf(name,"%d",st->Score[DSTAT_LEVEL]);
                  t = strlen(name); memcpy(&buffer[pos[1]],name,(t<len[1])?t:len[1]);
                  sprintf(name,"%d",pr->dstats.deaths[DSTAT_LEVEL]);
                  t = strlen(name); memcpy(&buffer[pos[2]],name,(t<len[2])?t:len[2]);
                  sprintf(name,"%d",pr->dstats.suicides[DSTAT_LEVEL]);
                  t = strlen(name); memcpy(&buffer[pos[3]],name,(t<len[3])?t:len[3]);

                  if(pr->state==STATE_INGAME)
                          sprintf(name,"%.0f",NetPlayers[pr->pnum].ping_time*1000.0f);
                  else
                          strcpy(name,"---");
                  t = strlen(name); memcpy(&buffer[pos[4]],name,(t<len[4])?t:len[4]);
                  buffer[pos[4]+len[4]+1] = '\n';
                  buffer[pos[4]+len[4]+2] = '\0';
                  DPrintf(buffer);
          }
  }
  */
}

static float PLRElapsedtime = 0.0f;
static float PLRLasttime = 0.0f;

void OnPLRInit(void) {
  PLRElapsedtime = 0.0f;
  PLRLasttime = DMFCBase->GetGametime();
}

void OnPLRInterval(void) {
  level_info *li = DMFCBase->GetLevelInfo();

  int height = DLLgrfont_GetHeight((DMFCBase->GetGameFontTranslateArray())[BIG_FONT_INDEX]);
  DLLgrtext_SetFont((DMFCBase->GetGameFontTranslateArray())[BIG_FONT_INDEX]);

  int y = 240;

  float ft, newtime;
  newtime = DMFCBase->GetGametime();
  ft = newtime - PLRLasttime;
  PLRLasttime = newtime;
  PLRElapsedtime += ft;

  float alpha;

  if (PLRElapsedtime > 5.0f) {
    alpha = 1.0f;
  } else {
    alpha = (5.0f - PLRElapsedtime) / 5.0f;
  }

  DLLrend_ClearScreen(GR_BLACK);
  DLLgrtext_SetColor(GR_RGB(40, 40, 255));
  DLLgrtext_CenteredPrintf(0, y - height - 1, TXT_LEVELCOMPLETE);

  if (li) {
    DLLgrtext_SetAlpha((uint8_t)(alpha * 255.0f));
    DLLgrtext_SetColor(GR_RGB(255, 255, 255));
    DLLgrtext_CenteredPrintf(0, y + 1, li->name);
  }

  DLLgrtext_Flush();
}
