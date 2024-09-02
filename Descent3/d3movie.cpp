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

#include <cstring>

#include "application.h"
#include "args.h"
#include "bitmap.h"
#include "d3movie.h"
#include "ddio.h"
#include "ddvid.h"
#include "game.h"
#include "gamefont.h"
#include "grtext.h"
#include "mem.h"
#include "mvelib.h"
#include "pserror.h"
#include "renderer.h"

namespace {
MovieFrameCallback_fp Movie_callback = nullptr;
uint16_t CurrentPalette[256];
int Movie_bm_handle = -1;
uint32_t Movie_current_framenum = 0;
bool Movie_looping = false;
} // namespace

static void *CallbackAlloc(uint32_t size);
static void CallbackFree(void *p);
static uint32_t CallbackFileRead(void *stream, void *pBuffer, uint32_t bufferCount);
static void InitializePalette();
static void CallbackSetPalette(const uint8_t *pBuffer, uint32_t start, uint32_t count);
static void CallbackShowFrame(uint8_t *buf, uint32_t bufw, uint32_t bufh, uint32_t sx,
                              uint32_t sy, uint32_t w, uint32_t h, uint32_t dstx, uint32_t dsty,
                              uint32_t hicolor);

#ifndef NO_MOVIES
static bool mve_InitSound();
static void mve_CloseSound();
#endif

// sets the directory where movies are stored
int mve_Init(const char *dir, const char *sndcard) {
#ifndef NO_MOVIES
  return MVELIB_NOERROR;
#else
  return MVELIB_INIT_ERROR;
#endif
}

// callback called per frame of playback of movies.
void mve_SetCallback(MovieFrameCallback_fp callBack) {
#ifndef NO_MOVIES
  Movie_callback = callBack;
#endif
}

// used to tell movie library how to render movies.
void mve_SetRenderProperties(int16_t x, int16_t y, int16_t w, int16_t h, renderer_type type, bool hicolor) {}

#if defined(POSIX)
// locates the case-sensitive movie file name
std::filesystem::path mve_FindMovieFileRealName(const std::filesystem::path &movie) {
  // split into directory and file...
  std::filesystem::path t_file = movie.filename();
  std::filesystem::path t_dir = movie.parent_path();
  std::filesystem::path t_out;

  // found a directory?
  if (!t_dir.empty()) {
    // map the bits (or fail)
    t_out = cf_FindRealFileNameCaseInsensitive(t_file, t_dir);
    if (t_out.empty())
      return t_out;
    // re-assemble
    return (t_dir / t_out);
  } else {
    // just a file, map that
    t_out = cf_FindRealFileNameCaseInsensitive(t_file);
    if (t_out.empty())
      return t_out;
    // re-assemble
    return t_out;
  }
}
#endif

// plays a movie using the current screen.
int mve_PlayMovie(const std::filesystem::path &pMovieName, oeApplication *pApp) {
#ifndef NO_MOVIES
  // first, find that movie..
  std::filesystem::path real_name;
#if defined(POSIX)
  real_name = mve_FindMovieFileRealName(pMovieName);
  if (real_name.empty()) {
    mprintf(0, "MOVIE: No such file %s\n", pMovieName.u8string().c_str());
    return MVELIB_FILE_ERROR;
  }
#else
  real_name = pMovieName;
#endif
  // open movie file.
  FILE *hFile = fopen(real_name.u8string().c_str(), "rb");
  if (hFile == nullptr) {
    mprintf(0, "MOVIE: Unable to open %s\n", real_name.u8string().c_str());
    return MVELIB_FILE_ERROR;
  }

  // setup
  MVE_sfCallbacks(CallbackShowFrame);
  MVE_memCallbacks(CallbackAlloc, CallbackFree);
  MVE_ioCallbacks(CallbackFileRead);
  MVE_palCallbacks(CallbackSetPalette);
  InitializePalette();
  Movie_bm_handle = -1;

  if (!mve_InitSound()) {
    mprintf(0, "Failed to initialize sound\n");
    fclose(hFile);
    return MVELIB_INIT_ERROR;
  }

  MVESTREAM *mve = MVE_rmPrepMovie(hFile, -1, -1, 0);
  if (mve == nullptr) {
    mprintf(0, "Failed to prepMovie %s\n", pMovieName.u8string().c_str());
    fclose(hFile);
    mve_CloseSound();
    return MVELIB_INIT_ERROR;
  }

  bool aborted = false;
  Movie_current_framenum = 0;
  int result;
  while ((result = MVE_rmStepMovie(mve)) == 0) {
    // let the OS do its thing
    pApp->defer();

    // check for bail
    int key = ddio_KeyInKey();
    if (key == KEY_ESC) {
      aborted = true;
      break;
    }
  }

  // free our bitmap
  if (Movie_bm_handle != -1) {
    bm_FreeBitmap(Movie_bm_handle);
    Movie_bm_handle = -1;
  }

  // close our file handle
  fclose(hFile);

  // determine the return code
  int err = MVELIB_NOERROR;
  if (aborted) {
    err = MVELIB_PLAYBACK_ABORTED;
  } else if (result != MVE_ERR_EOF) {
    err = MVELIB_PLAYBACK_ERROR;
  }

  // cleanup and shutdown
  MVE_rmEndMovie(mve);

  // reset sound
  mve_CloseSound();

  // return out
  return err;
#else
  return MVELIB_INIT_ERROR;
#endif
}

void *CallbackAlloc(uint32_t size) { return mem_malloc(size); }

void CallbackFree(void *p) { mem_free(p); }

uint32_t CallbackFileRead(void *stream, void *pBuffer, uint32_t bufferCount) {
  uint32_t numRead = fread(pBuffer, 1, bufferCount, (FILE *)stream);
  return (numRead == bufferCount) ? 1 : 0;
}

void InitializePalette() {
  for (unsigned short & i : CurrentPalette) {
    i = OPAQUE_FLAG | GR_RGB16(0, 0, 0);
  }
}

void CallbackSetPalette(const uint8_t *pBuffer, uint32_t start, uint32_t count) {
#ifndef NO_MOVIES
  pBuffer += start * 3;

  for (uint32_t i = 0; i < count; ++i) {
    uint32_t r = pBuffer[0] << 2;
    uint32_t g = pBuffer[1] << 2;
    uint32_t b = pBuffer[2] << 2;
    pBuffer += 3;

    CurrentPalette[start + i] = OPAQUE_FLAG | GR_RGB16(r, g, b);
  }
#endif
}

int NextPow2(int n) {
  n--;
  n |= n >> 1;
  n |= n >> 2;
  n |= n >> 4;
  n |= n >> 8;
  n |= n >> 16;
  n++;
  return n;
}

#ifndef NO_MOVIES
void BlitToMovieBitmap(uint8_t *buf, uint32_t bufw, uint32_t bufh, uint32_t hicolor,
                       bool usePow2Texture, int &texW, int &texH) {
  // get some sizes
  int drawWidth = bufw;
  int drawHeight = bufh;

  if (usePow2Texture) {
    int wPow2 = NextPow2(drawWidth);
    int hPow2 = NextPow2(drawHeight);
    int texSize = (wPow2 > hPow2) ? wPow2 : hPow2;
    texW = texSize;
    texH = texSize;
  } else {
    texW = drawWidth;
    texH = drawHeight;
  }

  if (Movie_bm_handle == -1) {
    // Allocate our bitmap
    Movie_bm_handle = bm_AllocBitmap(texW, texH, 0);
  }

  uint16_t *pPixelData = (uint16_t *)bm_data(Movie_bm_handle, 0);
  GameBitmaps[Movie_bm_handle].flags |= BF_CHANGED;
  if (hicolor) {
    uint16_t *wBuf = (uint16_t *)buf;
    for (int y = 0; y < drawHeight; ++y) {
      for (int x = 0; x < drawWidth; ++x) {
        uint16_t col16 = *wBuf++;
        // Convert to RGB555
        pPixelData[x] = col16 | OPAQUE_FLAG;
      }

      pPixelData += texW;
    }
  } else {
    for (int y = 0; y < drawHeight; ++y) {
      for (int x = 0; x < drawWidth; ++x) {
        uint8_t palIndex = *buf++;
        pPixelData[x] = CurrentPalette[palIndex];
      }

      pPixelData += texW;
    }
  }
}

void CallbackShowFrame(uint8_t *buf, uint32_t bufw, uint32_t bufh, uint32_t sx, uint32_t sy,
                       uint32_t w, uint32_t h, uint32_t dstx, uint32_t dsty, uint32_t hicolor) {
  // prepare our bitmap
  int texW, texH;
  BlitToMovieBitmap(buf, bufw, bufh, hicolor, true, texW, texH);

  // calculate UVs from texture
  unsigned int drawWidth = bufw;
  unsigned int drawHeight = bufh;
  float u = float(drawWidth - 1) / float(texW - 1);
  float v = float(drawHeight - 1) / float(texH - 1);

  StartFrame(0, 0, 640, 480, false);

  rend_ClearScreen(GR_BLACK);
  rend_SetAlphaType(AT_CONSTANT);
  rend_SetAlphaValue(255);
  rend_SetLighting(LS_NONE);
  rend_SetColorModel(CM_MONO);
  rend_SetOverlayType(OT_NONE);
  rend_SetWrapType(WT_CLAMP);
  rend_SetFiltering(0);
  rend_SetZBufferState(0);
  rend_DrawScaledBitmap(dstx, dsty, dstx + drawWidth, dsty + drawHeight, Movie_bm_handle, 0.0f, 0.0f, u, v);
  rend_SetFiltering(1);
  rend_SetZBufferState(1);

  // call our callback
  if (Movie_callback != nullptr) {
    Movie_callback(dstx, dsty, Movie_current_framenum);
  }
  ++Movie_current_framenum;

  EndFrame();

  rend_Flip();
}

// This callback is same as CallbackShowFrame() but don't flip renderer at end, so there no flickering
void CallbackShowFrameNoFlip(unsigned char *buf, unsigned int bufw, unsigned int bufh, unsigned int sx, unsigned int sy,
                       unsigned int w, unsigned int h, unsigned int dstx, unsigned int dsty, unsigned int hicolor) {
  // prepare our bitmap
  int texW, texH;
  BlitToMovieBitmap(buf, bufw, bufh, hicolor, true, texW, texH);

  // calculate UVs from texture
  unsigned int drawWidth = bufw;
  unsigned int drawHeight = bufh;
  float u = float(drawWidth - 1) / float(texW - 1);
  float v = float(drawHeight - 1) / float(texH - 1);

  StartFrame(0, 0, 640, 480, false);

  rend_ClearScreen(GR_BLACK);
  rend_SetAlphaType(AT_CONSTANT);
  rend_SetAlphaValue(255);
  rend_SetLighting(LS_NONE);
  rend_SetColorModel(CM_MONO);
  rend_SetOverlayType(OT_NONE);
  rend_SetWrapType(WT_CLAMP);
  rend_SetFiltering(0);
  rend_SetZBufferState(0);
  rend_DrawScaledBitmap(dstx, dsty, dstx + drawWidth, dsty + drawHeight, Movie_bm_handle, 0.0f, 0.0f, u, v);
  rend_SetFiltering(1);
  rend_SetZBufferState(1);

  // call our callback
  if (Movie_callback != nullptr) {
    Movie_callback(dstx, dsty, Movie_current_framenum);
  }
  ++Movie_current_framenum;

  EndFrame();
}
#endif

intptr_t mve_SequenceStart(const char *mvename, void *fhandle, oeApplication *app, bool looping) {
#ifndef NO_MOVIES
  // first, find that movie..
  std::filesystem::path real_name;
#if defined(POSIX)
  real_name = mve_FindMovieFileRealName(mvename);
  if (real_name.empty()) {
    mprintf(0, "MOVIE: No such file %s\n", mvename);
    fhandle = nullptr;
    return 0;
  }
#else
  real_name = mvename;
#endif
  fhandle = fopen(real_name.u8string().c_str(), "rb");

  if (fhandle == nullptr) {
    mprintf(1, "MOVIE: Unable to open %s\n", real_name.u8string().c_str());
    return 0;
  }

  // setup
  MVE_memCallbacks(CallbackAlloc, CallbackFree);
  MVE_ioCallbacks(CallbackFileRead);
  MVE_sfCallbacks(CallbackShowFrameNoFlip);
  MVE_palCallbacks(CallbackSetPalette);
  InitializePalette();
  Movie_bm_handle = -1;
  Movie_looping = looping;

  // let the render know we will be copying bitmaps to framebuffer (or something)
  rend_SetFrameBufferCopyState(true);

  MVESTREAM *mve = MVE_rmPrepMovie(fhandle, -1, -1, 0);
  if (mve == nullptr) {
    mprintf(0, "Failed to PrepMovie %s\n", mvename);
    fclose((FILE *)fhandle);
    return MVELIB_INIT_ERROR;
  }

  return (intptr_t)mve;
#else
  return nullptr;
#endif
}

intptr_t mve_SequenceFrame(intptr_t handle, void *fhandle, bool sequence, int *bm_handle) {
#ifndef NO_MOVIES
  if (bm_handle) {
    *bm_handle = -1;
  }

  if (handle == -1) {
    return -1;
  }

  static unsigned sw = 0, sh = 0, hicolor = 0;
  int err = 0;

reread_frame:

  // get the next frame of data
  uint8_t *pBuffer = nullptr;
  err = MVE_rmStepMovie((MVESTREAM *)handle);

  if (err == 0) {
    // blit to bitmap
    int texW, texH;
    BlitToMovieBitmap(pBuffer, sw, sh, hicolor, false, texW, texH);

    if (bm_handle) {
      *bm_handle = Movie_bm_handle;
    }

    return handle;
  }

  if (Movie_looping && err == MVE_ERR_EOF) {
    mve_reset((MVESTREAM *)handle);
    sequence = true;
    goto reread_frame;
  }

  return -1;
#else
  return -1;
#endif
}

bool mve_SequenceClose(intptr_t hMovie, void *hFile) {
#ifndef NO_MOVIES
  if (hMovie == -1)
    return false;

  MVE_rmEndMovie((MVESTREAM *)hMovie);

  // free our bitmap
  if (Movie_bm_handle != -1) {
    bm_FreeBitmap(Movie_bm_handle);
    Movie_bm_handle = -1;
  }

  // We're no longer needing this
  rend_SetFrameBufferCopyState(false);

  return true;
#else
  return false;
#endif
}

void mve_Puts(int16_t x, int16_t y, ddgr_color col, const char *txt) {
  grtext_SetFont(BRIEFING_FONT);
  grtext_SetColor(col);
  grtext_SetAlpha(255);
  grtext_SetFlags(GRTEXTFLAG_SHADOW);
  grtext_CenteredPrintf(0, y, txt);
  grtext_Flush();
}

void mve_ClearRect(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
  // Note: I can not figure out how to clear, and then write over it with text. It always covers my text!
  // rend_FillRect( GR_BLACK, x1, y1, x2, y2 );
}

#ifndef NO_MOVIES
bool mve_InitSound() {
  MVE_sndInit(FindArg("-nosound") == 0);

  return true;
}

void mve_CloseSound() {
  // TODO: close the driver out
}

#endif
