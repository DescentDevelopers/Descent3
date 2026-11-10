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

#include "logger_test_files.h"

namespace fs = std::filesystem;

namespace logger_test {

namespace {
constexpr const char *kLogFileName = "Descent3.log";
} // namespace

fs::path LogFile(const fs::path &dir) { return dir / kLogFileName; }

bool Contains(const std::string &haystack, const char *needle) { return haystack.find(needle) != std::string::npos; }

} // namespace logger_test
