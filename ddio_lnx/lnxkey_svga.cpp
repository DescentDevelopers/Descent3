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
 * $Logfile: /DescentIII/Main/ddio_lnx/lnxkey_svga.cpp $
 * $Revision: 1.2 $
 * $Date: 2000/04/27 11:17:53 $
 * $Author: icculus $
 *
 * Linux SVGALIB routines
 *
 * $Log: lnxkey_svga.cpp,v $
 * Revision 1.2  2000/04/27 11:17:53  icculus
 * Added #if 0 fence.
 *
 * Revision 1.1.1.1  2000/04/18 00:00:33  icculus
 * initial checkin
 *
 *
 * 5     7/14/99 9:06p Jeff
 * added comment header
 *
 * $NoKeywords: $
 */

// Keyboard handler for SVGAlib

#if 0

Don't use this code.

#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdarg.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <signal.h>
#include <dlfcn.h>

#include "ddio_common.h"
#include "ddio.h"
#include "mono.h"
#include "pserror.h"

//##########################################################
//SVGALib Interface Functions
//##########################################################
#define DECLARE_POINTERS
#include "linux/dyna_svga.h"
#undef DECLARE_POINTERS

void ddio_SVGA_EmergencyQuit(int id);
void ddio_SVGA_InternalKeyInit(void);
void ddio_SVGA_KeyBoardEventHandler(int scancode, int press);
static int ddio_SVGA_TransKey(int scancode);
void ddio_SVGA_DoKeyFrame(void);
void ddio_SVGA_InternalClose(void);
void ddio_svga_InternalKeyClose(void);

//##########################################################

extern volatile struct tLnxKeys
{
	union{
		int up_ticks;
		float up_time;
	};
	union{
		int down_ticks;
		float down_time;
	};
	bool status;
}LKeys[DDIO_MAX_KEYS];
bool DDIO_svga_key_suspend = false;


bool ddio_svga_InternalKeyInit(ddio_init_info *init_info)
{
	if(!LoadSVGALib(true))
	{
		fprintf(stderr,"Unable to initialize SVGAlib\n");
		return false;
	}
		
	static bool first_call = true;
	//reset key list
	for(int i=0;i<DDIO_MAX_KEYS; i++)
	{
		LKeys[i].down_ticks = 0;
  		LKeys[i].up_ticks = 0;
  		LKeys[i].status = false;
  	}
  	DDIO_svga_key_suspend = false;

  	ddio_SVGA_InternalKeyInit();

  	if(first_call)
  	{
  		atexit(ddio_svga_InternalKeyClose);
  		first_call = false;
  	}

  	return true;
}

void ddio_svga_InternalKeyClose()
{
	ddio_SVGA_InternalClose();

	//uninit the SVGAlib
	LoadSVGALib(false);
}

//	ddio_KeyFrame
//		handle input of keyboard per frame.
bool ddio_svga_KeyFrame(void)
{
	ddio_SVGA_DoKeyFrame();
	return true;
}

void ddio_svga_InternalKeyFrame(void)
{
	ddio_svga_KeyFrame();//which gets called?
}

bool ddio_svga_InternalKeyState(ubyte key)
{
	return LKeys[key].status;
}

void ddio_svga_InternalKeySuspend()
{
	DDIO_svga_key_suspend = true;
}

void ddio_svga_InternalKeyResume()
{
	DDIO_svga_key_suspend = false;
}

float ddio_svga_InternalKeyDownTime(ubyte key)
{
	float down_time = 0.0f;
	if(LKeys[key].status)
	{
		float timer = timer_GetTime();
		down_time = timer - LKeys[key].down_time;
		LKeys[key].down_time = timer;
	}else
	{
		down_time = LKeys[key].up_time - LKeys[key].down_time;
		LKeys[key].down_time = LKeys[key].up_time = 0.0f;
	}

	return down_time;
}

void ddio_svga_InternalResetKey(ubyte key)
{
	LKeys[key].down_time = 0.0f;
	LKeys[key].up_time = 0.0f;
	LKeys[key].status = false;
}

//#################################################
//SVGALib Interface Functions
//#################################################
void ddio_SVGA_InternalKeyInit(void)
{
	static bool first_time = true;

	if(!first_time)
	{
		return;
	}
	first_time = false;

	struct sigaction	sact;
	sact.sa_handler = ddio_SVGA_EmergencyQuit;
	sigfillset(&sact.sa_mask);
	sact.sa_flags = 0;
	sigaction(SIGINT, &sact, NULL);

	keyboard_init();
	keyboard_seteventhandler(ddio_SVGA_KeyBoardEventHandler);
}

void ddio_SVGA_InternalClose(void)
{
	if(!keyboard_close)
	{
		mprintf((0,"ddioSVGA: Unable to do internal close, SVGALib not available\n"));
		return;
	}
		
	static bool first_time = true;

	if(first_time)
	{
		keyboard_close();

		first_time = false;
	}
}

void ddio_SVGA_EmergencyQuit(int id)
{
	ddio_SVGA_InternalClose();
	//somehow we need to signal quit here
	exit(-1);
}

void ddio_SVGA_KeyBoardEventHandler(int scancode, int press)
{
	int transkey;

	transkey = ddio_SVGA_TransKey(scancode);	
	if (press == KEY_EVENTPRESS) {
		LKeys[transkey].down_time = timer_GetTime();
		ddio_UpdateKeyState(transkey,true);
	}
	if (press == KEY_EVENTRELEASE) {
		LKeys[transkey].up_time = timer_GetTime();
		ddio_UpdateKeyState(transkey,false);
	}
}

void ddio_SVGA_DoKeyFrame(void)
{
	if(keyboard_update)
		keyboard_update();
}

static int ddio_SVGA_TransKey(int sym)
{
  switch (sym) {
  case SCANCODE_REMOVE: 
    return KEY_DELETE; 
  case SCANCODE_INSERT: 
    return KEY_INSERT; 
  case SCANCODE_PAGEUP: 
    return KEY_PAGEUP; 
  case SCANCODE_PAGEDOWN: 
    return KEY_PAGEDOWN; 
  case SCANCODE_HOME: 
    return KEY_HOME; 
  case SCANCODE_END: 
    return KEY_END;    
  case SCANCODE_ESCAPE:
    return KEY_ESC;
  case SCANCODE_1:
    return KEY_1;
  case SCANCODE_2:
    return KEY_2;
  case SCANCODE_3:
    return KEY_3;
  case SCANCODE_4:
    return KEY_4;
  case SCANCODE_5:
    return KEY_5;
  case SCANCODE_6:
    return KEY_6;
  case SCANCODE_7:
    return KEY_7;
  case SCANCODE_8:
    return KEY_8;
  case SCANCODE_9:
    return KEY_9;
  case SCANCODE_0:
    return KEY_0;
  case SCANCODE_MINUS:
    return KEY_MINUS;
  case SCANCODE_EQUAL:
    return KEY_EQUAL;
  case SCANCODE_BACKSPACE:
    return KEY_BACKSP; 
  case SCANCODE_TAB:
    return KEY_TAB;
  case SCANCODE_Q:
    return KEY_Q;
  case SCANCODE_W:
    return KEY_W;
  case SCANCODE_E:
    return KEY_E;
  case SCANCODE_R:
    return KEY_R;
  case SCANCODE_T:
    return KEY_T;
  case SCANCODE_Y:
    return KEY_Y;
  case SCANCODE_U:
    return KEY_U;
  case SCANCODE_I:
    return KEY_I;
  case SCANCODE_O:
    return KEY_O;
  case SCANCODE_P:
    return KEY_P;
  case SCANCODE_BRACKET_LEFT:
    return KEY_LBRACKET;
  case SCANCODE_BRACKET_RIGHT:
    return KEY_RBRACKET;
  case SCANCODE_ENTER:
    return KEY_ENTER;
  case SCANCODE_LEFTCONTROL:
  	return KEY_LCTRL;
  case SCANCODE_RIGHTCONTROL:
    return KEY_RCTRL;
  case SCANCODE_A:
    return KEY_A;
  case SCANCODE_S:
    return KEY_S;
  case SCANCODE_D:
    return KEY_D;
  case SCANCODE_F:
    return KEY_F;
  case SCANCODE_G:
    return KEY_G;
  case SCANCODE_H:
    return KEY_H;
  case SCANCODE_J:
    return KEY_J;
  case SCANCODE_K:
    return KEY_K;
  case SCANCODE_L:
    return KEY_L;
  case SCANCODE_SEMICOLON:
    return KEY_SEMICOL;
  case SCANCODE_APOSTROPHE:
    return KEY_RAPOSTRO;
  case SCANCODE_GRAVE:
    return KEY_LAPOSTRO;
  case SCANCODE_LEFTSHIFT:
  	return KEY_LSHIFT;
  case SCANCODE_RIGHTSHIFT:
    return KEY_RSHIFT;
  case SCANCODE_BACKSLASH:
    return KEY_BACKSLASH;
  case SCANCODE_Z:
    return KEY_Z;
  case SCANCODE_X:
    return KEY_X;
  case SCANCODE_C:
    return KEY_C;
  case SCANCODE_V:
    return KEY_V;
  case SCANCODE_B:
    return KEY_B;
  case SCANCODE_N:
    return KEY_N;
  case SCANCODE_M:
    return KEY_M;
  case SCANCODE_COMMA:
    return KEY_COMMA;
  case SCANCODE_PERIOD:
    return KEY_PERIOD;
  case SCANCODE_SLASH:
    return KEY_SLASH;
  case SCANCODE_KEYPADMULTIPLY:
    return KEY_PADMULTIPLY;
  case SCANCODE_LEFTALT:
    return KEY_LALT;
  case SCANCODE_RIGHTALT:
    return KEY_RALT;
  case SCANCODE_SPACE:
    return KEY_SPACEBAR;
  case SCANCODE_F1:
    return KEY_F1;
  case SCANCODE_F2:
    return KEY_F2;
  case SCANCODE_F3:
    return KEY_F3;
  case SCANCODE_F4:
    return KEY_F4;
  case SCANCODE_F5:
    return KEY_F5;
  case SCANCODE_F6:
    return KEY_F6;
  case SCANCODE_F7:
    return KEY_F7;
  case SCANCODE_F8:
    return KEY_F8;
  case SCANCODE_F9:
    return KEY_F9;
  case SCANCODE_F10:
    return KEY_F10;
  case SCANCODE_F11:
    return KEY_F11;
  case SCANCODE_F12:
    return KEY_F12;
  case SCANCODE_KEYPADMINUS:
    return KEY_PADMINUS;
  case SCANCODE_KEYPADPLUS:
    return KEY_PADPLUS;
  case SCANCODE_KEYPADPERIOD:
    return KEY_PADPERIOD;
  case SCANCODE_KEYPADENTER:
    return KEY_PADENTER;
  case SCANCODE_KEYPADDIVIDE:
    return KEY_PADDIVIDE;
  case SCANCODE_BREAK:
    return KEY_PAUSE;
  case SCANCODE_BREAK_ALTERNATIVE:
    return KEY_PAUSE;
  case SCANCODE_CURSORBLOCKUP:
    return KEY_UP;
  case SCANCODE_CURSORBLOCKLEFT:
    return KEY_LEFT;
  case SCANCODE_CURSORBLOCKRIGHT:
    return KEY_RIGHT;
  case SCANCODE_CURSORBLOCKDOWN:
    return KEY_DOWN;
  default:
    break;
  }
  return 0;
}

#endif
