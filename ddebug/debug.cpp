/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 * Copyright (C) 2024 Descent Developers
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

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "debug.h"

void Debug_LogClose();

FILE *Debug_logfile = nullptr;

bool Debug_Logfile(const char *filename) {
  if (Debug_logfile == nullptr) {
    Debug_logfile = fopen(filename, "w");
    if (Debug_logfile == nullptr) {
      Debug_MessageBox(OSMBOX_OK, "Debug", "FYI Logfile couldn't be created.");
      return false;
    }
    atexit(Debug_LogClose);
  }
  Debug_LogWrite("BEGINNING LOG\n\n");

  return true;
}

void Debug_LogWrite(const char *str) {
  if (Debug_logfile)
    fwrite(str, strlen(str), 1, Debug_logfile);
}

void Debug_LogClose() {
  if (Debug_logfile) {
    Debug_LogWrite("\nEND LOG");
    fclose(Debug_logfile);
    Debug_logfile = nullptr;
  }
}
