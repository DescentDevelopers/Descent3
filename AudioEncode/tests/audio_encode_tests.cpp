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
#include <filesystem>
#include <limits>
#include <random>
#include <string>
#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include "adecode.h"
#include "aencode.h"
#include "audio_encode.h"
#include "d3_test/files.h"
#include "libacm.h"
#include "support/acm_io.h"
#include "support/signal_gen.h"

namespace fs = std::filesystem;

namespace {

// A lossy codec at its default 4:1 compression should reproduce a clean tone
// far better than this; a stream that is misaligned, has the wrong sign, or is
// otherwise garbled lands near or below 0 dB.
constexpr double kMinToneSnrDb = 20.0;

class AudioEncodeTest : public ::testing::Test {
protected:
  d3_test::TempDir dir_;
};

struct Layout {
  int levels;
  int rows;
};

class AudioEncodeLayoutTest : public AudioEncodeTest, public ::testing::WithParamInterface<Layout> {};

TEST_P(AudioEncodeLayoutTest, MonoToneRoundTripsWithHeaderAndLength) {
  const Layout layout = GetParam();
  // 5000 samples: more than one block for every layout, and not a multiple of
  // any block size, so the final partial block is exercised.
  const auto pcm = MakeTone(5000, 440.0, 22050.0, 8000.0);
  const auto bytes = Encode(dir_ / "encoded.acm", pcm, 1, 22050, layout.levels, layout.rows, 0.25f);

  const AcmLayout acm = ReadLayout(bytes);
  ASSERT_TRUE(acm.ok);
  EXPECT_EQ(acm.level, unsigned(layout.levels));
  EXPECT_EQ(acm.rows, unsigned(layout.rows));

  const Decoded dec = Decode(bytes);
  ASSERT_TRUE(dec.ok);
  EXPECT_EQ(dec.channels, 1u);
  EXPECT_EQ(dec.rate, 22050u);
  EXPECT_EQ(dec.sample_count, pcm.size());
  ASSERT_EQ(dec.pcm.size(), pcm.size());
  EXPECT_GT(SnrDb(pcm, dec.pcm), kMinToneSnrDb);
}

INSTANTIATE_TEST_SUITE_P(AudioEncode, AudioEncodeLayoutTest,
                         ::testing::Values(Layout{7, 16}, // the library's default (2048-sample blocks)
                                           Layout{6, 32}, Layout{4, 128}, Layout{3, 64},
                                           Layout{0, 256}), // no subband transform at all
                         [](const ::testing::TestParamInfo<Layout> &info) {
                           return "L" + std::to_string(info.param.levels) + "R" + std::to_string(info.param.rows);
                         });

TEST_F(AudioEncodeTest, StereoRoundTripKeepsChannelsSeparate) {
  // Distinct tones per channel: swapping or mixing channels would destroy the SNR.
  const auto left = MakeTone(4000, 300.0, 22050.0, 9000.0);
  const auto right = MakeTone(4000, 1100.0, 22050.0, 6000.0, 1.0);
  const auto pcm = Interleave(left, right);

  const Decoded dec = Decode(Encode(dir_ / "encoded.acm", pcm, 2, 22050, 7, 16, 0.25f));
  ASSERT_TRUE(dec.ok);
  EXPECT_EQ(dec.channels, 2u);
  EXPECT_EQ(dec.rate, 22050u);
  EXPECT_EQ(dec.sample_count, pcm.size());
  ASSERT_EQ(dec.pcm.size(), pcm.size());
  EXPECT_GT(SnrDb(left, Channel(dec.pcm, 0, 2)), kMinToneSnrDb);
  EXPECT_GT(SnrDb(right, Channel(dec.pcm, 1, 2)), kMinToneSnrDb);
}

TEST_F(AudioEncodeTest, SilenceDecodesToExactSilence) {
  const std::vector<int16_t> pcm(3000, 0);
  const Decoded dec = Decode(Encode(dir_ / "encoded.acm", pcm, 1, 22050, 7, 16, 0.25f));
  ASSERT_TRUE(dec.ok);
  ASSERT_EQ(dec.pcm.size(), pcm.size());
  EXPECT_TRUE(std::all_of(dec.pcm.begin(), dec.pcm.end(), [](int16_t s) { return s == 0; }));
}

TEST_F(AudioEncodeTest, VolumeScalesTheEncodedSignal) {
  const auto pcm = MakeTone(4096, 440.0, 22050.0, 10000.0);
  std::vector<int16_t> half(pcm.size());
  std::transform(pcm.begin(), pcm.end(), half.begin(), [](int16_t s) { return int16_t(s / 2); });

  const Decoded dec = Decode(Encode(dir_ / "encoded.acm", pcm, 1, 22050, 7, 16, 0.25f, 0.5f));
  ASSERT_TRUE(dec.ok);
  ASSERT_EQ(dec.pcm.size(), pcm.size());
  EXPECT_GT(SnrDb(half, dec.pcm), kMinToneSnrDb);
}

TEST_F(AudioEncodeTest, StrongerCompressionGivesSmallerOutputAndMoreError) {
  // White noise has no structure to exploit, so the bit budget set by
  // comp_ratio directly controls output size and fidelity.
  std::mt19937 rng(12345);
  std::uniform_int_distribution<int> dist(-8000, 8000);
  std::vector<int16_t> pcm(16384);
  for (auto &s : pcm)
    s = static_cast<int16_t>(dist(rng));

  const auto mild = Encode(dir_ / "encoded.acm", pcm, 1, 22050, 7, 16, 1.0f / 2.0f);
  const auto strong = Encode(dir_ / "encoded.acm", pcm, 1, 22050, 7, 16, 1.0f / 8.0f);
  EXPECT_LT(strong.size(), mild.size());

  // 16-bit input at 1:8 should be roughly 1/8 the raw size; allow generous slack.
  EXPECT_LT(strong.size(), pcm.size() * 2 / 4);

  const Decoded dmild = Decode(mild);
  const Decoded dstrong = Decode(strong);
  ASSERT_TRUE(dmild.ok);
  ASSERT_TRUE(dstrong.ok);
  EXPECT_GT(SnrDb(pcm, dmild.pcm), SnrDb(pcm, dstrong.pcm));
}

TEST_F(AudioEncodeTest, DecoderResultIndependentOfReadAndSourceChunking) {
  const auto pcm = MakeTone(5000, 700.0, 22050.0, 7000.0);
  const auto bytes = Encode(dir_ / "encoded.acm", pcm, 1, 22050, 7, 16, 0.25f);

  const Decoded whole = Decode(bytes);
  // Source hands out at most 3 bytes per call; caller reads 100 bytes at a time.
  const Decoded pieces = Decode(bytes, 3, 100);
  ASSERT_TRUE(whole.ok);
  ASSERT_TRUE(pieces.ok);
  ASSERT_EQ(whole.pcm.size(), pcm.size());
  EXPECT_EQ(pieces.pcm, whole.pcm);
}

TEST_F(AudioEncodeTest, DecoderReturnsZeroAfterEndOfStream) {
  const auto pcm = MakeTone(5000, 440.0, 22050.0, 5000.0);
  const auto bytes = Encode(dir_ / "encoded.acm", pcm, 1, 22050, 7, 16, 0.25f);

  ByteSource src{&bytes};
  uint32_t channels, rate, count;
  std::unique_ptr<AudioDecoder::IAudioDecoder> dec(
      AudioDecoder::CreateDecoder(ReadBytes, &src, channels, rate, count));
  ASSERT_NE(dec, nullptr);
  std::vector<uint8_t> buf(count * 2 + 64);
  EXPECT_EQ(dec->Read(buf.data(), uint32_t(buf.size())), count * 2);
  EXPECT_EQ(dec->Read(buf.data(), uint32_t(buf.size())), 0u);
}

TEST_F(AudioEncodeTest, DecoderRejectsEmptyStream) {
  const std::vector<uint8_t> empty;
  ByteSource src{&empty};
  uint32_t channels, rate, count;
  EXPECT_EQ(AudioDecoder::CreateDecoder(ReadBytes, &src, channels, rate, count), nullptr);
}

TEST_F(AudioEncodeTest, DecoderRejectsNonAcmData) {
  const char wav[] = "RIFF\x24\x08\x00\x00WAVEfmt \x10\x00\x00\x00\x01\x00\x01\x00\x22\x56\x00\x00";
  const std::vector<uint8_t> bytes(wav, wav + sizeof(wav) - 1);
  ASSERT_GE(bytes.size(), 14u);
  ByteSource src{&bytes};
  uint32_t channels, rate, count;
  EXPECT_EQ(AudioDecoder::CreateDecoder(ReadBytes, &src, channels, rate, count), nullptr);
}

TEST_F(AudioEncodeTest, DecoderRejectsTruncatedHeader) {
  const auto bytes = Encode(dir_ / "encoded.acm", MakeTone(1000, 440.0, 22050.0, 5000.0), 1, 22050, 7, 16, 0.25f);
  ASSERT_GT(bytes.size(), 14u);
  // ACM header is 14 bytes; keep the magic and sample count but drop the rest.
  const std::vector<uint8_t> truncated(bytes.begin(), bytes.begin() + 8);
  ByteSource src{&truncated};
  uint32_t channels, rate, count;
  EXPECT_EQ(AudioDecoder::CreateDecoder(ReadBytes, &src, channels, rate, count), nullptr);
}

TEST_F(AudioEncodeTest, CompressFailsWhenInputMissing) {
  EXPECT_FALSE(aenc_Compress((dir_ / "does_not_exist.raw").string().c_str(), (dir_ / "out.acm").string().c_str()));
  EXPECT_FALSE(fs::exists(dir_ / "out.acm"));
}

TEST_F(AudioEncodeTest, CompressDefaultsToMono22050With2048SampleBlocks) {
  const std::vector<int16_t> pcm(3000, 0);
  WriteRawPcm(dir_ / "in.raw", pcm);
  ASSERT_TRUE(aenc_Compress((dir_ / "in.raw").string().c_str(), (dir_ / "out.acm").string().c_str()));

  const auto bytes = d3_test::ReadFileBytes(dir_ / "out.acm");
  const AcmLayout acm = ReadLayout(bytes);
  ASSERT_TRUE(acm.ok);
  // Documented defaults in audio_encode.h: 7 levels, 16 samples per subband.
  EXPECT_EQ(acm.level, 7u);
  EXPECT_EQ(acm.rows, 16u);

  const Decoded dec = Decode(bytes);
  ASSERT_TRUE(dec.ok);
  EXPECT_EQ(dec.channels, 1u);
  EXPECT_EQ(dec.rate, 22050u);
  EXPECT_EQ(dec.sample_count, pcm.size());
}

TEST_F(AudioEncodeTest, CompressWritesRequestedRateAndChannels) {
  const std::vector<int16_t> pcm(4000, 0);
  WriteRawPcm(dir_ / "in.raw", pcm);
  const int rate = 44100;
  const int channels = 2;
  ASSERT_TRUE(aenc_Compress((dir_ / "in.raw").string().c_str(), (dir_ / "out.acm").string().c_str(), nullptr, nullptr,
                            &rate, &channels));

  const Decoded dec = Decode(d3_test::ReadFileBytes(dir_ / "out.acm"));
  ASSERT_TRUE(dec.ok);
  EXPECT_EQ(dec.channels, 2u);
  EXPECT_EQ(dec.rate, 44100u);
  EXPECT_EQ(dec.sample_count, pcm.size());
}

struct DerivedLayoutCase {
  int levels;  // -1: not passed
  int samples; // -1: not passed
  unsigned expected_levels;
  unsigned expected_rows;
};

class CompressDerivedLayoutTest : public AudioEncodeTest, public ::testing::WithParamInterface<DerivedLayoutCase> {};

TEST_P(CompressDerivedLayoutTest, OtherDimensionDerivedFor2048SampleBlocks) {
  const DerivedLayoutCase c = GetParam();
  WriteRawPcm(dir_ / "in.raw", std::vector<int16_t>(3000, 0));
  ASSERT_TRUE(aenc_Compress((dir_ / "in.raw").string().c_str(), (dir_ / "out.acm").string().c_str(),
                            c.levels >= 0 ? &c.levels : nullptr, c.samples >= 0 ? &c.samples : nullptr));

  const AcmLayout acm = ReadLayout(d3_test::ReadFileBytes(dir_ / "out.acm"));
  ASSERT_TRUE(acm.ok);
  EXPECT_EQ(acm.level, c.expected_levels);
  EXPECT_EQ(acm.rows, c.expected_rows);
}

INSTANTIATE_TEST_SUITE_P(AudioEncode, CompressDerivedLayoutTest,
                         ::testing::Values(
                             // samples only -> levels such that rows << levels == 2048
                             DerivedLayoutCase{-1, 16, 7, 16}, // what audiotaunts.cpp passes
                             DerivedLayoutCase{-1, 32, 6, 32}, DerivedLayoutCase{-1, 128, 4, 128},
                             // levels only -> rows = 2048 >> levels
                             DerivedLayoutCase{5, -1, 5, 64}, DerivedLayoutCase{7, -1, 7, 16}),
                         [](const ::testing::TestParamInfo<DerivedLayoutCase> &info) {
                           return info.param.levels >= 0 ? "Levels" + std::to_string(info.param.levels)
                                                         : "Samples" + std::to_string(info.param.samples);
                         });

TEST_F(AudioEncodeTest, CompressTreatsFactorBelowOneAsReciprocal) {
  std::mt19937 rng(777);
  std::uniform_int_distribution<int> dist(0, 8000);
  std::vector<int16_t> pcm(8192);
  for (auto &s : pcm)
    s = static_cast<int16_t>(dist(rng));
  WriteRawPcm(dir_ / "in.raw", pcm);

  const float four = 4.0f, quarter = 0.25f, two = 2.0f;
  ASSERT_TRUE(aenc_Compress((dir_ / "in.raw").string().c_str(), (dir_ / "four.acm").string().c_str(), nullptr, nullptr,
                            nullptr, nullptr, &four));
  ASSERT_TRUE(aenc_Compress((dir_ / "in.raw").string().c_str(), (dir_ / "quarter.acm").string().c_str(), nullptr,
                            nullptr, nullptr, nullptr, &quarter));
  ASSERT_TRUE(aenc_Compress((dir_ / "in.raw").string().c_str(), (dir_ / "two.acm").string().c_str(), nullptr, nullptr,
                            nullptr, nullptr, &two));

  const auto a = d3_test::ReadFileBytes(dir_ / "four.acm");
  const auto b = d3_test::ReadFileBytes(dir_ / "quarter.acm");
  const auto c = d3_test::ReadFileBytes(dir_ / "two.acm");
  EXPECT_EQ(a, b);
  // Sanity check that the factor actually matters for this input.
  EXPECT_NE(a, c);
}

} // namespace
