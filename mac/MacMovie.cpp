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

// Mac implementation

#include <stdio.h>

#include <DrawSprocket.h>

#include "descent.h"
#include "macapp.h"
#include "hlsoundlib.h"
#include "movie.h"
#include "mac_llsound.h"

// sets the directory where movies are stored
int mve_Init(const char *dir, const char *sndcard) {
  long myAttrs;
  OSErr err;

  err = Gestalt(gestaltQuickTime, &myAttrs);
  if (err) {
    return MVELIB_INIT_ERROR;
  }

  err = EnterMovies();
  if (err) {
    return MVELIB_INIT_ERROR;
  }

  return MVELIB_NOERROR;
}

int mve_PlayMovie(const char *mvename, oeApplication *app) {
  OSErr theErr, err;
  OSStatus theStatus;
  FSSpec theFSSpec;
  Rect r;
  int movieError;
  short fileRef;
  CGrafPtr port;
  Rect movieBox;
  extern DSpContextReference gDSpContext[];

  short movieResFile;
  tMacAppInfo macApp;

  err = FSMakeFSSpec(0, 0L, (const unsigned char *)c2pstr(mvename), &theFSSpec);
  if (err != noErr) {
    // mprintf((2, "could not make FSSpec in mve_PlayMovie()\n"));
    // Int3();
    return MVELIB_FILE_ERROR;
  }

  theErr = OpenMovieFile(&theFSSpec, &movieResFile, fsRdPerm);

  if (fileRef != -1 && theErr == noErr) {

    Movie aMovie = nil;
    short movieResID = 0;
    Str255 movieName;
    Boolean wasChanged;

    err = NewMovieFromFile(&aMovie, movieResFile, &movieResID, movieName, newMovieActive, &wasChanged);

    DSpContext_GetBackBuffer(gDSpContext[0], kDSpBufferKind_Normal, &port);
    SetPort((GrafPtr)port);
    BackColor(whiteColor);
    r = port->portRect;
    PaintRect(&r);
    DSpContext_InvalBackBufferRect(gDSpContext[0], &r);
    DSpContext_SwapBuffers(gDSpContext[0], NULL, 0);

    float saveVol = Sound_system.GetMasterVolume();
    Sound_system.SetMasterVolume(0.7);

    FlushEvents(everyEvent, 0);

    Descent->get_info(&macApp);
    GetMovieBox(aMovie, &movieBox);
    OffsetRect(&movieBox, 0, (macApp.wnd_h - movieBox.bottom) / 2);
    SetMovieBox(aMovie, &movieBox);
    SetMovieGWorld(aMovie, macApp.hwnd, nil);
    //		SetMovieVolume(aMovie, 0x0300);

    StartMovie(aMovie);

    EventRecord theEvent;
    bool done = false;
    while (!IsMovieDone(aMovie) && !done) {
      if (::GetOSEvent(everyEvent - diskMask, &theEvent)) {
        //			if (WaitNextEvent (everyEvent, &theEvent, 0, nil)) {
        switch (theEvent.what) {
        case mouseDown:
          done = true;
          break;
        case keyUp:
        case keyDown:
        case autoKey:
          if ((theEvent.message & charCodeMask) & 0x1B) // ESC
            done = true;
          break;
        }
      }
      MoviesTask(aMovie, DoTheRightThing);
    }
    // Clean up.
    DisposeMovie(aMovie);

    BackColor(blackColor);
    PaintRect(&r);
    DSpContext_InvalBackBufferRect(gDSpContext[0], &r);
    DSpContext_SwapBuffers(gDSpContext[0], NULL, 0);

    Sound_system.SetMasterVolume(saveVol);

    return MVELIB_NOERROR;
  }
  return MVELIB_PLAYBACK_ERROR;
}

// sets the directory where movies are stored
unsigned mve_SequenceStart(const char *mvename, int *fhandle, oeApplication *app, bool looping) { return 0; }

unsigned mve_SequenceFrame(unsigned handle, int fhandle, bool sequence, int *bm_handle) { return (unsigned)(-1); }

bool mve_SequenceClose(unsigned handle, int fhandle) { return false; }

void mve_SetRenderProperties(short x, short y, short w, short h, renderer_type type, bool hicolor) {}

void mve_SetCallback(void (*fn)(int, int, int)) {}

// call to print out text.
void mve_Puts(short x, short y, const char *txt) {}

void mve_ClearRect(short x1, short y1, short x2, short y2) {}

// call to print out text.
void mve_Puts(short x, short y, ddgr_color col, const char *txt) {}
