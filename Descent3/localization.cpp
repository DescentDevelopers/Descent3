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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "game.h"
#include "descent.h"
#include "mono.h"
#include "cfile.h"
#include "localization.h"
#include "mem.h"
#include "ddio.h"

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
};
int Num_languages = sizeof(Language_tags) / sizeof(tLangTag);

// The following data, in the anonymous namespace, are static to this file
namespace {
int Localization_language = -1;

int String_table_size = 0;
char **String_table = NULL;

// list of the string table files, they will be loaded in the order they are listed
const char *String_table_list[] = {"D3.STR", NULL};

const char *_Error_string = "!!ERROR MISSING STRING!!";
const char *_Empty_string = "\0";
} // namespace

void Localization_SetLanguage(int type) {
  ASSERT(type >= 0 && type < Num_languages);
  Localization_language = type;
}

int Localization_GetLanguage(void) { return Localization_language; }

#define COMMENT_TAG "!/!" // This line is to be ignored

#define STAG_CONTINUE -1 // this line is just a continuation of the last line
#define STAG_EMPTY -2    // empty line
#define STAG_COMMENT -3  // comment line
// 0 -> Num_languages means it's the start of a string that begins with that language

#define MAX_LINE_LENGTH 1024
#define MAX_STRING_LENGTH 8 * MAX_LINE_LENGTH
#define MAX_TAG_LENGTH 3

int GetTotalStringCount(void);
int LoadStringFile(const char *filename, int starting_offset);
int8_t _parse_line_information(char *line);
char *_parse_string_tag(char *buffer);
char *_parse_escape_chars(char *buffer);

// Call this to load up the string tables into memory
// Returns the number of strings loaded, if this is 0, then the program MUST not continue
int LoadStringTables(void) {
  static bool called = false;
  int old_language;

  if (called) {
    // Only call this guy once
    Int3();
    return 0;
  }
  called = true;

  old_language = Localization_language;

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

  // malloc our array of char *
  String_table = (char **)mem_malloc(sizeof(char *) * string_count);
  if (!String_table) {
    Localization_language = old_language;
    return 0;
  }

  for (int tcount = 0; tcount < string_count; tcount++)
    String_table[tcount] = NULL;

  int runcount = 0;
  int temp;
  int index = 0;
  while (String_table_list[index]) {
    temp = LoadStringFile(String_table_list[index], runcount);
    if (temp == 0) {
      Localization_language = old_language;
      return 0;
    }
    if (runcount + temp > string_count) {
      Localization_language = old_language;
      return 0;
    }
    runcount += temp;
    index++;
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
void FreeStringTables(void) {
  DestroyStringTable(String_table, String_table_size);
  String_table = NULL;
}

const char *GetStringFromTable(int index) {
  if ((index < 0) || (index >= String_table_size))
    return _Error_string;

  if (!String_table[index])
    return _Empty_string;

  return String_table[index];
}

void FixFilenameCase(const char *filename, char *newfile) {
  char path[_MAX_PATH], file[_MAX_FNAME], ext[256];
  ddio_SplitPath(filename, path, file, ext);

  char *p;

  p = file;
  while (*p) {
    *p = tolower(*p);
    p++;
  };
  p = ext;
  while (*p) {
    *p = tolower(*p);
    p++;
  };

  strcat(file, ext);

  if (strlen(path) > 0)
    ddio_MakePath(newfile, path, file, NULL);
  else
    strcpy(newfile, file);
}

// Given a filename, pointer to a char * array and a pointer to an int,
// it will load the string table and fill in the information
// returns true on success
bool CreateStringTable(const char *filename, char ***table, int *size) {
  ASSERT(filename);
  ASSERT(Localization_language != -1);
  if (!filename) {
    if (table)
      *table = NULL;
    if (size)
      *size = 0;
    return false;
  }
  CFILE *file;

  char fname[_MAX_PATH];
  FixFilenameCase(filename, fname);
  file = cfopen(fname, "rt");
  if (!file) {
    if (table)
      *table = NULL;
    if (size)
      *size = 0;
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
    if (_parse_line_information(tempbuffer) == Localization_language)
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
    mprintf(0, "Localization: Warning, 0 strings found in %s\n", filename);
    *table = NULL;
    *size = 0;
    return true;
  }

  *size = scount;
  char **strtable;

  // malloc our array of char *
  *table = (char **)mem_malloc(sizeof(char *) * scount);
  if (!*table) {
    if (table)
      *table = NULL;
    if (size)
      *size = 0;
    Localization_language = old_language;
    return false;
  }

  strtable = *table;
  for (int tcount = 0; tcount < scount; tcount++)
    strtable[tcount] = NULL;

  // now load the strings
  file = cfopen(fname, "rt");
  if (!file) {
    if (table)
      *table = NULL;
    if (size)
      *size = 0;
    Localization_language = old_language;
    return false;
  }

  bool reading_string = false;
  scount = 0;

  GrowString string;

  int line_info;

  while (!cfeof(file)) {
    cf_ReadString(tempbuffer, MAX_STRING_LENGTH, file);
    if (scount >= 198)
      scount = scount;

    line_info = _parse_line_information(tempbuffer);

    switch (line_info) {
    case STAG_CONTINUE:
      if (reading_string) {
        // we need to add on to the working buffer
        string += _parse_escape_chars(tempbuffer);
      }
      break;
    case STAG_EMPTY:
      // empty line !ACK! What should I do...do a new line char or skip?...skip for now
      break;
    case STAG_COMMENT:
      // ignore this line
      if (reading_string) {
        // ok we're done with the string, finish it and put it in the string table
        string.GetString(&strtable[scount]);
        scount++;
        string.Destroy();
      }
      reading_string = false;
      break;
    default: {
      if (line_info >= 0 && line_info < Num_languages) {
        if (reading_string) {
          // ok we're done with the string, finish it and put it in the string table
          string.GetString(&strtable[scount]);
          scount++;
          string.Destroy();
          reading_string = false;
        }

        // see if we should read this string
        if (line_info == Localization_language) {
          reading_string = true;
          // start filling in the buffer
          string.Destroy();
          string = _parse_escape_chars(_parse_string_tag(tempbuffer));
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
    string.GetString(&strtable[scount]);
    scount++;
    string.Destroy();
    reading_string = false;
  }

  cfclose(file);

  mprintf(0, "String Table (%s) loaded with %d strings\n", filename, *size);
  Localization_language = old_language;

  return (scount == (*size));
}

// Given a string table and it's count of strings, it will free up it's memory
void DestroyStringTable(char **table, int size) {
  if ((size > 0) && (table)) {
    for (int i = 0; i < size; i++) {
      if (table[i])
        mem_free(table[i]);
    }
  }

  if (table)
    mem_free(table);
}

// returns the total number of strings in all the string table files
// returns 0 on error
int GetTotalStringCount(void) {
  int scount = 0;
  int findex = 0;
  CFILE *file;
  char tempbuffer[MAX_LINE_LENGTH + 1];
  ASSERT(Localization_language != -1);

  while (String_table_list[findex]) {
    // open the file up
    char fname[_MAX_PATH];
    FixFilenameCase(String_table_list[findex], fname);
    file = cfopen(fname, "rt");
    if (!file)
      return 0;

    while (!cfeof(file)) {
      cf_ReadString(tempbuffer, MAX_LINE_LENGTH + 1, file);
      if (_parse_line_information(tempbuffer) == Localization_language)
        scount++;
    }

    cfclose(file);
    findex++;
  }
  return scount;
}

// Loads a string table file, returns number of strings read if everything went ok,else 0
int LoadStringFile(const char *filename, int starting_offset) {
  ASSERT(filename);
  ASSERT(Localization_language != -1);
  if (!filename)
    return 0;

  CFILE *file;
  char fname[_MAX_PATH];
  FixFilenameCase(filename, fname);
  file = cfopen(fname, "rt");
  if (!file)
    return 0;

  int scount = 0;
  char buffer[MAX_STRING_LENGTH];

  bool reading_string = false;

  GrowString string;
  int line_info;

  while (!cfeof(file)) {
    cf_ReadString(buffer, MAX_STRING_LENGTH, file);
    if (scount >= 198)
      scount = scount;

    line_info = _parse_line_information(buffer);

    switch (line_info) {
    case STAG_CONTINUE:
      if (reading_string) {
        // we need to add on to the working buffer
        string += _parse_escape_chars(buffer);
      }
      break;
    case STAG_EMPTY:
      // empty line !ACK! What should I do...do a new line char or skip?...skip for now
      break;
    case STAG_COMMENT:
      // ignore this line
      if (reading_string) {
        // ok we're done with the string, finish it and put it in the string table
        string.GetString(&String_table[scount + starting_offset]);
        scount++;
        string.Destroy();
      }
      reading_string = false;
      break;
    default: {
      if (line_info >= 0 && line_info < Num_languages) {
        if (reading_string) {
          // ok we're done with the string, finish it and put it in the string table
          string.GetString(&String_table[scount + starting_offset]);
          scount++;
          string.Destroy();
          reading_string = false;
        }

        // see if we should read this string
        if (line_info == Localization_language) {
          reading_string = true;
          // start filling in the buffer
          string.Destroy();
          string = _parse_escape_chars(_parse_string_tag(buffer));
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
    string.GetString(&String_table[scount + starting_offset]);
    scount++;
    string.Destroy();
    reading_string = false;
  }

  cfclose(file);
  return scount;
}

// returns STAG_* information about the line
int8_t _parse_line_information(char *line) {
  for (int i = 0; i < Num_languages; i++) {
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
char *_parse_string_tag(char *buffer) {
  int8_t i = _parse_line_information(buffer);

  switch (i) {
  case STAG_CONTINUE:
  case STAG_EMPTY:
    return buffer;
    break;
  case STAG_COMMENT:
    return buffer + (strlen(COMMENT_TAG));
    break;
  default:
    if (i >= 0 && i < Num_languages) {
      // we have a specific language...
      return buffer + (Language_tags[i].length);
    }
    Int3(); // Get Jeff
    break;
  }
  return buffer;
}

// parses out escape chars...like /t,/n
char *_parse_escape_chars(char *buffer) {
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

GrowString::GrowString() {
  root.string_data = NULL;
  root.next = NULL;
  curr = &root;
}
GrowString::~GrowString() { Destroy(); }

void GrowString::operator+=(char *str) {
  if (!str)
    return;
  if (root.string_data) {
    tbufferinfo *node;
    node = (tbufferinfo *)mem_malloc(sizeof(tbufferinfo));
    if (!node)
      return;
    node->string_data = (char *)mem_malloc(strlen(str) + 2);
    if (!node->string_data) {
      mem_free(node);
      return;
    }
    sprintf(node->string_data, "\n%s", str);
    curr->next = node;
    node->next = NULL;
    curr = node;
  } else {
    root.string_data = (char *)mem_malloc(strlen(str) + 1);
    if (!root.string_data)
      return;
    strcpy(root.string_data, str);
    root.next = NULL;
  }
}

void GrowString::Destroy(void) {
  if (root.string_data)
    mem_free(root.string_data);
  root.string_data = NULL;

  tbufferinfo *c, *next;
  c = next = root.next;
  while (c) {
    next = c->next;
    if (c->string_data)
      mem_free(c->string_data);
    mem_free(c);
    c = next;
  }
  root.next = NULL;
  root.string_data = NULL;
  curr = &root;
}

GrowString GrowString::operator+(char *str) {
  *this += str;
  return *this;
}

GrowString GrowString::operator+(GrowString &gs) {
  char *str = NULL;
  gs.GetString(&str);
  *this += str;
  if (str)
    mem_free(str);
  return *this;
}

void GrowString::operator+=(GrowString &gs) {
  char *str = NULL;
  gs.GetString(&str);
  *this += str;
  if (str)
    mem_free(str);
}

void GrowString::operator=(char *str) {
  Destroy();
  *this += str;
}

void GrowString::operator=(GrowString &gs) {
  char *str = NULL;
  gs.GetString(&str);
  *this = str;
  if (str)
    mem_free(str);
}

void GrowString::GetString(char **str) {
  *str = NULL;
  int size = Length();
  if (size == -1)
    return;

  tbufferinfo *next;
  next = root.next;

  *str = (char *)mem_malloc(size + 1);

  strcpy(*str, root.string_data);
  next = root.next;
  while (next) {
    if (next->string_data) {
      strcat(*str, next->string_data);
    }
    next = next->next;
  }
}

int GrowString::Length(void) {
  if (!root.string_data)
    return -1;

  tbufferinfo *next;
  next = root.next;
  int size = strlen(root.string_data);
  while (next) {
    if (next->string_data)
      size += strlen(next->string_data);
    next = next->next;
  }
  return size;
}
