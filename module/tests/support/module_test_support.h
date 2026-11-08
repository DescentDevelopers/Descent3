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

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <string>

#include "module.h"

#ifndef MODULE_TEST_FIXTURE_PATH
#error MODULE_TEST_FIXTURE_PATH must be defined
#endif

// Signatures of the functions exported by test_fixture_module.cpp.
typedef int(DLLFUNCCALL *AddFn)(int, int);
typedef int(DLLFUNCCALL *MagicFn)();

// Path of the built test_fixture_module.
inline const std::filesystem::path kFixture = std::filesystem::u8path(MODULE_TEST_FIXTURE_PATH);

// ASCII lower-case copy of `s`.
inline std::string ToLower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
  return s;
}
