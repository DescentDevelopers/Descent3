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

--- HISTORICAL COMMENTS FOLLOW ---

 * $Source: $
 * $Revision: 9 $
 * $Author: Samir $
 * $Date: 8/23/99 5:29p $
 *
 * Win32 Sound Library Internal Header.
 *
 * $Log: /DescentIII/Main/dd_sndlib/ds3dlib_internal.h $
 *
 * 9     8/23/99 5:29p Samir
 * incremental EAX 2.0 checkin
 *
 * 8     5/03/99 3:12a Samir
 * fixed up aureal so it works (a little slow though...)
 *
 * 7     4/29/99 3:01p Samir
 * added code for direct sound mixers only (and function for Aureal
 * really) that will use direct sound looping for simple loops.
 *
 * 6     4/22/99 10:33p Samir
 * modifications so that DirectSound mixers use one thread for all looping
 * and streaming sounds.   It worked without crashing for about twenty
 * minutes of playing from level 1 to level 2 of D3.  We'll see.
 *
 * 5     4/13/99 4:09p Samir
 * more priority stuff.
 *
 * 4     4/12/99 7:14p Samir
 * prioritization code added.
 *
 * 3     4/10/99 5:08p Samir
 * took out obsolete data from play_information structure that should save
 * around 70 bytes per instance.
 *
 * 2     4/06/99 8:29p Samir
 * added error check system.
 *
 * 1     4/06/99 8:16p Samir
 * Initial rev.
 *
 */

#ifndef DS3DLIB_INTERNAL_H
#define DS3DLIB_INTERNAL_H

#include <windows.h>
#include <mmsystem.h>

#include "win/directx/dsound.h"
#include "ssl_lib.h"

class win_llsSystem;

struct DSLOOPSTREAM {
  uint8_t method; // method 0 for streamed method, 1 for emulated method
  uint8_t playing;
  uint8_t please_close;

  union // dependant on method.
  {
    uint8_t close_on_next;
    int8_t loop_step; // method 1, = 0 for start loop, 1 = mid loop, 2 = end loop.
  };

  union // dependant on method.
  {
    DWORD bytes_left;
    float loop_timer; // method 1, time spent in current loop step
  };

  char *current_position; // used for streamed method
  HANDLE hEvent;          // used for event notification.
  DWORD half_buffer_point;
  DWORD last_half;
  int num_written;
  uint8_t time_slice;
  bool kill_me;
  bool f_sample_16bit;
  char silence_byte;
};

struct DSSTREAM {
  // These 2 pointers are the same on good sound cards
  LPDIRECTSOUNDBUFFER m_lp_primary_buffer; // pointer to the primary sound buffer
  LPDIRECTSOUNDBUFFER m_lp_looping_buffer; // pointer to the looping sound buffer
  bool m_f_secondary_looping;

  int BufferSize; // Size of the primary buffer

  int MaxWriteSamples;
  int MaxWriteBytes;

  uintptr_t thread_handle;
  //	unsigned thread_id;
  volatile bool thread_request_kill;
  volatile bool thread_alive;

  // Updating state variables
  int NextWritePos; // Last bit position we wrote sound data to
  int LastPlayPos;  // Last bit position that the play pointer was recorded at

};

struct tPSBInfo {
  float volume;
  union {
    float pan;
    pos_state *cur_pos;
  };
  float reverb;
  uint16_t freq;
  uint16_t priority;
  bool looping;
};

// Sound item info (cache list)
class sound_buffer_info {
public:
  sound_buffer_info() {
    m_status = SSF_UNUSED;
    m_sound_buffer = NULL;
    m_sound_buffer_3d = NULL;
    s = NULL;
  }

  play_information *play_info;

  int m_sound_index; // Index of sound
  int m_unique_id;   // Unique id for the currently playing sample

  // Not needed by the software mixer
  volatile DSLOOPSTREAM *s; // Streaming info for a looping sample
  union {
    LPDIRECTSOUNDBUFFER m_sound_buffer; // May be a pointer to a 2d or 3d sound buffer
    void *m_snd_obj;                    // used for non direct sound systems.
  };
  LPDIRECTSOUND3DBUFFER m_sound_buffer_3d; // Used for 3d interface
  LPKSPROPERTYSET m_lpksps;                // used mainly for EAX 2.0 but is a direct sound object.

  int16_t m_mixer_type;  // aureal, ds3d, ds_8?
  int16_t m_buffer_type; // Buffer type 2d or 3d

  char *sample_data;
  int sample_length; // used for storage purposes.

  bool stereo;
  int8_t bps;
  uint8_t m_status; // Sound status
  uint8_t pad;

  float m_volume; // kept for priority.
};

// stop a sound buffer.
void sb_stop_buffer(sound_buffer_info *sb);

// frees a sound buffer.
void sb_free_buffer(sound_buffer_info *sb);

// load data into sound buffer
bool sb_load_buffer(sound_buffer_info *sb, void *sample_data, int length);

// update current position of sound buffer
int sb_get_current_position(sound_buffer_info *sb, uint32_t *writep);

// locks a sound buffer
bool sb_lock_buffer(sound_buffer_info *sb, uint32_t dwWriteCursor, uint32_t dwWriteBytes, void **lplpvAudioPtr1,
                    uint32_t *lpdwAudioBytes1, void **lplpvAudioPtr2, uint32_t *lpdwAudioBytes2);

// unlocks an allocated sound buffer
bool sb_unlock_buffer(sound_buffer_info *sb, void *ptr1, uint32_t len1, void *ptr2, uint32_t len2);

// this will initialize the looping thread
bool sb_loop_thread_init(win_llsSystem *lls);

//	kill looped thread
void sb_loop_thread_kill();

// initializes a loop for the loop thread.
bool sb_loop_element_init(sound_buffer_info *sb, char *sample_ptr, int sound_length, int buffer_size);

// initialize streaming audio.
bool sb_stream_element_init(sound_buffer_info *sb, char *sample_ptr, int sound_length, int buffer_size);

// these work on both loops and streams.
void sb_loop_element_kill(sound_buffer_info *sb);
void sb_loop_element_wait_until_dead(sound_buffer_info *sb);

// these work on buffered streams only
void sb_stream_element_kill(sound_buffer_info *sb);
void sb_stream_buffered_update(sound_buffer_info *sb);

// used for buffered loops.
char *sb_get_loop_info(const sound_buffer_info *sb, int *loop_byte_start, int *loop_byte_end, bool *is_16_bit);
char *sb_get_loop_step_info(const sound_buffer_info *sb, int step, bool is16bit, int *length);
void sb_buffered_loop_step(win_llsSystem *lls, sound_buffer_info *sb, int force_next_step = -2);

#include "ds3dlib.h"
#include "ddsndgeometry.h"

#endif
