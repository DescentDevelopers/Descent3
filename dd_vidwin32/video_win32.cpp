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

--- HISTORICAL COMMENTS FOLLOW ---

 * $Logfile: /DescentIII/Main/ddvid_win32/video_win32.cpp $
 * $Revision: 6 $
 * $Date: 8/28/98 4:20p $
 * $Author: Jeff $
 *
 *	Video library.
 *
 * $Log: /DescentIII/Main/ddvid_win32/video_win32.cpp $
 *
 * 6     8/28/98 4:20p Jeff
 * fixed cinemtaics so they initialize ddraw to play the movie
 *
 * 5     7/15/98 5:56p Samir
 * commented out GDIX subsystem.
 *
 * 4     3/06/98 2:22p Samir
 * Added fullscreen windowed version.
 *
 * 3     2/03/98 3:12p Samir
 * Enable access to directdraw object by DDAccess libraries.
 *
 * 2     12/23/97 6:17p Samir
 * Added DDGR subsystem interface.
 *
 * 1     12/22/97 12:45p Samir
 * Initial revision
 *
 * $NoKeywords: $
 */

#include "DDAccess.h" // DD Access enabled

#include "ddvidlib.h"
#include "pserror.h"
#include "Application.h"

#include <string.h>

//////////////////////////////////////////////////////////////////////////////
//	Variables

tDDVideoInfo DDVideo_info;
static bool DDVideo_init = false;

char *DDVID_subsystem_names[] = {"GDIF", // Win32 GDI Fullscreen
                                 "GDI",  // Standard Win32 GDI DIBs
                                 "GDIX", // GDI and DirectDraw for mode settings.
                                 "DX",   // Direct X!
                                 NULL};

int DDVID_subsystems[] = {VID_GDIF_SUBSYSTEM, VID_GDI_SUBSYSTEM, VID_GDIX_SUBSYSTEM, VID_DX_SUBSYSTEM, -1};

//////////////////////////////////////////////////////////////////////////////
//	Prototypes

HRESULT WINAPI DDEnumModesCallback(LPDDSURFACEDESC lpDDSurfaceDesc, LPVOID lpContext);

//////////////////////////////////////////////////////////////////////////////
//	Functions

//	called first to allow fullscreen video access
bool ddvid_Init(oeApplication *app, char *driver) {
  int subsys_id;

  //	preinitialize system.
  if (!DDVideo_init) {
    DDVideo_info.lpDD = NULL;
    DDVideo_info.lpDDSFront = NULL;
    DDVideo_info.lpDDSBack = NULL;
    DDVideo_info.hWnd = NULL;
    DDVideo_info.nDDModes = 0;
    DDVideo_info.curmode = -1;
    DDVideo_info.surf_data = NULL;
    DDVideo_info.gdi.hBackBmp = NULL;
    DDVideo_info.gdi.hBackDC = NULL;
    DDVideo_info.gdi.data = NULL;
    DDVideo_info.gdi.pitch = 0;
    DDVideo_info.gdi.w = 0;
    DDVideo_info.gdi.h = 0;
    DDVideo_info.gdi.color_depth = 0;

    atexit(ddvid_Close);
  } else {
    ddvid_Close();
  }

  DDVideo_init = true;

  DDVideo_info.app = (oeWin32Application *)app;

  //	find subsystem id based off of subsystem requested.
  for (subsys_id = 0; DDVID_subsystems[subsys_id] != -1; subsys_id++) {
    if (strcmp(DDVID_subsystem_names[subsys_id], driver) == 0)
      break;
  }

  if (DDVID_subsystems[subsys_id] == -1)
    Error("Subsystem %s not found during startup.", driver);

  DDVideo_info.subsystem = DDVID_subsystems[subsys_id];
  DDVideo_info.hWnd = (HWND)(((oeWin32Application *)app)->m_hWnd);

  //	initialize directdraw object.
  switch (DDVideo_info.subsystem) {
  case VID_GDIF_SUBSYSTEM:
  case VID_GDI_SUBSYSTEM:
    return ddvidwin_Init();
  case VID_GDIX_SUBSYSTEM:
    return true;
  case VID_DX_SUBSYSTEM:
    return ddvidfs_Init();
  default:
    Int3();
  }

  return false;
}

//	closes ddvid system manually.
void ddvid_Close() {
  if (!DDVideo_init)
    return;

  switch (DDVideo_info.subsystem) {
  case VID_GDIF_SUBSYSTEM:
  case VID_GDI_SUBSYSTEM:
    ddvidwin_Close();
    break;
  case VID_GDIX_SUBSYSTEM:
    break;
  case VID_DX_SUBSYSTEM:
    ddvidfs_Close();
    break;
  default:
    Int3();
  }

  DDVideo_init = false;
}

//	sets the appropriate video mode.
bool ddvid_SetVideoMode(int w, int h, int color_depth, bool paged) {
  ASSERT(DDVideo_init);

  switch (DDVideo_info.subsystem) {
  case VID_GDIF_SUBSYSTEM:
    ddvidwin_CloseVideo();
    return ddvidwin_SetVideoMode(w, h, color_depth, paged, true);

  case VID_GDI_SUBSYSTEM:
    ddvidwin_CloseVideo();
    return ddvidwin_SetVideoMode(w, h, color_depth, paged);

  case VID_GDIX_SUBSYSTEM:
    ddvidwin_CloseVideo();
    ddvidfs_CloseVideo();
    return (ddvidfs_SetVideoMode(w, h, color_depth, false) &&
            ddvidwin_SetVideoMode(DDVideo_info.DDModes[DDVideo_info.curmode].dwWidth,
                                  DDVideo_info.DDModes[DDVideo_info.curmode].dwHeight, color_depth, paged));
  case VID_DX_SUBSYSTEM:
    ddvidfs_CloseVideo();
    return ddvidfs_SetVideoMode(w, h, color_depth, paged);
  default:
    Int3();
  }

  return false;
}

//	sets screen handle
void ddvid_SetVideoHandle(unsigned handle) { DDVideo_info.hVidWnd = (HWND)handle; }

//	retrieves screen information
void ddvid_GetVideoProperties(int *w, int *h, int *color_depth) {
  ASSERT(DDVideo_init);

  switch (DDVideo_info.subsystem) {
  case VID_GDIF_SUBSYSTEM:
  case VID_GDI_SUBSYSTEM:
    ddvidwin_GetVideoProperties(w, h, color_depth);
    break;
  case VID_GDIX_SUBSYSTEM:
    ddvidfs_GetVideoProperties(w, h, color_depth);
    break;
  case VID_DX_SUBSYSTEM:
    ddvidfs_GetVideoProperties(w, h, color_depth);
    break;
  default:
    Int3();
  }
}

//	retrieves screen aspect ratio.
float ddvid_GetAspectRatio() {
  float aspect_ratio = (float)((3.0 * GetSystemMetrics(SM_CXSCREEN)) / (4.0 * GetSystemMetrics(SM_CYSCREEN)));
  return aspect_ratio;
}

// only available to DD_ACCESS libraries.
//	dd_obj is the DIRECTDRAW OBJECT for the system.
//	dds_obj is the DIRECTDRAWSURFACE OBJECT for the screen
void ddvid_GetVideoDDrawProps(uint *dd_obj, uint *dds_obj) {
  switch (DDVideo_info.subsystem) {
  case VID_GDIX_SUBSYSTEM:
    *dd_obj = NULL;
    break;
  case VID_DX_SUBSYSTEM:
    *dd_obj = ddvidfs_GetDirectDrawObject();
    break;
  }

  *dds_obj = NULL;
}

//	flips screen if there's a back buffer
void ddvid_VideoFlip() {
  switch (DDVideo_info.subsystem) {
  case VID_GDIF_SUBSYSTEM:
  case VID_GDI_SUBSYSTEM:
    ddvidwin_VideoFlip();
    break;
  case VID_GDIX_SUBSYSTEM:
    ddvidwin_VideoFlip();
    break;
  case VID_DX_SUBSYSTEM:
    ddvidfs_VideoFlip();
    break;
  default:
    Int3();
  }
}

//	retreives frame buffer info for a video mode.
void ddvid_LockFrameBuffer(ubyte **data, int *pitch) {
  //	locks a direct draw paged surface or a paged window.  unpaged buffers won't lock.
  ASSERT(DDVideo_init);

  switch (DDVideo_info.subsystem) {
  case VID_GDIF_SUBSYSTEM:
  case VID_GDI_SUBSYSTEM: {
    *data = (ubyte *)DDVideo_info.gdi.data;
    *pitch = DDVideo_info.gdi.pitch;
  } break;
  case VID_GDIX_SUBSYSTEM: {
    *data = (ubyte *)DDVideo_info.gdi.data;
    *pitch = DDVideo_info.gdi.pitch;
  } break;
  case VID_DX_SUBSYSTEM: {
    HRESULT hres;
    DDSURFACEDESC ddsd;

    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);

    if (DDVideo_info.lpDDSBack) {
      hres = DDVideo_info.lpDDSBack->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
      if (hres != DD_OK) {
        Error("Unable to set lock DirectDraw surface(%d)\n", LOWORD(hres));
      }
      *pitch = (int)ddsd.lPitch;
      *data = (ubyte *)ddsd.lpSurface;
      DDVideo_info.surf_data = (char *)(*data);
    } else {
      *data = NULL;
      *pitch = 0;
    }
  } break;
  default:
    Int3();
  }
}

void ddvid_UnlockFrameBuffer() {
  ASSERT(DDVideo_init);

  switch (DDVideo_info.subsystem) {
  case VID_GDIF_SUBSYSTEM:
  case VID_GDI_SUBSYSTEM:
  case VID_GDIX_SUBSYSTEM:
    break;
  case VID_DX_SUBSYSTEM: {
    HRESULT hres;

    if (DDVideo_info.lpDDSBack) {
      hres = DDVideo_info.lpDDSBack->Unlock(DDVideo_info.surf_data);
      if (hres != DD_OK)
        Error("Unable to unlock DirectDraw surface(%d)\n", LOWORD(hres));
      DDVideo_info.surf_data = NULL;
    }
  } break;
  default:
    Int3();
  }
}
