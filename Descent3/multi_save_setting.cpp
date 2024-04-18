/*
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
#include "CFILE.H"
#include "multi.h"
#include "objinfo.h"
#include "ship.h"
#include "multi_save_settings.h"

int MultiSaveSettings(char *filename) {
  CFILE *cf;
  char szoutput[MAX_MPS_LINE_LEN];
  int i;
  cf = cfopen(filename, "wt");
  if (!cf)
    return 0;
  sprintf(szoutput, "NAME\t%s", Netgame.name);
  cf_WriteString(cf, szoutput);
  sprintf(szoutput, "MISSION\t%s", Netgame.mission);
  cf_WriteString(cf, szoutput);
  sprintf(szoutput, "SCRIPT\t%s", Netgame.scriptname);
  cf_WriteString(cf, szoutput);
  sprintf(szoutput, "PPS\t%d", Netgame.packets_per_second);
  cf_WriteString(cf, szoutput);
  sprintf(szoutput, "PEERPEER\t%s", (Netgame.flags & NF_PEER_PEER) ? "TRUE" : "FALSE");
  cf_WriteString(cf, szoutput);
  sprintf(szoutput, "PERMISSABLE\t%s", (Netgame.flags & NF_PERMISSABLE) ? "TRUE" : "FALSE");
  cf_WriteString(cf, szoutput);
  sprintf(szoutput, "RANDOMIZERESPAWN\t%s", (Netgame.flags & NF_RANDOMIZE_RESPAWN) ? "TRUE" : "FALSE");
  cf_WriteString(cf, szoutput);
  sprintf(szoutput, "ROTVEL\t%s", (Netgame.flags & NF_SENDROTVEL) ? "TRUE" : "FALSE");
  cf_WriteString(cf, szoutput);
  sprintf(szoutput, "USESMOOTHING\t%s", (Netgame.flags & NF_USE_SMOOTHING) ? "TRUE" : "FALSE");
  cf_WriteString(cf, szoutput);
  sprintf(szoutput, "BRIGHTPLAYERS\t%s", (Netgame.flags & NF_BRIGHT_PLAYERS) ? "TRUE" : "FALSE");
  cf_WriteString(cf, szoutput);
  sprintf(szoutput, "MAXPLAYERS\t%d", Netgame.max_players);
  cf_WriteString(cf, szoutput);
  sprintf(szoutput, "RESPAWNTIME\t%d", Netgame.respawn_time);
  cf_WriteString(cf, szoutput);
  sprintf(szoutput, "KILLGOAL\t%d", Netgame.killgoal);
  cf_WriteString(cf, szoutput);
  sprintf(szoutput, "TIMELIMIT\t%d", Netgame.timelimit);
  cf_WriteString(cf, szoutput);
  sprintf(szoutput, "ACCWEAP\t%s", (Netgame.flags & NF_USE_ACC_WEAP) ? "TRUE" : "FALSE");
  cf_WriteString(cf, szoutput);
  sprintf(szoutput, "MLOOK\t%s", (Netgame.flags & NF_ALLOW_MLOOK) ? "TRUE" : "FALSE");
  cf_WriteString(cf, szoutput);
  sprintf(szoutput, "DIFFICULTY\t%d", Netgame.difficulty);
  cf_WriteString(cf, szoutput);

  for (i = 0; i < MAX_OBJECT_IDS; i++) {
    if (Object_info[i].type == OBJ_POWERUP) {
      if (!Object_info[i].multi_allowed) {
        sprintf(szoutput, "OBJBAN\t%s", Object_info[i].name);
        cf_WriteString(cf, szoutput);
      }
    }
  }
  for (i = 0; i < MAX_SHIPS; i++) {
    if (Ships[i].used) {
      if (!PlayerIsShipAllowed(0, i)) {
        sprintf(szoutput, "SHIPBAN\t%s", Ships[i].name);
        cf_WriteString(cf, szoutput);
      }
    }
  }
  cfclose(cf);
  return 1;
}

int MultiLoadSettings(char *filename) {
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
    if (strcmpi(toklabel, "NAME") == 0) {
      strcpy(Netgame.name, tokval);
      // Do this so the name can still have spaces
      tokval = strtok(NULL, seps);
      while (tokval) {
        strcat(Netgame.name, " ");
        strcat(Netgame.name, tokval);
        tokval = strtok(NULL, seps);
      }
    } else if (strcmpi(toklabel, "MISSION") == 0) {
      strcpy(Netgame.mission, tokval);
      // Do this so the mission can still have spaces
      tokval = strtok(NULL, seps);
      while (tokval) {
        strcat(Netgame.mission, " ");
        strcat(Netgame.mission, tokval);
        tokval = strtok(NULL, seps);
      }
    } else if (strcmpi(toklabel, "SCRIPT") == 0) {
      strcpy(Netgame.scriptname, tokval);
      // Do this so the script can still have spaces
      tokval = strtok(NULL, seps);
      while (tokval) {
        strcat(Netgame.scriptname, " ");
        strcat(Netgame.scriptname, tokval);
        tokval = strtok(NULL, seps);
      }
    } else if (strcmpi(toklabel, "PPS") == 0) {
      Netgame.packets_per_second = atoi(tokval);
    } else if (strcmpi(toklabel, "PEERPEER") == 0) {
      if (strcmpi(tokval, "true") == 0)
        Netgame.flags |= NF_PEER_PEER;
      else
        Netgame.flags &= ~NF_PEER_PEER;
    } else if (strcmpi(toklabel, "PERMISSABLE") == 0) {
      if (strcmpi(tokval, "true") == 0)
        Netgame.flags |= NF_PERMISSABLE;
      else
        Netgame.flags &= ~NF_PERMISSABLE;
    } else if (strcmpi(toklabel, "RANDOMIZERESPAWN") == 0) {
      if (strcmpi(tokval, "true") == 0)
        Netgame.flags |= NF_RANDOMIZE_RESPAWN;
      else
        Netgame.flags &= ~NF_RANDOMIZE_RESPAWN;
    } else if (strcmpi(toklabel, "ACCWEAP") == 0) {
      if (strcmpi(tokval, "true") == 0)
        Netgame.flags |= NF_USE_ACC_WEAP;
      else
        Netgame.flags &= ~NF_USE_ACC_WEAP;
    } else if (strcmpi(toklabel, "ROTVEL") == 0) {
      if (strcmpi(tokval, "true") == 0)
        Netgame.flags |= NF_SENDROTVEL;
      else
        Netgame.flags &= ~NF_SENDROTVEL;
    } else if (strcmpi(toklabel, "USESMOOTHING") == 0) {
      if (strcmpi(tokval, "true") == 0)
        Netgame.flags |= NF_USE_SMOOTHING;
      else
        Netgame.flags &= ~NF_USE_SMOOTHING;
    } else if (strcmpi(toklabel, "BRIGHTPLAYERS") == 0) {
      if (strcmpi(tokval, "true") == 0)
        Netgame.flags |= NF_BRIGHT_PLAYERS;
      else
        Netgame.flags &= ~NF_BRIGHT_PLAYERS;
    } else if (strcmpi(toklabel, "MAXPLAYERS") == 0) {
      Netgame.max_players = atoi(tokval);
    } else if (strcmpi(toklabel, "RESPAWNTIME") == 0) {
      Netgame.respawn_time = atoi(tokval);
    } else if (strcmpi(toklabel, "KILLGOAL") == 0) {
      Netgame.killgoal = atoi(tokval);
      if (Netgame.killgoal)
        Netgame.flags |= NF_KILLGOAL;
      else
        Netgame.flags &= ~NF_KILLGOAL;

    } else if (strcmpi(toklabel, "TIMELIMIT") == 0) {
      Netgame.timelimit = atoi(tokval);
      if (Netgame.timelimit)
        Netgame.flags |= NF_TIMER;
      else
        Netgame.flags &= ~NF_TIMER;
    } else if (strcmpi(toklabel, "OBJBAN") == 0) {
      objid = FindObjectIDName(tokval);
      if (objid != -1) {
        Object_info[objid].multi_allowed = 0;
      }
    } else if (strcmpi(toklabel, "SHIPBAN") == 0) {
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
    } else if (strcmpi(toklabel, "MLOOK") == 0) {
      if (strcmpi(tokval, "true") == 0)
        Netgame.flags |= NF_ALLOW_MLOOK;
      else
        Netgame.flags &= ~NF_ALLOW_MLOOK;
    } else if (strcmpi(toklabel, "DIFFICULTY") == 0) {
      Netgame.difficulty = atoi(tokval);
      if ((Netgame.difficulty > 4) || (Netgame.difficulty < 0))
        Netgame.difficulty = 0;
    } else {
      mprintf((0, "Unknown line in multiplayer config file %s\t%s\n", toklabel, tokval));
    }
  };
  return 1;
}
