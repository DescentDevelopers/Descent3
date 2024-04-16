// ----------------------------------------------------------------------------
//	Mouse Interface
// ----------------------------------------------------------------------------

#include <stdlib.h>

#include "DDAccess.h"
#include "pserror.h"
#include "mono.h"
#include "ddio.h"
#include "ddio_win.h"
#include "Application.h"
#include "psclass.h"
#include "Macros.h"

typedef struct t_mse_button_info {
  bool is_down[N_MSEBTNS];
  ubyte down_count[N_MSEBTNS];
  ubyte up_count[N_MSEBTNS];
  DWORD time_down[N_MSEBTNS]; // in milliseconds windows ticks
  DWORD time_up[N_MSEBTNS];
} t_mse_button_info;

typedef struct t_mse_event {
  short btn;
  short state;
} t_mse_event;

#define MOUSE_ZMIN 0 // mouse wheel z min and max (increments of 120 = 10 units)
#define MOUSE_ZMAX 1200
#define N_DIMSEBTNS 4                     // # of REAL mouse buttons
#define MSEBTN_WHL_UP (N_DIMSEBTNS)       // button index for mouse wheel up
#define MSEBTN_WHL_DOWN (N_DIMSEBTNS + 1) // button index for mouse wheel down

// taken from winuser.h
#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x20a
#endif

// ----------------------------------------------------------------------------

static bool DDIO_mouse_init = 0;

static struct mses_state {
  LPDIRECTINPUTDEVICE lpdimse;
  RECT brect;              // limit rectangle of absolute mouse coords
  short x, y, z;           // current x,y,z in absolute mouse coords
  short cx, cy, cz;        // prior values of x,y,z from last mouse frame
  short zmin, zmax;        // 3 dimensional mouse devices use this
  int btn_mask, btn_flags; // btn_flags are the avaiable buttons on this device in mask form.
  bool emulated;           // are we emulating direct input?
  bool acquired;
  bool suspended;
  sbyte cursor_count;
  float x_aspect, y_aspect; // used in calculating coordinates returned from ddio_MouseGetState
  HANDLE hmseevt;           // signaled if mouse input is awaiting.
  short dx, dy, dz, imm_dz;
  short mode;         // mode of mouse operation.
  short nbtns, naxis; // device caps.
} DDIO_mouse_state;

static t_mse_button_info DIM_buttons;
static tQueue<t_mse_event, 16> MB_queue;

// ----------------------------------------------------------------------------

// di_mouse initialization.
LPDIRECTINPUTDEVICE dimouse_Init(LPDIRECTINPUT lpdi, HWND hwnd);

// di_mouse close
void dimouse_Shutdown(LPDIRECTINPUTDEVICE lpdimse);

// di_mouse acquire device, returns acquisition state of device
bool dimouse_Acquire(LPDIRECTINPUTDEVICE lpdimse, bool acquire);

// returns device data.
void dimouse_GetDeviceData(LPDIRECTINPUTDEVICE lpdimse);

// emulated get device state
void winmouse_GetDeviceData(POINT &pt);

inline void DDIOShowCursor(BOOL show) {
  if (show) {
    if (DDIO_mouse_state.cursor_count == -1) {
      ShowCursor(TRUE);
    }
    DDIO_mouse_state.cursor_count = 0;
  } else {
    if (DDIO_mouse_state.cursor_count == 0) {
      ShowCursor(FALSE);
    }
    DDIO_mouse_state.cursor_count = -1;
  }
}

//	---------------------------------------------------------------------------
//	Initialization Functions
extern bool w32_mouseman_hack;

bool ddio_MouseInit() {
  tWin32OS os;
  int major, minor;

  // see if we need to emulate directinput.
  os = oeWin32Application::version(&major, &minor);
  DDIO_mouse_state.emulated = (os == WinNT && major < 5) ? true : false;
  DDIO_mouse_state.lpdimse = NULL;
  if (w32_mouseman_hack) {
    DDIO_mouse_state.emulated = true;
  }

  if (!DDIO_mouse_state.emulated) {
    //	initialize direct input version?  attempt acquisition if so
    DDIO_mouse_state.lpdimse = dimouse_Init(DInputData.lpdi, (HWND)DInputData.app->m_hWnd);
    if (DDIO_mouse_state.lpdimse) {
      DDIO_mouse_state.acquired = dimouse_Acquire(DDIO_mouse_state.lpdimse, true);
    }
  } else {
    DDIO_mouse_state.nbtns = 3;
    DDIO_mouse_state.naxis = 2;
    DDIO_mouse_state.btn_flags = (MOUSE_LB + MOUSE_RB + MOUSE_CB);
    if (DDIO_mouse_state.naxis >= 3 || w32_mouseman_hack) {
      DDIO_mouse_state.zmin = MOUSE_ZMIN;
      DDIO_mouse_state.zmax = MOUSE_ZMAX;
      DDIO_mouse_state.nbtns = N_DIMSEBTNS + 2; // 2 next button slots reserved for mouse wheel.
      DDIO_mouse_state.btn_flags |= (1 << MSEBTN_WHL_UP);
      DDIO_mouse_state.btn_flags |= (1 << MSEBTN_WHL_DOWN);
    }
  }

  // standard initialization
  DDIO_mouse_state.emulated = (DDIO_mouse_state.lpdimse) ? false : true;
  DDIO_mouse_state.cursor_count = ShowCursor(TRUE); // get initial count
  while (DDIO_mouse_state.cursor_count >= 0)        // hide cursor until truly hidden.
  {
    DDIO_mouse_state.cursor_count = ShowCursor(FALSE);
  }

  ddio_MouseMode(MOUSE_STANDARD_MODE);

  DDIO_mouse_state.suspended = false;
  ddio_MouseReset();
  DDIO_mouse_init = true;

  return true;
}

// here we deinitialize our Mouse from DirectInput.
void ddio_MouseClose() {
  if (!DDIO_mouse_init)
    return;

  // direct input shutdown.
  if (DDIO_mouse_state.emulated == false) {
    dimouse_Shutdown(DDIO_mouse_state.lpdimse);
  }

  DDIO_mouse_init = false;
}

// get device caps
int ddio_MouseGetCaps(int *btn, int *axis) {
  *btn = (int)DDIO_mouse_state.nbtns;
  *axis = (int)DDIO_mouse_state.naxis;

  return DDIO_mouse_state.btn_flags;
}

void ddio_MouseMode(int mode) {
  if (mode == MOUSE_EXCLUSIVE_MODE) {
    DDIOShowCursor(FALSE);
  } else if (mode == MOUSE_STANDARD_MODE) {
    DDIOShowCursor(TRUE);
  } else {
    Int3();
    return;
  }

  DDIO_mouse_state.mode = mode;
}

//	ddio_MouseReset
//		resets the mouse system, resetting boundaries to application window size, and mouse pointer
//		to the center of the screen.
void ddio_MouseQueueFlush() {
  ZeroMemory(&DIM_buttons, sizeof(DIM_buttons));
  MB_queue.flush();
}

void ddio_MouseReset() {
  tWin32AppInfo appi;

  DInputData.app->get_info(&appi);
  DDIO_mouse_state.cx = appi.wnd_x + (appi.wnd_w / 2);
  DDIO_mouse_state.cy = appi.wnd_y + (appi.wnd_h / 2);

  SetRect(&DDIO_mouse_state.brect, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
  DDIO_mouse_state.zmin = MOUSE_ZMIN;
  DDIO_mouse_state.zmax = MOUSE_ZMAX;

  //	set up new coordinates for mouse pointer.
  DDIO_mouse_state.btn_mask = 0;
  DDIO_mouse_state.dx = 0;
  DDIO_mouse_state.dy = 0;
  DDIO_mouse_state.dz = 0;
  DDIO_mouse_state.imm_dz = 0;
  DDIO_mouse_state.x = (short)(DDIO_mouse_state.brect.right - DDIO_mouse_state.brect.left) / 2;
  DDIO_mouse_state.y = (short)(DDIO_mouse_state.brect.bottom - DDIO_mouse_state.brect.top) / 2;
  DDIO_mouse_state.z = (DDIO_mouse_state.zmax = DDIO_mouse_state.zmin) / 2;
  DDIO_mouse_state.cz = 0;
  DDIO_mouse_state.x_aspect = 1.0f;
  DDIO_mouse_state.y_aspect = 1.0f;

  // reset button states
  ddio_MouseQueueFlush();
}

//	gets limits on the position of the mouse cursor (or atleast what's returned from GetState)
void ddio_MouseGetLimits(int *left, int *top, int *right, int *bottom, int *zmin, int *zmax) {
  *left = DDIO_mouse_state.brect.left;
  *top = DDIO_mouse_state.brect.top;
  *right = DDIO_mouse_state.brect.right;
  *bottom = DDIO_mouse_state.brect.bottom;

  if (zmin)
    *zmin = DDIO_mouse_state.zmin;
  if (zmax)
    *zmax = DDIO_mouse_state.zmax;
}

//	sets limits on the position of the mouse cursor (or atleast what's returned from GetState)
void ddio_MouseSetLimits(int left, int top, int right, int bottom, int zmin, int zmax) {
  bool zaxis = (DDIO_mouse_state.naxis >= 3);
  SetRect(&DDIO_mouse_state.brect, left, top, right, bottom);
  DDIO_mouse_state.zmin = (!zmin && zaxis) ? MOUSE_ZMIN : zmin;
  DDIO_mouse_state.zmax = (!zmax && zaxis) ? MOUSE_ZMAX : zmax;
  DDIO_mouse_state.cx = left + (right - left) / 2;
  DDIO_mouse_state.cy = top + (bottom - top) / 2;
}

// virtual coordinate system for mouse (match to video resolution set for optimal mouse usage.
void ddio_MouseSetVCoords(int width, int height) { ddio_MouseSetLimits(0, 0, width, height); }

/*	x, y = absolute mouse position
        dx, dy = mouse deltas since last call
        return value is mouse button mask
*/
int ddio_MouseGetState(int *x, int *y, int *dx, int *dy, int *z, int *dz) {
  int btn_mask = DDIO_mouse_state.btn_mask;

  //	get return values.
  if (x)
    *x = DDIO_mouse_state.x;
  if (y)
    *y = DDIO_mouse_state.y;
  if (z)
    *z = DDIO_mouse_state.z;
  if (dx)
    *dx = DDIO_mouse_state.dx;
  if (dy)
    *dy = DDIO_mouse_state.dy;
  if (dz)
    *dz = DDIO_mouse_state.dz;

#ifdef _DEBUG
  mprintf_at((4, 0, 0, "MSE:X:%04d Y:%04d dX:%04d dY:%04d    WM_MOUSEWHEEL:%04d", DDIO_mouse_state.x,
              DDIO_mouse_state.y, DDIO_mouse_state.dx, DDIO_mouse_state.dy, w32_msewhl_delta));
#endif

  DDIO_mouse_state.dx = 0;
  DDIO_mouse_state.dy = 0;
  DDIO_mouse_state.dz = 0;
  DDIO_mouse_state.btn_mask = 0;

  return btn_mask;
}

// gets a mouse button event, returns false if none.
bool ddio_MouseGetEvent(int *btn, bool *state) {
  t_mse_event evt;

  if (MB_queue.recv(&evt)) {
    *btn = (int)evt.btn;
    *state = evt.state ? true : false;
    return true;
  }

  return false;
}

// return mouse button down time.
float ddio_MouseBtnDownTime(int btn) {
  DWORD ticks, curticks = GetTickCount();

  ASSERT(btn >= 0 && btn < N_MSEBTNS);

  if (DIM_buttons.is_down[btn]) {
    ticks = curticks - DIM_buttons.time_down[btn];
    DIM_buttons.time_down[btn] = curticks;
  } else {
    ticks = DIM_buttons.time_up[btn] - DIM_buttons.time_down[btn];
    DIM_buttons.time_down[btn] = 0;
    DIM_buttons.time_up[btn] = 0;
  }

  return (float)(ticks / 1000.0f);
}

// return mouse button down time
int ddio_MouseBtnDownCount(int btn) {
  if (btn < 0 || btn >= N_MSEBTNS)
    return 0;
  int n_downs = DIM_buttons.down_count[btn];

  if (n_downs) {
    DIM_buttons.down_count[btn] = 0;
  }

  return n_downs;
}

// return mouse button up count
int ddio_MouseBtnUpCount(int btn) {
  if (btn < 0 || btn >= N_MSEBTNS)
    return 0;
  int n_ups = DIM_buttons.up_count[btn];
  DIM_buttons.up_count[btn] = 0;
  return n_ups;
}

///////////////////////////////////////////////////////////////////////////////
// called by ddio system.
///////////////////////////////////////////////////////////////////////////////

void ddio_InternalMouseFrame() {
  DWORD res;
  POINT pt;

  if (!DDIO_mouse_init)
    return;

  if (DDIO_mouse_state.suspended) {
    DDIO_mouse_state.btn_mask = 0;
    DDIO_mouse_state.dx = 0;
    DDIO_mouse_state.dy = 0;
    DDIO_mouse_state.dz = 0;
    return;
  }

  GetCursorPos(&pt);

  //	in emulation mode
  if (DDIO_mouse_state.emulated) {
    winmouse_GetDeviceData(pt);
  } else {
    // check if event has been signaled.
    res = WaitForSingleObject(DDIO_mouse_state.hmseevt, 0);

    switch (res) {
    case WAIT_OBJECT_0:
      dimouse_GetDeviceData(DDIO_mouse_state.lpdimse);
      ResetEvent(DDIO_mouse_state.hmseevt); // clear for next frame.
      break;
    }
  }

  // create button mask now that DIM_buttons should be valid.
  int btn_mask = 0;

  if (DIM_buttons.is_down[0])
    btn_mask |= MOUSE_LB;
  if (DIM_buttons.is_down[1])
    btn_mask |= MOUSE_RB;
  if (DIM_buttons.is_down[2])
    btn_mask |= MOUSE_CB;
  if (DIM_buttons.is_down[3])
    btn_mask |= MOUSE_B4;
  if (DIM_buttons.is_down[4])
    btn_mask |= MOUSE_B5;
  if (DIM_buttons.is_down[5])
    btn_mask |= MOUSE_B6;
  if (DIM_buttons.is_down[6])
    btn_mask |= MOUSE_B7;
  if (DIM_buttons.is_down[7])
    btn_mask |= MOUSE_B8;

  // reset button states for wheel up and down, since we're emulating them.
  DIM_buttons.is_down[MSEBTN_WHL_UP] = false;
  DIM_buttons.is_down[MSEBTN_WHL_DOWN] = false;

  DDIO_mouse_state.btn_mask = btn_mask;

  // cleanup for specific mouse modes.
  if (DDIO_mouse_state.mode == MOUSE_STANDARD_MODE) {
    // figure out where the top-left of the screen is
    RECT clientRect;
    GetClientRect((HWND)DInputData.app->m_hWnd, &clientRect);

    POINT topLeftWindow;
    topLeftWindow.x = clientRect.left;
    topLeftWindow.y = clientRect.top;
    ClientToScreen((HWND)DInputData.app->m_hWnd, &topLeftWindow);

    // if in standard mode, don't use x,y,z retrieved from dimouse_GetDeviceData
    DDIO_mouse_state.x = (short)(pt.x - topLeftWindow.x);
    DDIO_mouse_state.y = (short)(pt.y - topLeftWindow.y);
    DDIO_mouse_state.z = 0;
  }

  if (DDIO_mouse_state.mode == MOUSE_EXCLUSIVE_MODE) {
    if (pt.x != DDIO_mouse_state.cx || pt.y != DDIO_mouse_state.cy) {
      SetCursorPos(DDIO_mouse_state.cx, DDIO_mouse_state.cy);
    }
  }

  //	check bounds of mouse cursor.
  if (DDIO_mouse_state.x < DDIO_mouse_state.brect.left)
    DDIO_mouse_state.x = (short)DDIO_mouse_state.brect.left;
  if (DDIO_mouse_state.x >= DDIO_mouse_state.brect.right)
    DDIO_mouse_state.x = (short)DDIO_mouse_state.brect.right - 1;
  if (DDIO_mouse_state.y < DDIO_mouse_state.brect.top)
    DDIO_mouse_state.y = (short)DDIO_mouse_state.brect.top;
  if (DDIO_mouse_state.y >= DDIO_mouse_state.brect.bottom)
    DDIO_mouse_state.y = (short)DDIO_mouse_state.brect.bottom - 1;
  if (DDIO_mouse_state.z > DDIO_mouse_state.zmax)
    DDIO_mouse_state.z = (short)DDIO_mouse_state.zmax;
  if (DDIO_mouse_state.z < DDIO_mouse_state.zmin)
    DDIO_mouse_state.z = (short)DDIO_mouse_state.zmin;
}

// used to prevent mouse input from being registered
void ddio_InternalMouseSuspend() {
  if (!DDIO_mouse_init)
    return;

  if (DDIO_mouse_state.lpdimse) {
    DDIO_mouse_state.acquired = dimouse_Acquire(DDIO_mouse_state.lpdimse, false);
  }
  DDIO_mouse_state.suspended = true;
}

void ddio_InternalMouseResume() {
  if (!DDIO_mouse_init)
    return;

  if (DDIO_mouse_state.lpdimse) {
    DDIO_mouse_state.acquired = dimouse_Acquire(DDIO_mouse_state.lpdimse, true);
  }
  DDIO_mouse_state.suspended = false;
}

///////////////////////////////////////////////////////////////////////////////
//	DirectInput mouse interface
///////////////////////////////////////////////////////////////////////////////

#define DINPUT_BUFFERSIZE 64

// di_mouse initialization.
LPDIRECTINPUTDEVICE dimouse_Init(LPDIRECTINPUT lpdi, HWND hwnd) {
  LPDIRECTINPUTDEVICE lpdimse;
  HRESULT hr;
  HANDLE hevt;
  DIDEVCAPS devcaps;
  DIPROPDWORD dipropdw = {{
                              sizeof(DIPROPDWORD),
                              sizeof(DIPROPHEADER),
                              0,
                              DIPH_DEVICE,
                          },
                          DIPROPAXISMODE_REL};
  int i;

  DDIO_mouse_state.hmseevt = NULL;

  // see if we need to emulate directinput.
  int major, minor;
  tWin32OS os = oeWin32Application::version(&major, &minor);
  if (os == WinNT && major < 5) {
    return NULL;
  }

  // see if we can get the keyboard device.
  hr = lpdi->CreateDevice(GUID_SysMouse, &lpdimse, NULL);
  if (hr != DI_OK) {
    DDIO_MESSAGE((hr, "DI_Mouse initialization failed."));
    return NULL;
  }

  hr = lpdimse->SetDataFormat(&c_dfDIMouse);
  if (hr != DI_OK) {
    DDIO_MESSAGE((hr, "DI_Mouse data format specification failed."));
    lpdimse->Release();
    return NULL;
  }

  // set key buffer size
  hr = lpdimse->SetProperty(DIPROP_AXISMODE, &dipropdw.diph);
  if (FAILED(hr)) {
    DDIO_MESSAGE((hr, "DI_mouse relative mode failed.."));
    lpdimse->Release();
    return NULL;
  }

  hr = lpdimse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
  if (hr != DI_OK) {
    DDIO_MESSAGE((hr, "DI_Mouse set cooperative level failed."));
    lpdimse->Release();
    return NULL;
  }

  // enable event notification.
  hevt = CreateEvent(NULL, TRUE, FALSE, "DDIOMouseEvent");
  if (!hevt) {
    DDIO_MESSAGE((0, "DI_mouse data event create failed."));
    lpdimse->Release();
    return NULL;
  }

  hr = lpdimse->SetEventNotification(hevt);
  if (FAILED(hr)) {
    DDIO_MESSAGE((0, "DI_mouse event notification failed."));
    CloseHandle(hevt);
    lpdimse->Release();
    return NULL;
  }

  // use buffered input.
  DIPROPDWORD dipdw = {
      // the header
      {
          sizeof(DIPROPDWORD),  // diph.dwSize
          sizeof(DIPROPHEADER), // diph.dwHeaderSize
          0,                    // diph.dwObj
          DIPH_DEVICE,          // diph.dwHow
      },
      // the data
      DINPUT_BUFFERSIZE, // dwData
  };

  hr = lpdimse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);

  if (FAILED(hr)) {
    DDIO_MESSAGE((hr, "DI_mouse Set buffer size(SysMouse)"));
    CloseHandle(hevt);
    lpdimse->Release();
    return NULL;
  }

  // we have a handle
  DDIO_mouse_state.hmseevt = hevt;

  // determine if there's a mousewheel (z-axis) attached.
  devcaps.dwSize = sizeof(DIDEVCAPS);
  hr = lpdimse->GetCapabilities(&devcaps);
  if (FAILED(hr) || !CHECK_FLAG(devcaps.dwFlags, DIDC_ATTACHED)) {
    DDIO_MESSAGE((hr, "DI_mouse: GetCaps failed"));
    CloseHandle(hevt);
    lpdimse->Release();
    return NULL;
  }

  DDIO_mouse_state.naxis = (short)((devcaps.dwAxes > 3) ? 3 : devcaps.dwAxes);
  DDIO_mouse_state.nbtns = N_DIMSEBTNS; // cap at max number of buttons directinput accepts for mice
  DDIO_mouse_state.btn_flags = 0;
  for (i = 0; (i < (int)devcaps.dwButtons) && (i < N_DIMSEBTNS); i++) {
    DDIO_mouse_state.btn_flags |= (1 << i);
  }

  if (DDIO_mouse_state.naxis >= 3) {
    DDIO_mouse_state.zmin = MOUSE_ZMIN;
    DDIO_mouse_state.zmax = MOUSE_ZMAX;
    DDIO_mouse_state.nbtns = N_DIMSEBTNS + 2; // 2 next button slots reserved for mouse wheel.
    DDIO_mouse_state.btn_flags |= (1 << MSEBTN_WHL_UP);
    DDIO_mouse_state.btn_flags |= (1 << MSEBTN_WHL_DOWN);
  }

  return lpdimse;
}

// di_mouse close
void dimouse_Shutdown(LPDIRECTINPUTDEVICE lpdimse) {
  if (lpdimse) {
    if (DDIO_mouse_state.hmseevt) {
      lpdimse->SetEventNotification(NULL);
      CloseHandle(DDIO_mouse_state.hmseevt);
      DDIO_mouse_state.hmseevt = NULL;
    }
    lpdimse->Release();
  }
}

// di_mouse acquire device, returns acquisition state of device
bool dimouse_Acquire(LPDIRECTINPUTDEVICE lpdimse, bool acquire) {
  HRESULT hr = acquire ? lpdimse->Acquire() : lpdimse->Unacquire();

  if (FAILED(hr)) {
    mprintf((0, "ACK: %x\n", hr));
  }

  return (acquire && SUCCEEDED(hr)) ? true : false;
}

// returns device data.
void dimouse_GetDeviceData(LPDIRECTINPUTDEVICE lpdimse) {
  DIDEVICEOBJECTDATA diobjdata[DINPUT_BUFFERSIZE];
  DWORD diobjitems = DINPUT_BUFFERSIZE;
  HRESULT hr;
  int i;

  DDIO_mouse_state.dx = 0;
  DDIO_mouse_state.dy = 0;
  DDIO_mouse_state.dz = 0;

  // don't read if suspended!
  if (DDIO_mouse_state.suspended) {
    return;
  }

  // attempt to acquire device.
  if (!DDIO_mouse_state.acquired) {
    DDIO_mouse_state.acquired = dimouse_Acquire(lpdimse, true);
  }

  // read device.
  if (DDIO_mouse_state.acquired) {
    hr = lpdimse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), diobjdata, &diobjitems, 0);
    if (SUCCEEDED(hr)) {
      // read and translate data
      for (i = 0; i < (int)diobjitems; i++) {
        int idx = -1;
        t_mse_event evt;

        switch (diobjdata[i].dwOfs) {
        case DIMOFS_X:
          DDIO_mouse_state.x += (int)diobjdata[i].dwData;
          DDIO_mouse_state.dx += (int)diobjdata[i].dwData;
          break;
        case DIMOFS_Y:
          DDIO_mouse_state.y += (int)diobjdata[i].dwData;
          DDIO_mouse_state.dy += (int)diobjdata[i].dwData;
          break;
        case DIMOFS_Z:
          // for z, simulate mouse press for emulated mouse buttons.
          DDIO_mouse_state.z += (int)diobjdata[i].dwData;
          DDIO_mouse_state.dz += (int)diobjdata[i].dwData;
          DDIO_mouse_state.imm_dz += (int)diobjdata[i].dwData;
          //	mprintf((0, "Zmse=%d\n", diobjdata[i].dwData));
          if (DDIO_mouse_state.imm_dz >= WHEEL_DELTA) {
            idx = MSEBTN_WHL_UP;
            DDIO_mouse_state.imm_dz = 0;
          } else if (DDIO_mouse_state.imm_dz <= (-WHEEL_DELTA)) {
            idx = MSEBTN_WHL_DOWN;
            DDIO_mouse_state.imm_dz = 0;
          }
          if (idx != -1) {
            DIM_buttons.down_count[idx]++;
            DIM_buttons.up_count[idx]++;
            DIM_buttons.is_down[idx] = true; // reset in internal mouse frame.
            DIM_buttons.time_down[idx] = diobjdata[i].dwTimeStamp;
            DIM_buttons.time_up[idx] = (diobjdata[i].dwTimeStamp + 100); // 10th of a second.
          }
          break;
        case DIMOFS_BUTTON0:
          idx = (idx == -1) ? 0 : idx;
        case DIMOFS_BUTTON1:
          idx = (idx == -1) ? 1 : idx;
        case DIMOFS_BUTTON2:
          idx = (idx == -1) ? 2 : idx;
        case DIMOFS_BUTTON3:
          idx = (idx == -1) ? 3 : idx;
          evt.btn = (short)idx;
          if ((diobjdata[i].dwData & 0x80) && !DIM_buttons.is_down[idx]) {
            DIM_buttons.down_count[idx]++;
            DIM_buttons.is_down[idx] = true;
            DIM_buttons.time_down[idx] = diobjdata[i].dwTimeStamp;
            evt.state = 1;
            MB_queue.send(evt);
          } else if (!(diobjdata[i].dwData & 0x80) && DIM_buttons.is_down[idx]) {
            DIM_buttons.up_count[idx]++;
            DIM_buttons.is_down[idx] = false;
            DIM_buttons.time_up[idx] = diobjdata[i].dwTimeStamp;
            evt.state = 0;
            MB_queue.send(evt);
          }
          break;

        default:
          mprintf((0, "di:unknown mouse event (%x,%x)\n", diobjdata[i].dwOfs, diobjdata[i].dwData));
          break;
        }
      }
    } else if (hr == DIERR_INPUTLOST) {
      DDIO_mouse_state.acquired = dimouse_Acquire(lpdimse, true);
    } else {
      DDIO_MESSAGE((hr, "DI_mouse unable to read."));
    }
  }
}

// emulated get device state
void winmouse_GetDeviceData(POINT &pt) {
  t_mse_event evt;
  short mode = DDIO_mouse_state.mode;

  DDIO_mouse_state.dx = (short)pt.x - ((mode == MOUSE_STANDARD_MODE) ? DDIO_mouse_state.x : DDIO_mouse_state.cx);
  DDIO_mouse_state.dy = (short)pt.y - ((mode == MOUSE_STANDARD_MODE) ? DDIO_mouse_state.y : DDIO_mouse_state.cy);
  DDIO_mouse_state.dz = 0;

  if ((ushort)GetKeyState(VK_LBUTTON) & 0x8000) {
    if (!DIM_buttons.is_down[0]) {
      DIM_buttons.down_count[0]++;
      DIM_buttons.time_down[0] = GetTickCount();
      DIM_buttons.is_down[0] = true;

      evt.btn = 0;
      evt.state = true;
      MB_queue.send(evt);
      mprintf((0, "MOUSE Button 0: Down\n"));
    }
  } else if (DIM_buttons.is_down[0]) {
    DIM_buttons.up_count[0]++;
    DIM_buttons.is_down[0] = false;
    DIM_buttons.time_up[0] = GetTickCount();
    evt.btn = 0;
    evt.state = false;
    MB_queue.send(evt);
    mprintf((0, "MOUSE Button 0: Up\n"));
  }

  if ((ushort)GetKeyState(VK_RBUTTON) & 0x8000) {
    if (!DIM_buttons.is_down[1]) {
      DIM_buttons.down_count[1]++;
      DIM_buttons.time_down[1] = GetTickCount();
      DIM_buttons.is_down[1] = true;
      evt.btn = 1;
      evt.state = true;
      MB_queue.send(evt);
    }
  } else if (DIM_buttons.is_down[1]) {
    DIM_buttons.up_count[1]++;
    DIM_buttons.is_down[1] = false;
    DIM_buttons.time_up[1] = GetTickCount();
    evt.btn = 1;
    evt.state = false;
    MB_queue.send(evt);
  }

  if ((ushort)GetKeyState(VK_MBUTTON) & 0x8000) {
    if (!DIM_buttons.is_down[2]) {
      DIM_buttons.down_count[2]++;
      DIM_buttons.time_down[2] = GetTickCount();
      DIM_buttons.is_down[2] = true;
      evt.btn = 2;
      evt.state = true;
      MB_queue.send(evt);
    }
  } else if (DIM_buttons.is_down[2]) {
    DIM_buttons.up_count[2]++;
    DIM_buttons.is_down[2] = false;
    DIM_buttons.time_up[2] = GetTickCount();
    evt.btn = 2;
    evt.state = false;
    MB_queue.send(evt);
  }

  if (mode == MOUSE_EXCLUSIVE_MODE) {
    DDIO_mouse_state.x += DDIO_mouse_state.dx;
    DDIO_mouse_state.y += DDIO_mouse_state.dy;
    DDIO_mouse_state.z += DDIO_mouse_state.dz;
  }

  int idx = -1;
  DDIO_mouse_state.imm_dz += w32_msewhl_delta;
  if (DDIO_mouse_state.imm_dz >= WHEEL_DELTA) {
    idx = MSEBTN_WHL_UP;
    DDIO_mouse_state.imm_dz = 0;
  } else if (DDIO_mouse_state.imm_dz <= (-WHEEL_DELTA)) {
    idx = MSEBTN_WHL_DOWN;
    DDIO_mouse_state.imm_dz = 0;
  }
  if (idx != -1) {
    DIM_buttons.down_count[idx]++;
    DIM_buttons.up_count[idx]++;
    DIM_buttons.is_down[idx] = true; // reset in internal mouse frame.
    DIM_buttons.time_down[idx] = GetTickCount();
    DIM_buttons.time_up[idx] = (DIM_buttons.time_down[idx] + 100); // 10th of a second.
    mprintf((0, "registered mouse wheel event %d\n", idx));
  }
}

///////////////////////////////////////////////////////////////////////////////
char Ctltext_MseBtnBindings[N_MSEBTNS][32] = {"mse-l\0\0\0\0\0\0\0\0\0\0\0\0",
                                              "mse-r\0\0\0\0\0\0\0\0\0\0\0\0",
                                              "mse-c\0\0\0\0\0\0\0\0\0\0\0\0",
                                              "mse-b4\0\0\0\0\0\0\0\0\0\0\0",
                                              "msew-u\0\0\0\0\0\0\0\0\0\0\0",
                                              "msew-d\0\0\0\0\0\0\0\0\0\0\0",
                                              "",
                                              ""};

char Ctltext_MseAxisBindings[][32] = {"mse-X\0\0\0\0\0\0\0\0\0\0\0\0", "mse-Y\0\0\0\0\0\0\0\0\0\0\0\0",
                                      "msewheel\0\0\0\0\0\0\0\0\0\0"};

// returns string to binding.
const char *ddio_MouseGetBtnText(int btn) {
  if (btn >= N_MSEBTNS || btn < 0)
    return ("");
  return Ctltext_MseBtnBindings[btn];
}

const char *ddio_MouseGetAxisText(int axis) {
  if (axis >= (sizeof(Ctltext_MseAxisBindings) / sizeof(char *)) || axis < 0)
    return ("");
  return Ctltext_MseAxisBindings[axis];
}
