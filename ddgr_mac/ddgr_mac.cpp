/*
 * $Logfile: /Descent3/main/ddgr_mac/ddgr_mac.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:56:53 $
 * $Author: kevinb $
 *
 * macintosh implementation of interface to ddgr library
 *
 * $Log: ddgr_mac.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:56:53  kevinb
 * initial 1.5 import
 *
 * 
 * 6     5/19/97 2:51 PM Jeremy
 * added default values to constructor
 * 
 * 5     5/15/97 1:47 AM Jeremy
 * changed mprintf's to be standard (with newline at end)
 * 
 * 4     5/11/97 8:01 PM Jeremy
 * implemented call to ddgr_os_surf_GetAspectRatio
 * 
 * 3     5/9/97 7:13 PM Jeremy
 * some bug fixes in initialization code
 * 
 * 2     4/15/97 7:02 PM Jeremy
 * initial implementation of initialization and closing of ddgr mac
 * library.  also added c version of scale bitmap16
 * 
 * 1     4/9/97 7:16 PM Jeremy
 * initial check in
 *
 * $NoKeywords: $
 */

//	---------------------------------------------------------------------------
//	ANSI Headers
//	---------------------------------------------------------------------------
#include <string.h>
#include <stdlib.h>

//	---------------------------------------------------------------------------
//	Macintosh Headers
//	---------------------------------------------------------------------------
#include <DrawSprocket.h>

//	---------------------------------------------------------------------------
//	Descent3 Headers
//	---------------------------------------------------------------------------
#include "pserror.h"
#include "gameos.h"

#include "ddgr.h"
#include "ddgr_mac.h"
#include "macOSSurface.h"
#include "bitmap.h"

//	---------------------------------------------------------------------------
//	File Level Prototypes
//	---------------------------------------------------------------------------
//	Clean up the ddgr library
void ddgr_Close(void);

//	---------------------------------------------------------------------------
//	File Level DataTypes
//	---------------------------------------------------------------------------

//	---------------------------------------------------------------------------
//	File Level Globals
//	---------------------------------------------------------------------------
#include "ddgr_mac.h"

//	---------------------------------------------------------------------------
//	Public Globals 
//	---------------------------------------------------------------------------


//	----------------------------------------------------
//	This object stores the data for the graphics library
//	on the mac.
//	If we were really using good OOP, all of the
//	functions in this file and this data libaray object
//	would be encapulated together, Ah, c'est la vie.
//	----------------------------------------------------
ddgr_mac_lib Mac_DDGR_Lib;

//	---------------------------------------------------------------------------
//	Mac data library implementations
//	---------------------------------------------------------------------------
ddgr_mac_lib::ddgr_mac_lib(void)
{
	m_initted			= false;
	m_windowed_mode		= false;
	m_color_key			= 0;
}

ddgr_mac_lib::~ddgr_mac_lib(void)
{
	;
}

void ddgr_mac_lib::SetWindowedMode(bool inWindowMode)
{
	m_windowed_mode = inWindowMode;
}

bool ddgr_mac_lib::IsWindowed(void)
{
	return m_windowed_mode;
}

bool ddgr_mac_lib::IsInitted(void)
{
	return m_initted;
}

void ddgr_mac_lib::SetInitted(bool inInitState)
{
	m_initted = inInitState;
}

/*void ddgr_mac_lib::SetDefaultColorKey(ddgr_color inColor)
{
	m_color_key = inColor;
}
*/

//	------------------------------------------------------------
//	ddgr_Init
//		info->hwnd = window handle
//		info->windowed = are screens windowed or fullscreen.
//		info->debug = debug mode 
//			0 = no debug
//			1 = full debug
//	note;
//		this function can be reused.  This is, you can call this
//		function as many times as you want.  You can switch from
//		windowed to full screen, debug to nodebug.
//	------------------------------------------------------------
ddgr_error ddgr_Init(ddgr_init_info *info)
{
	ddgr_error 	err = DDGRERR_SUCCESS;
	OSErr		macErr = noErr;
	static int	firstTime = true;

	//	If we're reinitializing the graphics system, then close the current graphic driver
	if (Mac_DDGR_Lib.IsInitted())
	{
		ddgr_Close();
	}
	
	if (firstTime)
	{
		//	first time initialization!
		atexit(ddgr_Close);
		firstTime = false;
	}
	
	// Inititializing mac os video graphics system
	if (info->windowed)
	{
		Mac_DDGR_Lib.SetWindowedMode(true);
		mprintf((0, "Attempting to initialize ddgr in windowed mode.\n"));

		mprintf((0, "Windowed mode currently not implemented.\n"));
		err = DDGRERR_DRIVERINIT;
	}
	else
	{
		Mac_DDGR_Lib.SetWindowedMode(false);
		mprintf((0, "Attempting to initialize ddgr in full screen mode.\n"));
		
		err = ddgr_os_surf_fullscreen_Init(info);
	}

	// Success!!
	if (!err)
	{
		Mac_DDGR_Lib.SetInitted(true);		
		mprintf((0, "DDGR Initialized\n"));
	}

	return err;
}

void ddgr_Close()
{
	ddgr_error err = DDGRERR_SUCCESS;

	if (Mac_DDGR_Lib.IsInitted())
	{
		// Closing mac os video graphics system
		if (Mac_DDGR_Lib.IsWindowed())
		{
			mprintf((0, "Attempting to close ddgr in windowed mode.\n"));
			mprintf((0, "Windowed mode currently not implemented.\n"));
		}
		else
		{
			mprintf((0, "Attempting to close ddgr in fullscreen mode.\n"));
			ddgr_os_surf_fullscreen_Close();
		}

		Mac_DDGR_Lib.SetInitted(false);		
		mprintf((0, "DDGR is closed.\n"));
	}
	else
	{
		mprintf((0, "Attempted closing *uninitialized* Mac DDGR!\n"));
	}
}

//	GetAspectRatio
//		returns aspect ratio calculated in video screen initialization
float ddgr_GetAspectRatio()
{
	return ddgr_os_surf_GetAspectRatio();
}

//	Set's the surface color key in ddgr_color format
void ddgr_SetSurfColorKey(ddgr_color color)
{
	Mac_DDGR_Lib.SetDefaultColorKey(color);
}

// A C version of a scaled bitmap blitter
void ddgr_ScaleBitmap16 (ushort *dest_data,int dest_width,int x1,int y1,int x2,int y2,
					      int bm,fix u0,fix v0,fix u1,fix v1)
				   
{
	int screen_width=(x2-x1);		// The size of bitmap on the screen
	int screen_height=(y2-y1);
	
	ushort *perm_src_data=bm_data (bm,0);
	ushort *src_data=perm_src_data;
	int src_w=bm_w(bm,0);

	// Set up our initial coordinates

	if (screen_width==0 || screen_height==0)
		return;

	dest_data+=(dest_width*y1+x1);

	fix xstep=(u1-u0)/(screen_width);
	fix ystep=(v1-v0)/(screen_height);

	fix fx_u= u0;
	fix fx_v= v0;

	fix u;
	int y=y1;
	int x,t;

	for (int i=0;i<screen_height;i++,y++,fx_v+=ystep)
	{
		for (x=x1,t=0,u=fx_u;t<screen_width;t++,x++,u+=xstep)
		{
			ushort pix=src_data[FixToInt(fx_v)*src_w+FixToInt(u)];
			if (pix!=TRANSPARENT_COLOR)
				*dest_data++=pix;
			else
				dest_data++;
		}

		dest_data+=(dest_width-screen_width);
	}
}

//¥======================================================
//	MacOS Graphics routines
//¥======================================================
