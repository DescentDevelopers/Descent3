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
*/

/*
 * $Logfile: /DescentIII/Main/ddio_lnx/lnxfile.cpp $
 * $Revision: 1.5 $
 * $Date: 2004/12/05 09:02:45 $
 * $Author: ryan $
 *
 * Linux file routines
 *
 * $Log: lnxfile.cpp,v $
 * Revision 1.5  2004/12/05 09:02:45  ryan
 * Valgrind fix.
 *
 * Revision 1.4  2004/02/25 00:04:06  ryan
 * Removed loki_utils dependency and ported to MacOS X (runs, but incomplete).
 *
 * Revision 1.3  2001/01/11 11:41:57  icculus
 * Allow .dotfiles to be shown in file dialogs.
 *
 * Revision 1.2  2000/06/24 01:15:15  icculus
 * patched to compile.
 *
 * Revision 1.1.1.1  2000/04/18 00:00:33  icculus
 * initial checkin
 *
 *
 * 13    10/02/99 2:59p Jeff
 * moved ddio_GetCDDrive to lnxcdrom.cpp
 *
 * 12    9/07/99 4:36p Jeff
 * fixed ddio_FindFileFirst/Next when a directory is returned
 *
 * 11    8/22/99 7:10p Jeff
 * fix directory lock function if the pid check returns an error, now
 * assume the pid is invalid
 *
 * 10    7/14/99 9:06p Jeff
 * added comment header
 *
 * $NoKeywords: $
 */

#include "ddio.h"
#include "ddio_lnx.h"
#include "pserror.h"
#include "mem.h"
#include "lnxfix.h"

#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <utime.h>
#include <glob.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <dirent.h>
#include <ctype.h>

#define _MAX_DIR 256

//	---------------------------------------------------------------------------
//	File operations

// get a file's length
int ddio_GetFileLength(FILE *filePtr) {
  int size = -1;
  if (filePtr) {
    int filedes = fileno(filePtr);
    struct stat info;

    fstat(filedes, &info);
    size = info.st_size;
  } else {
    mprintf((0, "Tried getting length of NULL fileptr!\n"));
    Int3();
  }
  return size;
}

// rcg06192000 extern "C" is my add, so nettest would link.
// extern "C"
//{
// Constructs a path in the local file system's syntax
// 	newPath: stores the constructed path
//  absolutePathHeader: absolute path on which the sub directories will be appended
//						(specified in local file system syntax)
//  takes a variable number of subdirectories which will be concatenated on to the path
//		the last argument in the list of sub dirs *MUST* be NULL to terminate the list
void ddio_MakePath(char *newPath, const char *absolutePathHeader, const char *subDir, ...) {
  const char delimiter = '/';
  va_list args;
  char *currentDir = NULL;
  int pathLength = 0;

  ASSERT(newPath);
  ASSERT(absolutePathHeader);
  ASSERT(subDir);

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

//}

// These functions allow one to find a file
// You use FindFileStart by giving it a wildcard (like *.*, *.txt, u??.*, whatever).  It returns
// a filename in namebuf.
// Use FindNextFile to get the next file found with the wildcard given in FindFileStart.
// Use FindFileClose to end your search.
glob_t ffres = {0, NULL, 0};
int globindex = -1;
int globerrfn(const char *path, int err) {
  mprintf((0, "Error accessing %s: %s .... \n", path, strerror(err)));
  return 0;
}

#if MACOSX
int noglob_findnext(struct find_t *f);
struct find_t {
  DIR *dir;
  char pattern[_MAX_PATH];
  char name[_MAX_PATH];
};

static find_t finder;

int noglob_findfirst(char *filename, int x, struct find_t *f) {
  char *ptr;

  if (strlen(filename) >= sizeof(f->pattern))
    return (1);

  strcpy(f->pattern, filename);
  // FixFilePath(f->pattern);
  ptr = strrchr(f->pattern, '/');

  if (ptr == NULL) {
    ptr = filename;
    f->dir = opendir(".");
  } else {
    *ptr = '\0';
    f->dir = opendir(f->pattern);
    memmove(f->pattern, ptr + 1, strlen(ptr + 1) + 1);
  }

  return (noglob_findnext(f));
}

static int check_pattern_nocase(const char *x, const char *y) {
  if ((x == NULL) || (y == NULL))
    return (0); /* not a match. */

  while ((*x) && (*y)) {
    if (*x == '*') {
      x++;
      while (*y != '\0') {
        if (toupper((int)*x) == toupper((int)*y))
          break;
        y++;
      }
    }

    else if (*x == '?') {
      if (*y == '\0')
        return (0); /* anything but EOS is okay. */
    }

    else {
      if (toupper((int)*x) != toupper((int)*y))
        return (0); /* not a match. */
    }

    x++;
    y++;
  }

  return (*x == *y); /* it's a match (both should be EOS). */
}

int noglob_findnext(struct find_t *f) {
  struct dirent *dent;

  if (f->dir == NULL)
    return (1); /* no such dir or we're just done searching. */

  while ((dent = readdir(f->dir)) != NULL) {
    if (check_pattern_nocase(f->pattern, dent->d_name)) {
      if (strlen(dent->d_name) < sizeof(f->name)) {
        strcpy(f->name, dent->d_name);
        return (0); /* match. */
      }
    }
  }

  // closedir(f->dir);
  // f->dir = NULL;
  return (1); /* no match in whole directory. */
}
#endif

bool ddio_FindFileStart(const char *wildcard, char *namebuf) {
  ASSERT(wildcard);
  ASSERT(namebuf);
  if (globindex != -1)
    ddio_FindFileClose();

#if MACOSX
  if (noglob_findfirst((char *)wildcard, 0, &finder) == 0) {
    strcpy(namebuf, finder.name);
    return true;
  }
  return false;
#else
  int rc, flags;
  flags = GLOB_MARK | GLOB_PERIOD | GLOB_TILDE;
  rc = glob(wildcard, flags, globerrfn, &ffres);
  if (rc == GLOB_NOSPACE) {
    mprintf((0, "Out of space during glob\n"));
    globindex = -1;
    return false;
  }
  if (!ffres.gl_pathc) {
    globindex = -1;
    return false;
  }

  globindex = 0;

  // if the last character is a / then we are to return that last section (it is a directory and
  // ddio_splitpath can't handle that).
  int gllen = strlen(ffres.gl_pathv[0]);
  if (ffres.gl_pathv[0][gllen - 1] == '/') {
    // directory to return!
    char *ptr = &ffres.gl_pathv[0][gllen - 2];
    char *start = &ffres.gl_pathv[0][0];
    char *end = ptr;
    while (ptr >= start) {
      if (*ptr == '/') {
        // stop! it's the end of the directory
        break;
      }
      ptr--;
    }
    if (ptr < start)
      ptr = start;
    if (*ptr == '/')
      ptr++;

    start = namebuf;
    while (ptr <= end) {
      *start = *ptr;
      start++;
      ptr++;
    }
    *start = '\0';
  } else {
    // filename to return
    char ext[256];
    ddio_SplitPath(ffres.gl_pathv[0], NULL, namebuf, ext);
    strcat(namebuf, ext);
  }
  return true;
#endif
}

bool ddio_FindNextFile(char *namebuf) {
  ASSERT(namebuf);
#if MACOSX
  if (noglob_findnext(&finder) == 0) {
    strcpy(namebuf, finder.name);
    return true;
  }
  return false;
#else
  if (globindex == -1)
    return false;
  globindex++;
  if (globindex >= ffres.gl_pathc)
    return false;

  // if the last character is a / then we are to return that last section (it is a directory and
  // ddio_splitpath can't handle that).
  int gllen = strlen(ffres.gl_pathv[globindex]);
  if (ffres.gl_pathv[globindex][gllen - 1] == '/') {
    // directory to return!
    char *ptr = &ffres.gl_pathv[globindex][gllen - 2];
    char *start = &ffres.gl_pathv[globindex][0];
    char *end = ptr;
    while (ptr >= start) {
      if (*ptr == '/') {
        // stop! it's the end of the directory
        break;
      }
      ptr--;
    }
    if (ptr < start)
      ptr = start;
    if (*ptr == '/')
      ptr++;

    start = namebuf;
    while (ptr <= end) {
      *start = *ptr;
      start++;
      ptr++;
    }
    *start = '\0';
  } else {
    // filename to return
    char ext[256];
    ddio_SplitPath(ffres.gl_pathv[globindex], NULL, namebuf, ext);
    strcat(namebuf, ext);
  }

  return true;
#endif
}

void ddio_FindFileClose() {
#if MACOSX
  if (finder.dir != NULL) {
    closedir(finder.dir);
    finder.dir = NULL;
  }
#else
  if (globindex == -1)
    return;
  globindex = -1;
  globfree(&ffres);
#endif
}

glob_t fdres = {0, NULL, 0};
int globdirindex = -1;
bool ddio_FindDirStart(const char *wildcard, char *namebuf) {
  ASSERT(wildcard);
  ASSERT(namebuf);
  if (globdirindex != -1)
    ddio_FindDirClose();
  int rc, flags;
  flags = GLOB_MARK; // put a / to the end of directories, making it easier to find em
  rc = glob(wildcard, flags, globerrfn, &fdres);
  if (rc == GLOB_NOSPACE) {
    mprintf((0, "Out of space during glob\n"));
    namebuf[0] = '\0';
    globdirindex = -1;
    return false;
  }

  if (!fdres.gl_pathc) {
    namebuf[0] = '\0';
    globdirindex = -1;
    return false;
  }

  globdirindex = 0;
  int glpathlen = strlen(fdres.gl_pathv[0]);

  if (fdres.gl_pathv[0][glpathlen - 1] == '/') {
    // return only the last directory
    fdres.gl_pathv[0][glpathlen - 1] = '\0'; // lose the ending '/'
    char *endptr = &fdres.gl_pathv[0][glpathlen - 1];
    while (endptr >= fdres.gl_pathv[0] && *endptr != '/')
      endptr--;
    if (*endptr == '/') {
      strcpy(namebuf, endptr + 1);
    } else {
      strcpy(namebuf, fdres.gl_pathv[0]);
    }
    return true;
  } else {
    return ddio_FindNextDir(namebuf);
  }

  namebuf[0] = '\0';
  return false;
}

bool ddio_FindNextDir(char *namebuf) {
  ASSERT(namebuf);
  if (globdirindex == -1)
    return false;
  globdirindex++;
  if (globdirindex >= fdres.gl_pathc)
    return false;

  bool done = false;
  bool found = false;

  while (!done) {
    if (globdirindex >= fdres.gl_pathc) {
      namebuf[0] = '\0';
      done = true;
      continue;
    }
    int glpathlen = strlen(fdres.gl_pathv[globdirindex]);
    if (fdres.gl_pathv[globdirindex][glpathlen - 1] == '/') {
      // return only the last directory
      fdres.gl_pathv[globdirindex][glpathlen - 1] = '\0'; // lose the ending '/'
      char *endptr = &fdres.gl_pathv[globdirindex][glpathlen - 1];
      while (endptr >= fdres.gl_pathv[globdirindex] && *endptr != '/')
        endptr--;
      if (*endptr == '/') {
        strcpy(namebuf, endptr + 1);
      } else {
        strcpy(namebuf, fdres.gl_pathv[globdirindex]);
      }
      done = true;
      found = true;
    } else {
      globdirindex++;
    }
  }

  return found;
}

void ddio_FindDirClose() {
  if (globdirindex == -1)
    return;
  globdirindex = -1;
  globfree(&fdres);
}

//	 pass in a pathname (could be from ddio_SplitPath), root_path will have the drive name.
void ddio_GetRootFromPath(const char *srcPath, char *root_path) {
  assert(root_path);
  assert(srcPath);

  strcpy(root_path, "/");
}

//	retrieve root names, free up roots array (allocated with malloc) after use
int ddio_GetFileSysRoots(char **roots, int max_roots) {
  if (max_roots < 1)
    return 0;

  roots[0] = mem_strdup("/");
  return 1;
}

//	given a path (with no filename), it will return the parent path
//	srcPath is the source given path
//	dest is where the parent path will be placed
//	returns true on success
//		dest should be at least _MAX_PATH in length
bool ddio_GetParentPath(char *dest, const char *srcPath) {
  assert(srcPath);
  assert(dest);

#define PARENT_DELIM ".."
  int spath_len = strlen(srcPath);
  char *temp;

  temp = (char *)mem_malloc(spath_len + strlen(PARENT_DELIM) + 3);
  if (!temp) {
    return false;
  }

  ddio_MakePath(temp, srcPath, PARENT_DELIM, NULL);
  ddio_CleanPath(dest, temp);
  mem_free(temp);
  return true;
}

// Generates a temporary filename based on the prefix, and basedir
// Parameters:
//		basedir - directory to put the files
//		prefix - prefix for the temp filename
//		filename - buffer to hold generated filename (must be at least _MAX_PATH in length)
//
// Returns TRUE if successful, FALSE if an error
bool ddio_GetTempFileName(char *basedir, char *prefix, char *filename) {
  char old_workdir[_MAX_PATH];
  bool success = false;

  if (strlen(prefix) > 64)
    return false;

  ddio_GetWorkingDir(old_workdir, _MAX_PATH);

  if (!ddio_SetWorkingDir(basedir)) {
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
        snprintf(t, sizeof(t), "%s%s.tmp", prefix, randname);

        // see if we can find this file
        FILE *fd = fopen(t, "rb");
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

// Checks to see if a lock file is located in the specified directory.
//	Parameters:
//		dir		Directory for which the lock file should be checked
//	Returns:
//		1		Lock file doesn't exist
//		2		Lock file was in a directory, but it belonged to a process that no longer
//				exists, so a lock file _can_ be made in the directory.
//		3		Lock file for this process already exists
//		0		Lock file currently exists in directory
//		-1		Illegal directory
//		-2		There is a lock file in the directory, but it is in an illegal format
int ddio_CheckLockFile(std::filesystem::path const& dir) {
  pid_t curr_pid = getpid();

  auto lockfilepath = dir / ".lock";
  bool found_lock_file_in_dir = false;
  FILE *file;

  mprintf((0, "LockFile: Checking [%s]...", lockfilepath.c_str()));
  chmod(lockfilepath.c_str(), S_IREAD | S_IWRITE);
  file = fopen(lockfilepath.c_str(), "rb");

  if (!file) {
    // File exists, but couldn't open it
    if (errno == EACCES) {
      //			ddio_SetWorkingDir(old_directory);
      return -2;
    }

    found_lock_file_in_dir = false;
  } else {
    found_lock_file_in_dir = true;

    // check the file, see if it is a lock file
    char c;
    //		c = fgetc(file); if(c!='L')	{fclose(file); ddio_SetWorkingDir(old_directory); return -2;}
    //		c = fgetc(file); if(c!='O')	{fclose(file); ddio_SetWorkingDir(old_directory); return -2;}
    //		c = fgetc(file); if(c!='C')	{fclose(file); ddio_SetWorkingDir(old_directory); return -2;}
    //		c = fgetc(file); if(c!='K')	{fclose(file); ddio_SetWorkingDir(old_directory); return -2;}
    c = fgetc(file);
    if (c != 'L') {
      fclose(file);
      return -2;
    }
    c = fgetc(file);
    if (c != 'O') {
      fclose(file);
      return -2;
    }
    c = fgetc(file);
    if (c != 'C') {
      fclose(file);
      return -2;
    }
    c = fgetc(file);
    if (c != 'K') {
      fclose(file);
      return -2;
    }

    // it appears to be a lock file, check the pid
    pid_t f_pid;
    fread(&f_pid, sizeof(pid_t), 1, file);
    fclose(file);

    // check the file id in the file, compared to our pid
    if (f_pid == curr_pid) {
      // lock file already exists for the current process
      //			ddio_SetWorkingDir(old_directory);
      return 3;
    }

    if (kill(f_pid, 0) == -1) {
      if (errno == ESRCH) {
        /* pid does not exist */

      } else {
        /* some other error */
        // technically this shouldn't happen, but I get it
        // when the pid no longer exists...so I'm going to
        // pretend it doesn't
        mprintf((0, "Error sending signal to pid for lock check (%d)\n", f_pid));
        // perror ("Error sending signal to pid for lock check, maybe remove lock file in temp directory");
        // ddio_SetWorkingDir(old_directory);
        // return 0;
      }

    } else {
      /* pid exists */
      //			ddio_SetWorkingDir(old_directory);
      return 0;
    }

    // the process no longer exists, we can create a lock file if needed
    // we'll delete the useless one now
    ddio_DeleteFile(lockfilepath);
  }

  //	ddio_SetWorkingDir(old_directory);
  return (found_lock_file_in_dir) ? 2 : 1;
}

// Creates a lock file in the specified directory
//	Parameters:
//		dir		Directory for which the lock file should be created in
//	Returns:
//		1		Lock file created
//		2		Lock file created (there was a lock file in that directory, but it belonged
//				to a process that no longer exists)
//		3		Lock file for this process already exists
//		0		Lock file not created, a lock file currently exists in the directory
//		-1		Illegal directory
//		-2		There is a lock file in the directory, but it is in an illegal format
//		-3		Unable to create lock file
int ddio_CreateLockFile(std::filesystem::path const& dir) {
  int result = ddio_CheckLockFile(dir);
  switch (result) {
  case 0:
  case -1:
  case -2:
  case 3:
    return result;
  };

  pid_t curr_pid = getpid();
  char old_directory[_MAX_PATH];
  ddio_GetWorkingDir(old_directory, _MAX_PATH);
  if (!ddio_SetWorkingDir(dir))
    return -1;

  FILE *file;
  file = fopen(".lock", "wb");
  if (!file) {
    ddio_SetWorkingDir(old_directory);
    return -3;
  }

  fputc('L', file);
  fputc('O', file);
  fputc('C', file);
  fputc('K', file);
  fwrite(&curr_pid, sizeof(pid_t), 1, file);

  fclose(file);
  ddio_SetWorkingDir(old_directory);

  // at this point result will either be 1 or 2 from checking the lock file
  // either way, a lock file has been created
  return result;
}

// Deletes a lock file (for the current process) in the specified directory
//	Parameters:
//		dir		Directory for which the lock file should be deleted from
//	Returns:
//		1		Lock file deleted
//		0		Lock file not deleted, the lock file in the directory does not belong to our
//				process
//		-1		Illegal directory
//		-2		A lock file exists in the directory, but wasn't deleted...illegal format
//		-3		Unable to delete file
int ddio_DeleteLockFile(std::filesystem::path const& dir) {
  pid_t curr_pid = getpid();
  char old_directory[_MAX_PATH];
  ddio_GetWorkingDir(old_directory, _MAX_PATH);
  if (!ddio_SetWorkingDir(dir))
    return -1;

  FILE *file;

  chmod(".lock", S_IREAD | S_IWRITE);
  file = fopen(".lock", "rb");

  if (!file)
    return 1;

  // check the file, see if it is a lock file
  char c;
  c = fgetc(file);
  if (c != 'L') {
    fclose(file);
    ddio_SetWorkingDir(old_directory);
    return -2;
  }
  c = fgetc(file);
  if (c != 'O') {
    fclose(file);
    ddio_SetWorkingDir(old_directory);
    return -2;
  }
  c = fgetc(file);
  if (c != 'C') {
    fclose(file);
    ddio_SetWorkingDir(old_directory);
    return -2;
  }
  c = fgetc(file);
  if (c != 'K') {
    fclose(file);
    ddio_SetWorkingDir(old_directory);
    return -2;
  }

  // it appears to be a lock file, check the pid
  pid_t f_pid;
  fread(&f_pid, sizeof(pid_t), 1, file);
  fclose(file);

  // check the file id in the file, compared to our pid
  if (f_pid != curr_pid) {
    // it doesn't belong to
    ddio_SetWorkingDir(old_directory);
    return 0;
  }

  // the lock file in the directory belongs to us!
  if (!ddio_DeleteFile(".lock")) {
    ddio_SetWorkingDir(old_directory);
    return -3;
  }

  ddio_SetWorkingDir(old_directory);

  return 1;
}
