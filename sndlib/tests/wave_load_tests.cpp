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
#include <cstdlib>
#include <filesystem>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "d3_test/files.h"
#include "ssl_lib.h"

#include "support/sdl_assert_guard.h"
#include "support/wav_builder.h"
#include "support/wave_load_test_support.h"

namespace {

class WaveLoadTest : public ::testing::Test {
protected:
  static constexpr int kSlotA = 1;
  static constexpr int kSlotB = 2;

  void TearDown() override {
    for (int slot : {kSlotA, kSlotB}) {
      SoundLoadFree(slot);
      SoundFiles[slot] = sound_file_info{};
    }
  }

  SdlAssertGuard assert_guard_;
  d3_test::TempDir dir_;
};

TEST_F(WaveLoadTest, HighQuality16BitKeepsWaveformShapeAndAttenuates) {
  const std::vector<int16_t> in = {0, 1000, -2000, 3000, -4000, 32767, -32768, 0};
  ASSERT_TRUE(Load(dir_.Write("a.wav", WavBuilder().PcmFmt(16).Data16(in).Build()), kSlotA, true));

  const sound_file_info &sf = SoundFiles[kSlotA];
  ASSERT_NE(sf.sample_16bit, nullptr);
  EXPECT_EQ(sf.sample_8bit, nullptr);
  EXPECT_EQ(sf.np_sample_length, static_cast<int>(in.size()));
  EXPECT_GE(sf.sample_length, sf.np_sample_length);

  // The loader scales samples down to leave headroom against clipping.
  const double k = sf.sample_16bit[3] / 3000.0;
  EXPECT_GT(k, 0.0);
  EXPECT_LE(k, 1.0);
  for (size_t i = 0; i < in.size(); ++i)
    EXPECT_NEAR(sf.sample_16bit[i], in[i] * k, 1.0) << "sample " << i;
}

TEST_F(WaveLoadTest, HighQualityOutputScalesLinearlyWithImportVolume) {
  const std::vector<int16_t> in = {1000, -2000, 3000, -4000, 8000, -16000, 20000, -20000};
  const std::filesystem::path path = dir_.Write("a.wav", WavBuilder().PcmFmt(16).Data16(in).Build());
  ASSERT_TRUE(Load(path, kSlotA, true, 1.0f));
  ASSERT_TRUE(Load(path, kSlotB, true, 0.5f));

  for (size_t i = 0; i < in.size(); ++i)
    EXPECT_NEAR(SoundFiles[kSlotB].sample_16bit[i], SoundFiles[kSlotA].sample_16bit[i] * 0.5, 1.0) << "sample " << i;
}

TEST_F(WaveLoadTest, ZeroImportVolumeLoadsSilence) {
  const std::vector<int16_t> in = {1000, -2000, 3000, -4000};
  ASSERT_TRUE(Load(dir_.Write("a.wav", WavBuilder().PcmFmt(16).Data16(in).Build()), kSlotA, true, 0.0f));
  for (int s : Samples16(kSlotA))
    EXPECT_EQ(s, 0);
}

TEST_F(WaveLoadTest, HighQuality8BitFileMatchesEquivalent16BitFile) {
  const std::vector<uint8_t> in8 = {128, 255, 0, 1, 127, 129, 200, 50, 90};
  std::vector<int16_t> in16;
  for (uint8_t u : in8)
    in16.push_back(To16(u));

  ASSERT_TRUE(Load(dir_.Write("8.wav", WavBuilder().PcmFmt(8).Data8(in8).Build()), kSlotA, true, 0.8f));
  ASSERT_TRUE(Load(dir_.Write("16.wav", WavBuilder().PcmFmt(16).Data16(in16).Build()), kSlotB, true, 0.8f));

  EXPECT_EQ(SoundFiles[kSlotA].sample_8bit, nullptr);
  ASSERT_NE(SoundFiles[kSlotA].sample_16bit, nullptr);
  EXPECT_EQ(SoundFiles[kSlotA].np_sample_length, SoundFiles[kSlotB].np_sample_length);
  EXPECT_EQ(Samples16(kSlotA), Samples16(kSlotB));
}

TEST_F(WaveLoadTest, LowQuality16BitFileMatchesEquivalent8BitFile) {
  const std::vector<uint8_t> in8 = {128, 255, 0, 1, 127, 129, 200, 50, 90, 160};
  std::vector<int16_t> in16;
  for (uint8_t u : in8)
    in16.push_back(To16(u));

  ASSERT_TRUE(Load(dir_.Write("8.wav", WavBuilder().PcmFmt(8).Data8(in8).Build()), kSlotA, false, 0.8f));
  ASSERT_TRUE(Load(dir_.Write("16.wav", WavBuilder().PcmFmt(16).Data16(in16).Build()), kSlotB, false, 0.8f));

  EXPECT_EQ(SoundFiles[kSlotB].sample_16bit, nullptr);
  ASSERT_NE(SoundFiles[kSlotB].sample_8bit, nullptr);
  ASSERT_EQ(SoundFiles[kSlotA].np_sample_length, SoundFiles[kSlotB].np_sample_length);

  // The two conversion paths round differently, so allow one step of difference.
  for (int i = 0; i < SoundFiles[kSlotA].np_sample_length; ++i)
    EXPECT_NEAR(SoundFiles[kSlotA].sample_8bit[i], SoundFiles[kSlotB].sample_8bit[i], 1) << "sample " << i;
}

TEST_F(WaveLoadTest, LowQualityPreservesSampleOrdering) {
  // The inputs are spaced widely enough to stay distinct under any clip attenuation
  // from 0.1 up to 1.0.
  const std::vector<int16_t> in = {-32767, -16000, -4000, 0, 4000, 16000, 32767};
  ASSERT_TRUE(Load(dir_.Write("a.wav", WavBuilder().PcmFmt(16).Data16(in).Build()), kSlotA, false));
  const std::vector<int> out = Samples8(kSlotA);
  for (size_t i = 1; i < in.size(); ++i)
    EXPECT_LT(out[i - 1], out[i]) << "between samples " << i - 1 << " and " << i;
}

// The mixer reads a few samples past the playable end when interpolating, so the
// loader pads each sound out. A range of lengths covers every remainder modulo the
// alignment. (The 8-bit case starts at 2 samples: a 1-byte data chunk is ignored by
// the chunk scanner.)
TEST_F(WaveLoadTest, PaddingAfterSampleDataIsSilence16BitFile) {
  for (int n = 1; n <= 9; ++n) {
    SCOPED_TRACE("samples: " + std::to_string(n));
    std::vector<int16_t> in(n, 12000);
    ASSERT_TRUE(Load(dir_.Write("a.wav", WavBuilder().PcmFmt(16).Data16(in).Build()), kSlotA, true));
    const sound_file_info &sf = SoundFiles[kSlotA];
    ASSERT_EQ(sf.np_sample_length, n);
    ASSERT_GE(sf.sample_length, n);
    for (int i = 0; i < n; ++i)
      EXPECT_NE(sf.sample_16bit[i], 0);
    for (int i = n; i < sf.sample_length; ++i)
      EXPECT_EQ(sf.sample_16bit[i], 0) << "padding sample " << i;
    SoundLoadFree(kSlotA);
  }
}

TEST_F(WaveLoadTest, PaddingAfterSampleDataIsSilence8BitFile) {
  for (int n = 2; n <= 9; ++n) {
    SCOPED_TRACE("samples: " + std::to_string(n));
    std::vector<uint8_t> in(n, 220);
    ASSERT_TRUE(Load(dir_.Write("a.wav", WavBuilder().PcmFmt(8).Data8(in).Build()), kSlotA, true));
    const sound_file_info &sf = SoundFiles[kSlotA];
    ASSERT_EQ(sf.np_sample_length, n);
    ASSERT_GE(sf.sample_length, n);
    for (int i = 0; i < n; ++i)
      EXPECT_NE(sf.sample_16bit[i], 0);
    for (int i = n; i < sf.sample_length; ++i)
      EXPECT_EQ(sf.sample_16bit[i], 0) << "padding sample " << i;
    SoundLoadFree(kSlotA);
  }
}

TEST_F(WaveLoadTest, UnknownChunksAreSkipped) {
  const std::vector<int16_t> in = {100, 200, 300, 400};
  const std::vector<uint8_t> plain = WavBuilder().PcmFmt(16).Data16(in).Build();
  const std::vector<uint8_t> with_extra = WavBuilder()
                                              .Chunk("JUNK", {1, 2, 3, 4, 5, 6})
                                              .PcmFmt(16)
                                              .Chunk("LIST", {'I', 'N', 'F', 'O', 0, 0, 0, 0})
                                              .Data16(in)
                                              .Chunk("cue ", {0, 0, 0, 0})
                                              .Build();
  ASSERT_TRUE(Load(dir_.Write("plain.wav", plain), kSlotA, true));
  ASSERT_TRUE(Load(dir_.Write("extra.wav", with_extra), kSlotB, true));
  EXPECT_EQ(Samples16(kSlotA), Samples16(kSlotB));
}

TEST_F(WaveLoadTest, HeaderOnlyCheckFindsFileButLoadsNoData) {
  const std::filesystem::path path = dir_.Write("a.wav", WavBuilder().PcmFmt(16).Data16({1, 2, 3, 4}).Build());
  EXPECT_TRUE(Load(path, kSlotA, true, 1.0f, /*load_data=*/false));
  EXPECT_EQ(SoundFiles[kSlotA].sample_16bit, nullptr);
  EXPECT_EQ(SoundFiles[kSlotA].sample_8bit, nullptr);
}

TEST_F(WaveLoadTest, MissingFileFails) {
  EXPECT_FALSE(Load(dir_ / "does_not_exist.wav", kSlotA, true));
  EXPECT_FALSE(Load(dir_ / "does_not_exist.wav", kSlotA, true, 1.0f, /*load_data=*/false));
}

// The engine only supports mono, 22050 Hz, 8- or 16-bit PCM.
struct RejectCase {
  const char *name;
  std::vector<uint8_t> bytes;
};

class WaveRejectTest : public WaveLoadTest, public ::testing::WithParamInterface<RejectCase> {};

TEST_P(WaveRejectTest, RejectsUnsupportedOrMalformedFile) {
  for (bool hq : {true, false}) {
    SCOPED_TRACE(hq ? "high quality" : "low quality");
    EXPECT_FALSE(Load(dir_.Write("bad.wav", GetParam().bytes), kSlotA, hq));
    EXPECT_EQ(SoundFiles[kSlotA].sample_16bit, nullptr);
    EXPECT_EQ(SoundFiles[kSlotA].sample_8bit, nullptr);
  }
}

const std::vector<int16_t> kSomeSamples = {1, 2, 3, 4, 5, 6, 7, 8};

INSTANTIATE_TEST_SUITE_P(
    BadFiles, WaveRejectTest,
    ::testing::Values(
        RejectCase{"NotRiff", WavBuilder().PcmFmt(16).Data16(kSomeSamples).Build("RIFX")},
        RejectCase{"NotWave", WavBuilder().PcmFmt(16).Data16(kSomeSamples).Build("RIFF", "AVI ")},
        RejectCase{"FloatFormat", WavBuilder().Fmt(3, 1, 22050, 32).Data16(kSomeSamples).Build()},
        RejectCase{"AdpcmFormat", WavBuilder().Fmt(2, 1, 22050, 16).Data16(kSomeSamples).Build()},
        RejectCase{"Stereo", WavBuilder().Fmt(1, 2, 22050, 16).Data16(kSomeSamples).Build()},
        RejectCase{"Rate44k", WavBuilder().Fmt(1, 1, 44100, 16).Data16(kSomeSamples).Build()},
        RejectCase{"Rate11k", WavBuilder().Fmt(1, 1, 11025, 16).Data16(kSomeSamples).Build()},
        RejectCase{"TwentyFourBit", WavBuilder().Fmt(1, 1, 22050, 24).Data16(kSomeSamples).Build()},
        RejectCase{"DataBeforeFormat", WavBuilder().Data16(kSomeSamples).PcmFmt(16).Build()},
        RejectCase{"TwoFormatsWithoutData",
                   WavBuilder().PcmFmt(16).PcmFmt(16).Data16(kSomeSamples).Build()}),
    [](const ::testing::TestParamInfo<RejectCase> &info) { return info.param.name; });

} // namespace
