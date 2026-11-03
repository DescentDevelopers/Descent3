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

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "adecode.h"
#include "aencode.h"
#include "d3_test/files.h"
#include "streamaudio.h"
#include "test_stream.h"

namespace {

struct SampleSource {
  const std::vector<int16_t> *samples;
  size_t pos;
};

int32_t ReadNextSample(void *data) {
  auto *src = static_cast<SampleSource *>(data);
  if (src->pos >= src->samples->size())
    return ReadSampleEof;
  return (*src->samples)[src->pos++];
}

struct MemoryReader {
  const std::vector<uint8_t> *bytes;
  size_t pos;
};

int32_t ReadFromMemory(void *data, void *buf, uint32_t qty) {
  auto *src = static_cast<MemoryReader *>(data);
  size_t n = std::min<size_t>(qty, src->bytes->size() - src->pos);
  std::memcpy(buf, src->bytes->data() + src->pos, n);
  src->pos += n;
  return static_cast<int32_t>(n);
}

// A non-trivial mono signal (two tones plus noise) so every buffer has distinct content.
std::vector<int16_t> MakeSignal(size_t count) {
  std::vector<int16_t> s(count);
  uint32_t lcg = 12345;
  for (size_t i = 0; i < count; i++) {
    lcg = lcg * 1664525u + 1013904223u;
    double noise = static_cast<double>(lcg >> 20) - 2048.0;
    double v = 6000.0 * std::sin(i * 0.031) + 3000.0 * std::sin(i * 0.0071) + noise;
    s[i] = static_cast<int16_t>(v);
  }
  return s;
}

} // namespace

TestStream MakeTestStream(const std::filesystem::path &dir, const std::string &name, size_t sample_count) {
  TestStream result;
  const std::vector<int16_t> signal = MakeSignal(sample_count);

  const std::filesystem::path acm_path = dir / (name + ".acm");
  FILE *f = std::fopen(acm_path.string().c_str(), "wb");
  if (!f)
    return result;
  SampleSource src{&signal, 0};
  int32_t encoded = AudioEncode(ReadNextSample, &src, 1, 22050, 0.97f, f, 7, 16, 0.25f);
  std::fclose(f);
  if (encoded <= 0)
    return result;
  const std::vector<uint8_t> acm = d3_test::ReadFileBytes(acm_path);

  MemoryReader reader{&acm, 0};
  uint32_t channels = 0, rate = 0, count = 0;
  std::unique_ptr<AudioDecoder::IAudioDecoder> decoder(
      AudioDecoder::CreateDecoder(ReadFromMemory, &reader, channels, rate, count));
  if (!decoder)
    return result;
  uint8_t chunk[8192];
  while (uint32_t n = decoder->Read(chunk, sizeof(chunk)))
    result.pcm.insert(result.pcm.end(), chunk, chunk + n);

  OSFArchive osf;
  const std::filesystem::path osf_path = dir / (name + ".osf");
  if (!osf.Open(osf_path, true))
    return result;
  osf.WriteBlock(const_cast<uint8_t *>(acm.data()), static_cast<int>(acm.size()));
  tOSFDigiHdr hdr;
  hdr.measure = 22050;
  char title[OSF_HDR_TITLE_LEN] = {};
  std::strncpy(title, name.c_str(), sizeof(title) - 1);
  if (!osf.SaveHeader(OSF_DIGITAL_STRM, OSF_DIGIACM_STRM, SAF_16BIT_M, 22050, static_cast<uint32_t>(acm.size()), &hdr,
                      title))
    return result;
  osf.Close();
  result.osf = osf_path;
  return result;
}

std::unique_ptr<AudioStream> OpenTestStream(const TestStream &stream) {
  auto s = std::make_unique<AudioStream>();
  if (!s->Open(stream.osf)) {
    ADD_FAILURE() << "could not open " << stream.osf;
    return nullptr;
  }
  return s;
}
