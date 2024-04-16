#ifdef DECLARE_POINTERS
#define FEXTERN
#else
#define FEXTERN extern
#endif
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/xf86dga.h>

#define XF86DGAQueryVersion DGAQueryVersion
#define XF86DGAQueryExtension DGAQueryExtension
#define XF86DGADirectVideo DGADirectVideo

typedef Bool (*DGAQueryVersion_fp)(Display* pa,int* pb,int* pc);
FEXTERN DGAQueryVersion_fp DGAQueryVersion;

typedef Bool (*DGAQueryExtension_fp)(Display* pa,int* pb,int* pc);
FEXTERN DGAQueryExtension_fp DGAQueryExtension;

typedef Status (*DGADirectVideo_fp)(Display* pa,int pb,int pc);
FEXTERN DGADirectVideo_fp DGADirectVideo;

#ifndef DECLARE_POINTERS
bool LoadOutrageXWindowsLib(bool load = true);
#else
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
void LoadOutrageXWindowsLibSetNULL(void)
{
	DGAQueryVersion = NULL;
	DGAQueryExtension = NULL;
	DGADirectVideo = NULL;
}

bool LoadOutrageXWindowsLib(bool load)
{
#define XWINDOWSEXTLIB	"OutrageDynXLib.so"
	static void *handle = NULL;

	if(!load)
	{
		LoadOutrageXWindowsLibSetNULL();
		if(handle)
		{
			dlclose(handle);
			handle = NULL;
			return true;
		}
		fprintf(stderr,"Library Unload Failed: %s\n",XWINDOWSEXTLIB);
		return false;
	}

	if(handle)
		return true;

	// Load the library
	handle = dlopen(XWINDOWSEXTLIB,RTLD_LAZY|RTLD_GLOBAL);
	if(!handle)
	{
		fprintf(stderr,"Library Load Failed: %s\n",XWINDOWSEXTLIB);
		return false;
	}

	DGAQueryVersion = (DGAQueryVersion_fp)dlsym(handle,"DGAQueryVersion");
	if(!DGAQueryVersion) goto load_error;

	DGAQueryExtension = (DGAQueryExtension_fp)dlsym(handle,"DGAQueryExtension");
	if(!DGAQueryExtension) goto load_error;
	
	DGADirectVideo = (DGADirectVideo_fp)dlsym(handle,"DGADirectVideo");
	if(!DGADirectVideo) goto load_error;

	return true;

load_error:
	LoadOutrageXWindowsLibSetNULL();
	fprintf(stderr,"Library Symbol Resolve Error: %s\n",XWINDOWSEXTLIB);
	dlclose(handle);
	handle = NULL;
	return false;
}
#endif