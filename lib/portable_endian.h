/*
-* Descent 3
-* Copyright (C) 2024 Parallax Software
-*
-* This program is free software: you can redistribute it and/or modify
-* it under the terms of the GNU General Public License as published by
-* the Free Software Foundation, either version 3 of the License, or
-* (at your option) any later version.
-*
-* This program is distributed in the hope that it will be useful,
-* but WITHOUT ANY WARRANTY; without even the implied warranty of
-* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-* GNU General Public License for more details.
-*
-* You should have received a copy of the GNU General Public License
-* along with this program.  If not, see <http://www.gnu.org/licenses/>.
-*/

#ifndef PORTABLE_ENDIAN_H
#define PORTABLE_ENDIAN_H

#if defined(__linux__) || defined(__CYGWIN__)
/* Define necessary macros for the header to expose all fields. */
# ifndef _BSD_SOURCE
#  define _BSD_SOURCE
# endif
# ifndef __USE_BSD
#  define __USE_BSD
# endif
# ifndef _DEFAULT_SOURCE
#  define _DEFAULT_SOURCE
# endif
# include <endian.h>
# include <features.h>
/* See http://linux.die.net/man/3/endian */
# if !defined(__GLIBC__) || !defined(__GLIBC_MINOR__) || ((__GLIBC__ < 2) || ((__GLIBC__ == 2) && (__GLIBC_MINOR__ < 9)))
#  include <arpa/inet.h>
#  if defined(__BYTE_ORDER) && (__BYTE_ORDER == __LITTLE_ENDIAN)
#   define htobe16(x) htons(x)
#   define htole16(x) (x)
#   define be16toh(x) ntohs(x)
#   define le16toh(x) (x)

#   define htobe32(x) htonl(x)
#   define htole32(x) (x)
#   define be32toh(x) ntohl(x)
#   define le32toh(x) (x)

#   define htobe64(x) (((uint64_t)htonl(((uint32_t)(((uint64_t)(x)) >> 32)))) | (((uint64_t)htonl(((uint32_t)(x)))) << 32))
#   define htole64(x) (x)
#   define be64toh(x) (((uint64_t)ntohl(((uint32_t)(((uint64_t)(x)) >> 32)))) | (((uint64_t)ntohl(((uint32_t)(x)))) << 32))
#   define le64toh(x) (x)
#  elif defined(__BYTE_ORDER) && (__BYTE_ORDER == __BIG_ENDIAN)
#   define htobe16(x) (x)
#   define htole16(x) ((((((uint16_t)(x)) >> 8))|((((uint16_t)(x)) << 8)))
#   define be16toh(x) (x)
#   define le16toh(x) ((((((uint16_t)(x)) >> 8))|((((uint16_t)(x)) << 8)))

#   define htobe32(x) (x)
#   define htole32(x) (((uint32_t)htole16(((uint16_t)(((uint32_t)(x)) >> 16)))) | (((uint32_t)htole16(((uint16_t)(x)))) << 16))
#   define be32toh(x) (x)
#   define le32toh(x) (((uint32_t)le16toh(((uint16_t)(((uint32_t)(x)) >> 16)))) | (((uint32_t)le16toh(((uint16_t)(x)))) << 16))

#   define htobe64(x) (x)
#   define htole64(x) (((uint64_t)htole32(((uint32_t)(((uint64_t)(x)) >> 32)))) | (((uint64_t)htole32(((uint32_t)(x)))) << 32))
#   define be64toh(x) (x)
#   define le64toh(x) (((uint64_t)le32toh(((uint32_t)(((uint64_t)(x)) >> 32)))) | (((uint64_t)le32toh(((uint32_t)(x)))) << 32))
#  else
#   error byte order not supported
#  endif
# endif
#elif defined(__APPLE__)

# include <libkern/OSByteOrder.h>

# define htobe16(x) OSSwapHostToBigInt16(x)
# define htole16(x) OSSwapHostToLittleInt16(x)
# define be16toh(x) OSSwapBigToHostInt16(x)
# define le16toh(x) OSSwapLittleToHostInt16(x)

# define htobe32(x) OSSwapHostToBigInt32(x)
# define htole32(x) OSSwapHostToLittleInt32(x)
# define be32toh(x) OSSwapBigToHostInt32(x)
# define le32toh(x) OSSwapLittleToHostInt32(x)

# define htobe64(x) OSSwapHostToBigInt64(x)
# define htole64(x) OSSwapHostToLittleInt64(x)
# define be64toh(x) OSSwapBigToHostInt64(x)
# define le64toh(x) OSSwapLittleToHostInt64(x)

# define __BYTE_ORDER    BYTE_ORDER
# define __BIG_ENDIAN    BIG_ENDIAN
# define __LITTLE_ENDIAN LITTLE_ENDIAN
# define __PDP_ENDIAN    PDP_ENDIAN

#elif defined(__OpenBSD__) || defined(__FreeBSD__)

# include <endian.h>

# define __BYTE_ORDER    BYTE_ORDER
# define __BIG_ENDIAN    BIG_ENDIAN
# define __LITTLE_ENDIAN LITTLE_ENDIAN
# define __PDP_ENDIAN    PDP_ENDIAN

#elif defined(__NetBSD__) || defined(__DragonFly__)

# include <sys/endian.h>

# define be16toh(x) betoh16(x)
# define le16toh(x) letoh16(x)

# define be32toh(x) betoh32(x)
# define le32toh(x) letoh32(x)

# define be64toh(x) betoh64(x)
# define le64toh(x) letoh64(x)

#elif (defined(_WIN16) || defined(_WIN32) || defined(_WIN64)) && !defined(__WINDOWS__)

#include <stdlib.h>
# ifdef __GNUC__
#  include <sys/param.h>
# endif

# if BYTE_ORDER == LITTLE_ENDIAN

#  define htobe16(x) _byteswap_ushort(x)
#  define htole16(x) (x)
#  define be16toh(x) _byteswap_ushort(x)
#  define le16toh(x) (x)

#  define htobe32(x) _byteswap_ulong(x)
#  define htole32(x) (x)
#  define be32toh(x) _byteswap_ulong(x)
#  define le32toh(x) (x)

#  define htobe64(x) _byteswap_uint64(x)
#  define htole64(x) (x)
#  define be64toh(x) _byteswap_uint64(x)
#  define le64toh(x) (x)

# elif BYTE_ORDER == BIG_ENDIAN
/* that would be xbox 360 */
#  define htobe16(x) (x)
#  define htole16(x) __builtin_bswap16(x)
#  define be16toh(x) (x)
#  define le16toh(x) __builtin_bswap16(x)

#  define htobe32(x) (x)
#  define htole32(x) __builtin_bswap32(x)
#  define be32toh(x) (x)
#  define le32toh(x) __builtin_bswap32(x)

#  define htobe64(x) (x)
#  define htole64(x) __builtin_bswap64(x)
#  define be64toh(x) (x)
#  define le64toh(x) __builtin_bswap64(x)

# else
#  error byte order not supported
# endif

# define __BYTE_ORDER    BYTE_ORDER
# define __BIG_ENDIAN    BIG_ENDIAN
# define __LITTLE_ENDIAN LITTLE_ENDIAN
# define __PDP_ENDIAN    PDP_ENDIAN

#elif defined(__QNXNTO__)

# include <gulliver.h>

# define __LITTLE_ENDIAN 1234
# define __BIG_ENDIAN    4321
# define __PDP_ENDIAN    3412

# if defined(__BIGENDIAN__)

#  define __BYTE_ORDER __BIG_ENDIAN
#  define htole16(x) ENDIAN_SWAP16(x)
#  define htole32(x) ENDIAN_SWAP32(x)
#  define htole64(x) ENDIAN_SWAP64(x)
#  define htobe16(x) (x)
#  define htobe32(x) (x)
#  define htobe64(x) (x)

# elif defined(__LITTLEENDIAN__)

#  define __BYTE_ORDER __LITTLE_ENDIAN
#  define htole16(x) (x)
#  define htole32(x) (x)
#  define htole64(x) (x)
#  define htobe16(x) ENDIAN_SWAP16(x)
#  define htobe32(x) ENDIAN_SWAP32(x)
#  define htobe64(x) ENDIAN_SWAP64(x)

# else
#  error byte order not supported
# endif

# define be16toh(x) ENDIAN_BE16(x)
# define be32toh(x) ENDIAN_BE32(x)
# define be64toh(x) ENDIAN_BE64(x)
# define le16toh(x) ENDIAN_LE16(x)
# define le32toh(x) ENDIAN_LE32(x)
# define le64toh(x) ENDIAN_LE64(x)

#else
#	error platform not supported
#endif

#if !defined(BIG_ENDIAN) && defined(__BIG_ENDIAN)
# define BIG_ENDIAN __BIG_ENDIAN
#elif !defined(BIG_ENDIAN)
# define BIG_ENDIAN 4321
#endif

#if !defined(LITTLE_ENDIAN) && defined(__LITTLE_ENDIAN)
# define LITTLE_ENDIAN __LITTLE_ENDIAN
#elif !defined(LITTLE_ENDIAN)
# define LITTLE_ENDIAN 1234
#endif

#if !defined(BYTE_ORDER) && defined(__BYTE_ORDER)
# define BYTE_ORDER __BYTE_ORDER
#elif !defined(BYTE_ORDER)
# if defined(__ARMEB__) || \
  defined(__THUMBEB__) || \
  defined(__AARCH64EB__) || \
  defined(_MIPSEB) || \
  defined(__MIPSEB) || \
  defined(__MIPSEB__) \
  defined(_ARCH_PPC) \
  defined(_ARCH_PPC64)
#  define BYTE_ORDER BIG_ENDIAN
# elif defined(__ARMEL__) || \
  defined(__THUMBEL__) || \
  defined(__AARCH64EL__) || \
  defined(_MIPSEL) || \
  defined(__MIPSEL) || \
  defined(__MIPSEL__)
#  define BYTE_ORDER LITTLE_ENDIAN
# else
#  error unable to identify endianess
# endif
#endif

#if !defined(BYTE_ORDER)
#  error unable to identify endianess
# endif

#if !defined(BIG_ENDIAN)
# error BIG_ENDIAN not defined
#endif

#if !defined(LITTLE_ENDIAN)
# error LITTLE_ENDIAN not defined
#endif

#endif /* PORTABLE_ENDIAN_H */
