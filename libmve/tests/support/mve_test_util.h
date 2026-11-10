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

#ifndef LIBMVE_TESTS_MVE_TEST_UTIL_H_
#define LIBMVE_TESTS_MVE_TEST_UTIL_H_

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <set>
#include <string>
#include <vector>

#include "d3_test/byte_writer.h"

namespace mvetest {

using Bytes = std::vector<uint8_t>;

struct MemStream {
  Bytes bytes;
  size_t pos = 0;
};

/// Same all-or-nothing contract as Descent3's CallbackFileRead (d3movie.cpp).
inline unsigned int MemRead(void *stream, void *buffer, unsigned int count) {
  auto *s = static_cast<MemStream *>(stream);
  size_t n = std::min<size_t>(s->bytes.size() - s->pos, count);
  if (n > 0)
    memcpy(buffer, s->bytes.data() + s->pos, n);
  s->pos += n;
  return n == count ? 1 : 0;
}

struct TrackingAllocator {
  static std::set<void *> &Live() {
    static std::set<void *> live;
    return live;
  }
  static int &BadFrees() {
    static int bad = 0;
    return bad;
  }
  static void *Alloc(unsigned int size) {
    void *p = malloc(size ? size : 1);
    Live().insert(p);
    return p;
  }
  static void Free(void *p) {
    if (p == nullptr)
      return;
    if (Live().erase(p) == 0) {
      ++BadFrees();
      return;
    }
    free(p);
  }
  static void Reset() {
    for (void *p : Live())
      free(p);
    Live().clear();
    BadFrees() = 0;
  }
};

// Number of blocks currently allocated through TrackingAllocator.
inline size_t LiveAllocations() { return TrackingAllocator::Live().size(); }

struct Segment {
  uint8_t major;
  uint8_t minor;
  Bytes payload;
};

inline Bytes EncodeSegment(const Segment &seg) {
  return d3_test::ByteWriter()
      .U16(static_cast<uint16_t>(seg.payload.size()))
      .U8(seg.major)
      .U8(seg.minor)
      .Bytes(seg.payload)
      .bytes();
}

class MveBuilder {
public:
  MveBuilder() {
    // The signature is 19 characters and a terminating NUL.
    Raw(d3_test::ByteWriter().FixedText("Interplay MVE File\x1A", 20).U16(0x001A).U16(0x0100).U16(0x1133));
  }

  MveBuilder &Chunk(const std::vector<Segment> &segments, uint16_t chunk_type = 0) {
    d3_test::ByteWriter body;
    for (const auto &seg : segments)
      body.Bytes(EncodeSegment(seg));
    return RawChunk(body, chunk_type);
  }

  MveBuilder &RawChunk(const Bytes &body, uint16_t chunk_type = 0) {
    return Raw(d3_test::ByteWriter().U16(static_cast<uint16_t>(body.size())).U16(chunk_type).Bytes(body));
  }

  MveBuilder &Raw(const Bytes &raw) {
    bytes_.insert(bytes_.end(), raw.begin(), raw.end());
    return *this;
  }

  Bytes &bytes() { return bytes_; }

private:
  Bytes bytes_;
};

} // namespace mvetest

#endif // LIBMVE_TESTS_MVE_TEST_UTIL_H_
