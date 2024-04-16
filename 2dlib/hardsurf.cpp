/*
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
