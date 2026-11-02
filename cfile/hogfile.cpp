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

 * $Logfile: /DescentIII/Main/Cfile/hog.cpp $
 * $Revision: 20 $
 * $Date: 8/06/99 4:29p $
 * $Author: Samir $
 *
 *
 *
 * $Log: /DescentIII/Main/Cfile/hog.cpp $
 *
 * 20    8/06/99 4:29p Samir
 * added function to read record information for a hog file entry.  needed
 * from mn3edit.
 *
 * 19    7/28/99 3:35p Kevin
 * Mac!
 *
 * 18    5/10/99 10:25p Ardussi
 * changes to compile on Mac
 *
 * 17    4/15/99 9:19p Jeff
 * Changes for Linux version
 *
 * 16    2/16/99 2:08p Nate
 * Made CreateNewHogFile() a bit friendlier
 *
 * 15    1/13/99 6:39a Jeff
 * fixed some case-sensitive #includes
 *
 * 13    10/15/98 2:52p Nate
 * Fixed FileCopy() by changing free() to mem_free()
 *
 * 12    10/08/98 4:23p Kevin
 * Changed code to comply with memory library usage. Always use mem_malloc
 * , mem_free and mem_strdup
 *
 * 11    8/16/98 4:23p Nate
 * Added CreateNewHogFile()
 *
 * 10    8/14/98 4:38p Nate
 * Fixed a few minor bugs and added better error reporting
 *
 * 9     8/14/98 1:01p Nate
 * Added better error reporting for the HogEditor
 *
 * 8     7/27/98 9:48a Nate
 * Fixed hog file header filling
 *
 * 7     7/24/98 6:39p Nate
 * fixed hog bugs.
 *
 * 6     7/20/98 3:34p Nate
 * Fixed up Hog file stuff
 *
 * 5     7/13/98 6:20p Nate
 * Fixed ReadHogEntry() and WriteHogEntry()
 *
 * 4     7/13/98 2:18p Nate
 *
 * 3     4/01/98 7:03p Samir
 * modified some cfile stuff.
 *
 * 2     3/31/98 6:13p Samir
 * new hogfile format.
 *
 * 1     3/31/98 6:08p Samir
 *
 *
 * $NoKeywords: $
 */

#include <cstring>

#include "byteswap.h"
#include "hogfile.h"

/*
  HOG FILE FORMAT v2.0

    HOG_TAG_STR     [strlen()]
    NFILES          [int32]
    HDRINFO         [HOG_HDR_SIZE]
    FILE_TABLE      [sizeof(FILE_ENTRY) * NFILES]
    FILE 0          [filelen(FILE 0)]
    FILE 1          [filelen(FILE 1)]
    .
    .
    .
    FILE NFILES-1   [filelen(NFILES -1)]
*/

bool ReadHogHeader(FILE *fp, tHogHeader *header) {
  if (fread(&header->magic, HOG_TAG_LEN, 1, fp) != HOG_TAG_LEN && strncmp(header->magic, HOG_TAG_STR, 4) != 0 )
    return false;

  if (fread(&header->nfiles, sizeof(header->nfiles), 1, fp) != 1)
    return false;
  header->nfiles = INTEL_INT(header->nfiles);

  if (fread(&header->file_data_offset, sizeof(header->file_data_offset), 1, fp) != 1)
    return false;
  header->file_data_offset = INTEL_INT(header->file_data_offset);

  return true;
}

bool ReadHogEntry(FILE *fp, tHogFileEntry *entry) {
  if (fread(entry->name, 1, HOG_FILENAME_LEN, fp) != HOG_FILENAME_LEN)
    return false;
  if (fread(&entry->flags, sizeof(entry->flags), 1, fp) != 1)
    return false;
  entry->flags = INTEL_INT(entry->flags);

  if (fread(&entry->len, sizeof(entry->len), 1, fp) != 1)
    return false;
  entry->len = INTEL_INT(entry->len);

  if (fread(&entry->timestamp, sizeof(entry->timestamp), 1, fp) != 1)
    return false;
  entry->timestamp = INTEL_INT(entry->timestamp);

  return true;
}
