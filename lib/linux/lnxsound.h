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

#ifndef __LINUX_DD_SOUND_H_
#define __LINUX_DD_SOUND_H_

#include <pthread.h>
#include "ssl_lib.h"
#include "mixer.h"

class sound_buffer_info;

void lnxsound_SetError(int code);
void lnxsound_ErrorText(char *fmt, ...);
inline void sb_adjust_properties_2d(sound_buffer_info *sb, float f_volume, float f_pan, ushort frequency);

class emulated_listener {
public:
  matrix orient;
  vector position;
  vector velocity;
};

class lnxsound : public llsSystem {
  // Public functions
public:
  lnxsound();
  ~lnxsound();

  // may be called before init (must be to be valid, the card passed here will be initialized in InitSoundLib)
  virtual void SetSoundCard(const char *name);
  virtual void PauseSound(int sound_uid);
  virtual void ResumeSound(int sound_uid);
  // set special parameters for the 3d environment.
  // of strcuture passed, you must set the appropriate 'flags' value for values you wish to modify
  virtual void SetEnvironmentValues(const t3dEnvironmentValues *env);

  // get special parameters for the 3d environment.
  // of strcuture passed, you must set the appropriate 'flags' value for values you wish to modify
  virtual void GetEnvironmentValues(t3dEnvironmentValues *env);

  // enable special parameters for the 3d environment.
  // of strcuture passed, you must set the appropriate 'flags' value for values you wish to modify
  virtual void SetEnvironmentToggles(const t3dEnvironmentToggles *env);

  // get states of special parameters for the 3d environment.
  // of strcuture passed, you must set the appropriate 'flags' value for values you wish to modify
  virtual void GetEnvironmentToggles(t3dEnvironmentToggles *env);

  // Starts the sound library, maybe have it send back some information -- 3d support?
  virtual int InitSoundLib(char mixer_type, oeApplication *sos, unsigned char max_sounds_played);
  // Cleans up after the Sound Library
  virtual void DestroySoundLib(void);

  // Locks and unlocks sounds (used when changing play_info data)
  virtual bool LockSound(int sound_uid);
  virtual bool UnlockSound(int sound_uid);

  virtual bool SetSoundQuality(char quality);
  virtual char GetSoundQuality(void);
  virtual bool SetSoundMixer(char mixer_type);
  virtual char GetSoundMixer(void);

  // Plays a 2d sound
  virtual int PlaySound2d(play_information *play_info, int sound_index, float volume, float pan, bool f_looped);
  virtual int PlayStream(play_information *play_info);

  virtual void SetListener(pos_state *cur_pos);
  virtual int PlaySound3d(play_information *play_info, int sound_index, pos_state *cur_pos, float master_volume,
                          bool f_looped, float reverb = 0.5f); //, unsigned short frequency
  virtual void AdjustSound(int sound_uid, float f_volume, float f_pan, unsigned short frequency);
  virtual void AdjustSound(int sound_uid, pos_state *cur_pos, float adjusted_volume, float reverb = 0.5f);

  virtual void StopAllSounds(void);

  // Checks if a sound is playing (removes finished sound);
  virtual bool IsSoundInstancePlaying(int sound_uid);
  virtual int IsSoundPlaying(int sound_index);

  //	virtual void AdjustSound(int sound_uid, play_information *play_info) = 0;

  // Stops 2d and 3d sounds
  virtual void StopSound(int sound_uid, unsigned char f_immediately = SKT_STOP_IMMEDIATELY);

  // Pause all sounds/resume all sounds
  virtual void PauseSounds(void);
  virtual void ResumeSounds(void);
  virtual bool CheckAndForceSoundDataAlloc(int sound_file_index);

  // Begin sound frame
  virtual void SoundStartFrame(void);

  // End sound frame
  virtual void SoundEndFrame(void);

  // returns the error string.
  virtual char *GetErrorStr() const;

  // Returns current error code
  int GetLastError() {
    int code = m_lib_error_code;
    m_lib_error_code = 0;
    return code;
  };

  // environmental sound interface
  // volume modifier (0-1), damping(0-1), 1 = complete, 0 = none
  //	decay 0.1 to 100 seconds, how long it takes for a sound to die.
  virtual bool SetGlobalReverbProperties(float volume, float damping, float decay);

  virtual bool GetDeviceSettings(int *sound_device, unsigned int *freq, unsigned int *bit_depth,
                                 unsigned int *channels);

  friend void lnxsound_SetError(int code);
  friend void lnxsound_ErrorText(char *fmt, ...);
  friend inline void sb_adjust_properties_2d(sound_buffer_info *sb, float f_volume, float f_pan, ushort frequency);

protected:
#ifdef _DEBUG
  short FindFreeSoundSlot(int sound_index, float volume, int priority);
#else
  short FindFreeSoundSlot(float volume, int priority);
#endif
  // This function limits the number of sounds cached to 255(8bits) and 256 bit is for invalid channel
  // The purpose is to create unique signatures for each sound played (and allow for
  // the slot_number to be quickly determined)
  inline int MakeUniqueId(int sound_slot);
  inline int ValidateUniqueId(int sound_uid);

  // Sound System Error Handler.
  void CheckForErrors(void);

  void SetError(int code) { m_lib_error_code = code; };
  void StartStreaming(void);
  void EndStreaming(void);

  int m_primary_frequency; // Set to the primary buffers frequency -- cmphack
  int m_primary_alignment;

  int sound_device;
  bool in_at_exit;
  bool m_in_sound_frame;
  bool m_pending_actions;
  char m_sound_quality;
  float m_cache_stress_timer;
  float m_timer_last_frametime;

  int m_total_sounds_played;
  int m_cur_sounds_played;

  emulated_listener m_emulated_listener;

  // Sam 6/29 - Added support for SDL
  software_mixer m_mixer;
};

#endif
