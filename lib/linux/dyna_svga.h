#ifdef DECLARE_POINTERS
#define FEXTERN
#else
#define FEXTERN extern
#endif
#include <vga.h>
#include <vgakeyboard.h>

//remapping
#define keyboard_init sokeyboard_init
#define keyboard_seteventhandler sokeyboard_seteventhandler
#define keyboard_close sokeyboard_close
#define keyboard_update sokeyboard_update

typedef int (*keyboard_init_fp)(void);
FEXTERN keyboard_init_fp sokeyboard_init;

typedef void (*keyboard_seteventhandler_fp)(__keyboard_handler handler);
FEXTERN keyboard_seteventhandler_fp sokeyboard_seteventhandler;

typedef void (*keyboard_close_fp)(void);
FEXTERN keyboard_close_fp sokeyboard_close;

typedef int (*keyboard_update_fp)(void);
FEXTERN keyboard_update_fp sokeyboard_update;

#ifndef DECLARE_POINTERS
bool LoadSVGALib(bool load = true);
#else
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
void LoadSVGALibSetNULL(void)
{
	sokeyboard_init = NULL;
	sokeyboard_seteventhandler = NULL;
	sokeyboard_close = NULL;
	sokeyboard_update = NULL;
}

bool LoadSVGALib(bool load)
{
#define SVGALIB	"libvga.so"
	static void *handle = NULL;

	if(!load)
	{
		LoadSVGALibSetNULL();
		if(handle)
		{
			dlclose(handle);
			handle = NULL;
			return true;
		}
		fprintf(stderr,"Library Unload Failed: %s\n",SVGALIB);
		return false;
	}

	if(handle)
		return true;

	// Load the library
	handle = dlopen(SVGALIB,RTLD_LAZY|RTLD_GLOBAL);
	if(!handle)
	{
		fprintf(stderr,"Library Load Failed: %s\n",SVGALIB);
		return false;
	}

	sokeyboard_init = (keyboard_init_fp)dlsym(handle,"keyboard_init");
	if(!sokeyboard_init) goto load_error;
	
	sokeyboard_seteventhandler = (keyboard_seteventhandler_fp)dlsym(handle,"keyboard_seteventhandler");
	if(!sokeyboard_seteventhandler) goto load_error;
	
	sokeyboard_close = (keyboard_close_fp)dlsym(handle,"keyboard_close");
	if(!sokeyboard_close) goto load_error;
	
	sokeyboard_update = (keyboard_update_fp)dlsym(handle,"keyboard_update");
	if(!sokeyboard_update) goto load_error;
	
	return true;

load_error:
	LoadSVGALibSetNULL();
	fprintf(stderr,"Library Symbol Resolve Error: %s\n",SVGALIB);
	dlclose(handle);
	handle = NULL;
	return false;
}
#endif