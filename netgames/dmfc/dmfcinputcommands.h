/*
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
void DMFCInputCommand_Team(char *input_string);
//	handle the server wanting to change the team for a player
//	Usage: "$changeteam <int pnum> <string team_name>" where pnum is the playernum, and team_name is the name of the
//team
void DMFCInputCommand_ChangeTeam(char *input_string);
//	handle the client requesting playerinfo for a player
//	Usage: "$playerinfo <int pnum>" where pnum is the playernum
void DMFCInputCommand_PlayerInfo(char *input_string);
//	handle the server requesting an autobalance of teams
//	Usage: "$balance"
void DMFCInputCommand_Balance(char *input_string);
//	handle turning off/on allowing the autobalancing of teams by the server
//	Usage: "$autobalance <off/on>" if off/on is "off" (or equivalent) it will turn it off, else turn it on
void DMFCInputCommand_AutoBalance(char *input_string);
//	handle turning off/on allowing clients to change their team
//	Usage: "$allowteamchange <off/on>" if off/on is "off" (or equivalent) it will turn it off, else turn it on
void DMFCInputCommand_AllowTeamChange(char *input_string);
//	handles kicking a player from the game
//	Usage: "$kick <int pnum>" where pnum is the playernumber of the player you want to kick
void DMFCInputCommand_Kick(char *input_string);
//	handles banning a player from the game
//	Usage: "$ban <int pnum>" where pnum is the playernumber of the player to ban
void DMFCInputCommand_Ban(char *input_string);
//	handles ending the level
//	Usage: "$endlevel"
void DMFCInputCommand_EndLevel(char *input_string);
//	handles the request to see all the players in the game
//	Usage: "$players"
void DMFCInputCommand_Players(char *input_string);
//	handles a request to see the scores/stats of the game (up to the mod to display the scores on request)
//	Usage: "$scores"
void DMFCInputCommand_Scores(char *input_string);
//	handles a request to save the game stats to file
//	Usage: "$savestats"
void DMFCInputCommand_SaveStats(char *input_string);
//	handles a request to turn on/off displaying callsigns on the HUD during the game.
//	Usage: "$hudcallsigns <full/team/none>"  full = everyone   team = only teammates   none = noone
//  NOTE: You can only set your HUD Callsign level up to the level that the server is
//	So if the server is only allowing up to team, you won't be able to set to full
void DMFCInputCommand_HudCallsigns(char *input_string);
//	handles a request from the server to set the max HUD Callsign level
//	Usage: "$serverhudcallsigns <full/team/none>" same as $hudcallsign but sets the max level for the server
void DMFCInputCommand_ServerHudCallsigns(char *input_string);
//	handles a request to enable/disable statistical messages
//	Usage: "$statmsgs on/off"
void DMFCInputCommand_StatMsgs(char *input_string);
//	handles a request to enable/disable the autosaving of the game stats on level end
//	Usage: "$autosavelevel on/off"
void DMFCInputCommand_AutoSaveLevel(char *input_string);
//	handles a request to enable/disable the autosaving of the game stats if we disconnect
//	Usage: "$autosavedisconnect on/off"
void DMFCInputCommand_AutoSaveDisconnect(char *input_string);
//	handles changing the level time limit of a netgame on the fly
//	Usage: "$settimelimit <int minutes>" where minutes is the number of minutes
void DMFCInputCommand_SetTimeLimit(char *input_string);
//	handles changing the level goal limit of a netgame on the fly
//	Usage: "$setgoallimit <int goal>" where goal is the number of points
void DMFCInputCommand_SetGoalLimit(char *input_string);
//	handles changing the number of max players in a netgame
//	Usage: "$setmaxplayers <int max>" where max is the maximum players
void DMFCInputCommand_SetMaxPlayers(char *input_string);
//	handles changing the respawn time of the powerups in a level of a netgame
//	Usage: "$setrespawntime <int seconds>" where seconds is the time in seconds
void DMFCInputCommand_SetRespawnTime(char *input_string);
//	handle the client requesting netgame info
//	Usage: "$netgameinfo"
void DMFCInputCommand_NetGameInfo(char *input_string);
//	handle the server changing the pps threshold of the game
//	Usage: "$setpps <int pps>" where pps is the Packets Per Second
void DMFCInputCommand_SetPPS(char *input_string);
//	handles listing the ban list on a dedicated server (so if you want to remove a player from it)
//	Usage: "$banlist"
void DMFCInputCommand_BanList(char *input_string);
//	handles removing a ban from a player, given the ban index num (see $banlist)
//	Usage: "$removeban <int bannum>" where bannum is the number corresponding to that given during a $banlist
void DMFCInputCommand_RemoveBan(char *input_string);
//	handles changing the filter for death messages
//	Usage: "$killmsgfilter <full/simple/none>"
void DMFCInputCommand_KillMsgFilter(char *input_string);
//	handles the server changing a team's name
//	Usage: "$setteamname <team_num> <new_team_name>
void DMFCInputCommand_SetTeamName(char *input_string);
//	handles a request for help to see a list of commands, dedicated server only
//	Usage: "$help [command]"
void DMFCInputCommand_Help(char *input_string);
//	handles a request to rehash the hosts.allow/deny
//	Usage: "$rehash"
void DMFCInputCommand_Rehash(char *input_string);
//	handles a request to enter/leave observer mode
//	Usage: "$observer <on/off>"
void DMFCInputCommand_Observer(char *input_string);
//	handles a request to piggyback a player
//	Usage: "$piggyback <pnum>"
void DMFCInputCommand_Piggyback(char *input_string);
//	handles a request to warp to another level
//	Usage: "$warp <level number>"
void DMFCInputCommand_Warp(char *input_string);
//	handles enable/disable remote administration
//	Usage: "$remoteadmin <on/off>"
void DMFCInputCommand_RemoteAdmin(char *input_string);
//	handles setting/changing the remote administration password
//	Usage: "$remoteadminpass <password>"
void DMFCInputCommand_RemoteAdminPass(char *input_string);
//	handles a remote admin command
//	Usage: "$remote <command> <option parms> <...>"
void DMFCInputCommand_Remote(char *input_string);
//	handles a request to make all clients wait/or stop waiting
//	Usage: "$wait <on/off>"
void DMFCInputCommand_Wait(char *input_string);

//	handles seeing who is logged in, and allows the server to log them out
//	If no parameter is given it lists all the players logged in.
//	To log out a player give the login-id as a parameter
//	Usage: "$remoteadminlogout [login-id]"
void DMFCInputCommand_RemoteAdminLogout(char *input_string);

#endif