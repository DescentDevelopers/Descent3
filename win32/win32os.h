/*	Win32 OS Specific internal header to win32 library */

#ifndef WIN32OS_H
#define WIN32OS_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "gameos.h"

typedef struct win32oss_info {
  HWND hwnd;
  HINSTANCE hinst;
} win32oss_info;

#endif