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

 * $Logfile: /DescentIII/Main/Dmfc/dmfcstats.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:57:21 $
 * $Author: kevinb $
 *
 * Functions for in-game stats system
 *
 * $Log: dmfcstats.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:57:21  kevinb
 * initial 1.5 import
 *
 *
 * 34    10/26/99 10:32a Jeff
 * fixed COM interface bug
 *
 * 33    9/03/99 5:48p Jeff
 * fixed crash if the stats is brought up before all player records are
 * recieved
 *
 * 32    7/14/99 11:48a Jeff
 * localized text for patch fixes
 *
 * 31    7/11/99 6:09p Jeff
 * for team games (or where the divide up by teams flag is set), process
 * everyone in the game and display them, before the disconnected people,
 * so everyone is displayed.
 *
 * 30    7/09/99 6:17p Jeff
 * fixed bugs when scrolling down in a dedicated server game
 *
 * 29    7/06/99 11:47p Jeff
 * selected pilot when stats are enabled is yourself, not server
 *
 * 28    5/23/99 3:04a Jason
 * fixed bug with player rankings not being updated correctly
 *
 * 27    5/22/99 1:12a Jeff
 * correctly handle Viewer_object
 *
 * 26    5/19/99 5:26p Jeff
 * fixed crash trying to view stats before getting player records from
 * server
 *
 * 25    5/13/99 4:55p Ardussi
 * changes for compiling on the Mac
 *
 * 24    5/03/99 12:07p Jeff
 * safety precautions
 *
 * 23    3/29/99 7:20p Jeff
 * added vis stuff for coop stat views
 *
 * 22    3/27/99 4:53p Jeff
 * player rankings in multiplayer games implemented.  Fixed join message
 * so it doesn't get cut off
 *
 * 21    3/17/99 12:25p Jeff
 * converted DMFC to be COM interface
 *
 * 20    3/05/99 1:30p Jeff
 * fixed 99% of the high res issues
 *
 * 19    2/11/99 12:51a Jeff
 * changed names of exported variables
 *
 * 18    2/08/99 7:49p Jeff
 * removed calls to get frame parameters (function is dead..pending
 * removal)
 *
 * 17    2/08/99 5:22p Jeff
 * exported rend_SetZBuffer...fixed up small view of stats
 *
 * 16    2/08/99 12:06a Jeff
 * some new flags for more configuration. Added an option to  see from a
 * player's view
 *
 * 15    2/06/99 6:59p Jeff
 * created RenderHUDGetTextLineWidth and RenderHUDGetTextHeight
 *
 * 14    1/31/99 7:26p Matt
 * Renamed a bunch of functions to have HUD capitalized
 *
 * 13    1/24/99 8:31p Jeff
 * updated stat manager to show team information
 *
 * 12    1/20/99 8:07p Jeff
 * disconnected players in color mode team show up as grey
 *
 * 11    1/19/99 3:55a Jeff
 * all strings localized out
 *
 * 10    1/16/99 2:55p Jeff
 * possibly fixed some stat manager bugs...needs testing
 *
 * 9     1/13/99 4:36p Jeff
 * fixed scroll bug
 *
 * 8     1/07/99 5:01p Jeff
 * added Int3's and updated all net games to use stats manager...correctly
 * too
 *
 * 7     1/04/99 12:21p Jeff
 * added support for hosts.allow/deny and updates stats manager a little
 *
 * 6     12/08/98 4:47p Jeff
 * umm, various changes, fixed pilot pics so that they work for everyone
 * now
 *
 * 5     12/08/98 3:29p Jeff
 * updated the team control dialog so the server can determine if they
 * want to make the clients wait
 *
 * 4     12/08/98 12:17p Jeff
 * various changes that include an improved Team Control dialog (doesn't
 * switch teams until exit..) and spew/respawn players that change teams
 *
 * 3     12/04/98 7:04p Jeff
 * almost finished up dmfc stat manager
 *
 * 2     12/03/98 7:05p Jeff
 * updated new stats
 *
 * $NoKeywords: $
 */

#include "gamedll_header.h"
#include "DMFC.h"
#include "dmfcinternal.h"


#include <algorithm>

extern char **DMFCStringTable;
extern int DMFCStringTableSize;
extern const char *_DMFCErrorString;
extern DMFCBase *basethis;
const char *DMFCGetString(int d);
extern player_record Player_records[MAX_PLAYER_RECORDS];

#define ROW_PIXEL_GAP 2
#define PAGE_DISP_TIME 2.0f
#define ALPHA_IN_TIME 0.4f

#define PLIST_WIN_W 400
#define PLIST_WIN_H 440
#define PLIST_WIN_X 10
#define PLIST_WIN_Y 20
#define DLIST_WIN_W 210
#define DLIST_WIN_H 440
#define DLIST_WIN_X PLIST_WIN_W + PLIST_WIN_X + 10
#define DLIST_WIN_Y 20

// constructor
CDmfcStats::CDmfcStats() {
  m_cPtrs = 0;
  m_init = false;
  m_flags = 0;
  m_SortedPRecs = NULL;
  m_pMaxNumberDisplayed = NULL;
  m_MaxCount = 0;
  m_iPLColCount = 0;
  m_iDColCount = 0;
  m_PLColInfo = NULL;
  m_DColInfo = NULL;
  m_PLColCallback = NULL;
  m_DColCallback = NULL;
  m_PLColCallbackBMP = NULL;
  m_DColCallbackBMP = NULL;
  m_TeamCallback = NULL;
  m_GameName = NULL;
  m_top_index = 0;
  m_selected_index = 0;
  m_bottom_index = -1;
  m_enabled = false;
  m_font_height = 0;
  m_y_start_plist = 0;
  m_y_start_detail = 0;
  m_x_start_plist = 0;
  m_x_start_detail = 0;
  m_selected_prec = 0;
  m_highlight_bmp = BAD_BITMAP_HANDLE;
  m_background_bmp = BAD_BITMAP_HANDLE;
  m_page_index = 0;
  m_can_fit_count = -1;
  m_page_display_time = 0;
  m_has_multiple_pages = false;
  m_alpha = 0;
  alpha_in_time = 0;
}

// destructor
CDmfcStats::~CDmfcStats() {
  if (m_GameName) {
    free(m_GameName);
  }
  if (m_PLColInfo) {
    free(m_PLColInfo);
  }
  if (m_DColInfo) {
    free(m_DColInfo);
  }

  if (m_highlight_bmp > BAD_BITMAP_HANDLE)
    DLLbm_FreeBitmap(m_highlight_bmp);

  if (m_background_bmp > BAD_BITMAP_HANDLE)
    DLLbm_FreeBitmap(m_background_bmp);
}

// initialization member function
bool CDmfcStats::Initialize(tDmfcStatsInit *init_info) {
  m_font_height = DLLgrfont_GetHeight(basethis->Game_fonts[HUD_FONT_INDEX]);
  // m_font_height			= DLLRenderHUDGetTextHeight("X");
  m_flags = init_info->flags;
  m_SortedPRecs = init_info->SortedPlayerRecords;
  if (!m_SortedPRecs)
    goto error_init;
  m_pMaxNumberDisplayed = init_info->MaxNumberDisplayed;
  if ((m_flags & DSIF_ONLY_X_PLAYERS_SHOWN) && (!m_pMaxNumberDisplayed))
    goto error_init;

  m_PLColCallback = init_info->clbPlayerColumn;
  m_DColCallback = init_info->clbDetailedColumn;
  m_PLColCallbackBMP = init_info->clbPlayerColumnBMP;
  m_DColCallbackBMP = init_info->clbDetailedColumnBMP;
  m_TeamCallback = init_info->clTeamLine;

  m_iPLColCount = init_info->cColumnCountPlayerList;
  m_iDColCount = init_info->cColumnCountDetailed;

  if (m_iPLColCount > 0 && !init_info->PlayerListColumns)
    goto error_init;

  if (m_iDColCount > 0 && !init_info->DetailedColumns)
    goto error_init;

  if (m_iPLColCount > 0) {
    m_PLColInfo = (tDmfcStatsColumnInfo *)malloc(sizeof(tDmfcStatsColumnInfo) * m_iPLColCount);
    if (!m_PLColInfo)
      goto error_init;
    memcpy(m_PLColInfo, init_info->PlayerListColumns, sizeof(tDmfcStatsColumnInfo) * m_iPLColCount);
  }

  if (m_iDColCount > 0) {
    m_DColInfo = (tDmfcStatsColumnInfo *)malloc(sizeof(tDmfcStatsColumnInfo) * m_iDColCount);
    if (!m_DColInfo) {
      goto error_init;
    }
    memcpy(m_DColInfo, init_info->DetailedColumns, sizeof(tDmfcStatsColumnInfo) * m_iDColCount);
  }

  if (init_info->GameName) {
    m_GameName = strdup(init_info->GameName);
  }

  m_y_start_plist = PLIST_WIN_Y + 2 * (m_font_height + ROW_PIXEL_GAP);
  m_y_start_detail = DLIST_WIN_Y + (m_font_height + ROW_PIXEL_GAP);
  m_x_start_plist = PLIST_WIN_X;
  m_x_start_detail = DLIST_WIN_X;

  m_highlight_bmp = DLLbm_AllocBitmap(32, 32, 0);
  m_background_bmp = DLLbm_AllocBitmap(32, 32, 0);
  ushort *data;

  if (m_highlight_bmp == -1)
    m_highlight_bmp = BAD_BITMAP_HANDLE;
  if (m_background_bmp == -1)
    m_background_bmp = BAD_BITMAP_HANDLE;

  if (m_highlight_bmp > BAD_BITMAP_HANDLE) {
    data = DLLbm_data(m_highlight_bmp, 0);
    for (int x = 0; x < 32 * 32; x++) {
      data[x] = GR_RGB16(50, 50, 50) | OPAQUE_FLAG;
    }
  }

  if (m_background_bmp > BAD_BITMAP_HANDLE) {
    data = DLLbm_data(m_background_bmp, 0);
    for (int x = 0; x < 32 * 32; x++) {
      data[x] = GR_RGB16(0, 0, 0) | OPAQUE_FLAG;
    }
  }

  m_init = true;
  return true;

error_init:
  if (m_GameName) {
    free(m_GameName);
  }
  if (m_PLColInfo) {
    free(m_PLColInfo);
  }
  if (m_DColInfo) {
    free(m_DColInfo);
  }
  return false;
}

//=================================

// enables/disables the displaying of the stats screen
void CDmfcStats::Enable(bool enable) {
  m_enabled = enable;
  if (enable) {
    player_record *pr = PRec_GetPRecordByPnum(basethis->GetPlayerNum());
    if (pr && pr->state != STATE_EMPTY) {
      m_selected_prec = pr - Player_records;
    } else {
      m_selected_prec = 0;
    }
    alpha_in_time = 0;
    m_alpha = 0;
    m_page_index = 0;
    m_can_fit_count = -1;
    m_page_display_time = 0;
    m_has_multiple_pages = false;

    basethis->Players[basethis->GetPlayerNum()].small_dll_obj = -1;
    if (m_flags & DSIF_SHOWPLAYERVIEW) {
      player_record *pr = basethis->GetPlayerRecord(m_selected_prec);
      if (pr && pr->state == STATE_INGAME) {
        basethis->Players[basethis->GetPlayerNum()].small_dll_obj = basethis->Players[pr->pnum].objnum;
      }
    }
  } else {
    basethis->Players[basethis->GetPlayerNum()].small_dll_obj = -1;
  }
}

// returns the state of the stats manager
bool CDmfcStats::IsEnabled() { return m_enabled; }

// scrolls the player list down one
void CDmfcStats::ScrollDown() {
  if (CanScrollDown()) {

    if (m_flags & DSIF_SEPERATE_BY_TEAM) {
      // we need to handle team stats a bit different
      // we must state on connected people until no more
      // connected people exist, then go through the list
      // through disconnected people
      player_record *pr = basethis->GetPlayerRecord(m_selected_prec);
      if (!pr || pr->state == STATE_EMPTY) {
        m_selected_index++;
        m_selected_prec = m_translate[m_selected_index];
      } else {
        bool isingame = (pr->state == STATE_INGAME) ? true : false;

        while (1) {
          m_selected_index++;
          if (m_selected_index == (m_MaxCount - 1)) {
            // we hit the bottom
            m_selected_prec = m_translate[m_selected_index];

            if (!isingame)
              break; // we have gotten to the bottom of everyone

            pr = basethis->GetPlayerRecord(m_selected_prec);

            // check to see if we want this player
            if (pr->state == STATE_INGAME)
              break;

            // now go through the disconnected folk
            isingame = false;
            m_selected_index = -1; // set to -1, since it will be incremented right away
            continue;
          }

          m_selected_prec = m_translate[m_selected_index];
          pr = basethis->GetPlayerRecord(m_selected_prec);

          if (!pr || pr->state == STATE_EMPTY)
            continue;

          // should we use this player?
          if (isingame) {
            if (pr->state != STATE_INGAME)
              continue; // move to the next
          } else {
            if (pr->state != STATE_DISCONNECTED)
              continue; // move to  the next
          }

          // all matches up if we haven't continue by here
          break;
        }
      }
    } else {
      m_selected_index++;
      m_selected_prec = m_translate[m_selected_index];
    }

    m_page_index = 0;
    m_can_fit_count = -1;
    m_page_display_time = 0;
    m_has_multiple_pages = false;

    basethis->Players[basethis->GetPlayerNum()].small_dll_obj = -1;
    if (m_flags & DSIF_SHOWPLAYERVIEW) {
      player_record *pr = basethis->GetPlayerRecord(m_selected_prec);
      if (pr && pr->state == STATE_INGAME) {
        basethis->Players[basethis->GetPlayerNum()].small_dll_obj = basethis->Players[pr->pnum].objnum;
      }
    }
  }
}

// scrolls the player list up one
void CDmfcStats::ScrollUp() {
  if (CanScrollUp()) {
    if (m_flags & DSIF_SEPERATE_BY_TEAM) {
      // we need to handle team stats a bit different
      // we must state on connected people until no more
      // connected people exist, then go through the list
      // through disconnected people
      player_record *pr = basethis->GetPlayerRecord(m_selected_prec);
      if (!pr || pr->state == STATE_EMPTY) {
        m_selected_index--;
        m_selected_prec = m_translate[m_selected_index];
      } else {
        bool isingame = (pr->state == STATE_INGAME) ? true : false;

        while (1) {
          m_selected_index--;
          if (m_selected_index == 0) {
            // we hit the top
            m_selected_prec = m_translate[m_selected_index];

            if (isingame)
              break; // we have gotten to the top of everyone

            pr = basethis->GetPlayerRecord(m_selected_prec);

            // check to see if we want this player
            if (pr->state == STATE_DISCONNECTED)
              break;

            // now go through the connected folk
            isingame = true;
            m_selected_index = m_MaxCount; // set to m_MaxCount, since it will be incremented right away
            continue;
          }

          m_selected_prec = m_translate[m_selected_index];
          pr = basethis->GetPlayerRecord(m_selected_prec);

          if (!pr || pr->state == STATE_EMPTY)
            continue;

          // should we use this player?
          if (isingame) {
            if (pr->state != STATE_INGAME)
              continue; // move to the next
          } else {
            if (pr->state != STATE_DISCONNECTED)
              continue; // move to  the next
          }

          // all matches up if we haven't continue by here
          break;
        }
      }
    } else {
      m_selected_index--;
      m_selected_prec = m_translate[m_selected_index];
    }

    m_page_index = 0;
    m_can_fit_count = -1;
    m_page_display_time = 0;
    m_has_multiple_pages = false;

    basethis->Players[basethis->GetPlayerNum()].small_dll_obj = -1;
    if (m_flags & DSIF_SHOWPLAYERVIEW) {
      player_record *pr = basethis->GetPlayerRecord(m_selected_prec);
      if (pr && pr->state == STATE_INGAME) {
        basethis->Players[basethis->GetPlayerNum()].small_dll_obj = basethis->Players[pr->pnum].objnum;
      }
    }
  }
}

// returns true if the player list can scroll down any more
bool CDmfcStats::CanScrollDown() { return (m_selected_index < (m_MaxCount - 1)) ? true : false; }

// returns true if the player list can scroll up any more
bool CDmfcStats::CanScrollUp() { return (bool)(m_selected_index > 0); }

void drawline(int lx, int rx, int y, float perc) {
  int width = ((float)(rx - lx)) * perc;
  int center = lx + (rx - lx) / 2;

  width = width / 2;
  lx = center - width;
  rx = center + width;

  /*
  rx = lx + width;
  */

  int ny = y;
  basethis->ConvertHUDCoord(lx, y, &lx, &y);
  basethis->ConvertHUDCoord(rx, ny, &rx, &ny);
  DLLrend_DrawLine(lx, y, rx, ny);
}

// renders the stats screen
void CDmfcStats::Render() {
  int i, c, curr_x, curr_y;
  player_record *pr;
  char buffer[256];
  DLLrend_SetFlatColor(GR_WHITE);
  float perc_done = (alpha_in_time < ALPHA_IN_TIME) ? (alpha_in_time / ALPHA_IN_TIME) : 1.0f;
  bool need_to_display_observer_icon;
  bool is_dedicated_server;
  int last_team = -1;
  int teams_displayed = 0;
  bool team_disp[DLLMAX_TEAMS];

  for (i = 0; i < DLLMAX_TEAMS; i++) {
    team_disp[i] = false;
  }

  // render all the player list columns
  curr_x = m_x_start_plist;
  curr_y = PLIST_WIN_Y + 1;

  // render window and headings
  DLLRenderHUDQuad(PLIST_WIN_X, PLIST_WIN_Y, PLIST_WIN_W, PLIST_WIN_H, 0, 0, 1, 1, m_background_bmp, perc_done * 200.0f,
                   0);

  DLLgrtext_SetFont(basethis->Game_fonts[HUD_FONT_INDEX]);
  int l_width = DLLRenderHUDGetTextLineWidth(m_GameName);
  DLLRenderHUDText(GR_WHITE, m_alpha, 0, (PLIST_WIN_W / 2) - (l_width / 2), curr_y, m_GameName);
  curr_y += (m_font_height + ROW_PIXEL_GAP);
  for (c = 0; c < m_iPLColCount; c++) {
    strcpy(buffer, m_PLColInfo[c].title);
    basethis->ClipString(m_PLColInfo[c].width, buffer, false);
    DLLRenderHUDText(GR_WHITE, m_alpha, 0, curr_x, curr_y, buffer);
    curr_x += m_PLColInfo[c].width;
  }
  curr_y += (m_font_height + (ROW_PIXEL_GAP) / 2);
  if (m_iPLColCount > 0)
    drawline(PLIST_WIN_X, PLIST_WIN_X + PLIST_WIN_W - 1, curr_y, perc_done);

  curr_x = m_x_start_plist;
  curr_y = m_y_start_plist + 4;

  bool displaying_in_game_players = true;
  bool not_overflowing = true;
  bool update_bottom_index = true;

redisplay_list:

  for (i = m_top_index; i < m_MaxCount; i++) {
    pr = PRec_GetPRecord(m_translate[i]);

    if (!pr || pr->state == STATE_EMPTY)
      continue;

    if (m_flags & DSIF_SEPERATE_BY_TEAM) {
      // for team games we seperate disconnected and ingame people
      if (displaying_in_game_players) {
        // filter out non-ingame players
        if (pr->state != STATE_INGAME)
          continue;
      } else {
        // filter out non-disconnected players
        if (pr->state != STATE_DISCONNECTED)
          continue;
      }
    }

    is_dedicated_server = basethis->IsPlayerDedicatedServer(pr);

    if ((m_flags & DSIF_SEPERATE_BY_TEAM) && (last_team != pr->team) && !is_dedicated_server) {
      i--; // this line doesn't count
      last_team = pr->team;
      buffer[0] = '\0';

      if (!team_disp[last_team]) {
        teams_displayed++;
        team_disp[last_team] = true;
      }

      if (m_TeamCallback) {
        snprintf(buffer, sizeof(buffer), DTXT_TEAMLABEL, basethis->GetTeamString(pr->team));
        int length = strlen(buffer);
        buffer[length] = ' '; // lose the \0

        (*m_TeamCallback)(pr->team, &buffer[length], sizeof(buffer) - length - 1);
        buffer[sizeof(buffer) - 1] = '\0';

        int width = (PLIST_WIN_X + PLIST_WIN_W - curr_x - 5);
        float ratio = DEFAULT_HUD_WIDTH / ((float)*basethis->Game_window_w);

        // clip the string to the width
        basethis->ClipString(width, buffer, false);

        int l = ratio * ((float)DLLRenderHUDGetTextLineWidth(buffer));
        int x = (width / 2) - (l / 2);

        DLLRenderHUDText(basethis->GetTeamColor(pr->team), m_alpha, 0, x, curr_y, buffer);
      }

      goto skip_line; // don't process this line
    }

    // draw the highlight bar if selected bitmap
    if (m_selected_index == i) {
      DLLRenderHUDQuad(m_x_start_plist, curr_y, PLIST_WIN_W, m_font_height, 0, 0, 1, 1, m_highlight_bmp,
                       perc_done * 128.0f, 0);
    }

    need_to_display_observer_icon =
        (pr->state == STATE_INGAME) ? (basethis->IsPlayerObserver(pr->pnum) ? true : false) : false;

    for (c = 0; c < m_iPLColCount; c++) {
      buffer[0] = '\0';

      if (!is_dedicated_server || m_PLColInfo[c].type == DSCOL_PILOT_NAME || m_PLColInfo[c].type == DSCOL_BLANK) {
        switch (m_PLColInfo[c].type) {
        case DSCOL_BMP:
          break;
        case DSCOL_KILLS_LEVEL:
          snprintf(buffer, sizeof(buffer), "%d", pr->dstats.kills[DSTAT_LEVEL]);
          break;
        case DSCOL_KILLS_OVERALL:
          snprintf(buffer, sizeof(buffer), "%d", pr->dstats.kills[DSTAT_OVERALL]);
          break;
        case DSCOL_KILLS_BOTH:
          snprintf(buffer, sizeof(buffer), "%d[%d]", pr->dstats.kills[DSTAT_LEVEL], pr->dstats.kills[DSTAT_OVERALL]);
          break;
        case DSCOL_DEATHS_LEVEL:
          snprintf(buffer, sizeof(buffer), "%d", pr->dstats.deaths[DSTAT_LEVEL]);
          break;
        case DSCOL_DEATHS_OVERALL:
          snprintf(buffer, sizeof(buffer), "%d", pr->dstats.deaths[DSTAT_OVERALL]);
          break;
        case DSCOL_DEATHS_BOTH:
          snprintf(buffer, sizeof(buffer), "%d[%d]", pr->dstats.deaths[DSTAT_LEVEL], pr->dstats.deaths[DSTAT_OVERALL]);
          break;
        case DSCOL_SUICIDES_LEVEL:
          snprintf(buffer, sizeof(buffer), "%d", pr->dstats.suicides[DSTAT_LEVEL]);
          break;
        case DSCOL_SUICIDES_OVERALL:
          snprintf(buffer, sizeof(buffer), "%d", pr->dstats.suicides[DSTAT_OVERALL]);
          break;
        case DSCOL_SUICIDES_BOTH:
          snprintf(buffer, sizeof(buffer), "%d[%d]", pr->dstats.suicides[DSTAT_LEVEL],
                   pr->dstats.suicides[DSTAT_OVERALL]);
          break;
        case DSCOL_PILOT_NAME:
          strcpy(buffer, pr->callsign);
          break;
        case DSCOL_PILOT_TEAM:
          snprintf(buffer, sizeof(buffer), "%s", basethis->GetTeamString(pr->team));
          break;
        case DSCOL_PING:
          if (pr->state == STATE_INGAME)
            snprintf(buffer, sizeof(buffer), "%.0f", basethis->NetPlayers[pr->pnum].ping_time * 1000.0f);
          else
            strcpy(buffer, "N/A");
          break;
        case DSCOL_CUSTOM:
          if (m_PLColCallback)
            (*m_PLColCallback)(m_translate[i], c, buffer, sizeof(buffer));
          break;
        case DSCOL_BLANK:
          buffer[0] = '\0';
          break;
        }
      } else {
        strcpy(buffer, "N/A");
      }

      int width_to_use = m_PLColInfo[c].width;

      if (m_PLColInfo[c].type != DSCOL_BMP) {

        if (need_to_display_observer_icon) {
          if (m_flags & DSIF_SHOW_OBSERVERICON && m_PLColInfo[c].type == DSCOL_PILOT_NAME) {
            need_to_display_observer_icon = false;
            DLLRenderHUDQuad(curr_x, curr_y, m_font_height, m_font_height, 0, 0, 1, 1, basethis->hBitmapObserver,
                             m_alpha, 0);
            curr_x += m_font_height + 1;
            width_to_use -= (m_font_height + 1);
          }
        }

        // clip the string to the width
        basethis->ClipString(std::min(width_to_use, (PLIST_WIN_X + PLIST_WIN_W - curr_x - 5)), buffer, false);

        // print out string
        ddgr_color color;
        switch (m_PLColInfo[c].color_type) {
        case DSCOLOR_NORMAL:
          color = (pr->state == STATE_INGAME) ? GR_GREEN : GR_GREY;
          break;
        case DSCOLOR_TEAM:
          color = (m_PLColInfo[c].type == DSCOL_PILOT_NAME)
                      ? (basethis->GetTeamColor(pr->team))
                      : ((pr->state == STATE_INGAME) ? (basethis->GetTeamColor(pr->team)) : (GR_GREY));
          break;
        case DSCOLOR_SHIPCOLOR:
          color = (pr->state == STATE_INGAME) ? basethis->Player_colors[pr->pnum] : GR_GREY;
          break;
        case DSCOLOR_CUSTOM:
          color = m_PLColInfo[c].color;
          break;
        default:
          color = (pr->state == STATE_INGAME) ? GR_GREEN : GR_GREY;
          break;
        };

        DLLRenderHUDText(color, m_alpha, 0, curr_x, curr_y, buffer);
      } else {

        // we need to draw a custom bitmap
        (*m_PLColCallbackBMP)(m_translate[i], c, curr_x, curr_y, m_PLColInfo[c].width, m_font_height + ROW_PIXEL_GAP,
                              m_alpha);
      }

      curr_x += width_to_use;

      if (curr_x > PLIST_WIN_W + PLIST_WIN_X)
        break; // we can't fit anymore columns
    }

    // update the bottom index
    if (update_bottom_index || i > m_bottom_index) {
      m_bottom_index = i;
      update_bottom_index = false; // we force an update the first time through
    }
  skip_line:
    curr_x = m_x_start_plist;
    curr_y += (m_font_height + ROW_PIXEL_GAP);

    if ((curr_y + m_font_height + ROW_PIXEL_GAP) > PLIST_WIN_H + PLIST_WIN_Y) {
      not_overflowing = false;
      break; // we are done here
    }
  }

  if ((m_flags & DSIF_SEPERATE_BY_TEAM) && displaying_in_game_players && not_overflowing) {
    // we can continue displaying, but just the disconnected people now
    last_team = -1; // so we redraw the team names
    displaying_in_game_players = false;

    // display the disconnected people
    goto redisplay_list;
  }

  // Display the remaining teams
  if ((m_flags & DSIF_SEPERATE_BY_TEAM) && (teams_displayed < DLLMAX_TEAMS) &&
      ((curr_y + m_font_height + ROW_PIXEL_GAP) < PLIST_WIN_H + PLIST_WIN_Y)) {
    int tcount = basethis->GetNumTeams();
    for (i = 0; i < tcount; i++) {
      if (team_disp[i])
        continue;

      team_disp[i] = true;
      last_team = i;
      buffer[0] = '\0';
      teams_displayed++;

      if (m_TeamCallback) {
        snprintf(buffer, sizeof(buffer), DTXT_TEAMLABEL, basethis->GetTeamString(i));
        int length = strlen(buffer);
        buffer[length] = ' '; // lose the \0

        (*m_TeamCallback)(i, &buffer[length], sizeof(buffer) - length - 1);
        buffer[sizeof(buffer) - 1] = '\0';

        int width = (PLIST_WIN_X + PLIST_WIN_W - curr_x - 5);
        float ratio = DEFAULT_HUD_WIDTH / ((float)*basethis->Game_window_w);

        // clip the string to the width
        basethis->ClipString(width, buffer, false);

        int l = ratio * ((float)DLLRenderHUDGetTextLineWidth(buffer));
        int x = (width / 2) - (l / 2);

        DLLRenderHUDText(basethis->GetTeamColor(i), m_alpha, 0, x, curr_y, buffer);
      }

      curr_y += (m_font_height + ROW_PIXEL_GAP);

      if ((curr_y + m_font_height + ROW_PIXEL_GAP) > PLIST_WIN_H + PLIST_WIN_Y) {
        break; // we are done here
      }
    }
  }

  // render the detailed menu
  curr_x = m_x_start_detail;
  curr_y = DLIST_WIN_Y + 1;

  DLLRenderHUDQuad(DLIST_WIN_X, DLIST_WIN_Y, DLIST_WIN_W, DLIST_WIN_H, 0, 0, 1, 1, m_background_bmp, perc_done * 200.0f,
                   0);
  pr = PRec_GetPRecord(m_selected_prec);
  if (!pr || pr->state == STATE_EMPTY || pr->pinfo == NULL)
    return;

  is_dedicated_server = basethis->IsPlayerDedicatedServer(pr);

  // display the heading for the player
  strcpy(buffer, pr->callsign);
  basethis->ClipString(DLIST_WIN_W, buffer, false);
  l_width = DLLRenderHUDGetTextLineWidth(buffer);
  DLLRenderHUDText(GR_WHITE, m_alpha, 0, DLIST_WIN_X + (DLIST_WIN_W / 2) - (l_width / 2), curr_y, buffer);
  curr_y += (m_font_height + ROW_PIXEL_GAP);

  if (m_flags & DSIF_SHOW_PIC && pr->state == STATE_INGAME && !is_dedicated_server) {

    if (basethis->PilotPicBmpHandles[pr->pnum] > BAD_BITMAP_HANDLE) {
      // display the pilot picture
      DLLRenderHUDQuad(DLIST_WIN_X + (DLIST_WIN_W / 2) - 32, curr_y, 64, 64, 0, 0, 1, 1,
                       basethis->PilotPicBmpHandles[pr->pnum], m_alpha, 0);
      curr_y += 70;
    } else if (basethis->AreLogosEnabled()) { // only show logo's if enabled
      bool animated;
      int bmp_to_disp = BAD_BITMAP_HANDLE;
      int logo_handle = basethis->GetPlayerLogoBmp(pr->pnum, &animated);
      if (logo_handle != -1) {
        if (animated) {
          // display a vclip

          vclip *vc = &basethis->GameVClips[logo_handle];
          float interval = 1.0f / 10.0f;
          int frames_elapsed = (int)(alpha_in_time / interval);

          int frame = frames_elapsed % vc->num_frames;
          bmp_to_disp = vc->frames[frame];

        } else {
          bmp_to_disp = logo_handle;
        }
        DLLRenderHUDQuad(DLIST_WIN_X + (DLIST_WIN_W / 2) - 32, curr_y, 64, 64, 0, 0, 1, 1, bmp_to_disp, m_alpha, 0);
        curr_y += 70;
      }
    }
  }

  int old_y = curr_y;
  int max_w = 0;
  int x_to_use = curr_x + 5;
  PInfo *pi;
  tPExtraInfo *ei;
  pi = (PInfo *)pr->pinfo;
  ei = pi->GetExtraInfo();

  // drawline(DLIST_WIN_X,DLIST_WIN_X+DLIST_WIN_W-1,curr_y,perc_done); curr_y += 3;

  // game status
  strcpy(buffer, DTXT_STAT_STATUS);
  // l_width = (DLLgrtext_GetTextLineWidth(buffer)*(*basethis->Hud_aspect_x));
  l_width = DLLRenderHUDGetTextLineWidth(buffer);
  max_w = l_width;
  DLLRenderHUDText(GR_WHITE, m_alpha, 0, x_to_use, curr_y, buffer);
  curr_y += (m_font_height + ROW_PIXEL_GAP);

  // ranking (if available)
  int player_rank;
  char player_rank_buffer[64];
  player_rank =
      (pr->state == STATE_INGAME && !is_dedicated_server) ? GetPlayerRankIndex(pr->pnum, player_rank_buffer) : -1;

  if (player_rank != -1) {
    strcpy(buffer, DTXT_RANK);
    l_width = DLLRenderHUDGetTextLineWidth(buffer);
    if (l_width > max_w)
      max_w = l_width;
    DLLRenderHUDText(GR_WHITE, m_alpha, 0, x_to_use, curr_y, buffer);
    curr_y += (m_font_height + ROW_PIXEL_GAP);
  }

  // team (if available)
  if (basethis->GetNumTeams() > 1 && !is_dedicated_server) {
    strcpy(buffer, DTXT_STAT_TEAM);
    l_width = DLLRenderHUDGetTextLineWidth(buffer);
    if (l_width > max_w)
      max_w = l_width;
    DLLRenderHUDText(GR_WHITE, m_alpha, 0, x_to_use, curr_y, buffer);
    curr_y += (m_font_height + ROW_PIXEL_GAP);
  }

  // time in game
  strcpy(buffer, DTXT_STAT_PLAYTIME);
  l_width = DLLRenderHUDGetTextLineWidth(buffer);
  if (l_width > max_w)
    max_w = l_width;
  DLLRenderHUDText(GR_WHITE, m_alpha, 0, x_to_use, curr_y, buffer);
  curr_y += (m_font_height + ROW_PIXEL_GAP);

  uint8_t streak_type = 0;

  if (!is_dedicated_server) {
    // none of this should be displayed for a dedicated server

    if (ei->kills_in_a_row)
      streak_type = 1;
    else if (ei->deaths_in_a_row)
      streak_type = 2;

    // kills/deaths in a row
    if (streak_type > 0) {
      strcpy(buffer, DTXT_STAT_STREAK);
      l_width = DLLRenderHUDGetTextLineWidth(buffer);
      if (l_width > max_w)
        max_w = l_width;
      DLLRenderHUDText(GR_WHITE, m_alpha, 0, x_to_use, curr_y, buffer);
      curr_y += (m_font_height + ROW_PIXEL_GAP);
    }

    // last player you killed
    if (!(m_flags & DSIF_NOLASTVICTIM)) {
      strcpy(buffer, DTXT_STAT_LASTVICTIM);
      l_width = DLLRenderHUDGetTextLineWidth(buffer);
      if (l_width > max_w)
        max_w = l_width;
      DLLRenderHUDText(GR_WHITE, m_alpha, 0, x_to_use, curr_y, buffer);
      curr_y += (m_font_height + ROW_PIXEL_GAP);
    }

    // last player to kill you
    if (!(m_flags & DSIF_NOLASTKILLER)) {
      strcpy(buffer, DTXT_STAT_LASTKILLER);
      l_width = DLLRenderHUDGetTextLineWidth(buffer);
      if (l_width > max_w)
        max_w = l_width;
      DLLRenderHUDText(GR_WHITE, m_alpha, 0, x_to_use, curr_y, buffer);
      curr_y += (m_font_height + ROW_PIXEL_GAP);
    }
  }

  curr_y = old_y;
  x_to_use += (max_w + 5);
  int width_to_clip;

  width_to_clip = DLIST_WIN_X + DLIST_WIN_W - x_to_use - 5;

  // game status
  if (pr->state == STATE_INGAME) {
    if (basethis->IsPlayerDedicatedServer(pr)) {
      strcpy(buffer, DTXT_STAT_SERVER);
    } else {
      if (basethis->IsPlayerObserver(pr->pnum)) {
        strcpy(buffer, DTXT_STAT_OBSERVER);
      } else {
        strcpy(buffer, DTXT_STAT_PLAYING);
      }
    }
  } else {
    strcpy(buffer, DTXT_STAT_DISCONNECTED);
  }
  basethis->ClipString(width_to_clip, buffer, false);
  DLLRenderHUDText(GR_WHITE, m_alpha, 0, x_to_use, curr_y, buffer);
  curr_y += (m_font_height + ROW_PIXEL_GAP);

  // player ranking (if available)
  if (player_rank != -1) {
    basethis->ClipString(width_to_clip, player_rank_buffer, false);
    DLLRenderHUDText(GR_WHITE, m_alpha, 0, x_to_use, curr_y, player_rank_buffer);
    curr_y += (m_font_height + ROW_PIXEL_GAP);
  }

  // team (if available)
  if (basethis->GetNumTeams() > 1 && !is_dedicated_server) {
    strcpy(buffer, basethis->GetTeamString(pr->team));
    basethis->ClipString(width_to_clip, buffer, false);
    DLLRenderHUDText(GR_WHITE, m_alpha, 0, x_to_use, curr_y, buffer);
    curr_y += (m_font_height + ROW_PIXEL_GAP);
  }

  // time in game
  strcpy(buffer, basethis->GetTimeString(basethis->GetTimeInGame(m_selected_prec)));
  basethis->ClipString(width_to_clip, buffer, false);
  DLLRenderHUDText(GR_WHITE, m_alpha, 0, x_to_use, curr_y, buffer);
  curr_y += (m_font_height + ROW_PIXEL_GAP);

  if (!is_dedicated_server) {
    switch (streak_type) {
    case 1: // kills in a row
      snprintf(buffer, sizeof(buffer), "%d %s", ei->kills_in_a_row,
               (ei->kills_in_a_row == 1) ? DTXT_STAT_KILL : DTXT_STAT_KILLS);
      basethis->ClipString(width_to_clip, buffer, false);
      DLLRenderHUDText(GR_WHITE, m_alpha, 0, x_to_use, curr_y, buffer);
      curr_y += (m_font_height + ROW_PIXEL_GAP);
      break;
    case 2: // deaths in a row
      snprintf(buffer, sizeof(buffer), "%d %s", ei->deaths_in_a_row,
               (ei->deaths_in_a_row == 1) ? DTXT_STAT_DEATH : DTXT_STAT_DEATHS);
      basethis->ClipString(width_to_clip, buffer, false);
      DLLRenderHUDText(GR_WHITE, m_alpha, 0, x_to_use, curr_y, buffer);
      curr_y += (m_font_height + ROW_PIXEL_GAP);
      break;
    default:
      break;
    }

    player_record *tpr;
    // last player you killed
    if (ei->last_kill_num != -1) {
      tpr = PRec_GetPRecord(ei->last_kill_num);
      if (tpr && tpr->state != STATE_EMPTY) {
        strcpy(buffer, tpr->callsign);
      } else {
        buffer[0] = '\0';
      }
    } else {
      buffer[0] = '\0';
    }
    if (!(m_flags & DSIF_NOLASTVICTIM)) {
      basethis->ClipString(width_to_clip, buffer, false);
      DLLRenderHUDText(GR_WHITE, m_alpha, 0, x_to_use, curr_y, buffer);
      curr_y += (m_font_height + ROW_PIXEL_GAP);
    }

    // last player to kill you
    if (ei->last_death_num != -1) {
      tpr = PRec_GetPRecord(ei->last_death_num);
      if (tpr && tpr->state != STATE_EMPTY) {
        strcpy(buffer, tpr->callsign);
      } else {
        buffer[0] = '\0';
      }
    } else {
      buffer[0] = '\0';
    }
    if (!(m_flags & DSIF_NOLASTKILLER)) {
      basethis->ClipString(width_to_clip, buffer, false);
      DLLRenderHUDText(GR_WHITE, m_alpha, 0, x_to_use, curr_y, buffer);
      curr_y += (m_font_height + ROW_PIXEL_GAP);
    }
  }

  // drawline(DLIST_WIN_X,DLIST_WIN_X+DLIST_WIN_W-1,curr_y,perc_done); curr_y += 10;

  curr_x = m_x_start_detail;

  // now do the detail columns (if any for this player)
  for (c = 0; c < m_iDColCount; c++) {
    strcpy(buffer, m_DColInfo[c].title);
    basethis->ClipString(m_DColInfo[c].width, buffer, false);
    DLLRenderHUDText(GR_WHITE, m_alpha, 0, curr_x, curr_y, buffer);
    curr_x += m_DColInfo[c].width;
  }

  if (m_iDColCount > 0)
    curr_y += (m_font_height + ROW_PIXEL_GAP);
  curr_x = m_x_start_detail;

  if (m_iDColCount > 0)
    drawline(DLIST_WIN_X, DLIST_WIN_X + DLIST_WIN_W - 1, curr_y, perc_done);
  curr_y += 3;

  for (c = 0; c < m_iDColCount; c++) {
    buffer[0] = '\0';
    if (!is_dedicated_server || m_DColInfo[c].type == DSCOL_PILOT_NAME || m_DColInfo[c].type == DSCOL_BLANK) {
      switch (m_DColInfo[c].type) {
      case DSCOL_BMP:
        break;
      case DSCOL_KILLS_LEVEL:
        snprintf(buffer, sizeof(buffer), "%d", pr->dstats.kills[DSTAT_LEVEL]);
        break;
      case DSCOL_KILLS_OVERALL:
        snprintf(buffer, sizeof(buffer), "%d", pr->dstats.kills[DSTAT_OVERALL]);
        break;
      case DSCOL_KILLS_BOTH:
        snprintf(buffer, sizeof(buffer), "%d[%d]", pr->dstats.kills[DSTAT_LEVEL], pr->dstats.kills[DSTAT_OVERALL]);
        break;
      case DSCOL_DEATHS_LEVEL:
        snprintf(buffer, sizeof(buffer), "%d", pr->dstats.deaths[DSTAT_LEVEL]);
        break;
      case DSCOL_DEATHS_OVERALL:
        snprintf(buffer, sizeof(buffer), "%d", pr->dstats.deaths[DSTAT_OVERALL]);
        break;
      case DSCOL_DEATHS_BOTH:
        snprintf(buffer, sizeof(buffer), "%d[%d]", pr->dstats.deaths[DSTAT_LEVEL], pr->dstats.deaths[DSTAT_OVERALL]);
        break;
      case DSCOL_SUICIDES_LEVEL:
        snprintf(buffer, sizeof(buffer), "%d", pr->dstats.suicides[DSTAT_LEVEL]);
        break;
      case DSCOL_SUICIDES_OVERALL:
        snprintf(buffer, sizeof(buffer), "%d", pr->dstats.suicides[DSTAT_OVERALL]);
        break;
      case DSCOL_SUICIDES_BOTH:
        snprintf(buffer, sizeof(buffer), "%d[%d]", pr->dstats.suicides[DSTAT_LEVEL],
                 pr->dstats.suicides[DSTAT_OVERALL]);
        break;
      case DSCOL_PILOT_NAME:
        strcpy(buffer, pr->callsign);
        break;
      case DSCOL_PILOT_TEAM:
        snprintf(buffer, sizeof(buffer), "%s", basethis->GetTeamString(pr->team));
        break;
      case DSCOL_PING:
        if (pr->state == STATE_INGAME)
          snprintf(buffer, sizeof(buffer), "%.0f", basethis->NetPlayers[pr->pnum].ping_time * 1000.0f);
        else
          strcpy(buffer, "---");
        break;
      case DSCOL_CUSTOM:
        if (m_DColCallback)
          (*m_DColCallback)(m_selected_prec, c, buffer, sizeof(buffer));
        break;
      case DSCOL_BLANK:
        buffer[0] = '\0';
        break;
      }
    } else {
      strcpy(buffer, "N/A");
    }

    if (m_DColInfo[c].type != DSCOL_BMP) {

      // clip the string to the width
      basethis->ClipString(std::min(m_DColInfo[c].width, (DLIST_WIN_X + DLIST_WIN_W - curr_x - 5)), buffer, false);

      // print out string
      ddgr_color color;
      switch (m_DColInfo[c].color_type) {
      case DSCOLOR_NORMAL:
        color = (pr->state == STATE_INGAME) ? GR_GREEN : GR_GREY;
        break;
      case DSCOLOR_TEAM:
        color = basethis->GetTeamColor(pr->team);
        break;
      case DSCOLOR_SHIPCOLOR:
        color = (pr->state == STATE_INGAME) ? basethis->Player_colors[pr->pnum] : GR_GREY;
        break;
      case DSCOLOR_CUSTOM:
        color = m_DColInfo[c].color;
        break;
      default:
        color = (pr->state == STATE_INGAME) ? GR_GREEN : GR_GREY;
        break;
      };

      DLLRenderHUDText(color, m_alpha, 0, curr_x, curr_y, buffer);

    } else {
      // we need to draw a custom bitmap
      (*m_DColCallbackBMP)(m_selected_prec, c, curr_x, curr_y, m_DColInfo[c].width, m_font_height + ROW_PIXEL_GAP,
                           m_alpha);
    }

    curr_x += m_DColInfo[c].width;

    if (curr_x > DLIST_WIN_W + DLIST_WIN_X)
      break; // we can't fit anymore columns
  }
  curr_y += (m_font_height + ROW_PIXEL_GAP);

  // finally do the who-killed-who stats for the player
  int col_callsign, col_kills, col_deaths;
  col_callsign = m_x_start_detail + 5;
  col_kills = col_callsign + 100;
  col_deaths = col_kills + 50;
  int colw_callsign, colw_kills, colw_deaths;
  colw_callsign = col_kills - col_callsign;
  colw_kills = col_deaths - col_kills;
  colw_deaths = DLIST_WIN_X + DLIST_WIN_W - col_deaths;

  if (!(m_flags & DSIF_NODETAILEDINFO)) {

    DLLRenderHUDText(GR_WHITE, m_alpha, 0, col_callsign, curr_y, DTXT_STAT_PILOTNAMELABEL);
    DLLRenderHUDText(GR_WHITE, m_alpha, 0, col_kills, curr_y, DTXT_STAT_KILLS);
    DLLRenderHUDText(GR_WHITE, m_alpha, 0, col_deaths, curr_y, DTXT_STAT_DEATHS);
    curr_y += (m_font_height + ROW_PIXEL_GAP);
    drawline(DLIST_WIN_X, DLIST_WIN_X + DLIST_WIN_W - 1, curr_y, perc_done);
    curr_y += 3;

    if (m_can_fit_count == -1) {
      // we need to recalculate the how-many-items-can-fit-on-this-page value
      int count = 0;
      int y = curr_y;

      while (y + (m_font_height + ROW_PIXEL_GAP) < DLIST_WIN_H + DLIST_WIN_Y) {
        count++;
        y += (m_font_height + ROW_PIXEL_GAP);
      }
      m_can_fit_count = count;
    }

    tPInfoStat stat;
    player_record *dpr;
    int stats_displayed = 0;
    bool clean_exit = true;
    int start_index = (m_page_index * m_can_fit_count);
    bool team_game = (basethis->GetNumTeams() > 1) ? true : false;

    if (basethis->FindPInfoStatFirst(m_selected_prec, &stat)) {
      if (stat.slot != m_selected_prec) {
        dpr = basethis->GetPlayerRecord(stat.slot);
        stats_displayed++;

        ASSERT(dpr != NULL);
        if (dpr && start_index < stats_displayed) {
          ddgr_color color;
          color = (dpr->state == STATE_INGAME)
                      ? ((team_game) ? basethis->GetTeamColor(dpr->team) : basethis->Player_colors[dpr->pnum])
                      : GR_GREY;

          strcpy(buffer, dpr->callsign);
          basethis->ClipString(colw_callsign - 5, buffer, false);
          DLLRenderHUDText(color, m_alpha, 0, col_callsign, curr_y, buffer);

          snprintf(buffer, sizeof(buffer), "%d", stat.kills);
          basethis->ClipString(colw_kills - 5, buffer, false);
          DLLRenderHUDText(color, m_alpha, 0, col_kills, curr_y, buffer);

          snprintf(buffer, sizeof(buffer), "%d", stat.deaths);
          basethis->ClipString(colw_deaths - 5, buffer, false);
          DLLRenderHUDText(color, m_alpha, 0, col_deaths, curr_y, buffer);

          curr_y += (m_font_height + ROW_PIXEL_GAP);
        }
      }

      while (basethis->FindPInfoStatNext(&stat)) {
        if (stat.slot != m_selected_prec) {
          dpr = basethis->GetPlayerRecord(stat.slot);
          stats_displayed++;

          ASSERT(dpr != NULL);
          if (dpr && start_index < stats_displayed) {
            ddgr_color color;
            color = (dpr->state == STATE_INGAME)
                        ? ((team_game) ? basethis->GetTeamColor(dpr->team) : basethis->Player_colors[dpr->pnum])
                        : GR_GREY;

            strcpy(buffer, dpr->callsign);
            basethis->ClipString(colw_callsign - 5, buffer, false);
            DLLRenderHUDText(color, m_alpha, 0, col_callsign, curr_y, buffer);

            snprintf(buffer, sizeof(buffer), "%d", stat.kills);
            basethis->ClipString(colw_kills - 5, buffer, false);
            DLLRenderHUDText(color, m_alpha, 0, col_kills, curr_y, buffer);

            snprintf(buffer, sizeof(buffer), "%d", stat.deaths);
            basethis->ClipString(colw_deaths - 5, buffer, false);
            DLLRenderHUDText(color, m_alpha, 0, col_deaths, curr_y, buffer);

            curr_y += (m_font_height + ROW_PIXEL_GAP);
          }
        }

        if (curr_y + (m_font_height + ROW_PIXEL_GAP) > DLIST_WIN_H + DLIST_WIN_Y) {

          if (basethis->FindPInfoStatNext(&stat)) {
            clean_exit = false; // there are more stats
            m_has_multiple_pages = true;
          }

          break; // can't fit anymore
        }
      }
    }
    basethis->FindPInfoStatClose();

    m_page_display_time += *basethis->Frametime;
    if ((m_page_display_time >= PAGE_DISP_TIME) && m_has_multiple_pages) {
      if (!clean_exit)
        m_page_index++;
      else
        m_page_index = 0; // if there was a clean exit, that means we were on the last page,
                          // go back to the beginning
      m_page_display_time = 0;
    }
  }

  if (m_flags & DSIF_SHOWPLAYERVIEW && m_flags & DSIF_NODETAILEDINFO && pr->state == STATE_INGAME) {
    DLLgrtext_Flush();

    float x, y, w, h;
    x = (float)DLIST_WIN_X + 2;
    y = (float)curr_y;
    h = w = (float)std::min(DLIST_WIN_W - 4, (DLIST_WIN_Y + DLIST_WIN_H - curr_y) - 4);
    object *obj_view = &basethis->Objects[basethis->Players[pr->pnum].objnum];

    x *= *basethis->Hud_aspect_x;
    y *= *basethis->Hud_aspect_y;
    w *= *basethis->Hud_aspect_x;
    h *= *basethis->Hud_aspect_y;
    x += *basethis->Game_window_x;
    y += *basethis->Game_window_y;

    if (obj_view->type == OBJ_PLAYER) {
      DisplayPlayerView((int)x, (int)y, (int)w, (int)h, obj_view);
    }
  }

  alpha_in_time += *basethis->Frametime;
  m_alpha = (perc_done * 255.0f);
}

// validates all the variables
void CDmfcStats::ValidateInfo() {
  // change index to match the last selected prec num so it doesn't move around
  int new_idx = -1;
  for (int p = 0; p < m_MaxCount; p++) {
    if (m_translate[p] == m_selected_prec) {
      // found it, update the index
      new_idx = p;
      break;
    }
  }

  if (new_idx == -1) {
    // the old prec wasn't found
    m_selected_index = 0;
  } else {
    m_selected_index = new_idx;
  }

  // make sure selected index is in view
  while (m_selected_index < m_top_index) {
    m_top_index--;
  }
  if (m_bottom_index != -1) { // make sure it has been set in a frame
    while (m_selected_index > m_bottom_index) {
      m_top_index++;
      m_bottom_index++;
    }
  }
}

// the do-it-all function, call once per frame
void CDmfcStats::DoFrame() {
  if (!m_enabled)
    return;

  int high_count;
  int count;
  int index;
  static slot_state last_state = STATE_EMPTY;
  player_record *pr;

  if (m_flags & DSIF_ONLY_X_PLAYERS_SHOWN)
    high_count = *m_pMaxNumberDisplayed;
  else
    high_count = MAX_PLAYER_RECORDS;

  // count the number of players in the game/disconnected
  // build translation table
  count = 0;
  index = 0;

  if (m_SortedPRecs[0] != 0) {
    count = 0;
    index = 0;
  }

  for (int p = 0; p < MAX_PLAYER_RECORDS; p++) {

    // increase count to see how many at most there are to display
    pr = PRec_GetPRecord(p);
    if (pr && pr->state != STATE_EMPTY && !basethis->IsPlayerDedicatedServer(pr))
      count++;

    // update translation table
    pr = PRec_GetPRecord(m_SortedPRecs[p]);
    if (pr && pr->state != STATE_EMPTY && !basethis->IsPlayerDedicatedServer(pr)) {
      m_translate[index] = m_SortedPRecs[p];
      index++;
    }
  }

  if (m_flags & DSIF_SEPERATE_BY_TEAM) {
    // Seperate by teams
    int team_index;
    int real_index = 0;
    int num_teams = basethis->GetNumTeams();
    int temp_array[MAX_PLAYER_RECORDS];
    int prec, p;

    bool getting_ingame_people = true;

  get_disconnected_people:

    for (team_index = 0; team_index < num_teams; team_index++) {
      for (p = 0; p < index; p++) {
        prec = m_translate[p];
        pr = PRec_GetPRecord(prec);
        if (pr->state != STATE_EMPTY && pr->team == team_index) {

          // we must make sure to move all disconnected people to the
          // bottom
          if ((getting_ingame_people && pr->state == STATE_INGAME) ||
              (!getting_ingame_people && pr->state == STATE_DISCONNECTED)) {
            temp_array[real_index] = prec;
            real_index++;
          }
        }
      }
      // next team
    }

    if (getting_ingame_people) {
      getting_ingame_people = false;
      goto get_disconnected_people;
    }

    memcpy(m_translate, temp_array, sizeof(int) * MAX_PLAYER_RECORDS);
  }

  m_MaxCount = std::min({high_count, count, index});

  pr = PRec_GetPRecord(m_selected_prec);
  if (!pr || pr->state != last_state) {
    m_can_fit_count = -1; // we need to recalc this
    m_page_index = 0;
    m_has_multiple_pages = false;
    if (pr)
      last_state = pr->state;
  }

  ValidateInfo();

  Render();

  // save the last selected prec num for the next frame
  m_selected_prec = m_translate[m_selected_index];
}

void CDmfcStats::DisplayPlayerView(int x, int y, int w, int h, object *viewer) {
  object *save_viewer_object = *basethis->Viewer_object;

  DLLrend_SetZBufferState(1);

  // Set up for rendering
  DLLStartFrame(x, y, x + w, y + h, true);

  // Reset facings for mine stuff
  DLLResetFacings();

  // Set the viewer for this render
  *basethis->Viewer_object = viewer;

  // Render the world
  DLLGameRenderWorld(viewer, &viewer->pos, viewer->roomnum, &viewer->orient, *basethis->Render_zoom, false);

  // Restore the viewre
  *basethis->Viewer_object = save_viewer_object;

  // Done rendering
  DLLEndFrame();
}
