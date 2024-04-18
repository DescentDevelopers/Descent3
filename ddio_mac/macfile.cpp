/*
 * $Logfile: /DescentIII/Main/ddio_mac/macfile.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:56:55 $
 * $Author: kevinb $
 *
 * File operations not covered properly in ANSI C
 *
 * $Log: macfile.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:56:55  kevinb
 * initial 1.5 import
 *
 *
 * 4     10/21/99 3:33p Jeff
 * Macintosh merges
 *
 * 3     8/01/99 10:37p Duane
 * Works with paht+wildcard or just wildcard
 *
 * 2     7/28/99 3:31p Kevin
 * Mac Stuff!
 *
 * 6     5/15/97 1:44 AM Jeremy
 * changed mprintf's to be standard (with newline at end), fixed two bugs:
 * ddio_filediff was returning opposite of correct value, ddio function to
 * set file time stamp had arguments in reverse order
 *
 * 5     5/9/97 8:04 PM Jeremy
 * changed include of <Strings.h> to <TextUtils.h>
 *
 * 4     5/6/97 6:37 PM Jeremy
 * implemented splitpath
 *
 * 3     5/6/97 4:22 PM Jeremy
 * fixed a bug in ddio_MakePath
 *
 * 2     5/6/97 3:11 PM Jeremy
 * added implementation of ddio_MakePath
 *
 * 1     3/13/97 6:53 PM Jeremy
 * macintosh implementation of device dependent io functions
 *
 * $NoKeywords: $
 */
// ANSI Headers
#include <unistd.h>
#include <stat.h>
#include <stdio.h>
#include <unix.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
// #include <assert.h>

// Mac Headers
#include <Files.h>
#include <Strings.h>
#include <TextUtils.h>
// Descent 3 Headers
#include "mono.h"
#include "pserror.h"
#include "ddio.h"
#include "ddio_mac.h"
#include "psglob.h"
#include "mem.h"
#include "cfile/cfile.h"
//	---------------------------------------------------------------------------
//	File Level Globals
//	---------------------------------------------------------------------------
//	---------------------------------------------------------------------------
//	Local File Prototypes
//	---------------------------------------------------------------------------
/*******************************************
 Utility functions specific to the Mac OS
*******************************************
int
stricmp(const char* s1, const char* s2)
{
        char c1, c2;
        while (1)
        {
                c1 = tolower(*s1++);
                c2 = tolower(*s2++);
                if (c1 < c2) return -1;
                if (c1 > c2) return 1;
                if (c1 == 0) return 0;
        }
}
int
strnicmp(const char *s1, const char *s2, int n)
{
    int i;
    char c1, c2;
    for (i=0; i<n; i++)
    {
        c1 = tolower(*s1++);
        c2 = tolower(*s2++);
        if (c1 < c2) return -1;
        if (c1 > c2) return 1;
        if (!c1) return 0;
    }
    return 0;
}
/************************/
//	---------------------------------------------------------------------------
//	File Operations
//	---------------------------------------------------------------------------
//	---------------------------------------------------------------------------
//	File operations
bool ddio_CreateDir(const char *path) {
  bool failed = true;
  failed = mkdir(path, 0);

  return !failed;
}
bool ddio_RemoveDir(const char *path) {
  bool failed = true;

  failed = rmdir(path);

  return (!failed);
}
//	retrieve the current working folder where file operation will occur.
//	Note ---> The path in Get/Set working directory is in the *LOCAL* file system's syntax
//	This pathname is relative *OR* fully qualified
static char SavedWorkingDir[_MAX_PATH];

void ddio_GetWorkingDir(char *path, int len) {
  getcwd(path, len);
  len = strlen(path);
  if (path[len - 1] == ':') {
    path[len - 1] = '\0';
  }
}
//	set the current working folder where file operation will occur.
//	Note ---> The path in Get/Set working directory is in the *LOCAL* file system's syntax
//	This pathname is relative *OR* fully qualified
bool ddio_SetWorkingDir(const char *path) {
  bool failed = true;

  failed = chdir(path);

  if (!failed) {
    //		if(path[strlen(path)-1] != ':')
    //			strcat(path, ":");
    strcpy(SavedWorkingDir, path);
    return true;
  } else {
    return false;
  }
}
//	check if two files are different
bool ddio_FileDiff(const char *fileNameA, const char *fileNameB) {
  struct stat infoA;
  struct stat infoB;

  bool filesAreEqual = false;
  if (fileNameA && fileNameB) {
    if (!(strcmp(fileNameA, fileNameB))) {
      filesAreEqual = true;
    } else // test file info
    {
      stat(fileNameA, &infoA);
      stat(fileNameB, &infoB);

      if ((infoA.st_mode == infoB.st_mode) && (infoA.st_size == infoB.st_size) && (infoA.st_mtime == infoB.st_mtime)) {
        filesAreEqual = true;
      }
    }
  }

  return !filesAreEqual;
}
//  get a file length of a FILE
int ddio_GetFileLength(FILE *filePtr) {
  int size = -1;
  if (filePtr) {
    int filedes = fileno(filePtr);
    struct stat info;

    fstat(filedes, &info);

    size = info.st_size;
  } else {
    mprintf((0, "Tried getting length of NULL fileptr!\n"));
    Debugger();
  }

  return size;
}
// Split a pathname into its component parts
void ddio_SplitPath(const char *srcPath, char *path, char *filename, char *ext) {
  int pathStart = -1;
  int pathEnd = -1;
  int fileStart = -1;
  int fileEnd = -1;
  int extStart = -1;
  int extEnd = -1;
  int totalLen = strlen(srcPath);
  // Check for an extension
  ///////////////////////////////////////
  int t = totalLen - 1;
  while ((srcPath[t] != '.') && (srcPath[t] != ':') && (t >= 0))
    t--;
  // see if we are at an extension
  if ((t >= 0) && (srcPath[t] == '.')) {
    // we have an extension
    extStart = t;
    extEnd = totalLen - 1;
    if (ext) {
      strncpy(ext, &(srcPath[extStart]), extEnd - extStart + 1);
      ext[extEnd - extStart + 1] = '\0';
    }
  } else {
    // no extension
    if (ext)
      ext[0] = '\0';
  }
  // Check for file name
  ////////////////////////////////////
  int temp = (extStart != -1) ? (extStart) : (totalLen - 1);
  while ((srcPath[temp] != ':') && (temp >= 0))
    temp--;
  if (temp < 0)
    temp = 0;
  if (srcPath[temp] == ':') {
    // we have a file
    fileStart = temp + 1;
    if (extStart != -1)
      fileEnd = extStart - 1;
    else
      fileEnd = totalLen - 1;
    if (filename) {
      strncpy(filename, &(srcPath[fileStart]), fileEnd - fileStart + 1);
      filename[fileEnd - fileStart + 1] = '\0';
    }
    pathStart = 0;
    pathEnd = fileStart - 2;
    // Copy the rest into the path name
    if (path) {
      strncpy(path, &(srcPath[pathStart]), pathEnd - pathStart + 1);
      path[pathEnd - pathStart + 1] = 0;
    }
  } else {
    // only file, no path
    fileStart = 0;
    if (extStart != -1)
      fileEnd = extStart - 1;
    else
      fileEnd = totalLen - 1;
    if (filename) {
      strncpy(filename, &(srcPath[fileStart]), fileEnd - fileStart + 1);
      filename[fileEnd - fileStart + 1] = 0;
    }

    // Only file no path
    if (path) {
      path[0] = 0;
    }
  }
#if 0
	// Check for an extension
	if (srcPath[totalLen - 4] == '.')
	{
		extStart = totalLen - 4;
		extEnd 	 = totalLen - 1;
		strncpy(ext, &(srcPath[extStart]), extEnd - extStart + 1);
		ext[extEnd - extStart + 1] = 0;
	}
	else
	{
		ext[0] = 0;
	}
		
	// Check for file name
	char* namePtr = strrchr(srcPath, ':');
	if (namePtr)
	{
		fileStart = namePtr - srcPath + 1;
		if (extStart != -1)
		{
			fileEnd = extStart - 2;
		}
		else
		{
			fileEnd = totalLen - 1;
		}
		
		strncpy(filename, &(srcPath[fileStart]), fileEnd - fileStart + 1);
		filename[fileEnd - fileStart + 1] = 0;
		
		pathStart = 0;
		pathEnd = fileStart - 2;
		
		// Copy the rest into the path name
		strncpy(path, &(srcPath[pathStart]), pathEnd - pathStart + 1);
		path[pathEnd - pathStart + 1] = 0;
	}
	else
	{
		fileStart = 0;
		if (extStart != -1)
		{
			fileEnd = extStart - 2;
		}
		else
		{
			fileEnd = totalLen - 1;
		}
		
		strncpy(filename, &(srcPath[fileStart]), fileEnd - fileStart + 1);
		filename[fileEnd - fileStart + 1] = 0;
		// Only file no path
		path[0] = 0;
	}
#endif
}
void ddio_CopyFileTime(char *dstFile, const char *srcFile) {
  CInfoPBRec srcPB;
  CInfoPBRec dstPB;
  OSErr err = noErr;
  Str255 srcMacFile = "\p";
  Str255 dstMacFile = "\p";
  Str255 volName = "\p";
  short volRefNum = 0;
  long dirID = 0;

  // Get the info for the current default volume (where the working directory lives)
  err = HGetVol(volName, &volRefNum, &dirID);
  if (err) {
    mprintf((0, "Error getting default volume info in ddio_CopyFileTime: %d\n", err));
    Debugger();
  } else {
    // Convert the c-string version of the path to a p-string
    strcpy((char *)srcMacFile, srcFile);
    c2pstr((char *)srcMacFile);
    // Set up the source file's parameter block
    memset(&srcPB, 0, sizeof(srcPB));
    srcPB.hFileInfo.ioNamePtr = srcMacFile;
    srcPB.hFileInfo.ioFDirIndex = 0;
    srcPB.hFileInfo.ioVRefNum = volRefNum;
    srcPB.hFileInfo.ioDirID = dirID;

    // Get the source file's info
    err = PBGetCatInfoSync(&srcPB);
    if (err == noErr) {
      // Convert the c-string version of the path to a p-string
      strcpy((char *)dstMacFile, dstFile);
      c2pstr((char *)dstMacFile);
      // Set up the target file's parameter block
      memset(&dstPB, 0, sizeof(dstPB));
      dstPB.hFileInfo.ioNamePtr = dstMacFile;
      dstPB.hFileInfo.ioVRefNum = volRefNum;

      dstPB.hFileInfo.ioFDirIndex = 0;
      dstPB.hFileInfo.ioDirID = dirID;

      // Get the target file's info
      err = PBGetCatInfoSync(&dstPB);
      if (err == noErr) {
        // Transfer the src file's time stamp to the target
        dstPB.hFileInfo.ioFlCrDat = srcPB.hFileInfo.ioFlCrDat;
        dstPB.hFileInfo.ioFlMdDat = srcPB.hFileInfo.ioFlMdDat;
        dstPB.hFileInfo.ioFlBkDat = srcPB.hFileInfo.ioFlBkDat;

        // Restore the directory id (which got bashed to the fileID by PBGetCatInfo
        dstPB.hFileInfo.ioDirID = dirID;

        err = PBSetCatInfoSync(&dstPB);
        if (err == noErr) {
          ; // success
        } else {
          mprintf((0, "Error setting target file's timestamp in ddio_CopyFileTime: %d\n", err));
          Debugger();
        }
      } else {
        mprintf((0, "Error getting target file's info in ddio_CopyFileTime: %d\n", err));
        Debugger();
      }
    } else {
      mprintf((0, "Error getting source file's time stamp in ddio_CopyFileTime: %d\n", err));
      Int3();
    }
  }
}
// deletes a file.  Returns 1 if successful, 0 on failure
int ddio_DeleteFile(char *name) {
  bool failed = true;

  failed = remove(name);

  return (!failed);
}
// Save/Restore the current working directory
void ddio_SaveWorkingDir(void) { ddio_GetWorkingDir(SavedWorkingDir, _MAX_PATH); }
void ddio_RestoreWorkingDir(void) { ddio_SetWorkingDir(SavedWorkingDir); }
// 	Checks if a directory exists (returns 1 if it does, 0 if not)
//	This pathname is *RELATIVE* not fully qualified
bool ddio_DirExists(const char *path) {
  bool res;

  ddio_SaveWorkingDir();
  res = chdir(path);
  if (res) {
    char tempbuffer[_MAX_PATH];
    strcpy(tempbuffer, ":");
    strcat(tempbuffer, path);
    res = chdir(tempbuffer);
  }
  ddio_RestoreWorkingDir();
  return (!res) ? true : false;
}
// Constructs a path in the local file system's syntax
// 	newPath: stores the constructed path
//  absolutePathHeader: absolute path on which the sub directories will be appended
//						(specified in local file system syntax)
//  takes a variable number of subdirectories which will be concatenated on to the path
//		the last argument in the list of sub dirs *MUST* be NULL to terminate the list
void ddio_MakePath(char *newPath, const char *absolutePathHeader, const char *subDir, ...) {
  const char delimiter = ':';
  va_list args;
  char *currentDir = NULL;
  int pathLength = 0;

  assert(newPath);
  if (absolutePathHeader == 0)
    mprintf((2, "WOOPS\n"));
  assert(absolutePathHeader);
  assert(subDir);

  if (newPath != absolutePathHeader) {
    strcpy(newPath, absolutePathHeader);
  }
  // Add the first sub directory
  pathLength = strlen(newPath);
  if (newPath[pathLength - 1] != delimiter) {
    newPath[pathLength] = delimiter; // add the delimiter
    newPath[pathLength + 1] = 0;     // terminate the string
  }
  strcat(newPath, subDir);

  // Add the additional subdirectories
  va_start(args, subDir);
  while ((currentDir = va_arg(args, char *)) != NULL) {
    pathLength = strlen(newPath);
    if (newPath[pathLength - 1] != delimiter) {
      newPath[pathLength] = delimiter; // add the delimiter
      newPath[pathLength + 1] = 0;     // terminate the string
    }
    strcat(newPath, currentDir);
  }
  va_end(args);
}
bool ddio_GetTempFileName(char *basedir, char *prefix, char *filename) {
  char old_workdir[_MAX_PATH];
  bool success = false;
  if (strlen(prefix) > 64)
    return false;
  ddio_GetWorkingDir(old_workdir, _MAX_PATH);
  if (!ddio_DirExists(basedir)) {
    return false; // invalid base directory
  }
  char randname[10];
  int index;
  int tries = 0;
  char rc;
  bool created = false;
  index = 0;

  while (!success && tries < 20) {
    // generate a bunch of random characters
    rc = (rand() % 128);
    if ((rc >= 'a' && rc <= 'z') || (rc >= 'A' && rc <= 'Z') || (rc >= '0' && rc <= '9')) {
      // valid character
      randname[index] = rc;
      index++;
      if (index == 10) {
        // we hit the size of our max, see if we generated a unique filename
        char t[_MAX_PATH];
        randname[9] = '\0';
        sprintf(t, "%s%s.tmp", prefix, randname);
        // see if we can find this file
        FILE *fd = mac_fopen(t, "rb");
        if (!fd) {
          // we found a good file!
          ddio_MakePath(filename, basedir, t, NULL);
          success = true;
          created = true;
        } else {
          // already taken
          fclose(fd);
          tries++;
          index = 0;
        }
      }
    } else {
      continue; // try again
    }
  }
  ddio_SetWorkingDir(old_workdir);
  return created;
}
//	These functions allow one to find a file
//		You use FindFileStart by giving it a wildcard (like *.*, *.txt, u??.*, whatever).  It returns
//		a filename in namebuf.
//		Use FindNextFile to get the next file found with the wildcard given in FindFileStart.
//		Use FindFileClose to end your search.
// These functions allow one to find a file
// You use FindFileStart by giving it a wildcard (like *.*, *.txt, u??.*, whatever).  It returns
// a filename in namebuf.
// Use FindNextFile to get the next file found with the wildcard given in FindFileStart.
// Use FindFileClose to end your search.
int fileIndex = 1;
char wildsave[10];
bool ddio_FindFileStart(const char *wildcard, char *namebuf) {
  ASSERT(wildcard);
  ASSERT(namebuf);

  fileIndex = 1;
  char tempbuffer[_MAX_PATH];

  if (strrchr(wildcard, ':')) {
    strcpy(tempbuffer, wildcard);

    char *c = strrchr(tempbuffer, '*');
    strcpy(wildsave, c);
    *c = '\0';

    if (!ddio_DirExists(tempbuffer))
      return (false);

  } else {
    strcpy(wildsave, wildcard);
  }

  ddio_SaveWorkingDir();
  ddio_SetWorkingDir(tempbuffer);
  if (ddio_FindNextFile(namebuf))
    return (true);
  else
    return (false);
}
bool ddio_FindNextFile(char *namebuf) {
  short err;
  FileParam fpb;
  CInfoPBRec cpb;
  Str255 volName = "\p";
  short volRefNum = 0;
  long dirID = 0;
  ASSERT(namebuf);
getAnother:
  namebuf[0] = 0;
  err = HGetVol(volName, &volRefNum, &dirID);
  if (err)
    return false;
  /*
          fpb.ioNamePtr=(unsigned char *)namebuf;
          fpb.ioFDirIndex=fileIndex;
          fpb.ioVRefNum=0;
          // retrun just file info
          err = PBGetFInfoSync( (ParmBlkPtr)&fpb );
  */
  // returns both file & dir info
  cpb.hFileInfo.ioVRefNum = volRefNum;
  cpb.hFileInfo.ioDirID = dirID;
  cpb.hFileInfo.ioNamePtr = (unsigned char *)namebuf;
  cpb.hFileInfo.ioFDirIndex = fileIndex;

  err = PBGetCatInfoSync(&cpb);

  //	if((cpb.hFileInfo.io.FlAttrib & (1<<4)))
  if (err == fnfErr) {
    return (false);
  } else {
    p2cstr((unsigned char *)namebuf);
    fileIndex++;
    if (PSGlobMatch(wildsave, namebuf, 0, 0)) {
      return (true);
    } else {
      goto getAnother;
    }
  }
  return (false);
}
void ddio_FindFileClose() {
  ddio_RestoreWorkingDir();
  fileIndex = 1;
  wildsave[0] = NULL;
}
//	 pass in a pathname (could be from ddio_SplitPath), root_path will have the drive name.
void ddio_GetRootFromPath(const char *srcPath, char *root_path) {
  assert(root_path);
  assert(srcPath);
  // the first char should be drive letter, second char should be a :
  //	if(!(srcPath[0]==':')){
  // everything is ok
  //		strncpy(root_path,srcPath,2);
  //		root_path[2] = '\0';
  //	}else{
  // invalid path (no root)
  root_path[0] = '\0';
  //	}
}
//	retrieve root names, free up roots array (allocated with malloc) after use
int ddio_GetFileSysRoots(char **roots, int max_roots) {
#ifdef FIXED
  char buffer[100];
  int ret = GetLogicalDriveStrings(100, buffer);
  if (ret == 0) {
    // there was an error
    return 0;
  }
  if (ret > 100) {
    // we didn't have enough space
    return 0;
  }
  int count = 0;
  bool done = false;
  char *strptr = buffer;
  char *string;
  int strsize;
  while ((count < max_roots) && (!done)) {
    if (*strptr != 0) {
      strsize = strlen(strptr);
      string = roots[count] = (char *)mem_malloc(strsize);
      if (!string)
        break;
      // remove the trailing \ from windows
      strncpy(string, strptr, strsize - 1);
      string[strsize - 1] = '\0';

      strptr += (strsize + 1);
      count++;
    } else
      done = true;
  }
  return count;
#endif // FIXED

  return 0;
}
//	given a path, it cleans it up (if the path is c:\windows\..\dos it would make it c:\dos)
//	srcPath is the original path
//	dest is the finished cleaned path.
//		dest should be at least _MAX_PATH in size
void ddio_CleanPath(char *dest, const char *srcPath) {
  strcpy(dest, srcPath);
  // break the path into directories
  char **directories;
  int dirs;
  int path_length;
  // make sure the path ends with a \ for sanity
  path_length = strlen(dest);
  if (dest[path_length - 1] != ':') {
    dest[path_length] = ':';
    dest[path_length + 1] = '\0';
    path_length++;
  }
  // now divide the full path into seperate NULL terminated strings,counting the number
  // of directories in the process
  dirs = 0;
  char *strptr = dest;
  while (*strptr != '\0') {
    if (*strptr == ':') {
      *strptr = '\0';
      dirs++;
    }
    strptr++;
  }
  // check to make sure we have a directory, if we don't then return the original path given
  if (dirs == 0) {
    strcpy(dest, srcPath);
    return;
  }
  // allocate the memory needed for the seperate strings of each directory
  directories = (char **)mem_malloc(sizeof(char *) * dirs);
  if (!directories) {
    strcpy(dest, srcPath);
    return;
  }
  // now get all the directories, and place into the individual strings
  strptr = dest;
  int count = 0;
  while (count < dirs) {
    directories[count] = mem_strdup(strptr);
    strptr += strlen(strptr) + 1;
    count++;
  }
  // now the fun part, figure out the correct order of the directories
  int *dir_order;
  dir_order = (int *)mem_malloc(sizeof(int) * dirs);
  if (!dir_order) {
    strcpy(dest, srcPath);
    return;
  }
  for (count = 0; count < dirs; count++)
    dir_order[count] = -1; // a -1 means the end of the sequence
  // now build the order based on the indicies
  int curr_index = 0;
  for (count = 0; count < dirs; count++) {

    if (!stricmp(directories[count], "..")) {
      // we have to back up a directory
      curr_index--; // back up
      if (curr_index < 0)
        curr_index = 0;           // can't go further than root
      dir_order[curr_index] = -1; // invalidate current slot
    } else if (stricmp(directories[count], ".")) {
      // we have a normal directory, add it's index
      dir_order[curr_index] = count;
      curr_index++;
    }
  }
  // now rebuild the correct path for use, when we hit -1, we're done
  dest[0] = '\0';
  for (count = 0; count < dirs; count++) {
    if (dir_order[count] == -1)
      break;
    else {
      strcat(dest, directories[dir_order[count]]);
      strcat(dest, ":");
    }
  }
  // now remove trailing \ char
  path_length = strlen(dest);
  if ((path_length > 0) && (dest[path_length - 1] == ':'))
    dest[path_length - 1] = '\0';
  // free up all the allocated memory and we're done
  for (count = 0; count < dirs; count++) {
    if (directories[count])
      mem_free(directories[count]);
  }
  if (directories)
    mem_free(directories);
  if (dir_order)
    mem_free(dir_order);
}
//	given a path (with no filename), it will return the parent path
//	srcPath is the source given path
//	dest is where the parent path will be placed
//	returns true on success
//		dest should be at least _MAX_PATH in length
bool ddio_GetParentPath(char *dest, const char *srcPath) {
  assert(srcPath);
  assert(dest);

  strcpy(dest, srcPath);
  char *c = strrchr(dest, ':');
  *c = '\0';

  return true;
}
// Finds a full path from a relative path
// Parameters:	full_path - filled in with the fully-specified path.  Buffer must be at least _MAX_PATH bytes long
//					rel_path - a path specification, either relative or absolute
// Returns TRUE if successful, FALSE if an error
bool ddio_GetFullPath(char *full_path, const char *rel_path) {
  char old_path[_MAX_PATH];
  ddio_GetWorkingDir(old_path, sizeof(old_path)); // save old directory
  if (!ddio_SetWorkingDir(rel_path))              // try switching to new directory
    return 0;                                     // couldn't switch, so return error
  ddio_GetWorkingDir(full_path, _MAX_PATH);       // get path from the OS
  ddio_SetWorkingDir(old_path);                   // now restore old path
  return 1;
}
// Renames file
// Returns true on success or false on an error
bool ddio_RenameFile(char *oldfile, char *newfile) {
  int rcode = rename(oldfile, newfile);
  if (!rcode)
    return true;
  else
    return false;
}
// Give a volume label to look for, and if it's found returns a path
// If it isn't found, return ""
char *ddio_GetCDDrive(char *vol) {

  bool res = true;

  ddio_SaveWorkingDir();
  res = ddio_DirExists(vol);
  ddio_RestoreWorkingDir();

  if (res)
    return mem_strdup(vol);
  else
    return '\0';
}
int ddio_CheckLockFile(const char *dir) { return 0; }
int ddio_CreateLockFile(const char *dir) { return 0; }
int ddio_DeleteLockFile(const char *dir) { return 0; }
// routine to take a DOS path and turn it into a macintosh
// pathname.
void macify_dospath(const char *dos_path, char *mac_path) {
  assert(dos_path != NULL);
  assert(mac_path != NULL);

  mac_path[0] = 0;

  // Remove leading relative directory references
  int i = 0;
  if (dos_path[i] == '.') {
    if (!strnicmp(dos_path, "..\\", 3)) {
      strcat(mac_path, "::");
      i += 3;
    } else if (!strnicmp(dos_path, ".\\", 2)) {
      strcat(mac_path, ":");
      i += 2;
    }

    while (dos_path[i] == '.') {
      if (!strnicmp(&(dos_path[i]), "..\\", 3)) {
        strcat(mac_path, ":");
        i += 3;
      } else if (!strnicmp(&(dos_path[i]), ".\\", 2)) {
        i += 2;
      }
    }

    strcat(mac_path, &(dos_path[i]));
  } else // no leading relative references
  {
    // Check if this is a full pathname
    if (dos_path[0] == '\\') {
      // Get current volume name
      Str255 volName = "\p";
      short volRefNum = 0;
      long dirID = 0;
      OSErr err = noErr;

      err = HGetVol(volName, &volRefNum, &dirID);
      assert(err == noErr);
      p2cstr(volName);
      strcat(mac_path, (char *)volName);
      strcat(mac_path, ":");
      strcat(mac_path, &(dos_path[1]));
    } else {
      strcat(mac_path, ":");
      strcat(mac_path, dos_path);
    }
  }

  // Leading references have been resolved
  // Now replace remaining '/' with ':'
  char *p = NULL;
  while ((p = strchr(mac_path, '\\')) != NULL) {
    *p = ':';
  }
}

#include <Errors.h>
// #include <Files.h>
#include <Script.h>
#include "file_io.h"

#define text_type 'TEXT'
#define binary_type 'BINA'
#define creator 'GSCe'

FILE *mac_fopen(char *filename, const char *open_mode) {
  __file_modes mode;
  short permission;
  OSErr ioResult;
  short refNum;
  OSType file_type;
  FILE *file;

  FSSpec spec;
  Str255 macName;

  strcpy((char *)macName, filename);
  c2pstr((char *)macName);

  ioResult = FSMakeFSSpec(0, 0L, macName, &spec);

  if (!__get_file_modes(open_mode, &mode))
    return (NULL);

  permission = (mode.io_mode == __read) ? fsRdPerm : fsRdWrPerm;

  ioResult = FSpOpenDF(&spec, permission, &refNum);

  if (ioResult) {
    if (ioResult != fnfErr || mode.open_mode == __must_exist)
      return (NULL);

    file_type = 'GSCf';
    //		file_type = (mode.binary_io) ? binary_type : text_type;

    if (!(ioResult = FSpCreate(&spec, creator, file_type, smSystemScript)))
      ioResult = FSpOpenDF(&spec, permission, &refNum);

    if (ioResult)
      return (NULL);
  } else if (mode.open_mode == __create_or_truncate) {
    ioResult = SetEOF(refNum, 0);

    if (ioResult) {
      FSClose(refNum);
      return (NULL);
    }
  }

  file = __handle_open(refNum, open_mode);

  if (!file)
    FSClose(refNum);

  return (file);
}
FILE *FSp_fopen(ConstFSSpecPtr spec, const char *open_mode, OSType type) {
  __file_modes mode;
  short permission;
  OSErr ioResult;
  short refNum;
  OSType file_type;
  FILE *file;

  if (!__get_file_modes(open_mode, &mode))
    return (NULL);

  permission = (mode.io_mode == __read) ? fsRdPerm : fsRdWrPerm;

  ioResult = FSpOpenDF(spec, permission, &refNum);

  if (ioResult) {
    if (ioResult != fnfErr || mode.open_mode == __must_exist)
      return (NULL);

    if (!(ioResult = FSpCreate(spec, creator, type, smSystemScript)))
      ioResult = FSpOpenDF(spec, permission, &refNum);

    if (ioResult)
      return (NULL);
  } else if (mode.open_mode == __create_or_truncate) {
    ioResult = SetEOF(refNum, 0);

    if (ioResult) {
      FSClose(refNum);
      return (NULL);
    }
  }

  file = __handle_open(refNum, open_mode);

  if (!file)
    FSClose(refNum);

  return (file);
}
