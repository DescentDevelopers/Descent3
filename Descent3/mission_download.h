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

 * $Logfile: /DescentIII/Main/mission_download.h $
 * $Revision: 5 $
 * $Date: 3/26/00 10:29p $
 * $Author: Kevin $
 *
 *  Mission file downloading system
 *
 *
 * $Log: /DescentIII/Main/mission_download.h $
 *
 * 5     3/26/00 10:29p Kevin
 * MOD Downloader for 1.4 patch.
 *
 * 4     4/15/99 1:40a Jeff
 * changes for linux compile
 *
 * 3     12/30/98 12:15p Kevin
 * Auto Mission Download system
 *
 * 2     12/28/98 2:22p Kevin
 * Initial mission downloading system
 *
 * 1     12/28/98 11:42a Kevin
 *
 *
 */

#define MAX_MISSION_URL_LEN 300
#define MAX_MISSION_URL_COUNT 5

#define URL_ASK_POLL_TIME 2

extern int Got_url;

struct msn_urls {
  char msnname[_MAX_PATH];
  char URL[MAX_MISSION_URL_COUNT][MAX_MISSION_URL_LEN];
  // Possibly some quality of service flags
};

// Function prototypes
void msn_DoAskForURL(uint8_t *indata, network_address *net_addr);
void msn_DoCurrMsnURLs(uint8_t *data, network_address *net_addr);
int msn_CheckGetMission(network_address *net_addr, char *filename);
int msn_ShowDownloadChoices(msn_urls *urls);
int msn_DownloadWithStatus(char *url, char *filename);
void msn_ClipURLToWidth(int width, char *string);
char *msn_SecondsToString(int time_sec);

int ModDownloadWithStatus(char *url, char *filename);
int CheckGetD3M(char *d3m);
