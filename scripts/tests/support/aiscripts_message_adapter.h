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

// The message-file wrappers of aiscripts_test_support.h for a script module with its own
// message list (ReadMessageFile / GetMessage). Include it right after the script source, in
// that module's aiscripts_<module>_module.cpp.

#pragma once

#include "aiscripts_test_support.h"

namespace aiscripts_test {

bool LoadMessageFile(const char *path) { return ReadMessageFile(path) != 0; }
const char *LookupMessage(const char *name) { return GetMessage(name); }
void ResetMessages() {
  ClearMessageList();
  InitMessageList();
}

} // namespace aiscripts_test
