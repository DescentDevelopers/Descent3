/*
 * Descent 3
 * Copyright (C) 2026 Descent Developers
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

#include <cstdlib>
#include <filesystem>
#include <string>
#include <vector>

#include "rtperformance.h"

// rtperformance.cpp writes its log into LocalD3Dir (manage.cpp), which the tests point at a temp directory.
#include "manage.h"

#ifdef USE_RTP
// Defined in rtperformance.cpp but not exported in its header.
extern INT64 Runtime_performance_flags;
#endif

// Splits `line` at every character of `delimiters`.
std::vector<std::string> Split(const std::string &line, const char *delimiters);

// Reads the log written into `dir`: fills `header` with its column names and returns the data rows.
std::vector<std::string> ReadLogRows(const std::filesystem::path &dir, std::vector<std::string> &header);

// The value of column `name` (one of `header`) in `row`.
double Column(const std::vector<std::string> &header, const std::string &row, const std::string &name);
