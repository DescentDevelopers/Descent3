#ifndef __MAC_DD_SOUND_H_
#define __MAC_DD_SOUND_H_
#include "ssl_lib.h"
class macsound : public llsSystem
{
// Public functions 
public:
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
	virtual int PlaySound3d(play_information *play_info, int sound_index, pos_state *cur_pos, float master_volume, bool f_looped, float reverb=0.5f); //, unsigned short frequency
	virtual void AdjustSound(int sound_uid, float f_volume, float f_pan, unsigned short frequency);
	virtual void AdjustSound(int sound_uid, pos_state *cur_pos, float adjusted_volume, float reverb=0.5f);
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
	int GetLastError() { int code = m_lib_error_code; m_lib_error_code = 0; return code; };
	// environmental sound interface
	// volume modifier (0-1), damping(0-1), 1 = complete, 0 = none
	//	decay 0.1 to 100 seconds, how long it takes for a sound to die.
	virtual bool SetGlobalReverbProperties(float volume, float damping, float decay);
protected:
	void SetError(int code) { m_lib_error_code = code; };
};
#endif