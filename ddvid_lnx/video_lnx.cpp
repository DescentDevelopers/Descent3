/*
 *	Video library.
 *
 * $NoKeywords: $
 */

#include "pserror.h"
#include "ddvid.h"
#include "application.h"
#include "linux/lnxapp.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#undef EGA
//#include <vga.h>
typedef struct {
  int width, height, bytesperpixel, linewidth;
} tinfo;

typedef struct tDDVideoInfo {
  oeLnxApplication *app;
  // vga_modeinfo *info;
  tinfo *info;
  bool paged;
} tDDVideoInfo;

//////////////////////////////////////////////////////////////////////////////
//	Variables

tDDVideoInfo DDVideo_info;
static bool DDVideo_init = false;

//////////////////////////////////////////////////////////////////////////////
//	Prototypes

//////////////////////////////////////////////////////////////////////////////
//	Functions

//	called first to allow fullscreen video access
bool ddvid_Init(oeApplication *app, char *driver) {
  int subsys_id;

  //	preinitialize system.
  if (!DDVideo_init) {
    DDVideo_info.app = NULL;
    DDVideo_info.info = NULL;
    atexit(ddvid_Close);
  } else {
    ddvid_Close();
  }

  DDVideo_init = true;

  DDVideo_info.app = (oeLnxApplication *)app;

  // vga_init();
  return true;
}

//	closes ddvid system manually.
void ddvid_Close() {
  if (!DDVideo_init)
    return;

  /*
    if(vga_getcurrentmode()!=TEXT)
      vga_setmode(TEXT);
  */
  DDVideo_init = false;
}

//	sets the appropriate video mode.
bool ddvid_SetVideoMode(int w, int h, int color_depth, bool paged) {
  /*
    ASSERT(DDVideo_init);

    int mode = -1;

    if( (w==640) && (h==480) ){
      switch(color_depth){
      case BPP_8:
        mode = G640x480x256;
        break;
      case BPP_16:
        mode = G640x480x32K;
        break;
      case BPP_24:
        mode = G640x480x64K;
        break;
      case BPP_32:
        mode = G640x480x16M;
        break;
      }
    }else
      if( (w==512) && (h==384) ){
        mode = -1;
      }else
        if( (w==800) && (h==600) ){
          switch(color_depth){
          case BPP_8:
            mode = G800x600x256;
            break;
          case BPP_16:
            mode = G800x600x32K;
            break;
          case BPP_24:
            mode = G800x600x64K;
            break;
          case BPP_32:
            mode = G800x600x16M;
            break;
          }
        }

    if(mode==-1)
      return false;

    if(!vga_hasmode(mode))
      return false;

    DDVideo_info.info = vga_getmodeinfo(mode);

    DDVideo_info.paged = (bool)(DDVideo_info.info->flags&HAVE_RWPAGE);

    if( (paged) && (!(DDVideo_info.paged)) ){
      DDVideo_info.info = NULL;
      Error("Pages not supported by mode %d\n",mode);
      return false;
    }

    vga_setmode(mode);
  */
  return true;
}

//	sets screen handle
void ddvid_SetVideoHandle(unsigned handle) {}

//	retrieves screen information
void ddvid_GetVideoProperties(int *w, int *h, int *color_depth) {
  *w = 640;
  *h = 480;
  *color_depth = 16;

  /*
    ASSERT(DDVideo_init);
    if(!DDVideo_info.info){
      *w = *h = *color_depth = 0;
      return;
    }
    *w = DDVideo_info.info->width;
    *h = DDVideo_info.info->height;
    *color_depth = DDVideo_info.info->bytesperpixel;
  */
}

//	retrieves screen aspect ratio.
float ddvid_GetAspectRatio() {
  /*
    float aspect_ratio = (float)((3.0 * vga_getxdim())/(4.0 * vga_getydim()));
    return aspect_ratio;
  */
  return 0.75f;
}

//	flips screen if there's a back buffer
void ddvid_VideoFlip() {
  if (!DDVideo_info.paged)
    return;
  // vga_flip();
}

//	retreives frame buffer info for a video mode.
void ddvid_LockFrameBuffer(ubyte **data, int *pitch) {
  *data = NULL;
  *pitch = 0;

  /*
    ASSERT(DDVideo_init);
    if(!DDVideo_info.info){
      *data = NULL;
      *pitch = 0;
      return;
    }

    //*data = vga_getgraphmem();
    *pitch = DDVideo_info.info->linewidth;
  */
}

void ddvid_UnlockFrameBuffer() {
  // ASSERT(DDVideo_init);
}
