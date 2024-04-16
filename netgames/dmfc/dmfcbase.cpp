/*
* $Logfile: /DescentIII/Main/dmfc/dmfcbase.cpp $
* $Revision: 1.1.1.1 $
* $Date: 2003/08/26 03:57:20 $
* $Author: kevinb $
*
* Base functions for DMFC implementation
*
* $Log: dmfcbase.cpp,v $
* Revision 1.1.1.1  2003/08/26 03:57:20  kevinb
* initial 1.5 import
*
 * 
 * 170   9/28/01 2:09p Matt
 * Don't show ping/loss info on HUD if peer-to-peer game.
 * 
 * 169   9/07/01 8:57a Matt
 * Fixed hud name bug from previous change.
 * 
 * 168   9/05/01 6:04p Matt
 * Added code to save the user's preferred HUD name level setting even if
 * the server bashes it down.
 * 
 * 167   10/26/99 10:31a Jeff
 * fixed COM interface bug
 * 
 * 166   10/21/99 9:27p Jeff
 * B.A. Macintosh code merge
 * 
 * 165   8/21/99 12:33a Jeff
 * Changed the name of the GetRealGametime function to
 * GetRealGametimePacket since it was confusing some compilers with the
 * other GetRealGametime function.
 * 
 * 164   8/15/99 4:36p Jeff
 * finished exporting all inventory class functions.  export object_info
 * array. added check for -nooutragelogo to display Outrage logo display.
 * 
 * 163   8/11/99 6:36p Jeff
 * don't set input command list root to NULL when creating input commands
 * (it would create a memleak and ignore any ICs added by a custom mod)
 * 
 * 162   8/11/99 1:21p Jeff
 * exported needed functions for camera windows
 * 
 * 161   7/30/99 11:59a Jeff
 * fixed bug with autobalance being turned off
 * 
 * 160   7/14/99 11:47a Jeff
 * localized text for patch fixes
 * 
 * 159   7/13/99 10:05a Jeff
 * text taunt token decoding
 * 
 * 158   7/11/99 3:31p Jeff
 * exported game arguments, made command line option to specify
 * autoexec.dmfc
 * 
 * 157   7/09/99 7:02p Jeff
 * put in countdown timer for when a level is about to end
 * 
 * 156   7/09/99 6:17p Jeff
 * added $remoteadminlogout and $wait commands
 * 
 * 155   7/09/99 2:53p Jeff
 * handle gametime better (pause it when needed) if the server is 'waiting
 * for players'
 * 
 * 154   7/08/99 6:25p Jeff
 * remote admin in and working
 * 
 * 153   7/08/99 2:39a Jeff
 * rough implementation of remote administration checked in.  Still needs
 * some polishing, but should work basically.
 * 
 * 152   7/07/99 5:00p Jeff
 * removed vararg functions from interface functions, just made different
 * versions of them
 * 
 * 151   6/11/99 5:36p Jeff
 * removed ai_info #ifdefs (better way of doing it)
 * 
 * 150   6/10/99 12:34p Jeff
 * GetCounterMeasureOwner doesn't use ->ai_info for non-Outrage versions
 * 
 * 149   6/10/99 11:10a Jeff
 * don't display the Outrage logo for non-Outrage games
 * 
 * 148   5/22/99 1:12a Jeff
 * correctly handle Viewer_object
 * 
 * 147   5/20/99 5:32p Jeff
 * called PlayerStopSounds if respawning a player
 * 
 * 146   5/20/99 3:51p Jeff
 * for apply damage to player, server says should be 1 (so it works in
 * peer-peer)
 * 
 * 145   5/13/99 4:55p Ardussi
 * changes for compiling on the Mac
 * 
 * 144   5/12/99 11:04p Jeff
 * dmfc and multiplayer games now have endian friendly packets (*whew*)
 * 
 * 143   5/10/99 2:43a Jeff
 * handle new scheme of player's joining in a team game, where the team is
 * set before player enters game in the main code, but the team is
 * determined via event call to dmfc
 * 
 * 142   5/09/99 6:20a Jeff
 * improved Entropy (added sounds, max virii per room).  Fixed rendering
 * bugs for other multiplayer dlls.
 * 
 * 141   5/08/99 11:06a Jeff
 * 
 * 140   5/07/99 4:34p Jason
 * fixed audio taunt icon
 * 
 * 139   5/07/99 12:52p Jeff
 * audio taunt icon is ppic if available.  coop has hard max team set of 4
 * 
 * 138   5/04/99 8:46p Jeff
 * display icon when someone plays an audio taunt
 * 
 * 137   5/03/99 8:39a Jeff
 * fixed apply damage to player
 * 
 * 136   4/30/99 10:52p Jeff
 * added $warp command
 * 
 * 135   4/30/99 7:36p Jeff
 * exported vis_effects to dmfc
 * 
 * 134   4/27/99 1:56p Jeff
 * audio taunts stuff in pilot menu, added stringtables
 * 
 * 133   4/26/99 3:41p Jeff
 * put in debug multiplayer command dump to file
 * 
 * 132   4/25/99 7:18p Jeff
 * added code to handle suicides in pinfo
 * 
 * 131   4/23/99 6:15p Jeff
 * fixed double calls to GameClose
 * 
 * 130   4/23/99 4:49p Jason
 * played with loss colors a bit
 * 
 * 129   4/23/99 4:24p Jason
 * fixed ping display bug
 * 
 * 128   4/23/99 12:46p Jeff
 * lag loss indicator on by default
 * 
 * 127   4/22/99 11:28a Jeff
 * changed percent sign as a result of Samir's change
 * 
 * 126   4/21/99 10:35p Jeff
 * fixed lag/loss displays
 * 
 * 125   4/20/99 8:57p Jeff
 * compile for Linux
 * 
 * 124   4/14/99 3:03p Jeff
 * 
 * 123   4/12/99 11:39a Jeff
 * removed movement smoothing, added permissable cs to netgame info
 * 
 * 122   4/04/99 4:55p Jeff
 * added functionality to call osiris functions from multiplayer d3ms
 * 
 * 121   4/03/99 4:06p Jeff
 * added loss/ping gauge
 * 
 * 120   4/02/99 9:02p Jeff
 * fixed crashes if there was an error initializing module
 * 
 * 119   3/30/99 9:01p Jeff
 * exported polymodels
 * 
 * 118   3/30/99 7:42p Jeff
 * fixed a misspelling on a function name
 * 
 * 117   3/22/99 6:20p Jeff
 * added 2 more audio taunts.  a mulitplayer event when someone plays an
 * audio taunt.  option to disable audio taunts.
 * 
 * 116   3/22/99 1:55p Jeff
 * make sure initialization happens (possible crashing)
 * 
 * 115   3/17/99 12:23p Jeff
 * converted DMFC to be COM interface
 * 
 * 114   3/09/99 1:13p Jeff
 * fixed control packet for wait being overflowed out and accidently sent
 * by the client
 * 
 * 113   3/05/99 1:29p Jeff
 * fixed 99% of the high res issues
 * 
 * 112   3/01/99 8:48p Jeff
 * fixed banning bug (when not a master tracker game)
 * 
 * 111   2/25/99 8:54p Jeff
 * Inventory supports level change persistant items.  Inventory supports
 * time-out objects.  Inventory Reset changed (takes a level of reset
 * now).  Quad lasers stay across level change (single player).  Guidebot
 * bug fixed (now back in ship on level start).  Quads time out when
 * spewed.  Invulnerability and cloak powerups no longer use game
 * event/callbacks, so they can be saved in game saves (moved to
 * MakePlayerInvulnerable and MakeObjectInvisible)
 * 
 * 110   2/11/99 12:50a Jeff
 * changed names of exported variables
 * 
 * 109   2/10/99 1:47p Matt
 * Changed object handle symbolic constants
 * 
 * 108   2/09/99 3:32p Jeff
 * table file parser takes quotes strings for force keywords
 * 
 * 107   2/07/99 2:06a Jeff
 * updated coop...fixed bug when getting countermeasure owner, if owner is
 * observer
 * 
 * 106   2/07/99 1:19a Jeff
 * added new multiplayer game events EVT_GAMEOBJKILLED and
 * EVT_GAMEOBJDESTROYED
 * 
 * 105   2/05/99 8:24p Jeff
 * added table file parser macros
 * 
 * 104   2/03/99 4:09p Jeff
 * moved function pointers to seperate file. created autoexec.dmfc
 * 
 * 103   2/02/99 8:43a Chris
 * I made buildings with AI work correctly (ie really big robots should be
 * buildings)
 * anim to and from states are now shorts instead of bytes
 * 
 * 102   1/31/99 7:26p Matt
 * Renamed a bunch of functions to have HUD capitalized
 * 
 * 101   1/21/99 11:16p Jeff
 * exported vecmat functions
 * 
 * 100   1/20/99 8:06p Jeff
 * added members into DLLinfo struct for game change segment events, pass
 * them over on execute dll packets
 * 
 * 99    1/19/99 5:34p Jeff
 * updated monsterball
 * 
 * 98    1/18/99 7:27p Jeff
 * localized strings in dmfcbase.cpp
 * 
 * 97    1/17/99 11:52p Jeff
 * added some new events, and changed a couple event handlers
 * 
 * 96    1/15/99 8:29p Jeff
 * updates to powerball
 * 
 * 95    1/15/99 7:52p Chris
 * Updated ObjSetPos() to include a f_update_attach_children flag
 * 
 * 94    1/15/99 3:53a Jeff
 * exported a couple more functions.  Added event handlers for Weapon
 * collide event
 * 
 * 93    1/12/99 2:55p Jeff
 * added/finished the waiting for player's to join dialog
 * 
 * 92    1/12/99 11:29a Jason
 * fixed a broken externed function
 * 
 * 91    1/07/99 5:01p Jeff
 * added Int3's and updated all net games to use stats manager...correctly
 * too
 * 
 * 90    1/06/99 7:02p Jeff
 * added a multiplayer event for game controls
 * 
 * 89    1/06/99 12:53a Jeff
 * put in support for $piggyback and $observer
 * 
 * 88    1/04/99 8:11p Jason
 * fixed packet loss tracking problem
 * 
 * 87    1/04/99 2:19p Jeff
 * exported table file management functions
 * 
 * 86    1/04/99 12:21p Jeff
 * added support for hosts.allow/deny and updates stats manager a little
 * 
 * 85    12/13/98 5:32p Jeff
 * fixed ugly crash due to freeing memory allocated in another heap
 * 
 * 84    12/09/98 12:38p Jeff
 * removed possible security bug displaying a player's ip address (now
 * server only can see it)
 * 
 * 83    12/08/98 4:47p Jeff
 * umm, various changes, fixed pilot pics so that they work for everyone
 * now
 * 
 * 82    12/08/98 3:29p Jeff
 * updated the team control dialog so the server can determine if they
 * want to make the clients wait
 * 
 * 81    12/08/98 12:17p Jeff
 * various changes that include an improved Team Control dialog (doesn't
 * switch teams until exit..) and spew/respawn players that change teams
 * 
 * 80    12/04/98 7:04p Jeff
 * almost finished up dmfc stat manager
 * 
 * 79    12/01/98 6:56p Jeff
 * put in quick and dirty implementation of pilot pics for testing
 * 
 * 78    11/19/98 5:56p Jeff
 * added slider exported and improved Hoard
 * 
 * 77    11/17/98 6:29p Jeff
 * mod can specify whether or not to display the team setup dialog on team
 * game start.  Added a menu item to display team setup dialog in mid-game
 * 
 * 76    11/17/98 12:36p Jeff
 * fixed dedicated server detection and display a dprintf for setting new
 * team
 * 
 * 75    11/16/98 5:35p Jeff
 * removed log functions, added support for changing team names, fixed ctf
 * to work better with different team names
 * 
 * 74    11/13/98 6:36p Jeff
 * created dmfc_dll (a DLL version of DMFC) and converted current mods to
 * use it
 * 
 * 73    11/12/98 12:16p Jeff
 * more changes to handle (ignore) dedicated server
 * 
 * 72    11/11/98 7:19p Jeff
 * changes made so that a dedicated server's team is always -1 (team game
 * or not)
 * 
 * 71    11/02/98 4:38p Jeff
 * added ability to sort and display by efficiency
 * 
 * 70    11/01/98 1:59a Jeff
 * made a $help inputcommand for help in a dedicated server environment
 * 
 * 69    10/30/98 12:47p Jeff
 * cut down a couple bytes on memory usage
 * 
 * 68    10/29/98 7:01p Jeff
 * creation of team placement dialog.  Moved TranslateEvent into DMFC
 * 
 * 67    10/24/98 2:35p Matt
 * Changed "callsign" to "name" or "player name" in the multiplayer menus
 * and commands.
 * 
 * 66    10/24/98 2:18p Jeff
 * 
 * 65    10/23/98 6:51p Jeff
 * fixed hud num of teams sort in ctf, and memory overwrite in registry
 * 
 * 64    10/23/98 11:22a Jeff
 * changes to handle mixcase, and display the client netgame info
 * correctly
 * 
 * 63    10/21/98 5:02p Jeff
 * removed player from HUD if in observer nide
 * 
 * 62    10/20/98 4:35p Jeff
 * added a flag for menu to add a <None> to MIT_PLIST...
 * 
 * 61    10/20/98 12:16p Jeff
 * added death message filter, hud callsign filter
 * 
 * 60    10/19/98 7:19p Matt
 * Added system to support different types of damage to the player and
 * have these different types make different sounds.
 * 
 * 59    10/18/98 7:59p Jeff
 * functions added to dmfc for client->server objnum matching.  Banning
 * now uses tracker id when available.
 * 
 * 58    10/17/98 7:30p Jeff
 * network_address compares don't compare port on somethings
 * 
 * 57    10/15/98 6:18p Jeff
 * created the is player banned event, removed prejoin event
 * 
 * 56    10/15/98 1:34p Jeff
 * added scrollable onscreen menu.  Remove ban in dmfc.  prejoin event
 * 
 * 55    10/13/98 6:01p Jeff
 * added attaching
 * 
 * 54    10/13/98 2:15a Jeff
 * created new event for when a player leaves a multiplayer game.  Fixed
 * some 'english' bugs in the network games.
 * 
 * 53    10/08/98 3:37p Jeff
 * general improvements (Netgame info things, save to registry).  Changes
 * so it would send packets on NETSEQ_OBJECTS
 * 
 * 51    10/05/98 2:49p Jeff
 * 
 * 50    10/02/98 6:10p Jeff
 * 
 * 49    10/01/98 11:30a Jeff
 * made the observer mode events into just a client event
 * 
 * 48    9/30/98 4:21p Jeff
 * team changing is handled correctly
 * 
 * 47    9/30/98 3:50p Jeff
 * general improvements (many)
 * 
 * 46    9/29/98 3:04p Jeff
 * added time in game and start_time support
 * 
 * 45    9/28/98 5:05p Jeff
 * made the statisitical death messages an option in the menu
 * 
 * 44    9/25/98 7:25p Jeff
 * 
 * 43    9/25/98 4:50p Jeff
 * 
 * 42    9/24/98 6:54p Jeff
 * added DisconnectMe() and made use of it when you get kicked/banned
 * 
 * 41    9/24/98 6:29p Jeff
 * 
 * 40    9/24/98 5:52p Jeff
 * starting adding statistical death messages, checked in for testing
 * 
 * 39    9/23/98 5:27p Jeff
 * fixed death message bug (using objnum instead of pnum)
 * 
 * 38    9/23/98 4:17p Jeff
 * basic changes/improvements, started changing death messages
 * 
 * 37    9/21/98 7:11p Jeff
 * made InputCommand interface API and moved existing input commands to
 * the interface.  Changed mprintf/ASSERT so they are valid in DMFC
*
* $NoKeywords: $
*/

#include "gamedll_header.h"
#include "DMFC.h"
#include "dmfcinternal.h"
#include "dmfcinputcommands.h"


#include <stdlib.h>
#include <stdarg.h>

char **DMFCStringTable;
int DMFCStringTableSize = 0;
char *_DMFCErrorString = "DMFC Missing String";
ubyte seeds1[31] = {49,73,0,44,87,253,35,74,62,250,4,247,251,72,244,30,59,61,60,52,50,237,23,48,56,55,65,232,231,230,0};
ubyte seeds2[6]	 = {70,95,103,102,112,0};



char *DMFCGetString(int d)
{
	if( (d<0) || (d>=DMFCStringTableSize) ) 
		return _DMFCErrorString; 
	else 
		return DMFCStringTable[d];
}

DMFCBase::DMFCBase(void)
{
	m_cPtrs = 0;

	m_iServerHUDCallsignLevel = HUD_CALLSIGN_LEVEL_FULL;
	m_iMyCurrentHUDCallsignLevel = m_iMyPreferredHUDCallsignLevel = HUD_CALLSIGN_LEVEL_FULL;
	m_iProtectedFlags = 0;
	m_bHasInitialized = false;
	LossGuageEnabled = true;
	DMFCInit = false;
	RealGametime = 0;
	DedicatedLevelWait = 0;
	m_bDisplayTimelimitCountdown = true;
	m_iTimeLimitCountdown = 10;	

	ENABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_PLRFIRSTTIME|DMFC_PRF_CANTEAMCHANGE);
	ENABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_AUTOSAVELEVELEND);
	ENABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_AUTOTIMELIMIT|DMFC_PRF_AUTODEATHMSG|DMFC_PRF_AUTOTEAMSELECT|DMFC_PRF_DISPSTATHUDMSGS);
	DISABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_INOPTIONS|DMFC_PRF_DISPPLAYERINFO|DMFC_PRF_DISPNETGAMEINFO);
	DISABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_AUTOSAVEDISC|DMFC_PRF_DISPMENUBACKGR|DMFC_PRF_PAUSETIME);

	m_iNumBanPlayers = 0;
	m_bMakeClientsWait = false;
	m_iPlayerDisplayed = 0;

	DatabaseRegisteredName[0] = '\0';


	SPRoot = NULL;			//no special packet handlers yet
	
	int i;

	for(i=0;i<MAX_DEATH_MSGS;i++){
		DeathMsgs[i].inuse = false;
		DeathMsgs[i].message = NULL;
		SuicideMsgs[i].inuse = false;
		SuicideMsgs[i].message = NULL;
	}

	for(i = 0; i<MAX_WEAPONS;i++){
		WeaponMessages[i].inuse = false;
		WeaponMessages[i].message = NULL;
		WeaponHash[i] = -1;
	}

	m_iDeathMsgCount = 0;
	m_iSuicideMsgCount = 0;
	m_iUIWindowID = -1;
	m_UIUserData = NULL;

	m_DenyList = NULL;
	m_AllowList = NULL;
	m_InputCommandRootNode = NULL;
	Hard_max_players = DLLMAX_PLAYERS;

	DecryptData(seeds2,5);

	TeamMenuItem = NULL;
	AutoBalanceItem = NULL;
	AllowTeamChangeItem = NULL;
	ObserverItem = NULL;
	m_BanList = NULL;
	StatisticMessagesItem = NULL;
	SaveStatsLevelEndItem = NULL;
	SaveStatsDisconnectItem = NULL;
	NetGameInfoItem = NULL;
	MenuBackgroundItem = NULL;
	ServerHUDCallsignsItem = NULL;
	HUDCallsignsItem = NULL;
	DeathMessageFilterItem = NULL;
	ShipLogosItem = NULL;
	HUDIndicatorItem = NULL;

	DisplayTauntIndicator = false;
	TauntIndicatorPlayerNum = -1;
	TauntIndicatorStartTime = 0;
	TauntIndicatorBMP = -1;

	for(i=0;i<DLLMAX_TEAMS;i++)
		DMFC_team_names[i][0]='\0';

	for(i=0;i<DLLMAX_PLAYERS;i++){
		PilotPicBmpHandles[i] = BAD_BITMAP_HANDLE;
	}

	InitializeVirtualHandles();
}

DMFCBase::~DMFCBase(void)
{
	int i;

	ResetPInfo();

	RemoveAllBans();

	for(i=0;i<DLLMAX_PLAYERS;i++){
		if(PilotPicBmpHandles[i]>BAD_BITMAP_HANDLE){
			DLLbm_FreeBitmap(PilotPicBmpHandles[i]);
			PilotPicBmpHandles[i] = BAD_BITMAP_HANDLE;
		}
	}

	if(TauntIndicatorBMP>BAD_BITMAP_HANDLE){
		DLLbm_FreeBitmap(TauntIndicatorBMP);
		TauntIndicatorBMP = BAD_BITMAP_HANDLE;
	}

	for(i=0;i<MAX_DEATH_MSGS;i++){
		if(DeathMsgs[i].inuse){
			if(DeathMsgs[i].message)
				free(DeathMsgs[i].message);
			DeathMsgs[i].inuse = false;
		}
		if(SuicideMsgs[i].inuse){
			if(SuicideMsgs[i].message)
				free(SuicideMsgs[i].message);
			SuicideMsgs[i].inuse = false;
		}
	}

	for(i = 0; i<MAX_WEAPONS;i++){
		if(WeaponMessages[i].inuse){
			if(WeaponMessages[i].message){
				free(WeaponMessages[i].message);
				WeaponMessages[i].message = NULL;
			}
		}
	}

	//remove all the special packet handlers
	tSPHandler *current,*next;
	current = next = SPRoot;
	while(current){
		next = current->next;
		free(current);
		current=next;
	}
	SPRoot = NULL;
}


//##################Setup and processing functions#######################

// DMFCBase::InitializeForLevel
//
//
void DMFCBase::InitializeForLevel(void)
{
	if(m_bHasInitialized)
		return;

	//Reset the timers
	InitTimers();

	m_bHasInitialized = true;
}

// DMFCBase::LoadSettings
//
//	Loads the settings of DMFC from the registry and initializes those variables
void DMFCBase::LoadSettings(void)
{
	//set default values
	ENABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_AUTOTIMELIMIT|DMFC_PRF_AUTODEATHMSG|DMFC_PRF_AUTOTEAMSELECT|DMFC_PRF_DISPSTATHUDMSGS);
	ENABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_AUTOSAVELEVELEND|DMFC_PRF_CANTEAMCHANGE);
	DISABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_AUTOSAVEDISC|DMFC_PRF_DISPMENUBACKGR);
	m_iServerHUDCallsignLevel = HUD_CALLSIGN_LEVEL_FULL;
	m_iMyCurrentHUDCallsignLevel = m_iMyPreferredHUDCallsignLevel = HUD_CALLSIGN_LEVEL_FULL;
	m_iDeathMessageFilter = DM_FILTER_FULL;
	LossGuageEnabled = true;

	bool bTemp;

	if(DatabaseRead3("DMFCAutoTimeLimit",&bTemp)){
		if(bTemp) ENABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_AUTOTIMELIMIT); 
		else DISABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_AUTOTIMELIMIT);
	}

	if(DatabaseRead3("DMFCAutoDeathMessage",&bTemp)){
		if(bTemp) ENABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_AUTODEATHMSG); 
		else DISABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_AUTODEATHMSG);
	}

	if(DatabaseRead3("DMFCAutoTeamSelect",&bTemp)){
		if(bTemp) ENABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_AUTOTEAMSELECT); 
		else DISABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_AUTOTEAMSELECT);
	}

	if(DatabaseRead3("DMFCStatHUDMessages",&bTemp)){
		if(bTemp) ENABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_DISPSTATHUDMSGS); 
		else DISABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_DISPSTATHUDMSGS);
	}

	if(DatabaseRead3("DMFCAutoSaveStatLevelEnd",&bTemp)){
		if(bTemp) ENABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_AUTOSAVELEVELEND);
		else DISABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_AUTOSAVELEVELEND);
	}

	if(DatabaseRead3("DMFCAutoSaveStatDisconnect",&bTemp)){
		if(bTemp) ENABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_AUTOSAVEDISC);
		else DISABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_AUTOSAVEDISC);
	}

	if(DatabaseRead3("DMFCMenuBackground",&bTemp)){
		if(bTemp) ENABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_DISPMENUBACKGR);
		else DISABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_DISPMENUBACKGR);
	}
	
	if(DatabaseRead3("DMFCCanChangeTeams",&bTemp)){
		if(bTemp) ENABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_CANTEAMCHANGE);
		else DISABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_CANTEAMCHANGE);
	}

	DatabaseRead3("DMFCLagLossIndicator",&LossGuageEnabled);

	
	DatabaseRead2("DMFCShowHudCallsigns"		,&m_iMyPreferredHUDCallsignLevel,sizeof(m_iMyPreferredHUDCallsignLevel));
	DatabaseRead2("DMFCServerShowHudCallsigns"	,&m_iServerHUDCallsignLevel,sizeof(m_iServerHUDCallsignLevel));
	DatabaseRead2("DMFCDeathMessageFilter"		,&m_iDeathMessageFilter,sizeof(m_iDeathMessageFilter));	

	m_iMyCurrentHUDCallsignLevel = m_iMyPreferredHUDCallsignLevel;
}

// DMFCBase::SaveSettings
//
//	Saves out the settings of DMFC to the registry
void DMFCBase::SaveSettings(void)
{
	bool bTemp;

	bTemp = (bool)((m_iProtectedFlags&DMFC_PRF_AUTOTIMELIMIT)!=0);
	DatabaseWrite2("DMFCAutoTimeLimit"			,bTemp);

	bTemp = (bool)((m_iProtectedFlags&DMFC_PRF_AUTODEATHMSG)!=0);
	DatabaseWrite2("DMFCAutoDeathMessage"		,bTemp);

	bTemp = (bool)((m_iProtectedFlags&DMFC_PRF_AUTOTEAMSELECT)!=0);
	DatabaseWrite2("DMFCAutoTeamSelect"			,bTemp);

	bTemp = (bool)((m_iProtectedFlags&DMFC_PRF_DISPSTATHUDMSGS)!=0);
	DatabaseWrite2("DMFCStatHUDMessages"		,bTemp);

	bTemp = (bool)((m_iProtectedFlags&DMFC_PRF_AUTOSAVELEVELEND)!=0);
	DatabaseWrite2("DMFCAutoSaveStatLevelEnd"	,bTemp);

	bTemp = (bool)((m_iProtectedFlags&DMFC_PRF_AUTOSAVEDISC)!=0);
	DatabaseWrite2("DMFCAutoSaveStatDisconnect"	,bTemp);

	bTemp = (bool)((m_iProtectedFlags&DMFC_PRF_DISPMENUBACKGR)!=0);
	DatabaseWrite2("DMFCMenuBackground"			,bTemp);

	bTemp = (bool)((m_iProtectedFlags&DMFC_PRF_CANTEAMCHANGE)!=0);
	DatabaseWrite2("DMFCCanChangeTeams"			,bTemp);

	DatabaseWrite2("DMFCLagLossIndicator",LossGuageEnabled);

	DatabaseWrite2("DMFCShowHudCallsigns"		,m_iMyPreferredHUDCallsignLevel);
	DatabaseWrite2("DMFCServerShowHudCallsigns"	,m_iServerHUDCallsignLevel);
	DatabaseWrite2("DMFCDeathMessageFilter"		,m_iDeathMessageFilter);
}

// DMFCBase::LoadFunctions
//
//	Initialized all the functions for a multiplayer game...must be first thing called
#ifndef MACINTOSH
void DMFCBase::LoadFunctions(int *api_func)
{
	#include "dmfcdllinit.h"
}
#endif

// DMFCBase::GameInit
//   
//   Sets up all the DLL functions and pointers and preps the class for use.  This ABSOLUTLY must be 
//   called, so if you override DMFCApp::GameInit, make sure that you put a call to this somewhere in
//   the override.
void DMFCBase::GameInit(int teams)
{	
	ASSERT(!DMFCInit);
	DMFCInit = true;

	LoadSettings();

	DLLCreateStringTable("dmfc.str",&DMFCStringTable,&DMFCStringTableSize);
	mprintf((0,"DMFC Note: %d strings loaded from string table\n",DMFCStringTableSize));

	//initialize player records
	PRec_Init();

	//initialize remote administration
	Remote_Initialize();

	//see if we should display Outrage logo at all
	if(DLLFindArg("-nooutragelogo"))
		m_bDisplayOutrageLogo = false;
	else
		m_bDisplayOutrageLogo = true;

	SetNumberOfTeams(teams);

	SetTeamName(RED_TEAM,DTXT_RED,false);
	SetTeamName(BLUE_TEAM,DTXT_BLUE,false);
	SetTeamName(GREEN_TEAM,DTXT_GREEN,false);
	SetTeamName(YELLOW_TEAM,DTXT_YELLOW,false);

	//Load bitmaps
	hBitmapObserver = DLLbm_AllocLoadFileBitmap("observer icon.ogf",0,BITMAP_FORMAT_1555);
	if(hBitmapObserver<=BAD_BITMAP_HANDLE)
		hBitmapObserver = BAD_BITMAP_HANDLE;


	RegisterPacketReceiver(SPID_TEAMASSIGNMENT,&DMFCBase::GetTeamAssignmentPacket);
	RegisterPacketReceiver(SPID_CHANGETEAM,&DMFCBase::GetChangeTeamPacket);
	RegisterPacketReceiver(SPID_REQGAMESTATE,&DMFCBase::GetGameStateRequest);
	RegisterPacketReceiver(SPID_DMFCGAMEINFO,&DMFCBase::GetDMFCGameInfo);
	RegisterPacketReceiver(SPID_VERSIONID,&DMFCBase::GetDMFCVersionCheck);
	RegisterPacketReceiver(SPID_PRECORD,PRec_ReceivePRecFromServer);
	RegisterPacketReceiver(SPID_PRECORDREQ,&DMFCBase::ReceiveRequestForPlayerRecords);
	RegisterPacketReceiver(SPID_CONTROLMSG,&DMFCBase::ReceiveControlMessage);
	RegisterPacketReceiver(SPID_NETGAMESYNC,&DMFCBase::ReceiveNetGameInfoSync);
	RegisterPacketReceiver(SPID_NEWTEAMNAME,&DMFCBase::ReceiveNewTeamName);
	RegisterPacketReceiver(SPID_REMOTEKEY,&DMFCBase::GetRemoteKey);
	RegisterPacketReceiver(SPID_REALGAMETIME,&DMFCBase::GetRealGametimePacket);
	AddWeaponHash("Laser","Laser Level 1 - Red","Laser Level 2 - Blue","Laser Level 3 - Purple",
		"Laser Level 4 - Green",NULL);
	AddWeaponHash("Vauss","Vauss Spark",NULL);
	AddWeaponHash("Raygun",NULL);
	AddWeaponHash("Plasma","plasma sparks",NULL);
	AddWeaponHash("Fusion",NULL);
	AddWeaponHash("Super Laser","Laser Level 5 -Yellow","Laser Level 6 -White",NULL);
	AddWeaponHash("Mass Driver",NULL);
	AddWeaponHash("Napalm",NULL);
	AddWeaponHash("EMDyellowleft","EMDyellowright","EMDgreenleft","EMDgreenright",NULL);
	AddWeaponHash("Omega",NULL);
	AddWeaponHash("Concussion",NULL);
	AddWeaponHash("Homing",NULL);
	AddWeaponHash("Smart","SmartPlasmaHomers",NULL);
	AddWeaponHash("Mega","MegaExplosion",NULL);
	AddWeaponHash("Frag","frag particles",NULL);
	AddWeaponHash("Guided",NULL);
	AddWeaponHash("NapalmRocket","NapalmBlob","NapalmParticles",NULL);
	AddWeaponHash("Cyclone Pack","Swarm",NULL);
	AddWeaponHash("Yellow Flare",NULL);

	SetWeaponDeathMessage("Laser"			,DTXT_DEATHLASER	,false);
	SetWeaponDeathMessage("Vauss"			,DTXT_DEATHVAUSS	,false);
	SetWeaponDeathMessage("RayGun"			,DTXT_DEATHMICROWAVE,true);
	SetWeaponDeathMessage("Plasma"			,DTXT_DEATHPLASMA	,false);
	SetWeaponDeathMessage("Fusion"			,DTXT_DEATHFUSION	,false);
	SetWeaponDeathMessage("Super Laser"		,DTXT_DEATHSUPERLASER,false);
	SetWeaponDeathMessage("Mass Driver"		,DTXT_DEATHMASS		,false);
	SetWeaponDeathMessage("Napalm"			,DTXT_DEATHNAPALM	,false);
	SetWeaponDeathMessage("EMDyellowleft"	,DTXT_DEATHEMD		,false);
	SetWeaponDeathMessage("Omega"			,DTXT_DEATHOMEGA	,false);
	SetWeaponDeathMessage("Concussion"		,DTXT_DEATHCONC		,true);
	SetWeaponDeathMessage("Homing"			,DTXT_DEATHHOMING	,false);
	SetWeaponDeathMessage("Smart"			,DTXT_DEATHSMART	,true);
	SetWeaponDeathMessage("Mega"			,DTXT_DEATHMEGA		,false);
	SetWeaponDeathMessage("Frag"			,DTXT_DEATHFRAG		,true);
	SetWeaponDeathMessage("Guided"			,DTXT_DEATHGUIDED	,false);
	SetWeaponDeathMessage("NapalmRocket"	,DTXT_DEATHNAPALMROCKET,true);
	SetWeaponDeathMessage("Cyclone Pack"	,DTXT_DEATHCYCLONE	,false);
	SetWeaponDeathMessage("Yellow Flare"	,DTXT_DEATHFLARE	,false);

/*
$$TABLE_WEAPON "Laser"
$$TABLE_WEAPON "Laser Level 1 - Red"
$$TABLE_WEAPON "Laser Level 2 - Blue"
$$TABLE_WEAPON "Laser Level 3 - Purple"
$$TABLE_WEAPON "Laser Level 4 - Green"
$$TABLE_WEAPON "Vauss"
$$TABLE_WEAPON "Vauss Spark"
$$TABLE_WEAPON "Raygun"
$$TABLE_WEAPON "Plasma"
$$TABLE_WEAPON "plasma sparks"
$$TABLE_WEAPON "Fusion"
$$TABLE_WEAPON "Super Laser"
$$TABLE_WEAPON "Laser Level 5 -Yellow"
$$TABLE_WEAPON "Laser Level 6 -White"
$$TABLE_WEAPON "Mass Driver"
$$TABLE_WEAPON "Napalm"
$$TABLE_WEAPON "EMDyellowleft"
$$TABLE_WEAPON "EMDyellowright"
$$TABLE_WEAPON "EMDgreenleft"
$$TABLE_WEAPON "EMDgreenright"
$$TABLE_WEAPON "Omega"
$$TABLE_WEAPON "Concussion"
$$TABLE_WEAPON "Homing"
$$TABLE_WEAPON "Smart"
$$TABLE_WEAPON "SmartPlasmaHomers"
$$TABLE_WEAPON "Mega"
$$TABLE_WEAPON "MegaExplosion"
$$TABLE_WEAPON "Frag"
$$TABLE_WEAPON "frag particles"
$$TABLE_WEAPON "Guided"
$$TABLE_WEAPON "NapalmRocket"
$$TABLE_WEAPON "NapalmBlob"
$$TABLE_WEAPON "NapalmParticles"
$$TABLE_WEAPON "Cyclone Pack"
$$TABLE_WEAPON "Swarm"
$$TABLE_WEAPON "Yellow Flare"
*/

	//Set up default messages so we have something
	AddDeathMessage(DTXT_KILLED1,true);
	AddSuicideMessage(DTXT_SUICIDE1);

	//Setup InputCommand handlers
	InputCommandInit();

	ParseStartupScript();

	TauntIndicatorBMP = DLLbm_AllocLoadFileBitmap("Taunt.ogf",0,BITMAP_FORMAT_1555);

	MenuBackgroundBMP = DLLbm_AllocBitmap(32,32,0);
	if(MenuBackgroundBMP>BAD_BITMAP_HANDLE){
		ushort *data = DLLbm_data(MenuBackgroundBMP,0);
		int j;
		for(j=0;j<32*32;j++)
			data[j] = OPAQUE_FLAG|GR_RGB(0,0,0);
	}

	MenuItem *lev1,*lev2,*lev3;
	
	if(GetLocalRole()==LR_SERVER){
		//Start Server Commands Menu
		lev1 = new MenuItem(DTXT_MNUSRVRCOMMAND,MIT_NORMAL,0,NULL);		//Server
		lev2 = new MenuItem(DTXT_MNUKICK,MIT_PLIST,0,KickPlayer);				// |- Kick
		lev1->AddSubMenu(lev2);
		lev2 = new MenuItem(DTXT_MNUBAN,MIT_PLIST,0,BanPlayer);					// |- Ban
		lev1->AddSubMenu(lev2);

		tCustomMenu cm;
		cm.GetItem = GetBannedPlayerString;
		cm.GetListCount = GetBanPlayerList;
		lev2 = new MenuItem(DTXT_OSM_REMOVEBAN,MIT_CUSTOM,0,RemoveBanByIndex,&cm);
		lev1->AddSubMenu(lev2);

		lev2 = new MenuItem(DTXT_OSM_REHASHLIST,MIT_NORMAL,0,RehashAllowDenyLists);
		lev1->AddSubMenu(lev2);
		
		lev2 = new MenuItem(DTXT_MNUENDLEVEL,MIT_NORMAL,0,EndMultiLevel);	// |- End Level
		lev1->AddSubMenu(lev2);
		ServerHUDCallsignsItem = lev2 = new MenuItem(DTXT_OSM_MAXHUDNAME,MIT_STATE,0,SwitchServerHudPlayerName,3,m_iServerHUDCallsignLevel,DTXT_PLAIN_NONE,DTXT_OSM_TEAMONLY,DTXT_OSM_FULL);
		lev1->AddSubMenu(lev2);
		if(teams>1){
			//Team Game Stuff here
			lev2 = new MenuItem(DTXT_MNUTEAMCONTROL,MIT_NORMAL,0,NULL);				// |- Team Control
			lev3 = new MenuItem(DTXT_MNUBALANCE,MIT_NORMAL,0,BalanceTeams);	//		|- Balance Teams
			lev2->AddSubMenu(lev3);														//    |- AutoTeam Balance Off/On
			AutoBalanceItem = lev3 = new MenuItem(DTXT_MNUAUTOBALANCE,MIT_STATE,0,SwitchAutoTeamSelect,2,(m_iProtectedFlags&DMFC_PRF_AUTOTEAMSELECT)?1:0,DTXT_MNUOFF,DTXT_MNUON);
			lev2->AddSubMenu(lev3);
			AllowTeamChangeItem = lev3 = new MenuItem(DTXT_MNUALLOWTEAMCNG,MIT_STATE,0,SwitchTeamChange,2,AllowTeamChange()?1:0,DTXT_MNUOFF,DTXT_MNUON);
			lev2->AddSubMenu(lev3);
			lev3 = new MenuItem(DTXT_OSM_TEAMCONFIG,MIT_NORMAL,0,OnScreenDisplayTeamConfig);
			lev2->AddSubMenu(lev3);
			lev1->AddSubMenu(lev2);
		}
			
		Menu.AddSubMenu(lev1);

		//End Server Commands Menu
	}

	if(GetLocalRole()==LR_CLIENT){
		//Start Client Commands Menu
		bool peertopeer = (bool)((Netgame->flags&NF_PEER_PEER)!=0);
		if(!peertopeer)
		{
			lev1 = HUDIndicatorItem = new MenuItem(DTXT_LOSSPINGIND,MIT_STATE,0,SwitchLossPingIndicator,2,(LossGuageEnabled)?1:0,DTXT_MNUOFF,DTXT_MNUON);
			Menu.AddSubMenu(lev1);
		}
		//End Client Commands Menu
	}

	//Do General stuff here
	lev1 = new MenuItem(DTXT_MNUOBSERVER,MIT_NORMAL,0,NULL);
	ObserverItem = lev2 = new MenuItem(DTXT_MNUOBSERVER,MIT_STATE,0,SwitchObserverMode,2,0,DTXT_MNUOFF,DTXT_MNUON);
	lev1->AddSubMenu(lev2);
	
	lev2 = new MenuItem(DTXT_OSM_PIGGYBACK,MIT_PLIST,0,SwitchPiggyBack);
	lev1->AddSubMenu(lev2);
	Menu.AddSubMenu(lev1);

	/*
	lev1 = new MenuItem("OnScreen Menu Options",MIT_NORMAL,0,NULL);
	lev2 = MenuBackgroundItem = new MenuItem("Background",MIT_STATE,0,SwitchMenuBackground,2,0,DTXT_MNUOFF,DTXT_MNUON);
	lev1->AddSubMenu(lev2);
	Menu.AddSubMenu(lev1);
	*/
	lev1 = MenuBackgroundItem = new MenuItem(DTXT_OSM_BACKGROUND,MIT_STATE,0,SwitchMenuBackground,2,0,DTXT_MNUOFF,DTXT_MNUON);
	Menu.AddSubMenu(lev1);

	HUDCallsignsItem = lev1 = new MenuItem(DTXT_OSM_HUDNAMELEVEL,MIT_STATE,0,SwitchHudPlayerNum,3,m_iMyPreferredHUDCallsignLevel,DTXT_PLAIN_NONE,DTXT_OSM_TEAMONLY,DTXT_OSM_FULL);
	Menu.AddSubMenu(lev1);

	ShipLogosItem = lev1 = new MenuItem(DTXT_OSM_SHIPLOGOS,MIT_STATE,0,SwitchShipLogoEnable,2,(AreLogosEnabled())?1:0,DTXT_MNUOFF,DTXT_MNUON);
	Menu.AddSubMenu(lev1);

	AudioTauntsItem = lev1 = new MenuItem(DTXT_OSM_AUDIOTAUNTS,MIT_STATE,0,SwitchAudioTauntsEnable,2,(AreTauntsEnabled())?1:0,DTXT_MNUOFF,DTXT_MNUON);
	Menu.AddSubMenu(lev1);

	//File Statitics menu
	lev1 = new MenuItem(DTXT_OSM_STATSTOFILE,MIT_NORMAL,0,NULL);
	
	lev2 = new MenuItem(DTXT_SAVESTATS,MIT_NORMAL,0,SaveStatsToFile);
	lev1->AddSubMenu(lev2);

	SaveStatsLevelEndItem = lev2 = new MenuItem(DTXT_OSM_STATLEVELEND,MIT_STATE,0,SwitchSaveStatsLevelEnd,2,1,DTXT_MNUOFF,DTXT_MNUON);
	lev1->AddSubMenu(lev2);

	SaveStatsDisconnectItem = lev2 = new MenuItem(DTXT_OSM_STATDISCONNECT,MIT_STATE,0,SwitchSaveStatsDisconnect,2,1,DTXT_MNUOFF,DTXT_MNUON);
	lev1->AddSubMenu(lev2);

	Menu.AddSubMenu(lev1);

	lev1 = new MenuItem(DTXT_MNUGETPLYRINFO,MIT_PLIST,MIF_INCLUDENONE,SwitchPlayerInfo);
	Menu.AddSubMenu(lev1);
	
	NetGameInfoItem = lev1 = new MenuItem(DTXT_OSM_NETGAMEINFO,MIT_STATE,0,SwitchNetGameInfo,2,(m_iProtectedFlags&DMFC_PRF_DISPNETGAMEINFO)?1:0,DTXT_MNUOFF,DTXT_MNUON);
	Menu.AddSubMenu(lev1);

	lev1 = new MenuItem(DTXT_OSM_HUDFILTER,MIT_NORMAL,0,NULL);
	lev2 = DeathMessageFilterItem = new MenuItem(DTXT_OSM_KILLMESSAGES,MIT_STATE,0,SwitchDeathMessageFilter,3,m_iDeathMessageFilter,DTXT_PLAIN_NONE,DTXT_OSM_SIMPLE,DTXT_OSM_FULL);
	lev1->AddSubMenu(lev2);	
	
	lev2 = StatisticMessagesItem = new MenuItem(DTXT_OSM_STATMESSAGES,MIT_STATE,0,SwitchStatHUDMessages,2,1,DTXT_MNUOFF,DTXT_MNUON);
	lev1->AddSubMenu(lev2);
	Menu.AddSubMenu(lev1);

	if(teams>1){
		//Team Game Stuff here
		char names[DLLMAX_TEAMS][MAX_TEAMNAME_LEN];
		strcpy(names[RED_TEAM],GetTeamString(RED_TEAM));
		strcpy(names[BLUE_TEAM],GetTeamString(BLUE_TEAM));
		switch(teams){
		case 2:
			TeamMenuItem = lev1 = new MenuItem(DTXT_MNUCHANGETEAMS,MIT_STATE,0,ChangeTeams,2,GetPlayerTeam(GetPlayerNum()),names[RED_TEAM],names[BLUE_TEAM]);
			break;
		case 3:
			strcpy(names[GREEN_TEAM],GetTeamString(GREEN_TEAM));
			TeamMenuItem = lev1 = new MenuItem(DTXT_MNUCHANGETEAMS,MIT_STATE,0,ChangeTeams,3,GetPlayerTeam(GetPlayerNum()),names[RED_TEAM],names[BLUE_TEAM],names[GREEN_TEAM]);
			break;
		case 4:
			strcpy(names[GREEN_TEAM],GetTeamString(GREEN_TEAM));
			strcpy(names[YELLOW_TEAM],GetTeamString(YELLOW_TEAM));
			TeamMenuItem = lev1 = new MenuItem(DTXT_MNUCHANGETEAMS,MIT_STATE,0,ChangeTeams,4,GetPlayerTeam(GetPlayerNum()),names[RED_TEAM],names[BLUE_TEAM],names[GREEN_TEAM],names[YELLOW_TEAM]);
			break;
		}			
		Menu.AddSubMenu(lev1);
	}

	lev1 = new MenuItem(DTXT_OSM_EXIT,MIT_NORMAL,0,MenuExitMenu);
	Menu.AddSubMenu(lev1);

	//save the hud level var
	int curr_call_level = m_iMyPreferredHUDCallsignLevel;
	m_iMyCurrentHUDCallsignLevel = -1;//set this to -1 so it definitly gets changes/set
	SwitchShowHudCallsignLevel(curr_call_level,false);

	Menu.SetInputFocus();

	EnableShipLogos(true);

	ReadInHostsAllowDeny();

	InitializeForLevel();
}


// DMFCBase::GameClose
//
//   Closes up any needed DLL and DMFC tasks.  This ABSOLUTLY must be called, so if you override
//   DMFCApp::GameClose, make sure that you put a call to this somewhere in the override
void DMFCBase::GameClose(void)
{
	ASSERT(DMFCInit);
	DMFCInit = false;

	SaveSettings();

	//Free bitmaps
	if(hBitmapObserver>BAD_BITMAP_HANDLE){
		DLLbm_FreeBitmap(hBitmapObserver);
		hBitmapObserver = BAD_BITMAP_HANDLE;
	}

	FreeHostsLists();

	//Close up InputCommands
	InputCommandFree();

	//close up and free player records
	PRec_Close();

	if(MenuBackgroundBMP>BAD_BITMAP_HANDLE)
		DLLbm_FreeBitmap(MenuBackgroundBMP);

	DLLDestroyStringTable(DMFCStringTable,DMFCStringTableSize);

}
	

void DMFCBase::DrawMenu(void)
{
	DLLgrtext_SetFont(Game_fonts[HUD_FONT_INDEX]);
	
	//Menu.Draw(10,100,DLLgrfont_GetHeight(Game_fonts[HUD_FONT_INDEX])+1,this,(m_iProtectedFlags&DMFC_PRF_DISPMENUBACKGR)?MenuBackgroundBMP:-1);
	//Menu.Draw(10,100,DLLRenderHUDGetTextHeight("X")+1,this,(m_iProtectedFlags&DMFC_PRF_DISPMENUBACKGR)?MenuBackgroundBMP:-1);
	int font_height = DLLgrfont_GetHeight(Game_fonts[HUD_FONT_INDEX]);

	Menu.Draw(10,(font_height*8)+10,font_height+1,(m_iProtectedFlags&DMFC_PRF_DISPMENUBACKGR)?MenuBackgroundBMP:-1);
}

char	DMFCPlayerInfo[6][100];
void DMFCBase::SetPlayerInfo(int pnum)
{
	if(!CheckPlayerNum(pnum)){
		return;
	}

	m_iPlayerDisplayed = pnum;

	int x,y;
	char buffer[100];
	y = 0;
	x = 15;
	int index = 0;
	bool display_addr = false;

	//Print out callsign
	sprintf(buffer,"%s (%s)",Players[pnum].callsign,(0==pnum)?DTXT_SERVER:DTXT_CLIENT);
	strcpy(DMFCPlayerInfo[index],buffer);	index++;

	if(m_iNumTeams>1 && Players[pnum].team!=-1){
		sprintf(buffer,DTXT_TEAMFORMAT,GetTeamString(GetPlayerTeam(pnum)));
		strcpy(DMFCPlayerInfo[index],buffer); index++;
	}

	//Print out playernum
	sprintf(buffer,DTXT_PLRNUMFORMAT,pnum);		
	strcpy(DMFCPlayerInfo[index],buffer);	index++;

#ifdef _DEBUG
	display_addr = true;
#endif
	if(GetLocalRole()==LR_SERVER)
		display_addr = true;

	//Print out net address
	if(display_addr){
		network_protocol proto = NP_NONE;

		if(pnum!=GetPlayerNum())
			proto = NetPlayers[pnum].addr.connection_type;
		else
			proto = NP_TCP;

		if(proto!=NP_NONE){
			char string[32];
			if(pnum!=0){
				DLLnw_GetNumbersFromHostAddress(&NetPlayers[pnum].addr,string);
			}else{
				int addr = DLLnw_GetThisIP();
				network_address local_addr;
				memset(&local_addr,0,sizeof(network_address));
				memcpy(local_addr.address,&addr,sizeof(int));
				local_addr.connection_type = NP_TCP;

				DLLnw_GetNumbersFromHostAddress(&local_addr,string);
			}
			strcpy(DMFCPlayerInfo[index],string);	index++;
		}
	}

	//Print out Ship info
	sprintf(buffer,DTXT_SHIPFORM,Ships[Players[pnum].ship_index].name);
	strcpy(DMFCPlayerInfo[index],buffer);	index++;

	for(;index<5;index++){
		DMFCPlayerInfo[index][0] = '\0';
	}
}

void DMFCBase::DisplayPlayerInfo(int background_bmp,bool dedicated_server)
{
	DLLgrtext_SetFont(Game_fonts[HUD_FONT_INDEX]);
	
	int height = DLLgrfont_GetHeight(Game_fonts[HUD_FONT_INDEX])+1;
	//int height = DLLRenderHUDGetTextHeight("X")+1;
	ddgr_color color = GR_RGB(180,255,180);
	int x,y;
	y = 0;
	x = 15;
	int index = 0;

	if(!CheckPlayerNum(m_iPlayerDisplayed)){
		m_iPlayerDisplayed = -1;
		DISABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_DISPPLAYERINFO);
		return;
	}

	if( (m_iProtectedFlags&DMFC_PRF_DISPMENUBACKGR) && background_bmp>BAD_BITMAP_HANDLE){
		//draw the background bitmap
		DLLrend_SetAlphaValue(255*0.85f);
		DLLrend_SetZBufferState (0);
		DLLrend_SetTextureType (TT_LINEAR);
		DLLrend_SetLighting (LS_NONE);
		DLLrend_SetAlphaType (AT_CONSTANT_TEXTURE);
		DLLrend_DrawScaledBitmap(0,0,*Game_window_w,6*height,background_bmp,0,0,1,1,1.0,-1,NULL);
		DLLrend_SetZBufferState(1);
	}

	//Print out callsign
	//DLLRenderHUDText(color,255,0,x,y,DMFCPlayerInfo[index]); y+=height;
	DLLgrtext_SetColor(color);
	DLLgrtext_SetAlpha(255);
	DLLgrtext_Printf(x,y,DMFCPlayerInfo[index]);
	y+=height;

	if(dedicated_server)
		DPrintf("%s\n",DMFCPlayerInfo[index]);
	index++;
	color = GR_RGB(40,255,40);

	for(;index<5;index++){
		//DLLRenderHUDText(color,255,0,x,y,DMFCPlayerInfo[index]); 
		DLLgrtext_SetColor(color);
		DLLgrtext_SetAlpha(255);
		DLLgrtext_Printf(x,y,DMFCPlayerInfo[index]);

		if(DMFCPlayerInfo[index][0]!='\0')
			y+=height;
		if(dedicated_server)
			DPrintf("%s\n",DMFCPlayerInfo[index]);
	}

	//print out the volatile information
	player_record *pr = PRec_GetPRecordByPnum(m_iPlayerDisplayed);
	char temp[150];

	if(!pr || pr->state!=STATE_INGAME)
		return;

	int precindex = translate_precptr_to_index(pr);
	float ti = GetTimeInGame(precindex);

	sprintf(temp,DTXT_PI_TIMEINGAME,GetTimeString(ti));
	//DLLRenderHUDText(color,255,0,x,y,temp); y+=height;
	DLLgrtext_SetColor(color);
	DLLgrtext_SetAlpha(255);
	DLLgrtext_Printf(x,y,temp);
	y+=height;

	if(dedicated_server)
		DPrintf("%s\n",temp);
}

// DMFCBase::SwitchPlayerInfoDisplay
//
//	Switches on/off displaying a Playerinfo
void DMFCBase::SwitchPlayerInfoDisplay(int pnum)
{
	m_iProtectedFlags ^= DMFC_PRF_DISPPLAYERINFO;
	if(m_iProtectedFlags&DMFC_PRF_DISPPLAYERINFO){

		if(IsDisplayingNetGameInfo())	//if we are displaying netgame info, turn it off
			SwitchNetGameInfoDisplay(0);

		if(!CheckPlayerNum(pnum)){
			DISABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_DISPPLAYERINFO);
			return;
		}
		SetPlayerInfo(pnum);
	}
}

// DMFCBase::SwitchNetGameInfoDisplay
//
//	Switches on/off displaying netgame info
void DMFCBase::SwitchNetGameInfoDisplay(int on)
{
	if(on) ENABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_DISPNETGAMEINFO);
	else DISABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_DISPNETGAMEINFO);

	if(on){
		if(DisplayingPlayerInfo()!=-1){
			//turn on displaying of player info before displaying netgame info
			SwitchPlayerInfoDisplay(-1);
		}
	}
}

//	DMFCBase::IsDisplayingNetGameInfo
//
//	returns true if we are currently displaying netgame info
bool DMFCBase::IsDisplayingNetGameInfo(void)
{
	return (m_iProtectedFlags&DMFC_PRF_DISPNETGAMEINFO)?true:false;
}

// DMFCBase::DisplayingPlayerInfo
//
//	Returns the pnum of the player info being displayed, -1 if none
int DMFCBase::DisplayingPlayerInfo(void)
{
	if(m_iProtectedFlags&DMFC_PRF_DISPPLAYERINFO)
		return m_iPlayerDisplayed;
	else
		return -1;
}

//DMFCBase::SetTeamName
//
//	Sets the team name for a given team
//	team:	integer value of the team to change
//	name:	new name for the team
//	announce:	if this is true, and we are the server, it will tell all the clients about the change
bool DMFCBase::SetTeamName(int team,char *name,bool announce)
{
	if(team<0 || team>DLLMAX_TEAMS)
		return false;
	if(!name)
		return false;

	char old_teamname[MAX_TEAMNAME_LEN];
	strcpy(old_teamname,DMFC_team_names[team]);

	strncpy(DMFC_team_names[team],name,MAX_TEAMNAME_LEN-1);
	DMFC_team_names[team][MAX_TEAMNAME_LEN-1] = '\0';

	CallOnTeamChangeName(team,old_teamname,DMFC_team_names[team]);

	if(announce && GetLocalRole()==LR_SERVER){
		//tell the clients about the new team name
		SendNewTeamName(team);
		DPrintf(DTXT_SETTEAMNAME,old_teamname,name);
	}

	return true;
}

	
// DMFCBase::GetTeamFromString
//
//		Returns the int value of a team based on a string, -1 if not a team
int DMFCBase::GetTeamFromString(char *str)
{
	if(!str)
		return -1;
	for(int i=0;i<DLLMAX_TEAMS;i++){
		if(!stricmp(str,DMFC_team_names[i]))
			return i;
	}
	return -1;
}

// DMFCBase::GetPlayerNum
//
//   Returns your playernum, useful when finding out if an event is referring to you (by looking at
//   the playernum passed to the event handler, compared to your playernum
int DMFCBase::GetPlayerNum(void)
{
	return *Player_num;
}

// DMFCBase::GetLocalRole
//
//   Returns your role in the multiplayer game.  The result returned will be either LR_SERVER or LR_CLIENT
int DMFCBase::GetLocalRole(void)
{
	return Netgame->local_role;
}

// DMFCBase::CheckPlayerNum
//
//    Returns true if the player number passed in is a valid player number (the player is connected)
bool DMFCBase::CheckPlayerNum(int player_num)
{
	if( (player_num<0) || (player_num>=DLLMAX_PLAYERS) )
		return false;
	if( (NetPlayers[player_num].flags&NPF_CONNECTED) && (NetPlayers[player_num].sequence>=NETSEQ_PLAYING) )
		return true;
	else
		return false;
}

//	DMFCBase::PacketCheckPlayerNum
//
//		Returns true if it's ok to send a packet to this player
bool DMFCBase::PacketCheckPlayerNum(int player_num)
{
	if( (player_num<0) || (player_num>=DLLMAX_PLAYERS) )
		return false;
	if( (NetPlayers[player_num].flags&NPF_CONNECTED) && (NetPlayers[player_num].sequence>=NETSEQ_OBJECTS) )
		return true;
	else
		return false;
}

// DMFCBase::CallClientEvent
//
//    Server Only.  This will send an event to a client for it to execute.
//    event = An EVT_CLIENT_*
//    me_objnum,it_objnum = Object numbers of the objects to be me and it for the event
//    destination = Player number of the client to send to, 0or -1 if to send to all
void DMFCBase::CallClientEvent(int event,int me_objnum,int it_objnum,int destination,bool parms)
{
	if(GetLocalRole()!=LR_SERVER)
		return;
	if( (destination<-1) || (destination>DLLMAX_PLAYERS) )
		return;
	DLLMultiSendClientExecuteDLL(event,me_objnum,it_objnum,destination,parms?Data:NULL);
}


// DMFCBase::GetTimeLeft
//
//    Server Only.  This will fill in the float pointer passed to it with how much time is
//    left in the multiplayer game.  The value placed in the float is only valid if GetTimeLeft
//    returns true.  If it returns false it was because either it is not a game with a time limit
//    or it is being called on a client.
bool DMFCBase::GetTimeLeft(float *time)
{
	if(GetLocalRole()!=LR_SERVER)
		return false;
	if(Netgame->flags&NF_TIMER){
		*time = (Netgame->timelimit*60.0f) - RealGametime;
		return true;
	}
	return false;
}

// DMFCBase::EndLevel
//
//    Server Only.  This will end the current multiplayer level, and will go on to the next level.
void DMFCBase::EndLevel(void)
{
	if(GetLocalRole()!=LR_SERVER)
		return;
	DLLMultiEndLevel();
}

// DMFCBase::GetScoreLimit
//
//   Server Only.  This will fill in the int pointer passed to it with what the scoring limit
//   is, set in in the multiplayer options.  The value point into the int is only valid if
//   GetScoreLimit returns true.  If it returns false it is because you are either not the
//   server or this option wasn't set in the multiplayer options
bool DMFCBase::GetScoreLimit(int *limit)
{
	if(GetLocalRole()!=LR_SERVER)
		return false;
	if(Netgame->flags&NF_KILLGOAL){
		*limit = Netgame->killgoal;
		return true;
	}
	return false;
}

// DMFCBase::AutoTimeLimit
//
//    Server Only.  This turns off or on the automatic level ending by DMFC of a timed multiplayer game.
//    If you turn it off, it is your responsibility to end a time multiplayer game when it's time
//    is up.  If it is turned on, DMFC will automatically handle ending the game.  By default it is on.
void DMFCBase::AutoTimeLimit(bool turnon)
{
	if(GetLocalRole()!=LR_SERVER)
		return;
	if((Netgame->flags&NF_TIMER)==0)
		return;
	if(turnon) ENABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_AUTOTIMELIMIT);
	else DISABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_AUTOTIMELIMIT);
}


// DMFCBase::AutoDeathMessage
//
//    This turns on or off DMFC's automatic handling of death messages.  If it is turned on (Default)
//    then when a player dies it will display a random death message from the list added by you using
//    AddDeathMessage.  If it is turned off, then it is your responsibility to handle the messages, you
//    can use DoRandomDeathMessage to display one when appropriate.
void DMFCBase::AutoDeathMessage(bool turnon)
{
	if(turnon) ENABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_AUTODEATHMSG);
	else DISABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_AUTODEATHMSG);
}


// DMFCBase::AddDeathMessage
//
//    This will add a death message to DMFC.
//    format = string in a "printf" type format (using %s for player callsigns) of the message
//    victim_first = Set this to true if the victim is listed first in the format
void DMFCBase::AddDeathMessage(char *string,bool victim_first)
{
	if( (m_iDeathMsgCount>=0) && (m_iDeathMsgCount<MAX_DEATH_MSGS) ){
		DeathMsgs[m_iDeathMsgCount].message = (char *)malloc(strlen(string)+1);
		if(DeathMsgs[m_iDeathMsgCount].message){
			DeathMsgs[m_iDeathMsgCount].inuse = true;
			DeathMsgs[m_iDeathMsgCount].victim_first = victim_first;
			strcpy(DeathMsgs[m_iDeathMsgCount].message,string);
			m_iDeathMsgCount++;
		}
	}
}
			

// DMFCBase::AddSuicideMessage
//
//    This will add a death message to DMFC.
//    format = string in a "printf" type format (using %s for player callsigns) of the message
void DMFCBase::AddSuicideMessage(char *string)
{
	if( (m_iSuicideMsgCount>=0) && (m_iSuicideMsgCount<MAX_DEATH_MSGS) ){
		SuicideMsgs[m_iSuicideMsgCount].message = (char *)malloc(strlen(string)+1);
		if(SuicideMsgs[m_iSuicideMsgCount].message){
			SuicideMsgs[m_iSuicideMsgCount].inuse = true;
			strcpy(SuicideMsgs[m_iSuicideMsgCount].message,string);
			m_iSuicideMsgCount++;
		}
	}
}

// DMFCBase::GetItObjNum
//
// Returns the it object number of the current event for use.
int DMFCBase::GetItObjNum(void)
{
	if(Data->it_handle==OBJECT_HANDLE_NONE)
		return -1;

	return Data->it_handle&HANDLE_OBJNUM_MASK;
}

// DMFCBase::GetMeObjNum
//
// Returns the me object number of the current event for use.
int DMFCBase::GetMeObjNum(void)
{
	if(Data->me_handle==OBJECT_HANDLE_NONE)
		return -1;
	return Data->me_handle&HANDLE_OBJNUM_MASK;
}


//DMFCBase::OnGameStateRequest
//
//		Server only.  Override this to listen for Game state requests from the clients
//    When this function is called a client (who's player number is passed in) is requesting
//    game state information.  Do what you need to do to send game state information to that player
void DMFCBase::OnGameStateRequest(int pnum)
{
	if(pnum==-1)
		return;
	mprintf((0,"%s is requesting Game State information\n",Players[pnum].callsign));
}


// DMFCBase::GetTeamForNewPlayer
//
//    A Helper function (Server only), which will give you the optimum team assignment (whoever has the
//    lowest amount of players on their team, for a new player.
//    player_num = player number of the new player
//    num_teams = The number of teams in your game
int DMFCBase::GetTeamForNewPlayer(int player_num,int num_teams)
{
	if( (player_num<0) || (player_num>=DLLMAX_PLAYERS) )
		return 0;

	//see if it's a dedicated server game, and if so, are we the server
	if(IAmDedicatedServer() && (player_num==GetPlayerNum()) ){
		return -1;	//place on the special "no team"
	}

	int i;
	int *Teams;
	Teams = (int *)malloc(sizeof(int)*num_teams);
	if(!Teams)
		return 0;

	for(i=0;i<num_teams;i++)
		Teams[i] = 0;

	for(i=0;i<DLLMAX_PLAYERS;i++)
	{
		if((i!=player_num) && (CheckPlayerNum(i)) && (Players[i].team!=-1) )
		{
			if(Players[i].team<num_teams)
				Teams[Players[i].team]++;
		}
	}

	int low_team,low_count;

	low_count = Teams[0];
	low_team = 0;

	for(i=0;i<num_teams;i++)
	{
		if(low_count>Teams[i])
		{
			low_count = Teams[i];
			low_team = i;
		}
	}
	free(Teams);
	return low_team;
}



// DMFCBase::SetNumberOfTeams
//
//    Sets the number of teams to be used in the game.  By default there is 1 team (everyone against everyone).
//    You can set up to a maximum of 4 teams.  Call this function as soon as possible.
//    teams = number of teams
void DMFCBase::SetNumberOfTeams(int teams)
{
	if( (teams<1) || (teams>4) )
		return;

	DLLSetMaxTeams(teams);
	DLLMultiPaintGoalRooms(NULL);

	m_iNumTeams = teams;

	char names[DLLMAX_TEAMS][MAX_TEAMNAME_LEN];
	strcpy(names[RED_TEAM],GetTeamString(RED_TEAM));
	strcpy(names[BLUE_TEAM],GetTeamString(BLUE_TEAM));
	if(TeamMenuItem){
		switch(m_iNumTeams){
		case 2:
			TeamMenuItem->SetStateItemList(m_iNumTeams,names[RED_TEAM],names[BLUE_TEAM]);
			break;
		case 3:
			strcpy(names[GREEN_TEAM],GetTeamString(GREEN_TEAM));
			TeamMenuItem->SetStateItemList(m_iNumTeams,names[RED_TEAM],names[BLUE_TEAM],names[GREEN_TEAM]);
			break;
		case 4:
			strcpy(names[GREEN_TEAM],GetTeamString(GREEN_TEAM));
			strcpy(names[YELLOW_TEAM],GetTeamString(YELLOW_TEAM));
			TeamMenuItem->SetStateItemList(m_iNumTeams,names[RED_TEAM],names[BLUE_TEAM],names[GREEN_TEAM],names[YELLOW_TEAM]);
			break;
		}
	}
}


// DMFCBase::AutoTeamSelect
//
//    Turns on or off DMFC's auto team assignment.  If it is on, then when a new player joins, they will
//    be placed on the team with fewest players.  If it is off, then you must handle that.  Defualt on.
void DMFCBase::AutoTeamSelect(bool turnon)
{
	if(turnon) ENABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_AUTOTEAMSELECT);
	else DISABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_AUTOTEAMSELECT);
}

// DMFCBase::RequestChangeTeams
//
//   If you are the server it will make the player change teams.  If you
//   are a client then it sends a request to the server to change teams
void DMFCBase::RequestTeamChange(int team,int pnum,bool spew_on_respawn)
{
	if(!CheckPlayerNum(pnum))
		return;
	if(Players[pnum].team==-1)	//Dedicated server can't change teams
		return;
	if( (team<0) || (team>=m_iNumTeams) )
		return;
	if(Players[pnum].team==team)
		return;

	if(GetLocalRole()==LR_SERVER){
		if(!AllowTeamChange()){
			return;
		}
		if(!CallOnCanChangeTeam(pnum,team)){
			return;
		}

		Players[pnum].team = team;
		CallOnPlayerChangeTeam(pnum,team,true,spew_on_respawn);
		SendTeamAssignment(pnum,team,spew_on_respawn);
	}else{
		if(pnum!=GetPlayerNum())
			return;
		SendChangeTeamRequest(team,spew_on_respawn);
	}
}


// DMFCBase::AddHudItemCallback
//
//    Adds an item to the hud.  Everytime the hud needs to be updated, it will call the
//    handler passed in.  The handler must be declared as:
//	  void func(struct tHUDItem *item);
//    type = HI_BITMAP  for bitmap, HI_TEXT for a text item
//    func = function callback
void DMFCBase::AddHUDItemCallback(int type,void (*func)(struct tHUDItem *))
{
	//add all the needed Hud Items
	tHUDItem item;
	switch(type)
	{
	case HI_TEXT:
		item.type = HUD_ITEM_CUSTOMTEXT;
		break;
	case HI_BITMAP:
		item.type = HUD_ITEM_CUSTOMIMAGE;
		break;
	default:
		item.type = HUD_ITEM_CUSTOMTEXT;
		break;
	}
	item.stat = STAT_CUSTOM;
	item.flags = HUD_FLAG_PERSISTANT;
	item.data.text = NULL;
	item.render_fn = func;
	item.y = 0;
	item.x = 0;
	DLLAddHUDItem(&item);
}

// DMFCBase::GetMyTeam
//
//    Returns the int value of the team you are on...only useful in a team game.
int DMFCBase::GetMyTeam(void)
{
	return Players[GetPlayerNum()].team;
}

// DMFCBase::GetTeamString
//
//     Returns a pointer to a string name of a team
//   team = integer value of the team
const char *DMFCBase::GetTeamString(int team)
{
	static char name[20];

	if(team>=0 && team<DLLMAX_TEAMS)
		strcpy(name,DMFC_team_names[team]);
	else
		strcpy(name,DTXT_LONER);

	return name;
}

// DMFCBase::GetTeamColor
//
//    Returns the color components of a team
//   team = integer value of the team
ddgr_color DMFCBase::GetTeamColor(int team)
{
	switch(team)
	{
	case RED_TEAM:
		return GR_RGB(255,40,40);
		break;
	case BLUE_TEAM:
		return GR_RGB(40,40,255);
		break;
	case GREEN_TEAM:
		return GR_RGB(40,255,40);
		break;
	case YELLOW_TEAM:
		return GR_RGB(255,255,40);
		break;
	default:
		return GR_RGB(255,255,255);
		break;
	}
}

// DMFCBase::GetNumTeams
//
//   Returns the number of teams in the game
int DMFCBase::GetNumTeams(void)
{
	return m_iNumTeams;
}

// DMFCBase::AllowTeamChange
//
//		Returns true if team changing is allowed
bool DMFCBase::AllowTeamChange(void)
{
	return (m_iProtectedFlags&DMFC_PRF_CANTEAMCHANGE)?true:false;
}

// DMFCBase::SwitchAllowTeamChange
//
//		Turns on/off allowing of team changing
void DMFCBase::SwitchAllowTeamChange(bool turnon)
{
	if(turnon) ENABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_CANTEAMCHANGE);
	else DISABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_CANTEAMCHANGE);
}

// DMFCBase::IsMenuUp
//
//	  Returns true if the on screen menu is being shown
bool DMFCBase::IsMenuUp(void)
{
	return (m_iProtectedFlags&DMFC_PRF_INOPTIONS)?true:false;
}

// DMFCBase::ClipString
//
//	   Given a width, the string will be clipped to that width.  If you pass true for arrow, then an arrow will be
//    appended if the string has been clipped.  Based off current font
void DMFCBase::ClipString(int width,char *string,bool arrow)
{
	if(!string)
		return;

	//float ratio = DEFAULT_HUD_WIDTH / ((float)*Game_window_w);
	
	int string_length = strlen(string);
	char arrow_string[2];
	int arrow_length = 0;
	sprintf(arrow_string,"%c",CHAR_RIGHT_ARROW);
	if(arrow)
	{
		//arrow_length = ratio * ((float)DLLRenderHUDGetTextLineWidth(arrow_string));
		arrow_length = DLLgrtext_GetTextLineWidth(arrow_string);
	}

	if(width<arrow_length){
		//Nothing can fit
		strcpy(string,"");
		return;
	}

	//adjust width based on the arrow
	width -= arrow_length;

	int size = 0;
	char save = string[0];
	while(size<string_length){
		string[size] = '\0';
		//if((((float)DLLRenderHUDGetTextLineWidth(string))*ratio)>=width){
		if(DLLgrtext_GetTextLineWidth(string)>=width){
			//We have to clip
			size--;
			if(arrow){
				string[size] = CHAR_RIGHT_ARROW;
				string[size+1] = '\0';					
			}
			return;
		}
		//replace the char and move to the next
		string[size] = save;
		size++;
		save = string[size];
	}
	//The string didn't need to be clipped
}


// DMFCBase::ConvertHUDAlpha
//
//	  Returns a converted alpha based on what you give, it will be a more transparent if the onscreen menu is up
float DMFCBase::ConvertHUDAlpha(float normal)
{
	if(!IsMenuUp())
		return normal;
	return normal * 0.3f;
}
ubyte DMFCBase::ConvertHUDAlpha(ubyte normal)
{
	if(!IsMenuUp())
		return normal;

	float conv = ((float)normal);
	conv = conv * 0.3f;
	return (ubyte)conv;
}


bool DMFCBase::DMFC_compare_slots(int a,int b)
{
	int ascore,bscore;
	player_record *apr,*bpr;
	apr = GetPlayerRecord(a);
	bpr = GetPlayerRecord(b);
	if( !apr )
		return true;
	if( !bpr )
		return false;
	if( apr->state==STATE_EMPTY )
		return true;
	if( bpr->state==STATE_EMPTY )
		return false;
	if( (apr->state==STATE_INGAME) && (bpr->state==STATE_INGAME) ){
		//both players were in the game
		ascore = apr->dstats.kills[DSTAT_LEVEL] - apr->dstats.suicides[DSTAT_LEVEL];
		bscore = bpr->dstats.kills[DSTAT_LEVEL] - bpr->dstats.suicides[DSTAT_LEVEL];
		return (ascore<bscore);
	}
	if( (apr->state==STATE_INGAME) && (bpr->state==STATE_DISCONNECTED) ){
		//apr gets priority since he was in the game on exit
		return false;
	}
	if( (apr->state==STATE_DISCONNECTED) && (bpr->state==STATE_INGAME) ){
		//bpr gets priority since he was in the game on exit
		return true;
	}
	//if we got here then both players were disconnected
	ascore = apr->dstats.kills[DSTAT_LEVEL] - apr->dstats.suicides[DSTAT_LEVEL];
	bscore = bpr->dstats.kills[DSTAT_LEVEL] - bpr->dstats.suicides[DSTAT_LEVEL];
	return (ascore<bscore);
}

//kills/(kills+deaths+suicides)
bool DMFCBase::DMFC_compare_slots_efficiency(int a,int b)
{
	int ascore,bscore;
	player_record *apr,*bpr;
	float atemp,btemp;

	apr = GetPlayerRecord(a);
	bpr = GetPlayerRecord(b);
	if( !apr )
		return true;
	if( !bpr )
		return false;
	if( apr->state==STATE_EMPTY )
		return true;
	if( bpr->state==STATE_EMPTY )
		return false;
	if( (apr->state==STATE_INGAME) && (bpr->state==STATE_INGAME) ){
		//both players were in the game
		atemp = (float)(apr->dstats.kills[DSTAT_LEVEL]+apr->dstats.suicides[DSTAT_LEVEL]+apr->dstats.deaths[DSTAT_LEVEL]);
		ascore = (float)(apr->dstats.kills[DSTAT_LEVEL])/((atemp)?atemp:0.0000001f);

		btemp = (float)(bpr->dstats.kills[DSTAT_LEVEL]+bpr->dstats.suicides[DSTAT_LEVEL]+bpr->dstats.deaths[DSTAT_LEVEL]);
		bscore = (float)(bpr->dstats.kills[DSTAT_LEVEL])/((btemp)?btemp:0.0000001f);
		return (ascore<bscore);
	}
	if( (apr->state==STATE_INGAME) && (bpr->state==STATE_DISCONNECTED) ){
		//apr gets priority since he was in the game on exit
		return false;
	}
	if( (apr->state==STATE_DISCONNECTED) && (bpr->state==STATE_INGAME) ){
		//bpr gets priority since he was in the game on exit
		return true;
	}
	//if we got here then both players were disconnected
	atemp = (float)(apr->dstats.kills[DSTAT_LEVEL]+apr->dstats.suicides[DSTAT_LEVEL]+apr->dstats.deaths[DSTAT_LEVEL]);
	ascore = (float)(apr->dstats.kills[DSTAT_LEVEL])/((atemp)?atemp:0.0000001f);

	btemp = (float)(bpr->dstats.kills[DSTAT_LEVEL]+bpr->dstats.suicides[DSTAT_LEVEL]+bpr->dstats.deaths[DSTAT_LEVEL]);
	bscore = (float)(bpr->dstats.kills[DSTAT_LEVEL])/((btemp)?btemp:0.0000001f);
	return (ascore<bscore);
}

// DMFCBase::GetSortedPlayerSlots
//
//  Fills in the passed array (of size maxsize) with the playernums sorted by (kills-suicides)
void DMFCBase::GetSortedPlayerSlots(int *sortedindex,int maxsize) 
{
	 int t;
	 int i, j;
	 int tempsort[MAX_PLAYER_RECORDS];

	//copy scores into scoreinfo array
	for(i=0;i<MAX_PLAYER_RECORDS;i++){
		tempsort[i] = i;
	}

	 for(i=1;i<=MAX_PLAYER_RECORDS-1;i++){
		t=tempsort[i];

		  // Shift elements down until
		  // insertion point found.
		  for(j=i-1;j>=0 && DMFC_compare_slots(tempsort[j],t); j--){
			tempsort[j+1] = tempsort[j];
		}

		  // insert
		  tempsort[j+1] = t;
	 }

	 if(maxsize<MAX_PLAYER_RECORDS)
		 memcpy(sortedindex,tempsort,maxsize*sizeof(int));
	 else
		 memcpy(sortedindex,tempsort,MAX_PLAYER_RECORDS*sizeof(int));
}

//	DMFCBase::GetSortedPlayerSlotsByEfficiency
//
//	Fills in the passed array (of size maxsize) with the playernums sorted by kills/(kills+deaths+suicides)
void DMFCBase::GetSortedPlayerSlotsByEfficiency(int *sortedindex,int maxsize)
{
	 int t;
	 int i, j;
	 int tempsort[MAX_PLAYER_RECORDS];

	//copy scores into scoreinfo array
	for(i=0;i<MAX_PLAYER_RECORDS;i++){
		tempsort[i] = i;
	}

	 for(i=1;i<=MAX_PLAYER_RECORDS-1;i++){
		t=tempsort[i];

		  // Shift elements down until
		  // insertion point found.
		  for(j=i-1;j>=0 && DMFC_compare_slots_efficiency(tempsort[j],t); j--){
			tempsort[j+1] = tempsort[j];
		}

		  // insert
		  tempsort[j+1] = t;
	 }

	 if(maxsize<MAX_PLAYER_RECORDS)
		 memcpy(sortedindex,tempsort,maxsize*sizeof(int));
	 else
		 memcpy(sortedindex,tempsort,MAX_PLAYER_RECORDS*sizeof(int));
}

//DMFCBase::DisplayOutrageLogo(void)
//
//   Displays the Outrage Logo for 5 seconds (call repeatedly, after 5 seconds it will just short circuit)
//	Doesn't do anything for non-Outrage games
void DMFCBase::DisplayOutrageLogo(void)
{
	if(!m_bDisplayOutrageLogo)
		return;

#ifdef OUTRAGE_VERSION
	static bool done = false;
	static float time = 0;
	static bool loadflag = true;
	static chunked_bitmap bmp;

	if(done)
		return;

	if(loadflag){
		//We need to load the handle
		loadflag = false;
		int temp = DLLbm_AllocLoadFileBitmap("Outlogo.ogf",0,BITMAP_FORMAT_1555);
		DLLbm_CreateChunkedBitmap(temp,&bmp);
		DLLbm_FreeBitmap(temp);
		time = 0;
	}

	//draw the Outrage Logo if its time
	if(time<5.0){
		int x,y;
		x = (*Game_window_w) - bmp.pw;
		y = (*Game_window_h) - bmp.ph;
		DLLrend_DrawScaledChunkedBitmap(&bmp,x,y,bmp.pw,bmp.ph,128);
		time += (*Frametime);
	}else{
		//we are done
		done = true;
		DLLbm_DestroyChunkedBitmap(&bmp);
	}
#endif
}



//DMFCBase::SwitchShowHudCallsignLevel
//
//
//   Sets the level for displaying of Player's Callsigns on the HUD
void DMFCBase::SwitchShowHudCallsignLevel(ubyte level,bool announce)
{
	m_iMyPreferredHUDCallsignLevel = level;

	if(level>m_iServerHUDCallsignLevel)
		level = m_iServerHUDCallsignLevel;

	if(m_iMyCurrentHUDCallsignLevel==level)
		return;

	m_iMyCurrentHUDCallsignLevel = level;

	switch(level){
	case HUD_CALLSIGN_LEVEL_FULL:
		DLLPlayerSetHUDNameFOV(10);
		if(announce)
			DLLAddHUDMessage(DTXT_HUDLEVEL_FULL);
		break;
	case HUD_CALLSIGN_LEVEL_TEAM:
		DLLPlayerSetHUDNameFOV(10);
		if(announce)
			DLLAddHUDMessage(DTXT_HUDLEVEL_TEAM);
		break;
	case HUD_CALLSIGN_LEVEL_NONE:
		DLLPlayerSetHUDNameFOV(-1);
		if(announce)
			DLLAddHUDMessage(DTXT_HUDLEVEL_NONE);
		break;
	default:
		mprintf((0,"DMFC: Invalid HUD Name Level\n"));
		return;
		break;
	};
}

//DMFCBase::SwitchServerHudCallsignLevel
//
//
//	Sets the max level of HUD callsign displayage...determined by the server
void DMFCBase::SwitchServerHudCallsignLevel(ubyte level)
{
	switch(level){
	case HUD_CALLSIGN_LEVEL_FULL:
		m_iServerHUDCallsignLevel = level;
		DLLAddHUDMessage(DTXT_SHUDLEVEL_FULL);
		break;
	case HUD_CALLSIGN_LEVEL_TEAM:
		m_iServerHUDCallsignLevel = level;
		DLLAddHUDMessage(DTXT_SHUDLEVEL_TEAM);
		break;
	case HUD_CALLSIGN_LEVEL_NONE:
		m_iServerHUDCallsignLevel = level;
		DLLAddHUDMessage(DTXT_SHUDLEVEL_NONE);
		break;
	default:
		mprintf((0,"DMFC: Invalid Server HUD Name Level\n"));
		return;
		break;
	}	

	//make sure we're not set higher than the server allows
	SwitchShowHudCallsignLevel(m_iMyPreferredHUDCallsignLevel);

	if(GetLocalRole()==LR_SERVER)
		SendDMFCGameInfo(SP_ALL);
}

//DMFCBase::ShouldIDisplayHUDName
//
//	Given a player num, it will determine if the callsign should be drawn on the HUD
bool DMFCBase::ShouldIDisplayHUDName(int pnum)
{
	switch(m_iMyCurrentHUDCallsignLevel){
	case HUD_CALLSIGN_LEVEL_FULL:
		return true;
		break;
	case HUD_CALLSIGN_LEVEL_TEAM:
		{
			//see if this is a team game
			if(GetNumTeams()>=2){
				//ok it's a team game, see if this player is on my team, if so, than disply
				if(Players[pnum].team==GetMyTeam()){
					return true;
				}else{
					return false;
				}
			}
			return false;
		}break;
	case HUD_CALLSIGN_LEVEL_NONE:
		return false;
		break;
	}
	return false;
}


//DMFCBase::GetCounterMeasureOwner
//
//
//	Given a counter measure it will determine the pnum of it's owner...if it can't find it, it returns -1
int DMFCBase::GetCounterMeasureOwner(object *robot)
{
	if(!robot)
		return -1;

	if(robot->type!=OBJ_ROBOT && (!(robot->type == OBJ_BUILDING && robot->ai_info)))
		return -1;
	
	object *killer;
	DLLGetUltimateParentForObject(&killer,robot);

	if(killer==robot)
		return -1;

	if( (killer->type!=OBJ_PLAYER) && (killer->type!=OBJ_OBSERVER) )
		return -1;

	return killer->id;
}


//DMFCBase::EncryptData
//
//	Encrypts (weak) a buffer of data
void DMFCBase::EncryptData(ubyte *data,int size)
{
	if(!data)
		return;
	if(size<=0)
		return;

	int offset = size;

	for(int i=0;i<size;i++){
		data[i] = data[i]+i - (offset+i);
		offset++;
	}
}

//DMFCBase::DecryptData
//
// Decrypts a buffer of data
void DMFCBase::DecryptData(ubyte *data,int size)
{
	if(!data)
		return;
	if(size<=0)
		return;

	int offset = size;

	for(int i=0;i<size;i++){
		data[i] = data[i]-i + (offset+i);
		offset++;
	}
}

void DMFCBase::VersionCheck(int pnum)
{
	SendVersionToClient(pnum);
}


// DMFCBase::FindPInfo
//
//	Given a player number it will search the list and find that player, returning the pointer, or NULL if it
//	can't be found or created
PInfo *DMFCBase::FindPInfo(int pnum)
{
	ASSERT( (pnum>=0) && (pnum<DLLMAX_PLAYERS) );
	player_record *pr = GetPlayerRecordByPnum(pnum);
	if(!pr)
		return NULL;
	return (PInfo *)pr->pinfo;
}

// DMFCBase::UpdatePInfo
//
//
//	Updates a victim's pinfo stat (pass in player nums)
void DMFCBase::UpdatePInfo(int victim,int killer,int amount)
{
	ASSERT( (victim>=0) || (victim<DLLMAX_PLAYERS) || (killer>=0) || (killer<DLLMAX_PLAYERS) );
	if( (victim<0) || (victim>=DLLMAX_PLAYERS) || (killer<0) || (killer>=DLLMAX_PLAYERS) ){
		
		return;
	}

	PInfo *id;
	int victim_slot = PRec_GetPlayerSlot(victim);
	tPKillerInfo ki;

	memset(&ki,0,sizeof(tPKillerInfo));
	id = FindPInfo(killer);

	if( (id==NULL) || (victim_slot==-1) ){
		mprintf((0,"Unable to find PInfos\n"));
		return;
	}
	
	//update killer
	ki.kills = amount;
	id->Update(victim_slot,&ki);


	PInfo *kpi,*vpi;
	kpi = id;
	vpi = FindPInfo(victim);
	int killer_slot = PRec_GetPlayerSlot(killer);

	if( (vpi==NULL) || (killer_slot==-1) ){
		mprintf((0,"Unable to find PInfos\n"));
		return;
	}

	if(victim!=killer){
		//regular kill
		kpi->HandleKill(victim_slot);	
		vpi->HandleDeath(killer_slot);
	}else{
		//suicide
		kpi->HandleSuicide();
	}
}


// DMFCBase::ResetPInfo
//
//
//	Resets all the PInfo stats (and frees memory)
void DMFCBase::ResetPInfo(void)
{
	for(int i=0;i<MAX_PLAYER_RECORDS;i++){
		player_record *pr = GetPlayerRecord(i);
		if( pr && pr->pinfo){
			((PInfo *)pr->pinfo)->ResetAll();
		}
	}
}

// DMFCBase::WriteDMFCStatsToFile
//
//
//	This function will write out DMFC's stats to the file given (it must be opened for writing)
void DMFCBase::WriteDMFCStatsToFile(CFILE *file)
{
#define BUFSIZE	150
	ASSERT(file!=NULL);
	char buffer[BUFSIZE];
	player_record *pr,*dpr;
	tPInfoStat stat;	
	int count,length;

	count =1;

	for(int p=0;p<MAX_PLAYER_RECORDS;p++){
		pr = GetPlayerRecord(p);
		if( pr && pr->state!=STATE_EMPTY) {
			//Write out header
			sprintf(buffer,"%d) %s%s",count,(pr->state==STATE_INGAME)?"":"*",pr->callsign);
			DLLcf_WriteString(file,buffer);
			length = strlen(buffer);
			memset(buffer,'=',length);
			buffer[length] = '\0';
			DLLcf_WriteString(file,buffer);

			if(FindPInfoStatFirst(p,&stat)){
				char tempbuffer[25];
				sprintf(buffer,DTXT_DMFC_STAT_HEADER);
				DLLcf_WriteString(file,buffer);
				memset(buffer,' ',BUFSIZE);
				dpr = GetPlayerRecord(stat.slot);

				sprintf(tempbuffer,"%s",dpr->callsign);
				memcpy(buffer,tempbuffer,strlen(tempbuffer));

				sprintf(tempbuffer,"%d",stat.kills);
				memcpy(&buffer[30],tempbuffer,strlen(tempbuffer));

				sprintf(tempbuffer,"%d",stat.deaths);
				memcpy(&buffer[40],tempbuffer,strlen(tempbuffer));
					
				int pos;
				pos = 40 + strlen(tempbuffer) + 1;
				if(pos<BUFSIZE)
					buffer[pos] = '\0';
						
				buffer[BUFSIZE-1] = '\0';
				DLLcf_WriteString(file,buffer);

				while(FindPInfoStatNext(&stat)){
					memset(buffer,' ',BUFSIZE);
					dpr = GetPlayerRecord(stat.slot);

					sprintf(tempbuffer,"%s",dpr->callsign);
					memcpy(buffer,tempbuffer,strlen(tempbuffer));

					sprintf(tempbuffer,"%d",stat.kills);
					memcpy(&buffer[30],tempbuffer,strlen(tempbuffer));

					sprintf(tempbuffer,"%d",stat.deaths);
					memcpy(&buffer[40],tempbuffer,strlen(tempbuffer));
						
					int pos;
					pos = 40 + strlen(tempbuffer) + 1;
					if(pos<BUFSIZE)
						buffer[pos] = '\0';
							
					buffer[BUFSIZE-1] = '\0';
					DLLcf_WriteString(file,buffer);
				}
			}
			FindPInfoStatClose();
			DLLcf_WriteString(file,"");	//skip a line
			count++;
		}
	}
}

tPInfoStat _CurrentPInfoStat;
int _CurrentPInfoSlot = -1;
bool _DoneWithKills = false;
bool _SlotsUsed[MAX_PLAYER_RECORDS];

// DMFCBase::FindPInfoStatFirst
//
//	Fills in a tPInfoStat that contains information about a given player record slot. Returns true on success, false
//	if there is no information available for the given slot. Call this first, then follow with repeated calls to
//	FindPInfoStatNext until you get a false value returned...call FindPInfoStatClose when done.
bool DMFCBase::FindPInfoStatFirst(int slot,tPInfoStat *stat)
{
	for(int i=0;i<MAX_PLAYER_RECORDS;i++)
		_SlotsUsed[i] = false;

	if( slot<0 || slot>=MAX_PLAYER_RECORDS)
		return false;

	if( !stat )
		return false;

	FindPInfoStatClose();

	player_record *pr,*dpr;
	tPKillerInfo *killer,*victim;
	int deaths;

	_SlotsUsed[slot] = true;
	_CurrentPInfoSlot = slot;
	memset(&_CurrentPInfoStat,0,sizeof(tPInfoStat));

	//return the given slot
	pr = GetPlayerRecord(slot);
	if( !pr || pr->state==STATE_EMPTY || !pr->pinfo)
		return false;

	killer = ((PInfo *)pr->pinfo)->GetFirstKiller();
	victim = NULL;

	if(!killer){
		_DoneWithKills = true;
		//go through the slots looking for deaths
		for(int p=0;p<MAX_PLAYER_RECORDS;p++){
			if(!_SlotsUsed[p]){
				_SlotsUsed[p] = true;
				dpr = GetPlayerRecord(p);
				deaths = 0;
				if( dpr && dpr->state!=STATE_EMPTY && dpr->pinfo){
					victim = ((PInfo *)dpr->pinfo)->GetKillerInfo(slot);
					if(victim)
						deaths = victim->kills;
				}
				if(deaths>0){
					_CurrentPInfoStat.slot = p;
					_CurrentPInfoStat.kills = 0;
					_CurrentPInfoStat.deaths = deaths;
					memcpy(stat,&_CurrentPInfoStat,sizeof(tPInfoStat));
					return true;
				}
			}
		}
		return false;
	}

	_CurrentPInfoStat.slot = killer->slot;
	_CurrentPInfoStat.kills = killer->kills;

	deaths = 0;
	dpr = GetPlayerRecord(killer->slot);
	if( dpr && dpr->state!=STATE_EMPTY && dpr->pinfo){
		victim = ((PInfo *)dpr->pinfo)->GetKillerInfo(slot);
	}
	
	if(victim)
		deaths = victim->kills;

	_CurrentPInfoStat.deaths = deaths;
	_SlotsUsed[killer->slot] = true;

	memcpy(stat,&_CurrentPInfoStat,sizeof(tPInfoStat));
	return true;
}

// DMFCBase::FindPInfoStatNext
//
//	Call this repeatedly until you get a value of false, finish by calling FindPInfoStatClose
bool DMFCBase::FindPInfoStatNext(tPInfoStat *stat)
{
	if(_CurrentPInfoSlot==-1)
		return false;
	if( !stat )
		return false;

	int slot;
	player_record *pr,*dpr;
	slot = _CurrentPInfoSlot;
	tPKillerInfo *killer,*victim;
	int deaths = 0;
	memset(&_CurrentPInfoStat,0,sizeof(tPInfoStat));

	//return the given slot
	pr = GetPlayerRecord(slot);
	if( !pr || pr->state==STATE_EMPTY || !pr->pinfo)
		return false;

	killer = ((PInfo *)pr->pinfo)->GetNextKiller();
	victim = NULL;

	if(!killer){
		_DoneWithKills = true;
		//go through the slots looking for deaths
		for(int p=0;p<MAX_PLAYER_RECORDS;p++){
			if(!_SlotsUsed[p]){
				_SlotsUsed[p] = true;
				dpr = GetPlayerRecord(p);
				deaths = 0;
				if( dpr && dpr->state!=STATE_EMPTY && dpr->pinfo){
					victim = ((PInfo *)dpr->pinfo)->GetKillerInfo(slot);
					if(victim)
						deaths = victim->kills;
				}
				if(deaths>0){
					_CurrentPInfoStat.slot = p;
					_CurrentPInfoStat.kills = 0;
					_CurrentPInfoStat.deaths = deaths;
					memcpy(stat,&_CurrentPInfoStat,sizeof(tPInfoStat));
					return true;
				}
			}
		}
		return false;
	}

	_CurrentPInfoStat.slot = killer->slot;
	_CurrentPInfoStat.kills = killer->kills;

	deaths = 0;
	dpr = GetPlayerRecord(killer->slot);
	if( dpr && dpr->state!=STATE_EMPTY && dpr->pinfo){
		victim = ((PInfo *)dpr->pinfo)->GetKillerInfo(slot);
	}
	
	if(victim)
		deaths = victim->kills;

	_CurrentPInfoStat.deaths = deaths;
	_SlotsUsed[killer->slot] = true;

	memcpy(stat,&_CurrentPInfoStat,sizeof(tPInfoStat));
	return true;
}

// DMFCBase::FindPInfoStatClose
//
//	Closes up a FindPInfo series of calls
void DMFCBase::FindPInfoStatClose(void)
{
	_CurrentPInfoSlot = -1;
	_DoneWithKills = false;
}


// DMFCBase::SetWeaponDeathMessage
//
//	Sets a death message for a weapon kill
bool DMFCBase::SetWeaponDeathMessage(char *weapon_name,char *message,bool victim_first)
{
	ASSERT(weapon_name!=NULL);
	ASSERT(message!=NULL);

	int weapon_index = DLLFindWeaponName(IGNORE_TABLE(weapon_name));
	if(weapon_index==-1){
		mprintf((0,"Unable to set WeaponMessage for %s...can't find it\n",weapon_name));
		return false;
	}

	int real_weapon = WeaponHash[weapon_index];

	if(real_weapon==-1){
		mprintf((0,"You forgot to call AddWeaponHash before adding this Message\n"));
		return false;
	}
	
	if( (WeaponMessages[real_weapon].inuse) && (WeaponMessages[real_weapon].message) ){
		free(WeaponMessages[real_weapon].message);
		WeaponMessages[real_weapon].message = NULL;
	}

	WeaponMessages[real_weapon].message = (char *)malloc(strlen(message)+1);
	if(WeaponMessages[real_weapon].message){
		strcpy(WeaponMessages[real_weapon].message,message);
		WeaponMessages[real_weapon].inuse = true;
		WeaponMessages[real_weapon].victim_first = victim_first;
	}
		
	return WeaponMessages[real_weapon].inuse;
}

// DMFCBase::GetWeaponDeathMessage
//
//  Returns the format string for a weapon death message, NULL if it doesn't exist
char *DMFCBase::GetWeaponDeathMessage(int index,bool *victim_first)
{
	if( (index<0) || (index>=MAX_WEAPONS) )
		return NULL;

	int real_weapon;

	real_weapon = WeaponHash[index];

	if(real_weapon==-1)
		return NULL;

	if( !WeaponMessages[real_weapon].inuse )
		return NULL;

	*victim_first = WeaponMessages[real_weapon].victim_first;
	return WeaponMessages[real_weapon].message;
}

// DMFCBase::AddWeaponHash
//
//	Since one weapon may actually consist of many weapons, in order to save space you can create
//  one weapon where all those other weapon id's will be mapped to it...use WeaponHash[id] to
//  get the actual weapon.  End list of children with a NULL
void DMFCBase::AddWeaponHash(char *parent, ... )
{
	ASSERT(parent!=NULL);

	int parent_id = DLLFindWeaponName(IGNORE_TABLE(parent));
	if(parent_id==-1){
		mprintf((0,"Unable to find parent weapon ID in AddWeaponHash (%s)\n",parent));
		return;
	}

	va_list marker;
	va_start (marker,parent);

	WeaponHash[parent_id] = parent_id;

	bool done = false;

	while(!done){
		int id;
		char *name;
		name = va_arg (marker,char *);
		if(name)
			id = DLLFindWeaponName(IGNORE_TABLE(name));
		else
			id = -1;

		if ( id == -1 ){
			done = true;
		}else{
			if ( (id<0) || (id>=MAX_WEAPONS) )
				done = true;
			else{
				WeaponHash[id] = parent_id;
			}
		}
	}

	va_end (marker);
}

// DMFCBase::AddWeaponHashArray
//
//	Since one weapon may actually consist of many weapons, in order to save space you can create
//  one weapon where all those other weapon id's will be mapped to it...use WeaponHash[id] to
//  get the actual weapon.
void DMFCBase::AddWeaponHashArray(char *parent,int count,char **array)
{
	ASSERT(parent!=NULL);

	int parent_id = DLLFindWeaponName(IGNORE_TABLE(parent));
	if(parent_id==-1){
		mprintf((0,"Unable to find parent weapon ID in AddWeaponHash (%s)\n",parent));
		return;
	}

	WeaponHash[parent_id] = parent_id;

	int index = 0;
	while(index<count){
		int id;
		char *name;
		name = array[index++];
		if(name)
			id = DLLFindWeaponName(IGNORE_TABLE(name));
		else
			break;

		if ( id >=0 && id<MAX_WEAPONS )
		{
			WeaponHash[id] = parent_id;
		}
	}
}

// DMFCBase::SetupPlayerRecord
//
//	Sets up memory for multiplayer mod user defined data in the Player Records. Pass in the size (in bytes)
//	of 1 struct (each player will get one).  Use GetPlayerRecordData to retrieve a pointer to the data.
//
//	pack_callback :	callback function called when data from the struct needs to be packed
//					into a packet.  It is SUPER important that this function packs the data
//					in little endian format. This function gets a pointer to the struct that
//					needs to be packed (user_info), and a buffer in which to pack it to.  This
//					function is to return the number of bytes it has packed.
//	unpack_callback : callback function called when data from the struct needs to be unpacket
//					from a packet.  This data will be in little endian format. Returns the number of
//					bytes unpacked.
//	returns:	1 if size given was <=0 (if so all previous user stats will be removed)
//				0 all went ok
//				-1 out of memory (all user stats memory will be freed)
int DMFCBase::SetupPlayerRecord(int sizeof_individual_data,int (*pack_callback)(void *user_info,ubyte *data),int (*unpack_callback)(void *user_info,ubyte *data))
{
	return PRec_SetupUserPRec(sizeof_individual_data,pack_callback,unpack_callback);
}

// DMFCBase::GetPlayerRecordData
//
//	Returns a pointer to the memory set aside for a particular player (NULL on error)
void *DMFCBase::GetPlayerRecordData(int pnum)
{
	int slot;
	slot = PRec_GetPlayerSlot(pnum);
	if(slot==-1)
		return NULL;
	return PRec_GetUserStats(slot);
}

// DMFCBase::GetPlayerRecord
//
//	Returns a pointer to the player record at the given slot (passed in slot must be in the range 0<=slot<MAX_PLAYER_RECORDS)
player_record *DMFCBase::GetPlayerRecord(int slot)
{
	return PRec_GetPRecord(slot);
}

// DMFCBase::GetPlayerRecordByPnum
//
//	Same as above, but you can specify by a player's pnum, return NULL on error
player_record *DMFCBase::GetPlayerRecordByPnum(int pnum)
{
	int slot;
	slot = PRec_GetPlayerSlot(pnum);
	if(slot==-1)
		return NULL;
	return PRec_GetPRecord(slot);
}

// DMFCBase::BanPlayerFromGame
//
//   Puts a temp ban on a player which will only last the duration of the game
void DMFCBase::BanPlayerFromGame(int pnum)
{
	if( pnum>=0 && pnum<DLLMAX_PLAYERS){
		mprintf((0,"Banning %s\n",Players[pnum].callsign));
		DPrintf(DTXT_DEDS_BAN,Players[pnum].callsign);
		tBanItem *c;
		c = m_BanList;
		if(!m_BanList){
			c = m_BanList = (tBanItem *)malloc(sizeof(tBanItem));
			if(!c)
				return;
		}else{
			while(c->next){
				c = c->next;
			}
			c->next = (tBanItem *)malloc(sizeof(tBanItem));
			c = c->next;
			if(!c)
				return;
		}
		c->next = NULL;
		memcpy(&c->addr,&NetPlayers[pnum].addr,sizeof(network_address));
		strncpy(c->callsign,Players[pnum].callsign,MAX_CALLSIGN_SIZE-1);
		c->callsign[MAX_CALLSIGN_SIZE-1] = '\0';
		c->tracker_id[0] = '\0';

		if(IsMasterTrackerGame() ) {
			strcpy(c->tracker_id,Players[pnum].tracker_id);
		}

		m_iNumBanPlayers++;
	}else{
		mprintf((0,"Unable to ban player...pnum not valid\n"));
		DPrintf(DTXT_DEDS_BAN_ERROR);
	}
}


// DMFCBase::RemoveAllBans
//
//	Removes all the temp bans
void DMFCBase::RemoveAllBans(void)
{
	tBanItem *c,*n;
	c = n = m_BanList;
	while(c){
		n = c->next;
		free(c);
		c = n;
	}
	m_BanList = NULL;
	m_iNumBanPlayers = 0;
}

// DMFCBase::GetNumBannedPlayers
//
//	Returns the number of players banned from the server
int DMFCBase::GetNumBannedPlayers(void)
{
	return m_iNumBanPlayers;
}

// DMCBase::GetBannedPlayerCallsign
//
//	Returns the callsign of the banned player at position index, NULL on error
char *DMFCBase::GetBannedPlayerCallsign(int index)
{
	if(index<0 || index>=m_iNumBanPlayers)
		return NULL;

	tBanItem *c;
	c = m_BanList;
	while(c){
		if(index==0){
			return c->callsign;
		}
		index--;
		c = c->next;
	}
	return NULL;
}

// DMFCBase::RemoveBan
//
//	Removes a temp ban on given the ban #...returns true on success
bool DMFCBase::RemoveBan(int index)
{
	if(index<0||index>=m_iNumBanPlayers)
		return false;

	tBanItem *c,*p;
	c = m_BanList;
	p = NULL;
	while(c){
		if(!index){
			//we found em, remove em
			if(p){
				p->next = c->next;
				free(c);
				m_iNumBanPlayers--;
			}else{
				m_BanList = c->next;
				free(c);
				m_iNumBanPlayers--;
			}
			return true;
		}
		p = c;
		c = c->next;
		index--;
	}
	return false;
}

// DMFCBase::IsPlayerBanned
//
//	returns true is the given pnum is banned from the game
bool DMFCBase::IsPlayerBanned(int pnum)
{
	mprintf((0,"Checking a ban on a player..."));

	if(pnum<0 || pnum>=DLLMAX_PLAYERS){
		mprintf((0,"Playernum not valid\n"));
		return false;
	}

	if(IsAddressBanned(&NetPlayers[pnum].addr,Players[pnum].tracker_id)){
		return true;
	}

	return false;
}

// DMFCBase::IsAddressBanned
//
//	returns true if the given address is banned from the game
bool DMFCBase::IsAddressBanned(network_address *addr,char *tracker_id)
{
	tBanItem *c;
	c = m_BanList;

	mprintf((0,"Checking a ban on an address..."));

	while(c){
		if( addr && CompareNetworkAddress(&c->addr,addr,false)){
			mprintf((0,"Player addr IS BANNED\n"));
			return true;
		}
		if(*m_bTrackerGame)
		{
			if( tracker_id && !strcmp(tracker_id,c->tracker_id) ){
				mprintf((0,"Player tid IS BANNED\n"));
				return true;
			}
		}

		c = c->next;
	}

	if(!addr){
		mprintf((0,"Player not banned\n"));
		return false;
	}

	//ok, the player doesn't have a temp ban on him, now, if it's tcp/ip, check
	//the hosts.allow/.deny 

	if(addr->connection_type!=NP_TCP){
		mprintf((0,"Player not banned\n"));
		return false;
	}

	unsigned long address;
	tHostsNode *curr;

	memcpy(&address, &addr->address, 4);

	//check the allow list, if the address is specified on this list,
	//than it isn't banned
	curr = m_AllowList;
	while(curr){
		if( (address&curr->mask)==(curr->ip&curr->mask) ){
			//its a match
			mprintf((0,"Player not banned\n"));
			return false;
		}
		curr = curr->next;
	}

	//check the deny list, if the address is specified on this list,
	//than it is banned
	curr = m_DenyList;
	while(curr){
		if( (address&curr->mask)==(curr->ip&curr->mask) ){
			//its a match
			mprintf((0,"Player IS banned\n"));
			return true;
		}
		curr = curr->next;
	}

	mprintf((0,"Player not banned\n"));
	return false;
}


// DMFCBase::DoDamageToPlayer
//
//	Server only...applies damage to player
void DMFCBase::DoDamageToPlayer(int pnum,int type, float amount,bool playsound)
{
	if(GetLocalRole()!=LR_SERVER)
		return;
	if(!CheckPlayerNum(pnum))
		return;
	if(pnum==GetPlayerNum()){
		//apply it to myself
		if(amount<=0){
			//just add it...no damage sound
			Objects[Players[pnum].objnum].shields -= amount;
		}else{			
			DLLApplyDamageToPlayer (&Objects[Players[pnum].objnum],NULL,type,amount,1,255,playsound);
		}
	}else{
		//add it to additional damage
		DLLApplyDamageToPlayer (&Objects[Players[pnum].objnum],NULL,type,amount,1,255,playsound);
	}
}

// DMFCBase::StartUIWindow
//
//	Prepares the game so that you can display UI stuff
void DMFCBase::StartUIWindow(int id,void *user_data)
{
	if(m_iUIWindowID!=-1){
		mprintf((0,"DMFCBase::StartUIWindow(): A Window ID (0x%X) is already defined\n",m_iUIWindowID));
		return;
	}

	*Game_interface_mode=GAME_DLL_INTERFACE;
	m_iUIWindowID = id;
	m_UIUserData = user_data;
}

//	DMFCBase::EnableStatisticalMessages
//
//	Turns on or off the statistical HUD messages that appear due to a player death
void DMFCBase::EnableStatisticalMessages(bool on)
{
	if(on) ENABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_DISPSTATHUDMSGS);
	else DISABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_DISPSTATHUDMSGS);
}

//	DMFCBase::GetTimeInGame
//
//	Returns the totaltime the player has been in the game at the moment you call
//	this function.  Pass in the player record slot.
float DMFCBase::GetTimeInGame(int slot)
{
	if(slot<0 || slot>=MAX_PLAYER_RECORDS)
		return 0;

	player_record *pr = PRec_GetPRecord(slot);

	if(!pr || pr->state==STATE_EMPTY)
		return 0;

	float ti = pr->total_time_in_game;

	if(pr->state==STATE_INGAME)
		ti += RealGametime - pr->start_time;

	return ti;
}

//	DMFCBase::GetTimeString
//
//	Returns a string that contains a formated time (i.e. 32 seconds, 1:15 minutes, 3:21:23 hours)
char *DMFCBase::GetTimeString(float sec)
{
	static char buffer[20];
	int minutes,seconds,hours;
	minutes = seconds = hours = 0;

	seconds = (int) sec;

	//we have seconds, so convert to minutes:seconds
	minutes = seconds/60;
	seconds = seconds%60;

	//now we have minutes and seconds, so convert minutes to hours:minutes
	hours = minutes/60;
	minutes = minutes%60;

	//now form our string hh:mm:ss
	if(hours){
		if(hours==1 && minutes==0 && seconds==0)
			sprintf(buffer,DTXT_TIME_HOUR);
		else
			sprintf(buffer,DTXT_TIME_HOURS,hours,minutes,seconds);
	}else if(minutes){
		if(minutes==1 && seconds==0)
			sprintf(buffer,DTXT_TIME_MINUTE);
		else
			sprintf(buffer,DTXT_TIME_MINUTES,minutes,seconds);
	}else{
		if(seconds==1)
			sprintf(buffer,DTXT_TIME_SECOND);
		else
			sprintf(buffer,DTXT_TIME_SECONDS,seconds);
	}
	return buffer;
}

// DMFCBase::PauseRealGameTime
//
//	Pauses/unpauses the realgame time
void DMFCBase::PauseRealGameTime(bool enable)
{
	if(GetLocalRole()!=LR_SERVER)
		return;

	if(enable)
	{
		ENABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_PAUSETIME);
		SendControlMessageToPlayer(SP_ALL,CM_PAUSETIME);
	}else
	{
		DISABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_PAUSETIME);
		SendControlMessageToPlayer(SP_ALL,CM_UNPAUSETIME);
	}
}


/*
 ********************************************************************************
 *																				*
 *						Input Command Functions									*
 *																				*
 ********************************************************************************
 *
*/

// DMFCBase::InputCommandHandle
//
//	Goes through the list of input commands and calls the associated handle (if it exists).  Returns
//	true if it called a handler, else false.
bool DMFCBase::InputCommandHandle(char *command_string)
{
	ASSERT(command_string!=NULL);

	//make sure it's a valid command (starts with a $)
	if(command_string[0]!='$')
		return false;

	/*
	if(!stricmp(command_string,"$help dump"))
	{
		mprintf((0,"Dumping help commands to file\n"));
		tInputCommandNode *node = m_InputCommandRootNode;

		CFILE *file;
		DLLOpenCFILE(&file,"C:\\DMFCHelp.txt","wt");

		if(!file)
			return false;

		char buffer[1024];

		while(node){

			sprintf(buffer,"$%s",node->data);
			DLLcf_WriteString(file,buffer);
			DLLcf_WriteString(file,"-------------------------------------------");
			sprintf(buffer,"%s\n",node->desc);
			DLLcf_WriteString(file,buffer);
			node = node->next;
		}

		DLLcfclose(file);
	}
	*/

	//now extract the command
	char command[64],index = 0;
	char *p = &command_string[1];

	//lowercase the command_string
	while(*p && *p!=' '){
		if( (*p>='A') && (*p<='Z') ){
			*p = tolower(*p);
		}			
		p++;
	}

	p = &command_string[1];

	//break off the first string
	while( (*p) && (*p!=' ') ){
		ASSERT(index<64);
		command[index] = *p;
		p++; index++;
	}
	command[index] = '\0';	//tack on an ending NULL

	//now go through our list and see if we can find the command added by the user
	tInputCommandNode *node = m_InputCommandRootNode;
	while(node){
		if(!stricmp(node->data,command)){
			//we found a match!
			if(node->handler){
				(*node->handler)(command_string);
				return true;
			}
			return false;	//for some reason the handler is NULL (it should never happen)
		}
		node = node->next;
	}
	
	//if we got here than no nodes match
	return false;
}

// DMFCBase::InputCommandFree
//
//	Frees up all memory allocated for the input commands ($ messages)
void DMFCBase::InputCommandFree(void)
{
	tInputCommandNode *next,*node = m_InputCommandRootNode;

	//Destroy the linked list of nodes
	while(node){
		next = node->next;
		if(node->data)
			free(node->data);
		if(node->desc)
			free(node->desc);
		free(node);
		node = next;
	}

	//set the root to NULL to finish up
	m_InputCommandRootNode = NULL;
}


// DMFCBase::InputCommandInit
//
//	Initializes the variables and data for the input commands.  Default DMFC commands are to
//	be placed in here.
void DMFCBase::InputCommandInit(void)
{
	if(AddInputCommand(DTXT_IC_ALLOWTEAMCHANGE,DTXT_IC_ALLOWTEAMCHANGED,DMFCInputCommand_AllowTeamChange,true)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_AUTOBALANCE,DTXT_IC_AUTOBALANCED,DMFCInputCommand_AutoBalance,true)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_AUTOSAVEDISC,DTXT_IC_AUTOSAVEDISCD,DMFCInputCommand_AutoSaveDisconnect,true)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_AUTOSAVELEVEL,DTXT_IC_AUTOSAVELEVELD,DMFCInputCommand_AutoSaveLevel,true)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_BALANCE,DTXT_IC_BALANCED,DMFCInputCommand_Balance,true)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_BAN,DTXT_IC_BAND,DMFCInputCommand_Ban,true)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_BANLIST,DTXT_IC_BANLISTD,DMFCInputCommand_BanList)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_CHANGETEAM,DTXT_IC_CHANGETEAMD,DMFCInputCommand_ChangeTeam,true)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_ENDLEVEL,DTXT_IC_ENDLEVELD,DMFCInputCommand_EndLevel,true)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_HELP,DTXT_IC_HELPD,DMFCInputCommand_Help)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_HUDNAME,DTXT_IC_HUDNAMED,DMFCInputCommand_HudCallsigns)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_KICK,DTXT_IC_KICKD,DMFCInputCommand_Kick,true)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_KILLMSGFILTER,DTXT_IC_KILLMSGFILTERD,DMFCInputCommand_KillMsgFilter)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_NETGAMEINFO,DTXT_IC_NETGAMEINFO,DMFCInputCommand_NetGameInfo)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_OBSERVER,DTXT_IC_OBSERVERD,DMFCInputCommand_Observer)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_PIGGYBACK,DTXT_IC_PIGGYBACKD,DMFCInputCommand_Piggyback)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_PLAYERINFO,DTXT_IC_PLAYERINFOD,DMFCInputCommand_PlayerInfo)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_PLAYERS,DTXT_IC_PLAYERSD,DMFCInputCommand_Players)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_REHASH,DTXT_IC_REHASHD,DMFCInputCommand_Rehash,true)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_REMOTE,DTXT_IC_REMOTED,DMFCInputCommand_Remote)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));		

	if(AddInputCommand(DTXT_IC_REMOTEADMIN,DTXT_IC_REMOTEADMIND,DMFCInputCommand_RemoteAdmin,true)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_REMOTEADMINLOGOUT,DTXT_IC_REMOTEADMINLOGOUTD,DMFCInputCommand_RemoteAdminLogout)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_REMOTEADMINPASS,DTXT_IC_REMOTEADMINPASSD,DMFCInputCommand_RemoteAdminPass,true)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_REMOVEBAN,DTXT_IC_REMOVEBAND,DMFCInputCommand_RemoveBan,true)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_SAVESTATS,DTXT_IC_SAVESTATSD,DMFCInputCommand_SaveStats,true)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_SCORES,DTXT_IC_SCORESD,DMFCInputCommand_Scores)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_SERVERHUDNAMES,DTXT_IC_SERVERHUDNAMESD,DMFCInputCommand_ServerHudCallsigns,true)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_SETGOALLIMIT,DTXT_IC_SETGOALLIMITD,DMFCInputCommand_SetGoalLimit,true)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_SETMAXPLAYERS,DTXT_IC_SETMAXPLAYERSD,DMFCInputCommand_SetMaxPlayers,true)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_SETPPS,DTXT_IC_SETPPSD,DMFCInputCommand_SetPPS,true)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_SETRESPAWNTIME,DTXT_IC_SETRESPAWNTIMED,DMFCInputCommand_SetRespawnTime,true)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_SETTEAMNAME,DTXT_IC_SETTEAMNAMED,DMFCInputCommand_SetTeamName,true)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_SETTIMELIMIT,DTXT_IC_SETTIMELIMITD,DMFCInputCommand_SetTimeLimit,true)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_STATMSGS,DTXT_IC_STATMSGSD,DMFCInputCommand_StatMsgs)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_TEAM,DTXT_IC_TEAMD,DMFCInputCommand_Team)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_WAIT,DTXT_IC_WAITD,DMFCInputCommand_Wait,true)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));

	if(AddInputCommand(DTXT_IC_WARP,DTXT_IC_WARPD,DMFCInputCommand_Warp,true)<1)
		mprintf((0,"DMFC Warning: Error Adding Input Command\n"));
}

// Returns true if the input command can be called remotely
//
// Returns 1 if the input command can be called remotely
// Returns -1 if the input command is not found
// Returns 0 if the input command can be called remotely
int DMFCBase::CanInputCommandBeUsedRemotely(char *command)
{
	tInputCommandNode *node = m_InputCommandRootNode;

	while(node){
		if(!stricmp(node->data,command)){
			//we found a match!
			return (node->allow_remote)?1:0;
		}
		node = node->next;
	}

	return -1;
}

//	Displays dedicated server help
void DMFCBase::DisplayInputCommandHelp(char *s)
{
	if(!IAmDedicatedServer())
		return;

	tInputCommandNode *node = m_InputCommandRootNode;

	if(s[0]=='\0'){
		char buffer[80];
		DPrintf(DTXT_IC_HEADEERLIST);
		DPrintf(DTXT_IC_HEADERINSTRUCT);
		//just display the list of commands
		while(node){
			memset(buffer,' ',80);
			memcpy(&buffer[0],node->data,strlen(node->data));
			node = node->next;
			if(node){
				memcpy(&buffer[30],node->data,strlen(node->data));
				int pos = 30 + strlen(node->data);
				if(pos<=78){
					buffer[pos] = '\n';
					buffer[pos+1] = '\0';
				}
			}
			buffer[78] = '\n';
			buffer[79] = '\0';

			DPrintf(buffer);

			if(node)
				node = node->next;
		}

		return;
	}

	//display help description
	while(node){
		if(!stricmp(node->data,s)){
			//we found a match!
			DPrintf("%s:\n",node->data);
			if(node->desc)
				DPrintf("%s\n",node->desc);
			else
				DPrintf(DTXT_IC_NOADDITIONALINFO);
			return;
		}
		node = node->next;
	}

	DPrintf(DTXT_IC_COMMANDNOTFOUND);

}

//copies a string from src to dest, making dest all lower case
void strlowcpy(char *dest,const char *src)
{
	ASSERT((dest!=NULL) && (src!=NULL));
	char *s,*d;
	s = (char *)src;
	d = dest;

	while(*s){
		*d = tolower(*s);
		s++; d++;
	}
	//do the final NULL
	*d = '\0';
}

// DMFCBase::AddInputCommand
//
//	When the client (or dedicated server) types a message that begins with a $, DMFC will look through
//	all the values you passed to AddInputCommand() and see if any match.  If so it passes the entire
//	command string to the given function handler. Returns 1 on success, -1 if out of memory, 0 if it already
//	exists. These commands are not case sensitive.
//	Ex. AddInputCommand("team");	//this handles all the '$team' passed in
signed char DMFCBase::AddInputCommand(char *command,char *description,void (*handler)(char *),bool allow_remotely)
{
	ASSERT(command!=NULL);
	ASSERT(handler!=NULL);

	//traverse to the end of the list checking for duplicates along the way
	tInputCommandNode *node = m_InputCommandRootNode;

	if(!node){
		//there is none currently in the list, add it now
		node = (tInputCommandNode *)malloc(sizeof(tInputCommandNode));
		if(!node){
			return -1;
		}
		//fill in data
		node->next = NULL;
		node->handler = handler;
		node->allow_remote = allow_remotely;
		node->data = (char *)malloc(strlen(command)+1);
		if(!node->data){//out of memory
			free(node);
			return -1;
		}
		strlowcpy(node->data,command);

		node->desc = (char *)malloc(strlen(description)+1);
		if(node->desc)
			strcpy(node->desc,description);
		
		//all went ok
		m_InputCommandRootNode = node;
		return 1;
	}

	while(node->next){
		//check this node to make sure it's not a duplicate
		if(!stricmp(node->data,command)){
			//we found a duplicate
			return 0;
		}
		node = node->next;
	}

	//we need to check the current node since while(node->next) breaks out before checking the last node
	if(!stricmp(node->data,command)){
		//we found a duplicate
		return 0;
	}

	//it looks like everything went ok, now just create a new node and tack it on to the end
	//of the list.  All that can go wrong now is an out of memory error.
	tInputCommandNode *temp;
	temp = (tInputCommandNode *)malloc(sizeof(tInputCommandNode));
	if(!temp){//out of memory
		return -1;
	}
	temp->next = NULL;
	temp->handler = handler;
	temp->allow_remote = allow_remotely;
	temp->data = (char *)malloc(strlen(command)+1);
	if(!temp->data){//out of memory
		free(temp);
		return -1;
	}
	strlowcpy(temp->data,command);

	temp->desc = (char *)malloc(strlen(description)+1);
	if(temp->desc)
		strcpy(temp->desc,description);

	//now tack it on the end of the list and thats all
	node->next = temp;
	return 1;
}

//	DMFCBase::DisconnectMe
//
//	Disconnects yourself from the game.  It should be called when you are kicked or banned to make the
//	whole process a bit nicer/cleaner.  Even if it doesn't get called when kicked, you will still
//	eventually disconnect.
void DMFCBase::DisconnectMe(void)
{
	Netgame->flags |= NF_EXIT_NOW;
}

//	Does a check on on the pinfo info making sure it is valid
void DMFCBase::CheckPInfo()
{
	for(int slot=0;slot<MAX_PLAYER_RECORDS;slot++){
		player_record *pr = GetPlayerRecord(slot);
		if(pr && pr->state!=STATE_EMPTY){
			//now go through the victim list and check
			tPKillerInfo *node;

			ASSERT(pr->pinfo!=NULL);			
			node = ((PInfo *)pr->pinfo)->GetFirstKiller();

			while(node){
				ASSERT(node->slot>=0 && node->slot<64);
				node = node->next;
			}
		}
	}
}

//	DMFCBase::EnableOnScreenMenu
//
// Turns on/off the onscreen menu
void DMFCBase::EnableOnScreenMenu(bool enable)
{
	if(enable) ENABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_INOPTIONS);
	else DISABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_INOPTIONS);
}

//	DMFCBase::EnableAutoSaveLevelEnd
//
//	Disables/Enables the autosaving of game stats to file on level end
void DMFCBase::EnableAutoSaveLevelEnd(bool enable)
{
	if(enable) ENABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_AUTOSAVELEVELEND);
	else DISABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_AUTOSAVELEVELEND);
}

//	DMFCBase::EnableAutoSaveDisconnect
//
//	Disables/Enables the autosaving of game stats to file on disconnect from the game
void DMFCBase::EnableAutoSaveDisconnect(bool enable)
{
	if(enable) ENABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_AUTOSAVEDISC);
	else DISABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_AUTOSAVEDISC);
}

//	DMFCBase::GenerateStatFilename
//
//	Given the following information it will return a full path to what
//	the recommended filename to save stats to should be.
//	root = Multiplayer DLL Name (filename will start with this)
//	end_of_level = pass true if this is the end of a level stats
void DMFCBase::GenerateStatFilename(char *filename,char *root,bool end_of_level)
{
	int level = Current_mission->cur_level;
	char *name = Netgame->name;
	struct tm *newtime;
	time_t long_time;

	time( &long_time );
	newtime = localtime( &long_time );

	char fname[256];
	char timestr[100];
	char day[8],month[8];

	switch(newtime->tm_wday){
	case 0:
		strcpy(day,DTXT_SUNDAY);
		break;
	case 1:
		strcpy(day,DTXT_MONDAY);
		break;
	case 2:
		strcpy(day,DTXT_TUESDAY);
		break;
	case 3:
		strcpy(day,DTXT_WEDNESDAY);
		break;
	case 4:
		strcpy(day,DTXT_THURSDAY);
		break;
	case 5:
		strcpy(day,DTXT_FRIDAY);
		break;
	case 6:
		strcpy(day,DTXT_SATURDAY);
		break;
	}

	switch(newtime->tm_mon){
	case 0:
		strcpy(month,DTXT_JANUARY);
		break;
	case 1:
		strcpy(month,DTXT_FEBRUARY);
		break;
	case 2:
		strcpy(month,DTXT_MARCH);
		break;
	case 3:
		strcpy(month,DTXT_APRIL);
		break;
	case 4:
		strcpy(month,DTXT_MAY);
		break;
	case 5:
		strcpy(month,DTXT_JUNE);
		break;
	case 6:
		strcpy(month,DTXT_JULY);
		break;
	case 7:
		strcpy(month,DTXT_AUGUST);
		break;
	case 8:
		strcpy(month,DTXT_SEPTEMBER);
		break;
	case 9:
		strcpy(month,DTXT_OCTOBER);
		break;
	case 10:
		strcpy(month,DTXT_NOVEMBER);
		break;
	case 11:
		strcpy(month,DTXT_DECEMBER);
		break;
	}

	sprintf(timestr,"%s._%s._%d_%d_%02d%02d",day,month,newtime->tm_mday,newtime->tm_year+1900,newtime->tm_hour,newtime->tm_min);
	sprintf(fname,"%s_%s_%d_%s%s.stats",root,name,level,(end_of_level)?DTXT_ENDOFLEVELCONCAT:"",timestr);

	//remove all spaces (convert to _)
	char *p = fname;
	while( (*p) ){
		if( *p == ' ' )
			*p = '_';
		p++;
	}

	//build the path info here
	DLLddio_MakePath(filename,LocalD3Dir,"netgames",fname,NULL);
}

//	DMFCBase::IsPlayerObserver
//
//	Returns true if the given pnum is currently an observer in the game, else returns false.
//	If an invalid pnum is given, then it returns false
bool DMFCBase::IsPlayerObserver(int pnum)
{
	if(!CheckPlayerNum(pnum))
		return false;
	
	if(Objects[Players[pnum].objnum].type==OBJ_OBSERVER)
		return true;

	return false;
}

//	DMFCBase::EnableOnScreenMenuBackground
//
//	Enables/disables the onscreen menu background 
void DMFCBase::EnableOnScreenMenuBackground(bool enable)
{
	if(enable) ENABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_DISPMENUBACKGR);
	else DISABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_DISPMENUBACKGR);
}

//	DMFCBase::DisplayNetGameInfo
//
//	Displays information about the Netgame that is currently being played
//	on to the screen.
void DMFCBase::DisplayNetGameInfo(int background_bmp,bool dedicated_server)
{
	//char server_config_name[PAGENAME_LEN];
	//char connection_name[PAGENAME_LEN];
	DLLgrtext_SetFont(Game_fonts[HUD_FONT_INDEX]);
	
	int height = DLLgrfont_GetHeight(Game_fonts[HUD_FONT_INDEX])+1;
	//int height = DLLRenderHUDGetTextHeight("X")+1;
	ddgr_color color = GR_RGB(180,255,180);
	int x,y;
	y = 0;
	x = 15;
	int w,maxx = 0;
	char fullbuffer[1024];

	int topy = 0;
	bool peertopeer = (bool)((Netgame->flags&NF_PEER_PEER)!=0);
	bool permissable = (bool)((Netgame->flags&NF_PERMISSABLE)!=0);

	if( (m_iProtectedFlags&DMFC_PRF_DISPMENUBACKGR) && background_bmp>BAD_BITMAP_HANDLE){
		//draw the background bitmap
		DLLrend_SetAlphaValue(255*0.85f);
		DLLrend_SetZBufferState (0);
		DLLrend_SetTextureType (TT_LINEAR);
		DLLrend_SetLighting (LS_NONE);
		DLLrend_SetAlphaType (AT_CONSTANT_TEXTURE);
		DLLrend_DrawScaledBitmap(0,0,*Game_window_w,8*height,background_bmp,0,0,1,1,1.0,-1,NULL);
		DLLrend_SetZBufferState(1);
	}

	//Print out Title
	//DLLRenderHUDText(color,255,0,x,y,DTXT_NGI_HEADING); y+=height;
	DLLgrtext_SetColor(color);
	DLLgrtext_SetAlpha(255);
	sprintf(fullbuffer,DTXT_NGI_HEADING);
	DLLgrtext_Printf(x,y,fullbuffer);
	w = DLLgrtext_GetTextLineWidth(fullbuffer);
	maxx = (w>maxx)?w:maxx;
	y+=height;

	if(dedicated_server)
		DPrintf(DTXT_NGI_HEADING);
	color = GR_RGB(0,255,0);
	topy = y;	//save this position

	//Print out Game name
	//DLLRenderHUDText(color,255,0,x,y,DTXT_NGI_GAMENAME,Netgame->name); y+=height;
	DLLgrtext_SetColor(color);
	DLLgrtext_SetAlpha(255);
	sprintf(fullbuffer,DTXT_NGI_GAMENAME,Netgame->name);
	DLLgrtext_Printf(x,y,fullbuffer);
	w = DLLgrtext_GetTextLineWidth(fullbuffer);
	maxx = (w>maxx)?w:maxx;
	y+=height;

	if(dedicated_server)
		DPrintf(DTXT_NGI_GAMENAME,Netgame->name);

	//Print out Mission name
	//DLLRenderHUDText(color,255,0,x,y,DTXT_NGI_MISSIONNAME,Netgame->mission); y+=height;
	DLLgrtext_SetColor(color);
	DLLgrtext_SetAlpha(255);
	sprintf(fullbuffer,DTXT_NGI_MISSIONNAME,Netgame->mission);
	DLLgrtext_Printf(x,y,fullbuffer);
	w = DLLgrtext_GetTextLineWidth(fullbuffer);
	maxx = (w>maxx)?w:maxx;
	y+=height;

	if(dedicated_server)
		DPrintf(DTXT_NGI_MISSIONNAME,Netgame->mission);

	//Print out Script name
	//DLLRenderHUDText(color,255,0,x,y,DTXT_NGI_SCRIPTNAME,Netgame->scriptname); y+=height;
	DLLgrtext_SetColor(color);
	DLLgrtext_SetAlpha(255);
	sprintf(fullbuffer,DTXT_NGI_SCRIPTNAME,Netgame->scriptname);
	DLLgrtext_Printf(x,y,fullbuffer);
	w = DLLgrtext_GetTextLineWidth(fullbuffer);
	maxx = (w>maxx)?w:maxx;
	y+=height;

	if(dedicated_server)
		DPrintf(DTXT_NGI_SCRIPTNAME,Netgame->scriptname);

	//Print out PPS
	if(GetLocalRole()==LR_SERVER){
		//DLLRenderHUDText(color,255,0,x,y,DTXT_NGI_PPS,Netgame->packets_per_second); y+=height;
		DLLgrtext_SetColor(color);
		DLLgrtext_SetAlpha(255);
		sprintf(fullbuffer,DTXT_NGI_PPS,Netgame->packets_per_second);
		DLLgrtext_Printf(x,y,fullbuffer);
		w = DLLgrtext_GetTextLineWidth(fullbuffer);
		maxx = (w>maxx)?w:maxx;
		y+=height;

		if(dedicated_server)
			DPrintf(DTXT_NGI_PPS,Netgame->packets_per_second);
	}else{
		//DLLRenderHUDText(color,255,0,x,y,DTXT_NGI_PPSSERVER,Netgame->packets_per_second,NetPlayers[GetPlayerNum()].pps); y+=height;
		DLLgrtext_SetColor(color);
		DLLgrtext_SetAlpha(255);
		sprintf(fullbuffer,DTXT_NGI_PPSSERVER,Netgame->packets_per_second,NetPlayers[GetPlayerNum()].pps);
		DLLgrtext_Printf(x,y,fullbuffer);
		w = DLLgrtext_GetTextLineWidth(fullbuffer);
		maxx = (w>maxx)?w:maxx;
		y+=height;

		if(dedicated_server)
			DPrintf(DTXT_NGI_PPSSERVER,Netgame->packets_per_second,NetPlayers[GetPlayerNum()].pps);
	}

	//Print out Max Players
	//DLLRenderHUDText(color,255,0,x,y,DTXT_NGI_MAXPLAYERS,Netgame->max_players); y+=height;
	DLLgrtext_SetColor(color);
	DLLgrtext_SetAlpha(255);
	sprintf(fullbuffer,DTXT_NGI_MAXPLAYERS,Netgame->max_players);
	DLLgrtext_Printf(x,y,fullbuffer);
	w = DLLgrtext_GetTextLineWidth(fullbuffer);
	maxx = (w>maxx)?w:maxx;
	y+=height;

	if(dedicated_server)
		DPrintf(DTXT_NGI_MAXPLAYERS,Netgame->max_players);

	//Accurate Collision Detection
	bool use_acc_weap = (bool) ( (Netgame->flags&NF_USE_ACC_WEAP)!=0);
	//DLLRenderHUDText(color,255,0,x,y,DTXT_NGI_ACCURATECOLL,(use_acc_weap)?DTXT_MNUON:DTXT_MNUOFF); y+=height;
	DLLgrtext_SetColor(color);
	DLLgrtext_SetAlpha(255);
	sprintf(fullbuffer,DTXT_NGI_ACCURATECOLL,(use_acc_weap)?DTXT_MNUON:DTXT_MNUOFF);
	DLLgrtext_Printf(x,y,fullbuffer);
	w = DLLgrtext_GetTextLineWidth(fullbuffer);
	maxx = (w>maxx)?w:maxx;
	y+=height;

	if(dedicated_server)
		DPrintf(DTXT_NGI_ACCURATECOLL,(use_acc_weap)?DTXT_MNUON:DTXT_MNUOFF);

	//move to second column
	y = topy;
	x += (maxx+10);

	//Rotatitional Velocity
	bool rot_vel_sent = (bool) ( (Netgame->flags&NF_SENDROTVEL)!=0);
	//DLLRenderHUDText(color,255,0,x,y,DTXT_NGI_SENDROTATIONAL,(rot_vel_sent)?DTXT_MNUON:DTXT_MNUOFF); y+=height;
	DLLgrtext_SetColor(color);
	DLLgrtext_SetAlpha(255);
	sprintf(fullbuffer,DTXT_NGI_SENDROTATIONAL,(rot_vel_sent)?DTXT_MNUON:DTXT_MNUOFF);
	DLLgrtext_Printf(x,y,fullbuffer);
	w = DLLgrtext_GetTextLineWidth(fullbuffer);
	y+=height;

	if(dedicated_server)
		DPrintf(DTXT_NGI_SENDROTATIONAL,(rot_vel_sent)?DTXT_MNUON:DTXT_MNUOFF);

	//Print out time limit
	if(Netgame->flags&NF_TIMER){
		DLLgrtext_SetColor(color);
		DLLgrtext_SetAlpha(255);
		DLLgrtext_Printf(x,y,DTXT_NGI_TIMELIMIT,Netgame->timelimit,(Netgame->timelimit==1)?DTXT_MINUTE:DTXT_MINUTES);
		y+=height;

		if(dedicated_server)
			DPrintf(DTXT_NGI_TIMELIMIT,Netgame->timelimit,(Netgame->timelimit==1)?DTXT_MINUTE:DTXT_MINUTES);

		float t_left = 0;
		t_left = (Netgame->timelimit*60.0f) - RealGametime;
	
		//Print out time left		
		DLLgrtext_SetColor(color);
		DLLgrtext_SetAlpha(255);
		DLLgrtext_Printf(x,y,DTXT_NGI_TIMELEFT,GetTimeString(t_left));
		y+=height;

		if(dedicated_server)
			DPrintf(DTXT_NGI_TIMELEFT,GetTimeString(t_left)); 
	}else{		
		DLLgrtext_SetColor(color);
		DLLgrtext_SetAlpha(255);
		DLLgrtext_Printf(x,y,DTXT_NGI_NOTIMELIMIT);
		y+=height;

		if(dedicated_server)
			DPrintf(DTXT_NGI_NOTIMELIMIT);
	}

	//Print out Goal
	if(Netgame->killgoal>0){
		//DLLRenderHUDText(color,255,0,x,y,DTXT_NGI_GOALLIMIT,Netgame->killgoal); y+=height;
		DLLgrtext_SetColor(color);
		DLLgrtext_SetAlpha(255);
		DLLgrtext_Printf(x,y,DTXT_NGI_GOALLIMIT,Netgame->killgoal);
		y+=height;

		if(dedicated_server)
			DPrintf(DTXT_NGI_GOALLIMIT,Netgame->killgoal);
	}else{
		//DLLRenderHUDText(color,255,0,x,y,DTXT_NGI_NOGOAL); y+=height;
		DLLgrtext_SetColor(color);
		DLLgrtext_SetAlpha(255);
		DLLgrtext_Printf(x,y,DTXT_NGI_NOGOAL);
		y+=height;

		if(dedicated_server)
			DPrintf(DTXT_NGI_NOGOAL);
	}

	//Print out Respawn time
	//DLLRenderHUDText(color,255,0,x,y,DTXT_NGI_RESPAWNTIME,GetTimeString(Netgame->respawn_time)); y+=height;
	DLLgrtext_SetColor(color);
	DLLgrtext_SetAlpha(255);
	DLLgrtext_Printf(x,y,DTXT_NGI_RESPAWNTIME,GetTimeString(Netgame->respawn_time));
	y+=height;

	if(dedicated_server)
		DPrintf(DTXT_NGI_RESPAWNTIME,GetTimeString(Netgame->respawn_time));

	//Print out Client/Server Packet Loss
	if( (GetPlayerNum()==0) || peertopeer ){
		//packet loss info not available
		//DLLRenderHUDText(color,255,0,x,y,DTXT_NGI_PACKETLOSSNA); y+= height;
		DLLgrtext_SetColor(color);
		DLLgrtext_SetAlpha(255);
		DLLgrtext_Printf(x,y,DTXT_NGI_PACKETLOSSNA);
		y+=height;

		if(dedicated_server)
			DPrintf(DTXT_NGI_PACKETLOSSNA);
	}else{
		float packetloss = NetPlayers[GetPlayerNum()].percent_loss;

		//DLLRenderHUDText(color,255,0,x,y,DTXT_NGI_PACKETLOSS,packetloss); y+=height;
		DLLgrtext_SetColor(color);
		DLLgrtext_SetAlpha(255);
		DLLgrtext_Printf(x,y,DTXT_NGI_PACKETLOSS,packetloss);
		y+=height;

		if(dedicated_server)
			DPrintf(DTXT_NGI_PACKETLOSS,packetloss);
	}

	//Print out Architecture
	//DLLRenderHUDText(color,255,0,x,y,DTXT_NGI_NETWORKMODEL,(peertopeer)?DTXT_PEERTOPEER:DTXT_CLIENTSERVER); y+=height;
	DLLgrtext_SetColor(color);
	DLLgrtext_SetAlpha(255);
	DLLgrtext_Printf(x,y,DTXT_NGI_NETWORKMODEL,(peertopeer)?DTXT_PEERTOPEER:((permissable)?DTXT_PERMISSABLE:DTXT_CLIENTSERVER));
	y+=height;

	if(dedicated_server)
		DPrintf(DTXT_NGI_NETWORKMODEL,(peertopeer)?DTXT_PEERTOPEER:((permissable)?DTXT_PERMISSABLE:DTXT_CLIENTSERVER));
}

/*
 ****************************************************************
 *                Registry Database Functions					*
 ****************************************************************
 */

// DatabaseRegister
//		Registers your multiplayer game with the database.  This must be
//	called before any other database function, or they will fail until this
//	is called.
void DMFCBase::DatabaseRegister(char *name)
{
	if(name){
		strncpy(DatabaseRegisteredName,name,MAX_DBNAME_SIZE-1);
		DatabaseRegisteredName[MAX_DBNAME_SIZE-1] = '\0';
	}
}

// DatabaseRead
//		Reads a string from the database
bool DMFCBase::DatabaseRead(const char *label,char *entry,int *entrylen)
{
	if(DatabaseRegisteredName[0]=='\0')
		return false;
	if(!label || !entry || !entrylen)
		return false;

	int reg_len = strlen(DatabaseRegisteredName);
	int copy = MAX_DBLABEL_SIZE - 1 - reg_len;
	char buffer[MAX_DBLABEL_SIZE];

	strcpy(buffer,DatabaseRegisteredName);
	strncpy(&buffer[reg_len],label,copy);
	buffer[MAX_DBLABEL_SIZE-1] = '\0';
	
	return DatabaseRead1(buffer,entry,entrylen);
}

// DatabaseRead
//		Reads wordsize bytes from the database
bool DMFCBase::DatabaseRead(const char *label, void *entry, int wordsize)
{
	if(DatabaseRegisteredName[0]=='\0')
		return false;
	if(!label || !entry)
		return false;

	int reg_len = strlen(DatabaseRegisteredName);
	int copy = MAX_DBLABEL_SIZE - 1 - reg_len;
	char buffer[MAX_DBLABEL_SIZE];

	strcpy(buffer,DatabaseRegisteredName);
	strncpy(&buffer[reg_len],label,copy);
	buffer[MAX_DBLABEL_SIZE-1] = '\0';

	return DatabaseRead2(buffer,entry,wordsize);
}

// DatabaseRead
//		Reads a bool from the database
bool DMFCBase::DatabaseRead(const char *label, bool *entry)
{
	if(DatabaseRegisteredName[0]=='\0')
		return false;
	if(!label || !entry)
		return false;

	int reg_len = strlen(DatabaseRegisteredName);
	int copy = MAX_DBLABEL_SIZE - 1 - reg_len;
	char buffer[MAX_DBLABEL_SIZE];

	strcpy(buffer,DatabaseRegisteredName);
	strncpy(&buffer[reg_len],label,copy);
	buffer[MAX_DBLABEL_SIZE-1] = '\0';

	return DatabaseRead3(buffer,entry);
}

// DatabaseWrite
//		Writes/Updates a string to the database
bool DMFCBase::DatabaseWrite(const char *label, const char *entry, int entrylen)
{
	if(DatabaseRegisteredName[0]=='\0')
		return false;
	if(!label || !entry)
		return false;

	int reg_len = strlen(DatabaseRegisteredName);
	int copy = MAX_DBLABEL_SIZE - 1 - reg_len;
	char buffer[MAX_DBLABEL_SIZE];

	strcpy(buffer,DatabaseRegisteredName);
	strncpy(&buffer[reg_len],label,copy);
	buffer[MAX_DBLABEL_SIZE-1] = '\0';

	return DatabaseWrite(buffer,entry,entrylen);
}

// DatabaseWrite
//		Writes/Updates a value to the database
bool DMFCBase::DatabaseWrite(const char *label, int entry)
{
	if(DatabaseRegisteredName[0]=='\0')
		return false;
	if(!label)
		return false;

	int reg_len = strlen(DatabaseRegisteredName);
	int copy = MAX_DBLABEL_SIZE - 1 - reg_len;
	char buffer[MAX_DBLABEL_SIZE];

	strcpy(buffer,DatabaseRegisteredName);
	strncpy(&buffer[reg_len],label,copy);
	buffer[MAX_DBLABEL_SIZE-1] = '\0';

	return DatabaseWrite(buffer,entry);
}

// CompareNetworkAddress
//
//	Compare's two network addresses, returns true if they are the same, false if not.  
//	use_port: if this is true, than it will consider the port part of the network address
bool DMFCBase::CompareNetworkAddress(network_address *one,network_address *two,bool use_port)
{
	if(use_port) {
		//compare the whole darned thing
		if( !memcmp(one,two,sizeof(network_address)) )
			return true;
		else
			return false;
	}else{
		//compare all but the port
		if( !memcmp(one->address,two->address,6) && 
			!memcmp(one->net_id,two->net_id,4) &&
			!memcmp(&one->connection_type,&two->connection_type,sizeof(network_protocol) )){
			//looks the same to me
			return true;
		}else{
			//something didn't match
			return false;
		}
	}

	return false;
}

// IsMasterTrackerGame
//
//	returns true if this game is being played on the master tracker
bool DMFCBase::IsMasterTrackerGame(void)
{
	return (bool)((*m_bTrackerGame)!=0);
}

//	ConvertLocalToServerObjnum
//
//	Given an objnum, it will convert the number from your local objnum to the server's objnum
//	It will return -1 on error
int DMFCBase::ConvertLocalToServerObjnum(int objnum)
{
	if(GetLocalRole()==LR_SERVER)
		return (objnum<0||objnum>=MAX_OBJECTS)?-1:objnum;

	if( objnum < 0 || objnum >= MAX_OBJECTS )
		return -1;

	int s = Local_object_list[objnum];
		
	return (s==65535)?-1:s;
}

//	ConvertServerToLocalObjnum
//
//	Given an objnum from the server, this function will convert the objnum to your local objnum
//  It will return -1 on error
int DMFCBase::ConvertServerToLocalObjnum(int objnum)
{
	if(GetLocalRole()==LR_SERVER)
		return (objnum<0||objnum>=MAX_OBJECTS)?-1:objnum;

	if( objnum < 0 || objnum >= MAX_OBJECTS )
		return -1;

	int s = Server_object_list[objnum];	
	
	return (s==65535)?-1:s;
}

//	DMFCBase::AnnounceTeamChangeDeny
//
//	Tells a player that team change request was denied
void DMFCBase::AnnounceTeamChangeDeny(int pnum)
{
	if(pnum==*Player_num){
		DLLAddHUDMessage(DTXT_CHANGETEAMDENIED);
	}else{
		if(GetLocalRole()==LR_SERVER){
			//tell the client the change has been denied
			SendControlMessageToPlayer(pnum,CM_TEAMCHANGEDENIED);
		}
	}
}

//DMFCBase::SetDeathMessageFilter
//
// Sets the death message filter
void DMFCBase::SetDeathMessageFilter(int level)
{
	if(level<0)
		level = 0;
	if(level>2)
		level = 2;

	if(level==m_iDeathMessageFilter)
		return;

	m_iDeathMessageFilter = level;
	switch(level){
	case DM_FILTER_FULL:
		DLLAddHUDMessage(DTXT_KILLMSGFULL);
		break;
	case DM_FILTER_SIMPLE:
		DLLAddHUDMessage(DTXT_KILLMSGSIMPLE);
		break;
	case DM_FILTER_NONE:
		DLLAddHUDMessage(DTXT_KILLMSGNONE);
		break;
	};
}

//	DMFCBase::MakeClientsWait
//
//	If passed true than all joining clients are forced to wait until they are given the signal to
//	join, either through SignalClientStart() or by passing false to MakeClientsWait
void DMFCBase::MakeClientsWait(bool wait)
{
	m_bMakeClientsWait = wait;

	if(!wait){
		//tell all the clients it's time to start
		for(int i=0;i<DLLMAX_PLAYERS;i++){
			if(CheckPlayerNum(i)){
				SignalClientStart(i);
			}
		}
	}else{
		//tell all the clients to hold up
		WaitForServer(true);
	}
}

//	DMFCBase::SignalClientStart
//
//	Given a player num, it tells the client that they can start moving and playing
void DMFCBase::SignalClientStart(int pnum)
{
	if(GetLocalRole()!=LR_SERVER){
		return;
	}

	SendControlMessageToPlayer(pnum,CM_SIGNALSTART);
	if(pnum==GetPlayerNum())
		WaitForServer(false);
}

//	DMFCBase::WaitForServer
//
//	Call this to start waiting, until the server tells us to stop (by passing false as a parm)
void DMFCBase::WaitForServer(bool wait)
{
	if(wait){
		Players[GetPlayerNum()].movement_scalar = 0;
		Players[GetPlayerNum()].weapon_recharge_scalar = 99999.9f;
		m_bMakeClientsWait = true;
	}else{
		Players[GetPlayerNum()].movement_scalar = 1.0f;
		Players[GetPlayerNum()].weapon_recharge_scalar = 1.0f;
		m_bMakeClientsWait = false;
	}

	if(GetLocalRole()!=LR_SERVER)
		return;

	if(!wait)
		return;

	for(int i=0;i<DLLMAX_PLAYERS;i++){
		if(CheckPlayerNum(i)){
			SendControlMessageToPlayer(i,CM_SIGNALWAIT);
		}
	}
}

//	DMFCBase::WaitingForServerLoop
//
//	Called per frame while waiting for the server to signal go
void DMFCBase::WaitingForServerLoop(void)
{
	static float time_ac = 0;
	static bool show = true;

	time_ac += *Frametime;

	if(time_ac>0.5){
		time_ac = 0;
		show = !show;
	}

	if(show){
		DLLgrtext_SetFont(Game_fonts[MENU_FONT_INDEX]);
		
		int height = DLLgrfont_GetHeight(Game_fonts[MENU_FONT_INDEX]);

		DLLRenderHUDTextFlags(HUDTEXT_CENTERED,GR_WHITE,255,0,0,240 - (height/2),DTXT_WAITFORSERVER);
		DLLgrtext_Flush();
	}

	//Make sure we stay waiting...
	Players[GetPlayerNum()].movement_scalar = 0;
	Players[GetPlayerNum()].weapon_recharge_scalar = 99999.9f;
}

//DMFCBase::IAmDedicatedServer
//
//	Returns true if we are a dedicated server
bool DMFCBase::IAmDedicatedServer(void)
{
	return *Dedicated_server;
}

//DMFCBase::IsPlayerDedicatedServer
//
//	Returns true if the passed in pnum/player_record is a dedicated server
bool DMFCBase::IsPlayerDedicatedServer(int pnum)
{
	//since we are looking for a server, the pnum must be 0
	if(pnum!=0)
		return false;

	if(Players[pnum].team==-1)
		return true;

	return false;	
}
bool DMFCBase::IsPlayerDedicatedServer(player_record *pr)
{
	if(pr->state==STATE_EMPTY)
		return false;

	if(pr->state==STATE_DISCONNECTED)
		return (pr->team==-1)?true:false;

	return IsPlayerDedicatedServer(pr->pnum);
}


//DMFCBase::GetPlayerTeam
//
//	Returns the team of the player...call this instead of accessing .team directly
int DMFCBase::GetPlayerTeam(int pnum)
{
	return (Players[pnum].team==-1)?0:Players[pnum].team;
}

//DMFCBase::ConvertHUDCoord
//
//	Given an x,y based on a virtual 640x480 screen, this will convert it to the x,y that should be
//	used based on the current screen size
void DMFCBase::ConvertHUDCoord(int x,int y,int *rx,int *ry)
{
	*rx = ((float)(x)*(*Hud_aspect_x));
	*ry = ((float)(y)*(*Hud_aspect_y));
}

//DMFCBase::GetPlayerLogoBmp
//
//	Given a player_num, it will return a bitmap handle to that player's ship logo, or 
//	-1 if they don't have a logo for their ship.
//	if is_vclip comes back as true, than it is not a bitmap handle, but a handle
//	to a vclip (animated bitmap).  It is an index into the DLLGameVClips[].
int DMFCBase::GetPlayerLogoBmp(int player_num,bool *is_vclip)
{
	if(!CheckPlayerNum(player_num))
		return -1; //invalid pnum

	int texture_handle = Players[player_num].custom_texture_handle;

	if(GameTextures[texture_handle].bm_handle<=BAD_BITMAP_HANDLE)
		return -1;

	*is_vclip = (GameTextures[texture_handle].flags&TF_ANIMATED)?true:false;

	return GameTextures[texture_handle].bm_handle;
}

//DMFCBase::EnableShipLogos
//
//	Disables or enables logo displaying for the client
void DMFCBase::EnableShipLogos(bool enable)
{
	if(enable){
		ENABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_SHIPLOGOSENABLED);
	}else{
		DISABLE_FLAGS(m_iProtectedFlags,DMFC_PRF_SHIPLOGOSENABLED);
	}
	
	DLLMultiSetLogoState(enable);
}

//DMFCBase::AreLogosEnabled
//
//	returns true if ship logos are enabled
bool DMFCBase::AreLogosEnabled(void)
{
	return (bool)((m_iProtectedFlags&DMFC_PRF_SHIPLOGOSENABLED)!=0);
}

//DMFCBase::EnableAudioTaunts
//
//	Disables or enables audio taunts
void DMFCBase::EnableAudioTaunts(bool enable)
{
	DLLtaunt_Enable(enable);
}

//DMFCBase::AreTauntsEnabled(void)
//
//	returns true if audio taunts are enabled
bool DMFCBase::AreTauntsEnabled(void)
{
	return DLLtaunt_AreEnabled();
}

//DMFCBase::RespawnPlayer
//
//	Takes the given player and respawns his at a random start point (in a team game, team respawn
//	points are taken into consideration, so make sure the player is on the correct team before you
//	respawn him.  spew_everything, if false, will override spew_energy_and_shields
void DMFCBase::RespawnPlayer(int pnum,bool spew_energy_and_shields,bool spew_everything)
{
	if(!CheckPlayerNum(pnum))
		return;

	object *obj = &Objects[Players[pnum].objnum];

	if(spew_everything)
		DLLPlayerSpewInventory(obj,spew_energy_and_shields,spew_everything);

	DLLPlayerStopSounds(pnum);

	DLLInitPlayerNewShip(pnum,(spew_everything)?INVRESET_ALL:INVRESET_DEATHSPEW);

	if(pnum!=GetPlayerNum())
		return;

	int	slot = DLLPlayerGetRandomStartPosition(pnum);
	
	DLLObjSetPos(obj,&Players[slot].start_pos,Players[slot].start_roomnum,&Players[slot].start_orient,NULL);
}

//If you are going to create submenus you MUST use this function. along with:
//void SetState(int state);
//bool SetStateItemList(int count, ... ); for MIT_STATE items
MenuItem *DMFCBase::CreateMenuItem(char *title,char type,ubyte flags,void (*fp)(int), ... )
{
	MenuItem *p;

	if(type!=MIT_CUSTOM && type!=MIT_STATE){
		p = new MenuItem(title,type,flags,fp);
		return p;
	}else{
		if(type==MIT_CUSTOM){
			tCustomMenu cm;

			va_list marker;
			va_start(marker,fp);
			memcpy(&cm,va_arg(marker,tCustomMenu *),sizeof(tCustomMenu));
			va_end(marker);

			p = new MenuItem(title,type,flags,fp,&cm);
			return p;
		}else{
			p = new MenuItem(title,type,flags,fp,0,0);	//set initially to 0 state items
			return p;
		}
	}

	return NULL;
}

void ParseHostsFile(char *filename,tHostsNode **root)
{
	CFILE *file;
	DLLOpenCFILE(&file,filename,"rt");

	tHostsNode *curr = *root;

	if(!file)
		return;

	char buffer[256];
	char save_buffer[256];

	char s_ip[16],s_mask[16];
	unsigned long ip_address;
	unsigned long mask;
	char *ptr;

	while(!DLLcfeof(file)){
		DLLcf_ReadString(buffer,256,file);

		//handle the buffer of data
		ptr = buffer;

		//parse white space
		while(*ptr && (*ptr==' '||*ptr=='\t') ) ptr++;
		strcpy(save_buffer,ptr);
	
		if(*ptr){
			//attempt to parse the dotted ip address (only 0-9,'.' and '*' allowed)
			char *start = ptr;
			
			//check to make sure its in a valid form

			int dot_count = 0;
			while(*ptr){
				if(*ptr=='.')
					dot_count++;
				if((*ptr<'0' || *ptr>'9') && *ptr!='*' && *ptr!='.' )
					goto error_parse;	//invalid character
				ptr++;
			}
			if(dot_count!=3)
				goto error_parse;	//not enough dots
			ptr = start;

			//check the string length, no matter what it shouldn't be more than 15
			if(strlen(ptr)>15)
				goto error_parse;

			//break up the string into seperate strings
			while(*ptr){
				if(*ptr=='.') *ptr = '\0';
				ptr++;
			}
			ptr = start;

			//check to make sure the string length of each string isn't more than 3
			int length,count;

			for(count=0;count<4;count++){
				length = strlen(ptr);
				if(length>3 || length==0)
					goto error_parse;
			
				if(length!=1){
					//make sure it's a pure number
					char *save = ptr;
					while( *ptr ){
						if( !isdigit(*ptr))
							goto error_parse;
						ptr++;
					}					
					ptr = save;
					int value =atoi(ptr);
					if(value<0 || value>255)
						goto error_parse;
				}
			
				ptr += length+1;
			}
			ptr = start;

			//Its a valid IP address mask! yeah...*whew* that was fun
	
			int value;
			char temp_str[3];
			s_ip[0] = s_mask[0] = '\0';

			for(count=0;count<4;count++){
				length = strlen(ptr);

				//Mask
				if( *ptr=='*'){
					value = 0;
				}else{
					value = 255;
				}
				sprintf(temp_str,"%d",value);
				strcat(s_mask,temp_str);
				if(count!=3)
					strcat(s_mask,".");

				//IP Address
				if( *ptr=='*'){
					value = 0;
				}else{
					value = atoi(ptr);
				}
				sprintf(temp_str,"%d",value);
				strcat(s_ip,temp_str);
				if(count!=3)
					strcat(s_ip,".");

				ptr += length +1;
			}
			
			//we now have a valid mask (s_mask) and a valid ip (s_ip)
			mprintf((0,"IP: %s   Mask: %s\n",s_ip,s_mask));
			ip_address = DLLnw_GetHostAddressFromNumbers(s_ip);
			mask = DLLnw_GetHostAddressFromNumbers(s_mask);

			//add the node
			if(!curr){
				curr = *root = (tHostsNode *)malloc(sizeof(tHostsNode));
			}else{
				curr->next = (tHostsNode *)malloc(sizeof(tHostsNode));
				curr = curr->next;
			}
			if(curr){
				curr->next = NULL;
				curr->ip = ip_address;
				curr->mask = mask;
			}			

			goto noerror_parse;
		}

error_parse:
		mprintf((0,"Error parsing IP Address Mask: %s\n",save_buffer));

noerror_parse:;

	}

	DLLcfclose(file);

}

// DMFCBase::ReadInHostsAllowDeny
//	
//	Reads in the hosts.allow and hosts.deny files (if available)
void DMFCBase::ReadInHostsAllowDeny(void)
{
	char allow_fn[_MAX_PATH],deny_fn[_MAX_PATH];
	bool allow_exist,deny_exist;

	//build the path info here
	DLLddio_MakePath(allow_fn,LocalD3Dir,"netgames","hosts.allow",NULL);
	DLLddio_MakePath(deny_fn,LocalD3Dir,"netgames","hosts.deny",NULL);

	allow_exist = (bool)(DLLcfexist(allow_fn)!=0);
	deny_exist = (bool)(DLLcfexist(deny_fn)!=0);

	m_DenyList = NULL;
	m_AllowList = NULL;

	//parse away
	if(deny_exist){
		mprintf((0,"Parsing hosts.deny\n"));
		ParseHostsFile(deny_fn,&m_DenyList);
	}
	if(allow_exist){
		mprintf((0,"Parsing hosts.allow\n"));
		ParseHostsFile(allow_fn,&m_AllowList);
	}
}

// DMFCBase::FreeHostsLists
//	
//	Frees all the memory allocated for the host allow/deny lists
void DMFCBase::FreeHostsLists(void)
{
	tHostsNode *curr = NULL,*next;

	curr = m_DenyList;
	while(curr){
		next = curr->next;
		free(curr);
		curr = next;
	}

	curr = m_AllowList;
	while(curr){
		next = curr->next;
		free(curr);
		curr = next;
	}
}

// DMFCBase::RehashAllowDeny
//
//	Flushes and reloads the hosts.allow/.deny lists
void DMFCBase::RehashAllowDeny(void)
{
	FreeHostsLists();
	ReadInHostsAllowDeny();
}

// DMFCBase::IsPlayerAlive
//
//	Returns true is the given pnum is a player, flying around the level (not dying, dead or observing)
bool DMFCBase::IsPlayerAlive(int pnum)
{
	if(!CheckPlayerNum(pnum))
		return false;

	if(Objects[Players[pnum].objnum].type!=OBJ_PLAYER)
		return false;

	if(Players[pnum].flags&(PLAYER_FLAGS_DYING|PLAYER_FLAGS_DEAD))
		return false;
	return true;
}


//	DMFCBase::ParseStartupScript
//
//
//	Loads up the startup script and sets variables accordingly
void DMFCBase::ParseStartupScript(void)
{
	CFILE *file;
	char path[_MAX_PATH];
	char buffer[256];
	int size;
	bool ok_to_read;

	int autoexec_arg = -1;

	if( (autoexec_arg=DLLFindArg("-autoexec")) != 0 )
	{
		// a specific autoexec.dmfc file was specified, use that
		strcpy(path,GetGameArg(autoexec_arg+1));
		mprintf((0,"Override AUTOEXEC.DMFC to %s\n",path));
	}else
	{
		// use the default autoexec.dmfc
		DLLddio_MakePath(path,LocalD3Dir,"netgames","autoexec.dmfc",NULL);
	}

	DLLOpenCFILE(&file,path,"rt");
	if(!file)
		return;

	while(!DLLcfeof(file)){
		ok_to_read = true;

		size = DLLcf_ReadString(&buffer[1],254,file);
		buffer[255] = '\0';

		if(size>=254){
			//invalid string, too long..trash the rest of the line
			ok_to_read = false;			
			while(size>=254){
				size = DLLcf_ReadString(buffer,254,file);
			}
		}

		if(!ok_to_read){
			mprintf((0,"AUTOEXEC.DMFC: Line too long\n"));
		}else{
			if(buffer[1]=='$')
			{
				InputCommandHandle(&buffer[1]);
			}else
			{
				buffer[0] = '$';	//insert starting $ sign for input command
				InputCommandHandle(buffer);
			}			
		}		
	}

	DLLcfclose(file);	
}

dllinfo *DMFCBase::GetDLLInfoCallData(void)
{
	return Data;
}

int DMFCBase::GetHighestRoomIndex(void)
{
	return *Highest_room_index;
}

int DMFCBase::GetGameWindowW(void)
{
	return *Game_window_w;
}

int DMFCBase::GetGameWindowH(void)
{
	return *Game_window_h;
}

int DMFCBase::GetGameWindowX(void)
{
	return *Game_window_x;
}

int DMFCBase::GetGameWindowY(void)
{
	return *Game_window_y;
}

int *DMFCBase::GetGameFontTranslateArray(void)
{
	return Game_fonts;
}

int DMFCBase::GetObserverModeBitmap(void)
{
	return hBitmapObserver;
}

float DMFCBase::GetFrametime(void)
{
	return *Frametime;
}

float DMFCBase::GetGametime(void)
{
	return *Gametime;
}

float DMFCBase::GetRealGametime(bool *ispaused)
{
	if(ispaused)
	{
		*ispaused = (m_iProtectedFlags&DMFC_PRF_PAUSETIME)?true:false;
	}

	return RealGametime;
}

float *DMFCBase::GetShieldDeltaArray(void)
{
	return ShieldDelta;
}

float DMFCBase::GetHudAspectX(void)
{
	return *Hud_aspect_x;
}

float DMFCBase::GetHudAspectY(void)
{
	return *Hud_aspect_y;
}

const char *DMFCBase::GetLocalD3Dir(void)
{
	return LocalD3Dir;
}

const tMission *DMFCBase::GetCurrentMission(void)
{
	return Current_mission;
}

room *DMFCBase::GetRooms(void)
{
	return Rooms;
}

object *DMFCBase::GetObjects(void)
{
	return Objects;
}

terrain_segment *DMFCBase::GetTerrainSegs(void)
{
	return Terrain_seg;
}

netgame_info *DMFCBase::GetNetgameInfo(void)
{
	return Netgame;
}

player *DMFCBase::GetPlayers(void)
{
	return Players;
}

netplayer *DMFCBase::GetNetPlayers(void)
{
	return NetPlayers;
}

ship *DMFCBase::GetShips(void)
{
	return Ships;
}

weapon *DMFCBase::GetWeapons(void)
{
	return Weapons;
}

texture *DMFCBase::GetGameTextures(void)
{
	return GameTextures;
}

vclip *DMFCBase::GetGameVClips(void)
{
	return GameVClips;
}

poly_model *DMFCBase::GetGamePolyModels(void)
{
	return Poly_models;
}

ddgr_color *DMFCBase::GetPlayerColors(void)
{
	return Player_colors;
}

game_controls DMFCBase::GetLastGameControls(void)
{
	return Last_game_controls;
}

int *DMFCBase::GetPilotPicBitmapHandles(void)
{
	return PilotPicBmpHandles;
}

void DMFCBase::GetViewerObjectPtr(object **v_obj)
{
	*v_obj = *Viewer_object;
}

void DMFCBase::SetViewerObjectPtr(object *v_obj)
{
	*Viewer_object = v_obj;
}

float DMFCBase::GetRenderZoom(void)
{
	return *Render_zoom;
}

IMenuItem *DMFCBase::GetOnScreenMenu(void)
{
	return &Menu;
}

tOSIRISModuleInit *DMFCBase::GetOsirisModuleData(void)
{
	return API.osiris_functions;
}

vis_effect *DMFCBase::GetVisEffectArray(int **Highviseptr)
{
	if(Highviseptr)
		*Highviseptr = Highest_vis_effect_index;

	return VisEffects;
}

char *DMFCBase::GetGameArg(int arg)
{
	static char EMPTY_ARG[1] = "";

	ASSERT(arg>=0 && arg<MAX_ARGS);
	if(arg<0 || arg>=MAX_ARGS)
		return (char *)EMPTY_ARG;

	char *arguments = (char *)GameArgs;

	int position = arg*MAX_CHARS_PER_ARG;

	return &arguments[position];
}

//	DMFCBase::EnableLossGuage
//
//	Turns on/off the Loss guage on the screen
void DMFCBase::EnableLossGuage(bool enable)
{
	bool peertopeer = (bool)((Netgame->flags&NF_PEER_PEER)!=0);

	if( (GetLocalRole()==LR_SERVER) || peertopeer ){
		return;
	}

	LossGuageEnabled = enable;
}

ddgr_color GetTriLinColor(float val,float low,float med,float hi,ddgr_color lowc,ddgr_color medc,ddgr_color hic)
{
	ddgr_color low_col,hi_col;
	float perc;

	if(val<=low)
	{
		//we're pure lowc
		perc = 1.0f;
		low_col = hi_col = lowc;
	}else if(val<=med)
	{
		//we're between lowc and medc
		low_col = lowc;
		hi_col = medc;

		perc = (val-low)/(med-low);
	}else if(val<=hi)
	{
		//we're between medc and hic
		low_col = medc;
		hi_col = hic;
		perc = (val-med)/(hi-med);
	}else
	{
		//we're pure hic
		perc = 1.0f;
		low_col = hi_col = hic;
	}

	if(perc<0.0f)	perc = 0.0f;
	if(perc>1.0f)	perc = 1.0f;

	ddgr_color color_to_use;
	int r,g,b;
	float amount_low,amount_hi;

	amount_low = 1.0 - perc;
	amount_hi = perc;

	r = GR_COLOR_RED(low_col)*amount_low + GR_COLOR_RED(hi_col)*amount_hi;
	g = GR_COLOR_GREEN(low_col)*amount_low + GR_COLOR_GREEN(hi_col)*amount_hi;
	b = GR_COLOR_BLUE(low_col)*amount_low + GR_COLOR_BLUE(hi_col)*amount_hi;

	color_to_use = GR_RGB(r,g,b);

	return color_to_use;
}

//	DMFCBase::LossGuageFrame
//
//	Processes the Loss guage for the frame
void DMFCBase::LossGuageFrame(void)
{
	if(!LossGuageEnabled)
		return;

	if(GetLocalRole()==LR_SERVER)
		return;

	//Don't show if peer-to-peer
	if (Netgame->flags & NF_PEER_PEER)
		return;


#define LOW_COLOR		GR_RGB(20,255,20)
#define MED_COLOR		GR_RGB(255,255,40)
#define HI_COLOR		GR_RGB(255,20,20)
#define LOW_LOSS		1.0f
#define MED_LOSS		5.0f
#define HI_LOSS			10.0f

	float packetloss = NetPlayers[GetPlayerNum()].percent_loss;

	ddgr_color loss_color = GetTriLinColor(packetloss,LOW_LOSS,MED_LOSS,HI_LOSS,LOW_COLOR,MED_COLOR,HI_COLOR);
	ddgr_color ping_color = GetTriLinColor(NetPlayers[GetPlayerNum()].ping_time*1000.0f,0,200.0f,400.0f,LOW_COLOR,MED_COLOR,HI_COLOR);

	DLLgrtext_SetFont(Game_fonts[HUD_FONT_INDEX]);

	int max_x = 0;
	int y = 2;

	DLLRenderHUDText(loss_color,255,2,0,y,DTXT_NETWORK_LOSS);
	max_x = DLLRenderHUDGetTextLineWidth(DTXT_NETWORK_LOSS);
	
	DLLRenderHUDText(ping_color,255,2,0,y+10,DTXT_NETWORK_PING);
	max_x = max(max_x,DLLRenderHUDGetTextLineWidth(DTXT_NETWORK_PING));

	max_x += 10;
	DLLRenderHUDText(loss_color,255,0,2+max_x,y,"%.2f%%",packetloss);
	DLLRenderHUDText(ping_color,255,0,2+max_x,y+10,"%dms",(int)(NetPlayers[GetPlayerNum()].ping_time*1000.0f));
}

level_info *DMFCBase::GetLevelInfo(void)
{
	return Level_info;
}

// 1 - (num_levels)
void DMFCBase::WarpToLevel(int lev)
{
	if(GetLocalRole()!=LR_SERVER)
		return;

	if(lev<1 || lev>Current_mission->num_levels)
		return;

	*Multi_next_level = lev;
	EndLevel();
}

void DMFCBase::SetPlayerTauntIndicator(int pnum)
{
	if(!CheckPlayerNum(pnum))
		return;

	DisplayTauntIndicator = true;
	TauntIndicatorStartTime = *Gametime;
	TauntIndicatorPlayerNum = pnum;
}

void DMFCBase::DisplayTauntIndicatorFrame(void)
{
	if(!DisplayTauntIndicator)
		return;

	if(!CheckPlayerNum(TauntIndicatorPlayerNum))
	{
		DisplayTauntIndicator = false;
		return;
	}

	float alpha,perc;
	int igt = floor((*Gametime));
	float igth = (*Gametime) - igt;
	if(igth>0.5f)
	{
		igth -= 0.5f;
	}
	
	if(igth>0.25f)
	{
		perc = 1.0f - ((igth-0.25)/0.25f);
	}else
	{
		perc = (igth/0.25f);
	}

	float base_alpha = 0.40f;
	float range = 1.0f - base_alpha;

	alpha = base_alpha + (perc*range);
	if(alpha<base_alpha) alpha = base_alpha;
	if(alpha>1.0f) alpha = 1.0f;

	int bmp = TauntIndicatorBMP;
	if(PilotPicBmpHandles[TauntIndicatorPlayerNum]>BAD_BITMAP_HANDLE)
	{
		bmp = PilotPicBmpHandles[TauntIndicatorPlayerNum];
	}
	
	if(bmp<=BAD_BITMAP_HANDLE)
	{
		DisplayTauntIndicator = false;
		return;
	}

	int bmh = 32;
	int bmw = 32;
	int x,y;

	x = 10;
	y = 200;
	
	DLLrend_SetAlphaType (AT_CONSTANT_TEXTURE);
	DLLrend_SetAlphaValue (alpha*255);
	DLLrend_SetLighting (LS_NONE);
	DLLrend_SetColorModel (CM_MONO);
	DLLrend_SetOverlayType (OT_NONE);
	DLLrend_SetFiltering (0);
	DLLrend_SetWrapType(WT_CLAMP);
	DLLrend_SetZBufferState(0);
	DLLrend_SetTextureType(TT_LINEAR);
		
	DLLrend_DrawScaledBitmap (x,y,x+bmw,y+bmh,bmp,0,0,1,1,1,-1,NULL);
	DLLrend_SetFiltering(1);
	DLLrend_SetZBufferState(1);

	int font_height;
	
	DLLgrtext_SetFont(Game_fonts[HUD_FONT_INDEX]);
	font_height = DLLgrfont_GetHeight(Game_fonts[HUD_FONT_INDEX]);

	x = 15 + bmw;
	y = (y + (bmh/2)) - (font_height/2);

	DLLgrtext_SetColor(GR_GREEN);
	DLLgrtext_SetAlpha(alpha*255.0f);
	DLLgrtext_Printf(x,y,Players[TauntIndicatorPlayerNum].callsign);

	if( (*Gametime - TauntIndicatorStartTime) > 3.0f )
		DisplayTauntIndicator = false;
}

// DMFCBase::SetMaxPlayerHardLimit
//
//	Sets a hard limit to the max number of players allowed in the game
//	changing the number of players can never go above this...defaults
//	to DLLMAX_PLAYERS
void DMFCBase::SetMaxPlayerHardLimit(int max)
{
	if(GetLocalRole()!=LR_SERVER)
		return;
	if(max>DLLMAX_PLAYERS || max<1)
		return;

	Hard_max_players = max;
	if(Netgame->max_players>Hard_max_players)
	{
		Netgame->max_players = max;
		DLLAddHUDMessage(DTXT_MAXPLAYERSSETMSG,max);
		SendNetGameInfoSync();
	}
}

// DMFCBase::MarkPlayersInGame
//
//	Goes through all the player records and marks those that are in the game
//	This is needed because at level end, all clients disconnect, so we lose
//	that information.
void DMFCBase::MarkPlayersInGame(void)
{
	player_record *pr;
	for(int i=0;i<MAX_PLAYER_RECORDS;i++)
	{
		pr = GetPlayerRecord(i);

		if(pr && pr->state==STATE_INGAME)
		{
			players_in_game[i] = pr->pnum;
		}else
		{
			players_in_game[i] = -1;
		}
	}
}

// DMFCBase::ResetPlayersInGame
//
//	Resets the Players-in-game list
void DMFCBase::ResetPlayersInGame(void)
{
	for(int i=0;i<MAX_PLAYER_RECORDS;i++)
	{
		players_in_game[i] = -1;
	}
}

//	DMFCBase::WasPlayerInGameAtLevelEnd
//
//	Given a player record it returns true/false whether
//	the player was in the game at the time the level ended.
int DMFCBase::WasPlayerInGameAtLevelEnd(int prec)
{
	if(prec<0 || prec>=MAX_PLAYER_RECORDS)
		return -1;

	return players_in_game[prec];
}

//DMFCBase::GetConnectingPlayerTeam
//
//	Returns the team of a player just connecting, this is to be set by the
//	game and is DMFC assumes that all players have the correct team on join
int DMFCBase::GetConnectingPlayerTeam(int slot)
{
	if(GetLocalRole()!=LR_SERVER)
		return -2;

	int prec_num;
	if(IsMasterTrackerGame())
		prec_num = PRec_FindPlayer(Players[slot].callsign,NULL,Players[slot].tracker_id);
	else
		prec_num = PRec_FindPlayer(Players[slot].callsign,&NetPlayers[slot].addr,NULL);

	int team = -2;

	if(prec_num!=-1){
		//we have a reconnecting player		

		//we need to reconnect the player to the player records before we get the team
		if(!PRec_ReconnectPlayerToSlot(slot,prec_num,Players,NetPlayers))
		{
			mprintf((0,"Unable to reassign reconnecting player (%s) to Player Record slot #%d\n",Players[slot].callsign,prec_num));
			Int3();
		}else
		{
			team = PRec_GetPlayerTeam(slot);

			//disconnect the player again since he isn't in the game just yet
			PRec_DisconnectPlayer(slot);
		}
	}else{
		//we have a new player		

		if(slot==0 && IAmDedicatedServer()){
			team = -1;
		}else
		{
			//get team assignment
			if(m_iProtectedFlags&DMFC_PRF_AUTOTEAMSELECT){
				team = GetTeamForNewPlayer(slot,m_iNumTeams);
			}else
			{
				team = RED_TEAM;
			}
		}
	}

	mprintf((0,"CONNECTING PLAYER (%s): Team assigned to %d\n",Players[slot].callsign,team));

	return team;
}

//	DMFCBase::SetDedicatedWait
//
//	Sets the level wait time for the dedicated server.  A dedicated server will make all
//	clients wait until this time (in seconds) is up each level.
void DMFCBase::SetDedicatedWait(float time_secs)
{
	if(time_secs<0)
		time_secs = 0;

	DedicatedLevelWait = time_secs;
}

//DMFCBase::EnableTimelimitCountdown
//
//	Enables/Disables the timelimit countdown (if there is a level time limit)
//	Optionally you can specify what time to start the count down. (default 10)
void DMFCBase::EnableTimelimitCountdown(bool enable,int seconds)
{
	m_bDisplayTimelimitCountdown = enable;

	if(enable && seconds>0)
	{
		m_iTimeLimitCountdown = seconds;
	}
}

//DMFCBase::DisplayTimelimitCountdown
//
//	Displays (if needed) the time limit countdown
void DMFCBase::DisplayTimelimitCountdown(void)
{
	if(!m_bDisplayTimelimitCountdown)
		return;

	if(!(Netgame->flags&NF_TIMER))
		return;
	
	float time_left = (Netgame->timelimit*60.0f) - RealGametime;

	if(time_left>m_iTimeLimitCountdown || time_left<0)
		return;

	//display a countdown!
	DLLgrtext_SetFont(Game_fonts[HUD_FONT_INDEX]);
	DLLgrtext_SetColor(GR_WHITE);
	DLLgrtext_SetAlpha(255);

	char buffer[256];
	sprintf(buffer,"Time Left: T - %d",(int)time_left);

	int font_height = DLLgrfont_GetHeight(Game_fonts[HUD_FONT_INDEX]);
	DLLgrtext_CenteredPrintf(0,font_height*10,buffer);	
}

//DMFCBase::TranslateTextMacro
//
//	Given a macro with tokens, this function will replace the tokens
//	and create a new string for display.
//	All tokens begin with $$ (i.e.  $$TEAMBASE)
void DMFCBase::TranslateTextMacro(const char *src,char *destination,int dest_size)
{
	ASSERT(destination!=NULL);
	ASSERT(src!=NULL);
	ASSERT(dest_size>=0);

	if(dest_size<=0)
		return;

	char token[128];
	char token_string[512];

	char *curr_dest = destination;
	int src_idx = 0;
	bool done = false;
	
	//now we need to go through the src and copy to destination
	//while looking for tokens, and making sure we don't go over our size
	while(dest_size>1 && !done)
	{
		switch(src[src_idx])
		{
		case '$':
			{
				//possible token!, check the next character
				if(src[src_idx+1]=='$')
				{
					//TOKEN!!!
					src_idx+=2;	//move to the beginning of the token
					if(src[src_idx]==' ' || src[src_idx]=='\0')
					{
						//ok, maybe it isn't a token
						*curr_dest = src[src_idx-2];
						curr_dest++;
						dest_size--;
						*curr_dest = src[src_idx-1];
						curr_dest++;
						dest_size--;
						*curr_dest = src[src_idx];						
						if(*curr_dest=='\0')
						{
							done = true;
						}else
						{
							src_idx++;
						}
						curr_dest++;
						dest_size--;
					}else
					{
						//process it, it's gotta be a token
						
						//extract the token out
						char *ptr = token;
						while(src[src_idx]!=' ' && src[src_idx]!='\0')
						{
							*ptr = src[src_idx];
							ptr++;
							src_idx++;
						}
						*ptr = '\0';

						//now replace the token...
						*token_string = '\0';
						mprintf((0,"Looking for token for %s\n",token));
						CallOnGetTokenString(token,token_string,512);

						//make sure we don't go too far
						int len = strlen(token_string);
						if(len>(dest_size-1))
							len = dest_size-1;

						ptr = token_string;

						while(len>0)
						{
							*curr_dest = *ptr;
							curr_dest++;
							dest_size--;
							ptr++;
							len--;
						}
					}
				}else
				{
					//just copy the character over
					*curr_dest = src[src_idx];
					src_idx++;
					curr_dest++;
					dest_size--;
				}
			}break;
		case '\0':
			done = true;
			break;
		default:
			{
				//just copy the character over
				*curr_dest = src[src_idx];
				src_idx++;
				curr_dest++;
				dest_size--;
			}break;
		}
	}

	*curr_dest = '\0';	//NULL terminate
}

//DMFCBase::SelectNextCameraView
//
//	This function, given which window (corresponds to left, middle, right), switches the 
//	current view of the small camera windows on the screen
void DMFCBase::SelectNextCameraView(int window)
{
	if(window<0 || window>=NUM_CAMERA_VIEWS)
		return;
	DLLSelectNextCameraView(window);
}

int DMFCBase::GetCameraViewType(int window)
{
	if(window<0 || window>=NUM_CAMERA_VIEWS)
		return -1;
	return Camera_view_mode[window];
}

// Given a generic object (OBJ_POWERUP,OBJ_ROBOT,OBJ_BUILDING or OBJ_CLUTTER) id
// in the range of 0 to MAX_OBJECT_IDS, this returns a pointer to it's information (see objinfo.h)
//	It returns NULL if an invalid id is given (or it's not used)
object_info *DMFCBase::GetObjectInfo(int objinfo_id)
{
	if(objinfo_id<0 || objinfo_id>=MAX_OBJECT_IDS)
		return NULL;

	if(Object_info[objinfo_id].type==OBJ_NONE)
		return NULL;

	return &Object_info[objinfo_id];
}
