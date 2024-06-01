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

* $Logfile: /DescentIII/Main/mtclient/mtgametrack.h $
* $Revision: 1.1.1.1 $
* $Date: 2003-08-26 03:58:40 $
* $Author: kevinb $
*
* Game Tracker client code header
*
* $Log: not supported by cvs2svn $
 * 
 * 14    10/03/01 12:44a Kevin
 * Smaller version of pxo packets
 * 
 * 13    4/29/99 10:13a Kevin
 * reduced time between game updates
 * 
 * 12    4/19/99 8:35p Jeff
 * compile under Linux
 * 
 * 11    4/17/99 3:44p Kevin
 * Demo2 changes & fixes
 * 
 * 10    4/07/99 9:15a Kevin
 * Changed PXO server addresses for release builds
 * 
 * 9     3/25/99 3:26p Kevin
 * Made PXO games be based on your chat channel
 * 
 * 8     2/16/99 12:36a Kevin
 * Fixes for release builds of OEM V3 and KAtmai
 * 
 * 7     2/03/99 6:14p Kevin
 * OEM changes
 * 
 * 6     1/07/99 11:51a Kevin
 * Added support for joining servers on alternate ports and hosting behind
 * a proxy/firewall
 * 
 * 5     12/23/98 6:38p Kevin
 * All UDP data (except gamespy) now uses one (registered) port number
 * 
 * 4     10/09/98 2:56p Kevin
 * Changed IP addresses for PXO demo
 * 
 * 3     10/01/98 11:37a Kevin
 * UI fixes and stuff
 * 
 * 2     6/01/98 10:10a Kevin
 * Added DLL connection interface and auto update DLL
 * 
 * 1     5/21/98 11:08a Kevin
 * 
 * 5     5/14/98 11:07a Kevin
 * Made gameover packet to the tracker reliable
 * 
 * 4     4/30/98 3:49p Kevin
 * Mastertracker pilot stats
 * 
 * 3     4/24/98 3:50p Kevin
 * Added mastertracker game tracking support
 * 
 * 2     4/23/98 4:49p Kevin
 * Added Master tracker Game tracker support
 * 
 * 1     4/23/98 3:28p Kevin
*
* $NoKeywords: $
*/

#include "pstypes.h"

#ifndef _GAMETRACK_HEADER_DEFINED
#define _GAMETRACK_HEADER_DEFINED

#ifdef WIN32
#pragma pack(push,r_udp)
#endif
#pragma pack(1)

//Definitions
#if 0//(!( defined(OEM) || defined(DEMO) || defined(RELEASE) ))
#define GAMETRACKER	"204.243.217.7"		//The scanner machine at Outrage
#define GAMETRACKERNAME "gtracker.pxsoftware.com"
#define GAMEPORT	3445
#else
#define GAMETRACKER	"209.223.64.251"		//The scanner machine at Outrage
#define GAMETRACKERNAME "gt.parallaxonline.com"
#define GAMEPORT	3445
#endif

#define MAX_GAME_BUFFERS	20					//Thats a lot considering 20 games per game_list struct

#define TRACKER_UPDATE_INTERVAL			60.0		//300 seconds
#define TRACKER_RESEND_TIME				2.0			//2 seconds
#define TRACKER_TIMEOUT						20			//20 seconds

#define MAX_NET_RETRIES 30
#define NET_ACK_TIMEOUT 1.5
#define NET_GAME_TIMEOUT 300.0			//time in seconds

#define MAX_GT_GAME_DATA_SIZE	1500

#define MAX_GENERIC_GAME_NAME_LEN	32

#define MAX_GAME_LISTS_PER_PACKET	20

#define MAX_D3_PLAYERS		32
#define MAX_D3_MISSION_NAME_LEN	32	
#define MAX_D3_PLAYER_NAME_LEN	20
#define MAX_D3_MISSION_FILE_LEN	32
#define MAX_D3_MISSION_URL_LEN	100

#define	GNT_SERVER_ACK			0
#define	GNT_CLIENT_ACK			1
#define	GNT_GAMESTARTED		2
#define	GNT_GAMEOVER			3
#define	GNT_GAMEUPDATE			4
#define	GNT_GAMELIST_REQ		5
#define	GNT_GAMELIST_DATA		6
#define	GNT_GAMELIST_FILTER_REQ	7
#define	GNT_GAMEUPDATE_SHORT	8
#define GNT_GAMEUPDATE_TINY     9
#define	GNT_GAMELIST_LITE_DATA		10
#define GNT_GAMELIST_LITE_FILTER_REQ 11



#define	GT_FREESPACE			1
#define	GT_DESCENT3				2
#define	GT_TUBERACER			3
#define	GT_UNUSED				0
#define GT_D3TNG                        42
#define GAME_HEADER_ONLY_SIZE		(sizeof(game_packet_header)-MAX_GT_GAME_DATA_SIZE)

struct game_packet_header {
	uint32_t len;				//Length of entire packet;
	uint8_t game_type;	//1==freespace (GT_FREESPACE), 2==D3, 3==tuberacer, etc.
	SOCKADDR_IN	addr;
	int	type;	//Used to specify what to do ie. Add a new net game (GNT_GAMESTARTED), remove a net game (game over), etc.
	uint32_t	sig;	//Unique identifier for client ACKs (The server always fills this in, the client responds)

	char data[MAX_GT_GAME_DATA_SIZE];
};

struct d3_net_game_data {
	char	game_name[MAX_GENERIC_GAME_NAME_LEN];
	int	difficulty;
	int	type;			//game type;
	int	state;
	int	max_players;
	int	current_num_players;
	char	mission_name[MAX_D3_MISSION_NAME_LEN];
	char	mission_file[MAX_D3_MISSION_FILE_LEN];
	char	mission_url[MAX_D3_MISSION_URL_LEN];
	char	players[MAX_D3_PLAYERS][MAX_D3_PLAYER_NAME_LEN];
	int	player_rank[MAX_D3_PLAYERS];
};

struct d3_net_game_data_short {
	char	game_name[MAX_GENERIC_GAME_NAME_LEN];
	int	type;			//game type;
	int	state;
	int	max_players;
	int	current_num_players;
	char	mission_name[MAX_D3_MISSION_NAME_LEN];
	char lobby[MAX_GENERIC_GAME_NAME_LEN*2];
};

struct d3_net_game_data_tiny {
  int options;//1==force port to arg1
  int arg1;
  char lobby[MAX_GENERIC_GAME_NAME_LEN*2];
};

/*
struct filter_game_list_struct {
	int	rank;
	char	preferred_players[][];
	
};
*/

struct active_games {
	int game_type; //ie. GT_FREESPACE GT_DESCENT3, etc.
	SOCKADDR	addr;
	uint32_t last_update;	//Time we last got an update from this game
	char	data[MAX_GT_GAME_DATA_SIZE]; //memory to hold the game specific data
        active_games *next;
};
/*
struct game_list {
	uint8_t game_type;
	char game_name[MAX_GAME_LISTS_PER_PACKET][MAX_GENERIC_GAME_NAME_LEN];
	uint32_t	game_server[MAX_GAME_LISTS_PER_PACKET];
	uint16_t game_port[MAX_GAME_LISTS_PER_PACKET];
};
*/

struct game_list {
  uint8_t game_type;
  uint32_t game_server[MAX_GAME_LISTS_PER_PACKET*4];
  uint16_t game_port[MAX_GAME_LISTS_PER_PACKET*4];
};

//Function prototypes

int InitGameTrackerClient(int gametype);
void IdleGameTracker();
void UpdateGameData(void *buffer);
game_list * GetGameList();
void RequestGameList();
int SendGameOver();
void StartTrackerGame(void *buffer);
void HandleGamePacket(uint8_t *data,int len, network_address *from);
void AckPacket(int sig);

#ifdef WIN32
#pragma pack(pop,r_udp)
#else
#pragma pack()
#endif
#endif
