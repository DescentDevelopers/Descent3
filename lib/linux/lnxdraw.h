#ifndef __LNXDRAW_H_
#define __LNXDRAW_H_

// #include "dyna_xwin.h"
// #include "dyna_xext.h"

#include <SDL.h>

typedef struct {
  //	Display *dDisplay;  // Which X-Windows Display to use
  int nScreen; // Which X-Windows screen to use
} LnxVideoDesc;

typedef struct {
  unsigned int bpp;      // bits per pixel.
  unsigned int dwFlags;  // flags for window
  unsigned int dwWidth;  // width of the created window
  unsigned int dwHeight; // height of the created window
  unsigned int dwXPos;   // Top-Left X position
  unsigned int dwYPos;   // Top-Left Y position

  char *lpszName; // Window name
  //	Window *pre_created_window; // Window already created
  //	XVisualInfo pre_created_visinfo;
} LnxWindowDesc;
// #define LNXDRAWF_USEPRECREATEDWIN	0x01

typedef struct {
  //	XSizeHints      *lpSizeHints;
  //	XWMHints        *lpWmHints;
  //	XClassHint      *lpClassHints;
  //	Visual          *lpXvisual;
  //	Window          wWindow;
  SDL_Surface *surface;
  //	bool            bHaveSharedMemory;
  bool bLocked;
  //	bool            WindowPreCreated;
  //	XVisualInfo     viVisualInfo;
  //	Colormap        cmColorMap;
  unsigned char *lpBuffer;
  //	GC              m_GC;
  //	XImage          *lpImage;
  //	XShmSegmentInfo shmInfo;
  unsigned int dwWidth;  // width of the created window
  unsigned int dwHeight; // height of the created window
  bool fullScreen;

  unsigned int lock_x, lock_y, lock_w, lock_h;
  unsigned char *lock_ptr;
} LnxWindow;

//////////////////////
// LnxDraw_InitVideo
//////////////////////
// Initializes the Linux video system (for X-Windows)
//
// Returns:
//       0 : no error
//      -1 : invalid parameter
//      -2 : already initialized
int LnxDraw_InitVideo(LnxVideoDesc *ldesc);

/////////////////////////
// LnxDraw_CreateWindow
/////////////////////////

// Creates and displays a window
//
// Returns:
//       0 : no error (handle in lphandle)
//      -1 : invalid parameter
//      -2 : Display not opened
//      -3 : Out of memory
int LnxDraw_CreateWindow(LnxWindowDesc *ldesc, LnxWindow **lphandle);

//////////////////////////
// LnxDraw_DestroyWindow
//////////////////////////
// Closes and deletes a window
//
// Returns:
//       0 : no error
//      -1 : invalid parameter
int LnxDraw_DestroyWindow(LnxWindow *handle);

////////////////////////
// LnxDraw_LockSurface
////////////////////////
// Locks the window surface, giving you a pointer to write data to
//
// Returns:
//       true : success
//       false : error
bool LnxDraw_LockSurface(LnxWindow *wnd, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2,
                         unsigned char **ptr, int *pitch);

//////////////////////////
// LnxDraw_UnlockSurface
//////////////////////////
// Unlocks the window surface, blitting the buffer
//
void LnxDraw_UnlockSurface(LnxWindow *wnd, unsigned char *ptr);

////////////////////////////
/// LnxDraw_Blit
////////////////////////////
// Blits a buffer to the window
//
// Returns:
//       0 : no error
//      -1 : invalid parameter
//      -2 : unknown error
int LnxDraw_Blit(LnxWindow *wnd, unsigned char *ptr, unsigned int x, unsigned int y, unsigned int w, unsigned int h);

////////////////////////
// LnxDraw_GetRGBMasks
////////////////////////
// Returns the RGB masks for the display
//
// Returns:
//       0 : no error
//      -1 : invalid parameters
int LnxDraw_GetRGBMasks(LnxWindow *wnd, unsigned int *r, unsigned int *g, unsigned int *b);

#endif
