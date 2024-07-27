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

 * $Logfile: /descent3/main/2dlib/screen.cpp $
 * $Revision: 12 $
 * $Date: 6/16/97 5:16p $
 * $Author: Jason $
 *
 * Screen creation class
 *
 * $Log: /descent3/main/2dlib/screen.cpp $
 *
 * 12    6/16/97 5:16p Jason
 * added renderer.h header
 *
 * 11    6/16/97 4:52p Samir
 * If a renderer, then call rendflip instead.
 *
 * 10    6/13/97 3:00p Samir
 * Only flip if screen has a backbuffer.
 *
 * 9     6/12/97 6:29p Samir
 * DDGR v2.0 Changes in 2d system implemented.
 *
 * 8     1/30/97 6:01p Samir
 * The partition of memory and os surfaces as well as the change in
 * ddgr_surface structure and all related changes.
 *
 * $NoKeywords: $
 */

#include "gr.h"
#include "pserror.h"
#include "renderer.h"

//	---------------------------------------------------------------------------
//	grSurface constructor and destructor
//	---------------------------------------------------------------------------

grScreen::grScreen(int w, int h, int bpp, const char *name)
    : grSurface(w, h, bpp, SURFTYPE_VIDEOSCREEN, SURFFLAG_BACKBUFFER, name) {}

grScreen::~grScreen() {}

//	---------------------------------------------------------------------------
//	screen refresh routines
//	---------------------------------------------------------------------------

void grScreen::flip() {
  ASSERT(surf_init());
  if (ddsfObj.flags & SURFFLAG_BACKBUFFER)
    ddgr_surf_FlipVideo(&ddsfObj);
  else if (ddsfObj.flags & SURFFLAG_RENDERER)
    rend_Flip();
}
