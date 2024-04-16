#ifndef __LNX_DSOUND_H_
#define __LNX_DSOUND_H_

#include "SystemInterfaces.h"
typedef struct {
  int sound_device;       // file device handle for sound
  unsigned int bps;       // (bytes per second) channels*freq*bit_depth/8
  unsigned int freq;      // frequency (22050, etc.)
  unsigned int bit_depth; // 8 or 16
  unsigned int channels;  // 1 or 2 (mono or stereo)
} LnxSoundDevice;

#define WAVEFORMATEX SoundWAVEFormatEx

#define WAVE_FORMAT_PCM 0x01

#define LinuxSoundCaps SysSoundCaps

#define LnxBufferDesc SysSoundBufferDesc

typedef struct {
  int freq_adjustment;
  int bps;
  unsigned int buffer_len;
  unsigned int play_cursor;
  unsigned int write_cursor;
  unsigned int flags;
  unsigned long left_vol, right_vol;

  unsigned char *buffer;

  signed long volume;
  signed long pan;

  WAVEFORMATEX wfx;

  LnxBufferDesc lbdesc;

  unsigned short freq;
  char playing;
  char __pad;
} LnxSoundBuffer;

///////////////////////////////
// LnxSound_CreateSoundBuffer
///////////////////////////////
// Creates a sound buffer to be used with mixing and output.
//
// Returns:
//       -1 : Invalid Parameter
//       -2 : Out of memory
//        0 : Ok!
int LnxSound_CreateSoundBuffer(LnxSoundDevice *dev, LnxBufferDesc *lbdesc, LnxSoundBuffer **lsndb);

////////////////////////////
// LnxSoundBuffer_Release
////////////////////////////
// Releases the memory associated with a sound buffer.  This pointer is
// no longer valid after return.
//
// Returns:
//       -1 : Invalid Parameter
//        0 : Ok!
int LnxSoundBuffer_Release(LnxSoundBuffer *buff);

//////////////////////////////
// LnxSoundBuffer_SetVolume
//////////////////////////////
// Sets the volume of a buffer.
//
// Returns:
//        0 : no error
//       -1 : Cannot set volume
//       -2 : Invalid parameters
int LnxSoundBuffer_SetVolume(LnxSoundBuffer *buff, signed long vol);

///////////////////////////
// LnxSoundBuffer_SetPan
///////////////////////////
// Sets the pan of a buffer.
//
// Returns:
//        0 : no error
//       -1 : Cannot set pan
//       -2 : Invalid parameters
int LnxSoundBuffer_SetPan(LnxSoundBuffer *buff, signed long pan);

/////////////////////////
// LnxSoundBuffer_Stop
/////////////////////////
// Stops a buffer from playing
//
// Returns:
//        0 : no error
//       -1 : invalid parameters
int LnxSoundBuffer_Stop(LnxSoundBuffer *buff);

/////////////////////////
// LnxSoundBuffer_Play
/////////////////////////
// Starts a buffer playing (or changes the flags for a buffer currently
// playing).
//
// Returns:
//        0 : no error
//       -1 : invalid parameters
int LnxSoundBuffer_Play(LnxSoundBuffer *buff, unsigned int flags);

////////////////////////////
// LnxSoundBuffer_GetCaps
////////////////////////////
// Starts a buffer playing (or changes the flags for a buffer currently
// playing).
//
// Returns:
//        0 : no error
//       -1 : invalid parameters
int LnxSoundBuffer_GetCaps(LnxSoundBuffer *buff, LinuxSoundCaps *caps);

//////////////////////////////
// LnxSoundBuffer_GetStatus
//////////////////////////////
// Returns the status of a buffer
//
// Returns:
//        0 : no error
//       -1 : invalid parameters
int LnxSoundBuffer_GetStatus(LnxSoundBuffer *buff, unsigned int *status);

///////////////////////////////////////
// LnxSoundBuffer_GetCurrentPosition
///////////////////////////////////////
// Returns the current play and write positions of the buffer
//
// Returns:
//        0 : no error
//       -1 : invalid parameters
int LnxSoundBuffer_GetCurrentPosition(LnxSoundBuffer *buff, unsigned int *ppos, unsigned int *wpos);

///////////////////////////////////////
// LnxSoundBuffer_SetCurrentPosition
///////////////////////////////////////
// Sets the current play position of the buffer
//
// Returns:
//        0 : no error
//       -1 : invalid parameters
int LnxSoundBuffer_SetCurrentPosition(LnxSoundBuffer *buff, unsigned int pos);

/////////////////////////
// LnxSoundBuffer_Lock
/////////////////////////
// Locks the given buffer, returning pointer(s) to the buffer(s) along with
// available the size of the buffer(s) for writing.
//
// Returns:
//        0 : no error
//       -1 : invalid parameters
int LnxSoundBuffer_Lock(LnxSoundBuffer *buff, unsigned int pos, unsigned int numbytes, void **ptr1,
                        unsigned int *numbytes1, void **ptr2, unsigned int *numbytes2, unsigned int flags);

///////////////////////////
// LnxSoundBuffer_Unlock
///////////////////////////
// Unlocks a buffer.
//
// Returns:
//        0 : no error
//       -1 : invalid parameters
int LnxSoundBuffer_Unlock(LnxSoundBuffer *buff, void *ptr1, unsigned int num1, void *ptr2, unsigned int num2);

#endif
