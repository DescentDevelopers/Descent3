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

/*
 * $Logfile: /DescentIII/Main/dd_lnxsound/lnxsound.cpp $
 * $Revision: 1.3 $
 * $Date: 2000/06/24 01:15:15 $
 * $Author: icculus $
 *
 * Low-level linux sound driver
 *
 * $Log: lnxsound.cpp,v $
 * Revision 1.3  2000/06/24 01:15:15  icculus
 * patched to compile.
 *
 * Revision 1.2  2000/05/29 05:17:52  icculus
 * Now uses SDL threads instead of pthreads (but NOT SDL audio). Other
 * fixes, too.
 *
 * Revision 1.1.1.1  2000/04/18 00:00:33  icculus
 * initial checkin
 *
 *
 * 18    10/17/99 3:47p Jeff
 * added a function to get sound config parameters
 *
 * 17    8/26/99 7:25p Jeff
 * improved sound.  Sounds pretty good now...worried about constant low
 * frame rate though (sub 10)
 *
 * 16    8/24/99 7:57p Jeff
 * sound working very well now...although there is a small pause every 2
 * seconds...
 *
 * 15    8/24/99 4:28a Jeff
 * no more circular buffers, just take mixer output and write it right
 * away.
 *
 * 14    8/23/99 4:17a Jeff
 * barely 'correct' sound implements.  A little static still and some
 * sounds don't play (3d issue?)
 *
 * 13    8/21/99 2:55a Jeff
 * fixed typo bug
 *
 * 12    8/20/99 7:21p Jeff
 * sound! well, a little, still pretty buggy, but it's something
 *
 * 11    8/20/99 1:34a Jeff
 * blah...got disconnected checking in the file
 *
 * 9     8/19/99 7:11p Jeff
 * initialize sound library and start mixing thread
 *
 * 8     8/17/99 3:11p Jeff
 * added llsGeometry::Clear
 *
 * 7     8/17/99 2:36p Jeff
 * updated for new geometry functions
 *
 * 6     4/17/99 2:11a Jeff
 * added low level geometry stubs
 *
 * 5     4/17/99 1:52a Jeff
 * commented out Sounds[], included in ddsoundload.cpp
 *
 * 4     4/17/99 1:15a Jeff
 * added SetGlobalReverbProperties
 *
 * 3     4/16/99 4:00a Jeff
 * declare some needed globals
 *
 * 2     4/14/99 1:55a Jeff
 * fixed case mismatched #includes
 *
 * 1     1/15/99 4:02a Jeff
 *
 * $NoKeywords: $
 */

//	NEED THIS SINCE DDSNDLIB is a DD library.
#include "DDAccess.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <linux/soundcard.h>
#include <stdarg.h>
#include <errno.h>
#include "cfile.h"
#include "pserror.h"
#include "ddebug/mono.h"
#include "soundload.h"
#include "ssl_lib.h"
#include "mem.h"
#include "application.h"
#include "linux/lnxsound.h"
#include "ddlnxsound.h"
#include "mixer.h"
#include "ddio.h"
#include "SDL.h"
#include "SDL_thread.h"

#define SOUNDLIB_SAMPLE_RATE 22050
#define SOUNDLIB_SAMPLE_SIZE 16
#define SOUNDLIB_CHANNELS 2

/*
// ===============================
// pthread library functions
// ===============================
#include <pthread.h>

typedef int (*pthread_create_fp)(pthread_t *__thread,__const pthread_attr_t *__attr,void *(*__start_routine) (void
*),void *__arg); typedef void (*pthread_exit_fp)(void *__retval); typedef int (*pthread_detach_fp)(pthread_t __th);
typedef pthread_t (*pthread_self_fp)(void);

pthread_create_fp dpthread_create = NULL;
pthread_exit_fp dpthread_exit = NULL;
pthread_detach_fp dpthread_detach = NULL;
pthread_self_fp dpthread_self = NULL;
void *pthread_lib = NULL;
*/

// ===============================
#define MAX_SOUNDS_PLAYING_AT_ONCE 256
static sound_buffer_info sound_cache[MAX_SOUNDS_PLAYING_AT_ONCE];
static int sound_buffer_size = MAX_SOUNDS_PLAYING_AT_ONCE;

LNXSTREAM m_sb_info;
lnxsound *ll_sound_ptr;

class lnxsound_buffer : public sound_buffer {
public:
  // if sound_device is -1 then it is a primary buffer
  lnxsound_buffer(int buffer_size, int *sound_device = NULL);
  ~lnxsound_buffer();

  int GetNumBufferBytes(void);
  void Write(unsigned char *buffer, int amount);

private:
  int *m_sound_device;
  int m_buffer_size;
};

// A peroidic mixer that uses the primary buffer as a stream buffer
int StreamTimer(void *user_ptr);

lnxsound::lnxsound() : llsSystem() {
  /*
          pthread_lib = NULL;
          dpthread_create = NULL;
          dpthread_exit = NULL;
          dpthread_detach = NULL;
          dpthread_self = NULL;
  */
  ll_sound_ptr = this;
  sound_device = -1;
  in_at_exit = false;
}

lnxsound::~lnxsound() {
  in_at_exit = true;
  DestroySoundLib();
  SetSoundCard(NULL);
}

// Starts the sound library, maybe have it send back some information -- 3d support?
int lnxsound::InitSoundLib(char mixer_type, oeApplication *sos, unsigned char max_sounds_played) {
  sound_device = open("/dev/dsp", O_WRONLY | O_NONBLOCK, 0);
  if (sound_device < 0)
    mprintf((0, "Sound: Unable to open /dev/dsp !"));
  else {
    // success
    mprintf((0, "Sound: /dev/dsp opened for output. Configuring..."));

    fcntl(sound_device, F_SETFL, 0); // remove nonblock flag.

    // setup the soundcard
    int arg, status;

    int tmp = 11 | (2 << 16);
    status = ioctl(sound_device, SNDCTL_DSP_SETFRAGMENT, &tmp);
    if (status) {
      Error("Sound: Unable to optimize fragment size\n");
      close(sound_device);
      sound_device = -1;
    }

    status = ioctl(sound_device, SNDCTL_DSP_RESET, 0);
    if (status == -1) {
      Error("Sound: Unable to reset sound system\n");
      close(sound_device);
      sound_device = -1;
    }

    // set sample size
    arg = SOUNDLIB_SAMPLE_SIZE;
    status = ioctl(sound_device, SOUND_PCM_WRITE_BITS, &arg);
    if (status == -1) {
      Error("Sound: Unable to set sample size (try --nosound)\n");
      close(sound_device);
      sound_device = -1;
    }
    if (arg != SOUNDLIB_SAMPLE_SIZE) {
      Error("Sound: Unable to set sample size to %d bits (try --nosound)\n", SOUNDLIB_SAMPLE_SIZE);
      close(sound_device);
      sound_device = -1;
    }

    // set the data type
    arg = AFMT_S16_LE;
    status = ioctl(sound_device, SOUND_PCM_SETFMT, &arg);
    if (status == -1) {
      Error("Sound: Unable to set data type (try --nosound)\n");
      close(sound_device);
      sound_device = -1;
    }
    if (arg != AFMT_S16_LE) {
      Error("Sound: Unable to set data type to signed 16bit Little Endian (try --nosound)\n");
      close(sound_device);
      sound_device = -1;
    }

    // set channels
    arg = SOUNDLIB_CHANNELS;
    status = ioctl(sound_device, SOUND_PCM_WRITE_CHANNELS, &arg);
    if (status == -1) {
      Error("Sound: Unable to set channels (try --nosound)\n");
      close(sound_device);
      sound_device = -1;
    }
    if (arg != SOUNDLIB_CHANNELS) {
      Error("Sound: Unable to set channels to %s (try --nosound)\n", (SOUNDLIB_CHANNELS == 2) ? "stereo" : "mono");
      close(sound_device);
      sound_device = -1;
    }

    // set sample rate
    arg = SOUNDLIB_SAMPLE_RATE;
    status = ioctl(sound_device, SOUND_PCM_WRITE_RATE, &arg);

    mprintf((0, "Sound: Sample rate was set to %d.", arg));

    if (status == -1) {
      Error("Sound: Unable to set sample rate (try --nosound)\n");
      close(sound_device);
      sound_device = -1;
    }

    if ((arg < SOUNDLIB_SAMPLE_RATE - 50) && (arg > SOUNDLIB_SAMPLE_RATE + 50)) {
      Error("Sound: Unable to set sample rate to ~%d (try --nosound)\n", SOUNDLIB_SAMPLE_RATE);
      close(sound_device);
      sound_device = -1;
    }

    if (sound_device >= 0) {
      mprintf((0, "Sound: Hardware configured. Kicking off stream thread..."));
      StartStreaming();
      m_total_sounds_played = 0;
      m_cur_sounds_played = 0;
      m_in_sound_frame = false;
      m_pending_actions = false;
      m_cache_stress_timer = 0.0f;
      m_timer_last_frametime = -1;
      m_sound_quality = SQT_HIGH;
    } // if
  }

  return (sound_device >= 0) ? 1 : 0;
}

bool lnxsound::GetDeviceSettings(int *device, unsigned int *freq, unsigned int *bit_depth, unsigned int *channels) {
  if (sound_device == -1)
    return false;

  *device = sound_device;
  *freq = SOUNDLIB_SAMPLE_RATE;
  *bit_depth = SOUNDLIB_SAMPLE_SIZE;
  *channels = SOUNDLIB_CHANNELS;

  return true;
}

// Cleans up after the Sound Library
void lnxsound::DestroySoundLib(void) {
  EndStreaming();
  if (sound_device > 0) {
    close(sound_device);
    sound_device = -1;
  }
}

// Locks and unlocks sounds (used when changing play_info data)
bool lnxsound::LockSound(int sound_uid) { return false; }

bool lnxsound::UnlockSound(int sound_uid) { return false; }

bool lnxsound::SetSoundQuality(char quality) {
  int i;

  if (quality == m_sound_quality)
    return true;

  //	 pause any sounds that may be playing
  PauseSounds();

  if (quality == SQT_NORMAL) {
    m_sound_quality = SQT_NORMAL;
  } else {
    m_sound_quality = SQT_HIGH;
  }

  for (i = 0; i < MAX_SOUNDS; i++) {
    if (Sounds[i].used != 0) {
      int j = Sounds[i].sample_index;

      if (SoundFiles[j].sample_8bit && m_sound_quality == SQT_HIGH) {
        GlobalFree(SoundFiles[j].sample_8bit);
        SoundFiles[j].sample_8bit = NULL;

        CheckAndForceSoundDataAlloc(i);
      }
      if (SoundFiles[j].sample_16bit && m_sound_quality == SQT_NORMAL) {
        int count;

        ASSERT(SoundFiles[j].sample_8bit == NULL);
        SoundFiles[j].sample_8bit = (unsigned char *)GlobalAlloc(0, SoundFiles[j].sample_length);

        // NOTE:  Interesting note on sound conversion:  16 bit sounds are signed (0 biase).  8 bit sounds are unsigned
        // (+128 biase).
        for (count = 0; count < (int)SoundFiles[j].sample_length; count++) {
          SoundFiles[j].sample_8bit[count] = (unsigned char)((((int)SoundFiles[j].sample_16bit[count]) + 32767) >> 8);
        }

        GlobalFree(SoundFiles[j].sample_16bit);
        SoundFiles[j].sample_16bit = NULL;
      }
    }
  }

  ResumeSounds();

  return true;
}

char lnxsound::GetSoundQuality(void) { return m_sound_quality; }

bool lnxsound::SetSoundMixer(char mixer_type) { return true; }

char lnxsound::GetSoundMixer(void) { return SOUND_MIXER_SOFTWARE_16; }

// Determines if a sound will play.  Takes into account maximum allowable
// sounds.
// Also put prioritization code in here
//		ignore reserved slots
#ifdef _DEBUG
short lnxsound::FindFreeSoundSlot(int sound_index, float volume, int priority)
#else
short lnxsound::FindFreeSoundSlot(float volume, int priority)
#endif
{
  int current_slot;
  sound_buffer_info *sb;

  for (current_slot = 0; current_slot < MAX_SOUNDS_PLAYING_AT_ONCE; current_slot++) {
    sb = &sound_cache[current_slot];
    if (sb->m_status == SSF_UNUSED) {
      return current_slot;
    }
  }

  // no more slots? take priority into account.
  // throw out lowest priority sound slot (must be lower than or equal to new sound priority)
  float weighted_priority = (priority * 2.0f) * volume;
  if (current_slot == MAX_SOUNDS_PLAYING_AT_ONCE) {
    int throw_out_slot = -1, equiv_priority_slot = -1;
    float weighted_priorityA, weighted_priorityB;
    for (current_slot = 0; current_slot < MAX_SOUNDS_PLAYING_AT_ONCE; current_slot++) {
      sb = &sound_cache[current_slot];
      if (!(sb->m_status & (SSF_PLAY_LOOPING + SSF_PLAY_STREAMING))) {
        weighted_priorityA = sb->play_info->priority * 2.0f * sb->m_volume;
        if (weighted_priorityA < weighted_priority) {
          if (throw_out_slot == -1) {
            throw_out_slot = current_slot;
          } else {
            play_information *play_info2 = sound_cache[throw_out_slot].play_info;
            weighted_priorityB = play_info2->priority * 2.0f * sb->m_volume;
            if (weighted_priorityB > weighted_priorityA) {
              throw_out_slot = current_slot;
            }
          }
        }

        else if (equiv_priority_slot == -1 && weighted_priorityA == weighted_priority) {
          equiv_priority_slot = current_slot;
        }
      }
    }

    // if no slot found to stop, look for a slot with priority == new priority
    if (throw_out_slot == -1) {
      throw_out_slot = equiv_priority_slot;
    }
    if (throw_out_slot > -1) {
      sb = &sound_cache[throw_out_slot];
      StopSound(sb->m_unique_id, SKT_HOLD_UNTIL_STOP);
      mprintf((0, "DDSNDLIB: Replace sound (p:%d) with sound (p:%d) in slot %d\n", sb->play_info->priority, priority,
               throw_out_slot));
      return throw_out_slot;
    }
  }

#ifdef _DEBUG
  if (sound_index > -1) {
    mprintf((0, "DDSNDLIB: Sound %s with priority (%d) too low.\n", Sounds[sound_index].name, priority));
  } else {
    mprintf((0, "DDSNDLIB: Sound unknown with priority (%d) too low.\n", priority));
  }
#endif
  return -1;
}

// Plays a 2d sound
int lnxsound::PlaySound2d(play_information *play_info, int sound_index, float f_volume, float f_pan, bool f_looped) {
  sound_buffer_info *sb = NULL;
  short sound_slot;

  if (sound_device == -1) {
    return -1;
  }

  // calculate volume and pan
  f_volume = (f_volume < 0.0f) ? 0.0f : (f_volume > 1.0f) ? 1.0f : f_volume;
  play_info->left_volume = play_info->right_volume = f_volume;

  f_pan = (f_pan < -1.0f) ? -1.0f : (f_pan > 1.0f) ? 1.0f : f_pan;
  if (f_pan < 0.0) {
    play_info->right_volume += f_volume * f_pan;
  } else {
    play_info->left_volume -= f_volume * f_pan;
  }

  // do common processing.
  if (SoundFiles[Sounds[sound_index].sample_index].used == 0) {
    mprintf((0, "Tryed to play %d sound, it DNE.\n", sound_index));
    return -1;
  }
#ifdef _DEBUG
  sound_slot = FindFreeSoundSlot(sound_index, f_volume, play_info->priority);
#else
  sound_slot = FindFreeSoundSlot(f_volume, play_info->priority);
#endif
  if (sound_slot < 0) {
    // do prioritization code here.
    return -1;
  }
  sb = &sound_cache[sound_slot];
  m_total_sounds_played++;
  sb->play_info = play_info;
  sb->m_unique_id = MakeUniqueId(sound_slot);
  sb->m_buffer_type = SBT_2D;
  sb->m_sound_index = sound_index;
  sb->m_status = SSF_UNUSED;

  ASSERT(sb->m_unique_id != -1);

  // play 2d sound
  sb->m_status = (f_looped) ? SSF_PLAY_LOOPING : SSF_PLAY_NORMAL;
  return sb->m_unique_id;
}

// This function limits the number of sounds cached to 255(8bits) and 256 bit is for invalid channel
// The purpose is to create unique signatures for each sound played (and allow for
// the slot_number to be quickly determined)
inline int lnxsound::MakeUniqueId(int sound_slot) { return ((((int)m_total_sounds_played) << 8) + sound_slot); }

inline int lnxsound::ValidateUniqueId(int sound_uid) {
  if (sound_uid == sound_cache[sound_uid & 0x00FF].m_unique_id) {
    return sound_uid & 0x00FF;
  } else {
    return -1;
  }
}

int lnxsound::PlayStream(play_information *play_info) {
  short sound_slot;
  int ds_flags = 0;

  ASSERT(play_info != NULL);

  float volume = (play_info->left_volume > play_info->right_volume) ? play_info->left_volume : play_info->right_volume;

  if (sound_device == -1)
    return -1;

#ifdef _DEBUG
  sound_slot = FindFreeSoundSlot(-1, volume, play_info->priority);
#else
  sound_slot = FindFreeSoundSlot(volume, play_info->priority);
#endif
  // Out of sound slots
  if (sound_slot < 0) {
    return -1;
  }

  m_total_sounds_played++;
  sound_cache[sound_slot].play_info = play_info;

  sound_cache[sound_slot].m_unique_id = MakeUniqueId(sound_slot);
  ASSERT(sound_cache[sound_slot].m_unique_id != -1);

  sound_cache[sound_slot].m_buffer_type = SBT_2D;
  sound_cache[sound_slot].m_status = SSF_PLAY_STREAMING;

  m_cur_sounds_played++;

  return (sound_cache[sound_slot].m_unique_id);
}

void lnxsound::SetListener(pos_state *cur_pos) {
  if (sound_device == -1)
    return;

  m_emulated_listener.orient = *cur_pos->orient;
  m_emulated_listener.position = *cur_pos->position;
  m_emulated_listener.velocity = *cur_pos->velocity;
}

int lnxsound::PlaySound3d(play_information *play_info, int sound_index, pos_state *cur_pos, float adjusted_volume,
                          bool f_looped, float reverb) //, unsigned short frequency
{
  short sound_slot;
  int ds_flags = 0;
  float volume;

  volume = adjusted_volume; // Adjust base volume by sent volume, let 3d stuff do the rest

  if (sound_device == -1)
    return -1;

  ASSERT(Sounds[sound_index].used != 0);
  if (Sounds[sound_index].used == 0)
    return -1;

  float dist;
  vector dir_to_sound = *cur_pos->position - m_emulated_listener.position;
  float pan;

  dist = vm_NormalizeVector(&dir_to_sound);
  if (dist < .1f) {
    dir_to_sound = m_emulated_listener.orient.fvec;
  }

  if (dist >= Sounds[sound_index].max_distance) {
    return -1;
  } else if (dist > Sounds[sound_index].min_distance) {
    volume *= (1.0 - ((dist - Sounds[sound_index].min_distance) /
                      (Sounds[sound_index].max_distance - Sounds[sound_index].min_distance)));
  }

  pan = (dir_to_sound * m_emulated_listener.orient.rvec);

  if (volume < 0.0f)
    volume = 0.0f;
  else if (volume > 1.0f)
    volume = 1.0f;

  if (pan < -1.0f)
    pan = -1.0f;
  else if (pan > 1.0f)
    pan = 1.0f;

  return PlaySound2d(play_info, sound_index, volume, pan, f_looped);
}

void lnxsound::AdjustSound(int sound_uid, float f_volume, float f_pan, unsigned short frequency) {
  int current_slot;

  if (sound_device == -1)
    return;

  if ((current_slot = ValidateUniqueId(sound_uid)) == -1)
    return;
  if (sound_cache[current_slot].m_status == SSF_UNUSED)
    return;

  sound_buffer_info *sb = &sound_cache[current_slot];
  play_information *play_info = sb->play_info;

  play_info->left_volume = play_info->right_volume = f_volume;
  if (f_pan < 0.0)
    play_info->right_volume += f_volume * f_pan;
  else
    play_info->left_volume -= f_volume * f_pan;
}

void lnxsound::AdjustSound(int sound_uid, pos_state *cur_pos, float adjusted_volume, float reverb) {
  if (sound_device == -1)
    return;

  int current_slot;

  if ((current_slot = ValidateUniqueId(sound_uid)) == -1)
    return;
  if (sound_cache[current_slot].m_status == SSF_UNUSED)
    return;

  // We need to determine the pan and volume
  float volume;

  volume = adjusted_volume;

  float dist;
  vector dir_to_sound = *cur_pos->position - m_emulated_listener.position;
  float pan;

  dist = vm_NormalizeVector(&dir_to_sound);
  if (dist < .1f) {
    dir_to_sound = m_emulated_listener.orient.fvec;
  }

  if (dist >= Sounds[sound_cache[current_slot].m_sound_index].max_distance) {
    volume = 0.0f;
  } else if (dist > Sounds[sound_cache[current_slot].m_sound_index].min_distance) {
    volume *= (1.0 - ((dist - Sounds[sound_cache[current_slot].m_sound_index].min_distance) /
                      (Sounds[sound_cache[current_slot].m_sound_index].max_distance -
                       Sounds[sound_cache[current_slot].m_sound_index].min_distance)));
  }

  pan = (dir_to_sound * m_emulated_listener.orient.rvec);

  if (volume < 0.0f)
    volume = 0.0f;
  else if (volume > 1.0f)
    volume = 1.0f;

  if (pan < -1.0f)
    pan = -1.0f;
  else if (pan > 1.0f)
    pan = 1.0f;

  AdjustSound(sound_cache[current_slot].m_unique_id, volume, pan, 22050);
}

void lnxsound::StopAllSounds(void) {
  int current_slot;

  for (current_slot = 0; current_slot < MAX_SOUNDS_PLAYING_AT_ONCE; current_slot++) {
    if (sound_cache[current_slot].m_status != SSF_UNUSED) {
      StopSound(sound_cache[current_slot].m_unique_id);
    }
  }
}

// Checks if a sound is playing (removes finished sound);
bool lnxsound::IsSoundInstancePlaying(int sound_uid) {
  int current_slot;

  if (sound_device == -1)
    return false;

  if ((current_slot = ValidateUniqueId(sound_uid)) == -1)
    return false;

  if (sound_cache[current_slot].m_status != SSF_UNUSED) {
    return true;
  }

  return false;
}

int lnxsound::IsSoundPlaying(int sound_index) {
  int current_slot;

  if (sound_device == -1)
    return -1;

  for (current_slot = 0; current_slot < MAX_SOUNDS_PLAYING_AT_ONCE; current_slot++) {
    if ((sound_cache[current_slot].m_status != SSF_UNUSED) &&
        (sound_cache[current_slot].m_sound_index == sound_index)) {
      return sound_cache[current_slot].m_unique_id;
    }
  }

  return -1;
}

// Stops 2d and 3d sounds
void lnxsound::StopSound(int sound_uid, unsigned char f_immediately = SKT_STOP_IMMEDIATELY) {
  int current_slot;
  sound_buffer_info *sb;

  if (sound_device == -1)
    return;

  if ((current_slot = ValidateUniqueId(sound_uid)) == -1)
    return;

  sb = &sound_cache[current_slot];

  if (sb->m_status == SSF_UNUSED)
    return;

  //	update sound count.
  m_cur_sounds_played--;

  if (f_immediately == SKT_STOP_AFTER_LOOP) {
    sb->m_status &= ~SSF_PLAY_LOOPING;
    sb->m_status |= SSF_PLAY_NORMAL;
    return;
  }

  sound_cache[current_slot].m_status = SSF_UNUSED;
}

// Pause all sounds/resume all sounds
void lnxsound::PauseSounds(void) {
  int current_slot;

  for (current_slot = 0; current_slot < MAX_SOUNDS_PLAYING_AT_ONCE; current_slot++) {
    sound_buffer_info *sb = &sound_cache[current_slot];
    if (sb->m_status != SSF_UNUSED && !(sb->m_status & SSF_PAUSED)) {
      sb->m_status |= SSF_PAUSED;
    }
  }
}

void lnxsound::ResumeSounds(void) {
  int current_slot;

  for (current_slot = 0; current_slot < MAX_SOUNDS_PLAYING_AT_ONCE; current_slot++) {
    sound_buffer_info *sb = &sound_cache[current_slot];

    if (sb->m_status != SSF_UNUSED && (sb->m_status & SSF_PAUSED)) {
      sound_cache[current_slot].m_status &= (~SSF_PAUSED);
    }
  }
}

void lnxsound::PauseSound(int sound_uid) {
  int current_slot;

  for (current_slot = 0; current_slot < MAX_SOUNDS_PLAYING_AT_ONCE; current_slot++) {
    sound_buffer_info *sb = &sound_cache[current_slot];
    if (sb->m_unique_id == sound_uid) {
      if (sb->m_status != SSF_UNUSED && !(sb->m_status & SSF_PAUSED)) {
        sb->m_status |= SSF_PAUSED;
      }
      break;
    }
  }
}

void lnxsound::ResumeSound(int sound_uid) {
  int current_slot;

  for (current_slot = 0; current_slot < MAX_SOUNDS_PLAYING_AT_ONCE; current_slot++) {
    if (sound_uid == sound_cache[current_slot].m_unique_id) {
      if (sound_cache[current_slot].m_status != SSF_UNUSED && (sound_cache[current_slot].m_status & SSF_PAUSED)) {
        sound_cache[current_slot].m_status &= (~SSF_PAUSED);
        break;
      }
    }
  }
}

bool lnxsound::CheckAndForceSoundDataAlloc(int sound_index) {
  int result;
  int sound_file_index = Sounds[sound_index].sample_index;

  //	ASSERT(sound_file_index >= 0 && sound_file_index < MAX_SOUND_FILES);

  if (sound_file_index < 0 || sound_file_index >= MAX_SOUND_FILES) {
    return false;
  }

  // Check if the sample data is already loaded
  if (SoundFiles[sound_file_index].sample_16bit != NULL || SoundFiles[sound_file_index].sample_8bit != NULL)
    return true;

  // If not, get the sound data
  result = SoundLoadWaveFile(SoundFiles[sound_file_index].name, Sounds[sound_index].import_volume, sound_file_index,
                             (m_sound_quality == SQT_HIGH), true);

  // Why would it load once (table load time) and not now?
  if (!result)
    return false;

  mprintf((0, "Sound %s loaded.\n", SoundFiles[sound_file_index].name));

  return true;
}

// Begin sound frame
void lnxsound::SoundStartFrame(void) {
  float frame_time;
  int current_slot;
  int i;

  if (m_timer_last_frametime == -1) {
    frame_time = 0.0f;
  } else {
    frame_time = (timer_GetMSTime() - m_timer_last_frametime) / 1000.0f;
  }
  m_timer_last_frametime = timer_GetMSTime();

  // perform necessary functions if sound events are pending for frame, this doesn't have to do anything
  // if the mixer doesn't require such actions.  Aureal does though.
  if (m_pending_actions) {
    mprintf((0, "pending actions\n"));
  }

  m_in_sound_frame = true;
  m_pending_actions = false;

  int counter = 0, loop_counter = 0, stream_counter = 0, buf_loop_counter = 0;

#ifdef _DEBUG
  int n_p5 = 0, n_p4 = 0, n_p3 = 0, n_p2 = 0, n_p1 = 0, n_p0 = 0;
#endif

  for (current_slot = 0; current_slot < MAX_SOUNDS_PLAYING_AT_ONCE; current_slot++) {
    sound_buffer_info *sb = &sound_cache[current_slot];
    if (sb->m_status != SSF_UNUSED) {
      counter++;
      if (sb->m_status & SSF_PLAY_LOOPING) {
        if (sb->m_status & SSF_BUFFERED_LOOP)
          buf_loop_counter++;
        loop_counter++;
      }
      if (sb->m_status & SSF_PLAY_STREAMING)
        stream_counter++;

#ifdef _DEBUG
      if (sb->play_info->priority == SND_PRIORITY_CRITICAL)
        n_p5++;
      else if (sb->play_info->priority == SND_PRIORITY_HIGHEST)
        n_p4++;
      else if (sb->play_info->priority == SND_PRIORITY_HIGH)
        n_p3++;
      else if (sb->play_info->priority == SND_PRIORITY_NORMAL)
        n_p2++;
      else if (sb->play_info->priority == SND_PRIORITY_LOW)
        n_p1++;
      else if (sb->play_info->priority == SND_PRIORITY_LOWEST)
        n_p0++;
#endif
    }
  }

  // update cache stress timer.
  if (counter < (MAX_SOUNDS_PLAYING_AT_ONCE * 3 / 4)) {
    m_cache_stress_timer += frame_time;
  } else {
    m_cache_stress_timer = 0.0f;
  }

#ifdef _DEBUG
  mprintf_at((3, 2, 0, "LNS: %02d/%02d", counter, MAX_SOUNDS_PLAYING_AT_ONCE));
  mprintf_at((3, 3, 1, "Lp: %02d", loop_counter));
  mprintf_at((3, 4, 1, "St: %02d", stream_counter));
  mprintf_at((3, 5, 0, " Ot: %02d", counter - loop_counter - stream_counter));

  mprintf_at((3, 2, 20, "P5:%02d P4:%02d P3:%02d", n_p5, n_p4, n_p3));
  mprintf_at((3, 3, 20, "P2:%02d P1:%02d P0:%02d", n_p2, n_p1, n_p0));
#endif
}

// End sound frame
void lnxsound::SoundEndFrame(void) {
  CheckForErrors(); // handles errors.
  m_in_sound_frame = false;
}

// Sound System Error Handler.
void lnxsound::CheckForErrors() {
  // if a fatal error occurred, quit and display an error
  // non fatal errors should be put inside a logfile, or just mprinted out.
  switch (m_lib_error_code) {
  case SSL_ERROR_SAMPLE_NODATA:
    Error("%s\nSample had no data.", m_error_text);
    break;

  case SSL_ERROR_STREAMMIXER:
    Error("%s\nMixer alignment check failed.", m_error_text);
    break;

  case SSL_ERROR_GENERIC:
    Error("%s\nGeneric error.", m_error_text);
    break;
  }

  // must call!
  llsSystem::CheckForErrors();
}

// returns the error string.
char *lnxsound::GetErrorStr() const {
  static char buffer[] = "No Error Given";
  return buffer;
}

bool lnxsound::SetGlobalReverbProperties(float volume, float damping, float decay) { return false; }

void lnxsound::StartStreaming(void) {
  /*
          // Load the thread library
          if(!pthread_lib)
          {
                  // load the library
                  pthread_lib = dlopen("libpthread.so",RTLD_NOW|RTLD_GLOBAL);
                  if(!pthread_lib)
                  {
                          Error("Unable to load libpthread.so\n");
                          exit(1);
                  }

                  dpthread_create = (pthread_create_fp)dlsym(pthread_lib,"pthread_create");
                  if(!dpthread_create)
                  {
                          dlclose(pthread_lib);
                          pthread_lib = NULL;
                          Error("Unable to find symbol pthread_create in libpthread.so\n");
                          exit(1);
                  }

                  dpthread_exit = (pthread_exit_fp)dlsym(pthread_lib,"pthread_exit");
                  if(!dpthread_exit)
                  {
                          dlclose(pthread_lib);
                          pthread_lib = NULL;
                          Error("Unable to find symbol pthread_exit in libpthread.so\n");
                          exit(1);
                  }

                  dpthread_detach = (pthread_detach_fp)dlsym(pthread_lib,"pthread_detach");
                  if(!dpthread_detach)
                  {
                          dlclose(pthread_lib);
                          pthread_lib = NULL;
                          Error("Unable to find symbol pthread_detach in libpthread.so\n");
                          exit(1);
                  }
                  dpthread_self = (pthread_self_fp)dlsym(pthread_lib,"pthread_self");
                  if(!dpthread_self)
                  {
                          dlclose(pthread_lib);
                          pthread_lib = NULL;
                          Error("Unable to find symbol pthread_detach in libpthread.so\n");
                          exit(1);
                  }
          }
  */
  m_sb_info.thread_request_kill = false;
  m_sb_info.thread_alive = false;
  m_sb_info.thread_waiting_for_death = false;
  m_sb_info.sound_device = &sound_device;
  m_sb_info.p_error_code = &m_lib_error_code;
  m_sb_info.fp_SetError = lnxsound_SetError;
  m_sb_info.fp_ErrorText = lnxsound_ErrorText;

  // Start mixing thread.

  //	m_sb_info.thread_handle = dpthread_create(&m_sb_info.thread_id, NULL, StreamTimer,&m_sb_info);
  m_sb_info.thread_id = SDL_CreateThread(StreamTimer, &m_sb_info);
  if (m_sb_info.thread_id == NULL)
    m_sb_info.thread_handle = ((m_sb_info.thread_id == NULL) ? -1 : 0);

  if (m_sb_info.thread_handle != 0) {
    mprintf((0, "Thread failed\n"));
    Int3();
    return;
  }
}

void lnxsound::EndStreaming(void) {
  m_sb_info.thread_request_kill = true;

  int count = 150;
  mprintf((0, "Waiting for sound thread..."));
  while (count > 0 && !m_sb_info.thread_waiting_for_death) {
    count--;
    Sleep(50);
    mprintf((0, "."));
  }
  mprintf((0, "Done %s\n", (count <= 0) ? "EARLY" : "All Good"));

  /*
          if(pthread_lib)
          {
                  if(!in_at_exit)
                  {
                          //causes a segfault..BLAH!
                          //dlclose(pthread_lib);
                  }
                  pthread_lib = NULL;
                  dpthread_create = NULL;
                  dpthread_exit = NULL;
                  dpthread_self = NULL;
                  dpthread_detach = NULL;
          }
  */
}

// may be called before init (must be to be valid, the card passed here will be initialized in InitSoundLib)
void lnxsound::SetSoundCard(const char *name) {}

// set special parameters for the 3d environment.
// of strcuture passed, you must set the appropriate 'flags' value for values you wish to modify
void lnxsound::SetEnvironmentValues(const t3dEnvironmentValues *env) {}

// get special parameters for the 3d environment.
// of strcuture passed, you must set the appropriate 'flags' value for values you wish to modify
void lnxsound::GetEnvironmentValues(t3dEnvironmentValues *env) {}

// enable special parameters for the 3d environment.
// of strcuture passed, you must set the appropriate 'flags' value for values you wish to modify
void lnxsound::SetEnvironmentToggles(const t3dEnvironmentToggles *env) {}

// get states of special parameters for the 3d environment.
// of strcuture passed, you must set the appropriate 'flags' value for values you wish to modify
void lnxsound::GetEnvironmentToggles(t3dEnvironmentToggles *env) {}

void lnxsound_SetError(int code) { ll_sound_ptr->SetError(code); }

void lnxsound_ErrorText(char *fmt, ...) {
  char buffer[384];
  va_list args;
  va_start(args, fmt);
  vsprintf(buffer, fmt, args);
  va_end(args);

  ll_sound_ptr->ErrorText(buffer);
}

// A peroidic mixer that uses the primary buffer as a stream buffer
int StreamTimer(void *user_ptr) {
  LNXSTREAM *sb_info = (LNXSTREAM *)user_ptr;

  /*
          int status;
          status = dpthread_detach(dpthread_self());
          if(status!=0)
          {
                  Error("Detach thread error %d\n",status);
                  exit(1);
          }
  */

  sb_info->thread_alive = true;

  int blocksize;
  ioctl(*sb_info->sound_device, SNDCTL_DSP_GETBLKSIZE, &blocksize);

  mprintf((0, "Sound: Current block size is (%d).", blocksize));

  /*
          if(blocksize != 2048)
          {
                  Error("Sound: Blocksize != 2048 bytes.\n");
                  exit(1);
          } // if
  */

  software_mixer mixer;
  lnxsound_buffer primary(blocksize, sb_info->sound_device);

  // setup mixer
  tMixerInit mi;
  mi.primary_buffer = &primary;
  mi.primary_frequency = SOUNDLIB_SAMPLE_RATE;
  mi.max_sounds_available = &sound_buffer_size;
  mi.sound_cache = sound_cache;
  mi.primary_alignment = SOUNDLIB_CHANNELS * (SOUNDLIB_SAMPLE_SIZE >> 3);
  mi.fp_SetError = sb_info->fp_SetError;
  mi.fp_ErrorText = sb_info->fp_ErrorText;
  mi.p_error_code = sb_info->p_error_code;
  mi.ll_sound_ptr = ll_sound_ptr;

  if (!mixer.Initialize(&mi)) {
    //		return NULL;
    return (0);
  } // if

  int sleep_time_i;
  double sleep_time;
  sleep_time = ((double)blocksize) / (4.0f * 22050.0f); // in seconds
  sleep_time_i = (int)(sleep_time * 1000000.0f);

  fd_set fdset;
  fd_set scratchset;
  struct timeval zerowait = {0, 0};
  int rc, mytime = 0;

  FD_ZERO(&fdset);
  FD_SET(0, &fdset);

  while (!sb_info->thread_request_kill) {
    mytime = 0;
    do {
      scratchset = fdset;
      rc = select(FD_SETSIZE, &scratchset, 0, 0, &zerowait);

      if (rc > 0) {
        mytime++;
      } else if (rc < 0) {
      }
    } while (rc > 0);
    mixer.DoFrame();
  }

  mprintf((0, "Sound: Exit sound thread"));
  sb_info->thread_alive = false;
  sb_info->thread_waiting_for_death = true;

  return 0;
}

lnxsound_buffer::lnxsound_buffer(int buffer_size, int *sound_device) {
  m_sound_device = sound_device;
  m_buffer_size = buffer_size;
}

lnxsound_buffer::~lnxsound_buffer() {}

void lnxsound_buffer::Write(unsigned char *buffer, int amount) {
  if (m_sound_device <= 0)
    return; // only primary buffers supported

  ioctl(*m_sound_device, SNDCTL_DSP_POST, 0);

  // slam the entire buffer
  int res = write(*m_sound_device, buffer, amount);
  if (res == -1) {
    mprintf((0, "ERROR WRITING SOUND BUFFER Device %d amount %d bytes\n", *m_sound_device, amount));
    return;
  } else if (res != amount) {
    mprintf((0, "ERROR: SOUND BUFFER NOT COMPLETELY WRITTEN %d\n", res));
  }
}

int lnxsound_buffer::GetNumBufferBytes(void) { return m_buffer_size; }

///////////////////////////////////////////////////////////////////////
// llsGeometry
#include "ddsndgeometry.h"

// specify a sound library to associate geometry with
bool llsGeometry::Init(llsSystem *snd_sys) { return false; }

// closes low level geometry system.
void llsGeometry::Shutdown() {}

void llsGeometry::StartFrame() {}
void llsGeometry::EndFrame() {}

// clears out geometry info
void llsGeometry::Clear() {}

// polygon lists
// is a group cached?, check before rendering it.
void llsGeometry::IsGroupValid(int group) {}

// marks beginning of a list of polygons to render, (-1 group for non cache)
void llsGeometry::StartPolygonGroup(int group) {}

// ends a list of polygons to render.
void llsGeometry::EndPolygonGroup(int group) {}

// renders a group.
void llsGeometry::RenderGroup(int group) {}

// primatives, nv = number of verts, and verts is an array of pointers to vertices.
// you can pass a sound material value if you want special reflective properties on this polygon.
void llsGeometry::AddPoly(int nv, vector **verts, unsigned tag, tSoundMaterial material) {}

// 4 verts here.
void llsGeometry::AddQuad(unsigned tag, vector **verts) {}

// 3 verts here.
void llsGeometry::AddTriangle(unsigned tag, vector **verts) {}

void llsGeometry::CreateMaterial(tSoundMaterial material, float transmit_gain, float transmit_highfreq,
                                 float reflect_gain, float reflect_highfreq) {}

void llsGeometry::DestroyMaterial(tSoundMaterial material) {}
