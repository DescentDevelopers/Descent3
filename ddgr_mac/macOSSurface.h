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
 * $Logfile: /Descent3/main/ddgr_mac/macOSSurface.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:56:53 $
 * $Author: kevinb $
 *
 * macintosh implementation of interface to ddgr library
 *
 * $Log: macOSSurface.h,v $
 * Revision 1.1.1.1  2003/08/26 03:56:53  kevinb
 * initial 1.5 import
 *
 *
 * 3     5/11/97 8:00 PM Jeremy
 * prototype for ddgr_os_surf_GetAspectRatio
 *
 * 2     4/15/97 7:01 PM Jeremy
 * initial implementation of fullscreen (via DrawSprocket) os_surfaces.
 * right now only initialization and closing of surfaces/video is
 * implemented.  still need to implement surface
 * creation/deletion/blitting.
 *
 * 1     4/11/97 6:02 PM Jeremy
 * initial checkin
 *
 * 1     4/9/97 7:16 PM Jeremy
 * initial check in
 *
 * $NoKeywords: $
 */

#include "ddgr_mac.h"
#include "pserror.h"

//	Initialize the macintosh for full screen graphics
ddgr_error ddgr_os_surf_fullscreen_Init(ddgr_init_info *info);
//	Close down the macintosh full screen graphics system
ddgr_error ddgr_os_surf_fullscreen_Close(void);

//	Initialize the macintosh for windowed graphics
ddgr_error ddgr_os_surf_windowed_Init(ddgr_init_info *info);
//	Close down the macintosh windowed graphics system
ddgr_error ddgr_os_surf_windowed_Close(void);

//	Get the Aspect Ratio for the current main video surface
float ddgr_os_surf_GetAspectRatio(void);
