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

#ifndef __LOCALIZATION_H__
#define __LOCALIZATION_H__

#include "LocDB.h"

#define LANGUAGE_ENGLISH 0
#define LANGUAGE_GERMAN 1
#define LANGUAGE_SPANISH 2
#define LANGUAGE_ITALIAN 3
#define LANGUAGE_FRENCH 4

extern std::unique_ptr<D3::LocDB> locdb;

/**
 * Load messages into specified bank of messages
 * @note This function only for compatibility with old API. Use locdb instance methods instead.
 * @param name bank of messages
 * @param message_file file containing messages
 * @return true on success
 */
size_t LoadMessages(const std::string &name, const std::filesystem::path &message_file);

/**
 * Clear specified bank of messages
 * @note This function only for compatibility with old API. Use locdb instance methods instead.
 * @param name bank of messages
 */
void FreeMessages(const std::string &name);

/**
 * Gets message by bank name and id.
 * @note This function only for compatibility with old API. Use locdb instance methods instead.
 * On language change or Load/Free messages received output string should be reinitialized.
 * @param bank_name bank name
 * @param id id of messages
 * @return return message; "Missing bank" or "Missing string" if there no such name
 */
const char *GetMessageBank(const std::string &bank_name, const std::string &id);

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

// Given a filename, pointer to a char * array and a pointer to an int,
// it will load the string table and fill in the information
// returns true on success
bool CreateStringTable(const char *filename, char ***table, int *size);
// Given a string table and its count of strings, it will free up its memory
void DestroyStringTable(char **table, int size);

// GrowString class
// handles a string of increasing size (using +,=,+=)
struct tbufferinfo {
  char *string_data;
  tbufferinfo *next;
};

class GrowString {
public:
  GrowString();
  ~GrowString();
  void Destroy();
  void operator+=(char *str);
  GrowString operator+(char *str);
  GrowString operator+(GrowString &gs);
  void operator+=(GrowString &gs);
  void operator=(char *str);
  void operator=(GrowString &gs);
  void GetString(char **str);
  int Length();

private:
  tbufferinfo root;
  tbufferinfo *curr;
};

#endif
