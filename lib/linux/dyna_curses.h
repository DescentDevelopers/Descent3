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
#include <curses.h>

#define LINES (*soLINES)
#define COLS (*soCOLS)
#define stdscr (*sostdscr)
#define initscr soinitscr
#define newwin sonewwin
#define wclear sowclear
#define wrefresh sowrefresh
#define scrollok soscrollok
#define leaveok soleaveok
#define wmove sowmove
#define delwin sodelwin
#define endwin soendwin
#define wprintw sowprintw
#define wnoutrefresh sownoutrefresh
#define mvwprintw somvwprintw
#define wtouchln sowtouchln
#define doupdate sodoupdate
#define mvwin somvwin

FEXTERN int *soLINES;
FEXTERN int *soCOLS;
FEXTERN WINDOW **sostdscr;

typedef WINDOW *(*initscr_fp)(void);
FEXTERN initscr_fp soinitscr;

typedef WINDOW *(*newwin_fp)(int, int, int, int);
FEXTERN newwin_fp sonewwin;

typedef int (*wclear_fp)(WINDOW *);
FEXTERN wclear_fp sowclear;

typedef int (*wrefresh_fp)(WINDOW *);
FEXTERN wrefresh_fp sowrefresh;

typedef int (*scrollok_fp)(WINDOW *, bool);
FEXTERN scrollok_fp soscrollok;

typedef int (*leaveok_fp)(WINDOW *, bool);
FEXTERN leaveok_fp soleaveok;

typedef int (*wmove_fp)(WINDOW *, int, int);
FEXTERN wmove_fp sowmove;

typedef int (*delwin_fp)(WINDOW *);
FEXTERN delwin_fp sodelwin;

typedef int (*endwin_fp)(void);
FEXTERN endwin_fp soendwin;

typedef int (*wprintw_fp)(WINDOW *, const char *, ...);
FEXTERN wprintw_fp sowprintw;

typedef int (*wnoutrefresh_fp)(WINDOW *);
FEXTERN wnoutrefresh_fp sownoutrefresh;

typedef int (*mvwprintw_fp)(WINDOW *, int, int, const char *, ...);
FEXTERN mvwprintw_fp somvwprintw;

typedef int (*wtouchln_fp)(WINDOW *, int, int, int);
FEXTERN wtouchln_fp sowtouchln;

typedef int (*doupdate_fp)(void);
FEXTERN doupdate_fp sodoupdate;

typedef int (*mvwin_fp)(WINDOW *, int, int);
FEXTERN mvwin_fp somvwin;

#ifndef DECLARE_POINTERS
bool LoadCursesLib(bool load = true);
#else
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
void LoadCursesLibSetNULL(void) {
  soLINES = NULL;
  soCOLS = NULL;
  sostdscr = NULL;
  soinitscr = NULL;
  sonewwin = NULL;
  sowclear = NULL;
  sowrefresh = NULL;
  soscrollok = NULL;
  soleaveok = NULL;
  sowmove = NULL;
  sodelwin = NULL;
  soendwin = NULL;
  sowprintw = NULL;
  sownoutrefresh = NULL;
  somvwprintw = NULL;
  sowtouchln = NULL;
  sodoupdate = NULL;
  somvwin = NULL;
}

bool LoadCursesLib(bool load) {
#define CURSESLIB "libncurses.so"
  static void *handle = NULL;

  if (!load) {
    LoadCursesLibSetNULL();
    if (handle) {
      dlclose(handle);
      handle = NULL;
      return true;
    }
    fprintf(stderr, "Library Unload Failed: %s\n", CURSESLIB);
    return false;
  }

  if (handle)
    return true;

  // Load the library
  handle = dlopen(CURSESLIB, RTLD_LAZY | RTLD_GLOBAL);
  if (!handle) {
    fprintf(stderr, "Library Load Failed: %s\n", CURSESLIB);
    return false;
  }

  soLINES = (int *)dlsym(handle, "LINES");
  if (!soLINES)
    goto load_error;

  soCOLS = (int *)dlsym(handle, "COLS");
  if (!soCOLS)
    goto load_error;

  sostdscr = (WINDOW **)dlsym(handle, "stdscr");
  if (!sostdscr)
    goto load_error;

  soinitscr = (initscr_fp)dlsym(handle, "initscr");
  if (!soinitscr)
    goto load_error;

  sonewwin = (newwin_fp)dlsym(handle, "newwin");
  if (!sonewwin)
    goto load_error;

  sowclear = (wclear_fp)dlsym(handle, "wclear");
  if (!sowclear)
    goto load_error;

  sowrefresh = (wrefresh_fp)dlsym(handle, "wrefresh");
  if (!sowrefresh)
    goto load_error;

  soscrollok = (scrollok_fp)dlsym(handle, "scrollok");
  if (!soscrollok)
    goto load_error;

  soleaveok = (leaveok_fp)dlsym(handle, "leaveok");
  if (!soleaveok)
    goto load_error;

  sowmove = (wmove_fp)dlsym(handle, "wmove");
  if (!sowmove)
    goto load_error;

  sodelwin = (delwin_fp)dlsym(handle, "delwin");
  if (!sodelwin)
    goto load_error;

  soendwin = (endwin_fp)dlsym(handle, "endwin");
  if (!soendwin)
    goto load_error;

  sowprintw = (wprintw_fp)dlsym(handle, "wprintw");
  if (!sowprintw)
    goto load_error;

  sownoutrefresh = (wnoutrefresh_fp)dlsym(handle, "wnoutrefresh");
  if (!sownoutrefresh)
    goto load_error;

  somvwprintw = (mvwprintw_fp)dlsym(handle, "mvwprintw");
  if (!somvwprintw)
    goto load_error;

  sowtouchln = (wtouchln_fp)dlsym(handle, "wtouchln");
  if (!sowtouchln)
    goto load_error;

  sodoupdate = (doupdate_fp)dlsym(handle, "doupdate");
  if (!sodoupdate)
    goto load_error;

  somvwin = (mvwin_fp)dlsym(handle, "mvwin");
  if (!somvwin)
    goto load_error;

  return true;

load_error:
  LoadCursesLibSetNULL();
  fprintf(stderr, "Library Symbol Resolve Error: %s\n", CURSESLIB);
  dlclose(handle);
  handle = NULL;
  return false;
}
#endif
