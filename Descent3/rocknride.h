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
*/

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

#define RNRP_POSITION 0x00   // Rock 'n' Ride Position Packet
#define RNRP_HIT 0x01        // Rock 'n' Ride Hit Packet
#define RNRP_GAMESTATUS 0x02 // Rock 'n' Ride Game Status Packet
typedef struct {
  ubyte packet_type;
  union {
    struct {
      ubyte x, y;
    } pos;
    ubyte status;
  };
} tRocknride_packet;

// Rock 'n' Ride Game Status codes
#define RNRGSC_PLAYERDIES 0 // The player has died
#define RNRGSC_INMENU 1     // The player is in a UI menu

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
void RNR_UpdateForceFeedbackInfo(float magnitude, vector *direction);

//	RNR_UpdateForceFeedbackInfo
//
//	Updates any Force Feedback effects
void RNR_UpdateForceFeedbackInfo(float magnitude, int *direction);

//	RNR_UpdateGameStatus
//
//	Updates a game status to a Rock 'n' Ride chair
void RNR_UpdateGameStatus(ubyte status);

#endif
