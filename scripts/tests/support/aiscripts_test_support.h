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

// Each aiscripts_<module>_module.cpp #includes one script source and then the adapter for
// its kind of module (aiscripts_factory_adapter.h or aiscripts_message_adapter.h), which
// implements the functions of this header that apply to it. The wrappers keep the test files free of
// the script's own headers (which pull in <windows.h> and its GetMessage macro) and of
// the exported entry points, which every script defines under the same names.

#pragma once

#include <string>
#include <vector>

namespace aiscripts_test {

void InstallEngineImports();

std::vector<std::string> RegisteredScriptNames();
int ScriptIdForName(const char *name);
void *CreateScript(int id);
void DestroyScript(int id, void *instance);

bool LoadMessageFile(const char *path);
const char *LookupMessage(const char *name);
void ResetMessages();

} // namespace aiscripts_test
