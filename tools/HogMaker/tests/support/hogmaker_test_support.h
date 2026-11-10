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

#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include "hogfile.h"

namespace hogmaker_test {

bool CaseInsensitiveLess(const char *a, const char *b);

struct RawHog {
  tHogHeader header{};
  std::vector<tHogFileEntry> entries;
};

bool ReadRawHog(const std::filesystem::path &path, RawHog &out);

int RunHogMaker(const std::vector<std::filesystem::path> &args, const std::filesystem::path &log);

std::map<std::string, std::string> ReadThroughGameLoader(const std::filesystem::path &hog,
                                                         const std::vector<std::string> &names);

std::string AllByteValues();

} // namespace hogmaker_test
