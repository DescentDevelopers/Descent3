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

#ifndef MOVIE_H_
#define MOVIE_H_

#include <cstdint>
#include <filesystem>

#include "application.h"
#include "mvelib.h"
#include "renderer.h"

#define MVELIB_NOERROR 0
#define MVELIB_FILE_ERROR (-1)
#define MVELIB_NOTINIT_ERROR (-2)
#define MVELIB_DDGR_ERROR (-3)
#define MVELIB_INIT_ERROR (-4)
#define MVELIB_PLAYBACK_ERROR (-5)
#define MVELIB_PLAYBACK_ABORTED (-6)

int mve_Init();

// simply plays a movie.
int mve_PlayMovie(const std::filesystem::path &pMovieName, oeApplication *pApp);

/**
 * Initialize sequence play
 * @param mvename filename of movie to play
 * @param looping restart play on end of movie
 * @return handle to movie sequence
 */
MVESTREAM *mve_SequenceStart(const char *mvename, bool looping = false);

/**
 * Frames movie started with mve_SequenceStart(). Optionally, pass a pointer to a variable which will receive bitmap
 * handle containing data.
 * @param handle the movie handle returned by mve_SequenceStart()
 * @param bm_handle bitmap handle containing data.
 * @return value is the handle passed into mve_SequenceFrame() or new one if movie looped.
 */
MVESTREAM *mve_SequenceFrame(MVESTREAM *handle, int *bm_handle);

/**
 * Close sequence and free allocated memory
 * @param handle movie handle
 * @return true if some memory freed, false if there was nothing to do.
 */
bool mve_SequenceClose(MVESTREAM *handle);

//	sets render frame boundaries.
void mve_SetRenderProperties(int16_t x, int16_t y, int16_t w, int16_t h, renderer_type type, bool hicolor);

// called every frame.
typedef void (*MovieFrameCallback_fp)(int x, int y, int movieFrameNum);
void mve_SetCallback(MovieFrameCallback_fp callBack);

// call to print out text.
void mve_Puts(int16_t x, int16_t y, ddgr_color col, const char *txt);

void mve_ClearRect(int16_t x1, int16_t y1, int16_t x2, int16_t y2);

#endif
