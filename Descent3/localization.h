/*
* Descent 3
* Copyright (C) 2024 Parallax Software
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

--- HISTORICAL COMMENTS FOLLOW ---

 * $Logfile: /DescentIII/main/localization.h $
 * $Revision: 4 $
 * $Date: 2/22/99 3:19p $
 * $Author: Jeff $
 *
 * Localization system header
 *
 * $Log: /DescentIII/main/localization.h $
 *
 * 4     2/22/99 3:19p Jeff
 * added function to get current language
 *
 * 3     2/11/99 10:35p Jeff
 * added support for multiple languages in string table files
 *
 * 2     6/12/98 5:55p Jeff
 * initial creation
 *
 * $NoKeywords: $
 */

#ifndef LOCALIZATION_H
#define LOCALIZATION_H

#include <string>
#include <vector>

#include "localization_external.h"

void Localization_SetLanguage(int type);
int Localization_GetLanguage();

// Call this to load up the string tables into memory
// Returns the number of strings loaded, if this is 0, then the program MUST not continue
int LoadStringTables();

// Deallocates all the memory used for the string tables
void FreeStringTables();

// Returns a pointer to the string at the index location from the string table
// if it is a bad index given, then the pointer to the error string "ERROR MISSING STRING" is given
const char *GetStringFromTable(int index);

/**
 * Creates table of strings from given filename
 * @param filename
 * @param table
 * @return true on success
 */
bool CreateStringTable(const std::filesystem::path &filename, std::vector<std::string> &table);

/**
 * Clears table from loaded strings
 * @param table
 */
void DestroyStringTable(std::vector<std::string> &table);

#endif
