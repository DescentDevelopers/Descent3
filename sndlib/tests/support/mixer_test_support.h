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

#pragma once

#include <cstdint>
#include <vector>

#include "mixer.h"
#include "ssl_lib.h"

constexpr int kNumSlots = 4;

// 16-bit stereo, which is how sdlsound.cpp configures the mixer.
constexpr int kFrameBytes = 4;
constexpr int kVolumeTolerance = 1;

struct Frame {
  int left;
  int right;
};

// Like the real lnxsound::StopSound, stopping a sound frees its cache slot.
class FakeLowLevelSound : public llsSystem {
public:
  sound_buffer_info *cache = nullptr;
  std::vector<int> stopped_uids;

  void StopSound(int sound_uid, uint8_t) override;

  void SetSoundCard(const char *) override {}
  int InitSoundLib(char, oeApplication *, uint8_t) override { return 0; }
  void DestroySoundLib() override {}
  bool LockSound(int) override { return true; }
  bool UnlockSound(int) override { return true; }
  bool SetSoundQuality(char) override { return true; }
  char GetSoundQuality() override { return SQT_NORMAL; }
  bool SetSoundMixer(char) override { return true; }
  char GetSoundMixer() override { return SOUND_MIXER_SOFTWARE_16; }
  int PlaySound2d(play_information *, int, float, float, bool) override { return -1; }
  int PlayStream(play_information *) override { return -1; }
  void SetListener(pos_state *) override {}
  int PlaySound3d(play_information *, int, pos_state *, float, bool, float) override { return -1; }
  void AdjustSound(int, float, float, uint16_t) override {}
  void AdjustSound(int, pos_state *, float, float) override {}
  void StopAllSounds() override {}
  bool IsSoundInstancePlaying(int) override { return false; }
  int IsSoundPlaying(int) override { return -1; }
  void PauseSounds() override {}
  void ResumeSounds() override {}
  void PauseSound(int) override {}
  void ResumeSound(int) override {}
  bool CheckAndForceSoundDataAlloc(int) override { return true; }
  void SoundStartFrame() override {}
  void SoundEndFrame() override {}
  bool SetGlobalReverbProperties(float, float, float) override { return false; }
  void SetEnvironmentValues(const t3dEnvironmentValues *) override {}
  void GetEnvironmentValues(t3dEnvironmentValues *) override {}
  void SetEnvironmentToggles(const t3dEnvironmentToggles *) override {}
  void GetEnvironmentToggles(t3dEnvironmentToggles *) override {}
};

extern std::vector<int> g_reported_errors;
void RecordError(int code);
void IgnoreErrorText(const char *, ...);

struct StreamFeed {
  std::vector<std::vector<uint8_t>> buffers;
  size_t next = 0;
  int calls = 0;
  void *seen_user_data = nullptr;
  int seen_handle = -1;
};

// Stream callback for play_information::m_stream_cback. The callback interface only carries an opaque user_data
// pointer, so the StreamFeed is passed through it and cast back.
void *FeedStream(void *user_data, int handle, int *size);

template <typename T> std::vector<uint8_t> AsBytes(const std::vector<T> &v) {
  const auto *p = reinterpret_cast<const uint8_t *>(v.data());
  return std::vector<uint8_t>(p, p + v.size() * sizeof(T));
}

// Sounds[] / SoundFiles[] index used for a cache slot.
int SoundIndex(int slot);

// Starts a static sound in cache[slot] (whose play_info must be set). `playable` is the non-padded length; any
// samples past it act as the loader's alignment padding.
void PlayStatic(sound_buffer_info *cache, int slot, int16_t *data16, uint8_t *data8, int sample_length, int playable,
                float lvol, float rvol, bool looping = false, int loop_start = 0, int loop_end = -1);

// Starts a static 16-bit sound whose whole buffer is playable.
void Play16(sound_buffer_info *cache, int slot, std::vector<int16_t> &data, float lvol = 1.0f, float rvol = 1.0f,
            bool looping = false, int loop_start = 0, int loop_end = -1);

// Starts a streaming sound in cache[slot] with `first` as its initial buffer and `feed` (if any) as its refill source.
void PlayStream(sound_buffer_info *cache, int slot, uint16_t format, std::vector<uint8_t> &first, StreamFeed *feed,
                float lvol = 1.0f, float rvol = 1.0f);

// Runs the mixer for `frames` stereo frames over a garbage-filled buffer and returns the output.
std::vector<Frame> Mix(software_mixer &mixer, int frames);

// Expects each channel of each frame to be within `tolerance` of the expected value.
void ExpectFrames(const std::vector<Frame> &actual, const std::vector<Frame> &expected,
                  int tolerance = kVolumeTolerance);

// Frames carrying the same sample on both channels.
std::vector<Frame> Mono(const std::vector<int> &samples);

// Whether a SIF_STREAMING_* format is mono.
bool IsMonoStreamFormat(uint16_t format);

// The frames the mixer should produce for `stereo` encoded in `format` (mono formats carry only the left channel).
std::vector<Frame> ExpectedStreamFrames(const std::vector<Frame> &stereo, uint16_t format);

// Encodes `stereo` as a stream buffer in `format`.
std::vector<uint8_t> EncodeStream(const std::vector<Frame> &stereo, uint16_t format);
