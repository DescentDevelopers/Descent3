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

#include <Fonts.h>
#include <sound.h>
#include <DrawSprocket.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "game.h"
#include "gl.h"
#include "aglRenderers.h"
#include "agl.h"
#include "macdisplays.h"
#include "macapp.h"
#include "descent.h"
#include "pstypes.h"
#include "config.h"
#include "application.h"
#include "rend_opengl.h"

const RGBColor rgbBlack = {0x0000, 0x0000, 0x0000};
const RGBColor rgbWhite = {0xFFFF, 0xFFFF, 0xFFFF};
/* globals */
DSpContextAttributes gDSpContextAttributes[N_SUPPORTED_VIDRES];
DSpContextReference gDSpContext[N_SUPPORTED_VIDRES] = {NULL, NULL, NULL};
short current_context = DSP_640x480;
DSpContextReference *gpContextRefUnused = NULL;

GDHandle hGD;
void WindowReset(WindowPtr pWindow, short width, short height);

short gnumDevices = 0;
// Set up DSp screens, handles multi-monitor correctly
void SetupScreen(CGrafPtr *ppWin)

// void SetupScreen (GDHandle *hGD, CGrafPtr *ppWin)
// void SetupScreen (GDHandle *hGD, CGrafPtr *ppWin, short *numDevices)
{
  DSpContextAttributes foundAttributes;
  DisplayIDType displayID;
  Rect rectWin;
  RGBColor rgbSave;
  GrafPtr pGrafSave;
  OSStatus err;
  int i;

  // check number of screens
  GDHandle hDevice = DMGetFirstScreenDevice(true);
  do {
    gnumDevices++;
    hDevice = DMGetNextScreenDevice(hDevice, true);
  } while (hDevice);

  // start DSp and find a good context
  err = DSpStartup();
  if (err)
    Error("%s:%d DSpStartup: Error %d ", __FILE__, __LINE__, err);

#ifdef DAJ_DEBUG
  DSpSetDebugMode(true);
#endif
  // Note: DSp currently REQUIRES the back buffer attributes even if only one buffer is required
  // Also note: Choose either 16 or 32 bpp
#ifdef USE_OPENGL
  for (i = 0; i < N_SUPPORTED_VIDRES; i++) {
    memset(&gDSpContextAttributes[i], 0, sizeof(DSpContextAttributes));
    //		gDSpContextAttributes[i].frequency 				= 75<<16;
    gDSpContextAttributes[i].displayDepthMask = kDSpDepthMask_32;
    gDSpContextAttributes[i].displayBestDepth = 32;
    gDSpContextAttributes[i].backBufferDepthMask = kDSpDepthMask_16;
    gDSpContextAttributes[i].backBufferBestDepth = 16;
    gDSpContextAttributes[i].pageCount = 1; // only the front buffer is needed
    gDSpContextAttributes[i].displayWidth = Video_res_list[i].width;
    gDSpContextAttributes[i].displayHeight = Video_res_list[i].height;
    gDSpContextAttributes[i].colorNeeds = kDSpColorNeeds_Require;
  }
#else
  memset(&gDSpContextAttributes[DSP_640x480], 0, sizeof(DSpContextAttributes));
  gDSpContextAttributes[DSP_640x480].displayDepthMask = kDSpDepthMask_16;
  gDSpContextAttributes[DSP_640x480].displayBestDepth = 16; // Render_preferred_state.bit_depth;
  gDSpContextAttributes[DSP_640x480].backBufferDepthMask = kDSpDepthMask_16;
  gDSpContextAttributes[DSP_640x480].backBufferBestDepth = 16;
  gDSpContextAttributes[DSP_640x480].pageCount = 1; // only the front buffer is needed
  gDSpContextAttributes[DSP_640x480].displayWidth = Video_res_list[DSP_640x480].width;
  gDSpContextAttributes[DSP_640x480].displayHeight = Video_res_list[DSP_640x480].height;
  gDSpContextAttributes[DSP_640x480].colorNeeds = kDSpColorNeeds_Require;
#endif
  // will display user dialog if context selection is required otherwise as find best context
#if 1
  err = DSpUserSelectContext(&gDSpContextAttributes[current_context], 0L, nil, &gDSpContext[current_context]);
  if (err == kDSpContextNotFoundErr)
    exit(0);
  else if (err)
    Error("%s:%d DSpUserSelectContext: Error %d ", __FILE__, __LINE__, err);
#else
  DSpFindBestContext(&gDSpContextAttributes[current_context], &gDSpContext[current_context]);
  if (err)
    Error("%s:%d DSpFindBestContext: Error %d ", __FILE__, __LINE__, err);
#endif
  // see what we actually found
  err = DSpContext_GetAttributes(gDSpContext[current_context], &foundAttributes);
  if (err)
    Error("%s:%d DSpContext_GetAttributes: Error %d ", __FILE__, __LINE__, err);

  // reset width and height to full screen and handle our own centering
  // HWA will not correctly center less than full screen size contexts
  gDSpContextAttributes[current_context].displayWidth = foundAttributes.displayWidth;
  gDSpContextAttributes[current_context].displayHeight = foundAttributes.displayHeight;
  gDSpContextAttributes[current_context].pageCount = 1; // only the front buffer is needed
  gDSpContextAttributes[current_context].contextOptions =
      0 | kDSpContextOption_DontSyncVBL; // no page flipping and no VBL sync needed

  DSpSetBlankingColor(&rgbBlack);

  // get our device for future use
  err = DSpContext_GetDisplayID(gDSpContext[current_context], &displayID);
  if (err)
    Error("%s:%d DSpContext_GetDisplayID: Error %d ", __FILE__, __LINE__, err);

  // get GDHandle for ID'd device
  err = DMGetGDeviceByDisplayID(displayID, &hGD, false);
  if (err)
    Error("%s:%d DMGetGDeviceByDisplayID: Error %d ", __FILE__, __LINE__, err);

#ifdef USE_OPENGL
#ifdef SPROCKET17
  for (i = 0; i < N_SUPPORTED_VIDRES; i++) {
    if (i != current_context) {
      err = DSpFindBestContextOnDisplayID(&gDSpContextAttributes[i], &gDSpContext[i], displayID);
      if (err)
        Error("%s:%d DSpFindBestContextOnDisplayID: Error %d ", __FILE__, __LINE__, err);
    }
  }
  // reserve our menu context
  err = DSpContext_Reserve(gDSpContext[current_context], &gDSpContextAttributes[current_context]);
  if (err)
    Error("DSpContext_Reserve: Error %d", err);

  // que up the game context for switching later
  for (i = 0; i < N_SUPPORTED_VIDRES; i++) {
    if (i != current_context && gDSpContext[i]) {
      err = DSpContext_Queue(gDSpContext[current_context], gDSpContext[i], &gDSpContextAttributes[i]);
      if (err)
        Error("%s:%d DSpContext_Queue: Error %d ", __FILE__, __LINE__, err);
    }
  }
#else
  gnumDevices--; // only count unsued screens
  short indexDevice = 0;
  if (gnumDevices) {
    gpContextRefUnused = (DSpContextReference *)NewPtr((long)sizeof(DSpContextReference) * gnumDevices);
    if (gpContextRefUnused == NULL)
      Error("%s:%d NewPtr gpContextRefUnused failed", __FILE__, __LINE__);
    hDevice = DMGetFirstScreenDevice(true); // check number of screens
    if (hDevice == 0)
      Error("%s:%d DMGetFirstScreenDevice: Error %d ", __FILE__, __LINE__, err);
    do {
      if (hDevice != hGD) // if this device is not the one the user chose
      {
        unsigned long displayID;
        DSpContextAttributes contextAttributes;
        err = DMGetDisplayIDByGDevice(hDevice, &displayID, false);
        if (err)
          Error("%s:%d DMGetDisplayIDByGDevice: Error %d ", __FILE__, __LINE__, err);

        err = DSpGetFirstContext(displayID, &gpContextRefUnused[indexDevice]); // get a context and
        if (err)
          Error("%s:%d DSpGetFirstContext: Error %d ", __FILE__, __LINE__, err);

        err = DSpContext_GetAttributes(gpContextRefUnused[indexDevice], &contextAttributes); // find attributes
        if (err)
          Error("%s:%d DSpContext_GetAttributes: Error %d ", __FILE__, __LINE__, err);

        err = DSpContext_Reserve(gpContextRefUnused[indexDevice], &contextAttributes); // reserve it
        if (err)
          Error("%s:%d DSpContext_Reserve: Error %d ", __FILE__, __LINE__, err);
        indexDevice++;
      }
      hDevice = DMGetNextScreenDevice(hDevice, true);
    } while (hDevice);
  }
  for (i = 0; i < N_SUPPORTED_VIDRES; i++) {
    if (i != current_context) {
      err = DSpFindBestContext(&gDSpContextAttributes[i], &gDSpContext[i]);
      if (err)
        Error("%s:%d DSpFindBestContext: Error %d ", __FILE__, __LINE__, err);

      err = DSpContext_GetAttributes(gDSpContext[i], &gDSpContextAttributes[i]); // see what we actually found
      if (err)
        Error("%s:%d DSpContext_GetAttributes: Error %d ", __FILE__, __LINE__, err);

      gDSpContextAttributes[i].displayWidth = Video_res_list[i].width;
      gDSpContextAttributes[i].displayHeight = Video_res_list[i].height;
      gDSpContextAttributes[i].pageCount = 1; // only the front buffer is needed
      gDSpContextAttributes[i].contextOptions =
          0 | kDSpContextOption_DontSyncVBL; // no page flipping and no VBL sync needed
    }
  }
  // reserve our menu context
  err = DSpContext_Reserve(gDSpContext[current_context], &gDSpContextAttributes[current_context]);
  if (err)
    Error("%s:%d DSpContext_Reserve: Error %d ", __FILE__, __LINE__, err);

#endif
#else
  err = DSpContext_Reserve(gDSpContext[DSP_640x480], &gDSpContextAttributes[DSP_640x480]);
  if (err)
    Error("%s:%d DSpContext_Reserve: Error %d ", __FILE__, __LINE__, err);

#endif

#ifndef DAJ_DEBUG
  HideCursor();
  DSpContext_FadeGammaOut(NULL, NULL); // remove for debug
#endif
  // activate our context
  err = DSpContext_SetState(gDSpContext[current_context], kDSpContextState_Active);
  if (err)
    Error("%s:%d DSpContext_SetState: Error %d ", __FILE__, __LINE__, err);

  // create a new window in our context (required for multi-monitor handling (for single monitor it would be better to
  // just use the context Note: OpenGL is expecting a window so it can enumerate the devices it is on, if you us a
  // CGrafPtr instead it MUST be on the main device always
  SetRect(&rectWin, 0, 0, 100, 100);
  *ppWin = (CGrafPtr)NewCWindow(NULL, &rectWin, "\pDescent 3", 0, plainDBox, (WindowPtr)-1, 0, 0);
  WindowReset((GrafPtr)*ppWin, gDSpContextAttributes[current_context].displayWidth,
              gDSpContextAttributes[current_context].displayHeight);

#ifndef DAJ_DEBUG
  DSpContext_FadeGammaIn(NULL, NULL);
#endif
}
//-----------------------------------------------------------------------------------------------------------------------
// clean up DSp
void ShutdownScreen(CGrafPtr *ppWin) {
  OSStatus err;

#ifndef DAJ_DEBUG
  DSpContext_FadeGammaOut(NULL, NULL);
#endif
  if (*ppWin)
    DisposeWindow((WindowPtr)*ppWin);
  *ppWin = NULL;

  err = DSpContext_SetState(gDSpContext[current_context], kDSpContextState_Inactive);
  if (err)
    Error("%s:%d DSpContext_SetState: Error %d ", __FILE__, __LINE__, err);

#ifndef DAJ_DEBUG
  DSpContext_FadeGammaIn(NULL, NULL);
#endif
  ShowCursor();

#ifdef USE_OPENGL
#ifdef SPROCKET17
  for (int i = 0; i < N_SUPPORTED_VIDRES; i++)
    if (gDSpContext[i])
      DSpContext_Release(gDSpContext[i]);
#else

  err = DSpContext_Release(gDSpContext[current_context]);
  if (err)
    Error("%s:%d DSpContext_Release: Error %d ", __FILE__, __LINE__, err);
  while (gnumDevices--) // dump our reserved unused devices
  {
    err = DSpContext_Release(gpContextRefUnused[gnumDevices]);
    if (err)
      Error("%s:%d DSpContext_Release: Error %d ", __FILE__, __LINE__, err);
  }

#endif
#else
  DSpContext_Release(gDSpContext[DSP_640x480]);
#endif
  DSpShutdown();
}
// Switch between contexts
void SwitchDSpContex(int newContext) {
  extern tMacAppInfo macApp;
  Rect rectWin;

  OSStatus err = 0;
#ifndef USE_OPENGL
  return;
#endif
  if (current_context == newContext)
    return;

  if (gDSpContext[current_context] && gDSpContext[newContext]) {
#ifdef SPROCKET17
    err = DSpContext_Switch(gDSpContext[current_context], gDSpContext[newContext]);
    if (err)
      Error("%s:%d DSpContext_Switch: Error %d ", __FILE__, __LINE__, err);
#else
    // fade out
#ifndef DAJ_DEBUG
    DSpContext_FadeGammaOut(NULL, NULL); // remove for debug
#endif

    HideWindow((GrafPtr)macApp.hwnd);

    // deactivate our context
    err = DSpContext_SetState(gDSpContext[current_context], kDSpContextState_Inactive);
    if (err)
      Error("%s:%d DSpContext_SetState: Error %d ", __FILE__, __LINE__, err);

    // release
    err = DSpContext_Release(gDSpContext[current_context]);
    if (err)
      Error("%s:%d DSpContext_Release: Error %d ", __FILE__, __LINE__, err);

    // reserve
    err = DSpContext_Reserve(gDSpContext[newContext], &gDSpContextAttributes[newContext]);
    if (err)
      Error("%s:%d DSpContext_Reserve: Error %d ", __FILE__, __LINE__, err);

    // activate
    err = DSpContext_SetState(gDSpContext[newContext], kDSpContextState_Active);
    if (err)
      Error("%s:%d DSpContext_SetState: Error %d ", __FILE__, __LINE__, err);

    WindowReset((GrafPtr)macApp.hwnd, gDSpContextAttributes[newContext].displayWidth,
                gDSpContextAttributes[newContext].displayHeight);

    // fade in
#ifndef DAJ_DEBUG
    DSpContext_FadeGammaIn(NULL, NULL);
#endif
#endif
    current_context = newContext;
  }
}
void PauseDSpContext() {
  OSStatus err = 0;

  extern tMacAppInfo macApp;

  if (gDSpContext[current_context] == NULL)
    return;

  opengl_DettachContext();

  //	HideWindow((GrafPtr)macApp.hwnd);

  err = DSpContext_SetState(gDSpContext[current_context], kDSpContextState_Paused);
  if (err)
    Error("%s:%d DSpContext_SetState: Error %d ", __FILE__, __LINE__, err);
}

void ResumeDSpContext() {
  OSStatus err = 0;

  if (gDSpContext[current_context] == NULL)
    return;
  err = DSpContext_SetState(gDSpContext[current_context], kDSpContextState_Active);
  if (err)
    Error("%s:%d DSpContext_SetState: Error %d ", __FILE__, __LINE__, err);

  extern tMacAppInfo macApp;
  opengl_AttachContext((CGrafPtr)macApp.hwnd);

  WindowReset((GrafPtr)macApp.hwnd, gDSpContextAttributes[current_context].displayWidth,
              gDSpContextAttributes[current_context].displayHeight);
}

// Window Reset

void WindowReset(WindowPtr pWindow, short width, short height) {
  RGBColor rgbSave;
  GrafPtr pGrafSave;
  short x, y;

  y = (short)((**hGD).gdRect.top + (((**hGD).gdRect.bottom - (**hGD).gdRect.top) - height) / 2); // h start
  x = (short)((**hGD).gdRect.left + (((**hGD).gdRect.right - (**hGD).gdRect.left) - width) / 2); // v start

  MoveWindow(pWindow, x, y, true);
  SizeWindow(pWindow, width, height, false);
  ShowWindow(pWindow); // will get a white flash here if not faded

  GetPort(&pGrafSave);
  SetPort(pWindow);
  GetForeColor(&rgbSave);
  RGBForeColor(&rgbBlack);
  PaintRect(&(pWindow->portRect));
  RGBForeColor(&rgbSave); // ensure color is reset for proper blitting
  SetPort(pGrafSave);
}
