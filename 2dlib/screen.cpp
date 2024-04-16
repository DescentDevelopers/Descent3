/*
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


#include <string.h>

#include "gr.h"
#include "pserror.h"
#include "renderer.h"


//	---------------------------------------------------------------------------
//	grSurface constructor and destructor
//	---------------------------------------------------------------------------

grScreen::grScreen(int w, int h, int bpp, const char *name)
	:grSurface(w,h,bpp,SURFTYPE_VIDEOSCREEN, SURFFLAG_BACKBUFFER, name)
{

}		


grScreen::~grScreen()
{

}


//	---------------------------------------------------------------------------
//	screen refresh routines
//	---------------------------------------------------------------------------


void grScreen::flip()
{
	ASSERT(surf_init());
	if (ddsfObj.flags & SURFFLAG_BACKBUFFER) ddgr_surf_FlipVideo(&ddsfObj);
	else if (ddsfObj.flags & SURFFLAG_RENDERER) rend_Flip();
}
