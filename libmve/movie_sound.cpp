/*
 * Descent 3
 * Copyright (C) 2024 Descent Developers
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

#include "movie_sound.h"

namespace D3 {

MovieSoundDevice::MovieSoundDevice(int sample_rate, uint16_t sample_size, uint8_t channels, bool is_compressed) {
  SDL_AudioSpec spec{};
  spec.freq = sample_rate;
  spec.format = (sample_size == 2) ? AUDIO_S16LSB : AUDIO_U8;
  spec.channels = channels;

  m_device_id = SDL_OpenAudioDevice(nullptr, 0, &spec, nullptr, 0);
  m_is_compressed = is_compressed;
  m_sample_size = sample_size;
};

MovieSoundDevice::~MovieSoundDevice() {
  if (m_device_id > 0) {
    SDL_CloseAudioDevice(m_device_id);
    m_device_id = 0;
  }
}

void MovieSoundDevice::FillBuffer(char *stream, int len) const {
  SDL_QueueAudio(m_device_id, stream, len);
};

void MovieSoundDevice::Play() { SDL_PauseAudioDevice(m_device_id, 0); }

void MovieSoundDevice::Stop() { SDL_PauseAudioDevice(m_device_id, 1); }

void MovieSoundDevice::Lock() { SDL_LockAudioDevice(m_device_id); }

void MovieSoundDevice::Unlock() { SDL_UnlockAudioDevice(m_device_id); }

} // namespace D3
