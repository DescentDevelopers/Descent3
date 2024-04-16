/*
* $Logfile: /DescentIII/main/voice.h $
* $Revision: 6 $
* $Date: 6/19/98 6:04p $
* $Author: Jeff $
*
* header file for voice system
*
* $Log: /DescentIII/main/voice.h $
 * 
 * 6     6/19/98 6:04p Jeff
 * added voice queue
 * 
 * 5     6/19/98 11:18a Jeff
 * removed useless valid flag
 * 
 * 4     6/18/98 5:18p Jeff
 * Use new streamaudio system
 * 
 * 3     6/17/98 8:08p Jeff
 * removed DoVoiceFrame
 * 
 * 2     6/08/98 3:55p Jeff
 * rough initial creation
*
* $NoKeywords: $
*/

#ifndef __VOICE_H__
#define __VOICE_H__

//voice flags
#define VF_COMPRESSED	0x00000001	//If bit is set then we are using compressed audio stream
#define VF_STEREO			0x00000002	//In Stereo
#define VF_16BIT			0x00000004
#define VF_8BIT			0x00000008
#define VF_INTERUPT		0x00000010	//It will interupt whatever voice is playing and play this
#define VF_POWERUP		0x00000020	//This voice is a powerup voice
#define VF_FORCE			0x00000040	//Forces the voice to play, even if turned off
#define VF_PLAYTABLE		0x00000100	//The filename parameter is actually the name of a sound from the sound table

bool InitVoices(void);
void CloseVoices(void);
void PlayVoice(char *filename,int flags);
void StopVoice(void);
void UpdateVoices(void);

extern bool PlayPowerupVoice;
extern bool PlayVoices;


#endif