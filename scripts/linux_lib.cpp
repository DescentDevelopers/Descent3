#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void _splitpath(const char *path, char *drive, char *dir, char *fname, char *ext);
int _vsnprintf(char *buffer, size_t count, const char *format, va_list argptr);
int stricmp(const char *string1, const char *string2);

void _splitpath(const char *srcPath, char *drive, char *path, char *filename, char *ext) {
  int pathStart = -1;
  int pathEnd = -1;
  int fileStart = -1;
  int fileEnd = -1;
  int extStart = -1;
  int extEnd = -1;

  int totalLen = strlen(srcPath);

  if (drive)
    *drive = '\0';

  // Check for an extension
  ///////////////////////////////////////
  int t = totalLen - 1;
  while ((srcPath[t] != '.') && (srcPath[t] != '/') && (t >= 0))
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
  while ((srcPath[temp] != '/') && (temp >= 0))
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

int _vsnprintf(char *buffer, size_t count, const char *format, va_list argptr) {
  return vsnprintf(buffer, count, format, argptr);
}

int stricmp(const char *string1, const char *string2) { return strcasecmp(string1, string2); }