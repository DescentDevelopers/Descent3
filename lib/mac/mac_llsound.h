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
 * $Logfile: /DescentIII/Main/lib/mac/mac_llsound.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:58:13 $
 * $Author: kevinb $
 *
 * Mac implementation of low level sound library
 *
 * $Log: mac_llsound.h,v $
 * Revision 1.1.1.1  2003/08/26 03:58:13  kevinb
 * initial 1.5 import
 *
 *
 * 5     4/12/00 6:56p Matt
 * From Duane for 1.4
 *
 * 4     7/28/99 5:02p Kevin
 * Mac!
 *
 * 3     5/17/99 1:51p Ardussi
 * changed to compile on Mac
 *
 * 2     5/13/99 5:05p Ardussi
 * changes for compiling on the Mac
 *
 * 3     5/21/97 6:54 PM Jeremy
 * changes to comply with chris's changes to the low level sound library
 * api
 *
 * 2     3/21/97 7:03 PM Jeremy
 * added declaration of low level mac sound interface virtual functions.
 *
 * 1     3/14/97 5:27 PM Jeremy
 * first checkin
 *
 * $NoKeywords: $
 */

#ifndef MAC_LL_SOUND_SYSTEM_H
#define MAC_LL_SOUND_SYSTEM_H

#include <Sound.h>
#include <SoundSprocket.h>
#include <SoundComponents.h>

#include "hlsoundlib.h"
#include "application.h"
#include "ssl_lib.h"

#define DIST_SCALE 8

class sound_channel_info {
  SndChannelPtr snd_channel;
  SSpLocalizationData snd_localization;

  play_information *play_info;

  int m_sound_uid; // Index of sound
  int m_unique_id; // Unique id for the currently playing sample

  short m_buffer_type; // Buffer type 2d or 3d

  char *sample_data;
  int sample_length; // used for storage purposes.

  bool stereo;
  unsigned char m_status; // Sound status
  float m_volume;         // kept for priority.
};
class emulated_listener {
public:
  matrix orient;
  vector position;
  vector velocity;
};

// extern float ll_volume;
// void SetLLVolume(float volume);
// float GetLLVolume(void);

extern void SetLLMasterVolume(float volume);

class mac_llsSystem : public llsSystem {

  // Public functions
public:
  virtual void SetMasterVolume(float volume);
  virtual float GetMasterVolume(void);

  // may be called before init (must be to be valid, the card passed here will be initialized in InitSoundLib)
  virtual void SetSoundCard(const char *name);

  // Starts the sound library, maybe have it send back some information -- 3d support?
  virtual int InitSoundLib(char mixer_type, oeApplication *sos, unsigned char max_sounds_played);
  //	virtual int InitSoundLib(oeApplication *sos, char mixer_type, char quality, bool f_kill_sound_list);
  // Cleans up after the Sound Library
  virtual void DestroySoundLib(void);

  virtual void NewSoundList(unsigned short num_sounds, unsigned int *sound_offset_array, char *sound_data,
                            unsigned int sound_data_size);

  // Locks and unlocks sounds (used when changing play_info data)
  virtual bool LockSound(int sound_uid);
  virtual bool UnlockSound(int sound_uid);

  virtual bool SetSoundQuality(char quality);
  virtual char GetSoundQuality(void);
  virtual bool SetSoundMixer(char mixer_type);
  virtual char GetSoundMixer(void);

  // Plays a 2d sound
  virtual int PlaySound2d(play_information *play_info, int sound_uid, float volume, float pan, bool f_looped);
  virtual int PlayStream(play_information *play_info);

  virtual void SetListener(pos_state *cur_pos);
  virtual int PlaySound3d(play_information *play_info, int sound_uid, pos_state *cur_pos, float master_volume,
                          bool f_looped, float reverb = 0.5f); //, unsigned short frequency)
  virtual void AdjustSound(int sound_uid, float f_volume, float f_pan, unsigned short frequency);
  virtual void AdjustSound(int sound_uid, pos_state *cur_pos, float adjusted_volume, float reverb);

  virtual void StopAllSounds(void);

  // Checks if a sound is playing (removes finished sound);
  virtual bool IsSoundInstancePlaying(int sound_uid);
  virtual int IsSoundPlaying(int sound_uid);

  // Stops 2d and 3d sounds
  virtual void StopSound(int sound_uid, unsigned char f_immediately = SKT_STOP_IMMEDIATELY);

  // Pause all sounds/resume all sounds
  virtual void PauseSounds(void);
  virtual void ResumeSounds(void);
  virtual void PauseSound(int sound_uid);
  virtual void ResumeSound(int sound_uid);

  virtual bool CheckAndForceSoundDataAlloc(int sound_file_index);

  // Begin sound frame
  virtual void SoundStartFrame(void);

  // End sound frame
  virtual void SoundEndFrame(void);

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

  virtual ubyte GetNumChannels(void);
  virtual void SetNumChannels(ubyte num_chan);

  virtual int FindFreeChannel(float volume, int priority);
  virtual void PlaySound(int file_index, int channel, bool f_looped);
  virtual void SetChannelVolume(int channel, float volume);

  inline int MakeUniqueId(int channel);

protected:
  typedef struct mac_sound_struct {
    SndChannelPtr snd_channel;
    int channel_snd_index;
    int sound_priority;
    float channel_priority;
    bool looped;
  } mac_sound_struct;

  // Total samples played since start of library
  unsigned short m_total_sounds_played; // Used for unique ids and for stats

  // Sound library status
  unsigned char m_f_sound_lib_init;   // Flag is set if sound library is initialized	-- cmphack
  unsigned char m_f_sound_lib_status; // Paused or running							--
                                      // cmphack

  // May not need some of these
  int m_primary_frequency; // Set to the primary buffers frequency -- cmphack
  int m_primary_alignment;
  int m_current_frequency; //                   					-- cmphack

  bool m_in_sound_frame;
  bool m_pending_actions; // are there sound events pending (outside of of start/end)
  char m_mixer_type;

  virtual void CheckForErrors(); // part of sound system error handler.
  void SetError(int code) { m_lib_error_code = code; };

private:
  unsigned char m_primary_bit_depth; // 16 or 8 bits per sample				-- cmphack
  char m_sound_quality;
  int m_channel_count;

  SSpListenerReference m_lr;
  SSpSourceReference m_sr[MAX_SOUNDS_MIXED];
  emulated_listener m_emulated_listener;

  long master_save;
  float m_volume;

  SSpLocalizationData snd_localization[3]; // for the two sound qualitys that have filtering

  mac_llsSystem *ll_sound_ptr;

  SndChannelPtr snd_channel[MAX_SOUNDS_MIXED];
  int channel_snd_index[MAX_SOUNDS_MIXED];
  int sound_priority[MAX_SOUNDS_MIXED];
  float channel_priority[MAX_SOUNDS_MIXED];
  bool looped[MAX_SOUNDS_MIXED];

  //	mac_sound_struct macSound[MAX_SOUNDS_MIXED];

  bool sndsprk_initialized;
};

#endif