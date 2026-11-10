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

#include "string_table.h"

#include "localization.h"

bool StringTableLoader::Load(std::string_view contents) {
  auto file = dir_.Write("strings.str", contents);
  char **table = nullptr;
  int table_size = -1;
  bool ok = CreateStringTable(file.string().c_str(), &table, &table_size);
  size = table_size;
  table_is_null = (table == nullptr);
  strings.clear();
  for (int i = 0; i < table_size; i++)
    strings.emplace_back(table[i] ? table[i] : "<null>");
  DestroyStringTable(table, table_size);
  return ok;
}
