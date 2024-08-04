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

--- HISTORICAL COMMENTS FOLLOW ---

 * $Logfile: /DescentIII/Main/win32/windebug.cpp $
 * $Revision: 33 $
 * $Date: 9/15/01 1:46p $
 * $Author: Kevin $
 *
 * Win32 Debugging 'kernal'
 *
 * $Log: /DescentIII/Main/win32/windebug.cpp $
 *
 * 33    9/15/01 1:46p Kevin
 * Added -nocrashbox to suppress crash dialog box
 *
 * 32    5/19/99 2:43a Jeff
 * handle some possible crashes.  The UCT->Localtime doesn't seem to work,
 * although it is setup correctly.
 *
 * 31    5/18/99 9:48p Kevin
 * adjust time to time zone for Jeff
 *
 * 30    5/07/99 5:46p Jason
 * changes for gamegauge release
 *
 * 29    4/18/99 1:18p Kevin
 * Demo2 build changes
 *
 * 28    4/12/99 7:14p Samir
 * added multiple pages per mono window.
 *
 * 27    4/01/99 11:07a Kevin
 * Added some more usefull debugging info
 *
 * 26    4/01/99 9:40a Matt
 * Fixed dump_text_to_clipboard.  It used to allocate a fixed number of
 * bytes to allow space for adding CRs to strings, but now I actually
 * count the number needed and allocate that much.
 *
 * 25    3/29/99 6:50p Kevin
 * Added some parameters to the log file
 *
 * 24    2/16/99 11:25a Kevin
 * Took out testing code.
 *
 * 23    2/16/99 12:36a Kevin
 * Fixes for release builds of OEM V3 and KAtmai
 *
 * 22    2/15/99 1:22p Kevin
 * Changes for GameGauge
 *
 * 21    2/13/99 1:56p Kevin
 *
 * 20    1/05/99 4:28p Kevin
 * Moved exception handling code to windebug.cpp
 *
 * 19    1/05/99 3:55p Kevin
 * Added structured exception handling
 *
 * 18    10/18/98 8:52p Matt
 * Revamped debug/error system.
 *
 * 17    10/12/98 10:20a Samir
 * added parameter to debug init.
 *
 * 16    10/08/98 4:24p Kevin
 * Changed code to comply with memory library usage. Always use mem_malloc
 * , mem_free and mem_strdup
 *
 * 15    6/23/98 2:40p Matt
 * Added Yes/No/Cancel type to OutrageMessageBox() and
 * Debug_MesssageBox(), and changed return value from a bool to an
 * integer.
 *
 * 14    4/21/98 2:30p Matt
 * dump_text_to_clipboard() wasn't putting terminating null on text string
 *
 * 13    4/08/98 7:19p Samir
 * Added runtime debugging option.
 *
 * 12    4/07/98 9:21p Samir
 * Added volition's call stack and memory dump code.  This will only
 * happen if you run the game without the -debug argument.
 *
 * 11    4/03/98 5:15p Samir
 * Implemented simple debug message filtering.
 *
 * 10    3/20/98 2:43p Samir
 * Some better Int3 support.
 *
 * 9     1/29/98 12:24p Samir
 * Added logfile support.
 *
 * 8     1/28/98 4:45p Samir
 * Mono check stuff.
 *
 * 7     1/28/98 11:56a Samir
 * Code to check for monochrome debug.
 *
 * 6     1/28/98 11:17a Samir
 * Added debugbreak stuff.
 *
 * 5     10/15/97 3:05p Samir
 * NT mono output works.
 *
 * 4     10/13/97 2:41p Samir
 * Debug breaks now are macros to work depending on the operating system.
 *
 * 3     9/04/97 12:00p Matt
 * Changed Debug_MessageBox() to return bool instead of int, since it was
 * already being used that way.
 *
 * 2     7/28/97 3:46p Samir
 * NT mono stubbed out.
 *
 * 18    6/11/97 1:10p Samir
 * Fixed up function names.
 *
 * 17    3/31/97 3:50p Samir
 * MessageBox should be task modal.
 *
 * 16    3/18/97 2:14p Samir
 * DebugBreak called instead of Int3
 *
 * 15    3/17/97 8:10p Matt
 * Don't print to monochrome screen unless D3_MONO evironment var is set
 *
 * 14    3/14/97 4:15p Matt
 * Changed title of OutrageMessageBox() dialogs
 *
 * 13    3/10/97 12:29p Samir
 * Moved mono code and altered lowlevel console interface to machine
 * library.
 *
 * 12    3/10/97 11:13a Samir
 * Changes made to reflect deletion of OSDebug class from system
 *
 * 11    3/04/97 7:15p Samir
 * Fixed bug in message_box, caused an Int3.
 *
 * 10    3/04/97 1:00p Samir
 * Fixed return type prototype errors.
 *
 * 9     3/04/97 12:55p Samir
 * Added types of messageboxes.
 *
 * 8     3/03/97 5:07p Samir
 * stupid
 *
 * 7     3/03/97 4:36p Samir
 * Removed instances of pserror.h code.  this is a lower level lib than
 * that!
 *
 * 6     2/27/97 7:00p Samir
 * Removed windows.h from oewin_os.h and moved to library.
 *
 * 5     2/26/97 7:04p Samir
 * Added Win32 DebugBreak
 *
 * 4     2/18/97 7:00p Samir
 * Now print out last Win32 error too.
 *
 * 3     1/31/97 3:31p Samir
 * forgot to use OSWINDBG_MAX_STRINGS constant in array init.
 *
 * 2     1/30/97 6:49p Samir
 * Added some winDebug code for interface with error handler
 *
 * 1     1/28/97 2:32p Samir
 * Standard debugging functions
 *
 * $NoKeywords: $
 */

#include <windows.h>
#include <DbgHelp.h>
#include <cstring>
#include <filesystem>

#include "debug.h"
#include "mem.h"
#include "pserror.h"

///////////////////////////////////////////////////////////////////////////////

bool Debug_break = false;

///////////////////////////////////////////////////////////////////////////////

bool Debug_Init(bool debugger, bool mono_debug) {
  // initialization of debugging consoles.

#ifndef RELEASE

  Debug_break = debugger;

  if (Debug_break) {
    mprintf(0, "Debug Break enabled.\n");
  }

#endif // ifndef RELEASE

  return true;
}

/*	Message Box functions  */

// Does a messagebox with a stack dump
// Messagebox shows topstring, then stack dump, then bottomstring
// Return types are as the same the Windows return values
int Debug_ErrorBox(int type, const char *title, const char *topstring, const char *bottomstring) {
  DWORD flags = 0;
  if (type == OSMBOX_OK)
    flags = MB_OK;
  else if (type == OSMBOX_YESNO)
    flags = MB_YESNO;
  else if (type == OSMBOX_YESNOCANCEL)
    flags = MB_YESNOCANCEL;
  else if (type == OSMBOX_ABORTRETRYIGNORE)
    flags = MB_ABORTRETRYIGNORE;
  else if (type == OSMBOX_OKCANCEL)
    flags = MB_OKCANCEL;
  else
    debug_break();

  char *tmpbuf = (char *)mem_malloc(strlen(topstring) + strlen(bottomstring) + 5);
  wsprintf(tmpbuf, "%s\r\n\r\n%s", topstring, bottomstring);

  ShowCursor(TRUE);
  int answer = MessageBox(nullptr, tmpbuf, title, flags | MB_TASKMODAL | MB_SETFOREGROUND);
  ShowCursor(FALSE);

  mem_free(tmpbuf);

  return answer;
}

// Return types are the same the Windows return values
int Debug_MessageBox(int type, const char *title, const char *str) {
  DWORD flags = 0;
  if (type == OSMBOX_OK)
    flags = MB_OK;
  else if (type == OSMBOX_YESNO)
    flags = MB_YESNO;
  else if (type == OSMBOX_YESNOCANCEL)
    flags = MB_YESNOCANCEL;
  else if (type == OSMBOX_ABORTRETRYIGNORE)
    flags = MB_ABORTRETRYIGNORE;
  else
    DebugBreak();

  ShowCursor(TRUE);
  int answer = MessageBox(nullptr, str, title, flags | MB_TASKMODAL | MB_SETFOREGROUND);
  ShowCursor(FALSE);

  return answer;
}

///////////////////////////////////////////////////////////////////////////////

void dump_text_to_clipboard(char *text);

///////////////////////////////////////////////////////////////////////////////

void DumpTextToClipboard(char *text) {
  char *ptr;

  // Count number of carriage returns to be added
  char last_char = 0;
  int extra = 0;
  ptr = text;
  while (*ptr) {
    if ((*ptr == '\n') && (last_char != '\r')) {
      extra++;
    }
    last_char = *ptr++;
  }

  // Length of string with CRs added
  size_t len = strlen(text) + extra + 1;

  HGLOBAL h_text = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, len);
  if (!h_text)
    return;
  ptr = (char *)GlobalLock(h_text);
  if (!ptr)
    return;

  // copy then, if you find any \n's without \r's, then add in the \r.
  last_char = 0;
  while (*text) {
    if ((*text == '\n') && (last_char != '\r')) {
      *ptr++ = '\r';
    }
    last_char = *text;
    *ptr++ = last_char;
    text++;
  }
  *ptr = 0; // terminate string
  GlobalUnlock(h_text);
  OpenClipboard(NULL);
  EmptyClipboard();
  SetClipboardData(CF_TEXT, h_text);
  CloseClipboard();
}

void dump_text_to_clipboard(char *text) { DumpTextToClipboard(text); }

///////////////////////////////////////////////////////////////////////////////

#pragma comment(lib, "DbgHelp.lib")

long __cdecl RecordExceptionInfo(PEXCEPTION_POINTERS data) {
  static bool BeenHere = false;
  if (BeenHere) // Going recursive! That must mean this routine crashed!
    return EXCEPTION_CONTINUE_SEARCH;
  BeenHere = true;

  SetMessageBoxTitle("Unexpected error");

  std::error_code ec;
  const std::filesystem::path dumFilePath = std::filesystem::temp_directory_path(ec) / "d3-crash.dmp";
  if (ec) {
    OutrageMessageBox("Could not find temporary directory to write crash dump file into: %s", ec.message().c_str());
    exit(1);
  }

  HANDLE hDumpFile = CreateFile(dumFilePath.u8string().c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, 0, 0);
  if (hDumpFile == INVALID_HANDLE_VALUE) {
    OutrageMessageBox("Could not create crash dump file, error code: %x", GetLastError());
    exit(1);
  }

  MINIDUMP_EXCEPTION_INFORMATION exceptInfo;
  exceptInfo.ThreadId = GetCurrentThreadId();
  exceptInfo.ExceptionPointers = data;
  exceptInfo.ClientPointers = TRUE;

  if (MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &exceptInfo, nullptr, nullptr)) {
    OutrageMessageBox("Crash dump file written to: %s\nYou can attach it to a bug report.", dumFilePath.u8string().c_str());
  } else {
    OutrageMessageBox("Could not write crash dump file, error code: %x", GetLastError());
    exit(1);
  }

  BeenHere = false;

  return Debug_break ? EXCEPTION_CONTINUE_SEARCH : EXCEPTION_EXECUTE_HANDLER;
}
