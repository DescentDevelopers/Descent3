/*
 * $Logfile: /DescentIII/Main/renderer/lnxscreenmode.cpp $
 * $Revision: 1.6 $
 * $Date: 2000/06/29 18:48:13 $
 * $Author: icculus $
 *
 * 
 *
 * $Log: lnxscreenmode.cpp,v $
 * Revision 1.6  2000/06/29 18:48:13  icculus
 * Dumb typo fixed.
 *
 * Revision 1.5  2000/06/29 06:41:24  icculus
 * mad commits.
 *
 * Revision 1.4  2000/06/24 01:15:15  icculus
 * patched to compile.
 *
 * Revision 1.3  2000/05/29 05:25:57  icculus
 * Command line fix: Changed "--nofullscreen" to "--windowed" (or "-w").
 *
 * Revision 1.2  2000/04/28 20:25:51  icculus
 * Updates, cleanups, and SDLification.
 *
 * Revision 1.1.1.1  2000/04/18 00:00:49  icculus
 * initial checkin
 *
 * 
 * 4     9/09/99 4:38p Jeff
 * fixed crashes if lib not loaded
 * 
 * 3     9/06/99 9:21p Jeff
 * lock video mode switching, don't set viewport when restoring (causing a
 * bunch of screen garbage)
 * 
 * 2     9/05/99 9:41p Jeff
 * first checkin of Linux Screen manager
 *
 * $NoKeywords: $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include "lnxscreenmode.h"
#include "pserror.h"
#include "args.h"

CLnxVideoModes LinuxVideoMode;

/*
typedef Bool (*VidModeQueryExtension_fp)(Display* dpy,int* event_base,int* error_base);
typedef Bool (*VidModeGetAllModeLines_fp)(Display* dpy,int screen,int* modecount,XF86VidModeModeInfo*** modelinesPtr);
typedef Bool (*VidModeSwitchToMode_fp)(Display* dpy,int screen,XF86VidModeModeInfo* modeline);
typedef Bool (*VidModeSetViewPort_fp)(Display* dpy,int screen,int x,int y);
typedef void (*VidModeFreeData_fp)(int num_modes,XF86VidModeModeInfo** modelinePtr);
typedef Bool (*VidModeGetModeLine_fp)(Display* dpy,int screen,int* dotclock,XF86VidModeModeLine* modeline);
typedef Bool (*VidModeLockModeSwitch_fp)(Display* dpy,int screen,int lock);

VidModeQueryExtension_fp VidModeQueryExtension;
VidModeGetAllModeLines_fp VidModeGetAllModeLines;
VidModeSwitchToMode_fp VidModeSwitchToMode;
VidModeSetViewPort_fp VidModeSetViewPort;
VidModeFreeData_fp VidModeFreeData;
VidModeGetModeLine_fp VidModeGetModeLine;
VidModeLockModeSwitch_fp VidModeLockModeSwitch;
*/

CLnxVideoModes::CLnxVideoModes()
{
	m_Inited = false;
	m_NumVideoModes = 0;
	m_ModeInfoList = NULL;
//	m_Display = NULL;
	m_VideoResolutionChanged = false;
/*
	VidModeQueryExtension = NULL;
	VidModeGetAllModeLines = NULL;
	VidModeSwitchToMode = NULL;
	VidModeSetViewPort = NULL;
	VidModeFreeData = NULL;
	VidModeGetModeLine = NULL;
	VidModeLockModeSwitch = NULL;
*/
}

CLnxVideoModes::~CLnxVideoModes()
{
	if(!m_Inited)
		return;
		
	// shutdown and free the memory
	RestoreVideoMode();
	
//	VidModeFreeData(m_NumVideoModes,m_ModeInfoList);
}

// enumerate all the possible video modes
bool CLnxVideoModes::Init(void) //Display *dpy,int screen)
{
	if(m_Inited)
		return true;

//	m_Display = dpy;
//	m_Screen = screen;
	m_NumVideoModes = 0;
	m_ModeInfoList = NULL;
	m_Inited = true;
    sdlflags = 0;
/*
	if(!LoadLibraries())
	{
		m_Inited = false;
		fprintf(stdout,"LoadLib: Unable to load VideoMode libraries\n");
		return false;
	}

	int dummy;
	if(!VidModeQueryExtension(dpy,&dummy,&dummy))
		return false;

	if(!VidModeGetAllModeLines(m_Display,m_Screen,&m_NumVideoModes,&m_ModeInfoList))
	{
		m_NumVideoModes = 0;
	}else
	{
*/

    const SDL_VideoInfo *vidInfo = NULL;


    sdlflags = SDL_HWSURFACE | SDL_FULLSCREEN;
    sdlflags = SDL_OPENGL | SDL_FULLSCREEN;
    if (FindArgChar("-windowed", 'w'))
        sdlflags &= ~SDL_FULLSCREEN;

    vidInfo = SDL_GetVideoInfo();

    m_ModeInfoList = SDL_ListModes(vidInfo->vfmt, sdlflags);
    if (m_ModeInfoList == NULL)
    {
        sdlflags &= ~SDL_FULLSCREEN;
        m_ModeInfoList = SDL_ListModes(vidInfo->vfmt, sdlflags); // try without fullscreen.
        if (m_ModeInfoList == NULL)
        {
            sdlflags = 0;
            m_ModeInfoList = SDL_ListModes(vidInfo->vfmt, sdlflags);   // give me ANYTHING.
            if (m_ModeInfoList == NULL)
            {
                fprintf(stderr, "SDL: SDL_ListModes() failed.\n");
                return -3;
            } // if
        } // if
    } // if

    if (m_ModeInfoList == (SDL_Rect **) -1)  // anything is fine.
    {
        mprintf((0, "Windowed surface is probably going to be used."));
        m_NumVideoModes = 0;
    } // if
    else
    {
        mprintf((0, "-------------------------\n"));
       	mprintf((0, "Supported Mode Lines:\n"));
       	for(int i = 0; m_ModeInfoList[i] != NULL; i++)
        {
                //,m_ModeInfoList[i]->hdisplay,m_ModeInfoList[i]->vdisplay);
       		mprintf((0, "     Mode %d: %dX%d\n", i, m_ModeInfoList[i]->w,
                                                    m_ModeInfoList[i]->h));
            m_NumVideoModes++;
        } // for
       	mprintf((0,"-------------------------\n"));
    } // else

	return true;
}

bool CLnxVideoModes::SwitchResolution(int width,int height)
{
/*
	if(!m_Inited)
		return false;

	int i;
	for(i=0;i<m_NumVideoModes;i++)
	{
		if(m_ModeInfoList[i]->hdisplay==width && m_ModeInfoList[i]->vdisplay==height)
		{
			// found a matching resolution, try to change to it
			if(SwitchToMode(m_ModeInfoList[i]))
			{
				fprintf(stdout,"Mode switch to %dX%d OK!\n",width,height);
				m_VideoResolutionChanged = (i==0)?false:true;
				return true;
			}
		}
	}

	return false;			
*/
    return(true);
}

void CLnxVideoModes::RestoreVideoMode(void)
{
/*
	if(!m_Inited)
		return;

	if(!m_VideoResolutionChanged)
		return;

	// index 0 is the original mode
	if(m_NumVideoModes<=1)
		return;

	m_VideoResolutionChanged = false;

	VidModeSwitchToMode(m_Display,m_Screen,m_ModeInfoList[0]);	
*/
    return;
}

/*
bool CLnxVideoModes::SwitchToMode(XF86VidModeModeInfo *mode)
{
	if(!m_Inited)
		return false;

	if(!VidModeSwitchToMode(m_Display,m_Screen,mode))
	{
		fprintf(stdout,"Video: Unable to switch mode to %dX%d\n",mode->hdisplay,mode->vdisplay);
		return false;
	}

	VidModeSetViewPort(m_Display,m_Screen,0,0);
	return true;
}

bool CLnxVideoModes::QueryExtension(Display *dpy)
{
	if(!m_Inited)
		return false;

	int dummy;
	if(!VidModeQueryExtension(dpy,&dummy,&dummy))
		return false;
	return true;
}

void CLnxVideoModes::GetModeLine(Display *dpy,int screen,int *dotclock,XF86VidModeModeLine *modeline)
{
	if(!m_Inited)
		return;

	VidModeGetModeLine(dpy,screen,dotclock,modeline);
}
*/

void CLnxVideoModes::Lock(bool lock)
{
/*
	if(!m_Inited)
		return;

	VidModeLockModeSwitch(m_Display,m_Screen,(lock)?1:0);
*/
}

bool CLnxVideoModes::LoadLibraries(void)
{
/*
	void *handle;
	char *libname = "OutrageDynXLib.so";
	handle = dlopen(libname,RTLD_NOW|RTLD_GLOBAL);
	if(!handle)
	{
		fprintf(stderr,"Library Load Failed: %s (%s)\n",libname,dlerror());
		return false;
	}

	VidModeQueryExtension = (VidModeQueryExtension_fp)dlsym(handle,"VidModeQueryExtension");
	VidModeGetAllModeLines = (VidModeGetAllModeLines_fp)dlsym(handle,"VidModeGetAllModeLines");
	VidModeSwitchToMode = (VidModeSwitchToMode_fp)dlsym(handle,"VidModeSwitchToMode");
	VidModeSetViewPort = (VidModeSetViewPort_fp)dlsym(handle,"VidModeSetViewPort");
	VidModeFreeData = (VidModeFreeData_fp)dlsym(handle,"VidModeFreeData");
	VidModeGetModeLine = (VidModeGetModeLine_fp)dlsym(handle,"VidModeGetModeLine");
	VidModeLockModeSwitch = (VidModeLockModeSwitch_fp)dlsym(handle,"VidModeLockModeSwitch");

	if(	!VidModeQueryExtension	||
		!VidModeGetAllModeLines	||
		!VidModeSwitchToMode	||
		!VidModeSetViewPort		||
		!VidModeFreeData		||
		!VidModeGetModeLine		||
		!VidModeLockModeSwitch)
	{
		fprintf(stderr,"Symbol Resolve Error: %s\n",libname);
		return false;
	}		
*/
	return true;
}

