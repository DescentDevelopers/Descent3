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
 * $Logfile: /Descent3/main/mac_sndlib/mac_llsound.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:58:16 $
 * $Author: kevinb $
 *
 * Mac implementation of low level sound library
 *
 * $Log: mac_llsound.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:58:16  kevinb
 * initial 1.5 import
 *
 *
 * 1     5/21/97 6:53 PM Jeremy
 *
 * $NoKeywords: $
 */

#include "mac_llsound.h"

// Starts the sound library, maybe have it send back some information -- 3d support?
int mac_llsSystem::InitSoundLib(osObject *sos, unsigned char max_sounds_played, unsigned char max_sounds_cached,
                                float volume) {
  return 0;
}

// Changes the sound list -- cmphack
void mac_llsSystem::NewSoundList(unsigned short num_sounds, unsigned int *sound_offset_array, char *sound_data,
                                 unsigned int sound_data_size) {
  ;
}

// Cleans up after the Sound Library
void mac_llsSystem::DestroySoundLib(void) { ; }

// Gets info on a sound index
void mac_llsSystem::GetSoundMaxMinDist(int sound_index, float *min_dist, float *max_dist) { ; }

void mac_llsSystem::GetSoundConeInfo(int sound_index, unsigned short *inner_cone_angle,
                                     unsigned short *outer_cone_angle, float outer_volume) {
  ;
}

// Plays a 2d sound
int mac_llsSystem::PlaySound2d(int sound_index, float volume, float pan, unsigned short frequency) { return 0; }

// Plays a 3d sound
int mac_llsSystem::PlaySound3d(int sound_index, pos_state *cur_pos, unsigned short frequency) { return -1; }

void mac_llsSystem::StopAllSounds(void) { ; }

// Checks if a sound is playing (removes finished sound);
int mac_llsSystem::IsSoundInstancePlaying(int sound_uid) { return -1; }

int mac_llsSystem::IsSoundPlaying(int sound_index) { return -1; }

// Set listener's position
void mac_llsSystem::SetListener(pos_state cur_pos) { ; }

// Sync's a single sound (changes something - frequency, volume, pan 3d stuff)
void mac_llsSystem::AdjustSound(int sound_uid, float volume, float pan, unsigned short frequency) { ; }

void mac_llsSystem::AdjustSound(int sound_uid, pos_state cur_pos) { ; }

// Stops 2d and 3d sounds
void mac_llsSystem::StopSound(int sound_uid, unsigned char f_immediately) { ; }

// Sets the master volume (2d and 3d sounds) -- chrishack -- use primary buffer
void mac_llsSystem::SetMasterVolume(float volume) { ; }

// Gets the master volume
float mac_llsSystem::GetMasterVolume(void) { return 0.0; }

// Pause all sounds/resume all sounds
void mac_llsSystem::PauseSounds(void) { ; }

void mac_llsSystem::ResumeSounds(void) { ; }

// Begin sound frame
void mac_llsSystem::SoundStartFrame(void) { ; }

// End sound frame
void mac_llsSystem::SoundEndFrame(void) { ; }
