/*
 * $Logfile: /DescentIII/Main/dmfc/dmfcinputcommand.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:57:20 $
 * $Author: kevinb $
 *
 * Functions for Input commands (function handlers)
 *
 * $Log: dmfcinputcommand.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:57:20  kevinb
 * initial 1.5 import
 *
 *
 * 26    2/08/00 4:33a Jason
 * Fixed memory overwrite during string parse
 *
 * 25    7/14/99 11:47a Jeff
 * localized text for patch fixes
 *
 * 24    7/09/99 6:18p Jeff
 * added $remoteadminlogout and $wait commands
 *
 * 23    7/08/99 2:39a Jeff
 * rough implementation of remote administration checked in.  Still needs
 * some polishing, but should work basically.
 *
 * 22    5/07/99 12:52p Jeff
 * audio taunt icon is ppic if available.  coop has hard max team set of 4
 *
 * 21    4/30/99 10:53p Jeff
 * added $warp command
 *
 * 20    3/17/99 12:24p Jeff
 * converted DMFC to be COM interface
 *
 * 19    2/11/99 12:50a Jeff
 * changed names of exported variables
 *
 * 18    1/31/99 7:26p Matt
 * Renamed a bunch of functions to have HUD capitalized
 *
 * 17    1/19/99 3:55a Jeff
 * all strings localized out
 *
 * 16    1/06/99 12:53a Jeff
 * put in support for $piggyback and $observer
 *
 * 15    1/04/99 12:21p Jeff
 * added support for hosts.allow/deny and updates stats manager a little
 *
 * 14    12/08/98 12:17p Jeff
 * various changes that include an improved Team Control dialog (doesn't
 * switch teams until exit..) and spew/respawn players that change teams
 *
 * 13    11/16/98 5:35p Jeff
 * removed log functions, added support for changing team names, fixed ctf
 * to work better with different team names
 *
 * 12    11/11/98 7:19p Jeff
 * changes made so that a dedicated server's team is always -1 (team game
 * or not)
 *
 * 11    11/02/98 11:41a Jeff
 * use my parse routines, and display help if the format of the command
 * was wrong
 *
 * 10    11/01/98 1:59a Jeff
 * made a $help inputcommand for help in a dedicated server environment
 *
 * 9     10/24/98 2:35p Matt
 * Changed "callsign" to "name" or "player name" in the multiplayer menus
 * and commands.
 *
 * 8     10/20/98 12:16p Jeff
 * added death message filter, hud callsign filter
 *
 * 7     10/15/98 1:34p Jeff
 * added scrollable onscreen menu.  Remove ban in dmfc.  prejoin event
 *
 * 6     10/08/98 3:37p Jeff
 * general improvements (Netgame info things, save to registry).  Changes
 * so it would send packets on NETSEQ_OBJECTS
 *
 * 5     10/05/98 2:50p Jeff
 *
 * 4     9/30/98 3:50p Jeff
 * general improvements (many)
 *
 * 3     9/23/98 4:17p Jeff
 * basic changes/improvements, started changing death messages
 *
 * 2     9/21/98 7:11p Jeff
 * made InputCommand interface API and moved existing input commands to
 * the interface.  Changed mprintf/ASSERT so they are valid in DMFC
 *
 * $NoKeywords: $
 */

#include "gamedll_header.h"
#include "DMFC.h"
#include "dmfcinternal.h"

extern char **DMFCStringTable;
extern int DMFCStringTableSize;
extern char *_DMFCErrorString;
extern DMFCBase *basethis;
char *DMFCGetString(int d);

bool StringParseWord(char *string, char *word, int size, char **newpos) {
  *newpos = string;
  word[0] = '\0';

  // get through all the whitespace
  while (*string && *string == ' ')
    string++;
  if (!(*string)) {
    // nothing is left in the string
    *newpos = string;
    return false;
  }

  // we're at the beginning of a word
  while (*string && *string != ' ') {
    if (size > 1) {
      *word = *string;
      size--;
      word++;
    }

    string++;
  }

  // tack on an ending \0
  *word = '\0';

  *newpos = string;
  return true;
}

bool StringParseNumber(char *string, int *number, char **newpos) {
  char temp[10];
  bool ret = StringParseWord(string, temp, 10, newpos);
  if (!ret) {
    *number = 0;
    return false;
  }

  *number = atoi(temp);
  return true;
}

//	handle the client wishing to change teams
//	Usage: "$team <string team_name>" where team_name is the name of the team you want to change to
void DMFCInputCommand_Team(char *input_string) {
  char team[100];

  // parse "$team"
  if (!StringParseWord(input_string, team, 100, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_TEAM);
    return;
  }

  // parse team name
  if (!StringParseWord(input_string, team, 100, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_TEAM);
    return;
  }

  int t = basethis->GetTeamFromString(team);

  int curr_team = basethis->Players[basethis->GetPlayerNum()].team;

  if ((t == -1) || (t == curr_team) || (curr_team == -1))
    return;

  mprintf((0, "Attempting to change teams to %s team\n", team));
  DLLAddHUDMessage(DTXT_TEAMCHANGEATTEMPT, team);
  basethis->RequestTeamChange(t, basethis->GetPlayerNum(), true);
}

//	handle the server wanting to change the team for a player
//	Usage: "$changeteam <int pnum> <string team_name>" where pnum is the playernum, and team_name is the name of the
// team
void DMFCInputCommand_ChangeTeam(char *input_string) {
  if (basethis->GetLocalRole() != LR_SERVER)
    return;
  char team[100];
  int p;

  // parse "$changeteam"
  if (!StringParseWord(input_string, team, 100, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_CHANGETEAM);
    return;
  }

  // parse the pnum
  if (!StringParseNumber(input_string, &p, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_CHANGETEAM);
    return;
  }

  // parse the newteam name
  if (!StringParseWord(input_string, team, 100, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_CHANGETEAM);
    return;
  }

  int t = basethis->GetTeamFromString(team);

  int curr_team = basethis->Players[p].team;

  if ((!basethis->CheckPlayerNum(p)) || (t == -1) || (t == curr_team) || (curr_team == -1))
    return;

  mprintf((0, "Attempting to change %s to %s team\n", basethis->Players[p].callsign, team));
  DLLAddHUDMessage(DTXT_STEAMCHANGEATTEMPT, basethis->Players[p].callsign, team);
  basethis->RequestTeamChange(t, p, true);
}

//	handle the client requesting playerinfo for a player
//	Usage: "$playerinfo <int pnum>" where pnum is the playernum
void DMFCInputCommand_PlayerInfo(char *input_string) {
  int p;
  char temp[20];

  // parse "$playerinfo"
  if (!StringParseWord(input_string, temp, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_PLAYERINFO);
    return;
  }

  // parse pnum
  if (!StringParseNumber(input_string, &p, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_PLAYERINFO);
    return;
  }

  SwitchPlayerInfo(p);

  if (basethis->CheckPlayerNum(p)) {
    DLLAddHUDMessage(DTXT_GETPLAYERINFO, basethis->Players[p].callsign);
  }

  if (basethis->IAmDedicatedServer()) {
    basethis->DisplayPlayerInfo(BAD_BITMAP_HANDLE, true);
  }
}

//	handle the server requesting an autobalance of teams
//	Usage: "$balance"
void DMFCInputCommand_Balance(char *input_string) {
  if (basethis->GetLocalRole() != LR_SERVER)
    return;
  BalanceTeams(0);
  DLLAddHUDMessage(DTXT_BALANCETEAMMSG);
}

//	handle turning off/on allowing the autobalancing of teams by the server
//	Usage: "$autobalance <off/on>" if off/on is "off" (or equivalent) it will turn it off, else turn it on
void DMFCInputCommand_AutoBalance(char *input_string) {
  if (basethis->GetLocalRole() != LR_SERVER)
    return;
  char s[20];

  // parse "$autobalance"
  if (!StringParseWord(input_string, s, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_AUTOBALANCE);
    return;
  }

  // parse the off/on
  if (!StringParseWord(input_string, s, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_AUTOBALANCE);
    return;
  }

  int on = 1;

  if (!strnicmp(s, DTXT_MNUOFF, strlen(DTXT_MNUOFF)))
    on = 0;
  SwitchAutoTeamSelect(on);
  DLLAddHUDMessage(DTXT_AUTOTEAMBALANCEMSG, (on) ? DTXT_MNUON : DTXT_MNUOFF);
}

//	handle turning off/on allowing clients to change their team
//	Usage: "$allowteamchange <off/on>" if off/on is "off" (or equivalent) it will turn it off, else turn it on
void DMFCInputCommand_AllowTeamChange(char *input_string) {
  if (basethis->GetLocalRole() != LR_SERVER)
    return;
  char s[20];

  // parse "$allowteamchange"
  if (!StringParseWord(input_string, s, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_ALLOWTEAMCHANGE);
    return;
  }

  // parse off/on
  if (!StringParseWord(input_string, s, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_ALLOWTEAMCHANGE);
    return;
  }

  int on = 1;
  if (!strnicmp(s, DTXT_MNUOFF, strlen(DTXT_MNUOFF)))
    on = 0;

  SwitchTeamChange(on);
  DLLAddHUDMessage(DTXT_ALLOWTEAMCHANGEMSG, (on) ? DTXT_MNUON : DTXT_MNUOFF);
}

//	handles kicking a player from the game
//	Usage: "$kick <int pnum>" where pnum is the playernumber of the player you want to kick
void DMFCInputCommand_Kick(char *input_string) {
  if (basethis->GetLocalRole() != LR_SERVER)
    return;
  int p;
  char temp[20];

  // parse "$kick"
  if (!StringParseWord(input_string, temp, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_KICK);
    return;
  }

  // parse pnum
  if (!StringParseNumber(input_string, &p, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_KICK);
    return;
  }

  if (!basethis->CheckPlayerNum(p))
    return;
  KickPlayer(p);
  if (p != *basethis->Player_num)
    DLLAddHUDMessage(DTXT_KICKMSG, basethis->Players[p].callsign);
}

//	handles banning a player from the game
//	Usage: "$ban <int pnum>" where pnum is the playernumber of the player to ban
void DMFCInputCommand_Ban(char *input_string) {
  if (basethis->GetLocalRole() != LR_SERVER)
    return;
  int p;
  char temp[20];

  // parse "$ban"
  if (!StringParseWord(input_string, temp, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_BAN);
    return;
  }

  // parse pnum
  if (!StringParseNumber(input_string, &p, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_BAN);
    return;
  }

  if (!basethis->CheckPlayerNum(p))
    return;

  BanPlayer(p);
  if (p != *basethis->Player_num)
    DLLAddHUDMessage(DTXT_BANMSG, basethis->Players[p].callsign);
}

//	handles ending the level
//	Usage: "$endlevel"
void DMFCInputCommand_EndLevel(char *input_string) {
  if (basethis->GetLocalRole() != LR_SERVER)
    return;
  EndMultiLevel(0);
  DLLAddHUDMessage(DTXT_ENDLEVELMSG);
}

//	handles the request to see all the players in the game
//	Usage: "$players"
void DMFCInputCommand_Players(char *input_string) {
  // Dedicated server print out of all the players and their player nums
  DPrintf(DTXT_PLAYERLISTHEADER);
  for (int i = 0; i < DLLMAX_PLAYERS; i++) {
    if (basethis->CheckPlayerNum(i)) {
      DPrintf("%02d: %s\n", i, basethis->Players[i].callsign);
    }
  }
}

//	handles a request to see the scores/stats of the game (up to the mod to display the scores on request)
//	Usage: "$scores"
void DMFCInputCommand_Scores(char *input_string) {
  int level;
  char temp[20];

  // parse $scores
  if (!StringParseWord(input_string, temp, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_SCORES);
    return;
  }

  // see if there is a level
  if (!StringParseNumber(input_string, &level, &input_string))
    level = -1;

  // call the event that the user want to see the scores (dedicated server
  basethis->CallOnPrintScores(level);
}

//	handles a request to turn on/off displaying callsigns on the HUD during the game.
//	Usage: "$hudnames <full/team/none>"  full = everyone   team = only teammates   none = noone
//  NOTE: You can only set your HUD Callsign level up to the level that the server is
//	So if the server is only allowing up to team, you won't be able to set to full
void DMFCInputCommand_HudCallsigns(char *input_string) {
  char s[20];

  // parse "$hudnames"
  if (!StringParseWord(input_string, s, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_HUDNAME);
    return;
  }

  // parse the level
  if (!StringParseWord(input_string, s, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_HUDNAME);
    return;
  }

  ubyte level = HUD_CALLSIGN_LEVEL_NONE;
  if (!stricmp(s, DTXT_OSM_FULL))
    level = HUD_CALLSIGN_LEVEL_FULL;
  else if (!stricmp(s, DTXT_IC_TEAM))
    level = HUD_CALLSIGN_LEVEL_TEAM;

  basethis->SwitchShowHudCallsignLevel(level);
}

//	handles a request from the server to set the max HUD Callsign level
//	Usage: "$serverhudnames <full/team/none>" same as $hudname but sets the max level for the server
void DMFCInputCommand_ServerHudCallsigns(char *input_string) {
  if (basethis->GetLocalRole() != LR_SERVER)
    return;

  char s[20];

  // parse "$serverhudnames"
  if (!StringParseWord(input_string, s, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_SERVERHUDNAMES);
    return;
  }

  // parse the level
  if (!StringParseWord(input_string, s, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_SERVERHUDNAMES);
    return;
  }

  ubyte level = HUD_CALLSIGN_LEVEL_NONE;
  if (!stricmp(s, DTXT_OSM_FULL))
    level = HUD_CALLSIGN_LEVEL_FULL;
  else if (!stricmp(s, DTXT_IC_TEAM))
    level = HUD_CALLSIGN_LEVEL_TEAM;

  basethis->SwitchServerHudCallsignLevel(level);
}

//	handles a request to save the game stats to file
//	Usage: "$savestats"
void DMFCInputCommand_SaveStats(char *input_string) { SaveStatsToFile(0); }

//	handles a request to enable/disable statistical messages
//	Usage: "$statmsgs on/off"
void DMFCInputCommand_StatMsgs(char *input_string) {
  char s[20];

  // parse "$statmsgs"
  if (!StringParseWord(input_string, s, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_STATMSGS);
    return;
  }

  // parse the level
  if (!StringParseWord(input_string, s, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_STATMSGS);
    return;
  }

  bool on = true;
  if (!strnicmp(s, DTXT_MNUOFF, strlen(DTXT_MNUOFF)))
    on = false;
  basethis->EnableStatisticalMessages(on);
  DLLAddHUDMessage(DTXT_STATSMSGSET, (on) ? DTXT_MNUON : DTXT_MNUOFF);
}

//	handles a request to enable/disable the autosaving of the game stats on level end
//	Usage: "$autosavelevel on/off"
void DMFCInputCommand_AutoSaveLevel(char *input_string) {
  char s[20];

  // parse "$autosavelevel"
  if (!StringParseWord(input_string, s, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_AUTOSAVELEVEL);
    return;
  }

  // parse the on/off
  if (!StringParseWord(input_string, s, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_AUTOSAVELEVEL);
    return;
  }

  bool on = true;
  if (!strnicmp(s, DTXT_MNUOFF, strlen(DTXT_MNUOFF)))
    on = false;
  basethis->EnableAutoSaveLevelEnd(on);
  DLLAddHUDMessage(DTXT_AUTOSAVELEVELMSG, (on) ? DTXT_MNUON : DTXT_MNUOFF);
}

//	handles a request to enable/disable the autosaving of the game stats if we disconnect
//	Usage: "$autosavedisconnect on/off"
void DMFCInputCommand_AutoSaveDisconnect(char *input_string) {
  char s[20];

  // parse the "$autosavedisconnect"
  if (!StringParseWord(input_string, s, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_AUTOSAVEDISC);
    return;
  }

  if (!StringParseWord(input_string, s, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_AUTOSAVEDISC);
    return;
  }

  bool on = true;
  if (!strnicmp(s, DTXT_MNUOFF, strlen(DTXT_MNUOFF)))
    on = false;
  basethis->EnableAutoSaveDisconnect(on);
  DLLAddHUDMessage(DTXT_AUTOSAVEDISCMSG, (on) ? DTXT_MNUON : DTXT_MNUOFF);
}

//	handles changing the level time limit of a netgame on the fly
//	Usage: "$settimelimit <int minutes>" where minutes is the number of minutes
void DMFCInputCommand_SetTimeLimit(char *input_string) {
  if (basethis->GetLocalRole() != LR_SERVER)
    return;
  int d;
  char temp[20];

  // parse "$settimelimit"
  if (!StringParseWord(input_string, temp, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_SETTIMELIMIT);
    return;
  }

  // parse the amount
  if (!StringParseNumber(input_string, &d, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_SETTIMELIMIT);
    return;
  }

  if (d < 0)
    d = 0;

  basethis->Netgame->timelimit = d;

  if (d == 0) {
    basethis->Netgame->flags &= ~NF_TIMER;
  } else {
    basethis->Netgame->flags |= NF_TIMER;
  }

  if (basethis->Netgame->flags & NF_TIMER)
    DLLAddHUDMessage(DTXT_TIMELIMITSETMSG, d);
  else
    DLLAddHUDMessage(DTXT_TIMELIMITOFFMSG);

  basethis->SendNetGameInfoSync();
}

//	handles changing the level goal limit of a netgame on the fly
//	Usage: "$setgoallimit <int goal>" where goal is the number of points
void DMFCInputCommand_SetGoalLimit(char *input_string) {
  if (basethis->GetLocalRole() != LR_SERVER)
    return;
  int d;
  char temp[20];

  // parse "$setgoallimit"
  if (!StringParseWord(input_string, temp, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_SETGOALLIMIT);
    return;
  }

  if (!StringParseNumber(input_string, &d, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_SETGOALLIMIT);
    return;
  }

  if (d < 0)
    d = 0;
  basethis->Netgame->killgoal = d;

  if (d == 0) {
    basethis->Netgame->flags &= ~NF_KILLGOAL;
  } else {
    basethis->Netgame->flags |= NF_KILLGOAL;
  }

  if (basethis->Netgame->flags & NF_KILLGOAL)
    DLLAddHUDMessage(DTXT_GOALLIMITSETMSG, d);
  else
    DLLAddHUDMessage(DTXT_GOALLIMITOFFMSG);

  basethis->SendNetGameInfoSync();
}

//	handles changing the number of max players in a netgame
//	Usage: "$setmaxplayers <int max>" where max is the maximum players
void DMFCInputCommand_SetMaxPlayers(char *input_string) {
  if (basethis->GetLocalRole() != LR_SERVER)
    return;
  int d;
  char temp[20];

  // parse "$setmaxplayers"
  if (!StringParseWord(input_string, temp, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_SETMAXPLAYERS);
    return;
  }

  // parse the number
  if (!StringParseNumber(input_string, &d, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_SETMAXPLAYERS);
    return;
  }

  // get the number of players in the game currently
  int curr_count = 0;
  for (int i = 0; i < DLLMAX_PLAYERS; i++) {
    if (basethis->CheckPlayerNum(i))
      curr_count++;
  }
  if (d < curr_count)
    d = curr_count;
  if (d > basethis->Hard_max_players)
    d = basethis->Hard_max_players;

  basethis->Netgame->max_players = d;

  DLLAddHUDMessage(DTXT_MAXPLAYERSSETMSG, d);

  basethis->SendNetGameInfoSync();
}

//	handles changing the respawn time of the powerups in a level of a netgame
//	Usage: "$setrespawntime <int seconds>" where seconds is the time in seconds
void DMFCInputCommand_SetRespawnTime(char *input_string) {
  if (basethis->GetLocalRole() != LR_SERVER)
    return;
  int d;
  char temp[20];

  // parse "$setrespawntime"
  if (!StringParseWord(input_string, temp, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_SETRESPAWNTIME);
    return;
  }

  // parse the value
  if (!StringParseNumber(input_string, &d, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_SETRESPAWNTIME);
    return;
  }

  if (d < 1)
    d = 1;

  basethis->Netgame->respawn_time = d;

  DLLAddHUDMessage(DTXT_RESPAWNTIMEMSG, d);

  basethis->SendNetGameInfoSync();
}

//	handle the client requesting netgame info
//	Usage: "$netgameinfo"
void DMFCInputCommand_NetGameInfo(char *input_string) {
  bool display = !basethis->IsDisplayingNetGameInfo();
  basethis->SwitchNetGameInfoDisplay((display) ? 1 : 0);

  // always display this on this input command for the dedicated server
  basethis->DisplayNetGameInfo(-1, true);
}

//	handle the server changing the pps threshold of the game
//	Usage: "$setpps <int pps>" where pps is the Packets Per Second
void DMFCInputCommand_SetPPS(char *input_string) {
  if (basethis->GetLocalRole() != LR_SERVER)
    return;
  int d;
  char temp[20];

  //"$setpps"
  if (!StringParseWord(input_string, temp, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_SETPPS);
    return;
  }

  // parse value
  if (!StringParseNumber(input_string, &d, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_SETPPS);
    return;
  }

  if (d < 1)
    d = 1;
  if (d > 20)
    d = 20;
  basethis->Netgame->packets_per_second = d;

  DLLAddHUDMessage(DTXT_PPSSETMSG, d);

  basethis->SendNetGameInfoSync();
}

//	handles listing the ban list on a dedicated server (so if you want to remove a player from it)
//	Usage: "$banlist"
void DMFCInputCommand_BanList(char *input_string) {
  if (basethis->GetLocalRole() != LR_SERVER)
    return;

  int size = basethis->GetNumBannedPlayers();
  char buffer[100];

  for (int i = 0; i < size; i++) {
    char *ptr = basethis->GetBannedPlayerCallsign(i);
    if (ptr) {
      sprintf(buffer, "[%d]%s\n", i, ptr);
      DPrintf(buffer);
    }
  }
}

//	handles removing a ban from a player, given the ban index num (see $banlist)
//	Usage: "$removeban <int bannum>" where bannum is the number corresponding to that given during a $banlist
void DMFCInputCommand_RemoveBan(char *input_string) {
  if (basethis->GetLocalRole() != LR_SERVER)
    return;

  int d;
  char temp[20];

  //"$removeban"
  if (!StringParseWord(input_string, temp, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_REMOVEBAN);
    return;
  }

  // value
  if (!StringParseNumber(input_string, &d, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_REMOVEBAN);
    return;
  }

  if (basethis->RemoveBan(d)) {
    DLLAddHUDMessage(DTXT_BANREMOVEDMSG);
    DPrintf(DTXT_BANREMOVEDMSG);
  } else {
    DLLAddHUDMessage(DTXT_BANNOTREMOVEDMSG);
    DPrintf(DTXT_BANNOTREMOVEDMSG);
  }
}

//	handles changing the filter for death messages
//	Usage: "$killmsgfilter <full/simple/none>"
void DMFCInputCommand_KillMsgFilter(char *input_string) {
  char s[20];

  //"$killmsgfilter"
  if (!StringParseWord(input_string, s, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_KILLMSGFILTER);
    return;
  }

  // parse level
  if (!StringParseWord(input_string, s, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_KILLMSGFILTER);
    return;
  }

  ubyte level = DM_FILTER_FULL;
  if (!stricmp(s, DTXT_PLAIN_NONE))
    level = DM_FILTER_NONE;
  else if (!stricmp(s, DTXT_OSM_SIMPLE))
    level = DM_FILTER_SIMPLE;

  basethis->SetDeathMessageFilter(level);
}

//	handles the server changing a team's name
//	Usage: "$setteamname <team_num> <new_team_name>
void DMFCInputCommand_SetTeamName(char *input_string) {
  char s[20];
  int team;

  // parse "$setteamname"
  if (!StringParseWord(input_string, s, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_SETTEAMNAME);
    return;
  }

  // parse team_num
  if (!StringParseNumber(input_string, &team, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_SETTEAMNAME);
    return;
  }

  if (team < 0 || team > DLLMAX_TEAMS) {
    DLLAddHUDMessage(DTXT_INVALIDTEAM, DLLMAX_TEAMS - 1);
    basethis->DisplayInputCommandHelp(DTXT_IC_SETTEAMNAME);
    return;
  }

  // parse team_name
  if (!StringParseWord(input_string, s, MIN(MAX_TEAMNAME_LEN, 20), &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_SETTEAMNAME);
    return;
  }

  basethis->SetTeamName(team, s, true);
}

//	handles a request for help to see a list of commands, dedicated server only
//	Usage: "$help [command]"
void DMFCInputCommand_Help(char *input_string) {
  char s[20];

  // parse "$help"
  if (!StringParseWord(input_string, s, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_HELP);
    return;
  }

  // see if there is another command
  if (!StringParseWord(input_string, s, 20, &input_string)) {
    s[0] = '\0';
  }

  basethis->DisplayInputCommandHelp(s);
}

//	handles a request to rehash the hosts.allow/deny
//	Usage: "$rehash"
void DMFCInputCommand_Rehash(char *input_string) {
  if (basethis->GetLocalRole() != LR_SERVER)
    return;

  RehashAllowDenyLists(0);
}

//	handles a request to enter/leave observer mode
//	Usage: "$observer <on/off>"
void DMFCInputCommand_Observer(char *input_string) {
  if (basethis->IAmDedicatedServer()) {
    DPrintf(DTXT_NOTINDEDICATED);
    return;
  }

  char s[20];
  bool turn_on = true;
  object *pobj;
  int pnum = basethis->GetPlayerNum();
  pobj = &basethis->Objects[basethis->Players[pnum].objnum];

  // parse "$observer"
  StringParseWord(input_string, s, 20, &input_string);

  // see if there is another command
  if (StringParseWord(input_string, s, 20, &input_string)) {
    if (stricmp(s, DTXT_MNUON))
      turn_on = false;
  } else {
    if (pobj->type == OBJ_OBSERVER) {
      turn_on = false;
    } else {
      turn_on = true;
    }
  }

  if (!basethis->CallOnAllowObserverChange(turn_on)) {
    DLLAddHUDMessage(DTXT_OBSERVERDENIED);
    return;
  }

  mprintf((0, "SwitchObserverMode %s [%d]\n", (turn_on) ? "On" : "Off", pnum));

  DLLMultiSendRequestToObserve(OBSERVER_MODE_ROAM, (turn_on) ? 1 : 0, 0);
}

//	handles a request to piggyback a player
//	Usage: "$piggyback <pnum>"
void DMFCInputCommand_Piggyback(char *input_string) {
  if (basethis->IAmDedicatedServer()) {
    DPrintf(DTXT_NOTINDEDICATED);
    return;
  }

  char s[20];
  bool turn_on = true;
  object *pobj;
  int pnum = basethis->GetPlayerNum();
  pobj = &basethis->Objects[basethis->Players[pnum].objnum];
  int topnum = -1;

  // parse "$piggyback"
  StringParseWord(input_string, s, 20, &input_string);

  if (!StringParseWord(input_string, s, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_PIGGYBACK);
    return;
  }

  char *ptr = s;
  while (*ptr) {
    if (!isdigit(*ptr)) {
      basethis->DisplayInputCommandHelp(DTXT_IC_PIGGYBACK);
      return;
    }
    ptr++;
  }

  topnum = atoi(s);
  if (!basethis->CheckPlayerNum(topnum)) {
    DLLAddHUDMessage(DTXT_INVALIDPNUM);
    return;
  }

  if (pobj->type == OBJ_OBSERVER) {
    if (basethis->CallOnAllowObserverChange(false))
      DLLMultiSendRequestToObserve(OBSERVER_MODE_ROAM, 0, 0);
  }
  if (pnum == basethis->GetPlayerNum()) {
    mprintf((0, "Returning to self\n"));
    return;
  }

  if (basethis->CallOnAllowObserverChange(true)) {
    mprintf((0, "Switching to piggyback for player %d\n", topnum));
    DLLMultiSendRequestToObserve(OBSERVER_MODE_PIGGYBACK, 1, basethis->Players[topnum].objnum);
  }
}

//	handles a request to warp to another level
//	Usage: "$warp <level number>"
void DMFCInputCommand_Warp(char *input_string) {
  if (basethis->GetLocalRole() != LR_SERVER)
    return;

  char s[20];
  int level;

  // parse "$warp"
  if (!StringParseWord(input_string, s, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_WARP);
    return;
  }

  // parse levelnum
  if (!StringParseNumber(input_string, &level, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_WARP);
    return;
  }

  basethis->WarpToLevel(level);
}

//	handles enable/disable remote administration
//	Usage: "$remoteadmin <on/off>"
extern bool Use_remote_admin;
void DMFCInputCommand_RemoteAdmin(char *input_string) {
  if (basethis->GetLocalRole() != LR_SERVER)
    return;
  char s[20];

  // parse "$remoteadmin"
  if (!StringParseWord(input_string, s, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_REMOTEADMIN);
    return;
  }

  // parse the off/on
  if (!StringParseWord(input_string, s, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_REMOTEADMIN);
    return;
  }

  int on = 1;

  if (!strnicmp(s, DTXT_MNUOFF, strlen(DTXT_MNUOFF)))
    on = 0;

  Remote_Enable((bool)(on != 0));

  if (on && !Use_remote_admin) {
    // no password set
    DLLAddHUDMessage(DTXT_UNABLETOREMOTE);
  } else {
    DLLAddHUDMessage(DTXT_REMOTEADMINENABLE, (Use_remote_admin) ? DTXT_MNUON : DTXT_MNUOFF);
  }
}

//	handles setting/changing the remote administration password
//	Usage: "$remoteadminpass <password>"
void DMFCInputCommand_RemoteAdminPass(char *input_string) {
  if (basethis->GetLocalRole() != LR_SERVER)
    return;
  char s[64];

  // parse "$remoteadminpass"
  if (!StringParseWord(input_string, s, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_REMOTEADMINPASS);
    return;
  }

  // parse the password
  if (!StringParseWord(input_string, s, 64, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_REMOTEADMINPASS);
    return;
  }

  Remote_SetPassword(s);
  DLLAddHUDMessage(DTXT_REMOTEADMINPASS);
}

//	handles seeing who is logged in, and allows the server to log them out
//	If no parameter is given it lists all the players logged in.
//	To log out a player give the login-id as a parameter
//	Usage: "$remoteadminlogout [login-id]"
void DMFCInputCommand_RemoteAdminLogout(char *input_string) {
  if (basethis->GetLocalRole() != LR_SERVER)
    return;
  char s[64];

  // parse "$remoteadminlogout"
  if (!StringParseWord(input_string, s, 64, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_REMOTEADMINLOGOUT);
    return;
  }

  // see if there is another command
  if (!StringParseWord(input_string, s, 64, &input_string)) {
    // list who is logged in
    Remote_ListLogins();
    return;
  }

  // logout
  int prec = atoi(s);
  Remote_Logout(prec);
}

//	handles a remote admin command
//	Usage: "$remote <command> <option parms> <...>"
void DMFCInputCommand_Remote(char *input_string) {
  if (basethis->GetLocalRole() == LR_SERVER)
    return;
  char s[20];

  // parse "$remote"
  if (!StringParseWord(input_string, s, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_REMOTE);
    return;
  }

  Remote_ClientProcess(input_string + 1 /*for space*/);
}

//	handles a request to make all clients wait/or stop waiting
//	Usage: "$wait <on/off>"
void DMFCInputCommand_Wait(char *input_string) {
  if (basethis->GetLocalRole() != LR_SERVER)
    return;
  if (!basethis->IAmDedicatedServer())
    return;

  char s[20];

  // parse "$wait"
  if (!StringParseWord(input_string, s, 20, &input_string)) {
    return;
  }

  // parse the off/on
  if (!StringParseWord(input_string, s, 20, &input_string)) {
    basethis->DisplayInputCommandHelp(DTXT_IC_WAIT);
    return;
  }

  bool on = true;

  if (!stricmp(s, DTXT_MNUOFF))
    on = false;
  else if (stricmp(s, DTXT_MNUON)) {
    // it's not ON of OFF
    float wait_time = atof(s);
    if (wait_time > 0) {
      DPrintf(DTXT_WAITSECS, wait_time);
      basethis->PauseRealGameTime(true);
      basethis->MakeClientsWait(true);
      basethis->SetDedicatedWait(wait_time);
    } else if (wait_time == 0) {
      DPrintf(DTXT_WAITOFF);
      basethis->PauseRealGameTime(false);
      basethis->MakeClientsWait(false);
      basethis->SetDedicatedWait(0);
    }
    return;
  }

  // turn on client waiting
  if (on) {
    DPrintf(DTXT_WAITON);
  } else {
    DPrintf(DTXT_WAITNOLONGER);
  }

  basethis->PauseRealGameTime(on);
  basethis->MakeClientsWait(on);
}
