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

#ifndef LIBMVE_MOVIE_SOUND_H_
#define LIBMVE_MOVIE_SOUND_H_

#include <SDL_audio.h>

#include "sound_interface.h"

namespace D3 {

/// Implementation class for sound device used on movie playback.
class MovieSoundDevice : ISoundDevice {
private:
  SDL_AudioDeviceID m_device_id = 0;

public:
  /**
   * Initialize sound device
   * @param sample_rate sample rate in Hz (22050, 44100...)
   * @param sample_size sample size (8, 16)
   * @param channels count of channels (1 for mono, 2 for stereo)
   * @param buf_size buffer size for SDL audio device
   * @param is_compressed mark stream as compressed (on streaming will be used decompression functions)
   */
  MovieSoundDevice(int sample_rate, uint16_t sample_size, uint8_t channels, uint32_t buf_size, bool is_compressed);
  ~MovieSoundDevice();

  /**
   * Check if sound device is properly initialized
   * @return true on success
   */
  [[nodiscard]] bool IsInitialized() const { return m_device_id > 0; }

  /**
   * Fill internal audio stream to be played
   * @param stream source buffer
   * @param len length of source buffer
   */
  void FillBuffer(char *stream, int len) const;

  void Play() override;
  void Stop() override;
  void Lock() override;
  void Unlock() override;

  using ISoundDevice::IsCompressed;

};

}

#endif // LIBMVE_MOVIE_SOUND_H_
