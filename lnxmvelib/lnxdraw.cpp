#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include "linux/lnxdraw.h"
#include "lnxscreenmode.h"

// static Display *lpDisplay      = NULL;
static int nDefaultScreen = -1;
static unsigned int dwOriginalWidth, dwOriginalHeight;
static LnxWindow **WindowList;
static int NumWindows = 0;
// static int GetXSharedMemory(int size);
inline void BltBuffer16ToPixMap24(unsigned char *pixmap, unsigned char *buffer, int width, int height);
inline void BltBuffer32ToPixMap24(unsigned char *pixmap, unsigned char *buffer, int width, int height);
inline void BltBuffer16ToPixMap16(unsigned char *pixmap, unsigned char *buffer, int width, int height);
inline void BltBuffer32ToPixMap16(unsigned char *pixmap, unsigned char *buffer, int width, int height);

static SDL_Rect dispSize;

//////////////////////
// LnxDraw_InitVideo
//////////////////////
// Initializes the Linux video system (for X-Windows)
//
// Returns:
//       0 : no error
//      -1 : invalid parameter
//      -2 : already initialized
int LnxDraw_InitVideo(LnxVideoDesc *ldesc) {
  return -1;
  if (!ldesc)
    return -1;
  //	if(lpDisplay)
  //		return -2;

  //	lpDisplay = ldesc->dDisplay;
  //	nDefaultScreen = ldesc->nScreen;

  //	dwOriginalWidth = DisplayWidth(lpDisplay,nDefaultScreen);
  //	dwOriginalHeight = DisplayHeight(lpDisplay,nDefaultScreen);

  memset(&dispSize, '\0', sizeof(dispSize));
  return 0;
}

int d3SDLEventFilter(const SDL_Event *event);

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
int LnxDraw_CreateWindow(LnxWindowDesc *ldesc, LnxWindow **lphandle) {

  return -1;
  if (!ldesc || !lphandle)
    return -1;
  *lphandle = NULL;

  // rcg09182000 don't need to quitsubsystem anymore...
  //    SDL_QuitSubSystem(SDL_INIT_VIDEO);  // here goes nothing...
  SDL_ClearError();
  int rc = SDL_Init(SDL_INIT_VIDEO);
  if (rc != 0) {
    fprintf(stderr, "SDL: SDL_Init() failed! rc == (%d).\n", rc);
    fprintf(stderr, "SDL_GetError() reports \"%s\".\n", SDL_GetError());
    return (-2);
  } // if
  SDL_SetEventFilter(d3SDLEventFilter);

  SDL_Rect **modes = LinuxVideoMode.getModes();
  Uint32 sdlflags = LinuxVideoMode.getSDLFlags();

  //	if(!lpDisplay)
  //		return -2;

  // determine what we have to work with (currently)
  //	unsigned int display_width,display_height;
  //	display_width = DisplayWidth(lpDisplay,nDefaultScreen);
  //	display_height = DisplayHeight(lpDisplay,nDefaultScreen);

  // allocate a window for use
  LnxWindow *wnd;
  wnd = (LnxWindow *)malloc(sizeof(*wnd));
  if (!wnd) {
    return -3;
  }

  int i = 0;

  dispSize.x = dispSize.y = 0;
  dispSize.w = ldesc->dwWidth;
  dispSize.h = ldesc->dwHeight;

  // need these two lines for a Voodoo3 bug.
  SDL_ShowCursor(0);
  SDL_WM_GrabInput(SDL_GRAB_ON);

  wnd->surface = SDL_SetVideoMode(ldesc->dwWidth, ldesc->dwHeight, ldesc->bpp, sdlflags);

  // failed? Try a window.
  if ((wnd->surface == NULL) && (sdlflags & SDL_FULLSCREEN)) {
    sdlflags &= ~SDL_FULLSCREEN;
    wnd->surface = SDL_SetVideoMode(ldesc->dwWidth, ldesc->dwHeight, ldesc->bpp, sdlflags);
  } // if

  SDL_WM_GrabInput(SDL_GRAB_OFF);
  if (!(sdlflags & SDL_FULLSCREEN))
    SDL_ShowCursor(1);

  if (wnd->surface == NULL) {
    fprintf(stderr, "ERROR: SDL could not set the video mode!\n");
    return -3;
  } // if

  wnd->fullScreen = (sdlflags & SDL_FULLSCREEN) ? true : false;

  SDL_WM_SetCaption("Descent 3", NULL);

  /*
          wnd->WindowPreCreated = (ldesc->dwFlags&LNXDRAWF_USEPRECREATEDWIN)?true:false;

          if(!wnd->WindowPreCreated)
          {
                  // allocate what the hints, etc.
                  wnd->lpSizeHints = XAllocSizeHints();
                  if(!wnd->lpSizeHints)
                  {
                          // TODO: Free above allocated hints
                          free(wnd);
                          return -3;
                  }

                  wnd->lpWmHints = XAllocWMHints();
                  if(!wnd->lpWmHints)
                  {
                          // TODO: Free above allocated hints
                          free(wnd);
                          return -3;
                  }

                  wnd->lpClassHints = XAllocClassHint();
                  if(!wnd->lpClassHints)
                  {
                          // TODO: Free above allocated hints
                          free(wnd);
                          return -3;
                  }
          }

          // try to match a visual
          if (!XMatchVisualInfo(lpDisplay,nDefaultScreen,16,TrueColor,&wnd->viVisualInfo))
          {
                  fprintf(stderr,"Error: Unable to get 16bit TrueColor Visual\n");

                  // TODO: Free above allocated hints
                  free(wnd);
                  return -3;
          }

          // see if we have shared memory available
          wnd->bHaveSharedMemory = (bool)(XShmQueryExtension(lpDisplay)!=0);
          wnd->shmInfo.shmaddr = NULL;

          // even if the display says it has shared memory, it still might not be
          // available (if we are not running on a local connection)
          if(wnd->bHaveSharedMemory)
          {
                  char *ptr;
                  char *displayname = (char *)getenv("DISPLAY");
                  if (displayname)
                  {
                          ptr = displayname;
                          while (*ptr && (*ptr != ':')) ptr++;
                          if (*ptr) *ptr = '\0';
                          if (!(!strcasecmp(displayname, "unix") || !*displayname))
                                  wnd->bHaveSharedMemory = false;
                  }else
                          wnd->bHaveSharedMemory = false;
          }
          //fprintf(stdout,"Shared Memory: %savailable\n",wnd->bHaveSharedMemory?"":"Not ");

          if(!wnd->WindowPreCreated)
          {
                  wnd->lpXvisual = wnd->viVisualInfo.visual;
                  wnd->cmColorMap = DefaultColormapOfScreen(DefaultScreenOfDisplay(lpDisplay));

                  unsigned long attrib_mask;
                  XSetWindowAttributes attrib;

                  // setup some attribute and hints for actual window creation
                  attrib_mask         = CWEventMask | CWColormap | CWBorderPixel;
                  attrib.event_mask   = KeyPressMask | KeyReleaseMask | PointerMotionMask |
                                        ButtonPressMask | ButtonReleaseMask | ExposureMask;
                  attrib.colormap     = wnd->cmColorMap;
                  attrib.border_pixel = 0;

                  wnd->lpSizeHints->width      = ldesc->dwWidth;
                  wnd->lpSizeHints->height     = ldesc->dwHeight;
                  wnd->lpSizeHints->min_width  = ldesc->dwWidth;
                  wnd->lpSizeHints->max_width  = ldesc->dwWidth;
                  wnd->lpSizeHints->min_height = ldesc->dwHeight;
                  wnd->lpSizeHints->max_height = ldesc->dwHeight;
                  wnd->lpSizeHints->x          = ldesc->dwXPos;
                  wnd->lpSizeHints->y          = ldesc->dwYPos;
                  wnd->lpSizeHints->flags     |= USSize | PMinSize | PMaxSize | USPosition;

                  wnd->wWindow = XCreateWindow(lpDisplay,RootWindow(lpDisplay,nDefaultScreen),
                                                                  ldesc->dwXPos,ldesc->dwYPos,
                                                                  ldesc->dwWidth,ldesc->dwHeight,
                                                                  0,
                                                                  wnd->viVisualInfo.depth,
                                                                  InputOutput,
                                                                  wnd->lpXvisual,
                                                                  attrib_mask,
                                                                  &attrib);

                  XStoreName(lpDisplay,wnd->wWindow,ldesc->lpszName);
                  XSetIconName(lpDisplay,wnd->wWindow,ldesc->lpszName);
                  XSetWMNormalHints(lpDisplay,wnd->wWindow,wnd->lpSizeHints);

                  // Display the window!
                  XMapWindow(lpDisplay,wnd->wWindow);

                  // Wait until it is actually visible
                  bool wait_for_draw = false;
                  XEvent event;
                  while (!wait_for_draw)
                  {
                          XNextEvent(lpDisplay, &event);
                          if (event.type == Expose && !event.xexpose.count)	wait_for_draw = true;
                  }
          }else
          {
                  wnd->wWindow = *ldesc->pre_created_window;

                  // resize window and stuff here
                  wnd->viVisualInfo = ldesc->pre_created_visinfo;
                  wnd->lpXvisual = wnd->viVisualInfo.visual;
                  wnd->cmColorMap = DefaultColormapOfScreen(DefaultScreenOfDisplay(lpDisplay));
          }
  */

  // We're done, add it to our Window list
  WindowList = (LnxWindow **)realloc(WindowList, sizeof(LnxWindow *) * (NumWindows + 1));
  if (!WindowList) {
    return -3;
  }

  WindowList[NumWindows] = wnd;
  *lphandle = wnd;
  NumWindows++;

  // Setup window for blitting
  wnd->bLocked = false;
  wnd->dwWidth = ldesc->dwWidth;
  wnd->dwHeight = ldesc->dwHeight;

  // Create the Graphics Context
  //	int valuemask;
  //	XGCValues xgcvalues;
  //	xgcvalues.graphics_exposures = False;
  //	valuemask = GCGraphicsExposures;

  //	wnd->m_GC = XCreateGC(lpDisplay,wnd->wWindow,valuemask,&xgcvalues);
  //	wnd->lock_ptr = (unsigned char *)malloc(wnd->dwWidth*wnd->dwHeight<<1);

  //	int id = GetXSharedMemory(wnd->dwWidth*wnd->dwHeight<<1);
  //	if(id<0)
  //	{
  //		wnd->bHaveSharedMemory = false;
  //		wnd->shmInfo.shmaddr = NULL;
  //	}else
  //	{
  //		// attach
  //		wnd->shmInfo.shmid = id;
  //		wnd->shmInfo.shmaddr = (char *)shmat(id, 0, 0);
  //	}

  // fprintf(stdout,"Draw: %s shared memory\n",(wnd->bHaveSharedMemory)?"Using":"Not Using");

  // Initial clear
  unsigned char *lock_ptr;
  int lock_pitch;
  if (LnxDraw_LockSurface(wnd, 0, 0, wnd->dwWidth - 1, wnd->dwHeight - 1, &lock_ptr, &lock_pitch)) {
    memset(lock_ptr, 0, wnd->dwWidth * wnd->dwHeight << 1);
    LnxDraw_UnlockSurface(wnd, lock_ptr);
  }

  return 0;
}

//////////////////////////
// LnxDraw_DestroyWindow
//////////////////////////
// Closes and deletes a window
//
// Returns:
//       0 : no error
//      -1 : invalid parameter
int LnxDraw_DestroyWindow(LnxWindow *handle) {
  return -1;
  int i;
  LnxWindow *wnd = NULL;

  for (i = 0; i < NumWindows; i++) {
    if (WindowList[i] == handle) {
      wnd = handle;
      break;
    }
  }

  if (!wnd)
    return -1;

  WindowList[i] = WindowList[NumWindows - 1];
  WindowList = (LnxWindow **)realloc(WindowList, sizeof(LnxWindow *) * (NumWindows - 1));
  NumWindows--;

  /*
          if(wnd->shmInfo.shmaddr)
          {
                  // Release shared memory.
                  shmdt(wnd->shmInfo.shmaddr);
                  shmctl(wnd->shmInfo.shmid, IPC_RMID, 0);
          }

          if(!wnd->WindowPreCreated)
          {
                  // Do what we need to do to close the window
                  XDestroyWindow(lpDisplay,wnd->wWindow);

                  XFree(wnd->lpSizeHints);
                  XFree(wnd->lpWmHints);
                  XFree(wnd->lpClassHints);
          }

          free(wnd->lock_ptr);
  */

  if (wnd->fullScreen) {
    SDL_WM_ToggleFullScreen(wnd->surface);
    SDL_ShowCursor(1);
  } // if

  free(wnd);

  return 0;
}

////////////////////////
// LnxDraw_LockSurface
////////////////////////
// Locks the window surface, giving you a pointer to write data to
//
// Returns:
//       true : success
//       false : error
bool LnxDraw_LockSurface(LnxWindow *wnd, unsigned x1, unsigned y1, unsigned x2, unsigned y2, unsigned char **ptr,
                         int *pitch) {
  return -1;
  if (!wnd || !ptr || !pitch)
    return false;
  if (wnd->bLocked)
    return false;
  wnd->bLocked = true;

  *pitch = wnd->dwWidth << 1;

  /*
          int w,h;
          if(x2<x1){ int t; t = x2; x2=x1; x1=t;}
          if(y2<y2){ int t; t = y2; y2=y1; y1=t;}
          w = x2 - x1 + 1;
          h = y2 - y1 + 1;

  try_no_shared:

          if(wnd->bHaveSharedMemory)
          {
                  wnd->lpImage =
  XShmCreateImage(lpDisplay,wnd->lpXvisual,wnd->viVisualInfo.depth,ZPixmap,0,&wnd->shmInfo,wnd->dwWidth,h);
                  wnd->lpImage->bitmap_bit_order = LSBFirst;
                  wnd->lpImage->byte_order = LSBFirst;
                  wnd->lpImage->bits_per_pixel = 16;
                  wnd->lpImage->bytes_per_line = wnd->dwWidth<<1;
                  wnd->lpImage->red_mask = wnd->lpXvisual->red_mask;
                  wnd->lpImage->green_mask = wnd->lpXvisual->green_mask;
                  wnd->lpImage->blue_mask = wnd->lpXvisual->blue_mask;
                  wnd->lpImage->data = wnd->shmInfo.shmaddr;

                  if(!wnd->lpImage->data)
                  {
                          wnd->bHaveSharedMemory = false;
                          //fprintf(stderr,"Shared Memory: Invalid Memory\n");
                          goto try_no_shared;
                  }

                  // attach the X server to it
                  wnd->shmInfo.readOnly = False;
                  if(!XShmAttach(lpDisplay,&wnd->shmInfo))
                  {
                          wnd->bHaveSharedMemory = false;
                          //fprintf(stderr,"Shared Memory: Unable to attach to server\n");
                          goto try_no_shared;
                  }
          }else
          {
                  wnd->lpImage = XCreateImage(lpDisplay,wnd->lpXvisual,wnd->viVisualInfo.depth,ZPixmap,0,(char
  *)wnd->lock_ptr,wnd->dwWidth,h,16,0); wnd->lpImage->bitmap_bit_order = LSBFirst; wnd->lpImage->byte_order = LSBFirst;
                  wnd->lpImage->bits_per_pixel = 16;
                  wnd->lpImage->bytes_per_line = wnd->dwWidth<<1;
                  wnd->lpImage->red_mask = wnd->lpXvisual->red_mask;
                  wnd->lpImage->green_mask = wnd->lpXvisual->green_mask;
                  wnd->lpImage->blue_mask = wnd->lpXvisual->blue_mask;
                  wnd->lpImage->data = (char *)wnd->lock_ptr;
          }

          *ptr = (unsigned char *)wnd->lpImage->data;
          wnd->lock_x = x1;
          wnd->lock_y = y1;
          wnd->lock_w = w;
          wnd->lock_h = h;
  */

  if (SDL_MUSTLOCK(wnd->surface)) {
    if (SDL_LockSurface(wnd->surface) < 0)
      return (false);
  } // if

  int imgHeight = y2 - y1;
  int linesDown = (wnd->dwHeight - imgHeight) / 2;

  *ptr = ((unsigned char *)wnd->surface->pixels) + ((wnd->surface->format->BytesPerPixel * wnd->dwWidth) * linesDown);
  return true;
}

//////////////////////////
// LnxDraw_UnlockSurface
//////////////////////////
// Unlocks the window surface, blitting the buffer
//
void LnxDraw_UnlockSurface(LnxWindow *wnd, unsigned char *ptr) {
  return;
  if (!wnd->bLocked)
    return;
  bool still_have_shared = true;
  wnd->bLocked = false;

  /*
          // blit the region
          if(wnd->bHaveSharedMemory)
          {
                  if(!XShmPutImage(lpDisplay,wnd->wWindow,wnd->m_GC,wnd->lpImage,0,0,wnd->lock_x,wnd->lock_y,wnd->lock_w,wnd->lock_h,True))
                  {
                          //fprintf(stderr,"XShmPutImage: blit failed\n");
                          wnd->bHaveSharedMemory = false;
                  }
                  // Detach from X server
                  XShmDetach(lpDisplay,&wnd->shmInfo);
          }else
          {
                  // draw the image
                  if(XPutImage(lpDisplay,wnd->wWindow,wnd->m_GC,wnd->lpImage,0,0,wnd->lock_x,wnd->lock_y,wnd->lock_w,wnd->lock_h))
                  {
                          //fprintf(stderr,"XPutImage: blit failed\n");
                  }
          }

          // sync up with server
          XSync(lpDisplay, False);

          // Kill the memory
          wnd->lpImage->data = NULL;//make sure we don't delete our memory here
          XDestroyImage(wnd->lpImage);
  */

  if (SDL_MUSTLOCK(wnd->surface))
    SDL_UnlockSurface(wnd->surface);

  SDL_UpdateRect(wnd->surface, 0, 0, wnd->dwWidth, wnd->dwHeight);

#ifdef __DUMP_MVE_TO_DISK
  static unsigned long framenum = 0;
  char filename[100];
  snprintf(filename, sizeof(filename), "./mve/frame%lu.bmp", framenum);
  SDL_SaveBMP(wnd->surface, filename);
  framenum++;
#endif
}

////////////////////////////
/// LnxDraw_Blit
////////////////////////////
// Blits a buffer to the window
//
// Returns:
//       0 : no error
//      -1 : invalid parameter
//      -2 : unknown error
int LnxDraw_Blit(LnxWindow *wnd, unsigned char *ptr, unsigned int x, unsigned int y, unsigned int w, unsigned int h) {
  return 0;
  /*
          if(!wnd || !ptr)
                  return -1;

          //blt to pixmap
          if(wnd->viVisualInfo.depth==16)
          {
                  //check for best case
                  if(x==0 && y==0 && w==wnd->dwWidth && h==wnd->dwHeight)
                  {
                          BltBuffer16ToPixMap16((unsigned char *)wnd->lpImage->data,ptr,wnd->dwWidth,wnd->dwHeight);
                  }else
                  {
                          int num_rows_to_blit;
                          int start_row,pitch;
                          unsigned char *curr_dest,*curr_src;

                          pitch = wnd->dwWidth<<1;
                          curr_dest = (unsigned char *)wnd->lpImage->data + (y*pitch) + (x<<1);
                          curr_src = ptr;
                          num_rows_to_blit = h;

                          // blit away
                          while(num_rows_to_blit>0)
                          {
                                  BltBuffer16ToPixMap16(curr_dest,curr_src,w,1);

                                  num_rows_to_blit--;
                                  curr_dest += pitch;
                                  curr_src += pitch;
                          }
                  }
          }else
          {
                  return -2;
          }

          //Update window
          if(wnd->bHaveSharedMemory)
          {
                  if(!XShmPutImage(lpDisplay,wnd->wWindow,wnd->m_GC,wnd->lpImage,0, 0, 0,
     0,wnd->dwWidth,wnd->dwHeight,True))
                  {
                          return -2;
                  }
          }else
          {
                  // draw the image
                  XPutImage(lpDisplay,wnd->wWindow,wnd->m_GC,wnd->lpImage,0, 0,0,0,wnd->dwWidth,wnd->dwHeight);
          }

          // sync up with server
          XSync(lpDisplay, False);

          return 0;
  */
}

////////////////////////
// LnxDraw_GetRGBMasks
////////////////////////
// Returns the RGB masks for the display
//
// Returns:
//       0 : no error
//      -1 : invalid parameters
int LnxDraw_GetRGBMasks(LnxWindow *wnd, unsigned int *r, unsigned int *g, unsigned int *b) {
  return -1;
  if (!wnd || !r || !g || !b)
    return -1;
  /*
   *r = wnd->lpImage->red_mask;
   *g = wnd->lpImage->green_mask;
   *b = wnd->lpImage->blue_mask;
   */

  SDL_PixelFormat *pixelFmt = wnd->surface->format;

  *r = pixelFmt->Rmask;
  *g = pixelFmt->Gmask;
  *b = pixelFmt->Bmask;
  return (0);
}

static int GetXSharedMemory(int size) {
  /*
          int key = (14<<24)|(70<<16)|(81<<8)|49;
          struct shmid_ds shminfo;
          int minsize = 640*480;
          int id;
          int	rc;
          int num_try=5;

          do
          {
                  //try to get the id
                  id = shmget((key_t) key, minsize,0x1FF);

                  if(id!=-1)
                  {
                          rc = shmctl(id, IPC_STAT, &shminfo);
                          if(!rc)
                          {
                                  if(shminfo.shm_nattch)
                                  {
                                          key++;
                                  }else
                                  {
                                          if(getuid()==shminfo.shm_perm.cuid)
                                          {
                                                  rc = shmctl(id,IPC_RMID,0);
                                                  if(!rc)
                                                  {
                                                          //fprintf(stderr,"Shared Memory: Stale memory killed\n");
                                                  }
                                                  else
                                                  {
                                                          //fprintf(stderr,"Shared Memory: Unable to kill stale
     memory\n"); return -2;
                                                  }

                                                  id = shmget((key_t)key,size,IPC_CREAT|0x1FF);
                                                  if(id==-1)
                                                  {
                                                          //fprintf(stderr,"Shared Memory: Unable to create shared
     memory block (size=%d)\n",size); return -2;
                                                  }

                                                  rc=shmctl(id,IPC_STAT,&shminfo);
                                                  break;
                                          }

                                          if(size>=shminfo.shm_segsz)
                                          {
                                                  //fprintf(stderr,"Shared Memory: Using User %d's shared
     memory\n",shminfo.shm_cpid); break; }else
                                          {
                                                  //fprintf(stderr,"Shared Memory: Stale memory (User %d, Key=0x%x) is
     too small\n",shminfo.shm_cpid,key); key++;
                                          }
                                  }
                          }else
                          {
                                  //fprintf(stderr,"Shared Memory: Unable to read stats on 0x%x\n",key);
                                  return -2;
                          }
                  }else
                  {
                          id = shmget((key_t)key,size,IPC_CREAT|0x1FF);
                          if(id==-1)
                          {
                                  //fprintf(stderr,"Shared Memory: Unable to get shared memory (error = %d)\n",errno);
                                  return -2;
                          }
                          break;
                  }
          }while (--num_try);

          if(num_try == 0)
          {
                  //fprintf(stderr,"Shared Memory: Unable to get shared memory (too many stale shared memory
     segments)\n");
          }

          return id;
  */
  return (0);
}

inline void BltBuffer32ToPixMap16(unsigned char *pixmap, unsigned char *buffer, int width, int height) {
  unsigned char *data;
  unsigned short int l;
  int r, g, b, a;
  unsigned int c;

  data = (unsigned char *)pixmap;
  for (l = height * width; l > 0; l--) {
    c = *(unsigned int *)buffer;
    a = ((c & 0xff000000) >> 24);
    r = ((c & 0x00ff0000) >> 16);
    g = ((c & 0x0000ff00) >> 8);
    b = (c & 0x000000ff);

    if (a)
      *(unsigned int *)data = (((r >> 3) << 10) + ((g >> 3) << 5) + (b >> 3));
    data += 2;
    buffer += 4;
  }
}

inline void BltBuffer16ToPixMap16(unsigned char *pixmap, unsigned char *buffer, int width, int height) {
  unsigned char *data;
  data = (unsigned char *)pixmap;
  memcpy(data, buffer, (width * height) << 1);
}

inline void BltBuffer32ToPixMap24(unsigned char *pixmap, unsigned char *buffer, int width, int height) {
  unsigned char *data;
  unsigned short int l;
  int r, g, b, a;
  unsigned int c;

  data = (unsigned char *)pixmap;
  for (l = height * width; l > 0; l--) {
    c = *(unsigned int *)buffer;
    a = ((c & 0xff000000) >> 24);
    r = ((c & 0x00ff0000) >> 16);
    g = ((c & 0x0000ff00) >> 8);
    b = (c & 0x000000ff);

    if (a)
      *(unsigned long *)data = ((r << 16) + (g << 8) + b);
    data += 4;
    buffer += 4;
  }
}

inline void BltBuffer16ToPixMap24(unsigned char *pixmap, unsigned char *buffer, int width, int height) {
  unsigned char *data;
  unsigned short int l;
  int r, g, b, a;
  unsigned short c;

  data = (unsigned char *)pixmap;
  for (l = height * width; l > 0; l--) {
    c = *(unsigned short *)buffer;
    a = ((c & 0x8000) >> 15);
    r = ((c & 0x7C00) >> 10);
    g = ((c & 0x03E0) >> 5);
    b = (c & 0x001F);

    if (a)
      *(unsigned long *)data = ((r << 19) + (g << 11) + (b << 3));
    data += 4;
    buffer += 2;
  }
}
