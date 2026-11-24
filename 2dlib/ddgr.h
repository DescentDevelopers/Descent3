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

 * $Logfile: /DescentIII/Main/lib/Ddgr.h $
 * $Revision: 6 $
 * $Date: 12/23/97 6:16p $
 * $Author: Samir $
 *
 * DDGR library interface header
 *
 * $Log: /DescentIII/Main/lib/Ddgr.h $
 *
 * 6     12/23/97 6:16p Samir
 * Moved ddgr_Close to header.
 *
 * 5     12/22/97 7:24p Samir
 * took out transparent color defs.
 *
 * 4     12/22/97 7:13p Samir
 * Moved constants to grdefs.h
 *
 * 3     9/12/97 4:13p Samir
 * Added some private data access functions.
 *
 * 1     6/23/97 9:25p Samir
 * added because source safe sucks
 *
 * 26    6/12/97 6:30p Samir
 * DDGR v2.0 Changes in 2d system implemented.
 *
 * 25    6/11/97 1:07p Samir
 * The removal of gameos and replaced with oeApplication, oeDatabase
 *
 * 24    5/22/97 4:03p Samir
 * Hacked OpenGL stuff.  WILL CHANGE
 *
 * 23    5/15/97 2:08p Samir
 * Added 8-bit support and driver info structure.
 *
 * 22    3/28/97 3:07p Samir
 * Clear function now takes a color
 *
 * 21    3/27/97 11:11a Samir
 * Revised headers to reflect movement of functions from ddgr to 2dlib
 *
 * 20    3/26/97 7:44p Samir
 * Took out prototypes to unimplemented functions.
 *
 * 19    2/27/97 6:16p Samir
 * moved high level functions to 2dlib internal library
 *
 * 18    2/03/97 6:00p Jason
 * added 2d bitmap scaling functions
 *
 * 17    1/30/97 6:01p Samir
 * The partition of memory and os surfaces as well as the change in
 * ddgr_surface structure and all related changes.
 *
 * $NoKeywords: $
 */

#pragma once

#include "gr.h"
#include "grdefs.h"

//	----------------------------------------------------------------------------
//	DDGR
//		version 1.0						// DirectDraw, GDI support
//		version 2.x						// fullscreen-windowed remanagement.
//	----------------------------------------------------------------------------

//	----------------------------------------------------------------------------
//	Data structures
//	----------------------------------------------------------------------------

/*	Type definitions */

class oeApplication;

/*
        Diagnostic DDGR system types and functions
*/

struct ddgr_init_info {
  oeApplication *obj;  // the app object created by app calling
  char *subsystem;     // subsystem name (i.e. 'DirectDraw', 'GDI')
  uint16_t windowed : 1; // are we running in a fullscreen or windowed mode
  uint16_t debug : 1;    // are we running in debug mode?
};

/*	app = application object.
        subsystem = subsystem name ('DirectDraw', 'GDI')
        fullscreen = whether it's full screen or windowed
*/
bool ddgr_Init(oeApplication *app, const char *subsystem, bool fullscreen);
void ddgr_Close();
void ddgr_GetSubsystem(char *name, bool *fullscreen);

//	returns aspect ratio of current display.
float ddgr_GetAspectRatio();

/*	Surface Functions
 */

/*	input:
                sf->name is optional.
                sf->w, sf->h, sf->bpp are mandatory
                sf->type and sf->flags are mandatory.
        output:
                sf->obj = surface object.
                sf->locks = 0
*/
bool ddgr_surf_Create(ddgr_surface *sf);
void ddgr_surf_Destroy(ddgr_surface *sf);

/*	retrieves a pointer to surface memory.  allowed to lock one surface multiple times.
                ptr is the returned pointer to surface memory.  used to unlock surface also
                rowsize is the size in bytes of one row of memory.
*/
bool ddgr_surf_Lock(ddgr_surface *sf, void **ptr, int *rowsize);
bool ddgr_surf_Unlock(ddgr_surface *sf, void *ptr);

//	attaches an OS handle to a surface
void ddgr_surf_AttachHandle(ddgr_surface *sf, unsigned handle);

/*	initializes a video screen from the given input surface which specifies the dimensions
        of the screen.
                sf->name is optional
                sf->w, sf->h are mandatory
                sf->bpp is recommended.  if set to BPP_DEFAULT, it uses the current display bit depth
                sf->type = SURFTYPE_VIDEOSCREEN
                sf->flags = 0 or SURFFLAG_BACKBUFFER.  surfaces with backbuffers allow rendering to
                                                them and display via flip.  monopage surfaces don't have this luxury.
*/
bool ddgr_surf_InitVideo(ddgr_surface *sf);

/*	close video reverses the operation of init video for that surface.  the display should stay
        the same, but no more operations may occur to that surface through this library.
*/
void ddgr_surf_CloseVideo(ddgr_surface *sf);

/*	flips the buffers in a surface.  really only useful for video screens
 */
bool ddgr_surf_FlipVideo(ddgr_surface *sf);

/*	graphic primitives
 */
void ddgr_surf_Clear(ddgr_surface *dsf, ddgr_color col, int l, int t, int w, int h);
bool ddgr_surf_Blt(ddgr_surface *dsf, int dx, int dy, ddgr_surface *ssf, int sx, int sy, int sw, int sh);
