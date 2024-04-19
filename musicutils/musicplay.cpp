#include "musicplay.h"
#include "win\directx\dsound.h"
#include "ds3dlib.h"
#include "streamaudio.h"
#include "application.h"


static win_llsSystem	Sound_system;
int paged_in_count=0;


bool MusicPlayInit(oeApplication *app)
{
	int retval = Sound_system.InitSoundLib(SOUND_MIXER_DS_8, app, 16);
	if (!retval) 
		return false;

	AudioStream::InitSystem(&Sound_system);

	return true;
}


void MusicPlayClose()
{
	AudioStream::Shutdown();
	Sound_system.DestroySoundLib();
}


void MusicPlayFrame()
{
	Sound_system.SoundStartFrame();
		
	AudioStream::Frame();

	Sound_system.SoundEndFrame();
}


