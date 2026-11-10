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

// The texts the GetChatModuleString() fake returns. The real ones come from each online module's string table.
#define TEST_FMT_NOT_ONLINE "not-online %s"
#define TEST_FMT_PRIVMSG_TO "to %s: %s"
#define TEST_FMT_PRIVMSG_FROM "from %s: %s"
#define TEST_BAD_NICK "bad-nick"

// The module API pointers chat_api.cpp uses (con_dll.h), as the game would set them when it loads the module.
typedef void (*mem_free_fp)(void *memblock);
extern mem_free_fp DLLmem_free;

namespace chat_test {

// Points the module API at malloc() and free(). The tests never reach the name lookup or the UI polling, so those
// stay null.
void InstallModuleApi();

} // namespace chat_test
