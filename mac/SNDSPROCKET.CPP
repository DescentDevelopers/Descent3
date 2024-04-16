#include <stdio.h>
#include <stdlib.h>
#include <Errors.h>
#include <Resources.h>
#include <Sound.h>
#include <SoundSprocket.h>
#include <SoundComponents.h>
#include <math.h>
#include "pstypes.h"
#define SOUND_ON
#define MAX_VOLUME 256
#define MAX_SOUND_CHANNELS 16
Boolean have_descent_cd = false;
int sndsprk_lomem = 0;
int master_save;
int sndsprk_initialized = 0;
ubyte sndsprk_paused = 0;
ubyte sndsprk_cd_started = 0; // have we started a disc that is not the Descent cd?
int midi_volume = 256;
int sndsprk_volume = MAX_VOLUME;
int sndsprk_midi_song_playing = 0;
int sndsprk_last_midi_song = 0;
int sndsprk_last_midi_song_loop = 0;
int sndsprk_max_channels = MAX_SOUND_CHANNELS;
ushort num_sounds = 0;
ushort num_channels = 0;
ushort near_channel = 0;
ushort far_channel = 0;
extern ubyte Config_master_volume;
static int CD_volume;
static SSpListenerReference gListener = nil;
// sound object stuff -- used for fans and the boss only??
#define SOF_USED 1          // Set if this sample is used
#define SOF_PLAYING 2       // Set if this sample is playing on a channel
#define SOF_LINK_TO_OBJ 4   // Sound is linked to a moving object. If object dies, then finishes play and quits.
#define SOF_LINK_TO_POS 8   // Sound is linked to segment, pos
#define SOF_PLAY_FOREVER 16 // Play forever (or until level is stopped), otherwise plays once
#define SOUND_PLAYING 1
#define SOUND_OBJECT_PLAYING 2
typedef struct song_resource {
  short midi_id;
  ubyte lead_inst;
  ubyte buffer_ahead;
  ushort tempo;
  ushort pitch_shift;
  ubyte sound_voices;
  ubyte max_notes;
  ushort norm_voices;
} song_resource;
typedef struct sound_object {
  short signature;  // A unique signature to this sound
  ubyte flags;      // Used to tell if this slot is used and/or currently playing, and how long.
  fix max_volume;   // Max volume that this sound is playing at
  fix max_distance; // The max distance that this sound can be heard at...
  fix distance;     // The distance that this sound is from the player
  int volume;       // Volume that this sound is playing at
  int pan;          // Pan value that this sound is playing at
  short handle;     // What handle this sound is playing on.  Valid only if SOF_PLAYING is set.
  short soundnum;   // The sound number that is playing
  union {
    struct {
      short segnum; // Used if SOF_LINK_TO_POS field is used
      short sidenum;
      vms_vector position;
    } pos;
    struct {
      short objnum; // Used if SOF_LINK_TO_OBJ field is used
      short objsignature;
    } obj;
  } link_type;
} sound_object;
int num_sound_objects = 0;
sound_object SoundObjects[MAX_SOUND_OBJECTS];
short next_signature = 0;
int old_sndnum[3] = {-1, -1, -1};
#define SOUND_OFFSET 10000
#define MIN_SOUND_DISTANCE 10
#define MAX_SOUND_DISTANCE 100
Ptr sound_ptr[MAX_SOUNDS + 1];
SndChannelPtr snd_channel_near[MAX_SOUND_CHANNELS];
SndChannelPtr snd_channel_far[MAX_SOUND_CHANNELS];
SndChannelPtr snd_channel_ambient[3] = {0, 0, 0};
SSpLocalizationData farLocalization, nearLocalization, ambientLocalization;
void calc_polar_sound(vms_vector *sound_pos, float *azimuth, float *elevation);
void sndsprk_reset_sndsprk_sounds() {
  int i;
  SndCommand sndCommand;
  if (!sndsprk_initialized)
    return;

  // DAJ	EndAllSound();
  //	num_sound_objects = 0;
  //	stop_redbook();

  sndCommand.param1 = 0;
  sndCommand.param2 = 0L;

  sndCommand.cmd = flushCmd;
  for (i = 0; i < num_channels; i++)
    SndDoImmediate(snd_channel_near[i], &sndCommand);
  sndCommand.cmd = quietCmd;
  for (i = num_channels; i < num_channels; i++)
    SndDoImmediate(snd_channel_far[i], &sndCommand);

  sndCommand.cmd = flushCmd;
  for (i = 0; i < num_channels; i++)
    SndDoImmediate(snd_channel_far[i], &sndCommand);
  sndCommand.cmd = quietCmd;
  for (i = num_channels; i < num_channels; i++)
    SndDoImmediate(snd_channel_near[i], &sndCommand);

  sndsprk_stop_loop(0);
  sndsprk_stop_loop(1);
  sndsprk_stop_loop(2);
}
void sndsprk_play_sample_3d(int sndnum, float distance, float elevation, float azimuth) {
  SndCommand sndCommand;
  OSStatus err;

  if (Newdemo_state == ND_STATE_RECORDING)
    newdemo_record_sound_3d_once(sndnum, elevation, distance);
#ifdef SOUND_ON
  if (sound_ptr[sndnum] && snd_channel_far[far_channel]) {
    farLocalization.currentLocation.elevation = elevation;
    farLocalization.currentLocation.azimuth = azimuth;
    farLocalization.currentLocation.distance = distance;
    err = SndSetInfo(snd_channel_far[far_channel], siSSpLocalization, &farLocalization);
    //		if(err != noErr)
    //			Error("Far SndSetInfo siSSpLocalization channel %d", far_channel);
    sndCommand.param1 = 0;
    sndCommand.param2 = 0L;

    sndCommand.cmd = flushCmd;
    err = SndDoImmediate(snd_channel_far[far_channel], &sndCommand);
    //		if(err != noErr)
    //			Error("Far Sound Channel %d is corrupt: flushCmd", far_channel);

    sndCommand.cmd = quietCmd; // Send quietCmd to stop any current sound.
    err = SndDoImmediate(snd_channel_far[far_channel], &sndCommand);
    //		if(err != noErr)
    //			Error("Far Sound Channel %d is corrupt: quietCmd", far_channel);
    sndCommand.cmd = bufferCmd;
    sndCommand.param2 = (long)(sound_ptr[sndnum]);
    err = SndDoCommand(snd_channel_far[far_channel], &sndCommand, true);
    //		if(err != noErr)
    //			Error("Far Sound Channel %d is corrupt: bufferCmd", far_channel);

    far_channel++;
    if (far_channel >= num_channels) {
      far_channel = 0;
    }
  }
#endif
}
void sndsprk_play_loop_3d(int chan, int sndnum, float distance, float elevation, float azimuth) {
  SndCommand sndCommand;
  OSStatus err;

  if (Newdemo_state == ND_STATE_RECORDING)
    newdemo_record_sound_3d_once(sndnum, azimuth, distance);
#ifdef SOUND_ON
  if (sound_ptr[sndnum]) {
    ambientLocalization.currentLocation.elevation = elevation;
    ambientLocalization.currentLocation.azimuth = azimuth;
    ambientLocalization.currentLocation.distance = distance;
    err = SndSetInfo(snd_channel_ambient[chan], siSSpLocalization, &ambientLocalization);
    //		if(err != noErr)
    //			Error("Ambient SndSetInfo siSSpLocalization channel %d", chan);

    if (sndnum != old_sndnum[chan]) {
      sndCommand.cmd = bufferCmd;
      sndCommand.param1 = 0;
      sndCommand.param2 = (long)(sound_ptr[sndnum]);
      err = SndDoCommand(snd_channel_ambient[chan], &sndCommand, true);
      //			if(err != noErr)
      //				Error("Ambient Sound Channel %d is corrupt: bufferCmd", chan);
      sndCommand.cmd = freqCmd;
      sndCommand.param1 = 0;
      sndCommand.param2 = 60;
      err = SndDoCommand(snd_channel_ambient[chan], &sndCommand, true);
      //			if(err != noErr)
      //				Error("Ambient Sound Channel %d is corrupt: freqCmd", chan);

      old_sndnum[chan] = sndnum;
    }
  }
#endif
}
void sndsprk_play_sample(int sndnum, fix max_volume) {
  SndCommand sndCommand;
  OSStatus err;

  if (Newdemo_state == ND_STATE_RECORDING)
    newdemo_record_sound(sndnum);
#ifdef SOUND_ON

  if (sound_ptr[sndnum] && snd_channel_near[near_channel]) {
    // set in set_max_channels & does not change
    SndSetInfo(snd_channel_near[near_channel], siSSpLocalization, &nearLocalization);

    sndCommand.param1 = 0;
    sndCommand.param2 = 0L;

    sndCommand.cmd = flushCmd;
    err = SndDoImmediate(snd_channel_near[near_channel], &sndCommand);
    //		if(err != noErr)
    //			Error("Near Sound Channel %d is corrupt: flushCmd", near_channel);

    sndCommand.cmd = quietCmd; // Send quietCmd to stop any current sound.
    err = SndDoImmediate(snd_channel_near[near_channel], &sndCommand);
    //		if(err != noErr)
    //			Error("Near Sound Channel %d is corrupt: quietCmd", near_channel);

    sndCommand.cmd = bufferCmd;
    sndCommand.param2 = (long)(sound_ptr[sndnum]);
    err = SndDoCommand(snd_channel_near[near_channel], &sndCommand, true);
    //		if(err != noErr)
    //			Error("Near Sound Channel %d is corrupt: bufferCmd", near_channel);

    near_channel++;
    if (near_channel >= num_channels) {
      //			mprintf((STDOUT, "near snd_channel roll over at %f\n", f2fl(GameTime)));
      near_channel = 0;
    }
  }
#endif
}
void sndsprk_play_sample_once(int sndnum, fix max_volume) { sndsprk_play_sample(sndnum, max_volume); }
void sndsprk_set_sndsprk_volume(int volume) {
  SndCommand sndCommand;
  int i;
  short one_side;
  long both_side;

  if (!sndsprk_initialized)
    return;

  one_side = volume * 0x20;
  both_side = (one_side << 16) | one_side;
  sndCommand.cmd = volumeCmd;
  sndCommand.param1 = 0;
  sndCommand.param2 = both_side;
  for (i = 0; i < num_channels; i++)
    SndDoImmediate(snd_channel_near[i], &sndCommand);
  for (i = 0; i < num_channels; i++)
    SndDoImmediate(snd_channel_far[i], &sndCommand);

  SndDoImmediate(snd_channel_ambient[0], &sndCommand);
  SndDoImmediate(snd_channel_ambient[1], &sndCommand);
  SndDoImmediate(snd_channel_ambient[2], &sndCommand);
}
void sndsprk_stop_current_song() {
  stop_redbook();
  sndsprk_midi_song_playing = 0;
}
void sndsprk_play_midi_song(int songnum, int loop) {
  OSErr err;
  song_resource *song;
  ushort midi_id;
  if (!sndsprk_initialized)
    return;
  sndsprk_last_midi_song = songnum;
  sndsprk_last_midi_song_loop = loop;
  if (!have_descent_cd) {
    if (!cd_playing && !sndsprk_cd_started)
      redbook_start_play();
    sndsprk_cd_started = 1;
    return;
  }
  sndsprk_stop_current_song();

  if (midi_volume < 1)
    return;
  play_redbook_track(songnum, loop);
  sndsprk_midi_song_playing = 1;
}
void do_midi_stuff() {
  if (sndsprk_last_midi_song_loop && !redbook_IsCDPlaying()) {
    play_redbook_track(sndsprk_last_midi_song, 1);
  }
}
void sndsprk_set_midi_volume(int volume) {
  int old_volume = midi_volume;
  if (!sndsprk_initialized)
    return;
  //	midi_volume = (volume*255)/8;	//TOO LOUD
  midi_volume = (volume * 0x10);

  redbook_set_volume(midi_volume);

  if (!old_volume && midi_volume) {
    sndsprk_play_midi_song(sndsprk_last_midi_song, sndsprk_last_midi_song_loop);
  } else if (old_volume && !midi_volume) {
    sndsprk_stop_current_song();
  }
}
void sndsprk_set_volume(int dvolume, int mvolume) {
  if (!sndsprk_initialized)
    return;
  sndsprk_set_sndsprk_volume(dvolume);
  sndsprk_set_midi_volume(mvolume);
}
void sndsprk_set_master_volume(int volume) {
  short one_side;
  long both_side;
  one_side = volume * 0x20;
  both_side = (one_side << 16) | one_side;
  SetDefaultOutputVolume(both_side);
  //	SetSoundVol(volume);
}
void sndsprk_reset() {
  //	num_sound_objects = 0;
}
void sndsprk_resume_all() {}
void sndsprk_pause_all() {}
void sndsprk_stop_all() {
  SndCommand sndCommand;
  int i;
  sndsprk_stop_current_song();
  sndsprk_stop_loop(0);
  sndsprk_stop_loop(1);
  sndsprk_stop_loop(2);
  sndCommand.param1 = 0;
  sndCommand.param2 = 0L;
  for (i = 0; i < num_channels; i++) {
    if (snd_channel_near[i]) {
      sndCommand.cmd = flushCmd;
      SndDoImmediate(snd_channel_near[i], &sndCommand);
      sndCommand.cmd = quietCmd; // Send quietCmd to stop any current sound.
      SndDoImmediate(snd_channel_near[i], &sndCommand);
    }
  }
  for (i = 0; i < num_channels; i++) {
    if (snd_channel_far[i]) {
      sndCommand.cmd = flushCmd;
      SndDoImmediate(snd_channel_far[i], &sndCommand);
      sndCommand.cmd = quietCmd; // Send quietCmd to stop any current sound.
      SndDoImmediate(snd_channel_far[i], &sndCommand);
    }
  }
}
void sndsprk_set_max_channels(int n) {
  int i;
  OSStatus err;
  unsigned int cpuLoadLimit;
  SoundComponentLink myLink;
  if (!sndsprk_initialized)
    return;
  if (sndsprk_midi_song_playing)
    sndsprk_stop_current_song();
  myLink.description.componentType = kSoundEffectsType;
  myLink.description.componentSubType = kSSpLocalizationSubType;
  myLink.description.componentManufacturer = kAnyComponentManufacturer;
  myLink.description.componentFlags = 0;
  myLink.description.componentFlagsMask = kAnyComponentFlagsMask;
  myLink.mixerID = nil;
  myLink.linkID = nil;
  if (n > num_channels) {

    for (i = num_channels; i < n; i++) {
      snd_channel_near[i] = (SndChannelPtr)NewPtr(sizeof(SndChannel));
      if (!snd_channel_near[i])
        Error("Unable to allocate NewPtr snd_channel_near %d", i);
      snd_channel_near[i]->qLength = 2;
      err = SndNewChannel(&snd_channel_near[i], sampledSynth, initStereo /*initMono+initNoInterp*/, NULL);
      if (err != noErr)
        Error("Error %d Unable to allocate near sound channel %d", err, i);

      err = SndSetInfo(snd_channel_near[i], siPreMixerSoundComponent, &myLink);
      if (err != noErr)
        Error("SndSetInfo siPreMixerSoundComponent near Error %d", err);
      err = SndSetInfo(snd_channel_near[i], siSSpLocalization, &nearLocalization);
      if (err != noErr)
        Error("SndSetInfo siSSpLocalization near Error %d", err);
    }
    for (i = num_channels; i < n; i++) {
      snd_channel_far[i] = (SndChannelPtr)NewPtr(sizeof(SndChannel));
      if (!snd_channel_far[i])
        Error("Unable to allocate NewPtr snd_channel_far %d", i);
      snd_channel_far[i]->qLength = 2;
      err = SndNewChannel(&snd_channel_far[i], sampledSynth, initStereo /*initMono+initNoInterp*/, NULL);
      if (err != noErr)
        Error("Error %d Unable to allocate far sound channel %d", err, i);

      err = SndSetInfo(snd_channel_far[i], siPreMixerSoundComponent, &myLink);
      if (err != noErr)
        Error("SndSetInfo siPreMixerSoundComponent far Error %d", err);
    }
  } else if (n < num_channels) {
    for (i = num_channels - 1; i >= n; i--) {
      SndDisposeChannel(snd_channel_near[i], true);
      snd_channel_near[i] = NULL;
      SndDisposeChannel(snd_channel_far[i], true);
      snd_channel_far[i] = NULL;
    }
  }
  num_channels = n;

  if (snd_channel_ambient[0] == 0) {
    for (i = 0; i < 3; i++) {
      snd_channel_ambient[i] = (SndChannelPtr)NewPtr(sizeof(SndChannel));
      snd_channel_ambient[i]->qLength = 2;
      err = SndNewChannel(&snd_channel_ambient[i], sampledSynth, initStereo /*initMono+initNoInterp*/, NULL);
      if (err != noErr)
        Error("Err %d Unable to allocate ambient Sound Channel %d", err, i);

      err = SndSetInfo(snd_channel_ambient[i], siPreMixerSoundComponent, &myLink);
      if (err != noErr)
        Error("Error %d in SndSetInfo ambient chan %d", err, i);
    }
  }
}
int sndsprk_start_sound_object(int i) {
  fix path_distance;
  int num_search_segs = 20;
  float distance, elevation, azimuth;
  calc_polar_sound(&SoundObjects[i].link_type.pos.position, &azimuth, &elevation);
  path_distance = find_connected_distance(&Viewer->pos, Viewer->segnum, &SoundObjects[i].link_type.pos.position,
                                          SoundObjects[i].link_type.pos.segnum, num_search_segs, WID_RENDPAST_FLAG);
  distance = f2fl(path_distance);
  if (distance > 0) {
    sndsprk_play_loop_3d(0, SoundObjects[i].soundnum, distance, elevation, azimuth);
    return 1;
  }
  return 0;
}
void sndsprk_stop_loop(int chan) {
  SndCommand sndCommand;
  if (old_sndnum[chan] != -1) {
    old_sndnum[chan] = -1;
    sndCommand.cmd = quietCmd; // Send quietCmd to stop any current sound.
    sndCommand.param1 = 0;
    sndCommand.param2 = 0L;
    SndDoImmediate(snd_channel_ambient[chan], &sndCommand);
  }
}
void sndsprk_get_sound_loc(vms_matrix *listener, vms_vector *listener_pos, int listener_seg, vms_vector *sound_pos,
                           int sound_seg, fix max_volume, int *volume, int *pan, fix max_distance) {
  vms_vector vector_to_sound;
  fix angle_from_ear, cosang, sinang;
  fix distance;
  fix path_distance;
  *volume = 0;
  *pan = 0;
  if (!sndsprk_initialized)
    return;

  max_distance = (max_distance * 5) / 4; // Make all sounds travel 1.25 times as far.
  //	Warning: Made the vm_vec_normalized_dir be vm_vec_normalized_dir_quick and got illegal values to acos in the
  //fang computation.
  distance = vm_vec_normalized_dir_quick(&vector_to_sound, sound_pos, listener_pos);

  if (distance < max_distance) {
    int num_search_segs = f2i(max_distance / 20);
    if (num_search_segs < 1)
      num_search_segs = 1;
    path_distance =
        find_connected_distance(listener_pos, listener_seg, sound_pos, sound_seg, num_search_segs, WID_RENDPAST_FLAG);
    // path_distance = distance;
    if (path_distance > -1) {
      *volume = max_volume - (path_distance / f2i(max_distance));
      // mprintf( (0, "Sound path distance %.2f, volume is %d / %d\n", f2fl(distance), *volume, max_volume ));
      if (*volume > 0) {
        angle_from_ear = vm_vec_delta_ang_norm(&listener->rvec, &vector_to_sound, &listener->uvec);
        fix_sincos(angle_from_ear, &sinang, &cosang);
        // mprintf( (0, "volume is %.2f\n", f2fl(*volume) ));
        if (Config_channels_reversed)
          cosang *= -1;
        *pan = fixmuldiv(cosang, 255, F1_0);
      } else {
        *volume = 0;
      }
    }
  }
}
int sndsprk_link_sound_to_object(int soundnum, short objnum, int forever, fix max_volume) { // 10 segs away
  return sndsprk_link_sound_to_object2(soundnum, objnum, forever, max_volume, 256 * F1_0);
}
int sndsprk_link_sound_to_pos(int soundnum, short segnum, short sidenum, vms_vector *pos, int forever, fix max_volume) {
  return sndsprk_link_sound_to_pos2(soundnum, segnum, sidenum, pos, forever, max_volume, F1_0 * 256);
}
int sndsprk_link_sound_to_object2(int org_soundnum, short objnum, int forever, fix max_volume, fix max_distance) {
  fix path_distance;
  float distance, elevation, azimuth;
  int num_search_segs = 20;

  calc_polar_sound(&Objects[objnum].pos, &azimuth, &elevation);
  path_distance = find_connected_distance(&Viewer->pos, Viewer->segnum, &Objects[objnum].pos, Objects[objnum].segnum,
                                          num_search_segs, WID_RENDPAST_FLAG);
  distance = f2fl(path_distance);
  if (!forever && distance > 0) {
    clamp(distance, MIN_SOUND_DISTANCE, MAX_SOUND_DISTANCE);
    sndsprk_play_sample_3d(org_soundnum, distance, elevation, azimuth);
  }
}
int sndsprk_link_sound_to_pos2(int org_soundnum, short sound_seg, short sidenum, vms_vector *sound_pos, int forever,
                               fix max_volume, fix max_distance) {
  fix path_distance;
  float distance, elevation, azimuth;
  int num_search_segs = 20;
  int i;

  if (num_sound_objects > MAX_SOUND_OBJECTS)
    //		Error("Max sound objects exceeded");
    return -1;

  path_distance =
      find_connected_distance(&Viewer->pos, Viewer->segnum, sound_pos, sound_seg, num_search_segs, WID_RENDPAST_FLAG);
  if (!forever) {
    if (path_distance >= 0) {
      distance = f2fl(path_distance);
      clamp(distance, MIN_SOUND_DISTANCE, MAX_SOUND_DISTANCE);
      calc_polar_sound(sound_pos, &azimuth, &elevation);
      sndsprk_play_sample_3d(org_soundnum, distance, elevation, azimuth);
    }
    return -1;
  }

  SoundObjects[num_sound_objects].signature = next_signature++;
  SoundObjects[num_sound_objects].flags = SOF_USED | SOF_LINK_TO_POS;
  if (forever)
    SoundObjects[num_sound_objects].flags |= SOF_PLAY_FOREVER;
  SoundObjects[num_sound_objects].link_type.pos.segnum = sound_seg;
  SoundObjects[num_sound_objects].link_type.pos.sidenum = sidenum;
  SoundObjects[num_sound_objects].link_type.pos.position = *sound_pos;
  SoundObjects[num_sound_objects].soundnum = (short)org_soundnum;
  SoundObjects[num_sound_objects].max_volume = max_volume;
  SoundObjects[num_sound_objects].max_distance = max_distance;
  SoundObjects[num_sound_objects].distance = path_distance;
  SoundObjects[num_sound_objects].volume = 0;
  SoundObjects[num_sound_objects].pan = 0;
  num_sound_objects++;

  //	sndsprk_get_sound_loc( &Viewer->orient, &Viewer->pos, Viewer->segnum,
  //                       &SoundObjects[i].link_type.pos.position, SoundObjects[i].link_type.pos.segnum,
  //                       SoundObjects[i].max_volume, &SoundObjects[i].volume, &SoundObjects[i].pan,
  //                       SoundObjects[i].max_distance );

  //	sndsprk_start_sound_object(i);
  //	sndsprk_play_loop_3d( org_soundnum, distance, elevation, azimuth);
  return SoundObjects[num_sound_objects].signature;
}
void calc_polar_sound(vms_vector *sound_pos, float *azimuth, float *elevation) {
  vms_vector vector_to_sound;
  vms_vector cross_vec;
  fix distfix;
  fix fixdot;
  //	fix updot, rtdot;
  //	float float_dot;
  fix cosang, sinang;
  fixang ang_from_ear;
  distfix = vm_vec_normalized_dir_quick(&vector_to_sound, sound_pos, &Viewer->pos);
  //	vm_vec_sub( &vector_to_sound, sound_pos, &Viewer->pos);
  //	distfix = vm_vec_normalize_quick(&vector_to_sound);
  fixdot = vm_vec_dot(&(Viewer->orient.rvec), &vector_to_sound);

  ang_from_ear = fix_acos(fixdot);
  fix_sincos(ang_from_ear, &sinang, &cosang);
  *azimuth = f2fl(cosang);
  *elevation = f2fl(sinang);
//	mprintf((0, "sndvec %f %f\n", *azimuth*57, *elevation*57));
/*
        fixdot = vm_vec_dot(&(Viewer->orient.fvec), &vector_to_sound);
        vm_vec_cross(&cross_vec, &(Viewer->orient.uvec), &vector_to_sound);
        float_dot = f2fl(fixdot);
        updot = vm_vec_dot(&cross_vec,&(Viewer->orient.fvec));
        if (updot < 0)
                *azimuth = -acosf(float_dot);
        else
                *azimuth = acosf(float_dot);

        rtdot = vm_vec_dot(&cross_vec,&(Viewer->orient.rvec));
        if (rtdot < 0)
                *elevation = -asinf(float_dot);
        else
                *elevation = asinf(float_dot);
*/
#ifdef DEBUG_ON
//	mprintf((0, "az %f ",   *azimuth*57.295780));
//	mprintf((0, "el %f ", *elevation*57.295780));
//	mprintf((0, "| %f %f %", f2fl(angle_from_ear), f2fl(cosang)*57.295780,f2fl(sinang)*57.295780));
//	mprintf((0, "\n"));
#endif
}
#define MAX_AMBIENT_DISTANCE 0xFF0000 // DAJ is 255
void sndsprk_sync_sounds() {
  int i;
  int oldvolume, oldpan;
  SndCommand snd_cmd;
  vms_vector pnt;
  int set = 0;
  fix path_distance;
  fix closet_dist[3] = {MAX_AMBIENT_DISTANCE, MAX_AMBIENT_DISTANCE, MAX_AMBIENT_DISTANCE};
  fix direct_distance;
  fix found;
  int num_search_segs = 4;
  float elevation, azimuth;
  int closet_sound[3] = {-1, -1, -1};
  static byte was_playing[3] = {0, 0, 0};
  int chan;

  if (!sndsprk_initialized)
    return;

  if (!num_sound_objects)
    return;

  // find the closest ambient sounds
  for (i = 0; i < num_sound_objects; i++) {
    if ((SoundObjects[i].flags & SOF_USED) && (SoundObjects[i].flags & SOF_PLAY_FOREVER)) {
      //* first check to see if its even in the sound range
      SoundObjects[i].distance = vm_vec_dist_quick(&Viewer->pos, &SoundObjects[i].link_type.pos.position);
      if (SoundObjects[i].distance <= MAX_AMBIENT_DISTANCE) {
        if (SoundObjects[i].soundnum == 224 || SoundObjects[i].soundnum == 41)
          chan = 0;
        else if (SoundObjects[i].soundnum == 235 || SoundObjects[i].soundnum == 150)
          chan = 1;
        else if (SoundObjects[i].soundnum == 121 || SoundObjects[i].soundnum == 42)
          chan = 2;
        else {
          mprintf((STDOUT, "%d is not one of the standard ambient sounds\n", SoundObjects[i].soundnum));
          continue;
        }
        found = find_connected_distance(&Viewer->pos, Viewer->segnum, &SoundObjects[i].link_type.pos.position,
                                        SoundObjects[i].link_type.pos.segnum, num_search_segs, WID_RENDPAST_FLAG);
        if (found > 0 && SoundObjects[i].distance < closet_dist[chan]) {
          closet_dist[chan] = SoundObjects[i].distance;
          closet_sound[chan] = i;
        }
      }
    }
  }
  // now play them
  for (i = 0; i < 3; i++) {
    int segnum, sidenum, wallnum;
    if ((closet_sound[i] != -1) && (closet_dist[i] > 0) && (closet_dist[i] < MAX_AMBIENT_DISTANCE)) {
      // first is it a valid wall number then look to see if we turned off the force field
      segnum = SoundObjects[closet_sound[i]].link_type.pos.segnum;
      sidenum = SoundObjects[closet_sound[i]].link_type.pos.sidenum;
      wallnum = Segments[segnum].sides[sidenum].wall_num;
      if ((wallnum != -1) && (Walls[wallnum].flags & WALL_ILLUSION_OFF)) {
        sndsprk_stop_loop(i);
        was_playing[i] = 0;
      } else {
        calc_polar_sound(&SoundObjects[closet_sound[i]].link_type.pos.position, &azimuth, &elevation);
        sndsprk_play_loop_3d(i, SoundObjects[closet_sound[i]].soundnum, f2fl(closet_dist[i]), elevation, azimuth);
        was_playing[i] = 1;
      }
    } else if (was_playing[i]) {
      sndsprk_stop_loop(i);
      was_playing[i] = 0;
    }
  }
}
void sndsprk_kill_sound_linked_to_segment(int segnum, int sidenum, int soundnum) { sndsprk_stop_loop(0); }
void sndsprk_kill_sound_linked_to_object(int objnum) {}
void sndsprk_close() {
  int i;
  if (!sndsprk_initialized)
    return;
  sndsprk_stop_current_song();
  sndsprk_stop_loop(0);
  sndsprk_stop_loop(1);
  sndsprk_stop_loop(2);
  for (i = 0; i < num_channels; i++) {
    if (snd_channel_near[i]) {
      SndDisposeChannel(snd_channel_near[i], true);
      DisposePtr((char *)snd_channel_near[i]);
    }
    if (snd_channel_far[i]) {
      SndDisposeChannel(snd_channel_far[i], true);
      DisposePtr((char *)snd_channel_far[i]);
    }
  }
  for (i = 0; i < 3; i++) {
    SndDisposeChannel(snd_channel_ambient[i], true);
    //		DisposePtr((char*)snd_channel_ambient[i]);
  }
  for (i = 0; i <= MAX_SOUNDS; i++) {
    if (sound_ptr[i]) {
      //			UnHoldMemory(sound_ptr[i], soundDataSize);
      DisposePtr(sound_ptr[i]);
    }
  }
  redbook_set_volume(CD_volume);
  SetDefaultOutputVolume(master_save);
}
void sndsprk_init_sounds() {}
void sndsprk_load_sounds() {
  Handle theSound;
  long soundDataSize, header;
  short i;
  SoundHeaderPtr sndhead;
  OSStatus err;
  NumVersion ver;

  ver = SndSoundManagerVersion();
  if (ver.majorRev != 3 && ver.minorAndBugRev != 33)
    Error("Sound Manager 3.33 is required");
  for (i = 0; i <= MAX_SOUNDS; i++) {
    theSound = GetResource('snd ', i + SOUND_OFFSET);
    if (theSound != NULL) {
      HLock((Handle)theSound);
      GetSoundHeaderOffset((SndListResource **)theSound, &header);
      soundDataSize = GetHandleSize((Handle)theSound) - header;
      sound_ptr[i] = NewPtr(soundDataSize);
      if (sound_ptr[i] != NULL) {
        BlockMove((Ptr)(*theSound + header), sound_ptr[i], soundDataSize);
        HUnlock((Handle)theSound);
        ReleaseResource((Handle)theSound);
        if (i == 121) // skip fan tick
          ((SoundHeaderPtr)(sound_ptr[i]))->loopStart = 128;
        else
          ((SoundHeaderPtr)(sound_ptr[i]))->loopStart = 0;
        ((SoundHeaderPtr)(sound_ptr[i]))->loopEnd = soundDataSize - 24;
        ((SoundHeaderPtr)(sound_ptr[i]))->baseFrequency = 60;
        num_sounds++;
        //				HoldMemory(sound_ptr[i], soundDataSize);
      } else {
        Error("Unable to allocate sound_ptr %d", i);
      }
    } else {
      sound_ptr[i] = NULL;
    }
  }
}
int sndsprk_init() {
  int i;
  OSStatus err;
  extern UInt16 gNumTracks;

  GetDefaultOutputVolume(&master_save);

  RedbookHandlerInit();
  if (RedbookIsCDInserted()) {
    have_descent_cd = true;
    gNumTracks = RedbookGetNumTracks();
  }
  CD_volume = redbook_get_volume();

  sndsprk_initialized = 1;

  for (i = 0; i < MAX_SOUND_CHANNELS; i++) {
    snd_channel_near[i] = NULL;
    snd_channel_far[i] = NULL;
  }
  farLocalization.cpuLoad = 0;
  farLocalization.medium = kSSpMedium_Air;
  farLocalization.humidity = 0;
  farLocalization.roomSize = 100;
  farLocalization.roomReflectivity = -100;
  farLocalization.reverbAttenuation = 0;
  farLocalization.sourceMode = kSSpSourceMode_Localized;
  farLocalization.referenceDistance = 10;
  farLocalization.coneAngleCos = 0;
  farLocalization.coneAttenuation = 0;
  farLocalization.currentLocation.elevation = 0;
  farLocalization.currentLocation.azimuth = 0;
  farLocalization.currentLocation.distance = 1;
  farLocalization.currentLocation.projectionAngle = 1;
  farLocalization.currentLocation.sourceVelocity = 0;
  farLocalization.currentLocation.listenerVelocity = 0;
  farLocalization.reserved0 = 0;
  farLocalization.reserved1 = 0;
  farLocalization.reserved2 = 0;
  farLocalization.reserved3 = 0;
  farLocalization.virtualSourceCount = 0;
  nearLocalization.cpuLoad = 0;
  nearLocalization.medium = kSSpMedium_Air;
  nearLocalization.humidity = 0;
  nearLocalization.roomSize = 0;
  nearLocalization.roomReflectivity = 0;
  nearLocalization.reverbAttenuation = 0;
  nearLocalization.sourceMode = kSSpSourceMode_Ambient;
  nearLocalization.referenceDistance = 5;
  nearLocalization.coneAngleCos = 0;
  nearLocalization.coneAttenuation = 0;
  nearLocalization.currentLocation.elevation = 0;
  nearLocalization.currentLocation.azimuth = 0;
  nearLocalization.currentLocation.distance = 5;
  nearLocalization.currentLocation.projectionAngle = 1;
  nearLocalization.currentLocation.sourceVelocity = 0;
  nearLocalization.currentLocation.listenerVelocity = 0;
  nearLocalization.reserved0 = 0;
  nearLocalization.reserved1 = 0;
  nearLocalization.reserved2 = 0;
  nearLocalization.reserved3 = 0;
  nearLocalization.virtualSourceCount = 0;
  ambientLocalization.cpuLoad = 0;
  ambientLocalization.medium = kSSpMedium_Air;
  ambientLocalization.humidity = 0;
  ambientLocalization.roomSize = 0;
  ambientLocalization.roomReflectivity = 0;
  ambientLocalization.reverbAttenuation = 0;
  ambientLocalization.sourceMode = kSSpSourceMode_Localized;
  ambientLocalization.referenceDistance = 10;
  ambientLocalization.coneAngleCos = 0;
  ambientLocalization.coneAttenuation = 0;
  ambientLocalization.currentLocation.elevation = 0;
  ambientLocalization.currentLocation.azimuth = 0;
  ambientLocalization.currentLocation.distance = 1;
  ambientLocalization.currentLocation.projectionAngle = 1;
  ambientLocalization.currentLocation.sourceVelocity = 0;
  ambientLocalization.currentLocation.listenerVelocity = 0;
  ambientLocalization.reserved0 = 0;
  ambientLocalization.reserved1 = 0;
  ambientLocalization.reserved2 = 0;
  ambientLocalization.reserved3 = 0;
  ambientLocalization.virtualSourceCount = 0;
  atexit(sndsprk_close);
  return 0;
}
OSErr ConvertFromWavToRawSound(void *inBufferP, UInt32 inBuffSize, void *outBufferP, UInt32 *outBuffSize) {
  OSErr theErr;
  SoundConverter theSC;
  UInt32 numFrames, numBytes, outBytes;
  UInt32 aditionalOutputFrames, additionalOutputSize;
  SInt32 currentBytes, bytesLeft;
  SoundComponentData input, output; // these hold info for the formats
  input.flags = 0;
  input.format = MicrosoftADPCMFormat; /*one of the wav formats*/
  input.numChannels = 2;
  input.sampleSize = 16;        /*8 for eight bit sound*/
  input.sampleRate = rate44khz; /* a constant defined in Sound.h*/
  input.sampleCount = 0;
  input.buffer = 0;
  input.reserved = 0;
  output.flags = 0;
  output.format = kSoundNotCompressed; /*'raw ' */
  output.numChannels = 2;
  output.sampleSize = 16;        /*8 for eight bit sound*/
  output.sampleRate = rate44khz; /* a constant defined in Sound.h*/
  output.sampleCount = 0;
  output.buffer = 0;
  output.reserved = 0;
  /*This opens a SoundConverter component. the  params are ...
  output format, output format, and the sound converter*/
  theErr = SoundConverterOpen(&input, &output, &theSC);
  if (theErr)
    return theErr;
  /* so how to allocate the new buffer? */
  theErr = SoundConverterGetBufferSize(theSC, inBuffSize, &numFrames, &numBytes, outBuffSize);
  if (theErr)
    return theErr;

  outBuffer = mem_malloc(outBytes);
  if (!outBuffer) {
    mprintf((0, "ConvertFromWavToRawSound: unable to allocate %d bytes", outBytes));
    Int3();
  }
  theErr = SoundConverterBeginConversion(theSC);
  if (theErr)
    return theErr;

  if (inBuffSize <= outBytes) { /*we can do it in one big chunk...*/
    theErr = SoundConverterConvertBuffer(theSC, inBufferP, numFrames, outBufferP, 0, 0);
    if (theErr)
      return theErr;

    theErr = SoundConverterEndConversion(theSC, outBufferP, &aditionalOutputFrames, &additionalOutputSize);
    if (theErr)
      return theErr;
  } else { /*we need to do it in chunks :(  */
    bytesLeft = *outBuffSize;
    while (bytesLeft > 0) {
      theErr = SoundConvertBuffer(theSC, inBufferP, numFrames, numBytes, outBufferP, 0, 0);
      if (theErr)
        return theErr;

      bytesLeft -= numBytes;
    }
    theErr = SoundConverterEndConversion(theSC, outBufferP, &aditionalOutputFrames, &additionalOutputSize);
    if (theErr)
      return theErr;
  }
  return theErr;
}
