/*
 * $Logfile: /DescentIII/Main/ddio_mac/macmouse.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:56:55 $
 * $Author: kevinb $
 *
 * Non-Game control mouse support (ie for menus, etc)
 *
 * $Log: macmouse.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:56:55  kevinb
 * initial 1.5 import
 *
 * 
 * 3     10/21/99 3:33p Jeff
 * Macintosh merges
 * 
 * 2     7/28/99 3:31p Kevin
 * Mac Stuff!
 * 
 * 3     5/15/97 1:46 AM Jeremy
 * fixed a bug in the mouse error handler for error reporting (was not
 * setting member variables to input parameters)
 * 
 * 2     5/13/97 11:13 AM Jeremy
 * simple mouse implementation (one button-two axis).  this is only for
 * non-game control (menus,etc.)
 * 
 * 1     5/12/97 11:54 AM Jeremy
 * initial checkin
 * 
 * 
 * $NoKeywords: $
 */
// ANSI Headers
#include <stdlib.h>
// Macintosh Headers
#include <Events.h>
#include <sioux.h>
// Descent 3 Headers
#include "pserror.h"
#include "mono.h"
#include "ddio.h"
// ----------------------------------------------------------------------------
//	File Level Constants
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//	File Level Data Structures
// ----------------------------------------------------------------------------
class CMouseData
{
	public:
	CMouseData(void);
	~CMouseData(void);
	
	public:
	bool	mInitted;
	
	int		mXAxis;
	int		mYAxis;
	
	int 	mMaxX;
	int		mMinX;
	int 	mMaxY;
	int		mMinY;
};
CMouseData::CMouseData(void)
{
	mInitted 		= false;
	mXAxis			= NULL;
	mYAxis			= NULL;
	
	mMaxX = 0x7FFFFFFF;
	mMinX = 0xFFFFFFFF;
	mMaxY = 0x7FFFFFFF;
	mMinY = 0xFFFFFFFF;
}
CMouseData::~CMouseData(void)
{
	;
}
class CMouseErr
{
	public:
	CMouseErr(char* inErrStr = NULL, OSErr inMacErr = noErr)
	{
		mErrStr = inErrStr;
		mMacErr = inMacErr;
	}
	
	char* 	mErrStr;
	OSErr	mMacErr;
};
// ----------------------------------------------------------------------------
//	File Level Global
// ----------------------------------------------------------------------------
static CMouseData	gMouse;
// ----------------------------------------------------------------------------
//	Functions
// ----------------------------------------------------------------------------
void ddio_MouseClose(void);
//	initializes mouse.  specify maximum number of buttons to support.
bool ddio_MouseInit(void)
{
	return true;
}
void ddio_GetMouseCaps(int *buttons, int *axes)
{
	*buttons = 1;
	*axes = 2;	
}
void ddio_MouseClose()
{
}
//	set bounds for return values of absolute coordinates.
void ddio_MouseSetLimits(int left, int top, int right, int bottom, int zmin, int zmax)
{
	gMouse.mMaxX = right;
	gMouse.mMinX = left;
	
	gMouse.mMaxY = bottom;
	gMouse.mMinY = top;
}
void ddio_MouseGetLimits(int *left, int *top, int *right, int *bottom, int *zmin, int *zmax)
{
	*right 	= gMouse.mMaxX;
	*left	= gMouse.mMinX;
	
	*bottom	= gMouse.mMaxY;
	*top	= gMouse.mMinY;
	*zmin = 0;
	*zmax = 0;
}
//	resets position of mouse to 0,0.
void ddio_MouseReset()
{
	ddio_MouseSetLimits(0, 0 ,640, 480);
}
//	returns absolute position of mouse, button state and mouse deltas.
int ddio_MouseGetState(int *x, int *y, int *dx, int *dy, int *z, int *dz)
{
	int		buttonState = 0;
	
	int mx, my;
	Point	mouseLoc = {0,0};
	::GetMouse(&mouseLoc);
	::LocalToGlobal(&mouseLoc);
	mx = mouseLoc.h;
	my = mouseLoc.v;
//	fprintf(stderr, "%06d %06d\n", mx, my);
	
	*x = mx;
	*y = my;
	
/*	
	static int lastX = 0;
	static int lastY = 0;
	*x = (mouseLoc.h > gMouse.mMaxX) ? gMouse.mMaxX : mouseLoc.h;
	*x = (mouseLoc.h < gMouse.mMinX) ? gMouse.mMinX : *x;
	
	*dx = *x - lastX;	
	lastX = *x;
	
	*y = (mouseLoc.v > gMouse.mMaxY) ? gMouse.mMaxY : mouseLoc.v;
	*y = (mouseLoc.v < gMouse.mMinY) ? gMouse.mMinY : *y;
	*dy = *y - lastY;	
	lastY = *y;
	
	if (z)
	{
		*z = 0;
	}
	if (dz)
	{
		*dz = 0;
	}
*/	
	if (Button())
	{
		buttonState = MOUSE_LB;	
	}
	
	return buttonState;
}
//	displays the mouse pointer.  Each Hide call = Show call.
static int Mouse_counter=0;
void ddio_MouseShow()
{
  Mouse_counter++;
  //if (Mouse_counter == 1) ShowCursor(TRUE);
}
void ddio_MouseHide()
{
  Mouse_counter--;
  //if (Mouse_counter == 0) ShowCursor(FALSE);
}
void ddio_InternalMouseSuspend(void)
{
}
void ddio_InternalMouseResume(void)
{
}
void ddio_InternalMouseFrame(void)
{
}
// resets mouse queue and button info only.
void ddio_MouseQueueFlush()
{
}
// returns string to binding.
const char *ddio_MouseGetBtnText(int btn)
{
	return "MSE";
}
const char *ddio_MouseGetAxisText(int axis)
{
	return "MSE";
}
void ddio_MouseMode(int mode)
{
}
// virtual coordinate system for mouse (match to video resolution set for optimal mouse usage.
void ddio_MouseSetVCoords(int width, int height)
{
}
// gets a mouse button event, returns false if none.
bool ddio_MouseGetEvent(int *btn, bool *state)
{
	EventRecord event;
	
//	GetNextEvent(everyEvent - diskMask, &event);
//	GetOSEvent(mDownMask + mUpMask, &event);
	GetNextEvent(mDownMask + mUpMask, &event);
//	GetNextEvent(everyEvent, &event);
	
//	SIOUXHandleOneEvent(&event);
	
	if ( event.what == mouseDown) {
//		mprintf((2, "mouse event down\n"));
		*btn = 1;
		*state = true;	
		return true;
	} else if ( event.what == mouseUp) {
//		mprintf((2, "mouse event up\n"));
		*btn = 1;
		*state = false;
		return true;
	} else {
//		mprintf((1, "mouse NC\n"));
		*btn = 1;
		*state = false;
		return false;
	}
}
///////////////////////////////////////////////////////////////////////////////
char Ctltext_MseBtnBindings[N_MSEBTNS][32] = {
	"mse-l\0\0\0\0\0\0\0\0\0\0\0\0",
	"mse-r\0\0\0\0\0\0\0\0\0\0\0\0",
	"mse-c\0\0\0\0\0\0\0\0\0\0\0\0",
	"mse-b4\0\0\0\0\0\0\0\0\0\0\0", 
	"msew-u\0\0\0\0\0\0\0\0\0\0\0",
	"msew-d\0\0\0\0\0\0\0\0\0\0\0",
	"",""
};
char Ctltext_MseAxisBindings[][32]	= {
	"mse-X\0\0\0\0\0\0\0\0\0\0\0\0", 
	"mse-Y\0\0\0\0\0\0\0\0\0\0\0\0", 
	"msewheel\0\0\0\0\0\0\0\0\0\0"
};
