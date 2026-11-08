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

#include "mixer_test_support.h"

#include <gtest/gtest.h>

void FakeLowLevelSound::StopSound(int sound_uid, uint8_t) {
  stopped_uids.push_back(sound_uid);
  for (int i = 0; i < kNumSlots; ++i) {
    if (cache[i].m_unique_id == sound_uid)
      cache[i].m_status = SSF_UNUSED;
  }
}

std::vector<int> g_reported_errors;
void RecordError(int code) { g_reported_errors.push_back(code); }
void IgnoreErrorText(const char *, ...) {}

void *FeedStream(void *user_data, int handle, int *size) {
  auto *feed = static_cast<StreamFeed *>(user_data);
  feed->calls++;
  feed->seen_user_data = user_data;
  feed->seen_handle = handle;
  if (feed->next >= feed->buffers.size()) {
    *size = 0;
    return nullptr;
  }
  std::vector<uint8_t> &buf = feed->buffers[feed->next++];
  *size = static_cast<int>(buf.size());
  return buf.data();
}

int SoundIndex(int slot) { return slot + 1; }

void PlayStatic(sound_buffer_info *cache, int slot, int16_t *data16, uint8_t *data8, int sample_length, int playable,
                float lvol, float rvol, bool looping, int loop_start, int loop_end) {
  const int idx = SoundIndex(slot);
  SoundFiles[idx].sample_16bit = data16;
  SoundFiles[idx].sample_8bit = data8;
  SoundFiles[idx].sample_length = sample_length;
  SoundFiles[idx].np_sample_length = playable;
  SoundFiles[idx].used = 1;
  Sounds[idx].sample_index = idx;
  Sounds[idx].loop_start = loop_start;
  Sounds[idx].loop_end = (loop_end < 0) ? playable - 1 : loop_end;
  Sounds[idx].used = 1;

  play_information &info = *cache[slot].play_info;
  cache[slot].m_sound_index = idx;
  cache[slot].m_status = looping ? SSF_PLAY_LOOPING : SSF_PLAY_NORMAL;
  info.left_volume = lvol;
  info.right_volume = rvol;
  info.m_samples_played = 0;
  info.sample_skip_interval = 0;
}

void Play16(sound_buffer_info *cache, int slot, std::vector<int16_t> &data, float lvol, float rvol, bool looping,
            int loop_start, int loop_end) {
  const int n = static_cast<int>(data.size());
  PlayStatic(cache, slot, data.data(), nullptr, n, n, lvol, rvol, looping, loop_start, loop_end);
}

void PlayStream(sound_buffer_info *cache, int slot, uint16_t format, std::vector<uint8_t> &first, StreamFeed *feed,
                float lvol, float rvol) {
  play_information &info = *cache[slot].play_info;
  cache[slot].m_status = SSF_PLAY_STREAMING;
  info.m_stream_format = format;
  info.m_stream_data = first.data();
  info.m_stream_size = static_cast<int>(first.size());
  info.m_stream_cback = feed ? &FeedStream : nullptr;
  info.user_data = feed;
  info.m_stream_handle = 42;
  info.left_volume = lvol;
  info.right_volume = rvol;
  info.m_samples_played = 0;
}

std::vector<Frame> Mix(software_mixer &mixer, int frames) {
  std::vector<int16_t> out(static_cast<size_t>(frames) * 2, 0x5A5A); // garbage: mixer must overwrite
  mixer.StreamMixer(reinterpret_cast<char *>(out.data()), frames * kFrameBytes);
  std::vector<Frame> result;
  for (int i = 0; i < frames; ++i)
    result.push_back({out[2 * i], out[2 * i + 1]});
  return result;
}

void ExpectFrames(const std::vector<Frame> &actual, const std::vector<Frame> &expected, int tolerance) {
  ASSERT_EQ(actual.size(), expected.size());
  for (size_t i = 0; i < actual.size(); ++i) {
    EXPECT_NEAR(actual[i].left, expected[i].left, tolerance) << "left channel, frame " << i;
    EXPECT_NEAR(actual[i].right, expected[i].right, tolerance) << "right channel, frame " << i;
  }
}

std::vector<Frame> Mono(const std::vector<int> &samples) {
  std::vector<Frame> f;
  for (int s : samples)
    f.push_back({s, s});
  return f;
}

bool IsMonoStreamFormat(uint16_t format) { return format == SIF_STREAMING_16_M || format == SIF_STREAMING_8_M; }

std::vector<Frame> ExpectedStreamFrames(const std::vector<Frame> &stereo, uint16_t format) {
  if (!IsMonoStreamFormat(format))
    return stereo;
  std::vector<Frame> e;
  for (const Frame &f : stereo)
    e.push_back({f.left, f.left});
  return e;
}

std::vector<uint8_t> EncodeStream(const std::vector<Frame> &stereo, uint16_t format) {
  std::vector<int> samples;
  for (const Frame &f : stereo) {
    samples.push_back(f.left);
    if (!IsMonoStreamFormat(format))
      samples.push_back(f.right);
  }
  if (format == SIF_STREAMING_16_M || format == SIF_STREAMING_16_S) {
    std::vector<int16_t> s16(samples.begin(), samples.end());
    return AsBytes(s16);
  }
  std::vector<uint8_t> s8;
  for (int s : samples)
    s8.push_back(static_cast<uint8_t>(s / 256 + 128));
  return s8;
}
