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
 * $Revision: 6 $
 * $Author: Samir $
 * $Date: 5/03/99 3:12a $
 *
 * Direct Sound subsystem.
 *
 * $Log: /DescentIII/Main/dd_sndlib/dsound3d.cpp $
 *
 * 6     5/03/99 3:12a Samir
 * fixed up aureal so it works (a little slow though...)
 *
 * 5     4/29/99 3:01p Samir
 * added code for direct sound mixers only (and function for Aureal
 * really) that will use direct sound looping for simple loops.
 *
 * 4     4/25/99 9:53p Samir
 * added debugging.
 *
 * 3     4/23/99 7:51p Samir
 * looping fixes for directsound.
 *
 * 2     4/22/99 10:33p Samir
 * modifications so that DirectSound mixers use one thread for all looping
 * and streaming sounds.   It worked without crashing for about twenty
 * minutes of playing from level 1 to level 2 of D3.  We'll see.
 *
 * 1     4/22/99 10:30p Samir
 * initial revision ( a bit messy)
 *
 */

#include "ds3dlib_internal.h"
#include "auddev.h"

#include "pserror.h"

#include <process.h>

static struct t_sb_loop_thread_data {
  win_llsSystem *m_ll_sndsys;
  uintptr_t thread_handle;
  short no_callbacks;
  bool request_kill;
  bool thread_alive;
} m_ds;

static ubyte m_sb_cur_timeslice;

////////////////////////////////////////////////////////////////////////////////
// DSLOOP_STREAM_METHOD

inline void sb_loop_thread_clean_buffer(sound_buffer_info *sb) {
  m_ds.m_ll_sndsys->GetSoundPos(sb->m_unique_id);
  sb_stop_buffer(sb);
  sb->s->playing = 0;
  sb->s->kill_me = true;

  if (SoundFiles[Sounds[sb->m_sound_index].sample_index].use_count > 0) {
    SoundFiles[Sounds[sb->m_sound_index].sample_index].use_count--;
    // DUPSND	if (SoundFiles[Sounds[sb->m_sound_index].sample_index].use_count == 0) {
    //	Global_DS_alloced_sounds--;
    // DUPSND	}
  }
}

// helper functions for loop streaming.
void sb_loop_stream_copy(sound_buffer_info *sb, char *ptr, DWORD len) {
  DWORD amt;
  bool f_looping = (sb->m_status & SSF_PLAY_LOOPING) != 0;
  char *sample_ptr;

  if (f_looping) {
    const int sound_index = sb->m_sound_index;
    int loop_start = Sounds[sound_index].loop_start;
    int loop_end = Sounds[sound_index].loop_end;
    if (sb->s->f_sample_16bit) {
      loop_start = loop_start << 1;
      loop_end = loop_end << 1;
      sample_ptr = (char *)SoundFiles[Sounds[sound_index].sample_index].sample_16bit;
    } else {
      sample_ptr = (char *)SoundFiles[Sounds[sound_index].sample_index].sample_8bit;
    }

    while (sb->s->num_written + (int)len >= loop_end) {
      int num_till_loop_end = loop_end - sb->s->num_written;
      int num_to_loop_start = sb->s->num_written - loop_start;

      if (num_till_loop_end > 0) {
        memcpy(ptr, sb->s->current_position, num_till_loop_end);
        len -= num_till_loop_end;
      }
      ptr += num_till_loop_end;

      sb->s->current_position = sample_ptr + loop_start;
      //			ASSERT(sb->s->current_position >= sample_ptr && sample_ptr == sb->sample_data);

      sb->s->num_written -= num_to_loop_start;
      sb->s->bytes_left += num_to_loop_start;

      //			ASSERT(sb->s->num_written == loop_start);
    }
  }

  amt = (len > sb->s->bytes_left) ? sb->s->bytes_left : len;

  //	int i_amt = (int)amt;

  //	ASSERT(i_amt >= 0);

  if (amt) {
    memcpy(ptr, sb->s->current_position, amt);
    sb->s->current_position += amt;
    sb->s->bytes_left -= amt;
    sb->s->num_written += amt;
    //		ASSERT(sb->s->current_position >= sb->sample_data);
  }

  len -= amt;

  if (len) {
    memset(ptr + amt, sb->s->silence_byte, len);
    sb->s->close_on_next = 1;
  }
}

// helper functions for loop streaming.
void sb_loop_stream_fillhalf(sound_buffer_info *sb, DWORD half) {
  char *ptr1 = NULL;
  char *ptr2 = NULL;
  uint len1, len2;

  if (sb_lock_buffer(sb, half, sb->s->half_buffer_point, (void **)&ptr1, &len1, (void **)&ptr2, &len2)) {
    // memset(ptr1, sb->s->silence_byte, len1);
    sb_loop_stream_copy(sb, ptr1, len1);
    if (ptr2) {
      sb_loop_stream_copy(sb, ptr2, len2);
    }
    sb_unlock_buffer(sb, ptr1, len1, ptr2, len2);
  }
}

// helper functions for streaming
void sb_stream_copy(sound_buffer_info *sb, char *ptr, DWORD len) {
  DWORD amt;

new_data:

  amt = (len > sb->s->bytes_left) ? sb->s->bytes_left : len;

  ASSERT(amt >= 0);

  if (amt) {
    ASSERT(sb->s->current_position);

    memcpy(ptr, sb->s->current_position, amt);
    sb->s->current_position += amt;
    sb->s->bytes_left -= amt;
  }

  len -= amt;

  if (len) {
    if (sb->play_info->m_stream_cback && sb->s->current_position) {
      memset(ptr + amt, sb->s->silence_byte, len);
      int new_len = sb->play_info->m_stream_size;
      sb->s->current_position =
          (char *)(*sb->play_info->m_stream_cback)(sb->play_info->user_data, sb->play_info->m_stream_handle, &new_len);
      sb->play_info->m_stream_data = sb->s->current_position;
      ASSERT(!(sb->play_info->m_stream_data && sb->play_info->m_stream_size <= 0));

      sb->s->bytes_left = sb->play_info->m_stream_size;

      if (sb->s->current_position == NULL) {
        sb->s->bytes_left = sb->play_info->m_stream_size = 0;
      }

      goto new_data;
    }

    memset(ptr + amt, sb->s->silence_byte, len);
    sb->s->close_on_next = 1;
  }
}

void sb_stream_fillhalf(sound_buffer_info *sb, DWORD half) {
  char *ptr1 = NULL;
  char *ptr2 = NULL;
  uint len1, len2;

  if (sb_lock_buffer(sb, half, sb->s->half_buffer_point, (void **)&ptr1, &len1, (void **)&ptr2, &len2)) {
    sb_stream_copy(sb, ptr1, len1);
    if (ptr2) {
      sb_stream_copy(sb, ptr2, len2);
    }
    sb_unlock_buffer(sb, ptr1, len1, ptr2, len2);
  }
}

// main looping thread.
void __cdecl sb_loop_thread(void *user_ptr) {
  sound_buffer_cache *sndcache;
  DWORD playp, writep, whichhalf;
  int i;
  ubyte iteration;

  // validate thread
  m_ds.m_ll_sndsys = (win_llsSystem *)user_ptr;
  m_ds.thread_alive = true;
  sndcache = &m_ds.m_ll_sndsys->m_sound_mixer;
  iteration = 0;

  mprintf((0, "DS3DLIB: Looping thread begins.\n"));

  // main thread body
  while (!m_ds.request_kill) {
    if (m_ds.no_callbacks++ == 0) {
      for (i = 0; i < sndcache->m_max_sounds_played; i++) {
        sound_buffer_info *sb = &sndcache->m_sound_cache[i];

        // skip unused slots and only handle looping (maybe streaming) slots.
        if ((sb->m_status == SSF_UNUSED) || (sb->m_status & SSF_BUFFERED_LOOP) || !sb->s) {
          continue;
        }
        if (!sb->s->playing) { // loops before calling play do have SSF_PLAY_LOOPING set, and MUST!!
          continue;
        }

        // at this point it's definitely either a streaming buffer or loop streaming buffer
        if (sb->m_status & SSF_PLAY_STREAMING) {
          // streams will stop at the request of the application always, unlike looping buffers (see below)
          if (sb->s->please_close) {
            sb_loop_thread_clean_buffer(sb);
            //	mprintf((0, "ds thread pleas_close request processed.\n"));
          } else if (sb->m_status & SSF_PAUSED) {
            continue; // just continue
          } else if ((iteration % 4) == (sb->s->time_slice % 4)) {
            // update streamed buffer only when allowed
            playp = sb_get_current_position(sb, (uint *)&writep);
            whichhalf = (playp < sb->s->half_buffer_point) ? 0 : sb->s->half_buffer_point;

            if (whichhalf != sb->s->last_half) {
              if (sb->s->close_on_next) {
                sb_loop_thread_clean_buffer(sb);
              } else {
                sb_stream_fillhalf(sb, sb->s->last_half);
                sb->s->last_half = whichhalf;
              }
            }
            //	mprintf((0, "ds thread stream update.\n"));
          }
        } else if (!(sb->m_status & SSF_BUFFERED_LOOP)) {
          // this slot is a looping slot.  check to see if app requested closing this loop.
          // also we don't check the looping flag because we could be playing the end part of a loop
          // and the stream filling code below will set close_on_next when done itself.
          if (sb->s->please_close) {
            sb_loop_thread_clean_buffer(sb);
          } else if (sb->m_status & SSF_PAUSED) {
            continue; // just continue
          } else if ((iteration % 4) == (sb->s->time_slice % 4)) {
            // update looped buffer only when allowed
            playp = sb_get_current_position(sb, (uint *)&writep);
            whichhalf = (playp < sb->s->half_buffer_point) ? 0 : sb->s->half_buffer_point;

            if (whichhalf != sb->s->last_half) {
              if (sb->s->close_on_next) {
                sb_loop_thread_clean_buffer(sb);
              } else {
                sb_loop_stream_fillhalf(sb, sb->s->last_half);
                sb->s->last_half = whichhalf;
              }
            }
          }
        }
      }
      iteration++;
    }
    m_ds.no_callbacks--;
    Sleep(DSPB_TICK_MILLISECONDS); // defer to OS
  }

  // invalidate thread
  m_ds.thread_alive = false;
  m_ds.m_ll_sndsys = NULL;
  mprintf((0, "DS3DLIB: Looping thread done.\n"));
}

///////////////////////////////////////////////////////////////////////////////
// this will initialize the looping thread

bool sb_loop_thread_init(win_llsSystem *lls) {
  // start looping thread
  m_sb_cur_timeslice = 0;

  m_ds.request_kill = false;
  m_ds.thread_alive = false;
  m_ds.no_callbacks = 1;

  m_ds.thread_handle = _beginthread(sb_loop_thread, 16384, (void *)lls);
  if (m_ds.thread_handle == -1) {
    return false;
  }
  if (!SetThreadPriority((HANDLE)m_ds.thread_handle, THREAD_PRIORITY_TIME_CRITICAL)) {
    return false;
  }

  m_ds.no_callbacks = 0;

  return true;
}

void sb_loop_thread_kill() {
  if (m_ds.thread_alive) {
    mprintf((0, "DS3DLIB: Killing looping thread.\n"));
    m_ds.request_kill = true;
    while (m_ds.thread_alive) {
      Sleep(DSPB_TICK_MILLISECONDS);
    }
  }
}

// a lot of looping info will be initialized here, including copying data, etc.
bool sb_loop_element_init(sound_buffer_info *sb, char *sample_ptr, int sound_length, int buffer_size) {
  // finish initting loop
  if (!sb->s) {
    Int3();
    return false;
  }

  sb->s->time_slice = m_sb_cur_timeslice; // allocate timeslice for updating.
  sb->s->half_buffer_point = buffer_size / 2;
  sb->s->current_position = sample_ptr;
  sb->s->bytes_left = sound_length;
  sb->s->silence_byte = (sb->s->f_sample_16bit) ? 0 : 128;
  sb_loop_stream_fillhalf(sb, 0);
  sb_loop_stream_fillhalf(sb, sb->s->half_buffer_point);

  sb->s->close_on_next = 0;
  sb->s->kill_me = false;
  m_ds.m_ll_sndsys->SetSoundPos(sb->m_unique_id, sb->play_info->m_samples_played);

  m_sb_cur_timeslice++;

  return true;
}

void sb_loop_element_kill(sound_buffer_info *sb) { sb->s->please_close = 1; }

void sb_loop_element_wait_until_dead(sound_buffer_info *sb) {
  if (!sb->s)
    return;

  while (!sb->s->kill_me) {
  }
}

// initialize streaming audio.
bool sb_stream_element_init(sound_buffer_info *sb, char *sample_ptr, int sound_length, int buffer_size) {
  // finish initting loop
  if (!sb->s) {
    Int3();
    return false;
  }

  sb->s->time_slice = m_sb_cur_timeslice; // allocate timeslice for updating.

  sb->s->half_buffer_point = buffer_size / 2;
  sb->s->current_position = sample_ptr;
  sb->s->bytes_left = sound_length;
  sb->s->silence_byte = (sb->s->f_sample_16bit) ? 0 : 128;
  sb_stream_fillhalf(sb, 0);
  sb_stream_fillhalf(sb, sb->s->half_buffer_point);
  sb->s->close_on_next = 0;
  sb->s->kill_me = false;

  if (sb->m_status & SSF_BUFFERED_STRM) {
    sb->s->hEvent = NULL;
    /*		sb->s->hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
                    if (sb->m_mixer_type == SOUND_MIXER_AUREAL) {
                            if (!A3D_SetSourceWaveEvent(sb->m_snd_obj, sb->s->half_buffer_point, sb->s->hEvent)) {
                                    CloseHandle(sb->s->hEvent);
                                    sb->s->hEvent = NULL;
                                    return false;
                            }
                    }
                    else {
                            Int3();
       // unimplemented for normal DirectSound CloseHandle(sb->s->hEvent); sb->s->hEvent = NULL;
                    }
    */
  }

  m_sb_cur_timeslice++;

  return true;
}

// these work on buffered streams only
void sb_stream_element_kill(sound_buffer_info *sb) {
  if (sb->m_status & SSF_BUFFERED_STRM) {
    if (sb->s->hEvent) {
      // if (sb->m_mixer_type == SOUND_MIXER_AUREAL) {
      //	A3D_SetSourceWaveEvent(sb->m_snd_obj, 0, NULL);
      //}
      CloseHandle(sb->s->hEvent);
      sb->s->hEvent = NULL;
    }
  }
}

void sb_stream_buffered_update(sound_buffer_info *sb) {
  DWORD playp, writep, whichhalf;
  ASSERT((sb->m_status & SSF_BUFFERED_STRM));

  playp = sb_get_current_position(sb, (uint *)&writep);
  whichhalf = (playp < sb->s->half_buffer_point) ? 0 : sb->s->half_buffer_point;

  if (whichhalf != sb->s->last_half) {
    //	mprintf((0, "DSOUND3D: event triggered. Updating stream half %d.\n", sb->s->last_half));
    if (sb->s->close_on_next) {
      extern win_llsSystem *ll_sound_ptr;
      ll_sound_ptr->StopSound(sb->m_unique_id);
    } else {
      sb_stream_fillhalf(sb, sb->s->last_half);
      sb->s->last_half = whichhalf;
    }
  }

  /*	if (WaitForSingleObject(sb->s->hEvent, 0) == WAIT_OBJECT_0) {
          // event was signaled by DirectSound/Aureal.  do appropriate stream fill.
                  if (sb->s->close_on_next) {
                          extern win_llsSystem *ll_sound_ptr;
                          ll_sound_ptr->StopSound(sb->m_unique_id);
                  }
                  else {
                          mprintf((0, "DSOUND3D: event triggered. Updating stream half %d.\n", sb->s->last_half));
                          A3D_ClearSourceWaveEvents(sb->m_snd_obj);
                          sb_stream_fillhalf(sb, sb->s->last_half);
                          sb->s->last_half = (sb->s->last_half) ?  0 : sb->s->half_buffer_point;
                          ResetEvent(sb->s->hEvent);
                          if (sb->m_mixer_type == SOUND_MIXER_AUREAL) {
                                  A3D_SetSourceWaveEvent(sb->m_snd_obj, sb->s->last_half, sb->s->hEvent);
                          }
                          else {
                                  Int3();
                          }
                  }
          }
  */
}

////////////////////////////////////////////////////////////////////////////////
// DSLOOP_BUFFER_METHOD

char *sb_get_loop_info(const sound_buffer_info *sb, int *loop_start, int *loop_end, bool *is_16_bit) {
  int sound_index = sb->m_sound_index;

  if (sound_index > -1) {
    *loop_start = Sounds[sound_index].loop_start;
    *loop_end = Sounds[sound_index].loop_end;

    if (SoundFiles[Sounds[sound_index].sample_index].sample_16bit) {
      *is_16_bit = true;
      return (char *)SoundFiles[Sounds[sound_index].sample_index].sample_16bit;
    }
    if (SoundFiles[Sounds[sound_index].sample_index].sample_8bit) {
      *is_16_bit = false;
      return (char *)SoundFiles[Sounds[sound_index].sample_index].sample_8bit;
    }
  }
  Int3();
  return NULL;
}

char *sb_get_loop_step_info(const sound_buffer_info *sb, int step, bool is16bit, int *length) {
  sound_file_info *sf;
  char *sample_ptr;

  if (sb->m_sound_index < 0) {
    Int3();
    return NULL;
  }

  int loop_start_byte = Sounds[sb->m_sound_index].loop_start;
  int loop_end_byte = Sounds[sb->m_sound_index].loop_end;

  if (is16bit) {
    loop_start_byte = loop_start_byte << 1;
    loop_end_byte = loop_end_byte << 1;
  }

  sf = &SoundFiles[Sounds[sb->m_sound_index].sample_index];
  sample_ptr = is16bit ? (char *)sf->sample_16bit : (char *)sf->sample_8bit;

  if (step == DSBUFLOOP_INIT_STEP) {
    *length = loop_start_byte;
    return sample_ptr;
  }
  if (step == DSBUFLOOP_LOOP_STEP) {
    *length = (loop_end_byte - loop_start_byte);
    return (sample_ptr + loop_start_byte);
  }
  if (step == DSBUFLOOP_FINISH_STEP) {
    int sample_length_byte = is16bit ? (sf->np_sample_length * 2) : sf->np_sample_length;
    *length = (sample_length_byte - loop_end_byte);
    return (sample_ptr + loop_end_byte);
  }

  Int3(); // illegal step!!
  *length = 0;

  return NULL;
}

// steps to next state of buffered loop.
// force_next_step = -2, do it automatically, otherwise set 'next_step' using passed value.
// this will stop and free the current ds object
// advance to next valid step (may skip a step)
// if new step is > 1, then we're done.
//	else create and play the new one if we can.

// before destroying old object, we must get the current sound properties depending on buffer type.
// and set them for the new sound object.
// we will call either direct sound or custom mixer functions.
// yuck.

void sb_buffered_loop_step(win_llsSystem *lls, sound_buffer_info *sb, int force_next_step) {
  if (!sb->s)
    return;
  if (!(sb->m_status & SSF_BUFFERED_LOOP))
    return;

  tPSBInfo psb;
  char *sample_ptr;
  int sound_length;

  pos_state old_pos_state;
  float old_pan;
  float old_volume = 0.0f;

  ASSERT(lls->m_mixer_type != SOUND_MIXER_SOFTWARE_16 && lls->m_mixer_type != SOUND_MIXER_NONE);

  // get current properties.
  old_pan = 0;
#ifdef SUPPORT_AUREAL
  if (lls->m_mixer_type == SOUND_MIXER_AUREAL) {
    matrix old_orient;
    vector old_pos, old_vel;
    old_volume = A3D_GetSourceVolume(sb->m_snd_obj);

    if (sb->m_buffer_type == SBT_3D) {
      A3D_GetSourceVelocity(sb->m_snd_obj, &old_vel);
      A3D_GetSourcePosition(sb->m_snd_obj, &old_pos);
      A3D_GetSourceOrientation(sb->m_snd_obj, &old_orient);
      old_pos_state.orient = &old_orient;
      old_pos_state.position = &old_pos;
      old_pos_state.velocity = &old_vel;
    } else {
      float lpan, rpan;
      A3D_GetSourcePan(sb->m_snd_obj, &lpan, &rpan);
      old_pan = rpan - lpan;
    }
  }
#endif

  // advance to next step.
  sound_length = 0;
  sb->s->loop_step = (force_next_step == -2) ? (sb->s->loop_step + 1) : force_next_step;
  while (!sound_length && sb->s->loop_step < 2) {
    sample_ptr = sb_get_loop_step_info(sb, sb->s->loop_step, sb->s->f_sample_16bit, &sound_length);
    sb->s->loop_step++;
  }
  if (!sound_length && sb->s->loop_step == 2) {
    //	mprintf((0, "DS3DLIB: Buffered loop %d advancing to post-end step (done)\n", sb->m_unique_id));
    lls->StopSound(sb->m_unique_id);
    return;
  } else {
    sb_stop_buffer(sb);
    sb_free_buffer(sb);
  }

  sb->s->loop_step--; // return to proper step.
  sb->s->loop_timer = 0.0f;
  sb->s->bytes_left = sound_length;
  sb->m_status = SSF_PLAY_LOOPING | SSF_BUFFERED_LOOP;
  sb->sample_data = sample_ptr;

  // allocate buffer for playback
  if (!lls->CreateSoundBuffer(sb, false, sb->s->bytes_left, true)) {
    return;
  }

  if (!sb_load_buffer(sb, sb->sample_data, sound_length)) {
    return;
  }

  // using old sound properties, play the next buffer with those qualities!
  psb.looping = (sb->s->loop_step == 0) ? true : false;

  if (sb->m_buffer_type == SBT_3D) {
    psb.cur_pos = &old_pos_state;
  } else {
    psb.pan = old_pan;
  }

  psb.volume = old_volume;
  psb.freq = 22050;
  lls->PlaySoundBuffer(sb, &psb);

  // must be at end to initiate thread management.
  sb->s->playing = 1;

  //	mprintf((0, "DDSNDLIB: Buffered loop %d advancing to step %d.\n", sb->m_unique_id, sb->s->loop_step));
}
