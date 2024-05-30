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

#ifndef __LNX_DSOUND_H_
#define __LNX_DSOUND_H_

// Max&Min values for settings
#define LNXSND_VOLUME_MAX 0
#define LNXSND_VOLUME_MIN -10000
#define LNXSND_PAN_LEFT -10000
#define LNXSND_PAN_RIGHT 10000

// Status/Buffer flags
#define LNXSND_PLAYING 0x0001
#define LNXSND_LOOPING 0x0002

// Buffer lock flags
#define LNXSND_LOCK_FROMWRITECURSOR 0x0001
#define LNXSND_LOCK_ENTIREBUFFER 0x0002

// Capability flags
#define LNXSND_CAPS_PRIMARYBUFFER 0x0001
#define LNXSND_CAPS_CTRLVOLUME 0x0002
#define LNXSND_CAPS_CTRLPAN 0x0004
#define LNXSND_CAPS_CTRLFREQUENCY 0x0008
#define LNXSND_CAPS_CTRLDEFAULT 0x000E
#define LNXSND_CAPS_LOCSOFTWARE 0x0010

struct LnxSoundDevice {
  int sound_device;       // file device handle for sound
  uint32_t bps;       // (bytes per second) channels*freq*bit_depth/8
  uint32_t freq;      // frequency (22050, etc.)
  uint32_t bit_depth; // 8 or 16
  uint32_t channels;  // 1 or 2 (mono or stereo)
};

struct WAVEFORMATEX {
  uint16_t wFormatTag;
  uint16_t nChannels;
  uint32_t nSamplesPerSec;
  uint32_t nAvgBytesPerSec;
  uint16_t nBlockAlign;
  uint16_t wBitsPerSample;
  uint16_t cbSize;
}; // Taken from Windows for porting
#define WAVE_FORMAT_PCM 0x01

struct LinuxSoundCaps {
  uint32_t dwFlags;
  uint32_t dwBufferBytes;
};

struct LnxBufferDesc {
  WAVEFORMATEX *lpwfxFormat;
  uint32_t dwBufferBytes;
  uint32_t dwFlags;
};

struct LnxSoundBuffer
  int freq_adjustment;
  int bps;
  uint32_t buffer_len;
  uint32_t play_cursor;
  uint32_t write_cursor;
  uint32_t flags;
  uint32_t left_vol, right_vol;

  uint8_t *buffer;

  int32_t volume;
  int32_t pan;

  WAVEFORMATEX wfx;

  LnxBufferDesc lbdesc;

  uint16_t freq;
  char playing;
  char __pad;
};

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
int LnxSoundBuffer_SetVolume(LnxSoundBuffer *buff, int32_t vol);

///////////////////////////
// LnxSoundBuffer_SetPan
///////////////////////////
// Sets the pan of a buffer.
//
// Returns:
//        0 : no error
//       -1 : Cannot set pan
//       -2 : Invalid parameters
int LnxSoundBuffer_SetPan(LnxSoundBuffer *buff, int32_t pan);

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
int LnxSoundBuffer_Play(LnxSoundBuffer *buff, uint32_t flags);

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
int LnxSoundBuffer_GetStatus(LnxSoundBuffer *buff, uint32_t *status);

///////////////////////////////////////
// LnxSoundBuffer_GetCurrentPosition
///////////////////////////////////////
// Returns the current play and write positions of the buffer
//
// Returns:
//        0 : no error
//       -1 : invalid parameters
int LnxSoundBuffer_GetCurrentPosition(LnxSoundBuffer *buff, uint32_t *ppos, uint32_t *wpos);

///////////////////////////////////////
// LnxSoundBuffer_SetCurrentPosition
///////////////////////////////////////
// Sets the current play position of the buffer
//
// Returns:
//        0 : no error
//       -1 : invalid parameters
int LnxSoundBuffer_SetCurrentPosition(LnxSoundBuffer *buff, uint32_t pos);

/////////////////////////
// LnxSoundBuffer_Lock
/////////////////////////
// Locks the given buffer, returning pointer(s) to the buffer(s) along with
// available the size of the buffer(s) for writing.
//
// Returns:
//        0 : no error
//       -1 : invalid parameters
int LnxSoundBuffer_Lock(LnxSoundBuffer *buff, uint32_t pos, uint32_t numbytes, void **ptr1,
                        uint32_t *numbytes1, void **ptr2, uint32_t *numbytes2, uint32_t flags);

///////////////////////////
// LnxSoundBuffer_Unlock
///////////////////////////
// Unlocks a buffer.
//
// Returns:
//        0 : no error
//       -1 : invalid parameters
int LnxSoundBuffer_Unlock(LnxSoundBuffer *buff, void *ptr1, uint32_t num1, void *ptr2, uint32_t num2);

#endif
