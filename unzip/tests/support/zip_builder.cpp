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

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>
#include <zlib.h>

#include "d3_test/byte_writer.h"
#include "zip_builder.h"

// Incompressible: forces deflate to emit stored blocks and makes the compressed size larger than the reader's
// internal I/O chunk sizes.
Bytes RandomBytes(size_t n, uint32_t seed) {
  Bytes out(n);
  uint32_t x = seed ? seed : 1;
  for (auto &b : out) {
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    b = static_cast<uint8_t>(x);
  }
  return out;
}

Bytes TextBytes(size_t n) {
  static const char kText[] = "The quick brown fox jumps over the lazy dog. Descent 3 mission data. ";
  Bytes out(n);
  for (size_t i = 0; i < n; ++i)
    out[i] = static_cast<uint8_t>(kText[i % (sizeof(kText) - 1)]);
  return out;
}

Bytes SampleBytes(size_t n, bool random) {
  return random ? RandomBytes(n, static_cast<uint32_t>(n) + 1) : TextBytes(n);
}

Bytes ToBytes(const std::string &s) { return Bytes(s.begin(), s.end()); }

Bytes AsBytes(const std::vector<char> &v) { return Bytes(v.begin(), v.end()); }

Bytes RawDeflate(const Bytes &in) {
  z_stream s{};
  EXPECT_EQ(deflateInit2(&s, Z_BEST_COMPRESSION, Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY), Z_OK);
  Bytes out(deflateBound(&s, static_cast<uLong>(in.size())) + 16);
  s.next_in = const_cast<Bytef *>(in.data());
  s.avail_in = static_cast<uInt>(in.size());
  s.next_out = out.data();
  s.avail_out = static_cast<uInt>(out.size());
  EXPECT_EQ(deflate(&s, Z_FINISH), Z_STREAM_END);
  out.resize(s.total_out);
  deflateEnd(&s);
  return out;
}

uint32_t Crc(const Bytes &data) {
  return static_cast<uint32_t>(crc32(0L, data.data(), static_cast<uInt>(data.size())));
}

Archive BuildZip(std::vector<Entry> entries, const std::string &archive_comment) {
  Archive ar;
  d3_test::ByteWriter b;
  std::vector<uint32_t> offsets;

  for (auto &e : entries) {
    if (!e.payload_set)
      e.payload = (e.method == kMethodDeflate) ? RawDeflate(e.data) : e.data;
    const uint32_t crc = Crc(e.data);
    const uint16_t flags = e.data_descriptor ? 0x0008 : 0;

    offsets.push_back(static_cast<uint32_t>(b.size()));
    b.U32(0x04034b50);
    b.U16(e.version_needed);
    b.U16(flags);
    b.U16(e.method);
    b.U16(0); // time
    b.U16(0); // date
    b.U32(e.data_descriptor ? 0 : crc);
    b.U32(e.data_descriptor ? 0 : static_cast<uint32_t>(e.payload.size()));
    b.U32(e.data_descriptor ? 0 : static_cast<uint32_t>(e.data.size()));
    b.U16(static_cast<uint16_t>(e.name.size()));
    b.U16(static_cast<uint16_t>(e.local_extra.size()));
    b.Text(e.name);
    b.Bytes(e.local_extra);
    b.Bytes(e.payload);
    if (e.data_descriptor) {
      b.U32(0x08074b50);
      b.U32(crc);
      b.U32(static_cast<uint32_t>(e.payload.size()));
      b.U32(static_cast<uint32_t>(e.data.size()));
    }
  }

  ar.cd_offset = static_cast<uint32_t>(b.size());
  for (size_t i = 0; i < entries.size(); ++i) {
    const Entry &e = entries[i];
    b.U32(0x02014b50);
    b.U16(20); // version made by (MS-DOS host)
    b.U16(e.version_needed);
    b.U16(e.data_descriptor ? 0x0008 : 0);
    b.U16(e.method);
    b.U16(0); // time
    b.U16(0); // date
    b.U32(Crc(e.data));
    b.U32(e.override_sizes ? e.central_compressed_size : static_cast<uint32_t>(e.payload.size()));
    b.U32(e.override_sizes ? e.central_uncompressed_size : static_cast<uint32_t>(e.data.size()));
    b.U16(static_cast<uint16_t>(e.name.size()));
    b.U16(static_cast<uint16_t>(e.central_extra.size()));
    b.U16(static_cast<uint16_t>(e.comment.size()));
    b.U16(0); // disk number start
    b.U16(0); // internal attributes
    b.U32(0); // external attributes
    b.U32(e.override_offset ? e.central_local_offset : offsets[i]);
    b.Text(e.name);
    b.Bytes(e.central_extra);
    b.Text(e.comment);
  }
  const uint32_t cd_size = static_cast<uint32_t>(b.size()) - ar.cd_offset;

  b.U32(0x06054b50);
  b.U16(0); // this disk
  b.U16(0); // disk with central directory
  b.U16(static_cast<uint16_t>(entries.size()));
  b.U16(static_cast<uint16_t>(entries.size()));
  b.U32(cd_size);
  b.U32(ar.cd_offset);
  b.U16(static_cast<uint16_t>(archive_comment.size()));
  b.Text(archive_comment);
  ar.bytes = b.bytes();
  return ar;
}

Entry Stored(const std::string &name, Bytes data) {
  Entry e;
  e.name = name;
  e.data = std::move(data);
  e.method = kMethodStored;
  return e;
}

Entry Deflated(const std::string &name, Bytes data) {
  Entry e;
  e.name = name;
  e.data = std::move(data);
  e.method = kMethodDeflate;
  return e;
}
