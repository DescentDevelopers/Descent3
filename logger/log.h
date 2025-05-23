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

#pragma once

// This is interface header for chosen logger library. Currently, this is plog.
#include <plog/Log.h>
#include "pstypes.h"

/*
In case of swapping to another solution here should be redefined following macros:

#define LOG_VERBOSE                     PLOG(plog::verbose)
#define LOG_DEBUG                       PLOG(plog::debug)
#define LOG_INFO                        PLOG(plog::info)
#define LOG_WARNING                     PLOG(plog::warning)
#define LOG_ERROR                       PLOG(plog::error)
#define LOG_FATAL                       PLOG(plog::fatal)

*/

/**
 * Initialize logger facility.
 * @param log_level desired log level (for example, plog::debug)
 * @param enable_filelog enable logging into Descent.log
 * @param enable_win_console enable console windows for WIN32 (no-op for POSIX systems)
 */
void InitLog(plog::Severity log_level, bool enable_filelog, bool enable_win_console);
