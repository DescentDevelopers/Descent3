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

// The factory wrappers of aiscripts_test_support.h for a script module that registers its
// scripts in a ScriptInfo table (GetGOScriptID / CreateInstance / DestroyInstance). Include it
// right after the script source, in that module's aiscripts_<module>_module.cpp.

#pragma once

#include <string>
#include <vector>

#include "aiscripts_test_support.h"

namespace aiscripts_test {

std::vector<std::string> RegisteredScriptNames() {
  std::vector<std::string> names;
  for (const auto &entry : ScriptInfo)
    names.emplace_back(entry.name);
  return names;
}

int ScriptIdForName(const char *name) { return GetGOScriptID(name, 0); }
void *CreateScript(int id) { return CreateInstance(id); }
void DestroyScript(int id, void *instance) { DestroyInstance(id, instance); }

} // namespace aiscripts_test
