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

 * $Logfile: /DescentIII/main/localization.cpp $
 * $Revision: 18 $
 * $Date: 11/05/99 4:05p $
 * $Author: Jeff $
 *
 * Localization implementation
 *
 * $Log: /DescentIII/main/localization.cpp $
 *
 * 18    11/05/99 4:05p Jeff
 * fixed bug with 'retry english' closing the file and then trying to read
 * it
 *
 * 17    10/12/99 11:04a Jeff
 * load english string table if foreign strings are not found
 *
 * 16    5/05/99 4:33p Jeff
 * fixed Linux filename fix when opening file for second time
 *
 * 15    5/04/99 8:50p Jeff
 * safety precautions
 *
 * 14    4/26/99 5:17p Jeff
 * loading string tables is filename friendly (for Linux)...filename must
 * be all lowercase
 *
 * 13    4/14/99 2:51a Jeff
 * fixed some case mismatched #includes
 *
 * 12    2/22/99 3:19p Jeff
 * added function to get current language
 *
 * 11    2/11/99 11:02p Jeff
 * fixed bug with aux string table loading
 *
 * 10    2/11/99 10:35p Jeff
 * added support for multiple languages in string table files
 *
 * 9     1/19/99 1:26p Jeff
 * handled \r
 *
 * 8     1/18/99 12:03p Jeff
 * return true if 0 strings are loaded from a correct string table, not
 * false
 *
 * 7     1/13/99 4:05p Jeff
 * when loading a script dll, a string table is looked for and
 * automatically loaded and passed to the newly loaded dll on init for
 * use.
 *
 * 6     6/26/98 4:52p Jeff
 * fixed stupid
 *
 * 5     6/26/98 4:00p Jeff
 * added parsing of escape characters for CreateStringTable()
 *
 * 4     6/17/98 3:28p Jeff
 *
 * 3     6/16/98 10:37a Jeff
 * fixed bug in growstring class, parse escape chars
 *
 * 2     6/12/98 5:55p Jeff
 * initial creation
 *
 * $NoKeywords: $
 */

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <filesystem>
#include <string>
#include <vector>

#include "cfile.h"
#include "localization.h"
#include "localization_external.h"
#include "log.h"
#include "pserror.h"

struct tLangTag {
  const char *tag;
  int length;
};

tLangTag Language_tags[] = {
    {"!=!", -1}, // English
    {"!G!", -1}, // German
    {"!S!", -1}, // Spanish
    {"!I!", -1}, // Italian
    {"!F!", -1}, // French
    {"!P!", -1}, // Polish
};

// The following data, in the anonymous namespace, are static to this file
namespace {
// Assume that we on English locale
int Localization_language = LANGUAGE_ENGLISH;

int String_table_size = 0;
std::vector<std::string> String_table;

// list of the string table files, they will be loaded in the order they are listed
const std::vector<std::string> String_table_list = {"D3.STR"};

const char *Error_string = "!!ERROR MISSING STRING!!";
const char *Empty_string = "\0";
} // namespace

void Localization_SetLanguage(int type) {
  ASSERT(type >= 0 && type < LANGUAGE_TOTAL);
  Localization_language = type;
}

int Localization_GetLanguage() { return Localization_language; }

#define COMMENT_TAG "!/!" // This line is to be ignored

#define STAG_CONTINUE (-1) // this line is just a continuation of the last line
#define STAG_EMPTY (-2)    // empty line
#define STAG_COMMENT (-3)  // comment line
// 0 -> Num_languages means it's the start of a string that begins with that language

#define MAX_LINE_LENGTH 1024
#define MAX_STRING_LENGTH (8 * MAX_LINE_LENGTH)
#define MAX_TAG_LENGTH 3

int GetTotalStringCount();
int LoadStringFile(const std::filesystem::path &filename, int starting_offset);
int parse_line_information(char *line);
char *parse_string_tag(char *buffer);
char *parse_escape_chars(char *buffer);

// Call this to load up the string tables into memory
// Returns the number of strings loaded, if this is 0, then the program MUST not continue
int LoadStringTables() {
  int old_language = Localization_language;

  int string_count = GetTotalStringCount();
  if (string_count == 0) {
    // attempt to load english version of language, so there is something
    if (Localization_language != LANGUAGE_ENGLISH) {
      Localization_language = LANGUAGE_ENGLISH;
      string_count = GetTotalStringCount();
    }

    if (string_count == 0) {
      return 0;
    }
  }

  String_table_size = 0;

  String_table = std::vector<std::string>(string_count);

  int runcount = 0;
  int temp;
  for (auto const &item : String_table_list) {
    temp = LoadStringFile(item, runcount);
    if (temp == 0) {
      Localization_language = old_language;
      return 0;
    }
    if (runcount + temp > string_count) {
      Localization_language = old_language;
      return 0;
    }
    runcount += temp;
  }

  if (runcount == 0) {
    Localization_language = old_language;
    return 0;
  }

  String_table_size = runcount;
  Localization_language = old_language;

  atexit(FreeStringTables);
  return String_table_size;
}

// Deallocates all the memory used for the string tables
void FreeStringTables() {
  String_table.clear();
}

const char *GetStringFromTable(int index) {
  if ((index < 0) || (index >= String_table_size))
    return Error_string;

  if (String_table[index].empty())
    return Empty_string;

  return String_table[index].c_str();
}

/**
 * Lowercase filename part in path
 * @param path
 * @return path with lowercased filename
 */
std::filesystem::path FixFilenameCase(const std::filesystem::path &path) {
  std::filesystem::path parent = path.parent_path();
  std::string fname = path.filename().string();
  std::transform(fname.begin(), fname.end(), fname.begin(), [](unsigned char c){ return std::tolower(c); });
  return (parent / fname);
}

bool CreateStringTable(const std::filesystem::path &filename, std::vector<std::string> &table) {
  ASSERT(!filename.empty());
  ASSERT(Localization_language != -1);

  std::filesystem::path fname = FixFilenameCase(filename);
  CFILE *file = cfopen(fname, "rt");
  if (!file) {
    return false;
  }

  int old_language, scount;
  char tempbuffer[MAX_STRING_LENGTH];

  old_language = Localization_language;

try_english:

  // first found out how many strings total in the file
  scount = 0;

  while (!cfeof(file)) {
    cf_ReadString(tempbuffer, MAX_LINE_LENGTH + 1, file);
    if (parse_line_information(tempbuffer) == Localization_language)
      scount++;
  }
  cfclose(file);

  if (scount == 0) {
    // no strings found, if we are searching a non-English version, then
    // load english versions
    if (Localization_language != LANGUAGE_ENGLISH) {
      Localization_language = LANGUAGE_ENGLISH;
      // open the file again
      file = cfopen(fname, "rt");
      goto try_english;
    }

    // no strings found
    Localization_language = old_language;
    LOG_WARNING << "Localization: Warning, 0 strings found in " << filename;
    return true;
  }

  table = std::vector<std::string>(scount);

  // now load the strings
  file = cfopen(fname, "rt");
  if (!file) {
    Localization_language = old_language;
    return false;
  }

  bool reading_string = false;
  scount = 0;

  std::string string;

  int line_info;

  while (!cfeof(file)) {
    cf_ReadString(tempbuffer, MAX_STRING_LENGTH, file);
    if (scount >= 198)
      scount = scount;

    line_info = parse_line_information(tempbuffer);

    switch (line_info) {
    case STAG_CONTINUE:
      if (reading_string) {
        // we need to add on to the working buffer
        string += parse_escape_chars(tempbuffer);
      }
      break;
    case STAG_EMPTY:
      // empty line !ACK! What should I do...do a new line char or skip?...skip for now
      break;
    case STAG_COMMENT:
      // ignore this line
      if (reading_string) {
        // ok we're done with the string, finish it and put it in the string table
        table[scount] = string;
        scount++;
        string.clear();
      }
      reading_string = false;
      break;
    default: {
      if (line_info >= 0 && line_info < LANGUAGE_TOTAL) {
        if (reading_string) {
          // ok we're done with the string, finish it and put it in the string table
          table[scount] = string;
          scount++;
          string.clear();
          reading_string = false;
        }

        // see if we should read this string
        if (line_info == Localization_language) {
          reading_string = true;
          // start filling in the buffer
          string.clear();
          string = parse_escape_chars(parse_string_tag(tempbuffer));
        }

      } else {
        Int3(); // get Jeff
        reading_string = false;
      }
    } break;
    } // end switch
  }   // end while

  if (reading_string) {
    // we're at the end of the file and we're reading, so but it in the string table
    table[scount] = string;
    scount++;
    string.clear();
    reading_string = false;
  }

  cfclose(file);

  LOG_INFO.printf("String Table (%s) loaded with %d strings", filename.u8string().c_str(), scount);
  Localization_language = old_language;

  return true;
}

void DestroyStringTable(std::vector<std::string> &table) {
  table.clear();
};

// returns the total number of strings in all the string table files
// returns 0 on error
int GetTotalStringCount() {
  int scount = 0;
  char tempbuffer[MAX_LINE_LENGTH + 1];
  ASSERT(Localization_language != -1);

  for (auto const &item : String_table_list) {
    // open the file up
    CFILE *file = cfopen(FixFilenameCase(item), "rt");
    if (!file)
      return 0;

    while (!cfeof(file)) {
      cf_ReadString(tempbuffer, MAX_LINE_LENGTH + 1, file);
      if (parse_line_information(tempbuffer) == Localization_language)
        scount++;
    }

    cfclose(file);
  }
  return scount;
}

// Loads a string table file, returns number of strings read if everything went ok,else 0
int LoadStringFile(const std::filesystem::path &filename, int starting_offset) {
  ASSERT(Localization_language != -1);
  if (filename.empty())
    return 0;

  CFILE *file = cfopen(FixFilenameCase(filename), "rt");
  if (!file)
    return 0;

  int scount = 0;
  char buffer[MAX_STRING_LENGTH];

  bool reading_string = false;

  std::string string;
  int line_info;

  while (!cfeof(file)) {
    cf_ReadString(buffer, MAX_STRING_LENGTH, file);
    if (scount >= 198)
      scount = scount;

    line_info = parse_line_information(buffer);

    switch (line_info) {
    case STAG_CONTINUE:
      if (reading_string) {
        // we need to add on to the working buffer
        string += parse_escape_chars(buffer);
      }
      break;
    case STAG_EMPTY:
      // empty line !ACK! What should I do...do a new line char or skip?...skip for now
      break;
    case STAG_COMMENT:
      // ignore this line
      if (reading_string) {
        // ok we're done with the string, finish it and put it in the string table
        String_table[scount + starting_offset] = string;
        scount++;
        string.clear();
      }
      reading_string = false;
      break;
    default: {
      if (line_info >= 0 && line_info < LANGUAGE_TOTAL) {
        if (reading_string) {
          // ok we're done with the string, finish it and put it in the string table
          String_table[scount + starting_offset] = string;
          scount++;
          string.clear();
          reading_string = false;
        }

        // see if we should read this string
        if (line_info == Localization_language) {
          reading_string = true;
          // start filling in the buffer
          string.clear();
          string = parse_escape_chars(parse_string_tag(buffer));
        }

      } else {
        Int3(); // get Jeff
        reading_string = false;
      }
    } break;
    } // end switch
  }   // end while

  if (reading_string) {
    // we're at the end of the file and we're reading, so but it in the string table
    String_table[scount + starting_offset] = string;
    scount++;
    string.clear();
    reading_string = false;
  }

  cfclose(file);
  return scount;
}

// returns STAG_* information about the line
int parse_line_information(char *line) {
  for (int i = 0; i < LANGUAGE_TOTAL; i++) {
    if (Language_tags[i].length == -1)
      Language_tags[i].length = strlen(Language_tags[i].tag);

    if (!strncmp(line, Language_tags[i].tag, Language_tags[i].length))
      return i;
  }

  if (!strncmp(line, COMMENT_TAG, strlen(COMMENT_TAG)))
    return STAG_COMMENT;
  if (strlen(line) == 0)
    return STAG_EMPTY;
  return STAG_CONTINUE;
}

// parses a string_tag out
char *parse_string_tag(char *buffer) {
  int i = parse_line_information(buffer);

  switch (i) {
  case STAG_CONTINUE:
  case STAG_EMPTY:
    return buffer;
    break;
  case STAG_COMMENT:
    return buffer + (strlen(COMMENT_TAG));
    break;
  default:
    if (i >= 0 && i < LANGUAGE_TOTAL) {
      // we have a specific language...
      return buffer + (Language_tags[i].length);
    }
    Int3(); // Get Jeff
    break;
  }
  return buffer;
}

// parses out escape chars...like /t,/n
char *parse_escape_chars(char *buffer) {
  char tempbuffer[MAX_STRING_LENGTH];
  int t_index, b_index;

  t_index = 0;
  b_index = 0;

  // search for a /
  while (buffer[b_index] != '\0') {
    // see if we are at a backslash
    if (buffer[b_index] != '\\') {
      tempbuffer[t_index] = buffer[b_index];
      t_index++;
      b_index++;
    } else {
      // we are at a backslash, so determine what to do
      b_index++;
      switch (toupper(buffer[b_index])) {
      case 'T':
        tempbuffer[t_index] = '\t';
        break;
      case 'N':
        tempbuffer[t_index] = '\n';
        break;
      case 'R':
        tempbuffer[t_index] = '\r';
        break;
      case '\"':
        tempbuffer[t_index] = '\"';
        break;
      case '\\':
        tempbuffer[t_index] = '\\';
        break;
      default:
        // see if we have a number
        if ((buffer[b_index] >= '0') && (buffer[b_index] <= '9')) {
          // we have a number, so parse the value
          uint8_t value = 0;
          while ((buffer[b_index] >= '0') && (buffer[b_index] <= '9')) {
            value = (value * 10) + buffer[b_index] - '0';
            b_index++;
          }
          // adjust b_index
          b_index--;
          tempbuffer[t_index] = value;
        }
        break;
      }
      b_index++;
      t_index++;
    }
  }
  tempbuffer[t_index] = '\0';
  strcpy(buffer, tempbuffer);
  return buffer;
}
