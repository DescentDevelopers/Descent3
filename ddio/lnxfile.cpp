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

#include <cerrno>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utime.h>

#if MACOSX
#include <ctype.h>
#endif

#include "ddio.h"
#include "log.h"
#include "crossplat.h"
#include "mem.h"
#include "pserror.h"

#define _MAX_DIR 256

//	---------------------------------------------------------------------------
//	File operations

//	retrieve the current working folder where file operation will occur.
void ddio_GetWorkingDir(char *path, int len) { getcwd(path, len); }

bool ddio_SetWorkingDir(const char *path) { return (chdir(path)) ? false : true; }

bool ddio_FileDiff(const std::filesystem::path &path1, const std::filesystem::path &path2) {
  struct stat abuf{}, bbuf{};

  if (stat(PATH_TO_CSTR(path1), &abuf))
    Int3(); // error getting stat info

  if (stat(PATH_TO_CSTR(path2), &bbuf))
    Int3(); // error getting stat info

  if ((abuf.st_size != bbuf.st_size) || (abuf.st_mtime != bbuf.st_mtime))
    return true;
  return false;
}

// get a file's length
int ddio_GetFileLength(FILE *filePtr) {
  int size = -1;
  if (filePtr) {
    int filedes = fileno(filePtr);
    struct stat info;

    fstat(filedes, &info);
    size = info.st_size;
  } else {
    LOG_FATAL << "Tried getting length of NULL fileptr!";
    Int3();
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
  while (t >= 0 && (srcPath[t] != '.') && (srcPath[t] != '/') && (t >= 0))
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
  while ((temp >= 0) && (srcPath[temp] != '/'))
    temp--;
  if (temp < 0)
    temp = 0;
  if (srcPath[temp] == '/') {
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
}

void ddio_CopyFileTime(const std::filesystem::path &dest, const std::filesystem::path &src) {
  struct stat abuf{};

  if (stat(PATH_TO_CSTR(src), &abuf))
    Int3();

  struct utimbuf bbuf{};
  bbuf.actime = abuf.st_atime;
  bbuf.modtime = abuf.st_mtime;

  if (utime(PATH_TO_CSTR(dest), &bbuf))
    Int3();
}

// deletes a file.  Returns 1 if successful, 0 on failure
int ddio_DeleteFile(const char *name) { return (!unlink(name)); }

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
  if (pathLength > 0 && newPath[pathLength - 1] != delimiter) {
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

std::vector<std::filesystem::path> ddio_GetSysRoots() {
  // In Linux assume that all other mount points are accessible via root mount point
  return std::vector<std::filesystem::path> { "/" };
};

//	retrieve root names, free up roots array (allocated with malloc) after use
int ddio_GetFileSysRoots(char **roots, int max_roots) {
  if (max_roots < 1)
    return 0;

  roots[0] = mem_strdup("/");
  return 1;
}

bool ddio_CheckProcess(int pid) {
  if (kill(pid, 0) == -1) {
    /* some other error, log it */
    LOG_WARNING_IF(errno != ESRCH).printf("Error sending signal to PID for lock check (%d)", pid);
    return false;
  } else {
    /* process exists */
    return true;
  }
}

int ddio_GetPID() {
  return getpid();
}
