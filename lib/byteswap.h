/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 * Copyright (C) 2024 Descent Developers
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
 * $Logfile: /DescentIII/Main/lib/BYTESWAP.H $
 * $Revision: 8 $
 * $Date: 4/19/00 5:32p $
 * $Author: Matt $
 *
 * Byteswapping macros (for big-endian machines)
 *
 * $Log: /DescentIII/Main/lib/BYTESWAP.H $
 *
 * 8     4/19/00 5:32p Matt
 * From Duane for 1.4
 * Added casts to SWAPSHORT and SWAPINT macros
 *
 * 7     10/21/99 9:27p Jeff
 * B.A. Macintosh code merge
 *
 * 6     5/09/99 11:41p Jeff
 * function free
 *
 * 5     5/05/99 5:27a Jeff
 * renamed endian.h to psendian.h
 *
 * 4     5/01/99 2:52p Jeff
 * added automatic endian detection of the system
 *
 * 3     4/17/99 7:49p Jeff
 * for some reason Linux thinks it's big endian, temp fix (undef) until I
 * get around to writting a endian check function
 *
 * 2     1/09/99 4:38p Jeff
 * added some ifdefs and fixes to get files to compile under Linux
 *
 * 5     5/15/97 2:22p Matt
 * Fixed (hopefully; it's not tested yet) byteswapping for floats
 *
 * 4     2/10/97 2:22p Matt
 * Added cast
 *
 * 3     2/10/97 2:14p Matt
 * Added BIG_ENDIAN define, & INT_FLOAT() macro
 *
 * $NoKeywords: $
 */

#ifndef BYTESWAP_H
#define BYTESWAP_H

#include <cstdint>
#include <cstdlib>

namespace D3 {

// std::byteswap from C++23
template <typename T> constexpr T byteswap(T n) {
  T m;
  for (size_t i = 0; i < sizeof(T); i++)
    reinterpret_cast<uint8_t *>(&m)[i] = reinterpret_cast<uint8_t *>(&n)[sizeof(T) - 1 - i];
  return m;
}

/**
 * Convert integer to/from BE order
 */
template <typename T> constexpr T convert_be(T val) {
#ifndef OUTRAGE_BIG_ENDIAN
  return byteswap(val);
#else
  return (val);
#endif
}

/**
 * Convert integer to/from LE order
 */
template <typename T> constexpr T convert_le(T val) {
#ifndef OUTRAGE_BIG_ENDIAN
  return (val);
#else
  return byteswap(val);
#endif
}

} // namespace D3

// Compatibility macros. Use D3::convert_le / D3::convert_be when possible

#define INTEL_INT(x) D3::convert_le(x)
#define INTEL_SHORT(x) D3::convert_le(x)
#define INTEL_FLOAT(x) D3::convert_le(x)
#define MOTOROLA_INT(x) D3::convert_be(x)
#define MOTOROLA_SHORT(x) D3::convert_be(x)
#define MOTOROLA_FLOAT(x) D3::convert_be(x)

#endif
