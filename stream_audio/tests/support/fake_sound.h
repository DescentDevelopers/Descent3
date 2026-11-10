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

#include <cstddef>
#include <cstdint>
#include <vector>

#include "ssl_lib.h"
#include "streamaudio.h"

class FakeSoundSystem final : public llsSystem {
public:
  struct Started {
    int handle;
    play_information *info;
  };
  std::vector<Started> started;
  std::vector<int> stopped;
  bool instances_playing = true;

  // The play_information of the most recent PlayStream call, or nullptr if there was none.
  play_information *LastStarted() const { return started.empty() ? nullptr : started.back().info; }

  int PlayStream(play_information *play_info) override {
    int handle = m_next_handle++;
    started.push_back({handle, play_info});
    return handle;
  }
  bool IsSoundInstancePlaying(int) override { return instances_playing; }
  void StopSound(int sound_uid, uint8_t) override { stopped.push_back(sound_uid); }

  void SetSoundCard(const char *) override {}
  int InitSoundLib(char, oeApplication *, uint8_t) override { return 1; }
  void DestroySoundLib() override {}
  bool LockSound(int) override { return true; }
  bool UnlockSound(int) override { return true; }
  bool SetSoundQuality(char) override { return true; }
  char GetSoundQuality() override { return 0; }
  bool SetSoundMixer(char) override { return true; }
  char GetSoundMixer() override { return 0; }
  int PlaySound2d(play_information *, int, float, float, bool) override { return -1; }
  void SetListener(pos_state *) override {}
  int PlaySound3d(play_information *, int, pos_state *, float, bool, float) override { return -1; }
  void AdjustSound(int, float, float, uint16_t) override {}
  void AdjustSound(int, pos_state *, float, float) override {}
  void StopAllSounds() override {}
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

private:
  int m_next_handle = 100;
};

class FakeMixer {
public:
  explicit FakeMixer(play_information *info) : m_info(info) {
    if (m_info->m_stream_data && m_info->m_stream_size > 0) {
      Append(m_info->m_stream_data, m_info->m_stream_size);
    }
  }

  bool NextBuffer() {
    if (m_ended)
      return false;
    AudioStream::Frame();
    m_info->m_stream_data = m_info->m_stream_cback(m_info->user_data, m_info->m_stream_handle, &m_info->m_stream_size);
    if (!m_info->m_stream_data) {
      m_ended = true;
      return false;
    }
    Append(m_info->m_stream_data, m_info->m_stream_size);
    return true;
  }

  void PlayUntil(size_t bytes) {
    while (m_out.size() < bytes && NextBuffer()) {
    }
  }

  const std::vector<uint8_t> &played() const { return m_out; }
  bool ended() const { return m_ended; }

private:
  void Append(const void *data, int size) {
    const uint8_t *p = static_cast<const uint8_t *>(data);
    m_out.insert(m_out.end(), p, p + size);
  }

  play_information *m_info;
  std::vector<uint8_t> m_out;
  bool m_ended = false;
};
