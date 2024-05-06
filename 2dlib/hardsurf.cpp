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

 * $Logfile: /descent3/main/2dlib/hardsurf.cpp $
 * $Revision: 8 $
 * $Date: 6/12/97 6:28p $
 * $Author: Samir $
 *
 * hardware surface class
 *
 * $Log: /descent3/main/2dlib/hardsurf.cpp $
 *
 * 8     6/12/97 6:28p Samir
 * DDGR v2.0 Changes in 2d system implemeted.
 *
 * 7     1/30/97 6:01p Samir
 * The partition of memory and os surfaces as well as the change in
 * ddgr_surface structure and all related changes.
 *
 * $NoKeywords: $
 */

#include "gr.h"

//	----------------------------------------------------------------------------
//	constructor and destructor
//	----------------------------------------------------------------------------

grHardwareSurface::grHardwareSurface() : grSurface() {}

grHardwareSurface::grHardwareSurface(int w, int h, int bpp, unsigned flags, const char *name)
    : grSurface(w, h, bpp, SURFTYPE_GENERIC, flags, name) {}

grHardwareSurface::~grHardwareSurface() {}

//	----------------------------------------------------------------------------
//	initialize a hardware surface with these values
//	----------------------------------------------------------------------------

bool grHardwareSurface::create(int w, int h, int bpp, unsigned flags, const char *name) {
  grSurface::create(w, h, bpp, SURFTYPE_GENERIC, flags, name);
  return 1;
}
