/*
** mvelibl.cpp
**
** Interplay Movie File (MVE) Player Library (32-Bit Linux Version)
** Written by Paul Allen Edelstein. Partial Linux port by Jeff Slutter.
**
** (c) 1997 Interplay Productions.  All Rights Reserved.
** This file is confidential and consists of proprietary information
** of Interplay Productions.  This file and associated libraries
** may not, in whole or in part, be disclosed to third parties,
** incorporated into any software product which is not being created
** for Interplay Productions, copied or duplicated in any form,
** without the prior written permission of Interplay Productions.
** Further, you may not reverse engineer, decompile or otherwise
** attempt to derive source code of this material.
**

--- HISTORICAL COMMENTS FOLLOW ---

** Linux Specific Notes:
**   Only HiColor mode movies are supported.  Others could
**      be supported, but the nf*, etc. functions need to be
**      implemented to do so.  See mveasm.cpp.
**
**   No gfx or resolution changing functions are implemented in
**      here.
*/
static char notice1[] = "(c) 1997 Interplay Productions.  All Rights Reserved.\n"
                        "This file is confidential and consists of proprietary information\n"
                        "of Interplay Productions.  This file and associated libraries\n"
                        "may not, in whole or in part, be disclosed to third parties,\n"
                        "incorporated into any software product which is not being created\n"
                        "for Interplay Productions, copied or duplicated in any form,\n"
                        "without the prior written permission of Interplay Productions.\n"
                        "Further, you may not reverse engineer, decompile or otherwise\n"
                        "attempt to derive source code of this material.\n";

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "platform.h"
#include "mvelibi.h"
#include "mvegfx.h"
#include "mvelibl.h"

static unsigned opt_fastmode = 0; // 0:normal, 1:even lines only, 2:dither between even/odd lines |4 to spread lines

unsigned opt_hscale_step = 4; // 3: 4/3, 4:4/4, 5:4/5 (not fully implemented)
unsigned opt_hscale_adj;
extern unsigned sf_ScreenHeight;
extern unsigned sf_ScreenWidth;

#define logLabel(x)

#include "snd8to16.h"
// len always specifies length of destination in bytes.
unsigned sndDecompM16(uint16_t *dst, const uint8_t *src, unsigned len, unsigned state);
unsigned sndDecompS16(uint16_t *dst, const uint8_t *src, unsigned len, unsigned state);

//----------------------------------------------------------------------
// Memory Management
//--------------------

static void *(*mem_alloc)(unsigned size);
static void (*mem_free)(void *p);

struct MemRec {
  void *ptr;
  unsigned size;
  bool dynamic;
};

static void MemInit(MemRec* m, unsigned size, void *p);
static void *MemAlloc(MemRec* m, unsigned size);
static void MemFree(MemRec* m);

void MVE_memCallbacks(void *(*fn_alloc)(unsigned size), void (*fn_free)(void *p)) {
  mem_alloc = fn_alloc;
  mem_free = fn_free;
}

static void MemFree(MemRec* m) {
  if (m->dynamic && mem_free) {
    (*mem_free)(m->ptr);
    m->dynamic = false; // prevent from being freed again!
  }
  m->size = 0;
}

static void MemInit(MemRec* m, unsigned size, void *p) {
  if (!p)
    return;

  MemFree(m);
  m->ptr = p;
  m->size = size;
  m->dynamic = false;
}

static void *MemAlloc(MemRec* m, unsigned size) {
  if (size <= m->size)
    return m->ptr;

  if (mem_alloc) {
    void *p;
    MemFree(m);
    size += 100; // Add some pad to reduce chance of another realloc.
    p = (*mem_alloc)(size);
    if (!p)
      return (void *)NULL;
    MemInit(m, size, p);
    m->dynamic = true;
    return m->ptr;
  }

  return (void *)NULL;
}

//----------------------------------------------------------------------
// Synchronization Management
//-----------------------------

static bool sync_active = false;
static int sync_time = 0;
static int sync_wait_quanta = 0;

static bool sync_late = false;
static bool sync_FrameDropped = false;

static void syncReset(uint32_t wait_quanta);
static void syncRelease(void);
static bool syncInit(uint32_t period, unsigned wait_quanta);
static bool syncWait(void);
static void syncSync(void);

static void syncReset(uint32_t wait_quanta) {
  sync_time = wait_quanta - platform_timeGetTime() * 1000;
  sync_active = true;
}

static void syncRelease(void) { sync_active = false; }

static bool syncInit(uint32_t period, unsigned wait_quanta) {
  int new_wait_quanta = -(int32_t)(period * wait_quanta + (wait_quanta >> 1));

  // If timer is still running and has same timing
  // characteristics, assume we are trying to continue smoothly
  // with another movie and ignore new syncInit() call.
  if (sync_active && sync_wait_quanta == new_wait_quanta)
    return true;

  syncWait();
  sync_wait_quanta = new_wait_quanta;
  syncReset(sync_wait_quanta);
  return true;
}

// Returns true if actually waited, false if called too late to wait.
static bool syncWait(void) {
  bool waited = false;
  if (!sync_active)
    return false;

  while ((int)(sync_time + platform_timeGetTime() * 1000) < 0) {
    waited = true;
  }

  sync_time += sync_wait_quanta;
  return waited;
}

// Returns true if actually waited, false if called too late to wait.
static int syncWaitLevel(int level) {
  int waited;
  if (!sync_active)
    return 0;

  level += sync_time;

  for (;;) {
    waited = level + platform_timeGetTime() * 1000;
    if (waited >= 0)
      break;
  }

  sync_time += sync_wait_quanta;
  return waited;
}

static void syncSync(void) {
  if (sync_active) {
    while ((int)(sync_time + platform_timeGetTime() * 1000) < 0)
      ;
  }
}

int MVE_syncTime(void) {
  if (sync_active)
    return sync_time + platform_timeGetTime() * 1000;
  return 0;
}

void MVE_logDumpStats(void) {}

//----------------------------------------------------------------------
// I/O Management
//-----------------

static unsigned (*io_read)(int handle, void *buf, unsigned count);

static MemRec io_mem_buf;

static int io_handle;
static ioHdrRec io_next_hdr;

static bool ioReset(int h);
static uint8_t *ioRead(unsigned qty);
static uint8_t *ioNextRecord(void);
static void ioRelease(void);

void MVE_ioCallbacks(unsigned (*fn_read)(int handle, void *buf, unsigned count)) { io_read = fn_read; }

static bool ioReset(int h) {
  mve_hdr *hdr;

  io_handle = h;
  hdr = (mve_hdr *)ioRead(sizeof(mve_hdr) + sizeof(ioHdrRec));
  if (!hdr)
    return false;
  hdr->SwapBytes();
  if (strcmp(hdr->FileType, MVE_FILE_TYPE) != 0 || hdr->id != ~hdr->version + 0x1234 ||
      // The following two checks may eventually be weakened.
      hdr->version != MVE_FILE_VERSION || hdr->HdrSize != sizeof(mve_hdr))
    return false;

  io_next_hdr = *(ioHdrRec *)(hdr + 1);
  io_next_hdr.SwapBytes();
  return true;
}

void MVE_memIO(void *p, unsigned size) { MemInit(&io_mem_buf, size, p); }

static uint8_t *ioRead(unsigned len) {
  uint8_t *buf;

  buf = (uint8_t *)MemAlloc(&io_mem_buf, len);
  if (!buf)
    return (uint8_t *)NULL;

  if (!(*io_read)(io_handle, buf, len))
    return (uint8_t *)NULL;

  return buf;
}

static uint8_t *ioNextRecord(void) {
  uint8_t *buf;
  logLabel("StartRead");
  buf = ioRead(io_next_hdr.len + sizeof(ioHdrRec));
  logLabel("EndRead");
  if (!buf)
    return (uint8_t *)NULL;
  io_next_hdr = *(ioHdrRec *)(buf + io_next_hdr.len);
  io_next_hdr.SwapBytes();
  return buf;
}

static void ioRelease(void) { MemFree(&io_mem_buf); }

//----------------------------------------------------------------------
// Sound Management
//-----------------------

#define SOUND_SUPPORT 1

#if SOUND_SUPPORT

static ISoundDevice *snd_ds = NULL;
static ISysSoundBuffer *snd_buffer = NULL;
static SysSoundCaps snd_buffer_caps;
static uint32_t snd_write_cursor = 0;

enum { snd_queue_max = 60 };

static struct _snd_queue {
  unsigned ptr;
} snd_queue[snd_queue_max];
static unsigned snd_fill = 0;
static unsigned snd_empty = 0;
static int snd_pad = 0;
static unsigned snd_stereo = 0;
static unsigned snd_comp16 = 0;
static unsigned snd_bits16 = 0;
static int32_t snd_volume = 0;
static int32_t snd_pan = 0;

#endif

void MVE_sndInit(ISoundDevice *lpDS) {
#if SOUND_SUPPORT
  snd_ds = lpDS;
#endif
}

void MVE_dsbSetVolume(int32_t lVolume) {
#if SOUND_SUPPORT
  snd_volume = lVolume;
  if (snd_buffer) {
    snd_buffer->SetVolume(snd_volume);
  }
#endif
}

void MVE_dsbSetPan(int32_t lPan) {
#if SOUND_SUPPORT
  snd_pan = lPan;
  if (snd_buffer) {
    snd_buffer->SetPan(snd_pan);
  }
#endif
}

static void sndReset(void) {
#if SOUND_SUPPORT
  if (snd_buffer) {
    snd_buffer->Stop();
    snd_buffer->Release();
    snd_buffer = NULL;
  }
#endif
}

static bool sndConfigure(unsigned rate, unsigned buflen, unsigned stereo, unsigned frequency, unsigned bits16,
                         unsigned comp16) {
#if SOUND_SUPPORT
  if (!snd_ds)
    return true;

  syncSync();
  sndReset();

  snd_stereo = stereo;
  snd_bits16 = bits16;
  snd_comp16 = comp16;
  snd_fill = 0;
  snd_empty = 0;

  SoundWAVEFormatEx snd_wfx;
  snd_wfx.wFormatTag = SOUND_WAVE_FORMAT_PCM;
  snd_wfx.nChannels = stereo ? 2 : 1;
  snd_wfx.nSamplesPerSec = frequency;
  snd_wfx.nBlockAlign = (stereo ? 2 : 1) * (bits16 ? 2 : 1);
  snd_wfx.nAvgBytesPerSec = frequency * snd_wfx.nBlockAlign;
  snd_wfx.wBitsPerSample = bits16 ? 16 : 8;

  SysSoundBufferDesc snd_bufferdesc;
  snd_bufferdesc.dwFlags = LNXSND_CAPS_CTRLDEFAULT;
  snd_bufferdesc.dwBufferBytes = (buflen + (buflen >> 1)) & ~3;
  snd_bufferdesc.lpwfxFormat = &snd_wfx;

  int dsrval = snd_ds->CreateSoundBuffer(&snd_bufferdesc, &snd_buffer);
  if (dsrval != 0)
    return false;

  snd_buffer->SetVolume(snd_volume);
  snd_buffer->SetPan(snd_pan);

  snd_write_cursor = 0;

  dsrval = snd_buffer->GetCaps(&snd_buffer_caps);
  if (dsrval != 0)
    return false;
#endif

  return true;
}

static void sndSync(void) {
  // Better frame dropping using more flexible synchronization
  sync_late = syncWaitLevel(sync_wait_quanta >> 2) > (-sync_wait_quanta >> 1) && !sync_FrameDropped;
  sync_FrameDropped = false;

#if SOUND_SUPPORT
  if (!snd_ds || !snd_buffer)
    return;

  uint32_t dsbstatus;
  uint32_t target;
  uint32_t play_cursor, write_cursor;
#define set_target(t)                                                                                                  \
  (target = (snd_queue[snd_empty].ptr + (t) + snd_buffer_caps.dwBufferBytes) % snd_buffer_caps.dwBufferBytes)
#define target_pending()                                                                                               \
  ((play_cursor <= write_cursor) ? (play_cursor <= target && target < write_cursor)                                    \
                                 : (play_cursor <= target || target < write_cursor))

  bool need_resync = false;
  for (;;) {
    int dsrval = snd_buffer->GetStatus(&dsbstatus);
    if (dsrval != 0)
      return;

    dsrval = snd_buffer->GetCurrentPosition(&play_cursor, &write_cursor);
    if (dsrval != 0)
      return;
    write_cursor = snd_write_cursor;

    // Don't get too far ahead of sound (target-pad not yet played)
    set_target(-snd_pad);
    if (target_pending() && (dsbstatus & LNXSND_PLAYING)) {
      need_resync = true;
    } else {
      break;
    }
  }

  if (need_resync) {
    syncReset(sync_wait_quanta + (sync_wait_quanta >> 2));
  }

  if (!(dsbstatus & LNXSND_PLAYING)) // If currently not playing
  {
    // Don't restart too soon (restart when target hasn't been played)
    set_target(0);
    if (target_pending()) {
      int dsrval = snd_buffer->SetCurrentPosition(target);
      if (dsrval != 0)
        return;

      dsrval = snd_buffer->Play(LNXSND_LOOPING);
      if (dsrval != 0)
        return;

      syncReset(sync_wait_quanta);
    }
  }
  // Because DirectSound can consume an unpredictable amount into primary, this won't always be invoked when it should
  else {
    // Don't get too far behind sound (has target+pad already been played?)
    set_target(snd_pad);
    {
      int amt = write_cursor - play_cursor;
      if (amt < 0)
        amt += snd_buffer_caps.dwBufferBytes;
      amt = snd_buffer_caps.dwBufferBytes - amt - 1;
      if (amt > (int)snd_buffer_caps.dwBufferBytes / 2) {
        amt = snd_buffer_caps.dwBufferBytes / 2;
      }
      play_cursor = (play_cursor - amt + snd_buffer_caps.dwBufferBytes) % snd_buffer_caps.dwBufferBytes;
    }

    if (!target_pending()) {
      snd_buffer->Stop();
      // dsrval = snd_buffer->GetCurrentPosition( &snd_write_cursor, &write_cursor );
    }
  }

  if (snd_empty != snd_fill) {
    if (snd_empty == snd_queue_max - 1) {
      snd_empty = 0;
    } else {
      ++snd_empty;
    }
  }
#endif
}

// For compressed streams, assumes len (which is in bytes) will be in multiples
//  of 2 for mono and 4 for stereo.
static unsigned sndAddHelper(uint8_t *dst, const uint8_t **pSrc, unsigned len, unsigned state, bool init) {
#if SOUND_SUPPORT
  const uint8_t *src;
  src = *pSrc;
  if (!src) {
    memset(dst, (snd_bits16 ? 0 : 0x80), len);
  } else {
    if (snd_comp16) {
      if (!snd_stereo) {
        if (init) {
          state = IntelSwapper(*(uint16_t *)src);
          *(uint16_t *)dst = state;
          src += 2;
          dst += 2;
          len -= 2;
        }

        state = sndDecompM16((uint16_t *)dst, src, len >> 1, state);

        src += len >> 1;
      } else {
        if (init) {
          state = IntelSwapper(*(uint32_t *)src);
          *(uint32_t *)dst = state;
          src += 4;
          dst += 4;
          len -= 4;
        }
        state = sndDecompS16((uint16_t *)dst, src, len >> 2, state);
        src += len >> 1;
      }
    } else {
      memcpy(dst, src, len);
      src += len;
    }
  }

  *pSrc = src;
  return state;
#else
  return 0;
#endif
}

static void sndAdd(const uint8_t *buf, unsigned len) {
#if SOUND_SUPPORT
  int dsrval;
  uint32_t play_cursor, write_cursor;
  uint32_t len1, len2;

  unsigned state = 0;
  bool init = true;
  uint8_t *ptr1, *ptr2;

  snd_pad = len;

  if (!snd_buffer || snd_fill + 1 == (snd_empty ? snd_empty : snd_queue_max))
    return;

  dsrval = snd_buffer->GetCurrentPosition(&play_cursor, &write_cursor);
  if (dsrval != 0)
    return;
  write_cursor = snd_write_cursor;

  dsrval = snd_buffer->Lock(write_cursor, len, (void **)&ptr1, &len1, (void **)&ptr2, &len2,
                            0 /*LNXSND_LOCK_FROMWRITECURSOR*/ /*flags*/);
  if (dsrval != 0)
    return;

  if (len1) {
    state = sndAddHelper(ptr1, &buf, len1, state, init);
    init = false;
    snd_write_cursor += len1;
  }

  if (len2) {
    sndAddHelper(ptr2, &buf, len2, state, init);
    snd_write_cursor = len2;
  }

  if (snd_write_cursor == snd_buffer_caps.dwBufferBytes) {
    snd_write_cursor = 0;
  }

  snd_buffer->Unlock(ptr1, len1, ptr2, len2);

  snd_queue[snd_fill].ptr = write_cursor;
  if (snd_fill == snd_queue_max - 1) {
    snd_fill = 0;
  } else {
    ++snd_fill;
  }
#endif
}

static void sndRelease(void) {
#if SOUND_SUPPORT
  // Nothing to free
#endif
}

static void sndPause(void) {
#if SOUND_SUPPORT
  if (snd_buffer) {
    snd_buffer->Stop();
  }
#endif
}

static void sndResume(void) {
  // Nothing need be done here to resume sound
  // The next call to sndSync will automatically resume the sound.
}

//--------------------------------------------------------------------
// NextFrame (Video Decompression)
//----------------------------------

// static removed from most nf_ vars to support mveliba.asm

static bool nf_memory_mode = false;

// NextFrame working storage
uint8_t *nf_buf_cur = NULL;
uint8_t *nf_buf_prv = NULL;
static MemRec nf_mem_buf1;
static MemRec nf_mem_buf2;

// NextFrame parameters
uint8_t nf_wqty = 0; // (width/SWIDTH)
uint8_t nf_hqty = 0; // (height/SHEIGHT)
uint8_t nf_fqty = 0; // Number of fields
unsigned nf_hicolor = 0;   // HiColor (0:none,1:normal,2:swapped)
//  <derived quantities>
unsigned nf_width = 0;      // wqty * SWIDTH
unsigned nf_height = 0;     // hqty * SHEIGHT;
unsigned nf_new_line = 0;   // width - SWIDTH
unsigned nf_new_row0 = 0;   // SHEIGHT*width*2-width
unsigned nf_back_right = 0; // (SHEIGHT-1)*width

// Frame parameters
//  Portion of current frame which has been updated
//  and needs to be sent to screen.
//
unsigned nf_new_x = 0;
unsigned nf_new_y = 0;
unsigned nf_new_w = 0;
unsigned nf_new_h = 0;

// Hicolor format translation tables
uint16_t nf_trans16_lo[256];
uint16_t nf_trans16_hi[256];

void MVE_memVID(void *p1, void *p2, unsigned size) {
  MemInit(&nf_mem_buf1, size, p1);
  MemInit(&nf_mem_buf2, size, p2);
}

void nfPkConfig(void);

// ffs()
// Returns position of most significant bit set (0 to 31).
// Assumes bits is nonzero.
static int ffs(unsigned bits) {
  int pos;
  unsigned t;
  t = bits & 0xFFFF0000;
  if (t)
    bits = t, pos = 16;
  else
    pos = 0;
  t = bits & 0xFF00FF00;
  if (t)
    bits = t, pos |= 8;
  t = bits & 0xF0F0F0F0;
  if (t)
    bits = t, pos |= 4;
  t = bits & 0xCCCCCCCC;
  if (t)
    bits = t, pos |= 2;
  if (bits & 0xAAAAAAAA)
    pos |= 1;
  return pos;
}

static bool nfConfig(int wqty, int hqty, int fqty, int hicolor) {
  unsigned size;

  if (!nf_memory_mode) {
    if (nf_buf_cur) {
      free(nf_buf_cur);
      nf_buf_cur = NULL;
    }
    if (nf_buf_prv) {
      free(nf_buf_prv);
      nf_buf_prv = NULL;
    }
  }

  nf_wqty = (uint8_t)wqty;
  nf_hqty = (uint8_t)hqty;
  nf_fqty = (uint8_t)fqty;
  nf_width = wqty * SWIDTH;
  nf_height = hqty * fqty * SHEIGHT;
  if (opt_fastmode) {
    nf_height >>= 1;
  }

  {
    if (hicolor) {
      const uint32_t pal_rmask = 0x001F;
      const uint32_t pal_bmask = 0xF800;
      const uint32_t pal_gmask = 0x07E0;

      const int pal_rshift = ffs(pal_rmask) - 4;
      const int pal_gshift = ffs(pal_gmask) - 4;
      const int pal_bshift = ffs(pal_bmask) - 4;
      unsigned i, r, g, b;
      for (i = 0, r = 0, g = 0; g < 8; ++g) {
        for (b = 0; b < 32; ++b, ++i) {
          nf_trans16_lo[i] = (((pal_rshift > 0 ? r << pal_rshift : r >> -pal_rshift) & pal_rmask) |
                              ((pal_gshift > 0 ? g << pal_gshift : g >> -pal_gshift) & pal_gmask) |
                              ((pal_bshift > 0 ? b << pal_bshift : b >> -pal_bshift) & pal_bmask));
        }
      }

      for (i = 0, r = 0, b = 0; r < 32; ++r) {
        for (g = 0; g < 32; g += 8, ++i) {
          nf_trans16_hi[i + 128] = nf_trans16_hi[i] =
              (((pal_rshift > 0 ? r << pal_rshift : r >> -pal_rshift) & pal_rmask) |
               ((pal_gshift > 0 ? g << pal_gshift : g >> -pal_gshift) & pal_gmask) |
               ((pal_bshift > 0 ? b << pal_bshift : b >> -pal_bshift) & pal_bmask));
        }
      }
    }
    int size = nf_width * nf_height << 1;

    nf_buf_cur = (uint8_t *)malloc(size);
    nf_buf_prv = (uint8_t *)malloc(size);
  }

  nf_new_line = nf_width * fqty - SWIDTH;
  nf_hicolor = hicolor;
  if (hicolor) {
    nf_width <<= 1;
    nf_new_line <<= 1;
  }

  nf_new_row0 = fqty * SHEIGHT * nf_width;
  nf_back_right = fqty * (SHEIGHT - 1) * nf_width;
  size = nf_width * nf_height;

  nfPkConfig();

  return true;
}

static void nfRelease(void) {
  free(nf_buf_cur);
  free(nf_buf_prv);
  nf_buf_cur = NULL;
  nf_buf_prv = NULL;
}

static void nfAdvance(void) {
  uint8_t *tmp = nf_buf_prv;
  nf_buf_prv = nf_buf_cur;
  nf_buf_cur = tmp;
}

// Decompress into subsection of current buffer specified
//  by x,y,w,h in units of SWIDTHxSHEIGHT (8x8).
//
void nfHiColorDecomp(const uint8_t *comp, unsigned x, unsigned y, unsigned w, unsigned h);

// Decompress into subsection of current buffer specified
//  by x,y,w,h in units of SWIDTHxSHEIGHT (8x8).
// Chgs specifies which squares to update.
// Parms are motion parms for squares to update.
//
void nfHiColorDecompChg(const uint16_t *chgs, const uint16_t *parms, const uint8_t *comp, unsigned x,
                        unsigned y, unsigned w, unsigned h);

// Non-HiColor versions

// Decompress into subsection of current buffer specified
//  by x,y,w,h in units of SWIDTHxSHEIGHT (8x8).
//
void nfDecomp(const uint8_t *comp, unsigned x, unsigned y, unsigned w, unsigned h);

void nfPkDecomp(uint8_t *ops, uint8_t *comp, unsigned x, unsigned y, unsigned w, unsigned h);
void nfPkDecompH(uint8_t *ops, uint8_t *comp, unsigned x, unsigned y, unsigned w, unsigned h);

void nfHPkDecomp(uint8_t *ops, uint8_t *comp, unsigned x, unsigned y, unsigned w, unsigned h);

// Decompress into subsection of current buffer specified
//  by x,y,w,h in units of SWIDTHxSHEIGHT (8x8).
// Chgs specifies which squares to update.
// Parms are motion parms for squares to update.
//
void nfDecompChg(const uint16_t *chgs, const uint16_t *parms, const uint8_t *comp, unsigned x,
                 unsigned y, unsigned w, unsigned h);

//---------------------------------------------------------------------
// ShowFrame
//------------

// TODO:
//  Support for software cursor.
//  Issues & Considerations:
//    1.  Current code only deals with writing to screen, not reading it.
//    2.  To prevent flicker, cursor should be drawn into current buffer
//	  before it is sent to screen, first saving area overwritten.
//	  After current before is sent to screen, the area modified
//	  by the cursor in the current buffer should be restored.
//    3.  Screen must also be updated in outside areas if cursor
//	  appears there.  This requires read/modify/write to screen,
//	  because current buffer may not exist or be valid for area
//	  of screen.  Also, contents of screen must be saved so that
//	  if next frame doesn't modify that area of screen, it can be
//	  restored when mouse moves away.
//    In other words:
//    (a):
//	Save area on screen where mouse will go.
//	Draw mouse onto screen.
//	When mouse moves, restore previous contents of screen and
//	 loop back to (a).
//	When screen is to be redrawn with video, draw mouse clipped
//	 into buffer, saving previous contents.  Draw video, then restore
//	 buffer.  Note that saving previous contents saves into same
//	 area that was used when area under mouse was originally saved,
//	 but it may only be a subrectangle.
//  Question: Should I implement VESA read from screen code?
//	      Or work with caller to implement?
//	With caller:
//	  Caller provides pointer to mouse image and save area buffers,
//	  and screen location.	Caller informs us when mouse changes
//	  (position or contents).  We deal with drawing mouse into internal
//	  buffer and updating save area.  Caller deals with drawing
//	  mouse on screen and saving areas from screen.  Color zero will
//	  be assumed transparent.  If old and new locations are within
//	  area we are about to draw, caller need do nothing other than
//	  call us (we can return a bool to let him know that).	We can
//	  call him to draw cursor just before showframe (whereas
//	  call back from pausing would be just after showframe).
//  Without special support?
//    Everytime frame is shown, redraw mouse on screen, saving previous
//    contents.	If mouse moves, restore previous contents and redraw.
//    Result will be a flickering mouse (mostly on, briefly off).

static mve_cb_ShowFrame sf_ShowFrame = NULL;

static unsigned sf_ResolutionHeight = 0; // Height of screen
static unsigned sf_ResolutionWidth = 0;  // Width of screen
unsigned sf_ScreenHeight = 0;            // Height of modifiable screen
unsigned sf_ScreenWidth = 0;             // Width of modifiable screen
// Private, see mveliba.asm :
unsigned sf_LineWidth = 0;      // Distance between lines in memory
static unsigned sf_hicolor = 0; // Hicolor mode (0:none,1:normal,2:swapped)

// Banked screen parameters, Private, see mveliba.asm
void *sf_SetBank = NULL;
unsigned sf_WinGran = 0;
uint32_t sf_WinSize = 0;
unsigned sf_WinGranPerSize = 0;
//{sf_WriteWinPtr and sf_WriteWinLimit replace sf_WriteWinSeg, see mveliba.asm}
uint8_t *sf_WriteWinPtr = NULL;
uint8_t *sf_WriteWinLimit = NULL;
unsigned sf_WriteWin = 0;

//
static bool sf_auto = true;  // True if mode can be set from movie
                             //  (mode not specified by caller).
static int sf_auto_mode = 0; // Current sf_auto mode.

static void sfVGA(unsigned w, unsigned h, unsigned resw, unsigned resh);
static void sfShowFrame(int dx, int dy, unsigned field);

void mve_ShowFrameField(uint8_t *buf, unsigned bufw, unsigned bufh, unsigned sx, unsigned sy, unsigned w,
                        unsigned h, unsigned dstx, unsigned dsty, unsigned field);
void mve_ShowFrameField(uint8_t *buf, unsigned bufw, unsigned bufh, unsigned sx, unsigned sy, unsigned w,
                        unsigned h, unsigned dstx, unsigned dsty, unsigned field);

void mve_ShowFrameFieldHi(uint8_t *buf, unsigned bufw, unsigned bufh, unsigned sx, unsigned sy, unsigned w,
                          unsigned h, unsigned dstx, unsigned dsty, unsigned field);

// Restrictions/Assumptions:
//    64K >= WinSize >= WinGran
//    WinSize % WinGran == 0
//    WinGran of 64K is represented by 0
//    SetBank is address of function with following protocol:
//	bh:  0=Set window, 1=Get Window
//	bl:  Window number (0 or 1)
//	dx:  Window position in video memory in units of WinGran.
//     on return, registers AX and DX are destroyed.
void MVE_sfSVGA(unsigned w, unsigned h, unsigned LineWidth, unsigned WriteWin, uint8_t *WriteWinPtr,
                uint32_t WinSize, unsigned WinGran, void *SetBank, unsigned hicolor) {
  sf_ScreenWidth = w;
  sf_ScreenHeight = h;
  sf_ResolutionWidth = w;
  sf_ResolutionHeight = h;
  sf_LineWidth = LineWidth;
  if (opt_fastmode & 4) {
    sf_LineWidth <<= 1;
  }
  sf_WriteWin = WriteWin;
  sf_WinSize = WinSize;
  sf_WriteWinPtr = WriteWinPtr;
  sf_WriteWinLimit = sf_WriteWinPtr + sf_WinSize;
  sf_WinGran = WinGran;
  sf_SetBank = SetBank;
  if (WinGran) {
    // Assumes WinGran divides evenly into WinSize.
    sf_WinGranPerSize = (unsigned)WinSize / WinGran;
  } else {
    // Assumes WinSize is also 64K.
    sf_WinGranPerSize = 1;
  }
  sf_auto = false;
  sf_hicolor = hicolor;
}

// QUESTION: Should sfShowFrame also take x,y,w,h as command args?
//  The issue is, will it always be true that area to update on
//  screen matches area updated in memory, since previous contents
//  of memory will be from two frames back, not one!
//  Answer:  By having compressor compare previous screen to desired screen,
//   areas that are identical can be located.  Only areas that change
//   need to be built in memory.  Remaining areas will not be correct,
//   but can still be used for source data for next screen.
//   Therefore, additional x,y,w,h args are not needed.
//   However, should this prove to be wrong, a minor opcode variant
//   can always be added which supplies the additional arguments.
static void sfShowFrame(int dx, int dy, unsigned field) {
  unsigned scaled_width = nf_width * 4 / opt_hscale_step;
  scaled_width = ((scaled_width - 12) & ~0xf) + 12; // Round down to a multiple of 16 + 12.
  opt_hscale_adj = nf_width - (scaled_width / 4 * opt_hscale_step);

  logLabel("StartShow");
  if (dx < 0) {
    if (nf_hicolor) {
      dx = (sf_ScreenWidth - (scaled_width >> 1)) >> 1;
    } else {
      dx = (sf_ScreenWidth - scaled_width) >> 1;
    }
  }

  if (nf_hicolor) {
    dx <<= 1;
  }

  if (dy < 0) {
    if (opt_fastmode & 4) // HACK
    {
      dy = (sf_ScreenHeight - nf_height * 2) >> 1;
    } else {
      dy = (sf_ScreenHeight - nf_height) >> 1;
    }
  }

  dx &= ~3; // Force to a multiple of 4 boundary for performance!

  if (opt_fastmode & 4) // HACK
  {
    dy >>= 1;
  }

  if (field) {
    mve_ShowFrameField(nf_buf_cur, nf_width, nf_height, nf_new_x, nf_new_y, nf_new_w, nf_new_h, dx, dy, field);
  } else {
    if (opt_hscale_step != 4) {
      sf_ShowFrame(nf_buf_cur, nf_width, nf_height, 0, nf_new_y, scaled_width, nf_new_h, dx, dy, nf_hicolor);
    } else {
      sf_ShowFrame(nf_buf_cur, nf_width, nf_height, nf_new_x, nf_new_y, nf_new_w, nf_new_h, dx, dy, nf_hicolor);
    }
  }

  logLabel("EndShow");
}

void MVE_sfCallbacks(mve_cb_ShowFrame fn_ShowFrame) { sf_ShowFrame = fn_ShowFrame; }

// Restriction: w must be a multiple of 4.
// Strong Recommendation: sx and dstx should be multiples of 4.
// dstx & dsty are deltas relative to sx & sy.
// <WIP> Field is a hack and should be removed.  Instead, nfConfig
//   should specify interlace mode and cause linestep to be twice
//   sf_LineWidth.  dstx/dsty should be made absolute to allow interlace
//   field to be determined by dst.
//   ON THE OTHER HAND -- All this makes user clipping quite complex...
//     A caller would probably like to deal in ordinary coordinates,
//     but then we still need 'field'.
//     Also note that when field is on, the actual height of the image
//     on the screen is 2*h alternate lines.
//

void mve_sfHiColorShowFrameChg(bool prvbuf, unsigned x, unsigned y, unsigned w, unsigned h, uint16_t *chgs,
                               unsigned dstx, unsigned dsty);

void mve_sfShowFrameChg(bool prvbuf, unsigned x, unsigned y, unsigned w, unsigned h, uint16_t *chgs,
                        unsigned dstx, unsigned dsty);

static void sfShowFrameChg(int dx, int dy, uint16_t *chgs) { logLabel("StartShowChg"); }

//---------------------------------------------------------------------
// Palette Management
//---------------------

static void (*pal_SetPalette)(uint8_t *p, unsigned start, unsigned count) = MVE_SetPalette;

uint8_t pal_tbl[3 * 256]; // Private, see mveliba.asm
#if DBL_DBG
uint8_t pal_tbl_old[3 * 256];
#endif
uint16_t pal15_tbl[256]; // Private, see mveliba.asm

void MVE_palCallbacks(void (*fn_SetPalette)(uint8_t *p, unsigned start, unsigned count)) {
  pal_SetPalette = fn_SetPalette;
}

void MVE_SetPalette(uint8_t *p, unsigned start, unsigned count);

static void palSetPalette(unsigned start, unsigned count) {
  if (!nf_hicolor) {
    (*pal_SetPalette)((uint8_t *)pal_tbl, start, count);
  }
}

static void palClrPalette(unsigned start, unsigned count) {
  static uint8_t clr_pal_tbl[256 * 3];
  if (!nf_hicolor) {
    (*pal_SetPalette)((uint8_t *)clr_pal_tbl, start, count);
  }
}

// Old Synth: 0,21,6
//	      128,32,4
//
// New Synth: 129,21,6
//	      17,28,4
//
static void palMakeSynthPalette(int base_r, int range_r, int range_rb, int base_g, int range_g, int range_gb) {
  uint8_t(*SynthPal)[3] = (uint8_t(*)[3])pal_tbl;
  int i, j;

  for (i = 0; i < range_r; ++i) {
    for (j = 0; j < range_rb; ++j) {
      int idx = base_r + i * range_rb + j;
      SynthPal[idx][0] = (char)(i * 63 / (range_r - 1));
      SynthPal[idx][1] = 0;
      SynthPal[idx][2] = (char)((j * 63 / (range_rb - 1)) * 5 / 8);
    }
  }

  for (i = 0; i < range_g; ++i) {
    for (j = 0; j < range_gb; ++j) {
      int idx = base_g + i * range_gb + j;
      SynthPal[idx][0] = 0;
      SynthPal[idx][1] = (char)(i * 63 / (range_g - 1));
      SynthPal[idx][2] = (char)((j * 63 / (range_gb - 1)) * 5 / 8);
    }
  }
}

static void palLoadPalette(uint8_t *buf, unsigned start, unsigned count) {
#if DBL_DBG
  memcpy(pal_tbl_old, pal_tbl, 256 * 3);
#endif
  memcpy(&pal_tbl[start * 3], buf, count * 3);
}

// If at least 11 palette entries aren't changed, this is more compact
//  than uncompressed 256 entry palette.
//
void palLoadCompPalette(uint8_t *buf);

//-----------------------------------------------------------------------
// Graphics
//----------

void gfxVres(uint8_t misc, uint8_t *crtc);

static void ForceVres2(uint8_t misc, unsigned vt, unsigned height, unsigned vrs /*vpos*/, unsigned _2t4,
                       unsigned msl, unsigned bdr_b, unsigned bdr_t) {}

void MVE_ForceVres350(void) {}

void MVE_ForceVresHalf(void) {}

// If auto is set, and this function is being called to change mode
//  for movie, be sure to turn auto back on after call!
//
bool MVE_gfxMode(int16_t mode) { return false; }

// Restore a text mode
void MVE_gfxReset(void) {
  sf_auto = true;
  sf_auto_mode = 0;
}

//----------------------------------------------------------------------
// Movie Interpreter
//--------------------

static int (*rm_ctl)(void);

static int rm_dx = 0;
static int rm_dy = 0;
static unsigned rm_track_bit = 0;
static uint8_t *rm_p = NULL;
static unsigned rm_len = 0;
static bool rm_hold = false;
static bool rm_active = false;
static unsigned rm_FrameCount = 0;
static unsigned rm_FrameDropCount = 0;

void MVE_rmCallbacks(int (*fn_ctl)(void)) { rm_ctl = fn_ctl; }

void MVE_rmFastMode(int mode) { opt_fastmode = mode; }

void MVE_rmHScale(int hscale) {
  if (hscale == 3)
    opt_hscale_step = 3;
  else
    opt_hscale_step = 4;
}

void MVE_rmFrameCounts(unsigned *FrameCount, unsigned *FrameDropCount) {
  *FrameCount = rm_FrameCount;
  *FrameDropCount = rm_FrameDropCount;
}

int MVE_rmPrepMovie(int hFile, int dx, int dy, unsigned track) {
  rm_dx = dx;
  rm_dy = dy;

  rm_track_bit = 1 << track;
  if (!rm_track_bit)
    rm_track_bit = 1;

  if (!ioReset(hFile)) {
    MVE_rmEndMovie();
    return MVE_ERR_BADFMT;
  }

  rm_p = ioNextRecord();
  rm_len = 0;
  if (!rm_p) {
    MVE_rmEndMovie();
    return MVE_ERR_IO;
  }

  rm_active = true;
  rm_hold = false;
  rm_FrameCount = 0;
  rm_FrameDropCount = 0;
  return 0;
}

int MVE_rmHoldMovie(void) {
  if (!rm_hold) {
    sndPause();
    rm_hold = true;
  }
  syncWait();
  return 0;
}

int dbgcount = 0;
int MVE_rmStepMovie(void) {
  unsigned len = rm_len;
  uint8_t *p = rm_p;

  int result;

  if (!rm_active)
    return MVE_ERR_PREP;

  if (rm_hold) {
    sndResume();
    rm_hold = false;
  }

  for (;; p = ioNextRecord(), len = 0) {
    uint16_t *DecompChg_chgs = (uint16_t *)NULL;
    uint16_t *DecompChg_parms = (uint16_t *)NULL;

    if (!p) {
      result = MVE_ERR_IO;
      goto done;
    }

    for (;;) {
      mcmd_hdr hdr;
      p += len; // Advance past data of previous command
      hdr = *(mcmd_hdr *)p;
      hdr.SwapBytes();
      p += sizeof(hdr);
      len = hdr.len;
      /*
                              FILE *logf = fopen("mve.log","a");
                              if(logf)
                              {
                                      fprintf(logf,"hdr len=%d,major=%d,minor=%d\n",hdr.len,hdr.major,hdr.minor);
                                      fclose(logf);
                              }

                              if(hdr.len==14896 &&
                                      hdr.major==17 &&
                                      hdr.minor==3)
                              {
                                      dbgcount++;
                                      if(dbgcount >= 3)
                                      {
                                              printf("***");
                                      }

                              }
      */
      switch (hdr.major) {
      default:
        continue;

      case mcmd_end:
        result = MVE_ERR_EOF;
        goto movie_eof;
      case mcmd_next:
        break;
      case mcmd_syncInit: {
        marg_syncInit *arg = (marg_syncInit *)p;
        arg->SwapBytes();
        if (!syncInit(arg->period, arg->wait_quanta)) {
          result = MVE_ERR_SYNC;
          goto done;
        }
        continue;
      }

      case mcmd_sndConfigure: {
        marg_sndConfigure *arg = (marg_sndConfigure *)p;
        arg->SwapBytes();
        unsigned comp16 = hdr.minor >= 1 ? arg->comp16 : 0;
        unsigned buflen = arg->buflen;
        if (hdr.minor == 0)
          buflen &= 0xFFFF;
        if (!sndConfigure(arg->rate, buflen, arg->stereo, arg->frequency, arg->bits16, comp16)) {
          result = MVE_ERR_SND;
          goto done;
        }
        continue;
      }

      case mcmd_sndSync: {
        sndSync();
        continue;
      }

      case mcmd_nfConfig: {
        marg_nfConfig *arg = (marg_nfConfig *)p;
        arg->SwapBytes();
        unsigned hicolor = hdr.minor >= 2 ? arg->hicolor : 0;
        if (!nfConfig(arg->wqty, arg->hqty, (hdr.minor >= 1 ? arg->fqty : 1), hicolor)) {
          result = MVE_ERR_NF;
          goto done;
        }
        // <WIP> To handle interlace mode, we need the following:
        //  A window width/height in addition to nf_width/height.
        //  Window width/height should be used here and in centering
        //  code in sfShowFrame.
        {
          unsigned scaled_width = nf_width * 4 / opt_hscale_step;
          scaled_width &= ~0xf; // Round down to a multiple of 16.
          if (nf_hicolor)
            scaled_width >>= 1; /*HACK*/
          if (scaled_width + (rm_dx < 0 ? 0 : rm_dx) > sf_ScreenWidth ||
              nf_height + (rm_dy < 0 ? 0 : rm_dy) > sf_ScreenHeight) {
            result = MVE_ERR_GFX_FIT;
            goto done;
          }
        }
        // NOTE:  Eventually, change this to allow sf_hicolor when !nf_hicolor and
        //  have show function convert from 256-color to hicolor format.
        //  HiColor will also need to disable hardware palette changes and
        //  maintain 15-bit software palette along with 24-bit palette.
        if (nf_hicolor && !sf_hicolor) {
          result = MVE_ERR_GFX_FIT;
          goto done;
        }
        continue;
      }

      case mcmd_nfDecomp: {
        marg_nfDecomp *arg = (marg_nfDecomp *)p;
        arg->SwapBytes();
        if (arg->advance) {
          nfAdvance();
        }

        nfDecomp(MCMD_DATA(arg), arg->x, arg->y, arg->w, arg->h);
        continue;
      }

      case mcmd_sfShowFrame: {
        unsigned field = 0;
        marg_sfShowFrame *arg = (marg_sfShowFrame *)p;
        arg->SwapBytes();

        ++rm_FrameCount;

        if (hdr.minor >= 1) {
          field = arg->field;
        }

        if (arg->pal_count && !DecompChg_chgs && !sf_hicolor) {
          palClrPalette(arg->pal_start, arg->pal_count);
        } else {
          palSetPalette(arg->pal_start, arg->pal_count);
        }

        if (DecompChg_chgs) {
          sfShowFrameChg(rm_dx, rm_dy, DecompChg_chgs);
        } else {
          if (sync_late && arg->pal_count == 0) {
            sync_FrameDropped = true;
            ++rm_FrameDropCount;
          } else {
            sfShowFrame(rm_dx, rm_dy, field);
          }
        }

        if (arg->pal_count && !DecompChg_chgs && !sf_hicolor) {
          palSetPalette(arg->pal_start, arg->pal_count);
        }

        goto FrameDone;
      }

      case mcmd_sndAdd:
      case mcmd_sndSilence: {
        marg_sndAdd *arg = (marg_sndAdd *)p;
        arg->SwapBytes();
        if (arg->TrackMask & rm_track_bit)
          sndAdd((hdr.major == mcmd_sndAdd ? MCMD_DATA(arg) : (uint8_t *)NULL), arg->qty);
        continue;
      }

      case mcmd_gfxMode: {
        marg_gfxMode *arg = (marg_gfxMode *)p;
        arg->SwapBytes();
        if (sf_auto) {
          int16_t mode = arg->mode;
          if (opt_fastmode && (opt_fastmode & 4) == 0)
            mode |= 0x8000;
          if (sf_auto_mode != mode)
            if (!MVE_gfxMode(mode)) {
              result = MVE_ERR_GFX_FAIL;
              goto done;
            }
          sf_auto = true;
          sf_auto_mode = mode;
        }
        continue;
      }

      case mcmd_palMakeSynthPalette: {
        marg_palMakeSynthPalette *arg = (marg_palMakeSynthPalette *)p;
        arg->SwapBytes();
        palMakeSynthPalette(arg->base_r, arg->range_r, arg->range_rb, arg->base_g, arg->range_g, arg->range_gb);
        continue;
      }

      case mcmd_palLoadPalette: {
        marg_palLoadPalette *arg = (marg_palLoadPalette *)p;
        arg->SwapBytes();
        palLoadPalette(MCMD_DATA(arg), arg->start, arg->count);
        continue;
      }

      case mcmd_palLoadCompPalette: {
        palLoadCompPalette(p);
        continue;
      }

      case mcmd_nfChanges: {
        DecompChg_chgs = (uint16_t *)p;
        continue;
      }
      case mcmd_nfParms: {
        DecompChg_parms = (uint16_t *)p;
        continue;
      }
      case mcmd_nfDecompChg: {
        marg_nfDecomp *arg = (marg_nfDecomp *)p;
        arg->SwapBytes();
        if (arg->advance) {
          nfAdvance();
        }

        nfDecompChg(DecompChg_chgs, DecompChg_parms, MCMD_DATA(arg), arg->x, arg->y, arg->w, arg->h);
        continue;
      }
      case mcmd_nfPkDecomp: {
        marg_nfDecomp *arg = (marg_nfDecomp *)p;
        arg->SwapBytes();
        if (hdr.minor < 3) {
          result = MVE_ERR_BADFMT;
          goto done;
        }

        if (arg->advance) {
          nfAdvance();
        }

        if (nf_hicolor) {
          if (opt_fastmode) {
            result = MVE_ERR_BADFMT;
            goto done;
          }

          nfHPkDecomp((uint8_t *)DecompChg_parms, MCMD_DATA(arg), arg->x, arg->y, arg->w, arg->h);
        } else if ((opt_fastmode & 3) == 1) {
          // Half mode (half height, even lines only)
          nfPkDecompH((uint8_t *)DecompChg_parms, MCMD_DATA(arg), arg->x, arg->y, arg->w, arg->h);
        } else if ((opt_fastmode & 3) == 2) {
          // Support for dithered mode disabled...
          //  so just use half mode instead
          nfPkDecompH((uint8_t *)DecompChg_parms, MCMD_DATA(arg), arg->x, arg->y, arg->w, arg->h);
        } else // opt_fastmode==0
        {
          // Normal mode
          nfPkDecomp((uint8_t *)DecompChg_parms, MCMD_DATA(arg), arg->x, arg->y, arg->w, arg->h);
        }
        continue;
      }
      }
      break;
    }
  }
done: // Error
  MVE_rmEndMovie();

movie_eof: // End of movie
  return result;

FrameDone: // Done with frame, return to caller for between frame processing
  rm_p = p;
  rm_len = len;
  return 0;
}

void MVE_rmEndMovie(void) {
  if (rm_active) {
    syncWait();
    syncRelease();
    sndReset();
    rm_active = false;
  }
}

int MVE_RunMovie(int hFile, int dx, int dy, unsigned track) {
  int result;
  result = MVE_rmPrepMovie(hFile, dx, dy, track);
  while (!result) {
    result = MVE_rmStepMovie();
    logLabel("StartUser");
    while (!result) {
      result = (*rm_ctl)();
      if (result != MVE_CTL_HOLD)
        break;
      result = MVE_rmHoldMovie();
    }
    logLabel("EndUser");
  }
  MVE_rmEndMovie();
  return (result == MVE_ERR_EOF) ? 0 : result;
}

int MVE_RunMovieContinue(int hFile, int dx, int dy, unsigned track) {
  int result;
  result = MVE_rmPrepMovie(hFile, dx, dy, track);
  while (!result) {
    result = MVE_rmStepMovie();
    logLabel("StartUser");
    while (!result) {
      result = (*rm_ctl)();
      if (result != MVE_CTL_HOLD)
        break;
      result = MVE_rmHoldMovie();
    }
    logLabel("EndUser");
  }
  // Continue version doesn't call MVE_rmEndMovie;
  return (result == MVE_ERR_EOF) ? 0 : result;
}

void MVE_ReleaseMem(void) {
  MVE_rmEndMovie();
  ioRelease();
  sndRelease();
  nfRelease();
}

//----------------------------------------------------------------------

const char *MVE_strerror(int code) {
  const char *errors[] = {
                    "Movie aborted with special code",
                    "Movie aborted",
                    "Movie completed normally", // 0
                    "Movie completed normally", //-1
                    "File I/O error or Unable to allocate I/O buffers",
                    "Unable to create timer",
                    "Unable to allocate sound buffers",
                    "Unable to allocate video buffers",
                    "Insufficient screen resolution for movie",
                    "Unable to setup graphics mode used by movie",
                    "Invalid movie file",
                    "Incorrect screen color mode",
                    "StepMovie() without PrepMovie()",
                    "Unable to initialize Linux Draw System",
                    "Unable to lock window surface",
                    "Unknown movie error code"
  };

  if (code >= MVE_CTL_EXIT + 1)
    code = MVE_CTL_EXIT + 1;
  if (code <= MVE_ERR_LAST - 1)
    code = MVE_ERR_LAST - 1;
  return errors[MVE_CTL_EXIT + 1 - code];
}

//----------------------------------------------------------------------
// Frame Reader
//--------------

struct MVE_frStreamRec {
  int (*callback)(unsigned op, unsigned subop, void *buf);

  // I/O Stream state
  unsigned (*io_read)(int handle, void *buf, unsigned count);
  MemRec io_mem_buf;
  int io_handle;
  ioHdrRec io_next_hdr;
  uint8_t *p;
  unsigned len;

  // NextFrame working storage
  bool nf_memory_mode;

  MemRec nf_mem_buf1;
  MemRec nf_mem_buf2;
  uint8_t *nf_buf_cur;
  uint8_t *nf_buf_prv;
  // NextFrame parameters
  uint8_t nf_wqty; // (width/SWIDTH)
  uint8_t nf_hqty; // (height/SHEIGHT)
  uint8_t nf_fqty; // Number of fields
  unsigned nf_hicolor;   // HiColor (0:none,1:normal,2:swapped)
  // NextFrame derived quantities
  unsigned nf_width;      // wqty * SWIDTH
  unsigned nf_height;     // hqty * SHEIGHT;
  unsigned nf_new_line;   // width - SWIDTH
  unsigned nf_new_row0;   // SHEIGHT*width*2-width
  unsigned nf_back_right; // (SHEIGHT-1)*width
  // Palette
  uint8_t pal_tbl[3 * 256];
  unsigned pal_start, pal_count;

};

static void frLoad(MVE_frStream frs) {
  io_read = frs->io_read;
  io_mem_buf = frs->io_mem_buf;
  io_handle = frs->io_handle;
  io_next_hdr = frs->io_next_hdr;

  nf_memory_mode = frs->nf_memory_mode;
  nf_mem_buf1 = frs->nf_mem_buf1;
  nf_mem_buf2 = frs->nf_mem_buf2;
  nf_buf_cur = frs->nf_buf_cur;
  nf_buf_prv = frs->nf_buf_prv;
  nf_wqty = frs->nf_wqty;
  nf_hqty = frs->nf_hqty;
  nf_fqty = frs->nf_fqty;
  nf_hicolor = frs->nf_hicolor;
  nf_width = frs->nf_width;
  nf_height = frs->nf_height;
  nf_new_line = frs->nf_new_line;
  nf_new_row0 = frs->nf_new_row0;
  nf_back_right = frs->nf_back_right;
}

static void frSave(MVE_frStream frs) {
  frs->io_read = io_read;
  frs->io_mem_buf = io_mem_buf;
  frs->io_handle = io_handle;
  frs->io_next_hdr = io_next_hdr;

  frs->nf_memory_mode = nf_memory_mode;
  frs->nf_mem_buf1 = nf_mem_buf1;
  frs->nf_mem_buf2 = nf_mem_buf2;
  frs->nf_buf_cur = nf_buf_cur;
  frs->nf_buf_prv = nf_buf_prv;
  frs->nf_wqty = nf_wqty;
  frs->nf_hqty = nf_hqty;
  frs->nf_fqty = nf_fqty;
  frs->nf_hicolor = nf_hicolor;
  frs->nf_width = nf_width;
  frs->nf_height = nf_height;
  frs->nf_new_line = nf_new_line;
  frs->nf_new_row0 = nf_new_row0;
  frs->nf_back_right = nf_back_right;
}

MVE_frStream MVE_frOpen(unsigned (*fn_read)(int handle, void *buf, unsigned count), int handle,
                        int (*fr_callback)(unsigned, unsigned, void *buf)) {
  MVE_frStream frs;
  MVE_frStreamRec save;
  bool failed = false;

  if (!mem_alloc)
    return (MVE_frStream)NULL;

  frs = (struct MVE_frStreamRec *)(*mem_alloc)(sizeof(*frs));
  if (!frs)
    return frs;
  memset(frs, 0, sizeof(*frs));

  frSave(&save);
  frLoad(frs);

  MVE_ioCallbacks(fn_read);
  failed = !ioReset(handle);
  frs->callback = fr_callback;

  if (!failed) {
    frs->p = ioNextRecord();
    frs->len = 0;
  }

  frSave(frs);
  frLoad(&save);

  if (failed) {
    MVE_frClose(frs);
    return (MVE_frStream)NULL;
  }

  return frs;
}

int MVE_frGet(MVE_frStream frs, uint8_t **pBuf, uint32_t *width, uint32_t *height,
              uint32_t *hicolor) {
  MVE_frStreamRec save;
  uint8_t *p;
  unsigned len;
  int result = 0;

  frSave(&save);
  frLoad(frs);
  p = frs->p;
  len = frs->len;

  for (;; p = ioNextRecord(), len = 0) {
    uint16_t *DecompChg_parms = (uint16_t *)NULL;

    if (!p) {
      result = MVE_ERR_IO;
      goto done;
    }
    for (;;) {
      mcmd_hdr hdr;
      p += len; // Advance past data of previous command
      hdr = *(mcmd_hdr *)p;
      hdr.SwapBytes();
      p += sizeof(hdr);
      len = hdr.len;
      switch (hdr.major) {
      default:
        if (frs->callback) {
          result = (*frs->callback)(hdr.major, hdr.minor, p);
          if (result)
            goto done;
        }
        continue;

      case mcmd_end:
        result = MVE_ERR_EOF;
        goto done;

      case mcmd_next:
        break;

      case mcmd_nfConfig: {
        marg_nfConfig *arg = (marg_nfConfig *)p;
        arg->SwapBytes();
        unsigned hicolor = hdr.minor >= 2 ? arg->hicolor : 0;
        unsigned opt_fastmode_save = opt_fastmode;
        opt_fastmode = 0;
        if (hicolor || (hdr.minor >= 1 && arg->fqty != 1) ||
            !nfConfig(arg->wqty, arg->hqty, (hdr.minor >= 1) ? arg->fqty : 1, hicolor)) {
          opt_fastmode = opt_fastmode_save;
          result = MVE_ERR_NF;
          goto done;
        }
        opt_fastmode = opt_fastmode_save;
        continue;
      }

      case mcmd_sfShowFrame: {
        unsigned field = 0;
        marg_sfShowFrame *arg = (marg_sfShowFrame *)p;
        arg->SwapBytes();
        if (hdr.minor >= 1)
          field = arg->field;
        if (field) {
          result = MVE_ERR_BADFMT;
          goto done;
        }
        *pBuf = nf_buf_cur;
        *width = nf_width;
        *height = nf_height;
        *hicolor = nf_hicolor;
        frs->pal_start = arg->pal_start;
        frs->pal_count = arg->pal_count;
        goto done;
      }

      case mcmd_nfParms: {
        DecompChg_parms = (uint16_t *)p;
        continue;
      }

      case mcmd_nfPkDecomp: {
        marg_nfDecomp *arg = (marg_nfDecomp *)p;
        arg->SwapBytes();
        if (hdr.minor < 3) {
          result = MVE_ERR_BADFMT;
          goto done;
        }
        if (arg->advance) {
          nfAdvance();
        }
        if (nf_hicolor) {
          result = MVE_ERR_BADFMT;
          goto done;
        }
        nfPkConfig();
        nfPkDecomp((uint8_t *)DecompChg_parms, MCMD_DATA(arg), arg->x, arg->y, arg->w, arg->h);
        continue;
      }
      case mcmd_palLoadPalette: {
        marg_palLoadPalette *arg = (marg_palLoadPalette *)p;
        arg->SwapBytes();
        memcpy(&frs->pal_tbl[arg->start * 3], MCMD_DATA(arg), arg->count * 3);
        continue;
      }
      }
      break;
    }
  }

done:
  frSave(frs);
  frs->p = p;
  frs->len = len;
  frLoad(&save);
  nfPkConfig();
  if (result)
    frs->pal_start = frs->pal_count = 0;
  return result;
}

void MVE_frPal(MVE_frStream frs, uint8_t **p, unsigned *start, unsigned *count) {
  *p = frs->pal_tbl;
  *start = frs->pal_start;
  *count = frs->pal_count;
}

void MVE_frClose(MVE_frStream frs) {
  MVE_frStreamRec save;

  frSave(&save);
  frLoad(frs);

  ioRelease();
  nfRelease();

  frLoad(&save);
  if (mem_free) {
    (*mem_free)(frs);
  }
}
