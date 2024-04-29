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

#ifdef DECLARE_POINTERS
#define FEXTERN
#else
#define FEXTERN extern
#endif
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define XAllocClassHint soXAllocClassHint
#define XAllocSizeHints soXAllocSizeHints
#define XAllocWMHints soXAllocWMHints
#define XCloseDisplay soXCloseDisplay
#define XCreateGC soXCreateGC
#define XCreateImage soXCreateImage
#define XCreatePixmap soXCreatePixmap
#define XCreatePixmapCursor soXCreatePixmapCursor
#define XCreateWindow soXCreateWindow
#define XDefineCursor soXDefineCursor
#define XFillRectangle soXFillRectangle
#define XFreeGC soXFreeGC
#define XFreePixmap soXFreePixmap
#define XGrabPointer soXGrabPointer
#define XKeycodeToKeysym soXKeycodeToKeysym
#define XCheckMaskEvent soXCheckMaskEvent
#define XLookupString soXLookupString
#define XMapWindow soXMapWindow
#define XMatchVisualInfo soXMatchVisualInfo
#define XNextEvent soXNextEvent
#define XOpenDisplay soXOpenDisplay
#define XPutImage soXPutImage
#define XSetIconName soXSetIconName
#define XSetWMNormalHints soXSetWMNormalHints
#define XStoreName soXStoreName
#define XSync soXSync
#define XWarpPointer soXWarpPointer
#define XAutoRepeatOn soXAutoRepeatOn
#define XAutoRepeatOff soXAutoRepeatOff
#define XGetWindowProperty soXGetWindowProperty
#define XQueryTree soXQueryTree
#define XInternAtom soXInternAtom
#define XLowerWindow soXLowerWindow
#define XRaiseWindow soXRaiseWindow
#define XChangeProperty soXChangeProperty
#define XChangeWindowAttributes soXChangeWindowAttributes
#define XMoveResizeWindow soXMoveResizeWindow
#define XUnmapWindow soXUnmapWindow
#define XDestroyWindow soXDestroyWindow
#define XFree soXFree
#define XGetWMNormalHints soXGetWMNormalHints
#define XSetWMProtocols soXSetWMProtocols
#define XSetWMHints soXSetWMHints
#define XSetStandardProperties soXSetStandardProperties
#define XFlush soXFlush
#define XMaskEvent soXMaskEvent

typedef XClassHint *(*XAllocClassHint_fp)(void);
FEXTERN XAllocClassHint_fp soXAllocClassHint;

typedef XSizeHints *(*XAllocSizeHints_fp)(void);
FEXTERN XAllocSizeHints_fp soXAllocSizeHints;

typedef XWMHints *(*XAllocWMHints_fp)(void);
FEXTERN XAllocWMHints_fp soXAllocWMHints;

typedef int (*XCloseDisplay_fp)(Display *);
FEXTERN XCloseDisplay_fp soXCloseDisplay;

typedef GC (*XCreateGC_fp)(Display *, Drawable, unsigned int, XGCValues *);
FEXTERN XCreateGC_fp soXCreateGC;

typedef XImage *(*XCreateImage_fp)(Display *, Visual *, unsigned int, int, int, char *, unsigned int, unsigned int, int,
                                   int);
FEXTERN XCreateImage_fp soXCreateImage;

typedef Pixmap (*XCreatePixmap_fp)(Display *, Drawable, unsigned int, unsigned int, unsigned int);
FEXTERN XCreatePixmap_fp soXCreatePixmap;

typedef Cursor (*XCreatePixmapCursor_fp)(Display *, Pixmap, Pixmap, XColor *, XColor *, unsigned int, unsigned int);
FEXTERN XCreatePixmapCursor_fp soXCreatePixmapCursor;

typedef Window (*XCreateWindow_fp)(Display *, Window, int, int, unsigned int, unsigned int, unsigned int, int,
                                   unsigned int, Visual *, unsigned int, XSetWindowAttributes *);
FEXTERN XCreateWindow_fp soXCreateWindow;

typedef int (*XDefineCursor_fp)(Display *, Window, Cursor);
FEXTERN XDefineCursor_fp soXDefineCursor;

typedef int (*XFillRectangle_fp)(Display *, Drawable, GC, int, int, unsigned int, unsigned int);
FEXTERN XFillRectangle_fp soXFillRectangle;

typedef int (*XFreeGC_fp)(Display *, GC);
FEXTERN XFreeGC_fp soXFreeGC;

typedef int (*XFreePixmap_fp)(Display *, Pixmap);
FEXTERN XFreePixmap_fp soXFreePixmap;

typedef int (*XGrabPointer_fp)(Display *, Window, Bool, unsigned int, int, int, Window, Cursor, Time);
FEXTERN XGrabPointer_fp soXGrabPointer;

typedef KeySym (*XKeycodeToKeysym_fp)(Display *, KeyCode, int);
FEXTERN XKeycodeToKeysym_fp soXKeycodeToKeysym;

typedef Bool (*XCheckMaskEvent_fp)(Display *, long, XEvent *);
FEXTERN XCheckMaskEvent_fp soXCheckMaskEvent;

typedef int (*XLookupString_fp)(XKeyEvent *, char *, int, KeySym, XComposeStatus *);
FEXTERN XLookupString_fp soXLookupString;

typedef int (*XMapWindow_fp)(Display *, Window);
FEXTERN XMapWindow_fp soXMapWindow;

typedef Status (*XMatchVisualInfo_fp)(Display *, int, int, int, XVisualInfo *);
FEXTERN XMatchVisualInfo_fp soXMatchVisualInfo;

typedef int (*XNextEvent_fp)(Display *, XEvent *);
FEXTERN XNextEvent_fp soXNextEvent;

typedef Display *(*XOpenDisplay_fp)(_Xconst char *);
FEXTERN XOpenDisplay_fp soXOpenDisplay;

typedef int (*XPutImage_fp)(Display *, Drawable, GC, XImage *, int, int, int, int, unsigned int, unsigned int);
FEXTERN XPutImage_fp soXPutImage;

typedef int (*XSetIconName_fp)(Display *, Window, _Xconst char *);
FEXTERN XSetIconName_fp soXSetIconName;

typedef void (*XSetWMNormalHints_fp)(Display *, Window, XSizeHints *);
FEXTERN XSetWMNormalHints_fp soXSetWMNormalHints;

typedef int (*XStoreName_fp)(Display *, Window, _Xconst char *);
FEXTERN XStoreName_fp soXStoreName;

typedef int (*XSync_fp)(Display *, Bool);
FEXTERN XSync_fp soXSync;

typedef int (*XWarpPointer_fp)(Display *, Window, Window, int, int, unsigned int, unsigned int, int, int);
FEXTERN XWarpPointer_fp soXWarpPointer;

typedef int (*XAutoRepeatOn_fp)(Display *);
FEXTERN XAutoRepeatOn_fp soXAutoRepeatOn;

typedef int (*XAutoRepeatOff_fp)(Display *);
FEXTERN XAutoRepeatOff_fp soXAutoRepeatOff;

typedef int (*XGetWindowProperty_fp)(Display *, Window, Atom, long, long, Bool, Atom, Atom *, int *, unsigned int *,
                                     unsigned int *, unsigned char **);
FEXTERN XGetWindowProperty_fp soXGetWindowProperty;

typedef Status (*XQueryTree_fp)(Display *, Window, Window *, Window *, Window **, unsigned int *);
FEXTERN XQueryTree_fp soXQueryTree;

typedef Atom (*XInternAtom_fp)(Display *, _Xconst char *, Bool);
FEXTERN XInternAtom_fp soXInternAtom;

typedef int (*XLowerWindow_fp)(Display *, Window);
FEXTERN XLowerWindow_fp soXLowerWindow;

typedef int (*XRaiseWindow_fp)(Display *, Window);
FEXTERN XRaiseWindow_fp soXRaiseWindow;

typedef int (*XChangeProperty_fp)(Display *, Window, Atom, Atom, int, int, _Xconst unsigned char *, int);
FEXTERN XChangeProperty_fp soXChangeProperty;

typedef int (*XChangeWindowAttributes_fp)(Display *, Window, unsigned int, XSetWindowAttributes *);
FEXTERN XChangeWindowAttributes_fp soXChangeWindowAttributes;

typedef int (*XMoveResizeWindow_fp)(Display *, Window, int, int, unsigned int, unsigned int);
FEXTERN XMoveResizeWindow_fp soXMoveResizeWindow;

typedef int (*XUnmapWindow_fp)(Display *, Window);
FEXTERN XUnmapWindow_fp soXUnmapWindow;

typedef int (*XDestroyWindow_fp)(Display *, Window);
FEXTERN XDestroyWindow_fp soXDestroyWindow;

typedef int (*XFree_fp)(void *);
FEXTERN XFree_fp soXFree;

typedef Status (*XGetWMNormalHints_fp)(Display *, Window, XSizeHints *, long *);
FEXTERN XGetWMNormalHints_fp soXGetWMNormalHints;

typedef Status (*XSetWMProtocols_fp)(Display *, Window, Atom *, int);
FEXTERN XSetWMProtocols_fp soXSetWMProtocols;

typedef int (*XSetWMHints_fp)(Display *, Window, XWMHints *);
FEXTERN XSetWMHints_fp soXSetWMHints;

typedef int (*XSetStandardProperties_fp)(Display *, Window, _Xconst char *, _Xconst char *, Pixmap, char **, int,
                                         XSizeHints *);
FEXTERN XSetStandardProperties_fp soXSetStandardProperties;

typedef int (*XFlush_fp)(Display *);
FEXTERN XFlush_fp soXFlush;

typedef int (*XMaskEvent_fp)(Display *, long, XEvent *);
FEXTERN XMaskEvent_fp soXMaskEvent;

#ifndef DECLARE_POINTERS
bool LoadXWindowsLib(bool load = true);
#else
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>

void LoadXWindowsLibSetNULL(void) {
  soXAllocClassHint = NULL;
  soXAllocSizeHints = NULL;
  soXAllocWMHints = NULL;
  soXCloseDisplay = NULL;
  soXCreateGC = NULL;
  soXCreateImage = NULL;
  soXCreatePixmap = NULL;
  soXCreatePixmapCursor = NULL;
  soXCreateWindow = NULL;
  soXDefineCursor = NULL;
  soXFillRectangle = NULL;
  soXFreeGC = NULL;
  soXFreePixmap = NULL;
  soXGrabPointer = NULL;
  soXLookupString = NULL;
  soXMapWindow = NULL;
  soXMatchVisualInfo = NULL;
  soXNextEvent = NULL;
  soXOpenDisplay = NULL;
  soXPutImage = NULL;
  soXSetIconName = NULL;
  soXSetWMNormalHints = NULL;
  soXStoreName = NULL;
  soXSync = NULL;
  soXWarpPointer = NULL;
  soXCheckMaskEvent = NULL;
  soXKeycodeToKeysym = NULL;
  soXLookupString = NULL;
  soXAutoRepeatOn = NULL;
  soXAutoRepeatOff = NULL;
  soXGetWindowProperty = NULL;
  soXQueryTree = NULL;
  soXInternAtom = NULL;
  soXLowerWindow = NULL;
  soXRaiseWindow = NULL;
  soXChangeProperty = NULL;
  soXChangeWindowAttributes = NULL;
  soXMoveResizeWindow = NULL;
  soXUnmapWindow = NULL;
  soXDestroyWindow = NULL;
  soXFree = NULL;
  soXGetWMNormalHints = NULL;
  soXSetWMProtocols = NULL;
  soXSetWMHints = NULL;
  soXSetStandardProperties = NULL;
  soXFlush = NULL;
  soXMaskEvent = NULL;
}

bool LoadXWindowsLib(bool load) {
#define XWINDOWSLIB "libX11.so"
  static void *handle = NULL;

  if (!load) {
    if (handle) {
      LoadXWindowsLibSetNULL();
      dlclose(handle);
      handle = NULL;
      return true;
    }
    fprintf(stderr, "Library Unload Failed: %s\n", XWINDOWSLIB);
    return false;
  }

  if (handle)
    return true;

  // Load the library
  handle = dlopen(XWINDOWSLIB, RTLD_LAZY | RTLD_GLOBAL);
  if (!handle) {
    fprintf(stderr, "Library Load Failed: %s\n", XWINDOWSLIB);
    return false;
  }

  soXAllocClassHint = (XAllocClassHint_fp)dlsym(handle, "XAllocClassHint");
  if (!soXAllocClassHint)
    goto load_error;

  soXAllocSizeHints = (XAllocSizeHints_fp)dlsym(handle, "XAllocSizeHints");
  if (!soXAllocSizeHints)
    goto load_error;

  soXAllocWMHints = (XAllocWMHints_fp)dlsym(handle, "XAllocWMHints");
  if (!soXAllocWMHints)
    goto load_error;

  soXCloseDisplay = (XCloseDisplay_fp)dlsym(handle, "XCloseDisplay");
  if (!soXCloseDisplay)
    goto load_error;

  soXCreateGC = (XCreateGC_fp)dlsym(handle, "XCreateGC");
  if (!soXCreateGC)
    goto load_error;

  soXCreateImage = (XCreateImage_fp)dlsym(handle, "XCreateImage");
  if (!soXCreateImage)
    goto load_error;

  soXCreatePixmap = (XCreatePixmap_fp)dlsym(handle, "XCreatePixmap");
  if (!soXCreatePixmap)
    goto load_error;

  soXCreatePixmapCursor = (XCreatePixmapCursor_fp)dlsym(handle, "XCreatePixmapCursor");
  if (!soXCreatePixmapCursor)
    goto load_error;

  soXCheckMaskEvent = (XCheckMaskEvent_fp)dlsym(handle, "XCheckMaskEvent");
  if (!soXCheckMaskEvent)
    goto load_error;

  soXKeycodeToKeysym = (XKeycodeToKeysym_fp)dlsym(handle, "XKeycodeToKeysym");
  if (!soXKeycodeToKeysym)
    goto load_error;

  soXLookupString = (XLookupString_fp)dlsym(handle, "XLookupString");
  if (!soXLookupString)
    goto load_error;

  soXCreateWindow = (XCreateWindow_fp)dlsym(handle, "XCreateWindow");
  if (!soXCreateWindow)
    goto load_error;

  soXDefineCursor = (XDefineCursor_fp)dlsym(handle, "XDefineCursor");
  if (!soXDefineCursor)
    goto load_error;

  soXFillRectangle = (XFillRectangle_fp)dlsym(handle, "XFillRectangle");
  if (!soXFillRectangle)
    goto load_error;

  soXFreeGC = (XFreeGC_fp)dlsym(handle, "XFreeGC");
  if (!soXFreeGC)
    goto load_error;

  soXFreePixmap = (XFreePixmap_fp)dlsym(handle, "XFreePixmap");
  if (!soXFreePixmap)
    goto load_error;

  soXGrabPointer = (XGrabPointer_fp)dlsym(handle, "XGrabPointer");
  if (!soXGrabPointer)
    goto load_error;

  soXLookupString = (XLookupString_fp)dlsym(handle, "XLookupString");
  if (!soXLookupString)
    goto load_error;

  soXMapWindow = (XMapWindow_fp)dlsym(handle, "XMapWindow");
  if (!soXMapWindow)
    goto load_error;

  soXMatchVisualInfo = (XMatchVisualInfo_fp)dlsym(handle, "XMatchVisualInfo");
  if (!soXMatchVisualInfo)
    goto load_error;

  soXNextEvent = (XNextEvent_fp)dlsym(handle, "XNextEvent");
  if (!soXNextEvent)
    goto load_error;

  soXOpenDisplay = (XOpenDisplay_fp)dlsym(handle, "XOpenDisplay");
  if (!soXOpenDisplay)
    goto load_error;

  soXPutImage = (XPutImage_fp)dlsym(handle, "XPutImage");
  if (!soXPutImage)
    goto load_error;

  soXSetIconName = (XSetIconName_fp)dlsym(handle, "XSetIconName");
  if (!soXSetIconName)
    goto load_error;

  soXSetWMNormalHints = (XSetWMNormalHints_fp)dlsym(handle, "XSetWMNormalHints");
  if (!soXSetWMNormalHints)
    goto load_error;

  soXStoreName = (XStoreName_fp)dlsym(handle, "XStoreName");
  if (!soXStoreName)
    goto load_error;

  soXSync = (XSync_fp)dlsym(handle, "XSync");
  if (!soXSync)
    goto load_error;

  soXWarpPointer = (XWarpPointer_fp)dlsym(handle, "XWarpPointer");
  if (!soXWarpPointer)
    goto load_error;

  soXAutoRepeatOn = (XAutoRepeatOn_fp)dlsym(handle, "XAutoRepeatOn");
  if (!soXAutoRepeatOn)
    goto load_error;

  soXAutoRepeatOff = (XAutoRepeatOff_fp)dlsym(handle, "XAutoRepeatOff");
  if (!soXAutoRepeatOff)
    goto load_error;

  soXGetWindowProperty = (XGetWindowProperty_fp)dlsym(handle, "XGetWindowProperty");
  if (!soXGetWindowProperty)
    goto load_error;

  soXQueryTree = (XQueryTree_fp)dlsym(handle, "XQueryTree");
  if (!soXQueryTree)
    goto load_error;

  soXInternAtom = (XInternAtom_fp)dlsym(handle, "XInternAtom");
  if (!soXInternAtom)
    goto load_error;

  soXLowerWindow = (XLowerWindow_fp)dlsym(handle, "XLowerWindow");
  if (!soXLowerWindow)
    goto load_error;

  soXRaiseWindow = (XRaiseWindow_fp)dlsym(handle, "XRaiseWindow");
  if (!soXRaiseWindow)
    goto load_error;

  soXChangeProperty = (XChangeProperty_fp)dlsym(handle, "XChangeProperty");
  if (!soXChangeProperty)
    goto load_error;

  soXChangeWindowAttributes = (XChangeWindowAttributes_fp)dlsym(handle, "XChangeWindowAttributes");
  if (!soXChangeWindowAttributes)
    goto load_error;

  soXMoveResizeWindow = (XMoveResizeWindow_fp)dlsym(handle, "XMoveResizeWindow");
  if (!soXMoveResizeWindow)
    goto load_error;

  soXUnmapWindow = (XUnmapWindow_fp)dlsym(handle, "XUnmapWindow");
  if (!soXUnmapWindow)
    goto load_error;

  soXDestroyWindow = (XDestroyWindow_fp)dlsym(handle, "XDestroyWindow");
  if (!soXDestroyWindow)
    goto load_error;

  soXFree = (XFree_fp)dlsym(handle, "XFree");
  if (!soXFree)
    goto load_error;

  soXGetWMNormalHints = (XGetWMNormalHints_fp)dlsym(handle, "XGetWMNormalHints");
  if (!soXGetWMNormalHints)
    goto load_error;

  soXSetWMProtocols = (XSetWMProtocols_fp)dlsym(handle, "XSetWMProtocols");
  if (!soXSetWMProtocols)
    goto load_error;

  soXSetWMHints = (XSetWMHints_fp)dlsym(handle, "XSetWMHints");
  if (!soXSetWMHints)
    goto load_error;

  soXSetStandardProperties = (XSetStandardProperties_fp)dlsym(handle, "XSetStandardProperties");
  if (!soXSetStandardProperties)
    goto load_error;

  soXFlush = (XFlush_fp)dlsym(handle, "XFlush");
  if (!soXFlush)
    goto load_error;

  soXMaskEvent = (XMaskEvent_fp)dlsym(handle, "XMaskEvent");
  if (!soXMaskEvent)
    goto load_error;

  return true;

load_error:
  LoadXWindowsLibSetNULL();
  fprintf(stderr, "Library Symbol Resolve Error: %s\n", XWINDOWSLIB);
  dlclose(handle);
  handle = NULL;
  return false;
}
#endif
