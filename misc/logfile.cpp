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

#include <utility>

#include "logfile.h"

#ifdef LOGGER
static bool log_enable = true;
#else
static bool log_enable = false;
#endif

void log_Enable() { log_enable = true; }
void log_Disable() { log_enable = false; }

logfile::~logfile() { logfile::end(); }

void logfile::start(const char *filename, const char *longname) {
  if (log_enable) {
    try {
      fp = std::fopen(filename, "wt");
      logfile::printf("%s\n", longname);
    } catch (...) {
      fp = nullptr;
    }
  }
}

void logfile::end() {
  if (fp) {
    try {
      std::fclose(fp);
      fp = nullptr;
    } catch (...) {
      fp = nullptr;
    }
  }
}

template <typename... T> void logfile::printf(const char *format, T &&...arguments) {
  if (fp && format) {
    char msg[256];
    std::snprintf(msg, sizeof(msg), format, std::forward<T>(arguments)...);

    logfile::puts(msg);
  }
}

void logfile::puts(const char *str) {
  if (fp && str) {
    try {
      std::fputs(str, fp);
    } catch (...) {
      logfile::end();
    }
  }
}

void logfile::update() {
  if (fp) {
    try {
      std::fflush(fp);
    } catch (...) {
      logfile::end();
    }
  }
}
