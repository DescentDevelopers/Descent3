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

#ifndef __SOUND_MIXER_H______
#define __SOUND_MIXER_H______

#include "ssl_lib.h"

class sound_buffer {
public:
  virtual int GetNumBufferBytes(void) = 0;
  virtual void Write(uint8_t *buffer, int amount) = 0;

private:
};

struct DSLOOPSTREAM {
  int playing;
  int please_close;
  char *current_position;
  int bytes_left;
  int no_callbacks;
  int half_buffer_point;
  int last_half;
  int close_on_next;
  char silence_byte;
  bool f_sample_16bit;
  int num_written;
  bool kill_me;
};

// Sound item info (cache list)
// override if you need to keep more information
class sound_buffer_info {
public:
  sound_buffer_info() {
    m_status = SSF_UNUSED;
    s = NULL;
  }

  play_information *play_info;

  int m_sound_index; // Index of sound
  int m_unique_id;   // Unique id for the currently playing sample

  // Not needed by the software mixer
  volatile DSLOOPSTREAM *s; // Streaming info for a looping sample

  int16_t m_mixer_type;  // ds3d, ds_8?
  int16_t m_buffer_type; // Buffer type 2d or 3d

  char *sample_data;
  int sample_length; // used for storage purposes.

  float m_volume;

  bool stereo;
  int8_t bps;
  uint8_t m_status; // Sound status
  uint8_t pad;
};

struct tMixerInit {
  sound_buffer *primary_buffer;
  int primary_frequency;
  int primary_alignment;
  int *max_sounds_available;      // pointer to the variable that is updated with the # of sounds in the sound_cache
  sound_buffer_info *sound_cache; // the array of sound information

  void (*fp_SetError)(int code);
  void (*fp_ErrorText)(const char *fmt, ...);
  int *p_error_code;

  llsSystem *ll_sound_ptr;
};

class software_mixer {
public:
  software_mixer();
  ~software_mixer();

  bool Initialize(tMixerInit *mi);
  void DoFrame(void);

  // The actual mixer code that sum's the sounds on each channel and does all the actual
  // mixing and effects (writes data to the locked primary buffer)
  void StreamMixer(char *ptr, int len);

private:
  llsSystem *m_ll_sound_ptr;
  bool m_init;
  sound_buffer *m_primary_buffer;
  int *Fast_mixer;
  int Fast_mixer_len;
  int m_primary_alignment;
  int m_BufferSize;

  uint8_t *m_buffer;

  int *m_max_sounds_available;
  sound_buffer_info *m_sound_cache;

  void (*m_fpSetError)(int code);
  void (*m_fpErrorText)(const char *fmt, ...);
  int *m_error_code;
};

#endif
