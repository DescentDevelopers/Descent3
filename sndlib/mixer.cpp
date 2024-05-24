/*
* Descent 3 
* Copyright (C) 2024 Parallax Software
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

#include <cstdlib>
#include <memory.h>

#include "mono.h"
#include "ssl_lib.h"
#include "mixer.h"
#include "pserror.h"

#define MIN_SOUND_MIX_VOLUME 0.0f
#define MAX_WRITE_AHEAD 0.04f // Seconds to write ahead of the play position (in seconds)
#define VOLUME_FIX_BITS 1024

static inline void opti_8m_mix(uint8_t *cur_sample_8bit, const int num_write, int &samples_played,
                               short *mixer_buffer16, const float l_volume, const float r_volume);
static inline void opti_8s_mix(uint8_t *cur_sample_8bit, const int num_write, int &samples_played,
                               short *mixer_buffer16, const float l_volume, const float r_volume);
static inline void opti_16m_mix(short *cur_sample_16bit, const int num_write, int &samples_played,
                                short *mixer_buffer16, const float l_volume, const float r_volume);
static inline void opti_16s_mix(short *cur_sample_16bit, const int num_write, int &samples_played,
                                short *mixer_buffer16, const float l_volume, const float r_volume);

software_mixer::software_mixer() {
  m_init = false;
  m_buffer = NULL;
}

software_mixer::~software_mixer() {
  if (m_buffer) {
    free(m_buffer);
  }
}

bool software_mixer::Initialize(tMixerInit *mi) {
  if (m_init) {
    mprintf((0, "Mixer: Already Init\n"));
    Int3();
    return false;
  }

  // Sam 6/29 - When using SDL, there's no primary buffer
  // if(!mi->primary_buffer || !mi->ll_sound_ptr)
  if (!mi->ll_sound_ptr) {
    mprintf((0, "Mixer: Bad Value passed on init\n"));
    Int3();
    return false;
  }

  m_init = true;

  m_ll_sound_ptr = mi->ll_sound_ptr;
  m_primary_buffer = mi->primary_buffer;

  if (m_primary_buffer) {
    m_BufferSize = m_primary_buffer->GetNumBufferBytes();
  } else {
    m_BufferSize = 0;
  }

  m_max_sounds_available = mi->max_sounds_available;
  m_sound_cache = mi->sound_cache;
  m_primary_alignment = mi->primary_alignment;

  m_error_code = mi->p_error_code;
  m_fpSetError = mi->fp_SetError;
  m_fpErrorText = mi->fp_ErrorText;

  if (m_BufferSize) {
    m_buffer = (uint8_t *)malloc(m_BufferSize);
  }

  return true;
}

// A peroidic mixer that uses the primary buffer as a stream buffer
void software_mixer::DoFrame(void) {
  if (!m_init)
    return;

  int amount = m_BufferSize;

  StreamMixer((char *)m_buffer, amount);
  m_primary_buffer->Write(m_buffer, amount);
}

// The actual mixer code that sum's the sounds on each channel and does all the actual
// mixing and effects (writes data to the locked primary buffer)
void software_mixer::StreamMixer(char *ptr, int len) {
  int i;
  short *mixer_buffer16 = (short *)ptr;
  int current_slot = 0;
  bool f_loop;
  bool f_mono;

  const int buff_len = len / m_primary_alignment;

  // this code will assure that this function will not be called when  sound system is in error mode.
  if (*m_error_code != SSL_OK) {
    mprintf((0, "MIX: Mixer in error code %d\n", *m_error_code));
    return;
  }

  // ASSERT(len <= m_BufferSize);
  ASSERT(ptr && len >= 0);

  if ((len % m_primary_alignment) != 0) {
    mprintf((0, "MIX:Len is not aligned!\n"));
    (*m_fpSetError)(SSL_ERROR_STREAMMIXER);
    (*m_fpErrorText)("ASSERT((len % ll_sound_ptr->m_primary_alignment) == 0)\nLen:%d PrA:%d", len, m_primary_alignment);
    return;
  }

  memset((char *)ptr, 0, len);

  // Mix the sound slots
  while (current_slot < (*m_max_sounds_available)) {
    sound_buffer_info *cur_buf = &m_sound_cache[current_slot];
    int num_samples = buff_len;
    mixer_buffer16 = (short *)ptr;
    f_mono = true;

    // Find slots with sounds in them
    if ((cur_buf->m_status != SSF_UNUSED) && !(cur_buf->m_status & SSF_PAUSED)) {
      float l_volume = cur_buf->play_info->left_volume;
      float r_volume = cur_buf->play_info->right_volume;
      int skip_interval = cur_buf->play_info->sample_skip_interval;
      int samples_played = cur_buf->play_info->m_samples_played;
      short *sample_16bit;
      uint8_t *sample_8bit;
      int np_sample_length = 0;
      int sample_length;
      int loop_start;
      int loop_end;

      if (cur_buf->m_status & SSF_PLAY_STREAMING) {
        switch (cur_buf->play_info->m_stream_format) {
        case SIF_STREAMING_16_M:
          sample_16bit = (short *)cur_buf->play_info->m_stream_data;
          sample_8bit = NULL;
          np_sample_length = sample_length = cur_buf->play_info->m_stream_size / 2;
          break;
        case SIF_STREAMING_8_M:
          sample_16bit = NULL;
          sample_8bit = (uint8_t *)cur_buf->play_info->m_stream_data;
          np_sample_length = sample_length = cur_buf->play_info->m_stream_size;
          break;
        case SIF_STREAMING_16_S:
          sample_16bit = (short *)cur_buf->play_info->m_stream_data;
          sample_8bit = NULL;
          np_sample_length = sample_length = cur_buf->play_info->m_stream_size / 4;
          f_mono = false;
          break;
        case SIF_STREAMING_8_S:
          sample_16bit = NULL;
          sample_8bit = (uint8_t *)cur_buf->play_info->m_stream_data;
          np_sample_length = sample_length = cur_buf->play_info->m_stream_size / 2;
          f_mono = false;
          break;
        default:
          Int3();
          break;
        }
        loop_start = 0;
        loop_end = np_sample_length - 1;
      } else {
        int sound_index = cur_buf->m_sound_index;
        int sample_index = Sounds[sound_index].sample_index;
        sound_file_info *snd_file = &SoundFiles[sample_index];
        sample_16bit = snd_file->sample_16bit;
        sample_8bit = snd_file->sample_8bit;
        np_sample_length = snd_file->np_sample_length;
        sample_length = snd_file->sample_length;
        loop_start = Sounds[sound_index].loop_start;
        loop_end = Sounds[sound_index].loop_end;
        if (!sample_16bit && !sample_8bit) {
          mprintf((0, "sound file %s didn't have data for samples.\n", snd_file->name));
        }
      }

      // cleanly continue if this happens, and inform a logfile, if it does. error handling
      //	ASSERT(sample_16bit || sample_8bit);
      if (!sample_16bit && !sample_8bit) {
        sound_file_info *snd_file = &SoundFiles[Sounds[cur_buf->m_sound_index].sample_index];
        mprintf((0, "MIX: No data for %s\n", snd_file->name));
        (*m_fpSetError)(SSL_ERROR_SAMPLE_NODATA);
        (*m_fpErrorText)("ASSERT(sample_16bit || sample_8bit)\nNo data found for sound file: %s", snd_file->name);
        cur_buf->m_status = SSF_UNUSED;
        goto error_bail;
      }

      ASSERT(np_sample_length <= sample_length);
      ASSERT(samples_played >= 0 || samples_played <= sample_length);

      int num_write;

    error_trap:

      // We have not looped -- yet
      f_loop = false;

      // Verify the volume levels are o.k.
      ASSERT(l_volume >= 0.0 && l_volume <= 1.0);
      ASSERT(r_volume >= 0.0 && r_volume <= 1.0);

    looping: // Will go to this label to do the next iteration of a looping sample

      if (cur_buf->m_status &
          (SSF_PLAY_LOOPING | SSF_PLAY_STREAMING)) // Looping sample's process is broken up into linear pieces
      {
        if (f_loop) {
          ASSERT(num_write >= 0);

          num_samples -= num_write;
          ASSERT(num_samples > 0);

          mixer_buffer16 += num_write << 1; // update to the new start position
                                            // (2x because of left and right channels)
          samples_played = loop_start;
        }

        if (cur_buf->m_status & SSF_PLAY_LOOPING) // Looping sample's process is broken up into linear pieces
        {
          ASSERT(loop_end < sample_length);
          if (loop_end < samples_played) {
            if (loop_end != loop_start) {
              while (loop_end < samples_played) {
                samples_played -= loop_end - loop_start;
              }

              ASSERT(samples_played >= 0);
            } else {
              cur_buf->m_status &= ~SSF_PLAY_LOOPING;
              cur_buf->m_status |= SSF_PLAY_NORMAL;
            }

            goto error_trap;
          }
        }
        // The number of samples to write to the primary buffer
        num_write = ((num_samples) < (loop_end - samples_played + 1)) ? (num_samples) : (loop_end - samples_played + 1);
        ASSERT(num_write >= 0 && num_write <= num_samples);

        if (num_write < num_samples)
          f_loop = true;
        else
          f_loop = false;

        if (num_write <= 0) {
          num_write = 0;
          mprintf((0, "d"));
          goto stream_done;
        }
      } else {
        // The number of samples to write to the primary buffer
        num_write =
            ((num_samples) < (np_sample_length - samples_played)) ? (num_samples) : (np_sample_length - samples_played);
        if (!(num_write > 0 && num_write <= num_samples)) {
          num_write = 0;
          goto done;
        }

        // Optimization for silent sounds
        if (l_volume <= MIN_SOUND_MIX_VOLUME && r_volume <= MIN_SOUND_MIX_VOLUME) {
          cur_buf->play_info->m_samples_played += num_write;
          goto done;
        }
      }

      if ((num_write < 0) || (num_write > num_samples)) // this was an assert
      {
        num_write = 0;
        mprintf((0, "D"));
        goto done;
      }

      // Mix at 16 bits per sample
      if (skip_interval == 0) {
        short *cur_sample_16bit = sample_16bit;
        uint8_t *cur_sample_8bit = sample_8bit;

        if (f_mono) {
          if (sample_8bit) {
            cur_sample_8bit += samples_played;
            opti_8m_mix(cur_sample_8bit, num_write, samples_played, mixer_buffer16, l_volume, r_volume);
          } else {
            cur_sample_16bit += samples_played;
            opti_16m_mix(cur_sample_16bit, num_write, samples_played, mixer_buffer16, l_volume, r_volume);
          }
        } else {
          if (sample_8bit) {
            cur_sample_8bit += (samples_played << 1);
            opti_8s_mix(cur_sample_8bit, num_write, samples_played, mixer_buffer16, l_volume, r_volume);
          } else {
            cur_sample_16bit += (samples_played << 1);
            opti_16s_mix(cur_sample_16bit, num_write, samples_played, mixer_buffer16, l_volume, r_volume);
          }
        }
      } else
      // Account for lower-sampling rate
      {
        if (skip_interval == 1) {
          for (i = 0; i < (num_write << 1); i += 2) {
            int sample;

            if (sample_16bit) {
              if (samples_played & 0x0001) {
                sample = ((int)sample_16bit[samples_played ^ 0x0001] + (int)sample_16bit[samples_played + 1]) >> 1;
              } else
                sample = sample_16bit[samples_played];
            } else {
              if (samples_played & 0x0001) {
                // Notes: (<<7) is from a (<<8) - (>>1)
                // Notes: (-256) is from (-128) + (-128)
                sample = ((int)sample_8bit[samples_played ^ 0x0001] + (int)sample_8bit[samples_played + 1] - 256) << 7;
              } else
                sample = (((int)sample_8bit[samples_played]) - (int)128) << 8;
            }

            samples_played++;

            ASSERT(i >= 0 && (i + 1 < num_samples * 2));

            int l_sample = mixer_buffer16[i] + (sample * l_volume);
            int r_sample = mixer_buffer16[i + 1] + (sample * r_volume);

            if (l_sample < -32767)
              l_sample = -32767;
            if (l_sample > 32767)
              l_sample = 32767;
            if (r_sample < -32767)
              r_sample = -32767;
            if (r_sample > 32767)
              r_sample = 32767;

            mixer_buffer16[i] = (short)l_sample;
            mixer_buffer16[i + 1] = (short)r_sample;
          }
        } else {
          for (i = 0; i < (num_write << 1); i += 2) {
            int sample;
            const int mod_pos = samples_played % 4;

            if (sample_16bit) {
              switch (mod_pos) {
              case 0:
                sample = sample_16bit[samples_played];
                break;
              case 1:
                sample = (sample_16bit[samples_played - 1] * 3 + sample_16bit[samples_played + 3]) >> 2;
                break;
              case 2:
                sample = (sample_16bit[samples_played - 2] + sample_16bit[samples_played + 2]) >> 1;
                break;
              case 3:
                sample = (sample_16bit[samples_played - 3] + sample_16bit[samples_played + 1] * 3) >> 2;
                break;
              }
            } else {
              switch (mod_pos) {
              case 0:
                sample = ((((int)sample_8bit[samples_played]) - 128) << 8);
                break;
              case 1:
                sample = (((((int)sample_8bit[samples_played - 1]) - 128) << 8) * 3 +
                          ((((int)sample_8bit[samples_played + 3]) - 128) << 8)) >>
                         2;
                break;
              case 2:
                sample = (((((int)sample_8bit[samples_played - 2]) - 128) << 8) +
                          ((((int)sample_8bit[samples_played + 2]) - 128) << 8)) >>
                         1;
                break;
              case 3:
                sample = (((((int)sample_8bit[samples_played - 3]) - 128) << 8) +
                          ((((int)sample_8bit[samples_played + 1]) - 128) << 8) * 3) >>
                         2;
                break;
              }
            }

            samples_played++;

            ASSERT(i >= 0 && (i + 1 < num_samples * 2));

            int l_sample = mixer_buffer16[i] + (sample * l_volume);
            int r_sample = mixer_buffer16[i + 1] + (sample * r_volume);

            if (l_sample < -32767)
              l_sample = -32767;
            if (l_sample > 32767)
              l_sample = 32767;
            if (r_sample < -32767)
              r_sample = -32767;
            if (r_sample > 32767)
              r_sample = 32767;

            mixer_buffer16[i] = (short)l_sample;
            mixer_buffer16[i + 1] = (short)r_sample;
          }
        }
      }

    stream_done:

      cur_buf->play_info->m_samples_played = samples_played;

      if (cur_buf->m_status & SSF_PLAY_STREAMING) {
        if (f_loop) {
          if (cur_buf->play_info->m_stream_cback && cur_buf->play_info->m_stream_data) {
            cur_buf->play_info->m_stream_data = (*cur_buf->play_info->m_stream_cback)(
                cur_buf->play_info->user_data, cur_buf->play_info->m_stream_handle, &cur_buf->play_info->m_stream_size);
            //						cur_buf->s->current_position = (char
            //*)cur_buf->play_info->m_stream_data; 						mprintf((0, "%x %d\n",
            // cur_buf->play_info->m_stream_data, cur_buf->play_info->m_stream_size));
            ASSERT(!(cur_buf->play_info->m_stream_data && cur_buf->play_info->m_stream_size <= 0));
            //						mprintf((0, "Data %X, length %d\n",
            // cur_buf->play_info->m_stream_data, cur_buf->play_info->m_stream_size));

            if (cur_buf->play_info->m_stream_data) {
              switch (cur_buf->play_info->m_stream_format) {
              case SIF_STREAMING_16_M:
                sample_16bit = (short *)cur_buf->play_info->m_stream_data;
                loop_end = sample_length = np_sample_length = cur_buf->play_info->m_stream_size / 2;
                break;
              case SIF_STREAMING_8_M:
                sample_8bit = (uint8_t *)cur_buf->play_info->m_stream_data;
                loop_end = sample_length = np_sample_length = cur_buf->play_info->m_stream_size;
                break;
              case SIF_STREAMING_16_S:
                sample_16bit = (short *)cur_buf->play_info->m_stream_data;
                loop_end = sample_length = np_sample_length = cur_buf->play_info->m_stream_size / 4;
                break;
              case SIF_STREAMING_8_S:
                sample_8bit = (uint8_t *)cur_buf->play_info->m_stream_data;
                loop_end = sample_length = np_sample_length = cur_buf->play_info->m_stream_size / 2;
                break;
              default:
                Int3();
                break;
              }
              loop_end -= 1;
            } else {
              mprintf((0, "SE: Data is NULL\n"));
              cur_buf->m_status &= ~SSF_PLAY_STREAMING;
              f_loop = false;
            }
          } else {
            mprintf((0, "SE: Callback/data is NULL\n"));
            cur_buf->m_status &= ~SSF_PLAY_STREAMING;
            f_loop = false;
          }
        }
      }

      if (f_loop)
        goto looping;

    done:

      if (cur_buf->play_info->m_samples_played >= (np_sample_length) &&
          !(cur_buf->m_status & (SSF_PLAY_LOOPING | SSF_PLAY_STREAMING))) {
        m_ll_sound_ptr->StopSound(cur_buf->m_unique_id);
      }
    }

  error_bail:
    current_slot++;
  }
}

inline void opti_8m_mix(uint8_t *cur_sample_8bit, const int num_write, int &samples_played, short *mixer_buffer16,
                        const float l_volume, const float r_volume) {
  int i;
  short *mb = mixer_buffer16;

  for (i = 0; i < (num_write << 1); i += 2) {
    short sample;
    int l_sample;
    int r_sample;

    sample = (((short)(*cur_sample_8bit)) - (short)128) << 8;
    cur_sample_8bit++;

    l_sample = *mb + (sample * l_volume);
    r_sample = *(mb + 1) + (sample * r_volume);

    samples_played++;

    if (l_sample < -32767)
      l_sample = -32767;
    if (l_sample > 32767)
      l_sample = 32767;
    if (r_sample < -32767)
      r_sample = -32767;
    if (r_sample > 32767)
      r_sample = 32767;

    *mb = (short)l_sample;
    mb++;
    *mb = (short)r_sample;
    mb++;
  }
}

inline void opti_8s_mix(uint8_t *cur_sample_8bit, const int num_write, int &samples_played, short *mixer_buffer16,
                        const float l_volume, const float r_volume) {
  int i;
  short *mb = mixer_buffer16;

  for (i = 0; i < (num_write << 1); i += 2) {
    short lsample;
    short rsample;
    int l_sample;
    int r_sample;

    lsample = (((short)(*cur_sample_8bit)) - (short)128) << 8;
    cur_sample_8bit++;
    rsample = (((short)(*cur_sample_8bit)) - (short)128) << 8;
    cur_sample_8bit++;

    l_sample = *mb + (lsample * l_volume);
    r_sample = *(mb + 1) + (rsample * r_volume);

    samples_played++;

    if (l_sample < -32767)
      l_sample = -32767;
    if (l_sample > 32767)
      l_sample = 32767;
    if (r_sample < -32767)
      r_sample = -32767;
    if (r_sample > 32767)
      r_sample = 32767;

    *mb = (short)l_sample;
    mb++;
    *mb = (short)r_sample;
    mb++;
  }
}

inline void opti_16m_mix(short *cur_sample_16bit, const int num_write, int &samples_played, short *mixer_buffer16,
                         const float l_volume, const float r_volume) {
  int i;
  short *mb = mixer_buffer16;

  for (i = 0; i < (num_write << 1); i += 2) {
    short sample;
    int l_sample;
    int r_sample;

    sample = *cur_sample_16bit;
    cur_sample_16bit++;

    l_sample = *mb + (sample * l_volume);
    r_sample = *(mb + 1) + (sample * r_volume);

    samples_played++;

    if (l_sample < -32767)
      l_sample = -32767;
    if (l_sample > 32767)
      l_sample = 32767;
    if (r_sample < -32767)
      r_sample = -32767;
    if (r_sample > 32767)
      r_sample = 32767;

    *mb = (short)l_sample;
    mb++;
    *mb = (short)r_sample;
    mb++;
  }
}

inline void opti_16s_mix(short *cur_sample_16bit, const int num_write, int &samples_played, short *mixer_buffer16,
                         const float l_volume, const float r_volume) {
  int i;
  short *mb = mixer_buffer16;

  for (i = 0; i < (num_write << 1); i += 2) {
    short lsample;
    short rsample;
    int l_sample;
    int r_sample;

    lsample = *cur_sample_16bit;
    cur_sample_16bit++;
    rsample = *cur_sample_16bit;
    cur_sample_16bit++;

    l_sample = *mb + (lsample * l_volume);
    r_sample = *(mb + 1) + (rsample * r_volume);

    samples_played++;

    if (l_sample < -32767)
      l_sample = -32767;
    if (l_sample > 32767)
      l_sample = 32767;
    if (r_sample < -32767)
      r_sample = -32767;
    if (r_sample > 32767)
      r_sample = 32767;

    *mb = (short)l_sample;
    mb++;
    *mb = (short)r_sample;
    *mb++;
  }
}
