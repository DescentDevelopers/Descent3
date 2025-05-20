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

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#include <SDL3/SDL.h>

#include "pserror.h"
#include "mem.h"
#include "log.h"
#include "ssl_lib.h"
#include "application.h"
#include "sdlsound.h"
#include "mixer.h"
#include "ddio.h"
#include "args.h"

#define SOUNDLIB_SAMPLE_RATE 22050
#define SOUNDLIB_SAMPLE_SIZE 16
#define SOUNDLIB_CHANNELS 2
#define SOUNDLIB_DEFAULT_SAMPLES 1024

// ===============================
#define MAX_SOUNDS_PLAYING_AT_ONCE 256
static sound_buffer_info sound_cache[MAX_SOUNDS_PLAYING_AT_ONCE];
static int sound_buffer_size = MAX_SOUNDS_PLAYING_AT_ONCE;

lnxsound *ll_sound_ptr;

// A peroidic mixer that uses the primary buffer as a stream buffer
void StreamAudio(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount);

lnxsound::lnxsound() : llsSystem() {
  ll_sound_ptr = this;
  sound_device = 0;
  in_at_exit = false;
}

lnxsound::~lnxsound() {
  in_at_exit = true;
  DestroySoundLib();
  SetSoundCard(nullptr);
}

// Starts the sound library, maybe have it send back some information -- 3d support?
int lnxsound::InitSoundLib(char mixer_type, oeApplication *sos, uint8_t max_sounds_played) {
  SDL_AudioSpec spec;

  // setup mixer
  tMixerInit mi;
  mi.primary_buffer = nullptr;
  mi.primary_frequency = SOUNDLIB_SAMPLE_RATE;
  mi.max_sounds_available = &sound_buffer_size;
  mi.sound_cache = sound_cache;
  mi.primary_alignment = SOUNDLIB_CHANNELS * (SOUNDLIB_SAMPLE_SIZE >> 3);
  mi.fp_SetError = lnxsound_SetError;
  mi.fp_ErrorText = lnxsound_ErrorText;
  mi.p_error_code = &m_lib_error_code;
  mi.ll_sound_ptr = ll_sound_ptr;

  if (!m_mixer.Initialize(&mi)) {
    return false;
  } // if

  int sampleCount = SOUNDLIB_DEFAULT_SAMPLES;
  int sampleArgIndex = FindArg("-sdlSndSize");
  if (sampleArgIndex != 0) {
    const char *sampleCountStr = GetArg(sampleArgIndex + 1);
    if (sampleCountStr) {
      sampleCount = atoi(sampleCountStr);
      if (sampleCount <= 0) {
        sampleCount = SOUNDLIB_DEFAULT_SAMPLES;
      }
    }
  }

  spec.freq = SOUNDLIB_SAMPLE_RATE;
  spec.format = SOUNDLIB_SAMPLE_SIZE == 8 ? SDL_AUDIO_U8 : SDL_AUDIO_S16;
  spec.channels = SOUNDLIB_CHANNELS;

  this->sound_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, StreamAudio, &m_mixer); 
  this->sound_device = SDL_GetAudioStreamDevice(sound_stream);
  if (sound_device == 0) {
    strcpy(m_error_text, SDL_GetError());
    return false;
  }

  LOG_INFO << "Sound: Hardware configured. Kicking off stream thread...";
  SDL_ResumeAudioDevice(sound_device);

  m_total_sounds_played = 0;
  m_cur_sounds_played = 0;
  m_in_sound_frame = false;
  m_pending_actions = false;
  m_cache_stress_timer = 0.0f;
  m_timer_last_frametime = -1;
  m_sound_quality = SQT_HIGH;

  return true;
}

bool lnxsound::GetDeviceSettings(SDL_AudioDeviceID *device, uint32_t *freq, uint32_t *bit_depth,
                                 uint32_t *channels) const {
  if (sound_device == 0)
    return false;

  *device = sound_device;
  *freq = SOUNDLIB_SAMPLE_RATE;
  *bit_depth = SOUNDLIB_SAMPLE_SIZE;
  *channels = SOUNDLIB_CHANNELS;

  return true;
}

// Cleans up after the Sound Library
void lnxsound::DestroySoundLib() {
  if (sound_device) {
    SDL_CloseAudioDevice(sound_device);
    sound_device = 0;
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
        mem_free(SoundFiles[j].sample_8bit);
        SoundFiles[j].sample_8bit = nullptr;

        CheckAndForceSoundDataAlloc(i);
      }
      if (SoundFiles[j].sample_16bit && m_sound_quality == SQT_NORMAL) {
        int count;

        ASSERT(SoundFiles[j].sample_8bit == nullptr);
        SoundFiles[j].sample_8bit = mem_rmalloc<uint8_t>(SoundFiles[j].sample_length);

        // NOTE:  Interesting note on sound conversion:  16 bit sounds are signed (0 biase).  8 bit sounds are unsigned
        // (+128 biase).
        for (count = 0; count < (int)SoundFiles[j].sample_length; count++) {
          SoundFiles[j].sample_8bit[count] = (uint8_t)((((int)SoundFiles[j].sample_16bit[count]) + 32767) >> 8);
        }

        mem_free(SoundFiles[j].sample_16bit);
        SoundFiles[j].sample_16bit = nullptr;
      }
    }
  }

  ResumeSounds();

  return true;
}

char lnxsound::GetSoundQuality() { return m_sound_quality; }

bool lnxsound::SetSoundMixer(char mixer_type) { return true; }

char lnxsound::GetSoundMixer() { return SOUND_MIXER_SOFTWARE_16; }

// Determines if a sound will play.  Takes into account maximum allowable
// sounds.
// Also put prioritization code in here
//		ignore reserved slots
#ifdef _DEBUG
int16_t lnxsound::FindFreeSoundSlot(int sound_index, float volume, int priority)
#else
int16_t lnxsound::FindFreeSoundSlot(float volume, int priority)
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
      LOG_DEBUG.printf("DDSNDLIB: Replace sound (p:%d) with sound (p:%d) in slot %d",
                       sb->play_info->priority, priority, throw_out_slot);
      return throw_out_slot;
    }
  }

#ifdef _DEBUG
  if (sound_index > -1) {
    LOG_DEBUG.printf("DDSNDLIB: Sound %s with priority (%d) too low.", Sounds[sound_index].name, priority);
  } else {
    LOG_DEBUG.printf("DDSNDLIB: Sound unknown with priority (%d) too low.", priority);
  }
#endif
  return -1;
}

// Plays a 2d sound
int lnxsound::PlaySound2d(play_information *play_info, int sound_index, float f_volume, float f_pan, bool f_looped) {
  sound_buffer_info *sb;
  int16_t sound_slot;

  if (sound_device == 0) {
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
    LOG_DEBUG.printf("Tried to play %d sound, it DNE.", sound_index);
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
inline int lnxsound::MakeUniqueId(int sound_slot) const { return ((((int)m_total_sounds_played) << 8) + sound_slot); }

inline int lnxsound::ValidateUniqueId(int sound_uid) {
  if (sound_uid == sound_cache[sound_uid & 0x00FF].m_unique_id) {
    return sound_uid & 0x00FF;
  } else {
    return -1;
  }
}

int lnxsound::PlayStream(play_information *play_info) {
  int16_t sound_slot;

  ASSERT(play_info != nullptr);

  float volume = std::max(play_info->left_volume, play_info->right_volume);

  if (sound_device == 0)
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
  if (sound_device == 0)
    return;

  m_emulated_listener.orient = *cur_pos->orient;
  m_emulated_listener.position = *cur_pos->position;
  m_emulated_listener.velocity = *cur_pos->velocity;
}

int lnxsound::PlaySound3d(play_information *play_info, int sound_index, pos_state *cur_pos, float adjusted_volume,
                          bool f_looped, float reverb) //, uint16_t frequency
{
  float volume = adjusted_volume; // Adjust base volume by sent volume, let 3d stuff do the rest

  if (sound_device == 0)
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
    volume *= (1.0f - ((dist - Sounds[sound_index].min_distance) /
                       (Sounds[sound_index].max_distance - Sounds[sound_index].min_distance)));
  }

  pan = (vm_Dot3Product(dir_to_sound,m_emulated_listener.orient.rvec));

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

void lnxsound::AdjustSound(int sound_uid, float f_volume, float f_pan, uint16_t frequency) {
  int current_slot;

  if (sound_device == 0)
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
  if (sound_device == 0)
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
    volume *= (1.0f - ((dist - Sounds[sound_cache[current_slot].m_sound_index].min_distance) /
                       (Sounds[sound_cache[current_slot].m_sound_index].max_distance -
                        Sounds[sound_cache[current_slot].m_sound_index].min_distance)));
  }

  pan = (vm_Dot3Product(dir_to_sound,m_emulated_listener.orient.rvec));

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

void lnxsound::StopAllSounds() {
  for (auto &current_slot : sound_cache) {
    if (current_slot.m_status != SSF_UNUSED) {
      StopSound(current_slot.m_unique_id);
    }
  }
}

// Checks if a sound is playing (removes finished sound);
bool lnxsound::IsSoundInstancePlaying(int sound_uid) {
  int current_slot;

  if (sound_device == 0)
    return false;

  if ((current_slot = ValidateUniqueId(sound_uid)) == -1)
    return false;

  if (sound_cache[current_slot].m_status != SSF_UNUSED) {
    return true;
  }

  return false;
}

int lnxsound::IsSoundPlaying(int sound_index) {
  if (sound_device == 0)
    return -1;

  for (auto &current_slot : sound_cache) {
    if ((current_slot.m_status != SSF_UNUSED) && (current_slot.m_sound_index == sound_index)) {
      return current_slot.m_unique_id;
    }
  }

  return -1;
}

// Stops 2d and 3d sounds
void lnxsound::StopSound(int sound_uid, uint8_t f_immediately) {
  int current_slot;
  sound_buffer_info *sb;

  if (sound_device == 0)
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
void lnxsound::PauseSounds() {
  for (auto &current_slot : sound_cache) {
    sound_buffer_info *sb = &current_slot;
    if (sb->m_status != SSF_UNUSED && !(sb->m_status & SSF_PAUSED)) {
      sb->m_status |= SSF_PAUSED;
    }
  }
}

void lnxsound::ResumeSounds() {
  for (auto &current_slot : sound_cache) {
    sound_buffer_info *sb = &current_slot;

    if (sb->m_status != SSF_UNUSED && (sb->m_status & SSF_PAUSED)) {
      current_slot.m_status &= (~SSF_PAUSED);
    }
  }
}

void lnxsound::PauseSound(int sound_uid) {
  for (auto &current_slot : sound_cache) {
    sound_buffer_info *sb = &current_slot;
    if (sb->m_unique_id == sound_uid) {
      if (sb->m_status != SSF_UNUSED && !(sb->m_status & SSF_PAUSED)) {
        sb->m_status |= SSF_PAUSED;
      }
      break;
    }
  }
}

void lnxsound::ResumeSound(int sound_uid) {
  for (auto &current_slot : sound_cache) {
    if (sound_uid == current_slot.m_unique_id) {
      if (current_slot.m_status != SSF_UNUSED && (current_slot.m_status & SSF_PAUSED)) {
        current_slot.m_status &= (~SSF_PAUSED);
        break;
      }
    }
  }
}

bool lnxsound::CheckAndForceSoundDataAlloc(int sound_index) {
  int sound_file_index = Sounds[sound_index].sample_index;

  //	ASSERT(sound_file_index >= 0 && sound_file_index < MAX_SOUND_FILES);

  if (sound_file_index < 0 || sound_file_index >= MAX_SOUND_FILES) {
    return false;
  }

  // Check if the sample data is already loaded
  if (SoundFiles[sound_file_index].sample_16bit != nullptr || SoundFiles[sound_file_index].sample_8bit != nullptr)
    return true;

  // If not, get the sound data
  int result = SoundLoadWaveFile(SoundFiles[sound_file_index].name, Sounds[sound_index].import_volume, sound_file_index,
                                 (m_sound_quality == SQT_HIGH), true);

  // Why would it load once (table load time) and not now?
  if (!result)
    return false;

  LOG_DEBUG.printf("Sound %s loaded.", SoundFiles[sound_file_index].name);

  return true;
}

// Begin sound frame
void lnxsound::SoundStartFrame() {
  float frame_time;

  if (m_timer_last_frametime == -1) {
    frame_time = 0.0f;
  } else {
    frame_time = (timer_GetMSTime() - m_timer_last_frametime) / 1000.0f;
  }
  m_timer_last_frametime = timer_GetMSTime();

  // perform necessary functions if sound events are pending for frame, this doesn't have to do anything
  // if the mixer doesn't require such actions.  Aureal does though.
  if (m_pending_actions) {
    LOG_DEBUG << "pending actions";
  }

  m_in_sound_frame = true;
  m_pending_actions = false;

  int counter = 0;
  for (auto &current_slot : sound_cache) {
    sound_buffer_info *sb = &current_slot;
    if (sb->m_status != SSF_UNUSED) {
      counter++;
    }
  }

  // update cache stress timer.
  if (counter < (MAX_SOUNDS_PLAYING_AT_ONCE * 3 / 4)) {
    m_cache_stress_timer += frame_time;
  } else {
    m_cache_stress_timer = 0.0f;
  }
}

// End sound frame
void lnxsound::SoundEndFrame() {
  CheckForErrors(); // handles errors.
  m_in_sound_frame = false;
}

// Sound System Error Handler.
void lnxsound::CheckForErrors() {
  // if a fatal error occurred, quit and display an error
  // non-fatal errors should be put inside a logfile, or just mprinted out.
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
const char *lnxsound::GetErrorStr() const {
  static char buffer[] = "No Error Given";
  return buffer;
}

bool lnxsound::SetGlobalReverbProperties(float volume, float damping, float decay) { return false; }

void lnxsound::StartStreaming() {}

void lnxsound::EndStreaming() {}

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

void lnxsound_ErrorText(const char *fmt, ...) {
  char buffer[384];
  va_list args;
  va_start(args, fmt);
  std::vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);

  ll_sound_ptr->ErrorText(buffer);
}

// A peroidic mixer that uses the primary buffer as a stream buffer
void StreamAudio(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount)
{
    if (additional_amount > 0) {
        Uint8 *data = SDL_stack_alloc(Uint8, additional_amount);
        if (data) {
            // StreamAudio(userdata, data, additional_amount);
            ((software_mixer *)userdata)->StreamMixer((char *)data, additional_amount);
            SDL_PutAudioStreamData(stream, data, additional_amount);
            SDL_stack_free(data);
        }
    }
}
