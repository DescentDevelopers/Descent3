/*
 * $Logfile: /DescentIII/Main/mac/MAC_LLSOUND.CPP $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:58:16 $
 * $Author: kevinb $
 *
 * Mac implementation of low level sound library
 *
 * $Log: MAC_LLSOUND.CPP,v $
 * Revision 1.1.1.1  2003/08/26 03:58:16  kevinb
 * initial 1.5 import
 *
 *
 * 4     4/18/00 12:58p Matt
 * Another update from Duane for 1.4
 *
 * 3     4/12/00 7:08p Matt
 * From Duane for 1.4
 *
 * 2     10/21/99 1:55p Kevin
 * Mac Merge!
 *
 * 1     7/28/99 2:31p Kevin
 * Mac only stuff
 *
 * 1     5/21/97 6:53 PM Jeremy
 *
 * $NoKeywords: $
 */
#include <Sound.h>
#include <SoundSprocket.h>
#include <SoundComponents.h>
#include "pserror.h"
#include "mem.h"
#include "mac_llsound.h"
#include "ssl_lib.h" // Shared sound header (between high and low-levels)

#define REF_DIST 50
#define CLIP_ATTENUATION 0.7f // look in ddsoundload.cpp for its twin

long master_volume_save = -1;
int saveSampleRate;
Component defaultSoundOutputDevice;

pascal void SoundCallBack(SndChannelPtr channel, SndCommand cmd) {
  //	long sound_uid = (long)channel->userInfo;
  channel->userInfo = -1; // set it to negative one to indicate I'm through with you
  //	channel->userInfo = -channel->userInfo;		//set it to negative to indicate I'm through with you
}
void SetLLMasterVolume(float volume) {
  short one_side;
  long both_side;

  if (master_volume_save == -1)
    GetDefaultOutputVolume(&master_volume_save);

  one_side = volume * 0x0100;
  both_side = (one_side << 16) | one_side;
  SetDefaultOutputVolume(both_side);
}
void mac_llsSystem::SetSoundCard(const char *name) { sndsprk_initialized = false; }
// Starts the sound library, maybe have it send back some information -- 3d support?
// mac_llsSystem::InitSoundLib(oeApplication *sos, char mixer_type, char m_sound_quality, bool f_kill_sound_list)
int mac_llsSystem::InitSoundLib(char mixer_type, oeApplication *sos, ubyte max_sounds_played) {
  int i;
  OSStatus err = noErr;
  extern UInt16 gNumTracks;
  if (sndsprk_initialized) {
    if (max_sounds_played != m_channel_count) {
      SetNumChannels(max_sounds_played);
      return 1;
    } else {
      return 0;
    }
  }
  sndsprk_initialized = true;
  ll_sound_ptr = this;
  m_in_sound_frame = false;
  m_pending_actions = false;

  m_channel_count = 0;

  m_mixer_type = SOUND_MIXER_NONE;
  if (master_volume_save == -1)
    GetDefaultOutputVolume(&master_volume_save);

  ComponentDescription theDesc = {kSoundOutputDeviceType, 0, 0, 0, 0};
  defaultSoundOutputDevice = FindNextComponent(NULL, &theDesc);

  err = GetSoundOutputInfo(defaultSoundOutputDevice, siSampleRate, &saveSampleRate);
  if (err)
    mprintf((1, "InitSoundLib: unable st Get output rate\n"));
  err = SetSoundOutputInfo(defaultSoundOutputDevice, siSampleRate, (void *)rate22050hz);
  if (err)
    mprintf((1, "InitSoundLib: unable st set output rate\n"));

  for (i = 0; i < 3; i++) {
    snd_localization[i].cpuLoad = 1;
    snd_localization[i].medium = kSSpMedium_Air;
    snd_localization[i].humidity = 0;
    snd_localization[i].roomSize = 0;
    snd_localization[i].roomReflectivity = 0;
    snd_localization[i].reverbAttenuation = 0;
    snd_localization[i].sourceMode = kSSpSourceMode_Localized;
    snd_localization[i].referenceDistance = REF_DIST;
    snd_localization[i].coneAngleCos = 0;
    snd_localization[i].coneAttenuation = 0;
    snd_localization[i].currentLocation.elevation = 0;
    snd_localization[i].currentLocation.azimuth = 0;
    snd_localization[i].currentLocation.distance = 0;
    snd_localization[i].currentLocation.projectionAngle = 1;
    snd_localization[i].currentLocation.sourceVelocity = 0;
    snd_localization[i].currentLocation.listenerVelocity = 0;
  }
  snd_localization[SQT_LOW].sourceMode = kSSpSourceMode_Unfiltered;

  SetNumChannels(max_sounds_played);

#ifdef SPROCKET17
  SSpListener_New(&m_lr);
  SSpListener_SetMetersPerUnit(m_lr, 1);
  SSpListener_SetReverb(m_lr, 0.0, 0.0, 0.0);
  for (i = 0; i < MAX_SOUNDS_MIXED; i++) {
    SSpSource_New(&m_sr[i]);
    SSpSource_SetMode(m_sr[i], kSSpSourceMode_Localized);
    //		SSpSource_SetSize(m_sr[i], 0.0, 0.0, 0.0);
    //		SSpSource_SetAngularAttenuation(m_sr[i], 0.0, 0.0);
  }
#endif
  return 1;
}
// Cleans up after the Sound Library
void mac_llsSystem::DestroySoundLib(void) {
  int i;

  if (!sndsprk_initialized)
    return;
#ifdef SPROCKET17
  if (m_sound_quality == SQT_HIGH) {
    SSpListener_Dispose(m_lr);
    for (i = 0; i < MAX_SOUNDS_MIXED; i++) {
      snd_channel[i] = NULL;
      SSpSource_Dispose(m_sr[i]);
    }
  }
#endif
  for (i = 0; i < m_channel_count; i++) {
    if (snd_channel[i]) {
      SndDisposeChannel(snd_channel[i], true);
    }
  }
  SetDefaultOutputVolume(master_volume_save);

  OSStatus err = noErr;
  err = SetSoundOutputInfo(defaultSoundOutputDevice, siSampleRate, (void *)saveSampleRate);
  if (err)
    Error("InitSoundLib: unable to set output rate on exit\n");
}
void mac_llsSystem::SetChannelVolume(int channel, float volume) {
  SndCommand sndCommand;
  OSErr err;

  short one_side = volume * 0x0100;
  long both_side = (one_side << 16) | one_side;

  sndCommand.cmd = volumeCmd;
  sndCommand.param1 = 0;
  sndCommand.param2 = both_side;
  err = SndDoImmediate(snd_channel[channel], &sndCommand);
  if (err)
    Error("Sound Channel %d is corrupt: volume", channel);
}
void mac_llsSystem::PlaySound(int file_index, int channel, bool f_looped) {
  SndCommand sndCommand;
  ExtSoundHeader sndHeader;
  OSStatus err;

  if (file_index < 0)
    return;

  if (SoundFiles[file_index].sample_16bit) {
    sndHeader.sampleSize = 16;
    sndHeader.samplePtr = (char *)SoundFiles[file_index].sample_16bit;
  } else if (SoundFiles[file_index].sample_8bit) {
    sndHeader.sampleSize = 8;
    sndHeader.samplePtr = (char *)SoundFiles[file_index].sample_8bit;
  } else
    Error("No sound data available");

  sndHeader.numFrames = SoundFiles[file_index].np_sample_length;

  sndHeader.numChannels = 1;
  //	sndHeader.sampleRate 		= rate44khz;
  sndHeader.sampleRate = rate22050hz;
  sndHeader.encode = extSH;
  sndHeader.baseFrequency = kMiddleC;
  sndHeader.markerChunk = NULL;
  sndHeader.loopStart = 0;
  sndHeader.loopEnd = sndHeader.numFrames;

  //	snd_channel[channel]->userInfo = file_index;
  snd_channel[channel]->userInfo = (file_index << 5) | channel;
  //	long sound_uid = (file_index<<5) | channel;
  //	snd_channel[channel]->userInfo = sound_uid;
  //	mprintf((2, "P %d  ch %d\n", file_index, channel));

  looped[channel] = f_looped;

  sndCommand.cmd = flushCmd;
  sndCommand.param1 = 0;
  sndCommand.param2 = 0L;
  err = SndDoImmediate(snd_channel[channel], &sndCommand);
  if (err)
    Error("Sound Channel %d is corrupt: flushCmd", channel);
  sndCommand.cmd = quietCmd;
  sndCommand.param1 = 0;
  sndCommand.param2 = 0L;
  err = SndDoImmediate(snd_channel[channel], &sndCommand);
  if (err)
    Error("Sound Channel %d is corrupt: quietCmd", channel);

  sndCommand.cmd = bufferCmd;
  sndCommand.param1 = 0;
  sndCommand.param2 = (long)(&sndHeader);
  err = SndDoCommand(snd_channel[channel], &sndCommand, true);
  if (err)
    Error("Sound Channel %d is corrupt: bufferCmd", channel);

  if (f_looped) {
    sndCommand.cmd = freqCmd;
    sndCommand.param1 = 0;
    sndCommand.param2 = 60L;
  } else {

    sndCommand.cmd = callBackCmd;
    sndCommand.param1 = 0;
    sndCommand.param2 = 0L;
  }
  err = SndDoCommand(snd_channel[channel], &sndCommand, true);
  if (err)
    Error("Sound Channel %d is corrupt: bufferCmd", channel);
}
int mac_llsSystem::PlayStream(play_information *play_info) { return 0; }

// Plays a 2d sound
int mac_llsSystem::PlaySound2d(play_information *play_info, int sound_index, float volume, float pan, bool f_looped) {
  int channel;

  if (sound_index < 0)
    return -1;

  int file_index = Sounds[sound_index].sample_index;

  // do common processing.
  if (SoundFiles[file_index].used == 0) {
    mprintf((2, "Tryed to play %d sound, it DNE.\n", file_index));
    return -1;
  }
  if (volume < 0.01)
    return -1;

  channel = FindFreeChannel(volume, play_info->priority);
  if (channel == -1)
    return -1;

  channel_snd_index[channel] = sound_index;
  sound_priority[channel] = play_info->priority;
  channel_priority[channel] = volume * sound_priority[channel];

  mprintf((1, "2d%4d L%d %16s pri %d vol %.2f\n", file_index, f_looped, SoundFiles[file_index].name,
           play_info->priority, volume));
#ifdef SPROCKET17
  if (m_sound_quality == SQT_HIGH || m_sound_quality == SQT_NORMAL)
#else
  if (m_sound_quality == SQT_HIGH)
#endif
  {
    SetChannelVolume(channel, 2.0);
    snd_localization[m_sound_quality].currentLocation.azimuth = pan * PIOVER2;
    snd_localization[m_sound_quality].currentLocation.elevation = 0;
    snd_localization[m_sound_quality].referenceDistance = 1;
    snd_localization[m_sound_quality].currentLocation.distance = volume;
    SndSetInfo(snd_channel[channel], siSSpLocalization, &snd_localization[m_sound_quality]);
    //		mprintf((2, "2d %3d ch %2d d %8.3f pan %f9.4\n", sound_index, channel,
    //			snd_localization[m_sound_quality].currentLocation.azimuth, volume));
  } else {
    SetChannelVolume(channel, volume);
  }

  PlaySound(file_index, channel, f_looped);

  return (snd_channel[channel]->userInfo);
}
// Plays a 3d sound
int mac_llsSystem::PlaySound3d(play_information *play_info, int sound_index, pos_state *cur_pos, float master_volume,
                               bool f_looped, float reverb) {
  int channel;
  float volume;

  if (sound_index < 0)
    return -1;

  // do common processing.
  if (SoundFiles[Sounds[sound_index].sample_index].used == 0) {
    mprintf((2, "Tryed to play %d sound, it DNE.\n", sound_index));
    return -1;
  }
  int file_index = Sounds[sound_index].sample_index;
  float min = Sounds[sound_index].min_distance;
  float max = Sounds[sound_index].max_distance;
  vector dir_to_sound = *cur_pos->position - m_emulated_listener.position;
  float dist = vm_NormalizeVectorFast(&dir_to_sound);

  if (dist < 0.1)
    dir_to_sound = m_emulated_listener.orient.fvec;

  if (dist >= Sounds[sound_index].max_distance)
    return -1;
  else if (dist < Sounds[sound_index].min_distance)
    volume = 1.0;
  else
    volume = (1.0 - ((dist - min) / (max - min)));

  //	if(volume < 0.01) {
  //		mprintf((1, "SMALL VOL %d %f\n", file_index, volume));
  //		return -1;
  //	}

  channel = FindFreeChannel(volume, play_info->priority);
  if (channel == -1) {
    return -1;
  }
  channel_snd_index[channel] = sound_index;
  sound_priority[channel] = play_info->priority;
  channel_priority[channel] = volume * sound_priority[channel];
//	mprintf((2, "ply %d chn %d vol %f pri %d\n", file_index, channel, volume, play_info->priority));
//	mprintf((1, "3d%4d L%d %16s dist %.5f (%.5f,%.5f) pri %d, vol %.5f\n", file_index, f_looped,
// SoundFiles[file_index].name, dist, min, max, 		play_info->priority, volume));
#ifdef SPROCKET17
  if (m_sound_quality == SQT_HIGH) {
    //		mprintf((1, "SRC %.5f %.5f %.5f  ",cur_pos->position->x, cur_pos->position->y, cur_pos->position->z));
    //		mprintf((1, "LST %.5f %.5f %.5f\n", m_emulated_listener.position.x, m_emulated_listener.position.y,
    // m_emulated_listener.position.z));
    SSpSource_SetPosition3f(m_sr[channel], cur_pos->position->x, cur_pos->position->y, cur_pos->position->z);
    SSpSource_SetOrientation3f(m_sr[channel], cur_pos->orient->fvec.x, cur_pos->orient->fvec.y,
                               cur_pos->orient->fvec.z);
    SSpSource_SetUpVector3f(m_sr[channel], cur_pos->orient->uvec.x, cur_pos->orient->uvec.y, cur_pos->orient->uvec.z);
    SSpSource_SetVelocity3f(m_sr[channel], 0, 0, 0);
    //		SSpSource_SetVelocity3f(m_sr[channel], cur_pos->velocity->x, cur_pos->velocity->y,
    // cur_pos->velocity->z);
    SSpSource_SetReferenceDistance(m_sr[channel], (max) / DIST_SCALE);

    SSpSource_CalcLocalization(m_sr[channel], m_lr, &snd_localization[m_sound_quality]);
    SndSetInfo(snd_channel[channel], siSSpLocalization, &snd_localization[m_sound_quality]);
  } else if (m_sound_quality == SQT_NORMAL)
#else
  if (m_sound_quality == SQT_HIGH)
#endif
  {
    float az = (dir_to_sound * m_emulated_listener.orient.rvec);
    float el = (dir_to_sound * m_emulated_listener.orient.uvec);
    if (el > PIOVER2)
      el = PIOVER2;
    else if (el < PIOVER2)
      el = -PIOVER2;

    snd_localization[m_sound_quality].referenceDistance = (max) / DIST_SCALE;
    snd_localization[m_sound_quality].currentLocation.distance = dist;
    snd_localization[m_sound_quality].currentLocation.azimuth = az;
    snd_localization[m_sound_quality].currentLocation.elevation = el;

    SndSetInfo(snd_channel[channel], siSSpLocalization, &snd_localization[m_sound_quality]);
  } else {
    SetChannelVolume(channel, volume);
  }

  PlaySound(file_index, channel, f_looped);

  return (snd_channel[channel]->userInfo);
}
// Sync's a single sound (changes something - frequency, volume, pan 3d stuff)
void mac_llsSystem::AdjustSound(int sound_uid, float volume, float pan, unsigned short frequency) {
  int channel;

  if (sound_uid < 0)
    return;

  for (int channel = 0; channel < m_channel_count; channel++) {
    if (snd_channel[channel]->userInfo == sound_uid) {
      channel_priority[channel] = volume * sound_priority[channel];

#ifdef SPROCKET17
      if (m_sound_quality == SQT_NORMAL || m_sound_quality == SQT_HIGH)
#else
      if (m_sound_quality == SQT_HIGH)
#endif
      {
        SetChannelVolume(channel, 2.0);
        snd_localization[m_sound_quality].currentLocation.azimuth = pan * PIOVER2;
        snd_localization[m_sound_quality].currentLocation.elevation = 0;
        snd_localization[m_sound_quality].referenceDistance = 1.0;
        snd_localization[m_sound_quality].currentLocation.distance = volume;
        SndSetInfo(snd_channel[channel], siSSpLocalization, &snd_localization[m_sound_quality]);
        //			mprintf((2, "adj2d %3d ch %2d d %8.3f pan %f9.4\n", sound_uid, channel,
        //				snd_localization[SQT_NORMAL].currentLocation.azimuth, volume));
      } else {
        SetChannelVolume(channel, volume);
      }
      //			break;
    }
  }
}
void mac_llsSystem::AdjustSound(int sound_uid, pos_state *cur_pos, float adjusted_volume, float reverb) {
  int channel;
  float volume;

  if (sound_uid < 0)
    return;

  vector dir_to_sound = *cur_pos->position - m_emulated_listener.position;
  float dist = vm_NormalizeVectorFast(&dir_to_sound);

  if (dist < 0.1)
    dir_to_sound = m_emulated_listener.orient.fvec;

  for (channel = 0; channel < m_channel_count; channel++) {
    if (snd_channel[channel]->userInfo == sound_uid) {
      float min = Sounds[channel_snd_index[channel]].min_distance;
      float max = Sounds[channel_snd_index[channel]].max_distance;
      if (dist >= max) {
        //				mprintf((1, " %d %.4f >MAX %.4f\n", sound_uid, dist, max));
        StopSound(sound_uid);
        continue;
      } else if (dist < min)
        volume = 1.0;
      else
        volume = (1.0 - ((dist - min) / (max - min)));

      //			if(volume <= 0.01) {
      //				mprintf((2, "SMALL VOL %d %.3f dist %.3f (%.3f %.3f)\n", sound_uid, volume,
      // dist, min, max)); 				StopSound(sound_uid); 				continue;
      //			}
      channel_priority[channel] = volume * sound_priority[channel];
#ifdef SPROCKET17
      if (m_sound_quality == SQT_HIGH) {
        SSpSource_SetPosition3f(m_sr[channel], cur_pos->position->x, cur_pos->position->y, cur_pos->position->z);
        SSpSource_SetOrientation3f(m_sr[channel], cur_pos->orient->fvec.x, cur_pos->orient->fvec.y,
                                   cur_pos->orient->fvec.z);
        SSpSource_SetUpVector3f(m_sr[channel], cur_pos->orient->uvec.x, cur_pos->orient->uvec.y,
                                cur_pos->orient->uvec.z);
        //				SSpSource_SetVelocity3f(m_sr[channel], 0, 0, 0);
        SSpSource_SetVelocity3f(m_sr[channel], cur_pos->velocity->x, cur_pos->velocity->y, cur_pos->velocity->z);

        SSpSource_CalcLocalization(m_sr[channel], m_lr, &snd_localization[m_sound_quality]);
        SndSetInfo(snd_channel[channel], siSSpLocalization, &snd_localization[m_sound_quality]);
      } else if (m_sound_quality == SQT_NORMAL)
#else
      if (m_sound_quality == SQT_HIGH)
#endif
      {
        float az = (dir_to_sound * m_emulated_listener.orient.rvec);
        float el = (dir_to_sound * m_emulated_listener.orient.uvec);
        if (el > PIOVER2)
          el = PIOVER2;
        else if (el < -PIOVER2)
          el = -PIOVER2;

        snd_localization[m_sound_quality].currentLocation.distance = dist;
        snd_localization[m_sound_quality].currentLocation.azimuth = az;
        snd_localization[m_sound_quality].currentLocation.elevation = el;

        SndSetInfo(snd_channel[channel], siSSpLocalization, &snd_localization[m_sound_quality]);
      } else {

        SetChannelVolume(channel, volume);
      }
      //			break;
    }
  }
}
// Changes the sound list -- cmphack
void mac_llsSystem::NewSoundList(unsigned short num_sounds, unsigned int *sound_offset_array, char *sound_data,
                                 unsigned int sound_data_size) {}
// Locks and unlocks sounds (used when changing play_info data)
bool mac_llsSystem::LockSound(int sound_uid) { return false; }
bool mac_llsSystem::UnlockSound(int sound_uid) { return false; }
bool mac_llsSystem::SetSoundQuality(char quality) {
  int i;
  OSErr err;

  if (quality == m_sound_quality)
    return true;
  //	 pause any sounds that may be playing
  PauseSounds();
  if (quality == SQT_NORMAL) {
    m_sound_quality = SQT_NORMAL;
  } else if (quality == SQT_LOW) {
    m_sound_quality = SQT_LOW;
  } else {
    m_sound_quality = SQT_HIGH;
  }

  if (sndsprk_initialized) {
    for (i = 0; i < m_channel_count; i++) {
#ifdef SPROCKET17
      if (m_sound_quality == SQT_HIGH || m_sound_quality == SQT_NORMAL)
#else
      if (m_sound_quality == SQT_HIGH)
#endif
        SetChannelVolume(i, 2.0);
      else
        SetChannelVolume(i, 0.5);

      err = SndSetInfo(snd_channel[i], siSSpLocalization, &snd_localization[m_sound_quality]);
      if (err)
        Error("unable to localize sound channel %d\n", i);
    }
  }
#if 1
  for (i = 0; i < MAX_SOUNDS; i++) {
    if (Sounds[i].used != 0 && Sounds[i].sample_index > -1) {
      int snd_index = Sounds[i].sample_index;
      ASSERT(snd_index < MAX_SOUNDS);
      if (SoundFiles[snd_index].sample_8bit && (m_sound_quality == SQT_HIGH || m_sound_quality == SQT_NORMAL)) {
        mem_free(SoundFiles[snd_index].sample_8bit);
        SoundFiles[snd_index].sample_8bit = NULL;

        CheckAndForceSoundDataAlloc(i);
      } else if (SoundFiles[snd_index].sample_16bit && m_sound_quality == SQT_LOW) {
        int count;
        ASSERT(SoundFiles[snd_index].sample_8bit == NULL);
        SoundFiles[snd_index].sample_8bit = (unsigned char *)mem_malloc(SoundFiles[snd_index].sample_length);
        // Do the volume clipping with the high quality sound
        for (count = 0; count < (int)SoundFiles[snd_index].sample_length; count++) {
          SoundFiles[snd_index].sample_16bit[count] *= CLIP_ATTENUATION;
        }
        // NOTE:  Interesting note on sound conversion:  16 bit sounds are signed (0 biase).  8 bit sounds are unsigned
        // (+128 biase).
        for (count = 0; count < (int)SoundFiles[snd_index].sample_length; count++) {
          SoundFiles[snd_index].sample_8bit[count] =
              (unsigned char)((((int)SoundFiles[snd_index].sample_16bit[count]) + 32767) >> 8);
        }
        mem_free(SoundFiles[snd_index].sample_16bit);
        SoundFiles[snd_index].sample_16bit = NULL;
      }
    }
  }
#endif
  ResumeSounds();
  return true;
}
char mac_llsSystem::GetSoundQuality(void) { return m_sound_quality; }
bool mac_llsSystem::SetSoundMixer(char mixer_type) { return true; }
char mac_llsSystem::GetSoundMixer(void) { return SOUND_MIXER_SOFTWARE_16; }
void mac_llsSystem::StopAllSounds(void) {
  int i;
  SndCommand sndCommand;
  OSErr err;

  for (i = 0; i < m_channel_count; i++) {
    if (snd_channel[i]->userInfo > 0) {

      sndCommand.cmd = flushCmd;
      sndCommand.param1 = 0;
      sndCommand.param2 = 0L;
      err = SndDoImmediate(snd_channel[i], &sndCommand);
      if (err)
        Error("Sound Channel %d is corrupt: flushCmd", i);
      sndCommand.cmd = quietCmd;
      sndCommand.param1 = 0;
      sndCommand.param2 = 0L;
      err = SndDoImmediate(snd_channel[i], &sndCommand);
      if (err)
        Error("Sound Channel %d is corrupt: quietCmd", i);

      snd_channel[i]->userInfo = -1;
      channel_priority[i] = 0;
    }
  }
}

// Stops 2d and 3d sounds
void mac_llsSystem::StopSound(int sound_uid, unsigned char f_immediately) {
  int i;
  SndCommand sndCommand;
  OSErr err;

  if (sound_uid < 0)
    return;

  for (i = 0; i < m_channel_count; i++) {
    //		mprintf((2, "%4d:%.5f ", snd_channel[i]->userInfo, channel_priority[i]));
    if ((snd_channel[i]->userInfo) == sound_uid) {
      //			mprintf((2, "stp %d chn %d\n", snd_channel[i]->userInfo, i));

      //			long sound_index = (long)ABS(snd_channel[i]->userInfo);
      //			mprintf((1, "S (ch %d %d) %ld\n", i, sound_index & 0x000f,  sound_index>>4 ));

      if (f_immediately == SKT_STOP_AFTER_LOOP) {
        if (looped[i] == false)
          return;
        sndCommand.cmd = freqCmd;
        sndCommand.param1 = 0;
        sndCommand.param2 = 0L;
        err = SndDoCommand(snd_channel[i], &sndCommand, true);
        if (err)
          Error("Sound Channel %d is corrupt: freqCmd", i);
      } else {
        sndCommand.cmd = flushCmd;
        sndCommand.param1 = 0;
        sndCommand.param2 = 0L;
        err = SndDoImmediate(snd_channel[i], &sndCommand);
        if (err)
          Error("Sound Channel %d is corrupt: flushCmd", i);

        sndCommand.cmd = quietCmd;
        sndCommand.param1 = 0;
        sndCommand.param2 = 0L;
        err = SndDoImmediate(snd_channel[i], &sndCommand);
        if (err)
          Error("Sound Channel %d is corrupt: quietCmd", i);
      }
      snd_channel[i]->userInfo = -1;
      //			snd_channel[i]->userInfo = -snd_channel[i]->userInfo;
      channel_priority[i] = 0;
      sound_priority[i] = 0;
      channel_snd_index[i] = -1;
      //			return;
    }
  }
  //	mprintf((2, "\n"));
}
// Checks if a sound is playing (removes finished sound);
bool mac_llsSystem::IsSoundInstancePlaying(int sound_uid) {
  int i;
  if (sound_uid < 0)
    return false;

  for (i = 0; i < m_channel_count; i++) {
    if (snd_channel[i]->userInfo == sound_uid) {
      return true;
    }
  }
  return false;
}
int mac_llsSystem::IsSoundPlaying(int sound_uid) {
  int i;
  if (sound_uid < 0)
    return -1;
  for (i = 0; i < m_channel_count; i++) {
    if (snd_channel[i]->userInfo == sound_uid) {
      return snd_channel[i]->userInfo;
    }
  }
  return -1;
}
// Set listener's position
void mac_llsSystem::SetListener(pos_state *cur_pos) {
  m_emulated_listener.orient = *cur_pos->orient;
  m_emulated_listener.position = *cur_pos->position;
  m_emulated_listener.velocity = *cur_pos->velocity;
//	m_emulated_listener.velocity.x = 0;
//	m_emulated_listener.velocity.y = 0;
//	m_emulated_listener.velocity.z = 0;
#ifdef SPROCKET17
  if (m_sound_quality == SQT_HIGH) {
    SSpListener_SetPosition3f(m_lr, m_emulated_listener.position.x, m_emulated_listener.position.y,
                              m_emulated_listener.position.z);
    SSpListener_SetOrientation3f(m_lr, m_emulated_listener.orient.fvec.x, m_emulated_listener.orient.fvec.y,
                                 m_emulated_listener.orient.fvec.z);
    SSpListener_SetUpVector3f(m_lr, m_emulated_listener.orient.uvec.x, m_emulated_listener.orient.uvec.y,
                              m_emulated_listener.orient.uvec.z);
    SSpListener_SetVelocity3f(m_lr, m_emulated_listener.velocity.x, m_emulated_listener.velocity.y,
                              m_emulated_listener.velocity.z);
  }
#endif
}
// float ll_volume;
// Sets the master volume (2d and 3d sounds) -- chrishack -- use primary buffer
void mac_llsSystem::SetMasterVolume(float volume)
// void mac_llsSystem::SetLLVolume(float volume)
{
  m_volume = volume;
}
// Gets the master volume
float mac_llsSystem::GetMasterVolume(void)
// float mac_llsSystem::GetLLVolume(void)
{
  return m_volume; // ll_volume;
}
// Pause all sounds/resume all sounds
void mac_llsSystem::PauseSounds(void) {
  int i;
  SndCommand sndCommand;
  OSErr err;

  for (i = 0; i < m_channel_count; i++) {
    if (snd_channel[i]->userInfo >= 0) {
      sndCommand.cmd = pauseCmd;
      sndCommand.param1 = 0;
      sndCommand.param2 = 0L;
      err = SndDoImmediate(snd_channel[i], &sndCommand);
      if (err)
        Error("Sound Channel %d is corrupt: pauseCmd", i);
    }
  }
}
void mac_llsSystem::ResumeSounds(void) {
  int i;
  SndCommand sndCommand;
  OSErr err;

  for (i = 0; i < m_channel_count; i++) {
    if (snd_channel[i]->userInfo >= 0) {
      sndCommand.cmd = resumeCmd;
      sndCommand.param1 = 0;
      sndCommand.param2 = 0L;
      err = SndDoImmediate(snd_channel[i], &sndCommand);
      if (err)
        Error("Sound Channel %d is corrupt: resumeCmd", i);
    }
  }
}
// Begin sound frame
void mac_llsSystem::SoundStartFrame(void) {}
void mac_llsSystem::PauseSound(int sound_uid) {
  int i;
  SndCommand sndCommand;
  OSErr err;

  if (sound_uid < 0)
    return;

  for (i = 0; i < m_channel_count; i++) {
    if (snd_channel[i]->userInfo == sound_uid) {
      sndCommand.cmd = pauseCmd;
      sndCommand.param1 = 0;
      sndCommand.param2 = 0L;
      err = SndDoImmediate(snd_channel[i], &sndCommand);
      if (err)
        Error("Sound Channel %d is corrupt: pauseCmd", i);

      return;
    }
  }
}
void mac_llsSystem::ResumeSound(int sound_uid) {
  int i;
  SndCommand sndCommand;
  OSErr err;

  if (sound_uid < 0)
    return;

  for (i = 0; i < m_channel_count; i++) {
    if (snd_channel[i]->userInfo == sound_uid) {
      sndCommand.cmd = resumeCmd;
      sndCommand.param1 = 0;
      sndCommand.param2 = 0L;
      err = SndDoImmediate(snd_channel[i], &sndCommand);
      if (err)
        Error("Sound Channel %d is corrupt: resumeCmd", i);

      return;
    }
  }
}
bool mac_llsSystem::CheckAndForceSoundDataAlloc(int sound_uid) {
  int result;

  if (sound_uid < 0)
    return;

  int sound_file_index = Sounds[sound_uid].sample_index;
  ASSERT(sound_file_index >= 0 && sound_file_index < MAX_SOUND_FILES);
  if (sound_file_index < 0 || sound_file_index >= MAX_SOUND_FILES) {
    return false;
  }
  // Check if the sample data is already loaded
  if (SoundFiles[sound_file_index].sample_16bit != NULL || SoundFiles[sound_file_index].sample_8bit != NULL)
    return true;
  // If not, get the sound data
  //	result = SoundLoadWaveFile(SoundFiles[sound_file_index].name, Sounds[sound_uid].import_volume, sound_file_index,
  // SQT_HIGH, true);
  result = SoundLoadWaveFile(SoundFiles[sound_file_index].name, Sounds[sound_uid].import_volume, sound_file_index,
                             (m_sound_quality > 0) ? 1 : 0, true);
  // Why would it load once (table load time) and not now?
  if (!result)
    return false;
  //	mprintf((1, "%s loaded.\n", SoundFiles[sound_file_index].name));
  return true;
}
// End sound frame
void mac_llsSystem::SoundEndFrame(void) {}
ubyte mac_llsSystem::GetNumChannels(void) { return m_channel_count; }
int mac_llsSystem::FindFreeChannel(float volume, int priority) {
  int i;

  int found = -1;
  int num_used_chan = 0;

  float weighted_priority = priority * volume;

  // first look for an empty slot
  for (i = 0; i < m_channel_count; i++) {
    if (snd_channel[i]->userInfo < 0) {
      channel_priority[i] = weighted_priority;
      return (i);
    }
  }

  if (priority == 0) // lowest priority does not register past here
    return -1;

  // return the lowest priority slot
  float min_priority = weighted_priority;
  int min_chan = -1;
  for (i = 0; i < m_channel_count; i++) {
    if (channel_priority[i] < min_priority) {
      min_priority = channel_priority[i];
      min_chan = i;
    }
  }

  if (min_chan != -1) {
    //		mprintf((1, "\nK %d snd %d pri %.5f > %.5f\n", min_chan, snd_channel[min_chan]->userInfo,
    // weighted_priority, channel_priority[min_chan]));
    StopSound(snd_channel[min_chan]->userInfo);
    return (min_chan);
  }

  mprintf((2, "NO FREE Sound CHANNEL: priority %f\n", weighted_priority));
  return -1;
}
void mac_llsSystem::SetNumChannels(ubyte num_chan) {
  int i;
  OSStatus err;
  unsigned int cpuLoadLimit;
  SoundComponentLink myLink;
  SndCommand sndCommand;

  if (!sndsprk_initialized)
    return;
  myLink.description.componentType = kSoundEffectsType;
  myLink.description.componentSubType = kSSpLocalizationSubType;
  myLink.description.componentManufacturer = kAnyComponentManufacturer;
  myLink.description.componentFlags = 0;
  myLink.description.componentFlagsMask = kAnyComponentFlagsMask;
  myLink.mixerID = nil;
  myLink.linkID = nil;
  if (num_chan > m_channel_count) {
    SndCallBackUPP myCallBackUPP;
    myCallBackUPP = NewSndCallBackProc(SoundCallBack);

    for (i = m_channel_count; i < num_chan; i++) {
      snd_channel[i] = NULL;
      err = SndNewChannel(&snd_channel[i], sampledSynth, initMono, myCallBackUPP);
      if (err)
        Error("Error %d Unable to allocate sound channel %d", err, i);
      err = SndSetInfo(snd_channel[i], siPreMixerSoundComponent, &myLink);
      if (err)
        Error("SndSetInfo siPreMixerSoundComponent near Error %d", err);

      err = SndSetInfo(snd_channel[i], siSSpLocalization, &snd_localization[SQT_LOW]);
      if (err)
        Error("unable to localize sound channel %d\n", i);

      snd_channel[i]->userInfo = -1;
      channel_priority[i] = 0;
    }
  } else if (num_chan < m_channel_count) {
    for (i = m_channel_count - 1; i >= num_chan; i--) {
      SndDisposeChannel(snd_channel[i], true);
      snd_channel[i] = NULL;
    }
  }
  m_channel_count = num_chan;
}
// environmental sound interface
// volume modifier (0-1), damping(0-1), 1 = complete, 0 = none
//	decay 0.1 to 100 seconds, how long it takes for a sound to die.
bool mac_llsSystem::SetGlobalReverbProperties(float volume, float damping, float decay) {
#ifdef SPROCKET17
  if (m_sound_quality == SQT_HIGH) {
    SSpListener_SetReverb(m_lr, 9, damping * -20, volume * -20);
    SSpListener_SetReverb(m_lr, 0, 0, 0);
  }
#endif
  return false;
}
// set special parameters for the 3d environment.
// of strcuture passed, you must set the appropriate 'flags' value for values you wish to modify
void mac_llsSystem::SetEnvironmentValues(const t3dEnvironmentValues *env) {}
// get special parameters for the 3d environment.
// of strcuture passed, you must set the appropriate 'flags' value for values you wish to modify
void mac_llsSystem::GetEnvironmentValues(t3dEnvironmentValues *env) {}
// enable special parameters for the 3d environment.
// of strcuture passed, you must set the appropriate 'flags' value for values you wish to modify
void mac_llsSystem::SetEnvironmentToggles(const t3dEnvironmentToggles *env) {}
// get states of special parameters for the 3d environment.
// of strcuture passed, you must set the appropriate 'flags' value for values you wish to modify
void mac_llsSystem::GetEnvironmentToggles(t3dEnvironmentToggles *env) {}
// Sound System Error Handler.
void mac_llsSystem::CheckForErrors() {
  // if a fatal error occurred, quit and display an error
  // non fatal errors should be put inside a logfile, or just mprinted out.
  switch (m_lib_error_code) {}
  // must call!
  llsSystem::CheckForErrors();
}

short *sound_render_audible_rooms(pos_state *listener_pos, float max_radius) { return 0; };
void SoundRenderReset(){};
void sound_render_end_frame(){};
bool sound_render_start_frame() { return 0; };

/*
inline int mac_llsSystem::MakeUniqueId(int sound_index, int channel)
{
        return (sound_index + (channel << 10));
}

inline int mac_llsSystem::DecodeId(int sound_index, int channel)
{
        return (sound_index - (channel >> 10));
}
*/
