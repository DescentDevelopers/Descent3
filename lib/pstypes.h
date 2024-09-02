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

#ifndef _TYPES_H
#define _TYPES_H

#include <cstdlib>
#include <cstdint>

// The maximum length for a file name, including extension.  It *does not* include the
// terminating NULL, so a buffer to hold a filename needs to be PSFILENAME_LEN+1 bytes long.
#define PSFILENAME_LEN 35

#if !defined(__APPLE__)
#define HOST_BIGENDIAN @HOST_BIGENDIAN @
#else
#if defined(__BIG_ENDIAN__)
#define HOST_BIGENDIAN 1
#define MACOSXPPC 1
#elif defined(__LITTLE_ENDIAN__)
#define HOST_BIGENDIAN 0
#define MACOSX86 1
#else
#error "Unknown HOST_BIGENDIAN!"
#endif
#endif

#endif
