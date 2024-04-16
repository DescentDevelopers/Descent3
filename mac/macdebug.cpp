/*
 * $Logfile: /DescentIII/Main/mac/macdebug.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:58:15 $
 * $Author: kevinb $
 *
 * MacOS Debugging routines
 *
 * $Log: macdebug.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:58:15  kevinb
 * initial 1.5 import
 *
 *
 * 4     4/12/00 7:08p Matt
 * From Duane for 1.4
 *
 * 3     10/21/99 1:55p Kevin
 * Mac Merge!
 *
 * 2     7/28/99 2:51p Kevin
 *
 * 4     5/15/97 1:51 AM Jeremy
 * made osMessage/Error box correctly handle newlines and added some error
 * checking for strings which are too long to fit into pascal strings
 *
 * 3     5/9/97 8:04 PM Jeremy
 * changed include of <Strings.h> to <TextUtils.h>
 *
 * 2     3/21/97 6:53 PM Jeremy
 * First crack at implementing debugging and console io routines.
 * Currently the functions display a mac dialog box for errors and
 * messages and send all console messages through the macintosh's modem
 * port.
 *
 * 1     2/28/97 12:16 PM Jeremy
 * MacOS specific Debug  object libraries
 *
 * $NoKeywords: $
 */

#include "debug.h"
#include "mono.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <sioux.h>

extern int debug_level;

void setup_sioux(void) {
  SIOUXSettings.standalone = true;
  SIOUXSettings.setupmenus = true;
  SIOUXSettings.asktosaveonclose = false;
  SIOUXSettings.toppixel = 526;
  SIOUXSettings.leftpixel = 10;
  SIOUXSettings.rows = 20;
  SIOUXSettings.columns = 120;
}

///////////////////////////////////////////////////////////////////////////////

bool Debug_break = false;
bool Debug_mono = false;

char *Debug_DumpInfo();
#include "stringtable.h"
#include "newui_core.h"
#include "newui.h"
#include "descent.h"
#include "renderer.h"

//	if we are running under a debugger, then pass true
bool Debug_Init(bool debugger, bool mono_debug) {
#ifdef DAJ_DEBUG
  //#ifndef RELEASE
  Debug_break = debugger;

  if (mono_debug) {

    mprintf((0, "Linux system.\n"));
  }

  if (Debug_break)
    mprintf((0, "Debug Break enabled.\n"));

#endif // ifndef RELEASE

  return true;
}

// Does a messagebox with a stack dump
// Messagebox shows topstring, then stack dump, then bottomstring
// Return types are the same as the Windows return values
int Debug_ErrorBox(int type, const char *title, const char *topstring, const char *bottomstring) {
  int answer = 0;
  //	char *dumptext = Debug_DumpInfo();

  Str255 debug_str;
  sprintf((char *)debug_str, "%s:%s\r\n%s", title, topstring, bottomstring);
#ifdef DAJ_DEBUG
  DebugStr(debug_str);
#else
  //	if (GetFunctionMode() > INIT_MODE) {
  rend_Close();
  ::ShowCursor();
  ::ParamText(c2pstr(topstring), c2pstr(bottomstring), NULL, NULL);
  ::Alert(911, NULL);
  //	} else {
  //		DoMessageBox(TXT_ERROR, (char *)debug_str, MSGBOX_OK, UICOL_WINDOW_TITLE,UICOL_TEXT_NORMAL);
  //	}
  exit(0);
//	fprintf(stderr,"%s:%s\r\n\r\n%s\r\n\r\n%s",title,topstring,dumptext,bottomstring);
#endif
  //	debug_break();

  return answer;
}

// displays an message box
// Returns the same values as the Win32 MessageBox() function
int Debug_MessageBox(int type, const char *title, const char *str) {
  return Debug_ErrorBox(type, str, "Descent 3 Message", "");
}

///////////////////////////////////////////////////////////////////////////////

char *Debug_DumpInfo(void) {
  static char e[] = "System Error";
  return e;
}
