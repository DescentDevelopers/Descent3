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
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "ssl_lib.h"
#include "d3_test/files.h"
#include "d3_test/game_clock.h"
#include "streamaudio.h"
#include "support/fake_sound.h"
#include "support/test_helpers.h"
#include "support/test_stream.h"

namespace {

// TwoBuffers fits in the initial read-ahead; SixBuffers is longer than the read-ahead and must be refilled while
// playing. Each length is a whole number of buffers so that no trailing partial buffer is involved.
enum StreamKind { kOneBuffer, kTwoBuffers, kSixBuffers, kNumStreamKinds };
const char *const kStreamNames[kNumStreamKinds] = {"OneBuffer", "TwoBuffers", "SixBuffers"};
const size_t kStreamSamples[kNumStreamKinds] = {10000, 60000, 6 * 44100};

class AudioStreamTest : public ::testing::Test {
protected:
  static void SetUpTestSuite() {
    d3_test::EnsureChronoTimer();
    s_dir = std::make_unique<d3_test::TempDir>();
    for (int i = 0; i < kNumStreamKinds; i++)
      s_streams[i] = MakeTestStream(s_dir->path(), kStreamNames[i], kStreamSamples[i]);
  }
  static void TearDownTestSuite() {
    s_dir.reset();
  }

  void SetUp() override {
    ASSERT_FALSE(s_dir->path().empty());
    for (int i = 0; i < kNumStreamKinds; i++) {
      ASSERT_FALSE(s_streams[i].osf.empty()) << "failed to generate " << kStreamNames[i];
      // AudioStream sizes its playback from the ACM header's sample count, so it must match the decoded data.
      ASSERT_EQ(s_streams[i].pcm.size(), kStreamSamples[i] * 2) << kStreamNames[i];
    }
    AudioStream::InitSystem(&m_sound);
  }

  void TearDown() override {
    // Reset the global stream table without touching any stream objects.
    AudioStream::InitSystem(nullptr);
  }

  FakeSoundSystem m_sound;

  static std::unique_ptr<d3_test::TempDir> s_dir;
  static TestStream s_streams[kNumStreamKinds];
};

std::unique_ptr<d3_test::TempDir> AudioStreamTest::s_dir;
TestStream AudioStreamTest::s_streams[kNumStreamKinds];

TEST_F(AudioStreamTest, OpenFailsWithoutSoundSystem) {
  AudioStream::InitSystem(nullptr);
  auto s = std::make_unique<AudioStream>();
  EXPECT_FALSE(s->Open(s_streams[kSixBuffers].osf));
  EXPECT_EQ(s->State(), STRM_INVALID);
}

TEST_F(AudioStreamTest, OpenedStreamIsStoppedAndReadyToPlay) {
  auto s = OpenTestStream(s_streams[kSixBuffers]);
  ASSERT_NE(s, nullptr);
  EXPECT_EQ(s->State(), STRM_STOPPED);
  EXPECT_TRUE(s->IsReady());
  EXPECT_TRUE(m_sound.started.empty()) << "opening must not start playback";
}

TEST_F(AudioStreamTest, OpenRejectsNonOsfFileAndStaysUsable) {
  const std::filesystem::path bogus = s_dir->Write("not_a_stream.osf", std::vector<uint8_t>(1000, 0x5a));

  auto s = std::make_unique<AudioStream>();
  EXPECT_FALSE(s->Open(bogus));
  EXPECT_EQ(s->State(), STRM_INVALID);

  EXPECT_TRUE(s->Open(s_streams[kOneBuffer].osf));
  EXPECT_EQ(s->State(), STRM_STOPPED);
}

struct LoopCase {
  StreamKind kind;
  int loop_count;
};

class AudioStreamLoopTest : public AudioStreamTest, public ::testing::WithParamInterface<LoopCase> {};

TEST_P(AudioStreamLoopTest, PlaysDecodedAudioLoopCountTimesThenStops) {
  const LoopCase c = GetParam();
  const std::vector<uint8_t> &pcm = s_streams[c.kind].pcm;
  auto s = OpenTestStream(s_streams[c.kind]);
  ASSERT_NE(s, nullptr);
  s->SetLoopCount(c.loop_count);
  ASSERT_TRUE(s->Play());
  ASSERT_EQ(m_sound.started.size(), 1u);
  const int handle = m_sound.started[0].handle;
  EXPECT_EQ(s->GetSoundHandle(), handle);

  FakeMixer mixer(m_sound.LastStarted());
  mixer.PlayUntil(pcm.size() * (c.loop_count + 2)); // bounded, in case the stream fails to end
  EXPECT_TRUE(mixer.ended());
  EXPECT_TRUE(mixer.played() == Repeat(pcm, c.loop_count))
      << "played " << mixer.played().size() << " bytes, expected " << c.loop_count << " x " << pcm.size();

  m_sound.instances_playing = false;
  AudioStream::Frame();
  EXPECT_EQ(s->State(), STRM_STOPPED);
  EXPECT_EQ(m_sound.stopped, std::vector<int>{handle});
}

INSTANTIATE_TEST_SUITE_P(Buffering, AudioStreamLoopTest,
                         ::testing::Values(LoopCase{kOneBuffer, 1}, LoopCase{kOneBuffer, 2}, LoopCase{kOneBuffer, 3},
                                           LoopCase{kTwoBuffers, 1}, LoopCase{kTwoBuffers, 2},
                                           LoopCase{kTwoBuffers, 3}, LoopCase{kSixBuffers, 1},
                                           LoopCase{kSixBuffers, 2}, LoopCase{kSixBuffers, 3}),
                         [](const ::testing::TestParamInfo<LoopCase> &info) {
                           return std::string(kStreamNames[info.param.kind]) + "_Loops" +
                                  std::to_string(info.param.loop_count);
                         });

TEST_F(AudioStreamTest, InfiniteLoopRunsUntilSoftStopThenEndsAtLoopBoundary) {
  const std::vector<uint8_t> &pcm = s_streams[kSixBuffers].pcm;
  auto s = OpenTestStream(s_streams[kSixBuffers]);
  ASSERT_NE(s, nullptr);
  s->SetLoopCount(0);
  ASSERT_TRUE(s->Play());

  FakeMixer mixer(m_sound.LastStarted());
  mixer.PlayUntil(pcm.size() * 3 / 2);
  ASSERT_FALSE(mixer.ended()) << "an infinitely looping stream ended on its own";

  s->Stop(true);
  mixer.PlayUntil(pcm.size() * 5); // bounded
  EXPECT_TRUE(mixer.ended());
  EXPECT_TRUE(mixer.played() == Repeat(pcm, 2))
      << "played " << mixer.played().size() << " bytes, expected 2 x " << pcm.size();
}

TEST_F(AudioStreamTest, ImmediateStopMidStreamStopsTheSound) {
  auto s = OpenTestStream(s_streams[kSixBuffers]);
  ASSERT_NE(s, nullptr);
  ASSERT_TRUE(s->Play());
  const int handle = m_sound.started.at(0).handle;
  FakeMixer mixer(m_sound.LastStarted());
  ASSERT_TRUE(mixer.NextBuffer());
  ASSERT_FALSE(mixer.ended());

  s->Stop();
  EXPECT_EQ(s->State(), STRM_STOPPED);
  EXPECT_EQ(m_sound.stopped, std::vector<int>{handle});
  EXPECT_EQ(s->GetSoundHandle(), -1);
}

TEST_F(AudioStreamTest, StoppingAStreamFreesItsPlaybackSlot) {
  std::vector<std::unique_ptr<AudioStream>> playing;
  for (int i = 0; i < STRM_LIMIT; i++) {
    auto s = OpenTestStream(s_streams[kOneBuffer]);
    ASSERT_NE(s, nullptr);
    ASSERT_TRUE(s->Play()) << "stream " << i;
    playing.push_back(std::move(s));
  }

  auto extra = OpenTestStream(s_streams[kOneBuffer]);
  ASSERT_NE(extra, nullptr);
  EXPECT_FALSE(extra->Play()) << "more than STRM_LIMIT streams playing";

  playing[1]->Stop();
  EXPECT_TRUE(extra->Play());
}

TEST_F(AudioStreamTest, StreamPlayStartsOnNextFramePlaysOnceAndCleansUp) {
  const std::vector<uint8_t> &pcm = s_streams[kSixBuffers].pcm;
  const int handle = StreamPlay(s_streams[kSixBuffers].osf, 1.0f, 0);
  ASSERT_GE(handle, 0);
  EXPECT_TRUE(m_sound.started.empty()) << "StreamPlay must defer the start to the next frame";

  AudioStream::Frame();
  ASSERT_EQ(m_sound.started.size(), 1u);
  EXPECT_EQ(StreamGetSoundHandle(handle), m_sound.started[0].handle);

  FakeMixer mixer(m_sound.LastStarted());
  mixer.PlayUntil(pcm.size() * 3); // bounded
  EXPECT_TRUE(mixer.ended());
  EXPECT_TRUE(mixer.played() == pcm) << "played " << mixer.played().size() << " bytes, expected " << pcm.size();

  m_sound.instances_playing = false;
  AudioStream::Frame();
  EXPECT_EQ(StreamGetSoundHandle(handle), -1) << "finished one-shot stream still active";

  StreamStop(handle);
}

} // namespace
