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


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <SDL.h>

#include "loki_utils.h"

const char *loki_getdatapath(void)
{
  static char *basepath = NULL;
  if(basepath == NULL) {
    basepath = SDL_GetBasePath();
    if (basepath == NULL) {
      fprintf(stderr, "ERROR: Couldn't find game directory!\n");
      exit(43);
    }
  }
  return basepath;
}

const char *loki_getprefpath(void)
{
  static char *prefpath = NULL;
  if (prefpath == NULL) {
    prefpath = SDL_GetPrefPath("Outrage Entertainment", "Descent 3");
    if (prefpath == NULL) {
      fprintf(stderr, "ERROR: Couldn't find preference directory!\n");
      exit(43);
    }
  }
  return prefpath;
}

/* end of loki_utils.c ... */
