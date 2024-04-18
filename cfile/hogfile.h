/*
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
#include "pstypes.h"

#define HOG_HDR_SIZE (64)
#define HOG_TAG_STR "HOG2"
#define HOG_FILENAME_LEN (36)

typedef struct tHogHeader {
  uint32_t nfiles;           // number of files in header
  uint32_t file_data_offset; // offset in file to filedata.
} tHogHeader;

typedef struct tHogFileEntry {
  char name[HOG_FILENAME_LEN]; // file name
  uint32_t flags;              // extra info
  uint32_t len;                // length of file
  uint32_t timestamp;          // time of file.
} tHogFileEntry;

enum HogErrors {
  HOGMAKER_ERROR = 0,   // Incorrect number of files passed in
  HOGMAKER_OK,          // Hog file was created successfully
  HOGMAKER_MEMORY,      // Could not allocate hog entry table
  HOGMAKER_OUTFILE,     // Error occurred writing to output hog file
  HOGMAKER_INFILE,      // An input file could not be found (filename is stored in hogerr_filename)
  HOGMAKER_COPY,        // An error occurred copying an input file into the hog file
  HOGMAKER_OPENOUTFILE, // The specified hog file could not be opened for output
};

// Used to return filenames involved in a NewHogFile() error
extern char hogerr_filename[PSPATHNAME_LEN];

int NewHogFile(const char *hogname, int nfiles, const char **filenames, void (*UpdateFunction)(char *) = nullptr);
bool ReadHogHeader(FILE *fp, tHogHeader *header);
bool ReadHogEntry(FILE *fp, tHogFileEntry *entry);
bool WRITE_FILE_ENTRY(FILE *fp, tHogFileEntry *entry);
bool FileCopy(FILE *ofp, FILE *ifp, int length);

// returns hog cfile info, using a library handle opened via cf_OpenLibrary.
bool cf_ReadHogFileEntry(int library, const char *filename, tHogFileEntry *entry, int *fileoffset);

#endif
