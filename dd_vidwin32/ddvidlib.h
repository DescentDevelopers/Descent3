/*
 * $Logfile: /DescentIII/Main/ddvid_win32/ddvidlib.h $
 * $Revision: 4 $
 * $Date: 3/06/98 2:22p $
 * $Author: Samir $
 *
 *	Video library internal header
 *
 * $Log: /DescentIII/Main/ddvid_win32/ddvidlib.h $
 *
 * 4     3/06/98 2:22p Samir
 * Added fullscreen windowed version.
 *
 * 3     2/03/98 3:12p Samir
 * Enable access to directdraw object by DDAccess libraries.
 *
 * 2     12/30/97 1:54p Samir
 * Upped max modes.
 *
 * 1     12/23/97 5:46p Samir
 * Initial revision
 *
 * $NoKeywords: $
 */

#ifndef DDVIDLIB_H
#define DDVIDLIB_H

#include "ddvid.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ddraw.h>
#include <stdlib.h>

#define VM_MAX_MODES 96

const int VID_GDIF_SUBSYSTEM = 0, // GDI fullscreen subsystem
    VID_GDI_SUBSYSTEM = 1,        // GDI subsystem
    VID_GDIX_SUBSYSTEM = 2,       // GDIX subsystem (GDI+DirectX)
    VID_DX_SUBSYSTEM = 3;         // DIRECTX subsystem

class oeWin32Application;

typedef struct tDDVideoInfo {
  oeWin32Application *app;
  HWND hWnd;
  HWND hVidWnd;
  int subsystem;

  LPDIRECTDRAW lpDD;
  LPDIRECTDRAWSURFACE lpDDSFront;
  LPDIRECTDRAWSURFACE lpDDSBack;
  DDSURFACEDESC DDModes[VM_MAX_MODES];
  int nDDModes;
  int curmode;
  char *surf_data;

  struct {
    HBITMAP hBackBmp;
    HDC hBackDC;
    char *data;
    int pitch;
    int w, h, color_depth;
    int ndevmodes;
    int olddevmode;
    int curdevmode;
  } gdi;
} tDDVideoInfo;

//	driver info.
extern tDDVideoInfo DDVideo_info;

// inits fullscreen system
bool ddvidfs_Init();

//	closes fullscreen system
void ddvidfs_Close();

//	uses direct draw.  if paged, allows frame buffer access.
bool ddvidfs_SetVideoMode(int w, int h, int color_depth, bool paged);

//	closes video mode for fs
void ddvidfs_CloseVideo();

//	retrieves screen information for fullscreen version
void ddvidfs_GetVideoProperties(int *w, int *h, int *color_depth);

//	flips screen if there's a back buffer
void ddvidfs_VideoFlip();

//	returns the directdraw object
uint ddvidfs_GetDirectDrawObject();

// inits windowed system
bool ddvidwin_Init();

//	closes windowed system
void ddvidwin_Close();

//	creates an offscreen back bitmap if needed. otherwise doesn't do a thing really.
bool ddvidwin_SetVideoMode(int w, int h, int color_depth, bool paged, bool reschange = false);

//	closes video mode for fs
void ddvidwin_CloseVideo();

//	retrieves screen information for windowed version
void ddvidwin_GetVideoProperties(int *w, int *h, int *color_depth);

//	flips screen if there's a back buffer
void ddvidwin_VideoFlip();

#endif
