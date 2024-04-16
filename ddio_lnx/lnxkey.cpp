/*
* $Logfile: /DescentIII/Main/ddio_lnx/lnxkey.cpp $
* $Revision: 1.3 $
* $Date: 2004/02/25 00:04:06 $
* $Author: ryan $
*
* Linux keyboard routines
*
* $Log: lnxkey.cpp,v $
* Revision 1.3  2004/02/25 00:04:06  ryan
* Removed loki_utils dependency and ported to MacOS X (runs, but incomplete).
*
* Revision 1.2  2000/04/27 11:17:07  icculus
* Removed some keyboard drivers, added SDL hooks.
*
* Revision 1.1.1.1  2000/04/18 00:00:33  icculus
* initial checkin
*
 * 
 * 13    7/14/99 9:06p Jeff
 * added comment header
*
* $NoKeywords: $
*/

/*
 * Linux Keyboard Interface, Non-SVGALIB Implementation
 * $NoKeywords: $
 */

// ----------------------------------------------------------------------------
//	Keyboard Interface
// ----------------------------------------------------------------------------
#include "pserror.h"
#include "mono.h"
#include "ddio.h"
#include "ddio_lnx.h"
#include "application.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <fcntl.h>
#include <termios.h>
//#include <linux/kd.h>
//#include <linux/vt.h>
#include <fcntl.h>
#include <sys/stat.h>
//#include <linux/keyboard.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
//#include <linux/unistd.h>

volatile struct tLnxKeys
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

//normal mode ddio
bool ddio_normal_InternalKeyInit(ddio_init_info *init_info);
void ddio_normal_InternalKeyClose();
bool ddio_normal_InternalKeyState(ubyte key);
void ddio_normal_InternalKeySuspend();
void ddio_normal_InternalKeyResume();
float ddio_normal_InternalKeyDownTime(ubyte key);
void ddio_normal_InternalResetKey(ubyte key);
bool ddio_normal_KeyFrame();
void ddio_normal_InternalKeyFrame(void);

//null mode ddio
bool ddio_null_InternalKeyInit(ddio_init_info *init_info);
void ddio_null_InternalKeyClose();
bool ddio_null_InternalKeyState(ubyte key);
void ddio_null_InternalKeySuspend();
void ddio_null_InternalKeyResume();
float ddio_null_InternalKeyDownTime(ubyte key);
void ddio_null_InternalResetKey(ubyte key);
bool ddio_null_KeyFrame();
void ddio_null_InternalKeyFrame(void);

// sdl mode ddio
bool ddio_sdl_InternalKeyInit(ddio_init_info *init_info);
void ddio_sdl_InternalKeyClose();
bool ddio_sdl_InternalKeyState(ubyte key);
void ddio_sdl_InternalKeySuspend();
void ddio_sdl_InternalKeyResume();
float ddio_sdl_InternalKeyDownTime(ubyte key);
void ddio_sdl_InternalResetKey(ubyte key);
bool ddio_sdl_KeyFrame();
void ddio_sdl_InternalKeyFrame(void);

/*
//xwin mode ddio
bool ddio_xwin_InternalKeyInit(ddio_init_info *init_info);
void ddio_xwin_InternalKeyClose();
bool ddio_xwin_InternalKeyState(ubyte key);
void ddio_xwin_InternalKeySuspend();
void ddio_xwin_InternalKeyResume();
float ddio_xwin_InternalKeyDownTime(ubyte key);
void ddio_xwin_InternalResetKey(ubyte key);
void ddio_xwin_InternalKeyFrame(void);

//svga mode ddio
bool ddio_svga_InternalKeyInit(ddio_init_info *init_info);
void ddio_svga_InternalKeyClose();
bool ddio_svga_InternalKeyState(ubyte key);
void ddio_svga_InternalKeySuspend();
void ddio_svga_InternalKeyResume();
float ddio_svga_InternalKeyDownTime(ubyte key);
void ddio_svga_InternalResetKey(ubyte key);
bool ddio_svga_KeyFrame();
void ddio_svga_InternalKeyFrame(void);
*/

enum{
	Input_normal,Input_null,Input_sdl //Input_svga,Input_xwin
}Keyboard_mode;

// ----------------------------------------------------------------------------
//	Initialization of keyboard device.
// ----------------------------------------------------------------------------

bool ddio_InternalKeyInit(ddio_init_info *init_info)
{
	oeLnxApplication *app = (oeLnxApplication *)init_info->obj;
	tLnxAppInfo app_info;

	if(!app)
	{
		return false;
	}

	app->get_info(&app_info);

	//determine if we are to use normal or null mode
	Keyboard_mode = Input_normal;
	if(app_info.flags&APPFLAG_USESERVICE)
	{
		//use null mode!
		Keyboard_mode = Input_null;
	}else
	{
  		Keyboard_mode = Input_sdl;
/*
		if(app_info.flags&APPFLAG_USESVGA)
		{
			//use svgalib mode!
			Keyboard_mode = Input_svga;
		}
		else if (app_info.m_Display) {
		// we're under xwindows and we're not forcing any type of keyboard input.
			Keyboard_mode = Input_xwin;
		}
*/
	}

	switch(Keyboard_mode)
	{
	case Input_normal:
		return ddio_normal_InternalKeyInit(init_info);
	case Input_null:
		return ddio_null_InternalKeyInit(init_info);
    case Input_sdl:
		return ddio_sdl_InternalKeyInit(init_info);

//	case Input_svga:
//		return ddio_svga_InternalKeyInit(init_info);
//	case Input_xwin:
//		return ddio_xwin_InternalKeyInit(init_info);
	}
	
	return false;	
}

void ddio_InternalKeyClose()
{
	switch(Keyboard_mode)
	{
	case Input_normal:
		return ddio_normal_InternalKeyClose();
	case Input_null:
		return ddio_null_InternalKeyClose();
	case Input_sdl:
		return ddio_sdl_InternalKeyClose();
/*
	case Input_svga:
		return ddio_svga_InternalKeyClose();
	case Input_xwin:
		return ddio_xwin_InternalKeyClose();
*/
	}
}

bool ddio_InternalKeyState(ubyte key)
{
	switch(Keyboard_mode)
	{
	case Input_normal:
		return ddio_normal_InternalKeyState(key);
	case Input_null:
		return ddio_null_InternalKeyState(key);
	case Input_sdl:
		return ddio_sdl_InternalKeyState(key);
/*
	case Input_svga:
		return ddio_svga_InternalKeyState(key);
	case Input_xwin:
		return ddio_xwin_InternalKeyState(key);
*/
	}

	return false;
}

void ddio_InternalKeySuspend()
{
	switch(Keyboard_mode)
	{
	case Input_normal:
		ddio_normal_InternalKeySuspend();
		break;
	case Input_null:
		ddio_null_InternalKeySuspend();
		break;
	case Input_sdl:
		return ddio_sdl_InternalKeySuspend();

/*
	case Input_svga:
		ddio_svga_InternalKeySuspend();
		break;
	case Input_xwin:
		ddio_xwin_InternalKeySuspend();
		break;
*/
	}
}

void ddio_InternalKeyResume()
{
	switch(Keyboard_mode)
	{
	case Input_normal:
		ddio_normal_InternalKeyResume();
		break;
	case Input_null:
		ddio_null_InternalKeyResume();
		break;
	case Input_sdl:
		return ddio_sdl_InternalKeyResume();

/*
	case Input_svga:
		ddio_svga_InternalKeyResume();
		break;
	case Input_xwin:
		ddio_xwin_InternalKeyResume();
		break;
*/
	}
}

float ddio_InternalKeyDownTime(ubyte key)
{
	switch(Keyboard_mode)
	{
	case Input_normal:
		return ddio_normal_InternalKeyDownTime(key);
	case Input_null:
		return ddio_null_InternalKeyDownTime(key);
	case Input_sdl:
		return ddio_sdl_InternalKeyDownTime(key);
/*
	case Input_svga:
		return ddio_svga_InternalKeyDownTime(key);
	case Input_xwin:
		return ddio_xwin_InternalKeyDownTime(key);
*/
	}

	return 0.0f;
}

void ddio_InternalResetKey(ubyte key)
{
	switch(Keyboard_mode)
	{
	case Input_normal:
		ddio_normal_InternalResetKey(key);
		break;
	case Input_null:
		ddio_null_InternalResetKey(key);
		break;
	case Input_sdl:
		return ddio_sdl_InternalResetKey(key);
/*
	case Input_svga:
		ddio_svga_InternalResetKey(key);
		break;
	case Input_xwin:
		ddio_xwin_InternalResetKey(key);
		break;
*/
	}
}

bool ddio_KeyFrame()
{
	switch(Keyboard_mode)
	{
	case Input_normal:
		return ddio_normal_KeyFrame();
	case Input_null:
		return ddio_null_KeyFrame();
/*
	case Input_svga:
		return ddio_svga_KeyFrame();
*/
	}

	return true;
}


void ddio_InternalKeyFrame(void)
{
	switch(Keyboard_mode)
	{
	case Input_normal:
		ddio_normal_InternalKeyFrame();
		break;
	case Input_null:
		ddio_null_InternalKeyFrame();
		break;
	case Input_sdl:
		return ddio_sdl_InternalKeyFrame();
/*
	case Input_svga:
		ddio_svga_InternalKeyFrame();
		break;
	case Input_xwin:
		ddio_xwin_InternalKeyFrame();
		break;
*/
	}
}

void ddio_SetKeyboardLanguage(int)
{
}


