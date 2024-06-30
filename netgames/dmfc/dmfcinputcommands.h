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

 * $Logfile: /DescentIII/Main/Dmfc/dmfcinputcommands.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:57:20 $
 * $Author: kevinb $
 *
 * header for input command function handlers
 *
 * $Log: dmfcinputcommands.h,v $
 * Revision 1.1.1.1  2003/08/26 03:57:20  kevinb
 * initial 1.5 import
 *
 *
 * 16    7/09/99 6:18p Jeff
 * added $remoteadminlogout and $wait commands
 *
 * 15    7/08/99 2:39a Jeff
 * rough implementation of remote administration checked in.  Still needs
 * some polishing, but should work basically.
 *
 * 14    4/30/99 10:53p Jeff
 * added $warp command
 *
 * 13    3/17/99 12:24p Jeff
 * converted DMFC to be COM interface
 *
 * 12    1/06/99 12:53a Jeff
 * put in support for $piggyback and $observer
 *
 * 11    1/04/99 12:21p Jeff
 * added support for hosts.allow/deny and updates stats manager a little
 *
 * 10    11/16/98 5:35p Jeff
 * removed log functions, added support for changing team names, fixed ctf
 * to work better with different team names
 *
 * 9     11/01/98 1:59a Jeff
 * made a $help inputcommand for help in a dedicated server environment
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

#ifndef __DMFC_INPUTCOMMANDS_H_
#define __DMFC_INPUTCOMMANDS_H_

//	handle the client wishing to change teams
//	Usage: "$team <string team_name>" where team_name is the name of the team you want to change to
void DMFCInputCommand_Team(const char *input_string);
//	handle the server wanting to change the team for a player
//	Usage: "$changeteam <int pnum> <string team_name>" where pnum is the playernum, and team_name is the name of the
// team
void DMFCInputCommand_ChangeTeam(const char *input_string);
//	handle the client requesting playerinfo for a player
//	Usage: "$playerinfo <int pnum>" where pnum is the playernum
void DMFCInputCommand_PlayerInfo(const char *input_string);
//	handle the server requesting an autobalance of teams
//	Usage: "$balance"
void DMFCInputCommand_Balance(const char *input_string);
//	handle turning off/on allowing the autobalancing of teams by the server
//	Usage: "$autobalance <off/on>" if off/on is "off" (or equivalent) it will turn it off, else turn it on
void DMFCInputCommand_AutoBalance(const char *input_string);
//	handle turning off/on allowing clients to change their team
//	Usage: "$allowteamchange <off/on>" if off/on is "off" (or equivalent) it will turn it off, else turn it on
void DMFCInputCommand_AllowTeamChange(const char *input_string);
//	handles kicking a player from the game
//	Usage: "$kick <int pnum>" where pnum is the playernumber of the player you want to kick
void DMFCInputCommand_Kick(const char *input_string);
//	handles banning a player from the game
//	Usage: "$ban <int pnum>" where pnum is the playernumber of the player to ban
void DMFCInputCommand_Ban(const char *input_string);
//	handles ending the level
//	Usage: "$endlevel"
void DMFCInputCommand_EndLevel(const char *input_string);
//	handles the request to see all the players in the game
//	Usage: "$players"
void DMFCInputCommand_Players(const char *input_string);
//	handles a request to see the scores/stats of the game (up to the mod to display the scores on request)
//	Usage: "$scores"
void DMFCInputCommand_Scores(const char *input_string);
//	handles a request to save the game stats to file
//	Usage: "$savestats"
void DMFCInputCommand_SaveStats(const char *input_string);
//	handles a request to turn on/off displaying callsigns on the HUD during the game.
//	Usage: "$hudcallsigns <full/team/none>"  full = everyone   team = only teammates   none = noone
//  NOTE: You can only set your HUD Callsign level up to the level that the server is
//	So if the server is only allowing up to team, you won't be able to set to full
void DMFCInputCommand_HudCallsigns(const char *input_string);
//	handles a request from the server to set the max HUD Callsign level
//	Usage: "$serverhudcallsigns <full/team/none>" same as $hudcallsign but sets the max level for the server
void DMFCInputCommand_ServerHudCallsigns(const char *input_string);
//	handles a request to enable/disable statistical messages
//	Usage: "$statmsgs on/off"
void DMFCInputCommand_StatMsgs(const char *input_string);
//	handles a request to enable/disable the autosaving of the game stats on level end
//	Usage: "$autosavelevel on/off"
void DMFCInputCommand_AutoSaveLevel(const char *input_string);
//	handles a request to enable/disable the autosaving of the game stats if we disconnect
//	Usage: "$autosavedisconnect on/off"
void DMFCInputCommand_AutoSaveDisconnect(const char *input_string);
//	handles changing the level time limit of a netgame on the fly
//	Usage: "$settimelimit <int minutes>" where minutes is the number of minutes
void DMFCInputCommand_SetTimeLimit(const char *input_string);
//	handles changing the level goal limit of a netgame on the fly
//	Usage: "$setgoallimit <int goal>" where goal is the number of points
void DMFCInputCommand_SetGoalLimit(const char *input_string);
//	handles changing the number of max players in a netgame
//	Usage: "$setmaxplayers <int max>" where max is the maximum players
void DMFCInputCommand_SetMaxPlayers(const char *input_string);
//	handles changing the respawn time of the powerups in a level of a netgame
//	Usage: "$setrespawntime <int seconds>" where seconds is the time in seconds
void DMFCInputCommand_SetRespawnTime(const char *input_string);
//	handle the client requesting netgame info
//	Usage: "$netgameinfo"
void DMFCInputCommand_NetGameInfo(const char *input_string);
//	handle the server changing the pps threshold of the game
//	Usage: "$setpps <int pps>" where pps is the Packets Per Second
void DMFCInputCommand_SetPPS(const char *input_string);
//	handles listing the ban list on a dedicated server (so if you want to remove a player from it)
//	Usage: "$banlist"
void DMFCInputCommand_BanList(const char *input_string);
//	handles removing a ban from a player, given the ban index num (see $banlist)
//	Usage: "$removeban <int bannum>" where bannum is the number corresponding to that given during a $banlist
void DMFCInputCommand_RemoveBan(const char *input_string);
//	handles changing the filter for death messages
//	Usage: "$killmsgfilter <full/simple/none>"
void DMFCInputCommand_KillMsgFilter(const char *input_string);
//	handles the server changing a team's name
//	Usage: "$setteamname <team_num> <new_team_name>
void DMFCInputCommand_SetTeamName(const char *input_string);
//	handles a request for help to see a list of commands, dedicated server only
//	Usage: "$help [command]"
void DMFCInputCommand_Help(const char *input_string);
//	handles a request to rehash the hosts.allow/deny
//	Usage: "$rehash"
void DMFCInputCommand_Rehash(const char *input_string);
//	handles a request to enter/leave observer mode
//	Usage: "$observer <on/off>"
void DMFCInputCommand_Observer(const char *input_string);
//	handles a request to piggyback a player
//	Usage: "$piggyback <pnum>"
void DMFCInputCommand_Piggyback(const char *input_string);
//	handles a request to warp to another level
//	Usage: "$warp <level number>"
void DMFCInputCommand_Warp(const char *input_string);
//	handles enable/disable remote administration
//	Usage: "$remoteadmin <on/off>"
void DMFCInputCommand_RemoteAdmin(const char *input_string);
//	handles setting/changing the remote administration password
//	Usage: "$remoteadminpass <password>"
void DMFCInputCommand_RemoteAdminPass(const char *input_string);
//	handles a remote admin command
//	Usage: "$remote <command> <option parms> <...>"
void DMFCInputCommand_Remote(const char *input_string);
//	handles a request to make all clients wait/or stop waiting
//	Usage: "$wait <on/off>"
void DMFCInputCommand_Wait(const char *input_string);

//	handles seeing who is logged in, and allows the server to log them out
//	If no parameter is given it lists all the players logged in.
//	To log out a player give the login-id as a parameter
//	Usage: "$remoteadminlogout [login-id]"
void DMFCInputCommand_RemoteAdminLogout(const char *input_string);

#endif
