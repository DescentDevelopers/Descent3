/*
 * $Logfile: /DescentIII/Main/mac/MACSOUND.CPP $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:58:15 $
 * $Author: kevinb $
 *
 * Low-level linux sound driver
 *
 * $Log: MACSOUND.CPP,v $
 * Revision 1.1.1.1  2003/08/26 03:58:15  kevinb
 * initial 1.5 import
 *
 *
 * 2     10/21/99 1:55p Kevin
 * Mac Merge!
 *
 * 1     7/28/99 2:31p Kevin
 * Mac only stuff
 *
 *
 * $NoKeywords: $
 */
//	NEED THIS SINCE DDSNDLIB is a DD library.
#include "DDAccess.h"
#include <stdio.h>
#include <stdlib.h>
// #include <process.h>
#include "cfile.h"
#include "pserror.h"
#include "mono.h"
#include "soundload.h"
#include "ssl_lib.h"
#include "mem.h"
#include "application.h"
#include "macsound.h"
// Starts the sound library, maybe have it send back some information -- 3d support?
int macsound::InitSoundLib(char mixer_type, oeApplication *sos, unsigned char max_sounds_played) { return 0; }
// Cleans up after the Sound Library
void macsound::DestroySoundLib(void) {}
// Locks and unlocks sounds (used when changing play_info data)
bool macsound::LockSound(int sound_uid) { return false; }
bool macsound::UnlockSound(int sound_uid) { return false; }
bool macsound::SetSoundQuality(char quality) { return true; }
char macsound::GetSoundQuality(void) { return 0; }
bool macsound::SetSoundMixer(char mixer_type) { return true; }

char macsound::GetSoundMixer(void) { return 0; }
// Plays a 2d sound
int macsound::PlaySound2d(play_information *play_info, int sound_index, float volume, float pan, bool f_looped) {
  return -1;
}
int macsound::PlayStream(play_information *play_info) { return -1; }
void macsound::SetListener(pos_state *cur_pos) {}
int macsound::PlaySound3d(play_information *play_info, int sound_index, pos_state *cur_pos, float master_volume,
                          bool f_looped, float reverb) //, unsigned short frequency
{
  return -1;
}
void macsound::AdjustSound(int sound_uid, float f_volume, float f_pan, unsigned short frequency) {}
void macsound::AdjustSound(int sound_uid, pos_state *cur_pos, float adjusted_volume, float reverb) {}
void macsound::StopAllSounds(void) {}

// Checks if a sound is playing (removes finished sound);
bool macsound::IsSoundInstancePlaying(int sound_uid) { return false; }
int macsound::IsSoundPlaying(int sound_index) { return 0; }
// Stops 2d and 3d sounds
void macsound::StopSound(int sound_uid, unsigned char f_immediately) {}
// Pause all sounds/resume all sounds
void macsound::PauseSounds(void) {}
void macsound::ResumeSounds(void) {}
bool macsound::CheckAndForceSoundDataAlloc(int sound_file_index) { return false; }
// Begin sound frame
void macsound::SoundStartFrame(void) {}
// End sound frame
void macsound::SoundEndFrame(void) {}
// returns the error string.
char *macsound::GetErrorStr() const {
  static char buffer[] = "Linux SND Driver Not Implementeded";
  return buffer;
}
bool macsound::SetGlobalReverbProperties(float volume, float damping, float decay) { return false; }
///////////////////////////////////////////////////////////////////////
// llsGeometry
#include "ddsndgeometry.h"
// specify a sound library to associate geometry with
bool llsGeometry::Init(llsSystem *snd_sys) { return false; }
// closes low level geometry system.
void llsGeometry::Shutdown() {}
void llsGeometry::StartFrame() {}
void llsGeometry::EndFrame() {}
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
void llsGeometry::AddPoly(int nv, vector **verts, tSoundMaterial material) {}
// 4 verts here.
void llsGeometry::AddQuad(vector **verts) {}

// 3 verts here.
void llsGeometry::AddTriangle(vector **verts) {}
void llsGeometry::CreateMaterial(tSoundMaterial material, float transmit_gain, float transmit_highfreq,
                                 float reflect_gain, float reflect_highfreq) {}
void llsGeometry::DestroyMaterial(tSoundMaterial material) {}
