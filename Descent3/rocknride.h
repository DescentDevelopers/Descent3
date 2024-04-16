/*
* $Logfile: /DescentIII/Main/rocknride.h $
* $Revision: 2 $
* $Date: 5/10/99 9:25p $
* $Author: Jeff $
*
* Rock 'n' Ride
*
* $Log: /DescentIII/Main/rocknride.h $
 * 
 * 2     5/10/99 9:25p Jeff
 * first phase of Rock 'n' Ride support added
*
* $NoKeywords: $
*/

#ifndef __ROCK_N_RIDE_H_
#define __ROCK_N_RIDE_H_

#include "pstypes.h"
#include "controls.h"
#include "vecmat.h"

extern bool RocknRide_enabled;

#define RNRP_POSITION	0x00	//Rock 'n' Ride Position Packet
#define RNRP_HIT		0x01	//Rock 'n' Ride Hit Packet
#define RNRP_GAMESTATUS	0x02	//Rock 'n' Ride Game Status Packet
typedef struct
{
	ubyte packet_type;
	union
	{
		struct
		{
			ubyte x,y;
		}pos;
		ubyte status;
	};
}tRocknride_packet;

//Rock 'n' Ride Game Status codes
#define RNRGSC_PLAYERDIES	0	// The player has died
#define RNRGSC_INMENU		1	// The player is in a UI menu

// RNR_Initialize
//
//	Initializes the Rock 'n' Ride device
//	Pass in the COMM port
bool RNR_Initialize(int comm_port);

// RNR_SendPacket
//
//	Sends off a packet to the Rock 'n' Ride device
void RNR_SendPacket(tRocknride_packet *packet);

//	RNR_UpdateControllerInfo
//
//	Updates any Rock 'n' Ride controller data for the frame (if needed)
void RNR_UpdateControllerInfo(game_controls *controls);

//	RNR_UpdateForceFeedbackInfo
//
//	Updates any Force Feedback effects
void RNR_UpdateForceFeedbackInfo(float magnitude,vector *direction);

//	RNR_UpdateForceFeedbackInfo
//
//	Updates any Force Feedback effects
void RNR_UpdateForceFeedbackInfo(float magnitude,int *direction);

//	RNR_UpdateGameStatus
//
//	Updates a game status to a Rock 'n' Ride chair
void RNR_UpdateGameStatus(ubyte status);

#endif

