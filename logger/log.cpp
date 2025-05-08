/*
 * Descent 3
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

#include <filesystem>
#include <plog/Log.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <plog/Initializers/RollingFileInitializer.h>

#ifdef WIN32
#include <plog/Appenders/DebugOutputAppender.h>
#include <cstdio>
#include <windows.h>
#include "debug.h"
#endif

#include "log.h"

void InitLog(plog::Severity log_level, bool enable_filelog, bool enable_win_console) {
  std::filesystem::path log_file = "Descent3.log";
  static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
  static plog::RollingFileAppender<plog::TxtFormatter> fileAppender((const char*)log_file.u8string().c_str());

#ifdef WIN32
  static plog::DebugOutputAppender<plog::TxtFormatter> debugAppender;

  if (enable_win_console) {
    // Open console window
    AllocConsole();
    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
  }
#endif

  plog::init(log_level, &consoleAppender);
  if (enable_filelog) {
    if (std::filesystem::is_regular_file(log_file)) {
      // Delete old log
      std::error_code ec;
      std::filesystem::remove(log_file, ec);
    }
    plog::get()->addAppender(&fileAppender);
  }
#ifdef WIN32
  if (IsDebuggerPresent()) {
    plog::get()->addAppender(&debugAppender);
  }
#endif
}
