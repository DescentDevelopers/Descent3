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
#include <vga.h>
#include <vgakeyboard.h>

// remapping
#define keyboard_init sokeyboard_init
#define keyboard_seteventhandler sokeyboard_seteventhandler
#define keyboard_close sokeyboard_close
#define keyboard_update sokeyboard_update

typedef int (*keyboard_init_fp)(void);
FEXTERN keyboard_init_fp sokeyboard_init;

typedef void (*keyboard_seteventhandler_fp)(__keyboard_handler handler);
FEXTERN keyboard_seteventhandler_fp sokeyboard_seteventhandler;

typedef void (*keyboard_close_fp)(void);
FEXTERN keyboard_close_fp sokeyboard_close;

typedef int (*keyboard_update_fp)(void);
FEXTERN keyboard_update_fp sokeyboard_update;

#ifndef DECLARE_POINTERS
bool LoadSVGALib(bool load = true);
#else
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
void LoadSVGALibSetNULL(void) {
  sokeyboard_init = NULL;
  sokeyboard_seteventhandler = NULL;
  sokeyboard_close = NULL;
  sokeyboard_update = NULL;
}

bool LoadSVGALib(bool load) {
#define SVGALIB "libvga.so"
  static void *handle = NULL;

  if (!load) {
    LoadSVGALibSetNULL();
    if (handle) {
      dlclose(handle);
      handle = NULL;
      return true;
    }
    fprintf(stderr, "Library Unload Failed: %s\n", SVGALIB);
    return false;
  }

  if (handle)
    return true;

  // Load the library
  handle = dlopen(SVGALIB, RTLD_LAZY | RTLD_GLOBAL);
  if (!handle) {
    fprintf(stderr, "Library Load Failed: %s\n", SVGALIB);
    return false;
  }

  sokeyboard_init = (keyboard_init_fp)dlsym(handle, "keyboard_init");
  if (!sokeyboard_init)
    goto load_error;

  sokeyboard_seteventhandler = (keyboard_seteventhandler_fp)dlsym(handle, "keyboard_seteventhandler");
  if (!sokeyboard_seteventhandler)
    goto load_error;

  sokeyboard_close = (keyboard_close_fp)dlsym(handle, "keyboard_close");
  if (!sokeyboard_close)
    goto load_error;

  sokeyboard_update = (keyboard_update_fp)dlsym(handle, "keyboard_update");
  if (!sokeyboard_update)
    goto load_error;

  return true;

load_error:
  LoadSVGALibSetNULL();
  fprintf(stderr, "Library Symbol Resolve Error: %s\n", SVGALIB);
  dlclose(handle);
  handle = NULL;
  return false;
}
#endif
