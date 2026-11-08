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

#ifndef DMFC_FAKES_H
#define DMFC_FAKES_H

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include "cfile.h"

namespace dmfc_test {

// File access uses the real cfile library (the game's wrappers forward to it). The address conversion behaves like
// inet_addr(): the four octets end up in memory in network order, which is how IsAddressBanned() compares them.
void TestOpenCFILE(CFILE **handle, const std::filesystem::path &filename, const char *mode);

uint32_t TestHostAddressFromNumbers(char *str);

extern std::vector<std::string> g_kick_calls;
extern std::vector<std::string> g_other_calls;

void KickHandler(const char *s);
void OtherHandler(const char *s);

int PackDummy(void *, uint8_t *);

// Measures text like grtext_GetTextLineWidth for a font whose glyphs are all 4 pixels wide and 1 pixel apart.
int FixedWidthTextLineWidth(const char *str);

} // namespace dmfc_test

#endif
