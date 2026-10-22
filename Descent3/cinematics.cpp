/*
* Descent 3 
* Copyright (C) 2024 Parallax Software
* Copyright (C) 2026 Descent Developers
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

#include "appdatabase.h"
#include "bitmap.h"
#include "cinematics.h"
#include "d3movie.h"
#include "ddio.h"
#include "dedicated_server.h"
#include "descent.h"
#include "game.h"
#include "log.h"
#include "subtitles.h"

static bool Cinematic_lib_init = false;
static int Cinematic_x = 0, Cinematic_y = 0, Cinematic_w = 0, Cinematic_h = 0;

static void CinematicCallback(int mve_x, int mve_y, int frame_num);

///////////////////////////////////////////////////////////////////////////////
//	functions

bool InitCinematics() {
  if (Dedicated_server) {
    Cinematic_lib_init = false;
    return true;
  }

  if (mve_Init() != MVELIB_NOERROR)
    return false;

  mve_SetCallback(CinematicCallback);

  Cinematic_lib_init = true;

  return true;
}

void SetMovieProperties(int x, int y, int w, int h, renderer_type type) {
  const bool kHiColorEnabled = true;
  Cinematic_x = x;
  Cinematic_y = y;
  Cinematic_w = w;
  Cinematic_h = h;
  mve_SetRenderProperties(x, y, w, h, type, kHiColorEnabled);
}

bool PlayMovie(const std::filesystem::path &moviename) {
  if (!Cinematic_lib_init)
    return false;

  // get in the right directory
  std::filesystem::path filename = moviename;
  // check extension
  std::filesystem::path extension = moviename.extension();
  if (stricmp(PATH_TO_CSTR(extension), ".mve") != 0 && stricmp(PATH_TO_CSTR(extension), ".mv8") != 0) {
    // we need an extension
    filename.replace_extension(".mve");
  }

  // Initializes the subtitles for a given movie file
  SubtInitSubtitles(moviename);

  SetMovieProperties(0, 0, Max_window_w, Max_window_h, Renderer_type);

  int mveerr = mve_PlayMovie(filename, Descent);

  // Shutdown the subtitle system
  SubtCloseSubtitles();

  bool retval = true;
  if (mveerr != MVELIB_NOERROR) {
    LOG_ERROR.printf("Movie error %d.", mveerr);
    retval = false;
  }

  return retval;
}

MVESTREAM *StartMovie(const char *moviename, bool looping) {
  if (!Cinematic_lib_init)
    return nullptr;

  // get in the right directory
  char filename[_MAX_PATH];
  strncpy(filename, moviename, sizeof(filename) - 1);
  filename[sizeof(filename) - 1] = 0;

  // check extension
  const char *extension = strrchr(filename, '.');
  if (extension == nullptr || (stricmp(extension, ".mve") != 0 && stricmp(extension, ".mv8") != 0)) {
    // we need an extension
    strncat(filename, ".mve", sizeof(filename) - strlen(filename) - 1);
    filename[sizeof(filename) - 1] = 0;
  }

  SetMovieProperties(0, 0, Max_window_w, Max_window_h, Renderer_type);

  MVESTREAM *hMovie = mve_SequenceStart(filename, looping);
  if (hMovie == nullptr)
    return nullptr;

  return hMovie;
}

bool FrameMovie(MVESTREAM *mve, int x, int y) {
  if (!Cinematic_lib_init || mve == nullptr)
    return false;

  int bm_handle;
  MVESTREAM *hNewMovie = mve_SequenceFrame(mve, &bm_handle);
  if (hNewMovie == nullptr) {
    // we must have looped, we have a new handle
    mve = hNewMovie;
  }

  //	do chunk conversion and blt if absolutely necessary.
  if (bm_handle > -1) {
    if (x == -1) {
      x = (Cinematic_w - bm_w(bm_handle, 0)) / 2;
    }

    if (y == -1) {
      y = (Cinematic_h - bm_h(bm_handle, 0)) / 2;
    }

    rend_SetAlphaType(AT_ALWAYS);
    rend_CopyBitmapToFramebuffer(bm_handle, x, y);
  }

  return (hNewMovie != nullptr);
}

void EndMovie(MVESTREAM *mve) {
  if (mve == nullptr || !Cinematic_lib_init)
    return;

  mve_SequenceClose(mve);
}

void CinematicCallback(int mveX, int mveY, int frameNum) { SubtDrawSubtitles(frameNum); }
