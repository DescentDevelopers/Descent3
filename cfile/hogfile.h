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

 * $Logfile: /DescentIII/Main/Lib/hogfile.h $
 * $Revision: 10 $
 * $Date: 8/06/99 4:29p $
 * $Author: Samir $
 *
 * HOG file constants.
 *
 * $Log: /DescentIII/Main/Lib/hogfile.h $
 *
 * 10    8/06/99 4:29p Samir
 * added function to read record information for a hog file entry.  needed
 * from mn3edit.
 *
 * 9     2/16/99 2:08p Nate
 * Made CreateNewHogFile() a bit friendlier
 *
 * 8     10/30/98 11:16a Nate
 * Added function prototype for writing out hog file entries
 *
 * 7     8/16/98 4:23p Nate
 * Added CreateNewHogFile()
 *
 * 6     8/14/98 4:38p Nate
 * Fixed a few minor bugs and added better error reporting
 *
 * 5     8/14/98 1:01p Nate
 * Added better error reporting for the HogEditor
 *
 * 4     7/20/98 3:34p Nate
 * Added FileCopy() prototype
 *
 * 3     4/01/98 7:03p Samir
 * modified some cfile stuff.
 *
 * 2     3/31/98 6:13p Samir
 * new hogfile format.
 *
 * $NoKeywords: $
 */

#ifndef HOGFILE_H
#define HOGFILE_H

#include <cstdint>
#include <cstdio>

#define HOG_HDR_SIZE (68)
#define HOG_TAG_STR "HOG2"
#define HOG_TAG_LEN (4)
#define HOG_FILENAME_LEN (36)

struct tHogHeader {
  char magic[HOG_TAG_LEN];   // magic "HOG2"
  uint32_t nfiles;           // number of files in header
  uint32_t file_data_offset; // offset in file to filedata.
};

struct tHogFileEntry {
  char name[HOG_FILENAME_LEN]; // file name
  uint32_t flags;              // extra info
  uint32_t len;                // length of file
  uint32_t timestamp;          // time of file.
};

bool ReadHogHeader(FILE *fp, tHogHeader *header);
bool ReadHogEntry(FILE *fp, tHogFileEntry *entry);

#endif
