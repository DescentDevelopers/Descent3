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

#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <memory.h>
#include "lnxdsound.h"
#include "args.h"

#include "SDL.h"
#include "SDL_audio.h"

#include <sched.h>

#define FRAGMENT_LENGTH (LnxBuffers[0]->bps >> 4)
#define FREQUENCY_SHIFT (14)

/*
 * TODO:
 *   * Might be wise to use mutex's for the enter/exit critical functions
 */
static int LnxNumBuffers = 0;
static LnxSoundBuffer **LnxBuffers = NULL;
static LnxSoundDevice LinuxSoundDevice;

static bool StartupSoundSystem(LnxSoundDevice *dev);
static void ShutdownSoundSystem(void);
static void LinuxSoundMixWithVolume(LnxSoundBuffer *dsb, unsigned char *buf, unsigned int len);
static unsigned int LinuxSoundMixNormalize(LnxSoundBuffer *dsb, unsigned char *buf, unsigned int len);
static unsigned int LinuxSoundMixInMainBuffer(LnxSoundBuffer *dsb, int len);
static void LinuxSoundMixBuffersIntoMain(int len);
static void LinuxSoundThreadHandler(void *unused, Uint8 *stream, int len);

static inline void enter_critical(void) { SDL_LockAudio(); }

static inline void exit_critical(void) { SDL_UnlockAudio(); }

///////////////////////////////
// LnxSound_CreateSoundBuffer
///////////////////////////////
// Creates a sound buffer to be used with mixing and output.
//
// Returns:
//       -1 : Invalid Parameter
//       -2 : Out of memory
//        0 : Ok!
int LnxSound_CreateSoundBuffer(LnxSoundDevice *dev, LnxBufferDesc *lbdesc, LnxSoundBuffer **lsndb) {
  WAVEFORMATEX *wfex;

  if (!lbdesc || !lsndb || !dev)
    return -1;

  wfex = lbdesc->lpwfxFormat;
  if (!wfex)
    return -1;

  // Check to see if we have a primary buffer yet, if not, create it
  // now
  if (lbdesc->dwFlags & LNXSND_CAPS_PRIMARYBUFFER) {
    if (LnxNumBuffers != 0)
      return -1;
  } else {
    if (LnxNumBuffers == 0) {
      // we need to create a primary buffer
      LnxSoundBuffer *primary;
      LnxBufferDesc primdesc;
      WAVEFORMATEX wf;

      memset(&primdesc, 0, sizeof(LnxBufferDesc));
      memset(&wf, 0, sizeof(wf));

      primdesc.dwBufferBytes = 0;
      primdesc.dwFlags = LNXSND_CAPS_PRIMARYBUFFER;
      primdesc.lpwfxFormat = &wf;

      int ret = LnxSound_CreateSoundBuffer(dev, &primdesc, &primary);
      if (ret != 0)
        return ret;
    }
  }

  *lsndb = (LnxSoundBuffer *)malloc(sizeof(LnxSoundBuffer));
  if (!(*lsndb))
    return -2;
  memset(*lsndb, 0, sizeof(LnxSoundBuffer));

  if (lbdesc->dwFlags & LNXSND_CAPS_PRIMARYBUFFER) {
    (*lsndb)->buffer_len = dev->bps;
    (*lsndb)->freq = dev->freq;
    (*lsndb)->bps = dev->bps;
    (*lsndb)->buffer = NULL;
  } else {
    (*lsndb)->buffer_len = lbdesc->dwBufferBytes;
    (*lsndb)->freq = lbdesc->lpwfxFormat->nSamplesPerSec;

    (*lsndb)->buffer = (unsigned char *)malloc((*lsndb)->buffer_len);
    if (!(*lsndb)->buffer) {
      free(*lsndb);
      *lsndb = NULL;
      return -2;
    }
    memset((*lsndb)->buffer, 0, (*lsndb)->buffer_len);
  }

  (*lsndb)->play_cursor = 0;
  (*lsndb)->write_cursor = 0;
  (*lsndb)->playing = 0;
  (*lsndb)->left_vol = (1 << 15);
  (*lsndb)->right_vol = (1 << 15);

  if (!(lbdesc->dwFlags & LNXSND_CAPS_PRIMARYBUFFER)) {
    (*lsndb)->freq_adjustment = ((*lsndb)->freq << FREQUENCY_SHIFT) / LnxBuffers[0]->freq;
    (*lsndb)->bps = (*lsndb)->freq * lbdesc->lpwfxFormat->nBlockAlign;
  }

  memcpy(&((*lsndb)->lbdesc), lbdesc, sizeof(LnxBufferDesc));

  if (!(lbdesc->dwFlags & LNXSND_CAPS_PRIMARYBUFFER)) {
    memcpy(&((*lsndb)->wfx), lbdesc->lpwfxFormat, sizeof(WAVEFORMATEX));
  } else {
    // set up the wave format based on the device settings (primary)
    (*lsndb)->wfx.wFormatTag = WAVE_FORMAT_PCM;
    (*lsndb)->wfx.nChannels = dev->channels;
    (*lsndb)->wfx.nSamplesPerSec = dev->freq;
    (*lsndb)->wfx.nBlockAlign = dev->channels * (dev->bit_depth / 8);
    (*lsndb)->wfx.nAvgBytesPerSec = dev->freq * (*lsndb)->wfx.nBlockAlign;
    (*lsndb)->wfx.wBitsPerSample = dev->bit_depth;
    (*lsndb)->wfx.cbSize = 0;
  }

  if (LnxBuffers) {
    enter_critical();
    LnxBuffers = (LnxSoundBuffer **)realloc(LnxBuffers, sizeof(LnxSoundBuffer *) * (LnxNumBuffers + 1));
    LnxBuffers[LnxNumBuffers] = *lsndb;
    LnxNumBuffers++;
    exit_critical();
  } else {
    LnxBuffers = (LnxSoundBuffer **)malloc(sizeof(LnxSoundBuffer *));
    LnxBuffers[0] = *lsndb;
    LnxNumBuffers++;

    // Initialize the Sound system and thread
    StartupSoundSystem(dev);
  }

  return 0;
}

////////////////////////////
// LnxSoundBuffer_Release
////////////////////////////
// Releases the memory associated with a sound buffer.  This pointer is
// no longer valid after return.
//
// Returns:
//       -1 : Invalid Parameter
//        0 : Ok!
int LnxSoundBuffer_Release(LnxSoundBuffer *buff) {
  int i;

  if (!buff)
    return -1;

  for (i = 0; i < LnxNumBuffers; i++) {
    if (LnxBuffers[i] == buff)
      break;
  }

  if (i < LnxNumBuffers) {
    if (LnxNumBuffers == 1) {
      // stop the thread! primary going down
      ShutdownSoundSystem();

      LnxNumBuffers = 0;
      LnxBuffers = NULL;
    } else {
      // wait until it is ok (our thread is in a good position)
      enter_critical();

      if (i == 0) {
        // can't delete the primary! whats going on here?
        return -1;
      }

      LnxBuffers[i] = LnxBuffers[LnxNumBuffers - 1];
      LnxBuffers = (LnxSoundBuffer **)realloc(LnxBuffers, sizeof(LnxSoundBuffer *) * (LnxNumBuffers - 1));
      LnxNumBuffers--;

      exit_critical();
    }

    if (buff->buffer)
      free(buff->buffer);
    free(buff);
  } else
    return -1;

  if (LnxNumBuffers == 1) {
    // we freed the last non-primary buffer
    // so remove the primary buffer that is remaining
    return LnxSoundBuffer_Release(LnxBuffers[0]);
  }

  return 0;
}

//////////////////////////////
// LnxSoundBuffer_SetVolume
//////////////////////////////
// Sets the volume of a buffer.
//
// Returns:
//        0 : no error
//       -1 : Cannot set volume
//       -2 : Invalid parameters
int LnxSoundBuffer_SetVolume(LnxSoundBuffer *buff, signed int vol) {
  if (!buff)
    return -1;

  if (!(buff->lbdesc.dwFlags & LNXSND_CAPS_CTRLVOLUME))
    return -1;

  if ((vol > LNXSND_VOLUME_MAX) || (vol < LNXSND_VOLUME_MIN))
    return -2;

  if (buff->lbdesc.dwFlags & LNXSND_CAPS_PRIMARYBUFFER) {
    // not supported
    enter_critical();
    buff->volume = vol;
    exit_critical();
    return 0;
  }

  enter_critical();

  buff->volume = vol;

  double vt;
  vt = (double)(buff->volume - (buff->pan > 0 ? buff->pan : 0));
  buff->left_vol = (unsigned int)(pow(2.0, vt / 600.0) * 32768.0);
  vt = (double)(buff->volume + (buff->pan < 0 ? buff->pan : 0));
  buff->right_vol = (unsigned int)(pow(2.0, vt / 600.0) * 32768.0);

  exit_critical();

  return 0;
}

///////////////////////////
// LnxSoundBuffer_SetPan
///////////////////////////
// Sets the pan of a buffer.
//
// Returns:
//        0 : no error
//       -1 : Cannot set pan
//       -2 : Invalid parameters
int LnxSoundBuffer_SetPan(LnxSoundBuffer *buff, signed int pan) {
  if (!buff)
    return -1;

  if ((pan > LNXSND_PAN_RIGHT) || (pan < LNXSND_PAN_LEFT))
    return -2;

  if (!(buff->lbdesc.dwFlags & LNXSND_CAPS_CTRLPAN) || (buff->lbdesc.dwFlags & LNXSND_CAPS_PRIMARYBUFFER)) {
    return -1;
  }

  enter_critical();

  buff->pan = pan;

  double pt;
  pt = (double)(buff->volume - (buff->pan > 0 ? buff->pan : 0));
  buff->left_vol = (unsigned int)(pow(2.0, pt / 600.0) * 32768.0);
  pt = (double)(buff->volume + (buff->pan < 0 ? buff->pan : 0));
  buff->right_vol = (unsigned int)(pow(2.0, pt / 600.0) * 32768.0);

  exit_critical();

  return 0;
}

/////////////////////////
// LnxSoundBuffer_Stop
/////////////////////////
// Stops a buffer from playing
//
// Returns:
//        0 : no error
//       -1 : invalid parameters
int LnxSoundBuffer_Stop(LnxSoundBuffer *buff) {
  if (!buff)
    return -1;

  enter_critical();
  buff->playing = 0;
  exit_critical();
  return 0;
}

/////////////////////////
// LnxSoundBuffer_Play
/////////////////////////
// Starts a buffer playing (or changes the flags for a buffer currently
// playing).
//
// Returns:
//        0 : no error
//       -1 : invalid parameters
int LnxSoundBuffer_Play(LnxSoundBuffer *buff, unsigned int flags) {
  if (!buff)
    return -1;

  enter_critical();
  buff->flags = flags;
  buff->playing = 1;
  exit_critical();
  return 0;
}

////////////////////////////
// LnxSoundBuffer_GetCaps
////////////////////////////
// Starts a buffer playing (or changes the flags for a buffer currently
// playing).
//
// Returns:
//        0 : no error
//       -1 : invalid parameters
int LnxSoundBuffer_GetCaps(LnxSoundBuffer *buff, LinuxSoundCaps *caps) {
  if (!caps || !buff)
    return -1;

  caps->dwFlags = buff->lbdesc.dwFlags | LNXSND_CAPS_LOCSOFTWARE;
  caps->dwBufferBytes = buff->lbdesc.dwBufferBytes;

  return 0;
}

//////////////////////////////
// LnxSoundBuffer_GetStatus
//////////////////////////////
// Returns the status of a buffer
//
// Returns:
//        0 : no error
//       -1 : invalid parameters
int LnxSoundBuffer_GetStatus(LnxSoundBuffer *buff, unsigned int *status) {
  if (!status || !buff)
    return -1;

  *status = 0;
  if (buff->playing)
    *status |= LNXSND_PLAYING;
  if (buff->flags & LNXSND_LOOPING)
    *status |= LNXSND_LOOPING;

  return 0;
}

///////////////////////////////////////
// LnxSoundBuffer_GetCurrentPosition
///////////////////////////////////////
// Returns the current play and write positions of the buffer
//
// Returns:
//        0 : no error
//       -1 : invalid parameters
int LnxSoundBuffer_GetCurrentPosition(LnxSoundBuffer *buff, unsigned int *ppos, unsigned int *wpos) {
  if (!buff)
    return -1;

  if (ppos)
    *ppos = buff->play_cursor;
  if (wpos)
    *wpos = buff->write_cursor;

  return 0;
}

///////////////////////////////////////
// LnxSoundBuffer_SetCurrentPosition
///////////////////////////////////////
// Sets the current play position of the buffer
//
// Returns:
//        0 : no error
//       -1 : invalid parameters
int LnxSoundBuffer_SetCurrentPosition(LnxSoundBuffer *buff, unsigned int pos) {
  if (!buff)
    return -1;

  enter_critical();
  buff->play_cursor = pos;
  exit_critical();
  return 0;
}

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
                        unsigned int *numbytes1, void **ptr2, unsigned int *numbytes2, unsigned int flags) {
  if (!buff)
    return -1;

  if (flags & LNXSND_LOCK_FROMWRITECURSOR)
    pos += buff->write_cursor;
  if (flags & LNXSND_LOCK_ENTIREBUFFER)
    numbytes = buff->buffer_len;
  if (numbytes > buff->buffer_len)
    numbytes = buff->buffer_len;

  assert(numbytes1 != numbytes2);
  assert(ptr1 != ptr2);

  if (pos + numbytes <= buff->buffer_len) {
    *(unsigned char **)ptr1 = buff->buffer + pos;
    *numbytes1 = numbytes;
    if (ptr2)
      *(unsigned char **)ptr2 = NULL;
    if (numbytes2)
      *numbytes2 = 0;
  } else {
    *(unsigned char **)ptr1 = buff->buffer + pos;
    *numbytes1 = buff->buffer_len - pos;
    if (ptr2)
      *(unsigned char **)ptr2 = buff->buffer;
    if (numbytes2)
      *numbytes2 = numbytes - (buff->buffer_len - pos);
  }
  return 0;
}

///////////////////////////
// LnxSoundBuffer_Unlock
///////////////////////////
// Unlocks a buffer.
//
// Returns:
//        0 : no error
//       -1 : invalid parameters
int LnxSoundBuffer_Unlock(LnxSoundBuffer *buff, void *ptr1, unsigned int num1, void *ptr2, unsigned int num2) {
  if (!buff)
    return -1;

  return 0;
}

///////////////////////////////////////////
// Internal Sound System routines
//////////////////////////////////////////////////////////////

//	Starts up the sound processing thread
static bool StartupSoundSystem(LnxSoundDevice *dev) {
  SDL_AudioSpec spec;

  if (LnxNumBuffers < 1)
    return false;

  const int kDefaultSampleCount = 1024;
  int sampleCount = kDefaultSampleCount;
  int sampleArgIndex = FindArg("-sdlSndSizeMovie");
  if (sampleArgIndex == 0) {
    sampleArgIndex = FindArg("-sdlSndSize");
  }
  if (sampleArgIndex != 0) {
    const char *sampleCountStr = GetArg(sampleArgIndex + 1);
    if (sampleCountStr) {
      sampleCount = atoi(sampleCountStr);
      if (sampleCount <= 0) {
        sampleCount = kDefaultSampleCount;
      }
    }
  }

  memcpy(&LinuxSoundDevice, dev, sizeof(LnxSoundDevice));
  spec.freq = dev->freq;
  spec.format = dev->bit_depth == 8 ? AUDIO_U8 : AUDIO_S16SYS;
  spec.channels = dev->channels;
  spec.samples = sampleCount;
  spec.callback = LinuxSoundThreadHandler;

  if (SDL_OpenAudio(&spec, NULL) < 0) {
    return false;
  }
  SDL_PauseAudio(0);
  return true;
}

//	Shutsdown the sound processing thread
static void ShutdownSoundSystem(void) { SDL_CloseAudio(); }

static inline void GetValues(const LnxSoundBuffer *dsb, unsigned char *buf, unsigned int *fl, unsigned int *fr) {
  signed short *bufs = (signed short *)buf;

  // 8 bit stereo
  if ((dsb->wfx.wBitsPerSample == 8) && dsb->wfx.nChannels == 2) {
    *fl = (*buf - 128) << 8;
    *fr = (*(buf + 1) - 128) << 8;
    return;
  }

  // 16 bit stereo
  if ((dsb->wfx.wBitsPerSample == 16) && dsb->wfx.nChannels == 2) {
    *fl = *bufs;
    *fr = *(bufs + 1);
    return;
  }

  // 8 bit mono
  if ((dsb->wfx.wBitsPerSample == 8) && dsb->wfx.nChannels == 1) {
    *fl = (*buf - 128) << 8;
    *fr = *fl;
    return;
  }

  // 16 bit mono
  if ((dsb->wfx.wBitsPerSample == 16) && dsb->wfx.nChannels == 1) {
    *fl = *bufs;
    *fr = *bufs;
    return;
  }
  return;
}

static inline void SetValues(unsigned char *buf, unsigned int fl, unsigned int fr) {
  signed short *bufs = (signed short *)buf;

  // 8 bit stereo
  if ((LnxBuffers[0]->wfx.wBitsPerSample == 8) && (LnxBuffers[0]->wfx.nChannels == 2)) {
    *buf = (fl + 32768) >> 8;
    *(buf + 1) = (fr + 32768) >> 8;
    return;
  }

  // 16 bit stereo
  if ((LnxBuffers[0]->wfx.wBitsPerSample == 16) && (LnxBuffers[0]->wfx.nChannels == 2)) {
    *bufs = fl;
    *(bufs + 1) = fr;
    return;
  }

  // 8 bit mono
  if ((LnxBuffers[0]->wfx.wBitsPerSample == 8) && (LnxBuffers[0]->wfx.nChannels == 1)) {
    *buf = (((fl + fr) >> 1) + 32768) >> 8;
    return;
  }

  // 16 bit mono
  if ((LnxBuffers[0]->wfx.wBitsPerSample == 16) && (LnxBuffers[0]->wfx.nChannels == 1)) {
    *bufs = (fl + fr) >> 1;
    return;
  }
  return;
}

static void LinuxSoundMixWithVolume(LnxSoundBuffer *dsb, unsigned char *buf, unsigned int len) {
  unsigned int i, inc = (LnxBuffers[0]->wfx.wBitsPerSample >> 3);
  unsigned char *bpc = buf;
  signed short *bps = (signed short *)buf;

  if ((!(dsb->lbdesc.dwFlags & LNXSND_CAPS_CTRLPAN) || (dsb->pan == 0)) &&
      (!(dsb->lbdesc.dwFlags & LNXSND_CAPS_CTRLVOLUME) || (dsb->volume == 0)))
    return;

  for (i = 0; i < len; i += inc) {
    unsigned int val;
    switch (inc) {
    case 1: {
      val = *bpc - 128;
      val = ((val * (i & inc ? dsb->right_vol : dsb->left_vol)) >> 15);
      *bpc = val + 128;
      bpc++;
    } break;
    case 2: {
      val = *bps;
      val = ((val * ((i & inc) ? dsb->right_vol : dsb->left_vol)) >> 15);
      *bps = val;
      bps++;
    } break;
    }
  }
}

static unsigned int LinuxSoundMixNormalize(LnxSoundBuffer *dsb, unsigned char *buf, unsigned int len) {
  unsigned int i, size, ipos, ilen, fieldL = 0, fieldR = 0;
  unsigned char *ibp, *obp;
  unsigned int iAdvance = dsb->wfx.nBlockAlign;
  unsigned int oAdvance = LnxBuffers[0]->wfx.nBlockAlign;

  ibp = dsb->buffer + dsb->play_cursor;
  obp = buf;

  if ((dsb->freq == LnxBuffers[0]->wfx.nSamplesPerSec) &&
      (dsb->wfx.wBitsPerSample == LnxBuffers[0]->wfx.wBitsPerSample) &&
      (dsb->wfx.nChannels == LnxBuffers[0]->wfx.nChannels)) {
    if ((ibp + len) < (unsigned char *)(dsb->buffer + dsb->buffer_len))
      memcpy(obp, ibp, len);
    else {
      memcpy(obp, ibp, dsb->buffer_len - dsb->play_cursor);
      memcpy(obp + (dsb->buffer_len - dsb->play_cursor), dsb->buffer, len - (dsb->buffer_len - dsb->play_cursor));
    }
    return len;
  }

  if (dsb->freq == LnxBuffers[0]->wfx.nSamplesPerSec) {
    ilen = 0;
    for (i = 0; i < len; i += oAdvance) {
      GetValues(dsb, ibp, &fieldL, &fieldR);
      ibp += iAdvance;
      ilen += iAdvance;
      SetValues(obp, fieldL, fieldR);
      obp += oAdvance;
      if (ibp >= (unsigned char *)(dsb->buffer + dsb->buffer_len))
        ibp = dsb->buffer;
    }
    return (ilen);
  }

  size = len / oAdvance;
  ilen = ((size * dsb->freq_adjustment) >> FREQUENCY_SHIFT) * iAdvance;
  for (i = 0; i < size; i++) {
    ipos = (((i * dsb->freq_adjustment) >> FREQUENCY_SHIFT) * iAdvance) + dsb->play_cursor;

    if (ipos >= dsb->buffer_len)
      ipos %= dsb->buffer_len;

    GetValues(dsb, (dsb->buffer + ipos), &fieldL, &fieldR);
    SetValues(obp, fieldL, fieldR);
    obp += oAdvance;
  }
  return ilen;
}

int DoMulDiv(int nNumber, int nNumerator, int nDenominator) {
  if (!nDenominator)
    return -1;
  int64_t ret;
  ret = (((int64_t)nNumber * nNumerator) + (nDenominator / 2)) / nDenominator;

  if ((ret > 0x7FFFFFFF) || (ret < 0xFFFFFFFF))
    return -1;
  return ret;
}

static void *TempSoundBuffer = NULL;
static int TempSoundBufferLen = 0;
static unsigned int LinuxSoundMixInMainBuffer(LnxSoundBuffer *dsb, int len) {
  unsigned int i, ilen, advance = (LnxBuffers[0]->wfx.wBitsPerSample >> 3);
  unsigned char *buf, *ibuf, *obuf;
  signed int temp, field;
  signed short *ibufs, *obufs;

  if (!(dsb->flags & LNXSND_LOOPING)) {
    temp = DoMulDiv(LnxBuffers[0]->wfx.nAvgBytesPerSec, dsb->buffer_len, dsb->bps) -
           DoMulDiv(LnxBuffers[0]->wfx.nAvgBytesPerSec, dsb->play_cursor, dsb->bps);
    len = (len > temp) ? temp : len;
  }
  len &= ~3; // align to 4 byte boundary

  if (!len) {
    dsb->playing = 0;
    dsb->write_cursor = 0;
    dsb->play_cursor = 0;
    return 0;
  }

  if (len > TempSoundBufferLen) {
    void *nb = realloc(TempSoundBuffer, len);
    if (nb) {
      TempSoundBuffer = nb;
      TempSoundBufferLen = len;
      buf = ibuf = (unsigned char *)nb;
    } else {
      return 0;
    }
  } else {
    buf = ibuf = (unsigned char *)TempSoundBuffer;
  }

  ilen = LinuxSoundMixNormalize(dsb, ibuf, len);
  if ((dsb->lbdesc.dwFlags & LNXSND_CAPS_CTRLPAN) || (dsb->lbdesc.dwFlags & LNXSND_CAPS_CTRLVOLUME)) {
    LinuxSoundMixWithVolume(dsb, ibuf, len);
  }

  obuf = LnxBuffers[0]->buffer + LnxBuffers[0]->play_cursor;
  for (i = 0; i < len; i += advance) {
    obufs = (signed short *)obuf;
    ibufs = (signed short *)ibuf;
    if (LnxBuffers[0]->wfx.wBitsPerSample == 16) {
      field = *ibufs;
      field += *obufs;
      field = (field > 32767) ? (32767) : field;
      field = (field < -32768) ? (-32768) : field;
      *obufs = field;
    } else {
      field = (*ibuf - 128);
      field += (*obuf - 128);
      field = (field > 127) ? (127) : field;
      field = (field < -128) ? (-128) : field;
      *obuf = field + 128;
    }
    ibuf += advance;
    obuf += advance;
    if (obuf >= (unsigned char *)(LnxBuffers[0]->buffer + LnxBuffers[0]->buffer_len))
      obuf = LnxBuffers[0]->buffer;
  }

  // adjust positions of the cursors in the buffer
  dsb->play_cursor += ilen;
  dsb->write_cursor = dsb->play_cursor + ilen;

  if (dsb->play_cursor >= dsb->buffer_len) {
    if (!(dsb->flags & LNXSND_LOOPING)) {
      // we're not looping, this buffer is done, reset it
      dsb->playing = 0;
      dsb->write_cursor = 0;
      dsb->play_cursor = 0;
    } else {
      // loop back around
      dsb->play_cursor = dsb->play_cursor % dsb->buffer_len;
    }
  }

  if (dsb->write_cursor >= dsb->buffer_len) {
    dsb->write_cursor = dsb->write_cursor % dsb->buffer_len;
  }

  return len;
}

static void LinuxSoundMixBuffersIntoMain(int len) {
  LnxSoundBuffer *dsb;

  // only go to 1 since 0 is the main buffer
  for (int i = LnxNumBuffers - 1; i > 0; i--) {
    if (!(dsb = LnxBuffers[i]))
      continue;

    if (dsb->buffer_len && dsb->playing) {
      LinuxSoundMixInMainBuffer(dsb, len);
    }
  }
}

static void LinuxSoundThreadHandler(void *unused, Uint8 *stream, int len) {
  SDL_memset(stream, '\0', len);

  LnxBuffers[0]->buffer = stream;
  LnxBuffers[0]->buffer_len = len;
  LnxBuffers[0]->play_cursor = 0;
  LnxBuffers[0]->write_cursor = 0;

  LinuxSoundMixBuffersIntoMain(len);

  LnxBuffers[0]->buffer = NULL;
}
