/*
 * $Logfile: /DescentIII/Main/mac/MACAPP.CPP $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:58:14 $
 * $Author: kevinb $
 *
 *	Mac Application Object.  Encapsulates some app info for libraries
 *
 * $Log: MACAPP.CPP,v $
 * Revision 1.1.1.1  2003/08/26 03:58:14  kevinb
 * initial 1.5 import
 *
 *
 * 2     10/21/99 1:55p Kevin
 * Mac Merge!
 *
 * 1     7/28/99 2:31p Kevin
 * Mac only stuff
 *
 *
 * $NoKeywords: $
 */
#define OEAPP_INTERNAL_MODULE
#include "Application.h"
#include "AppConsole.h"
#include "mono.h"
#include "networking.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pserror.h"
#include "ddio.h"
#include "mem.h"
// taken from Macuser.h
#ifndef WHEEL_DELTA
#define WHEEL_DELTA 120
#endif
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x20a
#endif
/* Main windows Procedure for this OS object
 */
const int MAX_MacAPPS = 4;
static struct tAppNodes {
  WindowPtr hWnd;
  oeMacApplication *app;
} Mac_AppObjects[MAX_MacAPPS];
// system mouse info.
short w32_msewhl_delta = 0; // -val = up, pos val = down, 0 = no change.
bool w32_mouseman_hack = false;
/*	Mac Application Object
                This object entails initialization and cleanup of all operating system
                elements, as well as data that libraries may need to initialize their
                systems.
        The Mac Application object creates the application window and housekeeps
        the window and instance handle for the application.
        We also allow the option of setting these handles from outside the Application object.
*/
extern void con_Defer();
bool oeMacApplication::os_initialized = false;
bool oeMacApplication::first_time = true;
CGrafPtr pWin;
// short numDevices = 0;
//	Creates the window handle and instance
oeMacApplication::oeMacApplication(const char *name, unsigned flags) //: oeApplication()
{
  Rect rect;

  if (oeMacApplication::first_time) {
    int i;
    for (i = 0; i < MAX_MacAPPS; i++) {
      Mac_AppObjects[i].hWnd = NULL;
      Mac_AppObjects[i].app = NULL;
    }
    oeMacApplication::first_time = false;

    mem_Init();
  }
  if (flags & OEAPP_CONSOLE) {
    m_X = CW_USEDEFAULT;
    m_Y = CW_USEDEFAULT;
    m_W = 640;
    m_H = 480;
  } else {
    //	initialize main window and display it.
#if 0
#ifdef RELEASE
		rect = qd.screenBits.bounds;
#else
		SetRect(&rect, 0,0,640,480);
#endif
		m_X = 0;
		m_Y = 0;
		m_W = rect.right - rect.left;
		m_H = rect.bottom - rect.top;
#endif
  }
  m_Flags = flags;
  strcpy(m_WndName, name);
  os_init();
  m_hWnd = NULL;
  m_WasCreated = true;
  m_DeferFunc = NULL;
#ifndef DAJ_DEBUG
  // #if 0
  SetupScreen(&m_hWnd);
#else
#ifdef USE_OPENGL
  SetRect(&rect, 0, 20, 640, 500);
  m_hWnd = (CGrafPort *)NewCWindow(NULL, &rect, "\pDescent 3", false, plainDBox, (WindowPtr)-1L, true, 0L);
  ShowWindow((GrafPort *)m_hWnd);
  rect = (*m_hWnd->portPixMap)->bounds;
  m_X = rect.left;
  m_Y = rect.bottom;
  m_W = rect.right - rect.left;
  m_H = rect.bottom - rect.top;
#endif
#endif
}

//	Create object with a premade window handle/instance
oeMacApplication::oeMacApplication(tMacAppInfo *appinfo) // :oeApplication()
{
  Rect rect;
  //	store handles
  m_hWnd = appinfo->hwnd;
  m_Flags = appinfo->flags;
  //	returns the dimensions of the window
  rect = m_hWnd->portRect;
  //	rect = (**((WindowPeek)m_hWnd)->contRgn).rgnBBox;
  appinfo->wnd_x = m_X = rect.left;
  appinfo->wnd_y = m_Y = rect.top;
  appinfo->wnd_w = m_W = rect.right - rect.left;
  appinfo->wnd_h = m_H = rect.bottom - rect.top;
  m_WasCreated = false;
  os_init();
  clear_handlers();
  m_DeferFunc = NULL;
}

oeMacApplication::~oeMacApplication() {
  //	do this only if we created the window, not just initializing the window
  if (m_WasCreated) {
    if (m_hWnd) {
#ifndef DAJ_DEBUG
      // #if 1
      ShutdownScreen(&m_hWnd);
#else
#ifdef USE_OPENGL
      CloseWindow((GrafPort *)m_hWnd);
#endif
#endif
    }
  }
}
//	initializes the object
void oeMacApplication::init() {
  if (m_Flags & OEAPP_CONSOLE) {
  } else {
  }
}
//	Function to retrieve information from object through a platform defined structure.
void oeMacApplication::get_info(void *info) {
  tMacAppInfo *appinfo = (tMacAppInfo *)info;
  appinfo->hwnd = m_hWnd;
  appinfo->flags = m_Flags;

  Rect rect = (m_hWnd->portRect);
  //	Rect rect = (*m_hWnd->portPixMap)->bounds;
  appinfo->wnd_x = m_X = rect.left;
  appinfo->wnd_y = m_Y = rect.top;
  appinfo->wnd_w = m_W = rect.right - rect.left;
  appinfo->wnd_h = m_H = rect.bottom - rect.top;
  //	appinfo->wnd_x = m_X;
  //	appinfo->wnd_y = m_Y;
  //	appinfo->wnd_w = m_W;
  //	appinfo->wnd_h = m_H;
}
void oeMacApplication::set_sizepos(int x, int y, int w, int h) {
  if (!m_hWnd)
    return;
  m_X = x;
  m_Y = y;
  m_W = w;
  m_H = h;
  MoveWindow((GrafPort *)m_hWnd, x, y, TRUE);
  SizeWindow((GrafPort *)m_hWnd, w, h, false);
}
// real defer code.
#define DEFER_PROCESS_ACTIVE 1     // process is still active
#define DEFER_PROCESS_INPUT_IDLE 2 // process input from os not pending.
// #include "ddio.h"
int oeMacApplication::defer_block() {
  // IDLE PROCESSING
  if (m_DeferFunc)
    (*m_DeferFunc)(this->active());
  return (0);
}
//	defer returns some flags.   essentially this function defers program control to OS.
unsigned oeMacApplication::defer() {
  int result;
  con_Defer();
  // system mouse info.
  w32_msewhl_delta = 0;
  do {
    result = defer_block();
  } while ((result == DEFER_PROCESS_ACTIVE) || (result == DEFER_PROCESS_INPUT_IDLE));
  return 0;
}
//	set a function to run when deferring to OS.
void oeMacApplication::set_defer_handler(void (*func)(bool)) { m_DeferFunc = func; }
// initializes OS components.
void oeMacApplication::os_init() {}
//	These functions allow you to add message handlers.
bool oeMacApplication::add_handler(unsigned msg, tOEMacMsgCallback fn) {
  int i = 0;
  //	search for redundant callbacks.
  for (i = 0; i < MAX_MSG_FUNCTIONS; i++) {
    if (m_MsgFn[i].msg == msg && m_MsgFn[i].fn == fn)
      return true;
  }
  for (i = 0; i < MAX_MSG_FUNCTIONS; i++) {
    if (m_MsgFn[i].fn == NULL) {
      m_MsgFn[i].msg = msg;
      m_MsgFn[i].fn = fn;
      return true;
    }
  }
  Debugger(); // We have reached the max number of message functions!
  return false;
}
// These functions remove a handler
bool oeMacApplication::remove_handler(unsigned msg, tOEMacMsgCallback fn) {
  int i;
  if (!fn)
    Debugger();
  for (i = 0; i < MAX_MSG_FUNCTIONS; i++) {
    if (msg == m_MsgFn[i].msg && m_MsgFn[i].fn == fn) {
      m_MsgFn[i].fn = NULL;
      return true;
    }
  }
  return false;
}
// Run handler for message (added by add_handler)
bool oeMacApplication::run_handler(WindowPtr wnd, unsigned msg, unsigned wParam, long lParam) {
  int j;
  //	run user-defined message handlers
  // the guess here is that any callback that returns a 0, will not want to handle the window's WndProc function.
  for (j = 0; j < MAX_MSG_FUNCTIONS; j++)
    if (msg == m_MsgFn[j].msg && m_MsgFn[j].fn) {
      if (!(*m_MsgFn[j].fn)(wnd, msg, wParam, lParam))
        return false;
    }
  return true;
}
void oeMacApplication::clear_handlers() {
  int j;
  for (j = 0; j < MAX_MSG_FUNCTIONS; j++)
    m_MsgFn[j].fn = NULL;
}
void oeMacApplication::delay(float secs) {
  int result;
  // DAJ	long msecs = (long)(secs * 1000.0);
  long time_start;
  w32_msewhl_delta = 0;
  time_start = timer_GetTime();
  Sleep(0);
  while (timer_GetTime() < (time_start + secs))
  // DAJ	while (timer_GetTime() < (time_start+msecs))
  {
    this->defer_block();
  }
  // block if messages are still pending (for task switching too, this call will not return until messages are clear
  do {
    result = this->defer_block();
  } while (result == DEFER_PROCESS_ACTIVE || result == DEFER_PROCESS_INPUT_IDLE);
}
#ifdef FIXED
LRESULT MacAPI MyWndProc(HWND hWnd, UINT msg, UINT wParam, LPARAM lParam) {
  int i = -1;
  bool force_default = false;
  for (i = 0; i < MAX_MacAPPS; i++)
    if (Mac_AppObjects[i].hWnd == hWnd)
      break;
  if (i == MAX_MacAPPS)
    i = -1;
  switch (msg) {
    LPCREATESTRUCT lpCreateStruct;
  case WM_CREATE:
    // here we store the this pointer to the app object this instance belongs to.
    lpCreateStruct = (LPCREATESTRUCT)lParam;
    for (i = 0; i < MAX_MacAPPS; i++)
      if (Mac_AppObjects[i].hWnd == NULL)
        break;
    if (i == MAX_MacAPPS)
      debug_break();
    Mac_AppObjects[i].hWnd = hWnd;
    Mac_AppObjects[i].app = (oeMacApplication *)lpCreateStruct->lpCreateParams;
    Mac_AppObjects[i].app->clear_handlers();
    force_default = true;
    break;
  case WM_DESTROY:
    //	get window handle and clear it.
    if (i == MAX_MacAPPS)
      debug_break();
    Mac_AppObjects[i].hWnd = NULL;
    Mac_AppObjects[i].app = NULL;
    i = -1;
    break;
  case WM_SYSCOMMAND:
    // bypass screen saver and system menu.
    if ((wParam & 0xFFF0) == SC_SCREENSAVE || (wParam & 0xFFF0) == SC_MONITORPOWER)
      return 0;
    if ((wParam & 0xfff0) == SC_KEYMENU)
      return 0;
    break;
  case WM_SYSKEYDOWN:
  case WM_SYSKEYUP:
    if (lParam & 0x20000000)
      return 0;
    break;
  case WM_POWERBROADCAST: // Won't allow OS to suspend operation for now.
    mprintf((0, "WM_POWERBROADCAST=%u,%d\n", wParam, lParam));
    if (wParam == PBT_APMQUERYSUSPEND) {
      return BROADCAST_QUERY_DENY;
    }
    break;
  case WM_MOUSEWHEEL:
  case 0xcc41:
    if (w32_mouseman_hack) {
      if (msg != 0xcc41) {
        w32_msewhl_delta = HIWORD(wParam);
      } else {
        w32_msewhl_delta = (short)(wParam);
      }
    } else if (msg == WM_MOUSEWHEEL) {
      w32_msewhl_delta = HIWORD(wParam);
    }
    break;
  }

  oeMacApplication *Macapp = Mac_AppObjects[i].app;
  //	if this window not on list, then run default window proc.
  if (i == -1 || Macapp == NULL || force_default)
    return DefwindowProc(hWnd, msg, wParam, lParam);
  if (!Macapp->run_handler((HWnd)hWnd, (unsigned)msg, (unsigned)wParam, (long)lParam))
    return 0;

  // run user defined window procedure.
  return (LRESULT)Macapp->WndProc((HWnd)hWnd, (unsigned)msg, (unsigned)wParam, (long)lParam);
}
// detect if application can handle what we want of it.
bool oeMacApplication::GetSystemSpecs(const char *fname) {
  FILE *fp = fopen(fname, "wt");
  tMacOS os;
  int maj, min, build;
  char desc[256];
  if (!fp)
    return false;
  os = oeMacApplication::version(&maj, &min, &build, desc);
  fprintf(0, "OS: %s %d.%d.%d %s\n", "Mac", maj, min, build, desc);
  // get system memory info
  MEMORYSTATUS mem_stat;
  mem_stat.dwLength = sizeof(MEMORYSTATUS);
  GlobalMemoryStatus(&mem_stat);
  fprintf(fp, "Memory:\n");
  fprintf(fp, "\tLoad:\t\t\t%u\n\tTotalPhys:\t\t%u\n\tAvailPhys:\t\t%u\nPageFile:\t\t%u\n",
          (unsigned)mem_stat.dwMemoryLoad, (unsigned)mem_stat.dwTotalPhys, (unsigned)mem_stat.dwAvailPhys,
          (unsigned)mem_stat.dwTotalPageFile);
  fprintf(fp, "\tPageFileFree:\t%u\n\tVirtual:\t\t%u\n\tVirtualFree:\t%u\n", (unsigned)mem_stat.dwAvailPageFile,
          (unsigned)mem_stat.dwTotalVirtual, (unsigned)mem_stat.dwAvailVirtual);
  fclose(fp);
  return true;
}
#endif
