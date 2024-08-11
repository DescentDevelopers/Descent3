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

 * $Logfile: /DescentIII/Main/module/module.cpp $
 * $Revision: 20 $
 * $Date: 10/21/99 2:43p $
 * $Author: Kevin $
 *
 * Source for Dynamic Loadable Module functions
 *
 * $Log: /DescentIII/Main/module/module.cpp $
 *
 * 20    10/21/99 2:43p Kevin
 * Mac Merge
 *
 * 19    8/24/99 4:33p Jeff
 * fixed bug with finding alternate file names in Linux
 *
 * 18    8/22/99 7:12p Jeff
 * when open a module on a Linux system, if the original open fails, then
 * check for alternate files with different case.
 *
 * 17    8/16/99 11:48a Nate
 * (JEFF) Use calls to dd_ instead of ddio_ (this library cannot be
 * dependant on any other libs)
 *
 * 16    7/28/99 5:22p Kevin
 * Mac Merge fixes
 *
 * 15    7/28/99 2:17p Kevin
 * Macintosh Stuff!
 *
 * 14    5/13/99 5:07p Ardussi
 * changes for compiling on the Mac
 *
 * 13    4/19/99 4:01a Jeff
 * fixed splitpath
 *
 * 12    4/18/99 3:14p Jeff
 * fixed splitpath for linux
 *
 * 11    4/16/99 1:06a Jeff
 *
 * 10    1/19/99 1:08p Jason
 * added dynamically loadable dlls
 *
 * 9     1/14/99 10:36a Jeff
 * removed ddio_ dependency (requires a #ifdef around functions taken out
 * of ddio_)
 *
 * 8     1/13/99 6:50a Jeff
 * made linux friendly (#include case-sensitivity)
 *
 * 7     1/11/99 1:01p Jeff
 * code will convert an .so->.dll for win32 and .dll->.so for Linux
 *
 * 6     1/11/99 12:53p Jeff
 * added a function that given a module name it will make sure it has an
 * extension.  Made Osiris friendly with modules with no extension
 *
 * 5     1/11/99 12:29p Jeff
 * tack on an automatic extension to the module name if one isn't given
 * (system dependent)
 *
 * 4     1/10/99 6:47a Jeff
 * Changes made to get linux version to compile
 *
 * 3     7/06/98 10:45a Jeff
 * Made Linux friendly
 *
 * 2     6/05/98 2:15p Jeff
 * Initial creation
 *
 * 1     6/05/98 2:14p Jeff
 *
 * $NoKeywords: $
 */

#include <algorithm>
#include <filesystem>

#if defined(POSIX)
#include <dlfcn.h>
#include "linux_fix.h"
#endif

#include "ddio.h"
#include "module.h"
#include "pserror.h"

/**
 * Returns fixed case file name to actual case on disk for case-sensitive filesystems (Linux).
 * This is actually copy of cf_FindRealFileNameCaseInsensitive() from CFILE.
 * @param fname the fixed case name to map to reality
 * @param directory optional directory to search within (default - current path)
 * @return filename with actual case name or empty path if there no mapping in filesystem
 * @note This function returns only filename without directory part, i.e.
 * mod_FindRealFileNameCaseInsensitive("test/test.txt") will return only "test.txt" on success.
 */
std::filesystem::path mod_FindRealFileNameCaseInsensitive(const std::filesystem::path &fname,
                                                          const std::filesystem::path &directory = ".") {
  // Dumb check, maybe there already all ok?
  if (exists((directory / fname))) {
    return fname.filename();
  }

  std::filesystem::path result, search_path, search_file;

  search_path = directory / fname.parent_path();
  search_file = fname.filename();

  // If directory does not exist, nothing to search.
  if (!std::filesystem::is_directory(search_path) || search_file.empty()) {
    return {};
  }

  // Search component in search_path
  auto const &it = std::filesystem::directory_iterator(search_path);

  auto found = std::find_if(it, end(it), [&search_file, &search_path, &result](const auto& dir_entry) {
    return stricmp(dir_entry.path().filename().u8string().c_str(), search_file.u8string().c_str()) == 0;
  });

  if (found != end(it)) {
    // Match, append to result
    result = found->path();
    search_path = result;
  } else {
    // Component not found, mission failed
    return {};
  }

  return result.filename();
}

int ModLastError = MODERR_NOERROR;
//	Returns the real name of the module.  If a given file has an extension, it will
//	just return that filename.  If the given file has no given extension, the
//	system specific extension is concatted and returned.
void mod_GetRealModuleName(const char *modfilename, char *realmodfilename) {
  char pathname[_MAX_PATH], filename[_MAX_FNAME], extension[_MAX_EXT];
  ddio_SplitPath(modfilename, pathname, filename, extension);
  if (*extension == '\0')
#if defined(WIN32)
    strcat(filename, ".dll");
#elif defined(__LINUX__)
    strcat(filename, ".so");
#elif defined(MACOSX)
    strcat(filename, ".dylib");
#else
    #error Unsupported platform!
#endif
  else {
#if defined(WIN32)
    if (!stricmp(extension, ".so") || !stricmp(extension, "msl") || !stricmp(extension, "dylib"))
      strcat(filename, ".dll");
    else
      strcat(filename, extension);
#elif defined(__LINUX__)
    if (!stricmp(extension, ".dll") || !stricmp(extension, "msl") || !stricmp(extension, "dylib"))
      strcat(filename, ".so");
    else
      strcat(filename, extension);
#elif defined(MACOSX)
      if (!stricmp(extension, ".dll") || !stricmp(extension, "msl") || !stricmp(extension, "so"))
        strcat(filename, ".dylib");
      else
        strcat(filename, extension);
#else
  #error Unsupported platform!
#endif
  }
  if (*pathname != '\0')
    ddio_MakePath(realmodfilename, pathname, filename, NULL);
  else
    strcpy(realmodfilename, filename);
}
// Loads a dynamic module into memory for use.
// Returns true on success, false otherwise
// modfilename is the name of the module (without an extension such as DLL, or so)
bool mod_LoadModule(module *handle, const char *imodfilename, int flags) {
  if (!imodfilename) {
    ModLastError = MODERR_OTHER;
    return false;
  }
  if (!handle) {
    ModLastError = MODERR_INVALIDHANDLE;
    return false;
  }
  handle->handle = NULL;
  char modfilename[_MAX_PATH];
  mod_GetRealModuleName(imodfilename, modfilename);
#if defined(WIN32)
  handle->handle = LoadLibrary(modfilename);
  if (!handle->handle) {
    // There was an error loading the module
    DWORD err = GetLastError();
    switch (err) {
    case ERROR_DLL_INIT_FAILED:
      ModLastError = MODERR_MODINITFAIL;
      break;
    case ERROR_DLL_NOT_FOUND:
      ModLastError = MODERR_MODNOTFOUND;
      break;
    case ERROR_INVALID_DLL:
      ModLastError = MODERR_INVALIDMOD;
      break;
    default:
      ModLastError = MODERR_OTHER;
      break;
    }
    return false;
  }
#elif defined(POSIX)
  int f = 0;
  if (flags & MODF_LAZY)
    f |= RTLD_LAZY;
  if (flags & MODF_NOW)
    f |= RTLD_NOW;
  if (flags & MODF_GLOBAL)
    f |= RTLD_GLOBAL;
  handle->handle = dlopen(modfilename, f);
  if (!handle->handle) {
    // ok we couldn't find the given name...try other ways
    char dir[_MAX_PATH], fname[_MAX_PATH], nname[_MAX_PATH], ext[64];
    ddio_SplitPath(modfilename, dir, fname, ext);
    strcat(fname, ext);

    std::filesystem::path new_filename = mod_FindRealFileNameCaseInsensitive(dir, fname);

    if (new_filename.empty()) {
      mprintf(0, "Module Load Err: %s\n", dlerror());
      ModLastError = MODERR_MODNOTFOUND;
      return false;
    }

    // ok we have a different filename
    ddio_MakePath(modfilename, dir, nname, NULL);
    mprintf(0, "MOD: Attempting to open %s instead of %s\n", modfilename, fname);
    handle->handle = dlopen(modfilename, f);
    if (!handle->handle) {
      mprintf(0, "Module Load Err: %s\n", dlerror());
      ModLastError = MODERR_MODNOTFOUND;
      return false;
    }
  }
#endif
  // Success
  return true;
}
// Frees a previously loaded module from memory, it can no longer be used
// Returns true on success, false otherwise
bool mod_FreeModule(module *handle) {
  bool ret = true;

  if (!handle) {
    ModLastError = MODERR_INVALIDHANDLE;
    return false;
  }
  if (!handle->handle) {
    ModLastError = MODERR_OTHER;
    return false;
  }
#if defined(WIN32)
  ret = (FreeLibrary(handle->handle) != 0);
#elif defined(POSIX)
  dlclose(handle->handle); // dlclose() returns an int, but no docs say what values!!!
#endif
  handle->handle = NULL;
  return ret;
}
// Returns a pointer to a function within a loaded module.  If it returns NULL there was an error.  Check
// mod_GetLastError to see if there was an error symstr is the name of the function you want to get the symbol for (Do
// NOT give any pre/suffix to this name) parmbytes is the size (in bytes) of the parameter list the function should have
MODPROCADDRESS mod_GetSymbol(module *handle, const char *symstr, uint8_t parmbytes) {
  char buffer[256];
  MODPROCADDRESS sym;
  if (!handle) {
    ModLastError = MODERR_INVALIDHANDLE;
    return NULL;
  }
  if (!symstr) {
    ModLastError = MODERR_OTHER;
    return NULL;
  }
  if (!handle->handle) {
    ModLastError = MODERR_NOMOD;
    return NULL;
  }
#if defined(WIN32)
  // We need to first form the correct symbol name (for Windows)
  if (parmbytes == 255)
    sprintf(buffer, "%s", symstr);
  else
    sprintf(buffer, "_%s@%d", symstr, parmbytes);
  DWORD err;
  sym = GetProcAddress(handle->handle, buffer);
  if (!sym) {
    err = GetLastError();

    // Try again using no byte ordinals
    if (parmbytes != 255) {
      sprintf(buffer, "%s", symstr);
      sym = GetProcAddress(handle->handle, buffer);
      if (!sym)
        err = GetLastError();
    }
  }
  if (!sym) {
    // there was an error

    switch (err) {
    case ERROR_DLL_INIT_FAILED:
      ModLastError = MODERR_MODINITFAIL;
      break;
    case ERROR_DLL_NOT_FOUND:
      ModLastError = MODERR_MODNOTFOUND;
      break;
    case ERROR_INVALID_DLL:
      ModLastError = MODERR_INVALIDMOD;
      break;
    default:
      ModLastError = MODERR_OTHER;
      break;
    }
    return NULL;
  }
#elif defined(POSIX)
  sym = dlsym(handle->handle, symstr);
  if (!sym) {
    ModLastError = MODERR_OTHER;
    return NULL;
  }
#endif
  return sym;
}
// Returns an error code to what the last error was.  When this function is called the last error is cleared, so by
// calling this function it not only returns the last error, but it removes it, so if you were to call this function
// again, it would return no error
int mod_GetLastError(void) {
  // Clear out the errors
#if defined(WIN32)
  SetLastError(0);
#elif defined(POSIX)
  dlerror();
#endif
  int ret = ModLastError;
  ModLastError = MODERR_NOERROR;
  return ret;
}
