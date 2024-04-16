/*
* $Logfile: /DescentIII/main/rocknride.cpp $
* $Revision: 5 $
* $Date: 9/23/99 12:02p $
* $Author: Jeff $
*
* Rock 'n' Ride
*
* $Log: /DescentIII/main/rocknride.cpp $
 * 
 * 5     9/23/99 12:02p Jeff
 * include stdlib for atexit
 * 
 * 4     7/28/99 3:44p Kevin
 * Mac!
 * 
 * 3     5/11/99 11:18a Jeff
 * finished rock 'n' ride
 * 
 * 2     5/10/99 9:25p Jeff
 * first phase of Rock 'n' Ride support added
*
* $NoKeywords: $
*/
#include "mono.h"
#include "ddio.h"
#include "debug.h"
#include "game.h"
#include "pserror.h"
#include "rocknride.h"
#include <memory.h>
#include <string.h>
#include <stdlib.h>
bool RocknRide_enabled = false;
float RocknRide_lastcontroller_update = 0.0f;
float RocknRide_lastforce_update = 0.0f;
#define RNR_CONTROLLER_UPDATE 1.0f/20.0f
#define RNR_FORCE_UPDATE 1.0f/60.0f
tSerialPort RocknRide_serial_port;
// RNR_Shutdown
//
//	Shutsdown the Rock 'n' Ride device
void RNR_Shutdown(void)
{
#ifndef MACINTOSH
	if(!RocknRide_enabled)
		return;
	if(RocknRide_serial_port)
		ddio_SerialClosePort(RocknRide_serial_port);
#endif
}
// RNR_Initialize
//
//	Initializes the Rock 'n' Ride device
//	Pass in the COMM port
bool RNR_Initialize(int comm_port)
{
#ifndef MACINTOSH
	if(RocknRide_enabled)
		return true;
	static bool called = false;
	RocknRide_serial_port = ddio_SerialOpenPort(comm_port,9600);
	if(!RocknRide_serial_port)
	{
		mprintf((0,"Couldn't open serial port\n"));
		return false;
	}
	RocknRide_enabled = true;
	//only setup once
	if(!called)
	{
		called = true;
		atexit(RNR_Shutdown);
	}
#endif
	return true;
}
// RNR_SendPacket
//
//	Sends off a packet to the Rock 'n' Ride device
void RNR_SendPacket(tRocknride_packet *packet)
{
#ifndef MACINTOSH
	if(!RocknRide_enabled)
		return;
	ASSERT(packet);
	if(!packet)
		return;
	ubyte data[32];
	int size = -1;
	switch(packet->packet_type)
	{
	case RNRP_POSITION:
		size = 3;
		data[0] = 'P';
		data[1] = packet->pos.x;
		data[2] = packet->pos.y;
		break;
	case RNRP_HIT:
		size = 3;
		data[0] = 'H';
		data[1] = packet->pos.x;
		data[2] = packet->pos.y;
		break;
	case RNRP_GAMESTATUS:
		size = 2;
		data[0] = 'G';
		data[1] = packet->status;
		break;
	}
	if(size!=-1)
	{
		//send off the packet to the device
		//@@char test[32];
		//@@memcpy(test,data,size);
		//@@test[size] = '\0';
		//@@mprintf((0,"DATA: %s\n",test));
		for(int i=0;i<size;i++)
		{
			if(!ddio_SerialWriteByte(RocknRide_serial_port,data[i]))
				break;
		}
	}
#endif
}
//	RNR_UpdateControllerInfo
//
//	Updates any Rock 'n' Ride controller data for the frame (if needed)
void RNR_UpdateControllerInfo(game_controls *controls)
{
#ifndef MACINTOSH
	if(!RocknRide_enabled)
		return;
	ASSERT(controls);
	if(!controls)
		return;
	if(RocknRide_lastcontroller_update>Gametime)
	{
		RocknRide_lastcontroller_update = 0.0f;	//handle new game started
	}
	if(RocknRide_lastcontroller_update+RNR_CONTROLLER_UPDATE>Gametime)
		return;	//don't update yet
	
	RocknRide_lastcontroller_update = Gametime;
	ubyte x_val,y_val;
	x_val = 128 + (127.0f*controls->heading_thrust);
	y_val = 128 + (127.0f*controls->pitch_thrust);
	tRocknride_packet packet;
	packet.packet_type = RNRP_POSITION;
	packet.pos.x = x_val;
	packet.pos.y = y_val;
	RNR_SendPacket(&packet);
#endif
}
//	RNR_UpdateForceFeedbackInfo
//
//	Updates any Force Feedback effects
void RNR_UpdateForceFeedbackInfo(float magnitude,vector *direction)
{
#ifndef MACINTOSH
	if(!RocknRide_enabled)
		return;
	ASSERT(magnitude>=0 && magnitude<=1);
	
	if(RocknRide_lastforce_update>Gametime)
	{
		RocknRide_lastforce_update = 0.0f;	//handle new game started
	}
	if(RocknRide_lastforce_update+RNR_FORCE_UPDATE>Gametime)
		return;	//don't update yet
	
	RocknRide_lastforce_update = Gametime;
	matrix mat = Identity_matrix;
	angvec ag;
	vm_VectorToMatrix(&mat,direction);
	vm_ExtractAnglesFromMatrix(&ag,&mat);
	
	float sh,ch;
	vm_SinCos (ag.h,&sh,&ch);
		
	float x_val = magnitude*ch;
	float y_val = magnitude*sh;
	x_val *= 255.0f;
	y_val *= 255.0f;
	ubyte x_b = (ubyte)x_val;
	ubyte y_b = (ubyte)y_val;
	tRocknride_packet packet;
	packet.packet_type = RNRP_HIT;
	packet.pos.x = x_b;
	packet.pos.y = y_b;
	RNR_SendPacket(&packet);
#endif
}
//	RNR_UpdateForceFeedbackInfo
//
//	Updates any Force Feedback effects
void RNR_UpdateForceFeedbackInfo(float magnitude,int *direction)
{
#ifndef MACINTOSH
	if(!RocknRide_enabled)
		return;
	if(RocknRide_lastforce_update>Gametime)
	{
		RocknRide_lastforce_update = 0.0f;	//handle new game started
	}
	if(RocknRide_lastforce_update+RNR_FORCE_UPDATE>Gametime)
		return;	//don't update yet
	
	RocknRide_lastforce_update = Gametime;
	
	Int3();	//I don't think we ever use FF with an int direction
#endif
}
//	RNR_UpdateGameStatus
//
//	Updates a game status to a Rock 'n' Ride chair
void RNR_UpdateGameStatus(ubyte status)
{
#ifndef MACINTOSH
	if(!RocknRide_enabled)
		return;
	tRocknride_packet packet;
	packet.packet_type = RNRP_GAMESTATUS;
	
	bool send = true;
	switch(status)
	{
	case RNRGSC_PLAYERDIES:
		packet.status = 'D';
		break;
	case RNRGSC_INMENU:
		packet.status = 'P';		
		break;
	default:
		send = false;
		break;
	}	
	if(send)
		RNR_SendPacket(&packet);
#endif
}