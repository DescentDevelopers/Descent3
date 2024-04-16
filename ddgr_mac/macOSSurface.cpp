/*
 * $Logfile: /Descent3/main/ddgr_mac/macOSSurface.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:56:53 $
 * $Author: kevinb $
 *
 * macintosh implementation of ddgr_surfaces
 *
 * $Log: macOSSurface.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:56:53  kevinb
 * initial 1.5 import
 *
 *
 * 10    5/21/97 5:09 PM Jeremy
 * made a quick hack to convert the contents of the frame buffer from
 * 5-6-5 format to 1-5-5-5 format pixel data right before the video flip.
 * use the #define USE_COLOR_CONVERSION_HACK to turn this on or off (see
 * flipVideo for details)
 *
 * 9     5/20/97 11:38 PM Jeremy
 * temporarily removed the mprintf's in os_surf_clear
 *
 * 8     5/19/97 7:08 PM Jeremy
 * removed extra gamma fade in of monitor when closing down video
 *
 * 7     5/17/97 6:50 PM Jeremy
 * implemented clearing function
 *
 * 6     5/15/97 1:47 AM Jeremy
 * changed mprintf's to be standard (with newline at end), also fixed a
 * potential bug where a variable was declared twice in a function (blit)
 * but in different scopes
 *
 * 5     5/13/97 11:15 AM Jeremy
 * removed bug in initialization code (was checking an extra variable
 * m_initted instead of m_video_initted)
 *
 * 4     5/11/97 8:00 PM Jeremy
 * implemented ddgr_os_surf_GetAspectRatio
 *
 * 3     5/9/97 7:13 PM Jeremy
 * implemented blitting (still need to implement clearing)
 *
 * 2     4/15/97 7:01 PM Jeremy
 * initial implementation of fullscreen (via DrawSprocket) os_surfaces.
 * right now only initialization and closing of surfaces/video is
 * implemented.  still need to implement surface
 * creation/deletion/blitting.
 *
 * 1     4/9/97 7:16 PM Jeremy
 * initial check in
 *
 * $NoKeywords: $
 */

//#define check

//	---------------------------------------------------------------------------
//	Macintosh Headers
//	---------------------------------------------------------------------------
#include <DrawSprocket.h>
#include <QuickDraw.h>

//	---------------------------------------------------------------------------
//	Descent3 Headers
//	---------------------------------------------------------------------------
#include "pserror.h"

#include "gr.h"
#include "ddgr_mac.h"
#include "macOSSurface.h"

//	---------------------------------------------------------------------------
//	Local File Types
//	---------------------------------------------------------------------------

//	The FullScreenData class is intended to encapsulate the
//	data necessary for interacting with DrawSprocket
//	and some utility functions for dealing with full screen info
//	into one structure
class CFullScreenDataObj {
public:
  CFullScreenDataObj(void);
  ~CFullScreenDataObj(void);

  bool m_video_initted;
  DSpContextAttributes m_screen_attr;
  DSpContextReference m_context;
  DSpAltBufferReference m_underlay;
  GDHandle m_save_gdevice;
  GrafPtr m_save_port;
};

// This is the data type of the object pointed to in the surfaces obj ptr
//	for os_surfaces
class CMacOffscreenDataObj {
public:
  CMacOffscreenDataObj(void) { m_GWorld = nil; }

  GWorldPtr m_GWorld;
};

// The Error class is used for passing error information around
//  It is internal to this file
class CMacOSSurfErr {
public:
  CMacOSSurfErr(char *inErrStr, OSErr inMacErr, ddgr_error inDDGRErr) {
    m_ErrStr = inErrStr;
    m_MacErr = inMacErr;
    m_DDGRErr = inDDGRErr;
  }

  char *m_ErrStr;
  OSErr m_MacErr;
  ddgr_error m_DDGRErr;
};

//	---------------------------------------------------------------------------
//	File Level Globals
//	---------------------------------------------------------------------------
static CFullScreenDataObj gFullScreenData;

//	------------------------------------------------------------
//	Implementation of the CFullScreenDataObj class
//	It is intended that an instance of this object class
//	be around for the whole game if full screen mode is used.
//	------------------------------------------------------------
CFullScreenDataObj::CFullScreenDataObj(void) {
  // Initialize the data members
  m_video_initted = false;

  memset(&m_screen_attr, 0, sizeof(m_screen_attr));

  m_context = nil;
  m_underlay = nil;
  m_save_gdevice = nil;
  m_save_port = nil;
}

CFullScreenDataObj::~CFullScreenDataObj(void) {
  // Release the contexts if they were allocated?
}

//	---------------------------------------------------------------------------
//	ddgr_os_surf_InitVideo

ddgr_error ddgr_os_surf_InitVideo(ddgr_surface *sf) {
  ddgr_error err = DDGRERR_SUCCESS;
  OSErr macErr = noErr;

  ASSERT(Mac_DDGR_Lib.IsInitted());
  ASSERT(sf != NULL);
  ASSERT(sf->type == SURFTYPE_VIDEOSCREEN);
  ASSERT(!Mac_DDGR_Lib.IsWindowed());

  mprintf((0, "Initializing video.\n"));

  bool userCanSelect = false;

  // ¥ Initialize the desired screen attributes structure
  gFullScreenData.m_screen_attr.displayWidth = sf->w;
  gFullScreenData.m_screen_attr.displayHeight = sf->h;
  gFullScreenData.m_screen_attr.colorNeeds = kDSpColorNeeds_Require;
  gFullScreenData.m_screen_attr.backBufferDepthMask = sf->bpp;
  gFullScreenData.m_screen_attr.displayDepthMask = sf->bpp;
  gFullScreenData.m_screen_attr.backBufferBestDepth = sf->bpp;
  gFullScreenData.m_screen_attr.displayBestDepth = sf->bpp;
  gFullScreenData.m_screen_attr.pageCount = (sf->flags & SURFFLAG_BACKBUFFER) ? 2 : 1;

  try {
    // ¥ Check whether multiple monitors are available
    macErr = DSpCanUserSelectContext(&(gFullScreenData.m_screen_attr), &userCanSelect);
    if (macErr)
      throw(CMacOSSurfErr("Error checking if user could select from different monitors.", macErr, DDGRERR_DRIVERINIT));

    if (userCanSelect) {
      // ¥ Allow user to select a monitor from those available
      macErr = DSpUserSelectContext(&(gFullScreenData.m_screen_attr), 0, nil, &(gFullScreenData.m_context));
      if (macErr)
        throw(CMacOSSurfErr("Error while allowing user to choose a monitor.", macErr, DDGRERR_DRIVERINIT));
    } else {
      // ¥ Autoselect the best monitor/context
      macErr = DSpFindBestContext(&(gFullScreenData.m_screen_attr), &(gFullScreenData.m_context));
      if (macErr)
        throw(CMacOSSurfErr("Error while autoselecting a monitor.", macErr, DDGRERR_DRIVERINIT));
    }

    // ¥ Request hardware pageflipping if it is available when reserving the context
    gFullScreenData.m_screen_attr.contextOptions |= kDSpContextOption_PageFlip;

    // ¥ Reserve the context (take over the monitor)
    macErr = DSpContext_Reserve(gFullScreenData.m_context, &(gFullScreenData.m_screen_attr));
    if (macErr)
      throw(CMacOSSurfErr("Error while reserving the draw sprocket context.", macErr, DDGRERR_DRIVERINIT));

    // ¥ Fade out the monitor while resolution switching
    macErr = DSpContext_FadeGammaOut(gFullScreenData.m_context, NULL);
    if (macErr)
      throw(CMacOSSurfErr("Error while fading out monitor.", macErr, DDGRERR_DRIVERINIT));

    // ¥ Activate the context (do the resolution/depth switch)
    macErr = DSpContext_SetState(gFullScreenData.m_context, kDSpContextState_Active);
    if (macErr)
      throw(CMacOSSurfErr("Error while activating context", macErr, DDGRERR_DRIVERINIT));

    // ¥ Fade the monitor back in at the new resolution/depth
    macErr = DSpContext_FadeGammaIn(gFullScreenData.m_context, NULL);
    if (macErr)
      throw(CMacOSSurfErr("Error while fading in monitor ", macErr, DDGRERR_DRIVERINIT));

    //¥ All's well!
    gFullScreenData.m_video_initted = true;
  } catch (CMacOSSurfErr theCaughtError) {
    mprintf((0, theCaughtError.m_ErrStr));
    mprintf((0, "DSp MacOS Error: %d\n", theCaughtError.m_MacErr));
    if (gFullScreenData.m_context) {
      //¥ The monitor might be faded out so fade it back in and release the context
      DSpContext_FadeGammaIn(gFullScreenData.m_context, NULL);
      DSpContext_Release(gFullScreenData.m_context);
      gFullScreenData.m_context = nil;
    }
    err = theCaughtError.m_DDGRErr;
  }

  return err;
}

//	---------------------------------------------------------------------------
//	ddgr_os_surf_CloseVideo
//		will kill all objects created in os_surf_InitVideo

void ddgr_os_surf_CloseVideo(ddgr_surface *sf) {
  ASSERT(Mac_DDGR_Lib.IsInitted());
  ASSERT(sf != NULL);
  ASSERT(sf->type == SURFTYPE_VIDEOSCREEN);
  ASSERT(sf->locks == 0);
  ASSERT(!Mac_DDGR_Lib.IsWindowed());

  OSErr macErr = noErr;

  mprintf((0, "Closing video.\n"));

  try {
    // ¥ Fade out the monitor while resolution switching
    macErr = DSpContext_FadeGammaOut(gFullScreenData.m_context, NULL);
    if (macErr)
      throw(CMacOSSurfErr("Error while fading out monitor.", macErr, DDGRERR_DRIVERINIT));

    // ¥ Deactivate the context (undo the resolution/depth switch)
    macErr = DSpContext_SetState(gFullScreenData.m_context, kDSpContextState_Inactive);
    if (macErr)
      throw(CMacOSSurfErr("Error while de-activating context", macErr, DDGRERR_DRIVERINIT));

  } catch (CMacOSSurfErr theCaughtError) {
    mprintf((0, "Error while closing video!\n"));
    mprintf((0, theCaughtError.m_ErrStr));
    mprintf((0, "\n"));
    mprintf((0, "---> DSp MacOS Error: %d\n", theCaughtError.m_MacErr));
  }

  if (gFullScreenData.m_context) {
    //¥ The monitor might be faded out so fade it back in and release the context
    DSpContext_FadeGammaIn(gFullScreenData.m_context, NULL);
    DSpContext_Release(gFullScreenData.m_context);
    gFullScreenData.m_context = nil;
  }

  //¥ Video is deinitialized!
  gFullScreenData.m_video_initted = false;
}

//	---------------------------------------------------------------------------
//	ddgr_os_surf_FlipVideo
//		if this surface supports page flipping, will page flip.  Otherwise, we
//		flag an error.

// This allows the use of the (costly) color conversion to get 5-6-5 ---> 1-5-5-5
//	but it only works when you are redrawing every pixel on the screen because
//	it steps through the frame buffer and converts each pixel
#define USE_COLOR_CONVERSION_HACK

ddgr_error ddgr_os_surf_FlipVideo(ddgr_surface *sf) {
  ASSERT(Mac_DDGR_Lib.IsInitted());
  ASSERT(!Mac_DDGR_Lib.IsWindowed());
  ASSERT(sf != NULL);
  ASSERT(sf->type == SURFTYPE_VIDEOSCREEN);
  ASSERT(sf->locks == 0);
  ASSERT(!Mac_DDGR_Lib.IsWindowed());

  //	mprintf((0, "Clearing OS Surface!\n"));

  ddgr_error err = DDGRERR_SUCCESS;

  CGrafPtr macBackBufferPtr = NULL;
  PixMapHandle dstPixMapH = NULL;
  CGrafPtr savePort = NULL;
  GDHandle saveGDevice = NULL;
  OSErr macErr = noErr;

  GetGWorld(&savePort, &saveGDevice);

  try {
    if (!(sf->flags & SURFFLAG_BACKBUFFER)) {
      throw(CMacOSSurfErr("Tried to swap surface with no backbuffer.\n", noErr, DDGRERR_FAIL));
    }

#ifdef USE_COLOR_CONVERSION_HACK

    macErr = DSpContext_GetBackBuffer(gFullScreenData.m_context, kDSpBufferKind_Normal, &macBackBufferPtr);
    if (macErr || !macBackBufferPtr) {
      throw(CMacOSSurfErr("Could not obtain full screen surface backbuffer ptr!", macErr, DDGRERR_FAIL));
    }

    dstPixMapH = macBackBufferPtr->portPixMap;
    SetGWorld(macBackBufferPtr, nil);

    if (!dstPixMapH) {
      throw(CMacOSSurfErr("Could not obtain full screen pixmaps for clearing os surface", macErr, DDGRERR_FAIL));
    }

    bool lockSuccess = false;
    lockSuccess = LockPixels(dstPixMapH);
    if (!lockSuccess) {
      throw(CMacOSSurfErr("Error: Could not lock destination pixels for flipping!", 0, DDGRERR_FAIL));
    }

    PixMapPtr macPixMapPtr = *dstPixMapH;

    // For now assume, 16 bpp
    ASSERT(sf->bpp == BPP_16);

    //!! Do the Conversion
    int i = 0;
    int numRows = macPixMapPtr->bounds.bottom - macPixMapPtr->bounds.top;
    int numCols = macPixMapPtr->bounds.right - macPixMapPtr->bounds.left;
    int numTimes = numRows * numCols;
    ushort oldColor = 0;
    ushort newColor = 0;
    ushort r = 0;
    ushort g = 0;
    ushort b = 0;

    ushort *base = (ushort *)macPixMapPtr->baseAddr;

    for (i = 0; i < numTimes; i++) {
      oldColor = base[i];

      r = (oldColor & 0xF800) >> 11;
      g = (oldColor & 0x07E0) >> 6;
      b = (oldColor & 0x001F);

      newColor = 0;
      newColor |= (r << 10);
      newColor |= (g << 5);
      newColor |= b;

      base[i] = newColor;
    }

    UnlockPixels(dstPixMapH);

#endif // USE_COLOR_CONVERSION_HACK

    // swap the buffers
    macErr = DSpContext_SwapBuffers(gFullScreenData.m_context, NULL, 0);
    if (macErr) {
      throw(CMacOSSurfErr("DSpContext_SwapBuffers returned error: %d\n", macErr, DDGRERR_FAIL));
    }
  } catch (CMacOSSurfErr errObj) {
    mprintf((0, errObj.m_ErrStr));
    mprintf((0, "\n"));
    mprintf((0, "MacOS Error: %d\n", errObj.m_MacErr));
    err = errObj.m_DDGRErr;
  }

  SetGWorld(savePort, saveGDevice);
  return err;
}

//	---------------------------------------------------------------------------
//	ddgr_os_surf_Create
//		create a surface in we are in fullscreen mode

ddgr_error ddgr_os_surf_Create(ddgr_surface *sf) {
  ddgr_error err = DDGRERR_SUCCESS;
  OSErr macErr = noErr;

  ASSERT(Mac_DDGR_Lib.IsInitted());
  ASSERT(!Mac_DDGR_Lib.IsWindowed());
  ASSERT(sf != NULL);
  ASSERT(sf->type == SURFTYPE_OFFSCREEN_OS);

  mprintf((0, "Creating OS Surface!\n"));

  try {
    // Allocate a mac os surface data object
    CMacOffscreenDataObj *surfDataObj = nil;
    surfDataObj = new CMacOffscreenDataObj;
    if (surfDataObj == nil) {
      throw(CMacOSSurfErr("Error allocating memory for mac os surface data object!", noErr, DDGRERR_OUTOFMEMORY));
    }

    // Store the reference to the new data object for use later
    sf->obj = surfDataObj;

    // Allocate a new offscreen graphics world (cgrafptr)
    GWorldPtr newOffscreen = nil;
    Rect surfBoundsRect = {0, 0, 0, 0};

    surfBoundsRect.top = 0;
    surfBoundsRect.left = 0;
    surfBoundsRect.right = surfBoundsRect.left + sf->w;
    surfBoundsRect.bottom = surfBoundsRect.top + sf->h;

    mprintf((0, "OS SURFACE CREATE: surface bounds rect = %d, %d, %d, %d (t,l,b,r).  Is this correct?\n",
             surfBoundsRect.top, surfBoundsRect.left, surfBoundsRect.right, surfBoundsRect.bottom));

    macErr = NewGWorld(&newOffscreen, sf->bpp, &surfBoundsRect, nil, nil, 0);
    if (macErr) {
      throw(CMacOSSurfErr("Error allocating memory for mac offscreen buffer!", noErr, DDGRERR_OUTOFMEMORY));
    }

    // Store the new offscreen gworld for use later (blitting and such)
    ((CMacOffscreenDataObj *)sf->obj)->m_GWorld = newOffscreen;

    // Reset unused data members
    sf->locks = 0;
    sf->rowsize = 0;
  } catch (CMacOSSurfErr errObj) {
    mprintf((0, errObj.m_ErrStr));
    mprintf((0, "\n"));
    mprintf((0, "MacOS Error: %d\n", errObj.m_MacErr));
    err = errObj.m_DDGRErr;
  }

  return err;
}

//	---------------------------------------------------------------------------
//	ddgr_os_surf_Destroy

void ddgr_os_surf_Destroy(ddgr_surface *sf) {
  OSErr macErr = noErr;

  ASSERT(Mac_DDGR_Lib.IsInitted());
  ASSERT(!Mac_DDGR_Lib.IsWindowed());
  ASSERT(sf != NULL);
  ASSERT(sf->type == SURFTYPE_OFFSCREEN_OS);
  ASSERT(sf->locks == 0);

  mprintf((0, "Deleting OS Surface!\n"));

  try {
    // Get the mac os surface data object
    CMacOffscreenDataObj *surfDataObj = nil;

    surfDataObj = (CMacOffscreenDataObj *)sf->obj;
    if (surfDataObj == nil) {
      throw(CMacOSSurfErr("Error:  No data object associated with offscreen surface!", 0, DDGRERR_FAIL));
    }

    // Free the associated new offscreen graphics world (cgrafptr)
    DisposeGWorld(surfDataObj->m_GWorld);

    // Clear the data member
    surfDataObj->m_GWorld = nil;

    // Dispose of the surface data object
    delete surfDataObj;

    // Clear the surface's data member
    sf->obj = nil;
  } catch (CMacOSSurfErr errObj) {
    mprintf((0, "Could not destroy surface!\n"));
    mprintf((0, errObj.m_ErrStr));
    mprintf((0, "\n"));
    mprintf((0, "MacOS Error: %d\n", errObj.m_MacErr));
  }
}

//	---------------------------------------------------------------------------
//	ddgr_os_surf_Clear
//		clears  a surface based on the given rectangle in left, top, width, height

ddgr_error ddgr_os_surf_Clear(ddgr_surface *dsf, ddgr_color col, int l, int t, int w, int h) {
  ASSERT(Mac_DDGR_Lib.IsInitted());
  ASSERT(!Mac_DDGR_Lib.IsWindowed());
  ASSERT(dsf != NULL);
  ASSERT(dsf->type == SURFTYPE_OFFSCREEN_OS || dsf->type == SURFTYPE_VIDEOSCREEN);
  ASSERT(dsf->locks == 0);
  ASSERT(!Mac_DDGR_Lib.IsWindowed());

  //	mprintf((0, "Clearing OS Surface!\n"));

  ddgr_error err = DDGRERR_SUCCESS;

  CGrafPtr macBackBufferPtr = NULL;
  PixMapHandle dstPixMapH = NULL;
  CGrafPtr savePort = NULL;
  GDHandle saveGDevice = NULL;
  OSErr macErr = noErr;

  GetGWorld(&savePort, &saveGDevice);

  try {
    if (dsf->type == SURFTYPE_OFFSCREEN_OS) {
      CMacOffscreenDataObj *dataObj = nil;
      dataObj = (CMacOffscreenDataObj *)dsf->obj;
      if (dataObj == nil) {
        throw(CMacOSSurfErr("Error:  No data object associated with offscreen surface!", 0, DDGRERR_FAIL));
      }

      dstPixMapH = GetGWorldPixMap(dataObj->m_GWorld);
      SetGWorld(dataObj->m_GWorld, nil);
    } else // FullScreen
    {
      macErr = DSpContext_GetBackBuffer(gFullScreenData.m_context, kDSpBufferKind_Normal, &macBackBufferPtr);
      if (macErr || !macBackBufferPtr) {
        throw(CMacOSSurfErr("Could not obtain full screen surface backbuffer ptr!", macErr, DDGRERR_FAIL));
      }

      dstPixMapH = macBackBufferPtr->portPixMap;
      SetGWorld(macBackBufferPtr, nil);
    }

    if (!dstPixMapH) {
      throw(CMacOSSurfErr("Could not obtain full screen pixmaps for clearing os surface", macErr, DDGRERR_FAIL));
    }

    bool lockSuccess = false;
    lockSuccess = LockPixels(dstPixMapH);
    if (!lockSuccess) {
      throw(CMacOSSurfErr("Error: Could not lock destination pixels for blit!", 0, DDGRERR_FAIL));
    }

    PixMapPtr macPixMapPtr = *dstPixMapH;

    //!! Do the Clear
    int i = 0;
    int j = 0;
    int bitsPerPixel = macPixMapPtr->pixelSize;

    ASSERT((bitsPerPixel == BPP_DEFAULT) || (bitsPerPixel == BPP_8) || (bitsPerPixel == BPP_16) ||
           (bitsPerPixel == BPP_24) || (bitsPerPixel == BPP_32));

    RGBColor clearColor = {0, 0, 0};
    RGBColor saveColor = {0, 0, 0};
    Rect clearRect = {0, 0, 0, 0};

    ::GetForeColor(&saveColor);

    // Set up the color
    clearColor.red = GR_COLOR_RED(col);
    clearColor.green = GR_COLOR_GREEN(col);
    clearColor.blue = GR_COLOR_BLUE(col);

    // Set up the bounding rect
    clearRect.top = t;
    clearRect.bottom = t + h;
    clearRect.left = l;
    clearRect.right = l + w;

    // Do the clear
    //			mprintf((0, "Clearing OS Surface to ddgr_color = %d, RGB = (%d, %d, %d)\n",
    //					col, clearColor.red, clearColor.green, clearColor.blue));

    ::RGBForeColor(&clearColor);
    ::PaintRect(&clearRect);

    ::RGBForeColor(&saveColor);

    UnlockPixels(dstPixMapH);
  } catch (CMacOSSurfErr errObj) {
    mprintf((0, errObj.m_ErrStr));
    mprintf((0, "\n"));
    mprintf((0, "MacOS Error: %d\n", errObj.m_MacErr));
    err = errObj.m_DDGRErr;
  }

  SetGWorld(savePort, saveGDevice);
  return err;
}

//	---------------------------------------------------------------------------
//	ddgr_os_surf_Blt
//		blts a non-scaled bitmap using windowed or fullscreen modes
//		note that we DONT have to handle fullscreen->windowed or vice-versa,
//		since they will never coexist

ddgr_error ddgr_os_surf_Blt(ddgr_surface *dsf, int dx, int dy, ddgr_surface *ssf, int sx, int sy, int sw, int sh) {
  ASSERT(Mac_DDGR_Lib.IsInitted());
  ASSERT(dsf != NULL && ssf != NULL);
  ASSERT(dsf->type == SURFTYPE_VIDEOSCREEN || dsf->type == SURFTYPE_OFFSCREEN_OS);
  ASSERT(ssf->type == SURFTYPE_VIDEOSCREEN || ssf->type == SURFTYPE_OFFSCREEN_OS);
  ASSERT(dsf->locks == 0);
  ASSERT(ssf->locks == 0);

  ASSERT(!Mac_DDGR_Lib.IsWindowed());

  ddgr_error err = DDGRERR_SUCCESS;

  CGrafPtr macBackBufferPtr = NULL;
  PixMapHandle dstPixMapH = NULL;
  PixMapHandle srcPixMapH = NULL;
  CGrafPtr savePort = NULL;
  GDHandle saveGDevice = NULL;
  OSErr macErr = noErr;

  GetGWorld(&savePort, &saveGDevice);

  try {
    if (dsf->type == SURFTYPE_OFFSCREEN_OS) {
      CMacOffscreenDataObj *dataObj = nil;
      dataObj = (CMacOffscreenDataObj *)dsf->obj;
      if (dataObj == nil) {
        throw(CMacOSSurfErr("Error:  No data object associated with offscreen surface!", 0, DDGRERR_FAIL));
      }

      dstPixMapH = GetGWorldPixMap(dataObj->m_GWorld);
      SetGWorld(dataObj->m_GWorld, nil);
    } else // FullScreen
    {
      macErr = DSpContext_GetBackBuffer(gFullScreenData.m_context, kDSpBufferKind_Normal, &macBackBufferPtr);
      if (macErr || !macBackBufferPtr) {
        throw(CMacOSSurfErr("Could not obtain full screen surface backbuffer ptr!", macErr, DDGRERR_FAIL));
      }

      dstPixMapH = macBackBufferPtr->portPixMap;
      SetGWorld(macBackBufferPtr, nil);
    }

    if (ssf->type == SURFTYPE_OFFSCREEN_OS) {
      CMacOffscreenDataObj *dataObj = nil;
      dataObj = (CMacOffscreenDataObj *)ssf->obj;
      if (dataObj == nil) {
        throw(CMacOSSurfErr("Error:  No data object associated with offscreen surface!", 0, DDGRERR_FAIL));
      }

      srcPixMapH = GetGWorldPixMap(dataObj->m_GWorld);
    } else // FullScreen
    {
      macErr = DSpContext_GetBackBuffer(gFullScreenData.m_context, kDSpBufferKind_Normal, &macBackBufferPtr);
      if (macErr || !macBackBufferPtr) {
        throw(CMacOSSurfErr("Could not obtain full screen surface backbuffer ptr!", macErr, DDGRERR_FAIL));
      }

      srcPixMapH = macBackBufferPtr->portPixMap;
    }

    if (!srcPixMapH || !dstPixMapH) {
      throw(CMacOSSurfErr("Could not obtain full screen pixmaps for blitting os surface", macErr, DDGRERR_FAIL));
    }

    bool lockSuccess = false;
    lockSuccess = LockPixels(srcPixMapH);
    if (!lockSuccess) {
      throw(CMacOSSurfErr("Error: Could not lock source pixels for blit!", 0, DDGRERR_FAIL));
    }

    lockSuccess = LockPixels(dstPixMapH);
    if (!lockSuccess) {
      throw(CMacOSSurfErr("Error: Could not lock destination pixels for blit!", 0, DDGRERR_FAIL));
    }

    // Source and dest rectangles are the same (no scaling)
    Rect srcRect = {sy, sx, sx + sw, sy + sh};
    Rect dstRect = {dy, dx, dx + sw, dy + sh};

    // DO THE BLIT!
    CopyBits((BitMap *)(*srcPixMapH), (BitMap *)(*dstPixMapH), &srcRect, &dstRect, srcCopy, nil);

    UnlockPixels(srcPixMapH);
    UnlockPixels(dstPixMapH);

  } catch (CMacOSSurfErr errObj) {
    mprintf((0, errObj.m_ErrStr));
    mprintf((0, "\n"));
    mprintf((0, "MacOS Error: %d\n", errObj.m_MacErr));
    err = errObj.m_DDGRErr;
  }

  SetGWorld(savePort, saveGDevice);
  return err;
}

//	---------------------------------------------------------------------------
//	ddgr_os_surf_Lock and Unlock
//		performs a lock which retrieves the data pointer and rowsize of a surface
//		for direct rendering.

ddgr_error ddgr_os_surf_Lock(ddgr_surface *sf) {
  ddgr_error err = DDGRERR_SUCCESS;
  OSErr macErr = noErr;

  ASSERT(Mac_DDGR_Lib.IsInitted());
  ASSERT(!Mac_DDGR_Lib.IsWindowed());
  ASSERT(sf != NULL);
  ASSERT(sf->type == SURFTYPE_OFFSCREEN_OS || sf->type == SURFTYPE_VIDEOSCREEN);

  CGrafPtr macBackBufferPtr = NULL;
  PixMapHandle macPixMapH = NULL;

  try {
    if (sf->type == SURFTYPE_OFFSCREEN_OS) {
      CMacOffscreenDataObj *dataObj = nil;
      dataObj = (CMacOffscreenDataObj *)sf->obj;
      if (dataObj == nil) {
        throw(CMacOSSurfErr("Error:  No data object associated with offscreen surface!", 0, DDGRERR_FAIL));
      }

      macPixMapH = GetGWorldPixMap(dataObj->m_GWorld);
    } else if (sf->type == SURFTYPE_VIDEOSCREEN) {
      macErr = DSpContext_GetBackBuffer(gFullScreenData.m_context, kDSpBufferKind_Normal, &macBackBufferPtr);
      if (macErr) {
        throw(CMacOSSurfErr("Could not obtain full screen surface backbuffer ptr!", macErr, DDGRERR_FAIL));
      }

      macPixMapH = macBackBufferPtr->portPixMap;
    } else {
      throw(CMacOSSurfErr("Attempted to lock surface with invalid surftype!", 0, DDGRERR_FAIL));
    }

    Boolean lockSuccess = false;
    lockSuccess = LockPixels(macPixMapH);
    if (!lockSuccess) {
      throw(CMacOSSurfErr("Error: Could not lock pixel data!", noErr, DDGRERR_FAIL));
    }

    PixMapPtr macPixMapPtr = *macPixMapH;

    sf->w = macPixMapPtr->bounds.right - macPixMapPtr->bounds.left;
    sf->h = macPixMapPtr->bounds.bottom - macPixMapPtr->bounds.top;
    sf->bpp = macPixMapPtr->pixelSize;
    sf->rowsize = (macPixMapPtr->rowBytes) & 0x7FFF;

    sf->data = GetPixBaseAddr(macPixMapH);

    sf->locks++;
  } catch (CMacOSSurfErr errObj) {
    mprintf((0, errObj.m_ErrStr));
    mprintf((0, "\n"));
    mprintf((0, "MacOS Error: %d\n", errObj.m_MacErr));
    err = errObj.m_DDGRErr;
  }

  return err;
}

ddgr_error ddgr_os_surf_Unlock(ddgr_surface *sf) {
  ASSERT(Mac_DDGR_Lib.IsInitted());
  ASSERT(!Mac_DDGR_Lib.IsWindowed());
  ASSERT(sf != NULL);
  ASSERT(sf->type == SURFTYPE_OFFSCREEN_OS || sf->type == SURFTYPE_VIDEOSCREEN);
  ASSERT(sf->locks);

  ddgr_error err = DDGRERR_SUCCESS;
  OSErr macErr = noErr;

  CGrafPtr macBackBufferPtr = NULL;
  PixMapHandle macPixMapH = nil;

  try {
    if (sf->type == SURFTYPE_OFFSCREEN_OS) {
      CMacOffscreenDataObj *dataObj = nil;

      dataObj = (CMacOffscreenDataObj *)sf->obj;
      if (dataObj == nil) {
        throw(CMacOSSurfErr("Error:  No data object associated with offscreen surface!", 0, DDGRERR_FAIL));
      }

      macPixMapH = GetGWorldPixMap(dataObj->m_GWorld);
    } else if (sf->type == SURFTYPE_VIDEOSCREEN) {
      macErr = DSpContext_GetBackBuffer(gFullScreenData.m_context, kDSpBufferKind_Normal, &macBackBufferPtr);
      if (macErr) {
        throw(CMacOSSurfErr("Could not obtain full screen surface backbuffer ptr!", macErr, DDGRERR_FAIL));
      }

      macPixMapH = macBackBufferPtr->portPixMap;
    } else {
      throw(CMacOSSurfErr("Attempted to lock surface with invalid surftype!", 0, DDGRERR_FAIL));
    }

    UnlockPixels(macPixMapH);

    sf->data = NULL;
    sf->locks--;
  } catch (CMacOSSurfErr errObj) {
    mprintf((0, errObj.m_ErrStr));
    mprintf((0, "\n"));
    mprintf((0, "MacOS Error: %d\n", errObj.m_MacErr));
    err = errObj.m_DDGRErr;
  }

  return err;
}

//	---------------------------------------------------------------------------
//	ddgr_os_surf_AttachHandle(ddgr_suface *sf, unsigned handle)
//		attaches an OS handle to a surface

ddgr_error ddgr_os_surf_AttachHandle(ddgr_surface *sf, unsigned handle) {
  ddgr_error err = DDGRERR_SUCCESS;

  ASSERT(Mac_DDGR_Lib.IsInitted());
  ASSERT(!Mac_DDGR_Lib.IsWindowed());
  ASSERT(sf != NULL);
  ASSERT(sf->type == SURFTYPE_OFFSCREEN_OS || sf->type == SURFTYPE_VIDEOSCREEN);

  err = DDGRERR_FAIL;
  mprintf((0, "Attaching handle to OS Surface is *NOT* implemented\n"));
  return err;
}

//¥======================================================
//	MacOS Full Screen Graphics routines
//¥======================================================

//	Initialize the macintosh for full screen graphics
ddgr_error ddgr_os_surf_fullscreen_Init(ddgr_init_info *info) {
  ddgr_error err = DDGRERR_SUCCESS;
  OSErr macErr = noErr;

  // Save the curreent macintosh port and graphics device to be restored later
  GetPort(&(gFullScreenData.m_save_port));
  gFullScreenData.m_save_gdevice = GetGDevice();

  // Initialize DrawSprocket
  macErr = DSpStartup();
  if (macErr) {
    mprintf((0, "Error initializing draw sprocket!: %d\n", macErr));
    err = DDGRERR_DRIVERINIT;
  }

  return err;
}

ddgr_error ddgr_os_surf_fullscreen_Close(void) {
  ddgr_error err = DDGRERR_SUCCESS;
  OSErr macErr = noErr;

  // Close DrawSprocket
  macErr = DSpShutdown();
  if (macErr) {
    mprintf((0, "Error initializing draw sprocket!: %d\n", macErr));
    err = DDGRERR_DRIVERINIT;
  }

  // Save the curreent macintosh port and graphics device to be restored later
  SetPort(gFullScreenData.m_save_port);
  SetGDevice(gFullScreenData.m_save_gdevice);

  return err;
}

float ddgr_os_surf_GetAspectRatio(void) {
  ASSERT(!Mac_DDGR_Lib.IsWindowed());
  ASSERT(gFullScreenData.m_screen_attr.displayWidth != 0);
  ASSERT(gFullScreenData.m_screen_attr.displayHeight != 0);

  float aspect = 1.0;

  aspect = (float)gFullScreenData.m_screen_attr.displayWidth / (float)gFullScreenData.m_screen_attr.displayHeight;

  return aspect;
}