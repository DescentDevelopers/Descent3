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

 * $Logfile: /DescentIII/main/multi_save_setting.cpp $
 * $Revision: 12 $
 * $Date: 5/05/00 9:04a $
 * $Author: Matt $
 *
 * Multiplayer settings save/load
 *
 * $Log: /DescentIII/main/multi_save_setting.cpp $
 *
 * 12    5/05/00 9:04a Matt
 * Allow ship names for the SHIPBAN command to have spaces, so the black
 * pyro will work.
 *
 * 11    5/09/99 1:34p Kevin
 * Added diffuculty level system to multiplayer
 *
 * 10    4/14/99 2:51a Jeff
 * fixed some case mismatched #includes
 *
 * 9     3/25/99 3:29p Jason
 * added option to randomize powerup respawn points
 *
 * 8     2/05/99 1:17p Jason
 * added some options
 *
 * 7     12/07/98 12:54p Kevin
 * Added Accurate weapon collisions flag
 *
 * 6     12/01/98 12:47p Jason
 * got rid of NF_DROPMISORDERED and added NF_USE_SMOOTHING
 *
 * 5     10/17/98 2:32p Kevin
 * FIxed problem with banned users getting stuck on the ban message
 * screen.
 *
 * 4     10/02/98 10:11a Kevin
 * dealt with spaces in the name of values
 *
 * 3     9/23/98 6:33p Kevin
 * Fixed load settings
 *
 * 2     9/23/98 2:54p Kevin
 * Saved multi config and changed UI to conform
 *
 * 1     9/23/98 10:15a Kevin
 * Initial Version
 *
 * $NoKeywords: $
 */

#include <stdio.h>
#include "cfile.h"
#include "multi.h"
#include "objinfo.h"
#include "ship.h"
#include "multi_save_settings.h"

int MultiSaveSettings(const char *filename) {
  CFILE *cf;
  char szoutput[MAX_MPS_LINE_LEN];
  int i;
  cf = cfopen(filename, "wt");
  if (!cf)
    return 0;
  snprintf(szoutput, sizeof(szoutput), "NAME\t%s", Netgame.name);
  cf_WriteString(cf, szoutput);
  snprintf(szoutput, sizeof(szoutput), "MISSION\t%s", Netgame.mission);
  cf_WriteString(cf, szoutput);
  snprintf(szoutput, sizeof(szoutput), "SCRIPT\t%s", Netgame.scriptname);
  cf_WriteString(cf, szoutput);
  snprintf(szoutput, sizeof(szoutput), "PPS\t%d", Netgame.packets_per_second);
  cf_WriteString(cf, szoutput);
  snprintf(szoutput, sizeof(szoutput), "PEERPEER\t%s", (Netgame.flags & NF_PEER_PEER) ? "TRUE" : "FALSE");
  cf_WriteString(cf, szoutput);
  snprintf(szoutput, sizeof(szoutput), "PERMISSABLE\t%s", (Netgame.flags & NF_PERMISSABLE) ? "TRUE" : "FALSE");
  cf_WriteString(cf, szoutput);
  snprintf(szoutput, sizeof(szoutput), "RANDOMIZERESPAWN\t%s",
           (Netgame.flags & NF_RANDOMIZE_RESPAWN) ? "TRUE" : "FALSE");
  cf_WriteString(cf, szoutput);
  snprintf(szoutput, sizeof(szoutput), "ROTVEL\t%s", (Netgame.flags & NF_SENDROTVEL) ? "TRUE" : "FALSE");
  cf_WriteString(cf, szoutput);
  snprintf(szoutput, sizeof(szoutput), "USESMOOTHING\t%s", (Netgame.flags & NF_USE_SMOOTHING) ? "TRUE" : "FALSE");
  cf_WriteString(cf, szoutput);
  snprintf(szoutput, sizeof(szoutput), "BRIGHTPLAYERS\t%s", (Netgame.flags & NF_BRIGHT_PLAYERS) ? "TRUE" : "FALSE");
  cf_WriteString(cf, szoutput);
  snprintf(szoutput, sizeof(szoutput), "MAXPLAYERS\t%d", Netgame.max_players);
  cf_WriteString(cf, szoutput);
  snprintf(szoutput, sizeof(szoutput), "RESPAWNTIME\t%d", Netgame.respawn_time);
  cf_WriteString(cf, szoutput);
  snprintf(szoutput, sizeof(szoutput), "KILLGOAL\t%d", Netgame.killgoal);
  cf_WriteString(cf, szoutput);
  snprintf(szoutput, sizeof(szoutput), "TIMELIMIT\t%d", Netgame.timelimit);
  cf_WriteString(cf, szoutput);
  snprintf(szoutput, sizeof(szoutput), "ACCWEAP\t%s", (Netgame.flags & NF_USE_ACC_WEAP) ? "TRUE" : "FALSE");
  cf_WriteString(cf, szoutput);
  snprintf(szoutput, sizeof(szoutput), "MLOOK\t%s", (Netgame.flags & NF_ALLOW_MLOOK) ? "TRUE" : "FALSE");
  cf_WriteString(cf, szoutput);
  snprintf(szoutput, sizeof(szoutput), "DIFFICULTY\t%d", Netgame.difficulty);
  cf_WriteString(cf, szoutput);

  for (i = 0; i < MAX_OBJECT_IDS; i++) {
    if (Object_info[i].type == OBJ_POWERUP) {
      if (!Object_info[i].multi_allowed) {
        snprintf(szoutput, sizeof(szoutput), "OBJBAN\t%s", Object_info[i].name);
        cf_WriteString(cf, szoutput);
      }
    }
  }
  for (i = 0; i < MAX_SHIPS; i++) {
    if (Ships[i].used) {
      if (!PlayerIsShipAllowed(0, i)) {
        snprintf(szoutput, sizeof(szoutput), "SHIPBAN\t%s", Ships[i].name);
        cf_WriteString(cf, szoutput);
      }
    }
  }
  cfclose(cf);
  return 1;
}

int MultiLoadSettings(const char *filename) {
  CFILE *cf;
  char szinput[MAX_MPS_LINE_LEN];
  char *toklabel, *tokval;
  char seps[] = "\t ";
  int objid;
  cf = cfopen(filename, "rt");
  if (!cf)
    return 0;

  while (cf_ReadString(szinput, MAX_MPS_LINE_LEN - 1, cf)) {

    toklabel = strtok(szinput, seps);
    tokval = strtok(NULL, seps);
    if (!tokval) {
      continue;
    }
    if (stricmp(toklabel, "NAME") == 0) {
      strcpy(Netgame.name, tokval);
      // Do this so the name can still have spaces
      tokval = strtok(NULL, seps);
      while (tokval) {
        strcat(Netgame.name, " ");
        strcat(Netgame.name, tokval);
        tokval = strtok(NULL, seps);
      }
    } else if (stricmp(toklabel, "MISSION") == 0) {
      strcpy(Netgame.mission, tokval);
      // Do this so the mission can still have spaces
      tokval = strtok(NULL, seps);
      while (tokval) {
        strcat(Netgame.mission, " ");
        strcat(Netgame.mission, tokval);
        tokval = strtok(NULL, seps);
      }
    } else if (stricmp(toklabel, "SCRIPT") == 0) {
      strcpy(Netgame.scriptname, tokval);
      // Do this so the script can still have spaces
      tokval = strtok(NULL, seps);
      while (tokval) {
        strcat(Netgame.scriptname, " ");
        strcat(Netgame.scriptname, tokval);
        tokval = strtok(NULL, seps);
      }
    } else if (stricmp(toklabel, "PPS") == 0) {
      Netgame.packets_per_second = atoi(tokval);
    } else if (stricmp(toklabel, "PEERPEER") == 0) {
      if (stricmp(tokval, "true") == 0)
        Netgame.flags |= NF_PEER_PEER;
      else
        Netgame.flags &= ~NF_PEER_PEER;
    } else if (stricmp(toklabel, "PERMISSABLE") == 0) {
      if (stricmp(tokval, "true") == 0)
        Netgame.flags |= NF_PERMISSABLE;
      else
        Netgame.flags &= ~NF_PERMISSABLE;
    } else if (stricmp(toklabel, "RANDOMIZERESPAWN") == 0) {
      if (stricmp(tokval, "true") == 0)
        Netgame.flags |= NF_RANDOMIZE_RESPAWN;
      else
        Netgame.flags &= ~NF_RANDOMIZE_RESPAWN;
    } else if (stricmp(toklabel, "ACCWEAP") == 0) {
      if (stricmp(tokval, "true") == 0)
        Netgame.flags |= NF_USE_ACC_WEAP;
      else
        Netgame.flags &= ~NF_USE_ACC_WEAP;
    } else if (stricmp(toklabel, "ROTVEL") == 0) {
      if (stricmp(tokval, "true") == 0)
        Netgame.flags |= NF_SENDROTVEL;
      else
        Netgame.flags &= ~NF_SENDROTVEL;
    } else if (stricmp(toklabel, "USESMOOTHING") == 0) {
      if (stricmp(tokval, "true") == 0)
        Netgame.flags |= NF_USE_SMOOTHING;
      else
        Netgame.flags &= ~NF_USE_SMOOTHING;
    } else if (stricmp(toklabel, "BRIGHTPLAYERS") == 0) {
      if (stricmp(tokval, "true") == 0)
        Netgame.flags |= NF_BRIGHT_PLAYERS;
      else
        Netgame.flags &= ~NF_BRIGHT_PLAYERS;
    } else if (stricmp(toklabel, "MAXPLAYERS") == 0) {
      Netgame.max_players = atoi(tokval);
    } else if (stricmp(toklabel, "RESPAWNTIME") == 0) {
      Netgame.respawn_time = atoi(tokval);
    } else if (stricmp(toklabel, "KILLGOAL") == 0) {
      Netgame.killgoal = atoi(tokval);
      if (Netgame.killgoal)
        Netgame.flags |= NF_KILLGOAL;
      else
        Netgame.flags &= ~NF_KILLGOAL;

    } else if (stricmp(toklabel, "TIMELIMIT") == 0) {
      Netgame.timelimit = atoi(tokval);
      if (Netgame.timelimit)
        Netgame.flags |= NF_TIMER;
      else
        Netgame.flags &= ~NF_TIMER;
    } else if (stricmp(toklabel, "OBJBAN") == 0) {
      objid = FindObjectIDName(tokval);
      if (objid != -1) {
        Object_info[objid].multi_allowed = 0;
      }
    } else if (stricmp(toklabel, "SHIPBAN") == 0) {
      // Do this so the name can have spaces
      char buf[100];
      strcpy(buf, tokval);
      tokval = strtok(NULL, seps);
      while (tokval) {
        strcat(buf, " ");
        strcat(buf, tokval);
        tokval = strtok(NULL, seps);
      }
      PlayerSetShipPermission(-1, buf, 0);
    } else if (stricmp(toklabel, "MLOOK") == 0) {
      if (stricmp(tokval, "true") == 0)
        Netgame.flags |= NF_ALLOW_MLOOK;
      else
        Netgame.flags &= ~NF_ALLOW_MLOOK;
    } else if (stricmp(toklabel, "DIFFICULTY") == 0) {
      Netgame.difficulty = atoi(tokval);
      if ((Netgame.difficulty > 4) || (Netgame.difficulty < 0))
        Netgame.difficulty = 0;
    } else {
      mprintf(0, "Unknown line in multiplayer config file %s\t%s\n", toklabel, tokval);
    }
  };
  return 1;
}
