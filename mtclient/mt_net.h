/*
* $Logfile: /DescentIII/Main/mtclient/mt_net.h $
* $Revision: 1.1.1.1 $
* $Date: 2003-08-26 03:58:40 $
* $Author: kevinb $
*
* Mastertracker network code
*
* $Log: not supported by cvs2svn $
 * 
 * 3     12/23/98 6:38p Kevin
 * All UDP data (except gamespy) now uses one (registered) port number
 * 
 * 2     6/01/98 10:10a Kevin
 * Added DLL connection interface and auto update DLL
 * 
 * 1     5/21/98 11:21a Kevin
*
* $NoKeywords: $
*/

//Sends a packet to the game tracker
int SendGameTrackerPacker(void *packet);

//Checks for an incoming game tracker packet.
int ReceiveGameTracker(void *packet);

void InitMTSockets();

void CloseMTSockets(void);
int ReceivePilotTracker(void *packet);
int SendPilotTrackerPacket(void *packet);

#define PXO_NETID_GAME_TRACKER	3
#define PXO_NETID_USER_TRACKER	4