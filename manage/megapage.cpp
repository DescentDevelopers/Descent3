/*
 * $Logfile: /DescentIII/Main/manage/megapage.cpp $
 * $Revision: 9 $
 * $Date: 8/11/99 5:32p $
 * $Author: Jeff $
 *
 *
 * $Log: /DescentIII/Main/manage/megapage.cpp $
 *
 * 9     8/11/99 5:32p Jeff
 * changes to fix addon tablefile support so it works correctly
 *
 * 8     7/28/99 2:28p Kevin
 * Macintosh Changes!
 *
 * 7     4/14/99 1:33a Jeff
 * fixed case mismatched #includes
 *
 * 6     3/04/99 4:47p Jason
 * temp fix (ie BAD HACK) for OEM table file woes
 *
 * 5     1/13/99 7:05a Jeff
 * put #ifdef around #include <windows.h>
 *
 * 4     12/29/98 4:30p Jason
 * added add-on data functionality
 *
 * 3     11/06/98 12:35p Jason
 * more speedups for manage system
 *
 * 2     11/05/98 7:55p Jason
 * changes for new manage system
 *
 * 3     6/05/97 6:58p Jason
 * added more megacell functionality
 *
 * 2     6/05/97 2:52p Jason
 * added megacell functions
 *
 * 1     6/05/97 11:13a Jason
 * page for megacells
 *
 * $NoKeywords: $
 */

#if defined(WIN32)
#include <windows.h>
#endif
#include "CFILE.H"
#include "manage.h"
#include "megacell.h"
#include "megapage.h"
#include "texpage.h"
#include "mono.h"
#include "pserror.h"
#include "polymodel.h"
#include "ddio.h"
#if defined(LINUX)
#include <string.h>
#endif
// megacellpage commands that are read/written
// A command is followed by a byte count describing how many bytes
// are in the data for the command
#define MEGACELLPAGE_COMMAND_NAME 1
#define MEGACELLPAGE_COMMAND_END 2
#define MEGACELLPAGE_COMMAND_CELL_NAME 3
#define MEGACELLPAGE_COMMAND_VERSION 4
#define MEGACELLPAGE_COMMAND_WIDTH 5
#define MEGACELLPAGE_COMMAND_HEIGHT 6
#define MEGACELL_VERSION 2
// Given an open file pointer and a megacell_page struct, writes that megacell page out
void mng_WriteMegacellPage(CFILE *outfile, mngs_megacell_page *megacellpage) {
  int i;
  ASSERT(outfile != NULL);
  ASSERT(megacellpage != NULL);

  cf_WriteByte(outfile, PAGETYPE_MEGACELL);

  cf_WriteByte(outfile, MEGACELLPAGE_COMMAND_NAME); // write out megacell name
  cf_WriteByte(outfile, strlen(megacellpage->megacell_struct.name) + 1);
  cf_WriteString(outfile, megacellpage->megacell_struct.name);

  // Write out its cell names
  for (i = 0; i < MAX_MEGACELL_WIDTH * MAX_MEGACELL_HEIGHT; i++) {
    cf_WriteByte(outfile, MEGACELLPAGE_COMMAND_CELL_NAME); // get ready to write out name
    cf_WriteByte(outfile, strlen(megacellpage->cellname[i]) + 2);
    cf_WriteByte(outfile, i);
    cf_WriteString(outfile, megacellpage->cellname[i]);
  }

  cf_WriteByte(outfile, MEGACELLPAGE_COMMAND_WIDTH);
  cf_WriteByte(outfile, 1);
  cf_WriteByte(outfile, megacellpage->megacell_struct.width);
  cf_WriteByte(outfile, MEGACELLPAGE_COMMAND_HEIGHT);
  cf_WriteByte(outfile, 1);
  cf_WriteByte(outfile, megacellpage->megacell_struct.height);
  cf_WriteByte(outfile, MEGACELLPAGE_COMMAND_END); // we're all done
  cf_WriteByte(outfile, 0);
}
// Given an open file pointer and a megacell_page struct, writes that megacell page out
void mng_WriteNewMegacellPage(CFILE *outfile, mngs_megacell_page *megacellpage) {
  int i;
  ASSERT(outfile != NULL);
  ASSERT(megacellpage != NULL);
  int offset = StartManagePage(outfile, PAGETYPE_MEGACELL);
  cf_WriteShort(outfile, MEGACELL_VERSION);

  cf_WriteString(outfile, megacellpage->megacell_struct.name);

  // Write out its cell names
  for (i = 0; i < MAX_MEGACELL_WIDTH * MAX_MEGACELL_HEIGHT; i++)
    cf_WriteString(outfile, megacellpage->cellname[i]);

  cf_WriteByte(outfile, megacellpage->megacell_struct.width);
  cf_WriteByte(outfile, megacellpage->megacell_struct.height);
  EndManagePage(outfile, offset);
}
// Reads a megacell page from an open file.  Returns 0 on error.
int mng_ReadMegacellPage(CFILE *infile, mngs_megacell_page *megacellpage) {
  int done = 0;
  char command;
  ushort len;
  int i, temp;
  if (!Old_table_method)
    return mng_ReadNewMegacellPage(infile, megacellpage);
  ASSERT(infile != NULL);
  memset(megacellpage, 0, sizeof(mngs_megacell_page));
  while (!done) {
    // Read in command byte then read in the length of that commands data

    command = cf_ReadByte(infile);
    len = cf_ReadByte(infile);
    switch (command) {
    case MEGACELLPAGE_COMMAND_END:
      done = 1;
      break;
    case MEGACELLPAGE_COMMAND_CELL_NAME: // the name of the megacell model
      temp = cf_ReadByte(infile);
      cf_ReadString(megacellpage->cellname[temp], PAGENAME_LEN, infile);
      break;
    case MEGACELLPAGE_COMMAND_NAME:
      cf_ReadString(megacellpage->megacell_struct.name, PAGENAME_LEN, infile);
      break;
    case MEGACELLPAGE_COMMAND_WIDTH:
      megacellpage->megacell_struct.width = cf_ReadByte(infile);
      break;
    case MEGACELLPAGE_COMMAND_HEIGHT:
      megacellpage->megacell_struct.height = cf_ReadByte(infile);
      break;
    default:
      // Ignore the ones we don't know
      for (i = 0; i < len; i++)
        cf_ReadByte(infile);
      break;
    }
  }
  // This is a valid new page
  megacellpage->megacell_struct.used = 1;
  return 1; // successfully read
}
// Reads a megacell page from an open file.  Returns 0 on error.
int mng_ReadNewMegacellPage(CFILE *infile, mngs_megacell_page *megacellpage) {
  int i;
  ASSERT(infile != NULL);
  memset(megacellpage, 0, sizeof(mngs_megacell_page));
  int version = cf_ReadShort(infile);

  cf_ReadString(megacellpage->megacell_struct.name, PAGENAME_LEN, infile);

  // Write out its cell names
  for (i = 0; i < MAX_MEGACELL_WIDTH * MAX_MEGACELL_HEIGHT; i++)
    cf_ReadString(megacellpage->cellname[i], PAGENAME_LEN, infile);

  megacellpage->megacell_struct.width = cf_ReadByte(infile);
  megacellpage->megacell_struct.height = cf_ReadByte(infile);
  // This is a valid new page
  megacellpage->megacell_struct.used = 1;
  return 1; // successfully read
}
// Reads in the megacell named "name" into megacellpage struct
// Returns 0 on error or couldn't find, else 1 if all is good
int mng_FindSpecificMegacellPage(char *name, mngs_megacell_page *megacellpage, int local) {
  CFILE *infile;
  ubyte pagetype;
  int done = 0, found = 0;

  if (local)
    infile = cfopen(LocalTableFilename, "rb");
  else
    infile = cfopen(TableFilename, "rb");
  if (!infile) {
    mprintf((0, "Couldn't open table file to find megacell!\n"));
    Int3();
    return 0;
  }

  // Read in the entire page file until we find the page we want
  while (!done) {
    if (cfeof(infile)) {
      done = 1;
      continue;
    }
    pagetype = cf_ReadByte(infile);
    int len = cf_ReadInt(infile);
    // If not a megacell page, just read it in and ignore it
    if (pagetype != PAGETYPE_MEGACELL) {
      cfseek(infile, len - 4, SEEK_CUR);
      continue;
    }
    mng_ReadNewMegacellPage(infile, megacellpage);

    if (!stricmp(name, megacellpage->megacell_struct.name)) {
      // This is the page we want
      found = 1;
      done = 1;
    }
  }
  cfclose(infile);

  return found; // successful!
}
// Given a megacell page, allocs a megacell and calls AssignMegacellPageToMegacell to actually
// load models and values. Rturns megacell handle on success, -1 if fail
int mng_SetAndLoadMegacell(mngs_megacell_page *megacellpage) {
  int n;

  n = AllocMegacell();
  if (n < 0)
    return -1;
  if (!mng_AssignMegacellPageToMegacell(megacellpage, n))
    return -1;

  return n;
}
// Given a megacellpage and a megacell handle, attempts to make megacell n correspond to
// to the megacellpage.
// Returns 1 on success, 0 otherwise
int mng_AssignMegacellPageToMegacell(mngs_megacell_page *megacellpage, int n) {
  megacell *megacellpointer = &Megacells[n];
  int img_handle, i;
  // copy our values
  memcpy(megacellpointer, &megacellpage->megacell_struct, sizeof(megacell));
  strcpy(megacellpointer->name, megacellpage->megacell_struct.name);
  // Try and load our megacell images from the disk
  for (i = 0; i < MAX_MEGACELL_WIDTH * MAX_MEGACELL_HEIGHT; i++) {
    if (!stricmp(megacellpage->cellname[i], "INVALID IMAGE NAME")) {
      megacellpointer->texture_handles[i] = 0;
    } else {
      img_handle = mng_GetGuaranteedTexturePage(megacellpage->cellname[i]);
      if (img_handle < 0)
        megacellpointer->texture_handles[i] = 0;
      else
        megacellpointer->texture_handles[i] = img_handle;
    }
  }

  return 1;
}
// Copies values from a megacell into a megacell_page
void mng_AssignMegacellToMegacellPage(int n, mngs_megacell_page *megacellpage) {
  megacell *megacellpointer = &Megacells[n];
  // Assign the  values
  memcpy(&megacellpage->megacell_struct, megacellpointer, sizeof(megacell));

  strcpy(megacellpage->megacell_struct.name, megacellpointer->name);
  for (int i = 0; i < MAX_MEGACELL_WIDTH * MAX_MEGACELL_HEIGHT; i++) {
    if (megacellpointer->texture_handles[i] != 0)
      strcpy(megacellpage->cellname[i], GameTextures[megacellpointer->texture_handles[i]].name);
    else
      strcpy(megacellpage->cellname[i], "INVALID IMAGE NAME");
  }
}
// Loads a megacell found in the net table file.  It then allocs a megacell and
// then calls SetAndLoadMegacell to actually load in any images/models associated
// with it
void mng_LoadNetMegacellPage(CFILE *infile) {
  mngs_megacell_page megacellpage;
  memset(&megacellpage, 0, sizeof(mngs_megacell_page));

  if (mng_ReadNewMegacellPage(infile, &megacellpage)) {
    int ret = mng_SetAndLoadMegacell(&megacellpage);
    ASSERT(ret >= 0);
  } else
    mprintf((0, "Could not load megacellpage named %s!\n", megacellpage.megacell_struct.name));
}
// Reads a megacell page from a local table file.  It then allocs a megacell and
// loads any images/models associated with that megacell
void mng_LoadLocalMegacellPage(CFILE *infile) {
  mngs_megacell_page megacellpage;
  int ok = 0;
  memset(&megacellpage, 0, sizeof(mngs_megacell_page));
  if (mng_ReadNewMegacellPage(infile, &megacellpage)) {
    // Check to see if this is a local copy that is supposed
    // to go over a network copy (supersede the net copy)
    int i = FindMegacellName(megacellpage.megacell_struct.name);
    if (i != -1) {
      // Make sure we really have this page checked out
      mngs_Pagelock pl;

      strcpy(pl.name, megacellpage.megacell_struct.name);
      pl.pagetype = PAGETYPE_MEGACELL;

      /*if (Network_up && Stand_alone==0)
      {
              int locked=mng_CheckIfPageOwned(&pl,TableUser);
              if (locked!=1)
                      Int3(); // Your local vs net copies of the lock file do not match
      }*/

      mng_AssignMegacellPageToMegacell(&megacellpage, i);
      ok = 1;
    } else {
      // This is a local megacell that has never been checked in
      if ((i = mng_SetAndLoadMegacell(&megacellpage)) < 0)
        ok = 0;
      else
        ok = 1;
    }

    ASSERT(ok == 1);
    if (Loading_addon_table == -1)
      mng_AllocTrackLock(megacellpage.megacell_struct.name, PAGETYPE_MEGACELL);
  } else
    mprintf((0, "Could not load megacellpage named %s!\n", megacellpage.megacell_struct.name));
}
