#ifdef DECLARE_POINTERS
#define FEXTERN
#else
#define FEXTERN extern
#endif
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>

#define XShmAttach soXShmAttach
#define XShmCreateImage soXShmCreateImage
#define XShmDetach soXShmDetach
#define XShmPutImage soXShmPutImage
#define XShmQueryExtension soXShmQueryExtension

typedef Status (*XShmAttach_fp)(Display *, XShmSegmentInfo *);
FEXTERN XShmAttach_fp soXShmAttach;

typedef XImage *(*XShmCreateImage_fp)(Display *, Visual *, unsigned int, int, char *, XShmSegmentInfo *, unsigned int,
                                      unsigned int);
FEXTERN XShmCreateImage_fp soXShmCreateImage;

typedef Status (*XShmDetach_fp)(Display *, XShmSegmentInfo *);
FEXTERN XShmDetach_fp soXShmDetach;

typedef Status (*XShmPutImage_fp)(Display *, Drawable, GC, XImage *, int, int, int, int, unsigned int, unsigned int,
                                  Bool);
FEXTERN XShmPutImage_fp soXShmPutImage;

typedef Bool (*XShmQueryExtension_fp)(Display *);
FEXTERN XShmQueryExtension_fp soXShmQueryExtension;

#ifndef DECLARE_POINTERS
bool LoadXWindowsExtLib(bool load = true);
#else
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
void LoadXWindowsExtLibSetNULL(void) {
  soXShmAttach = NULL;
  soXShmCreateImage = NULL;
  soXShmDetach = NULL;
  soXShmPutImage = NULL;
  soXShmQueryExtension = NULL;
}

bool LoadXWindowsExtLib(bool load) {
#define XWINDOWSEXTLIB "libXext.so"
  static void *handle = NULL;

  if (!load) {
    LoadXWindowsExtLibSetNULL();
    if (handle) {
      dlclose(handle);
      handle = NULL;
      return true;
    }
    fprintf(stderr, "Library Unload Failed: %s\n", XWINDOWSEXTLIB);
    return false;
  }

  if (handle)
    return true;

  // Load the library
  handle = dlopen(XWINDOWSEXTLIB, RTLD_LAZY | RTLD_GLOBAL);
  if (!handle) {
    fprintf(stderr, "Library Load Failed: %s\n", XWINDOWSEXTLIB);
    return false;
  }

  soXShmAttach = (XShmAttach_fp)dlsym(handle, "XShmAttach");
  if (!soXShmAttach)
    goto load_error;

  soXShmCreateImage = (XShmCreateImage_fp)dlsym(handle, "XShmCreateImage");
  if (!soXShmCreateImage)
    goto load_error;

  soXShmDetach = (XShmDetach_fp)dlsym(handle, "XShmDetach");
  if (!soXShmDetach)
    goto load_error;

  soXShmPutImage = (XShmPutImage_fp)dlsym(handle, "XShmPutImage");
  if (!soXShmPutImage)
    goto load_error;

  soXShmQueryExtension = (XShmQueryExtension_fp)dlsym(handle, "XShmQueryExtension");
  if (!soXShmQueryExtension)
    goto load_error;

  return true;

load_error:
  LoadXWindowsExtLibSetNULL();
  fprintf(stderr, "Library Symbol Resolve Error: %s\n", XWINDOWSEXTLIB);
  dlclose(handle);
  handle = NULL;
  return false;
}
#endif