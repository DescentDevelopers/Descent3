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

#include "acm_io.h"

#include <algorithm>
#include <cstdio>
#include <memory>

#include <gtest/gtest.h>

#include "adecode.h"
#include "aencode.h"
#include "d3_test/byte_writer.h"
#include "d3_test/files.h"
#include "libacm.h"

int32_t ReadPcm(void *data) {
  auto *src = static_cast<PcmSource *>(data);
  if (src->pos >= src->samples->size())
    return static_cast<int32_t>(ReadSampleEof);
  return (*src->samples)[src->pos++];
}

int32_t ReadBytes(void *data, void *buffer, uint32_t amount) {
  auto *src = static_cast<ByteSource *>(data);
  size_t n = std::min<size_t>({amount, src->bytes->size() - src->pos, src->max_chunk});
  std::copy_n(src->bytes->data() + src->pos, n, static_cast<uint8_t *>(buffer));
  src->pos += n;
  return static_cast<int32_t>(n);
}

Decoded Decode(const std::vector<uint8_t> &bytes, size_t source_chunk, uint32_t read_chunk) {
  Decoded out;
  ByteSource src{&bytes, 0, source_chunk};
  std::unique_ptr<AudioDecoder::IAudioDecoder> dec(
      AudioDecoder::CreateDecoder(ReadBytes, &src, out.channels, out.rate, out.sample_count));
  if (dec == nullptr)
    return out;
  out.ok = true;

  std::vector<uint8_t> buf(read_chunk);
  std::vector<uint8_t> raw;
  // Bound the loop so a decoder that never reports EOF fails instead of hanging.
  const size_t limit = size_t(out.sample_count) * 2 + 2 * read_chunk;
  for (;;) {
    uint32_t got = dec->Read(buf.data(), read_chunk);
    if (got == 0 || raw.size() > limit)
      break;
    raw.insert(raw.end(), buf.begin(), buf.begin() + got);
  }
  dec.reset();

  out.pcm.resize(raw.size() / 2);
  for (size_t i = 0; i < out.pcm.size(); ++i)
    out.pcm[i] = static_cast<int16_t>(raw[2 * i] | (raw[2 * i + 1] << 8));
  return out;
}

int AcmRead(void *ptr, int size, int n, void *datasrc) {
  int32_t got = ReadBytes(datasrc, ptr, uint32_t(size) * uint32_t(n));
  return got > 0 ? got / size : got;
}

AcmLayout ReadLayout(const std::vector<uint8_t> &bytes) {
  AcmLayout out;
  ByteSource src{&bytes};
  ACMStream *acm = nullptr;
  acm_io_callbacks io = {AcmRead};
  if (acm_open_decoder(&acm, &src, io, 0) != ACM_OK)
    return out;
  out.ok = true;
  out.level = acm->info.acm_level;
  out.rows = acm->info.acm_rows;
  acm_close(acm);
  return out;
}

std::vector<uint8_t> Encode(const std::filesystem::path &file, const std::vector<int16_t> &pcm, unsigned channels,
                            unsigned rate, int levels, int rows, float comp_ratio, float volume) {
  FILE *out = fopen(file.string().c_str(), "w+b");
  EXPECT_NE(out, nullptr);
  if (!out)
    return {};
  PcmSource src{&pcm, 0};
  int32_t ret = AudioEncode(ReadPcm, &src, channels, rate, volume, out, levels, rows, comp_ratio);
  fclose(out);
  EXPECT_GT(ret, 0);
  EXPECT_EQ(src.pos, pcm.size()) << "encoder should consume all input samples";
  return d3_test::ReadFileBytes(file);
}

void WriteRawPcm(const std::filesystem::path &p, const std::vector<int16_t> &pcm) {
  d3_test::ByteWriter out;
  for (int16_t s : pcm)
    out.I16(s);
  ASSERT_TRUE(d3_test::WriteFile(p, out));
}
