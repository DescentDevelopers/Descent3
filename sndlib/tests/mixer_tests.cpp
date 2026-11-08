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
#include <vector>

#include <gtest/gtest.h>

#include "mixer.h"
#include "ssl_lib.h"

#include "support/mixer_test_support.h"
#include "support/sdl_assert_guard.h"

namespace {

class MixerTest : public ::testing::Test {
protected:
  void SetUp() override {
    g_reported_errors.clear();
    for (int i = 0; i < kNumSlots; ++i) {
      cache_[i] = sound_buffer_info();
      info_[i] = play_information{};
      cache_[i].play_info = &info_[i];
      cache_[i].m_unique_id = 0x100 + i;
    }
    fake_.cache = cache_;

    tMixerInit mi{};
    mi.primary_buffer = nullptr;
    mi.primary_frequency = 22050;
    mi.primary_alignment = kFrameBytes;
    mi.max_sounds_available = &num_slots_;
    mi.sound_cache = cache_;
    mi.fp_SetError = &RecordError;
    mi.fp_ErrorText = &IgnoreErrorText;
    mi.p_error_code = &error_code_;
    mi.ll_sound_ptr = &fake_;
    ASSERT_TRUE(mixer_.Initialize(&mi));
  }

  void TearDown() override {
    for (int i = 0; i < kNumSlots; ++i) {
      Sounds[SoundIndex(i)] = sound_info{};
      SoundFiles[SoundIndex(i)] = sound_file_info{};
    }
  }

  SdlAssertGuard assert_guard_;
  FakeLowLevelSound fake_;
  sound_buffer_info cache_[kNumSlots];
  play_information info_[kNumSlots];
  int num_slots_ = kNumSlots;
  int error_code_ = SSL_OK;
  software_mixer mixer_;
};

TEST_F(MixerTest, NoActiveSoundsProducesSilence) {
  ExpectFrames(Mix(mixer_, 8), Mono({0, 0, 0, 0, 0, 0, 0, 0}), 0);
}

TEST_F(MixerTest, MonoSampleIsScaledByEachChannelVolume) {
  std::vector<int16_t> data = {1000, -2000, 3000, -4000};
  Play16(cache_, 0, data, 1.0f, 0.25f);

  ExpectFrames(Mix(mixer_, 4), {{1000, 250}, {-2000, -500}, {3000, 750}, {-4000, -1000}});
}

TEST_F(MixerTest, ConcurrentSoundsAreSummed) {
  std::vector<int16_t> a = {1000, 1000, 1000, 1000};
  std::vector<int16_t> b = {-300, 200, -100, 0};
  Play16(cache_, 0, a);
  Play16(cache_, 2, b);

  ExpectFrames(Mix(mixer_, 4), Mono({700, 1200, 900, 1000}));
}

TEST_F(MixerTest, OverflowingSumSaturatesInsteadOfWrapping) {
  std::vector<int16_t> a = {30000, -30000};
  std::vector<int16_t> b = {30000, -30000};
  Play16(cache_, 0, a);
  Play16(cache_, 1, b);

  std::vector<Frame> out = Mix(mixer_, 2);
  EXPECT_EQ(out[0].left, INT16_MAX);
  EXPECT_EQ(out[0].right, INT16_MAX);
  // Saturated at (or one above) the most negative 16-bit value.
  EXPECT_LE(out[1].left, INT16_MIN + 1);
  EXPECT_LE(out[1].right, INT16_MIN + 1);
}

TEST_F(MixerTest, UnusedAndPausedSlotsContributeNothing) {
  std::vector<int16_t> paused = {5000, 5000, 5000, 5000};
  std::vector<int16_t> unused = {7000, 7000, 7000, 7000};
  Play16(cache_, 0, paused);
  cache_[0].m_status |= SSF_PAUSED;
  Play16(cache_, 1, unused);
  cache_[1].m_status = SSF_UNUSED;

  ExpectFrames(Mix(mixer_, 4), Mono({0, 0, 0, 0}), 0);
  EXPECT_EQ(info_[0].m_samples_played, 0);
  EXPECT_TRUE(fake_.stopped_uids.empty());
}

TEST_F(MixerTest, NonLoopingSoundPlaysOnceThenIsStopped) {
  std::vector<int16_t> data = {100, 200, 300};
  Play16(cache_, 0, data);

  ExpectFrames(Mix(mixer_, 6), Mono({100, 200, 300, 0, 0, 0}));
  ASSERT_EQ(fake_.stopped_uids.size(), 1u);
  EXPECT_EQ(fake_.stopped_uids[0], cache_[0].m_unique_id);
}

TEST_F(MixerTest, SoundIsNotStoppedBeforeItsLastSampleIsMixed) {
  std::vector<int16_t> data = {100, 200, 300, 400};
  Play16(cache_, 0, data);

  Mix(mixer_, 3);
  EXPECT_TRUE(fake_.stopped_uids.empty());
  Mix(mixer_, 1);
  EXPECT_EQ(fake_.stopped_uids.size(), 1u);
}

TEST_F(MixerTest, PlaybackIsContinuousAcrossMixCalls) {
  std::vector<int16_t> data = {10, 20, 30, 40, 50, 60, 70};
  Play16(cache_, 0, data);
  std::vector<Frame> chunked = Mix(mixer_, 3);
  for (const Frame &f : Mix(mixer_, 5))
    chunked.push_back(f);

  ExpectFrames(chunked, Mono({10, 20, 30, 40, 50, 60, 70, 0}));
}

TEST_F(MixerTest, LoopingSoundRepeatsItsLoopRegion) {
  // loop_end is inclusive (the editor clamps it to np_sample_length - 1).
  std::vector<int16_t> data = {10, 20, 30, 40, 50};
  Play16(cache_, 0, data, 1.0f, 1.0f, /*looping=*/true, /*loop_start=*/1, /*loop_end=*/3);

  ExpectFrames(Mix(mixer_, 10), Mono({10, 20, 30, 40, 20, 30, 40, 20, 30, 40}));
  EXPECT_TRUE(fake_.stopped_uids.empty());
}

TEST_F(MixerTest, LoopingContinuesAcrossMixCalls) {
  std::vector<int16_t> data = {10, 20, 30};
  Play16(cache_, 0, data, 1.0f, 1.0f, /*looping=*/true);
  std::vector<Frame> chunked = Mix(mixer_, 2);
  for (const Frame &f : Mix(mixer_, 5))
    chunked.push_back(f);

  ExpectFrames(chunked, Mono({10, 20, 30, 10, 20, 30, 10}));
}

TEST_F(MixerTest, LoopingBufferEndingExactlyOnLoopEndResumesAtLoopStart) {
  std::vector<int16_t> data = {10, 20, 30, 40, 50};
  Play16(cache_, 0, data, 1.0f, 1.0f, /*looping=*/true, /*loop_start=*/1, /*loop_end=*/3);
  ExpectFrames(Mix(mixer_, 4), Mono({10, 20, 30, 40}));
  ExpectFrames(Mix(mixer_, 6), Mono({20, 30, 40, 20, 30, 40}));
}

TEST_F(MixerTest, LoopReleasedMidPlaybackFinishesTheSampleThenStops) {
  // SKT_STOP_AFTER_LOOP is implemented by clearing SSF_PLAY_LOOPING and setting
  // SSF_PLAY_NORMAL.
  std::vector<int16_t> data = {10, 20, 30, 40, 50};
  Play16(cache_, 0, data, 1.0f, 1.0f, /*looping=*/true, /*loop_start=*/1, /*loop_end=*/3);
  ExpectFrames(Mix(mixer_, 2), Mono({10, 20}));

  cache_[0].m_status = SSF_PLAY_NORMAL;
  ExpectFrames(Mix(mixer_, 5), Mono({30, 40, 50, 0, 0}));
  EXPECT_EQ(fake_.stopped_uids.size(), 1u);
}

TEST_F(MixerTest, EightBitSamplesAreUnsignedAndCenteredOn128) {
  std::vector<uint8_t> data = {128, 192, 64, 255};
  PlayStatic(cache_, 0, nullptr, data.data(), 4, 4, 1.0f, 1.0f);

  ExpectFrames(Mix(mixer_, 4), Mono({0, 16384, -16384, 127 * 256}));
}

TEST_F(MixerTest, MutedSoundStillAdvancesAndStopsOnTime) {
  std::vector<int16_t> data = {1000, 2000, 3000};
  Play16(cache_, 0, data, 0.0f, 0.0f);

  ExpectFrames(Mix(mixer_, 2), Mono({0, 0}), 0);
  EXPECT_EQ(info_[0].m_samples_played, 2);
  EXPECT_TRUE(fake_.stopped_uids.empty());
  Mix(mixer_, 2);
  EXPECT_EQ(fake_.stopped_uids.size(), 1u);
}

// sample_skip_interval simulates a lower sample rate: only every 2nd (interval 1)
// or every 4th (interval 4) source sample is used, and the samples in between are
// linearly interpolated. The skipped source samples hold junk that must not leak
// into the output.
constexpr int16_t kJunk16 = 9999;
constexpr uint8_t kJunk8 = 250;

TEST_F(MixerTest, SkipInterval1InterpolatesEveryOtherSample16Bit) {
  std::vector<int16_t> data = {0, kJunk16, 200, kJunk16, 400, kJunk16, 600, kJunk16, 800};
  PlayStatic(cache_, 0, data.data(), nullptr, 9, 8, 1.0f, 1.0f);
  info_[0].sample_skip_interval = 1;

  ExpectFrames(Mix(mixer_, 8), Mono({0, 100, 200, 300, 400, 500, 600, 700}));
}

TEST_F(MixerTest, SkipInterval1InterpolatesEveryOtherSample8Bit) {
  // 8-bit value v maps to (v - 128) * 256, so steps of 8 are steps of 2048.
  std::vector<uint8_t> data = {128, kJunk8, 136, kJunk8, 144, kJunk8, 152, kJunk8, 160};
  PlayStatic(cache_, 0, nullptr, data.data(), 9, 8, 1.0f, 1.0f);
  info_[0].sample_skip_interval = 1;

  ExpectFrames(Mix(mixer_, 8), Mono({0, 1024, 2048, 3072, 4096, 5120, 6144, 7168}));
}

TEST_F(MixerTest, SkipInterval4InterpolatesEveryFourthSample16Bit) {
  std::vector<int16_t> data = {0,   kJunk16, kJunk16, kJunk16, 400, kJunk16,
                               kJunk16, kJunk16, 800, kJunk16, kJunk16, kJunk16};
  PlayStatic(cache_, 0, data.data(), nullptr, 12, 8, 1.0f, 1.0f);
  info_[0].sample_skip_interval = 4;

  ExpectFrames(Mix(mixer_, 8), Mono({0, 100, 200, 300, 400, 500, 600, 700}));
}

TEST_F(MixerTest, SkipInterval4InterpolatesEveryFourthSample8Bit) {
  std::vector<uint8_t> data = {128,    kJunk8, kJunk8, kJunk8, 144,    kJunk8,
                               kJunk8, kJunk8, 160,    kJunk8, kJunk8, kJunk8};
  PlayStatic(cache_, 0, nullptr, data.data(), 12, 8, 1.0f, 1.0f);
  info_[0].sample_skip_interval = 4;

  ExpectFrames(Mix(mixer_, 8), Mono({0, 1024, 2048, 3072, 4096, 5120, 6144, 7168}));
}

TEST_F(MixerTest, MisalignedLengthIsReportedAsStreamMixerError) {
  std::vector<int16_t> out(3, 0);
  mixer_.StreamMixer(reinterpret_cast<char *>(out.data()), 6); // 1.5 frames
  ASSERT_EQ(g_reported_errors.size(), 1u);
  EXPECT_EQ(g_reported_errors[0], SSL_ERROR_STREAMMIXER);
}

TEST_F(MixerTest, SampleWithoutDataIsDroppedWithoutDisturbingOtherSounds) {
  std::vector<int16_t> good = {500, 500};
  PlayStatic(cache_, 0, nullptr, nullptr, 2, 2, 1.0f, 1.0f);
  Play16(cache_, 1, good);

  ExpectFrames(Mix(mixer_, 2), Mono({500, 500}));
  EXPECT_EQ(cache_[0].m_status, SSF_UNUSED);
  ASSERT_EQ(g_reported_errors.size(), 1u);
  EXPECT_EQ(g_reported_errors[0], SSL_ERROR_SAMPLE_NODATA);
}

struct StreamFormatCase {
  const char *name;
  uint16_t format;
};

class MixerStreamFormatTest : public MixerTest, public ::testing::WithParamInterface<StreamFormatCase> {
protected:
  // Logical frames, all multiples of 256 so they are exactly representable as 8-bit.
  const std::vector<Frame> stereo_ = {{2560, -2560}, {-5120, 7680}, {256, 0}, {0, -256}};
};

TEST_P(MixerStreamFormatTest, DecodesStreamBuffer) {
  std::vector<uint8_t> buf = EncodeStream(stereo_, GetParam().format);
  PlayStream(cache_, 0, GetParam().format, buf, nullptr);

  ExpectFrames(Mix(mixer_, static_cast<int>(stereo_.size())), ExpectedStreamFrames(stereo_, GetParam().format));
}

INSTANTIATE_TEST_SUITE_P(AllFormats, MixerStreamFormatTest,
                         ::testing::Values(StreamFormatCase{"Mono16", SIF_STREAMING_16_M},
                                           StreamFormatCase{"Mono8", SIF_STREAMING_8_M},
                                           StreamFormatCase{"Stereo16", SIF_STREAMING_16_S},
                                           StreamFormatCase{"Stereo8", SIF_STREAMING_8_S}),
                         [](const ::testing::TestParamInfo<StreamFormatCase> &info) { return info.param.name; });

TEST_F(MixerTest, StereoStreamAppliesVolumePerChannel) {
  std::vector<int16_t> frames = {4000, 4000, -4000, -4000};
  std::vector<uint8_t> buf = AsBytes(frames);
  PlayStream(cache_, 0, SIF_STREAMING_16_S, buf, nullptr, 0.5f, 0.25f);

  ExpectFrames(Mix(mixer_, 2), {{2000, 1000}, {-2000, -1000}});
}

TEST_F(MixerTest, ExhaustedStreamBufferIsRefilledFromCallbackWithoutGap) {
  std::vector<int16_t> first = {1, 2, 3};
  std::vector<uint8_t> first_bytes = AsBytes(first);
  StreamFeed feed;
  feed.buffers.push_back(AsBytes(std::vector<int16_t>{4, 5, 6, 7, 8}));
  PlayStream(cache_, 0, SIF_STREAMING_16_M, first_bytes, &feed);

  ExpectFrames(Mix(mixer_, 6), Mono({1, 2, 3, 4, 5, 6}), 0);
  EXPECT_EQ(feed.calls, 1);
  EXPECT_EQ(feed.seen_user_data, &feed);
  EXPECT_EQ(feed.seen_handle, 42);

  ExpectFrames(Mix(mixer_, 2), Mono({7, 8}), 0);
}

TEST_F(MixerTest, StreamEndsAndIsStoppedWhenCallbackHasNoMoreData) {
  std::vector<int16_t> only = {11, 22};
  std::vector<uint8_t> only_bytes = AsBytes(only);
  StreamFeed feed;
  PlayStream(cache_, 0, SIF_STREAMING_16_M, only_bytes, &feed);

  ExpectFrames(Mix(mixer_, 5), Mono({11, 22, 0, 0, 0}), 0);
  EXPECT_EQ(feed.calls, 1);
  ASSERT_EQ(fake_.stopped_uids.size(), 1u);
  EXPECT_EQ(fake_.stopped_uids[0], cache_[0].m_unique_id);
}

} // namespace
