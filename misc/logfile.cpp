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

#include "logfile.h"
#include "pstring.h"
#include <stdio.h>
#include <stdarg.h>

#ifdef _DEBUG
static bool log_enable = true;
#else
static bool log_enable = false;
#endif

void log_Enable(bool enable) { log_enable = true; }

void log_Disable() { log_enable = false; }

logfile::logfile() { fp = NULL; }

logfile::~logfile() { end(); }

// restarts the logfile (opens a new one.)
void logfile::start(const char *fname, const char *longname) {
  if (log_enable) {
    try {
      fp = (FILE *)fopen(fname, "wt");
      logfile::printf("%s\n", longname);
    } catch (...) {
      fp = NULL;
    }
  }
}

void logfile::end() {
  if (fp) {
    try {
      fclose((FILE *)fp);
      fp = NULL;
    } catch (...) {
      fp = NULL;
    }
  }
}

void logfile::printf(const char *fmt, ...) {
  if (fp && fmt) {
    char msg[256];
    va_list arglist;
    va_start(arglist, fmt);
    Pvsprintf(msg, sizeof(msg), fmt, arglist);
    va_end(arglist);

    logfile::puts(msg);
  }
}

void logfile::puts(const char *msg) {
  if (fp && msg) {
    try {
      fputs(msg, (FILE *)fp);
    } catch (...) {
      end();
    }
  }
}

void logfile::update() {
  if (fp) {
    try {
      fflush((FILE *)fp);
    } catch (...) {
      end();
    }
  }
}
