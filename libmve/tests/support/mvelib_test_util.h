/*
 * Descent 3
 * Copyright (C) 2026 Descent Developers
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

#ifndef LIBMVE_TESTS_MVELIB_TEST_UTIL_H_
#define LIBMVE_TESTS_MVELIB_TEST_UTIL_H_

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <vector>

#include "mve_test_util.h"
#include "mvelib.h"

namespace mvetest {

inline Bytes Payload(size_t n, uint8_t seed) {
  Bytes b(n);
  for (size_t i = 0; i < n; i++)
    b[i] = static_cast<uint8_t>(seed + i * 7);
  return b;
}

struct Call {
  unsigned char major;
  unsigned char minor;
  Bytes data;
  void *context;
};

inline std::vector<Call> g_calls;
inline int g_stop_on_major = -1;

inline int RecordingHandler(unsigned char major, unsigned char minor, unsigned char *data, int len, void *context) {
  g_calls.push_back({major, minor, Bytes(data, data + len), context});
  return major == g_stop_on_major ? 0 : 1;
}

// The payload of the file's current segment, or nothing at the end of the chunk.
inline Bytes SegmentData(MVEFILE *f) {
  int size = mvefile_get_next_segment_size(f);
  unsigned char *p = mvefile_get_next_segment(f);
  if (p == nullptr || size < 0)
    return {};
  return Bytes(p, p + size);
}

// mve_read callback over a FILE; mve_reset rewinds with fseek, so it needs a real FILE.
inline unsigned int FileRead(void *stream, void *buffer, unsigned int count) {
  return fread(buffer, 1, count, static_cast<FILE *>(stream)) == count ? 1 : 0;
}

} // namespace mvetest

#endif // LIBMVE_TESTS_MVELIB_TEST_UTIL_H_
