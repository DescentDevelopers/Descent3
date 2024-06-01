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

#ifndef RELEASE

#include <cstdarg>
#include <cstdio>

#include "Debug.h"
#include "networking.h"

#include <windows.h>
#include <winioctl.h>
#include <assert.h>
#include <conio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

extern bool Debug_NT;
extern bool Debug_print_block = false;

///////////////////////////////////////////////////////////////////////////////
//	NT MONO DRIVER SPECS

//
// Define the various device type values.  Note that values used by Microsoft
// Corporation are in the range 0-32767, and 32768-65535 are reserved for use
// by customers.
//

#define FILE_DEVICE_MONO 0x00008100

//
// Macro definition for defining IOCTL and FSCTL function control codes.  Note
// that function codes 0-2047 are reserved for Microsoft Corporation, and
// 2048-4095 are reserved for customers.
//

#define MONO_IOCTL_INDEX 0x810

//
// The MONO device driver IOCTLs
//

#define IOCTL_MONO_PRINT CTL_CODE(FILE_DEVICE_MONO, MONO_IOCTL_INDEX, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_MONO_CLEAR_SCREEN CTL_CODE(FILE_DEVICE_MONO, MONO_IOCTL_INDEX + 1, METHOD_BUFFERED, FILE_ANY_ACCESS)

//	---------------------------------------------------------------------------
//	console debugging functions

#define MAX_NUM_WINDOWS 4
#define MAX_NUM_VWINDOWS 8

struct mono_element {
  uint8_t character;
  uint8_t attribute;
};

struct WINDOW {
  int16_t first_row;
  int16_t height;
  int16_t first_col;
  int16_t width;
  int16_t cursor_row;
  int16_t cursor_col;
  int16_t open;
  struct mono_element save_buf[25][80];
  struct mono_element text[25][80];
};

static bool Mono_initialized = 0;
static mono_element (*Mono_screen)[25][80];
static char Dbgstr_format[512];
static WINDOW Window[MAX_NUM_WINDOWS];
static uint8_t Mono_virtual_window_list[MAX_NUM_VWINDOWS];

#ifndef RELEASE
#define MAX_MONO_LENGTH 2048
#else
#define MAX_MONO_LENGTH 256
#endif

static char Mono_buffer[MAX_MONO_LENGTH];

static HANDLE hNTMonoDriver = NULL;
static int Debug_logfile = -1;

#define ROW Window[n].first_row
#define HEIGHT Window[n].height
#define COL Window[n].first_col
#define WIDTH Window[n].width
#define CROW Window[n].cursor_row
#define CCOL Window[n].cursor_col
#define OPEN Window[n].open
#define CHAR(r, c) (*Mono_screen)[ROW + (r)][COL + (c)].character
#define ATTR(r, c) (*Mono_screen)[ROW + (r)][COL + (c)].attribute
#define XCHAR(r, c) Window[n].text[ROW + (r)][COL + (c)].character
#define XATTR(r, c) Window[n].text[ROW + (r)][COL + (c)].attribute

void con_scroll(int n);
void con_drawbox(int n);
void con_clear(int n);
void con_setcursor(int r, int c);
void con_mputc(int n, char ch);

#define MAX_TCPLOG_LEN 2000
SOCKET tcp_log_sock;
SOCKADDR_IN tcp_log_addr;
char tcp_log_buffer[MAX_TCPLOG_LEN];

bool nw_InitTCPLogging(char *ip, uint16_t port) {
  int addrlen = sizeof(SOCKADDR_IN);
  tcp_log_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (INVALID_SOCKET == tcp_log_sock) {
    return false;
  }

  memset(&tcp_log_addr, 0, sizeof(SOCKADDR_IN));
  tcp_log_addr.sin_family = AF_INET;
  tcp_log_addr.sin_addr.s_addr = INADDR_ANY;
  tcp_log_addr.sin_port = 0;

  if (SOCKET_ERROR == bind(tcp_log_sock, (SOCKADDR *)&tcp_log_addr, sizeof(sockaddr))) {
    return false;
  }
  unsigned long arg = 1;
  ioctlsocket(tcp_log_sock, FIONBIO, &arg);

  tcp_log_addr.sin_addr.s_addr = inet_addr(ip);
  tcp_log_addr.sin_port = htons(port);
  connect(tcp_log_sock, (SOCKADDR *)&tcp_log_addr, addrlen);
  return true;
}

void nw_TCPPrintf(int n, char *format, ...) {
  std::va_list args;

  if (tcp_log_sock == INVALID_SOCKET)
    return;

  //	filter out messages for other windows
  if (n != 0)
    return;

  va_start(args, format);
  std::vsnprintf(tcp_log_buffer, MAX_TCPLOG_LEN, format, args);

  fd_set read_fds;
  timeval tv = {0, 0};

  FD_ZERO(&read_fds);
  FD_SET(tcp_log_sock, &read_fds);

  if (select(0, NULL, &read_fds, 0, &tv)) {
    send(tcp_log_sock, tcp_log_buffer, strlen(tcp_log_buffer), 0);
  } else {
    // Int3();
  }
}

void Debug_ConsoleExit() {
  if (hNTMonoDriver)
    CloseHandle(hNTMonoDriver);
}

bool Debug_ConsoleInit() {
  int n = 0;

  if (Mono_initialized)
    return 1;

  // Only use monochrome if D3_MONO environment var is set
  atexit(Debug_ConsoleExit);

  if (Debug_NT == true) {
    hNTMonoDriver =
        CreateFile("\\\\.\\MONO", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hNTMonoDriver != (HANDLE)(-1))
      Mono_initialized = 1;
    else
      Mono_initialized = 0;
  }
#ifdef OLD
  else {
    _outp(0x3b4, 0x0f);
    _outp(0x3b4 + 1, 0x55);

    if (_inp(0x3b4 + 1) != 0x55) {
      if (getenv("D3_MONO"))
        Mono_initialized = 1;
      else
        Mono_initialized = 0;
    } else {
      _outp(0x3b4 + 1, 0);
      Mono_initialized = 1;
    }
    Mono_screen = (mono_element(*)[25][80])0xB0000;
  }
#endif

  if (Mono_initialized)
    OPEN = 1;
  else
    OPEN = 0;

  ROW = 2;
  COL = 0;
  WIDTH = 80;
  HEIGHT = 24;
  CCOL = 0;
  CROW = 0;

  con_clear(0);

  for (n = 1; n < MAX_NUM_WINDOWS; n++) {
    OPEN = 0;
    ROW = 2;
    COL = 1;
    WIDTH = 78;
    HEIGHT = 23;
    CROW = 0;
    CCOL = 0;
  }

  // map 0 always to itself.
  Mono_virtual_window_list[0] = 0;

  for (n = 1; n < MAX_NUM_VWINDOWS; n++) {
    Mono_virtual_window_list[n] = -1;
  }

  return 1;
}

void Debug_ConsoleRedirectMessages(int virtual_window, int physical_window) {
  int i, n;

  if (virtual_window < 1 || virtual_window >= MAX_NUM_VWINDOWS) {
    return;
  }
  if (physical_window < 1 || physical_window >= MAX_NUM_WINDOWS) {
    return;
  }

  n = physical_window;
  if (!OPEN)
    return;

  // clear out any bindings and map virtual window to physical window.
  for (i = 0; i < MAX_NUM_VWINDOWS; i++) {
    if (Mono_virtual_window_list[i] == physical_window) {
      Mono_virtual_window_list[i] = -1;
    }
  }

  Mono_virtual_window_list[virtual_window] = physical_window;
  con_clear(physical_window);
}

void Debug_LogClose();

bool Debug_Logfile(const char *filename) {
  if (Debug_logfile == -1) {
    Debug_logfile = _open(filename, _O_CREAT | _O_WRONLY | _O_TEXT, _S_IREAD | _S_IWRITE);
    if (Debug_logfile == -1) {
      Debug_MessageBox(OSMBOX_OK, "Debug", "FYI Logfile couldn't be created.");
      return false;
    }
    atexit(Debug_LogClose);
  }
  Debug_LogWrite("BEGINNING LOG\n\n");

  return true;
}

void Debug_LogWrite(const char *str) {
  if (Debug_logfile > -1)
    _write(Debug_logfile, str, strlen(str));
}

void Debug_LogClose() {
  if (Debug_logfile > -1) {
    Debug_LogWrite("\nEND LOG");
    _close(Debug_logfile);
    Debug_logfile = -1;
  }
}

void Debug_ConsoleOpen(int n, int row, int col, int width, int height, const char *title) {
  if (!Mono_initialized)
    return; // error! no mono card
  if (n >= MAX_NUM_WINDOWS)
    return;

  if (OPEN)
    Debug_ConsoleClose(n);

  OPEN = 1;
  ROW = row;
  COL = col;
  WIDTH = width;
  HEIGHT = height;

  Mono_virtual_window_list[n] = n;

  if (Debug_NT) {

  } else {
    for (row = -1; row < HEIGHT + 1; row++)
      for (col = -1; col < WIDTH + 1; col++) {
        CHAR(row, col) = 32;
        ATTR(row, col) = 7;
        XCHAR(row, col) = 32;
        XATTR(row, col) = 7;
      }

    con_drawbox(n);
    CROW = -1;
    CCOL = 0;
    Debug_ConsolePrintf(n, title);
    CROW = 0;
    CCOL = 0;
    con_setcursor(ROW + CROW, COL + CCOL);
  }
}

void Debug_ConsoleClose(int n) {
  int row, col;

  if (!OPEN)
    return;
  if (n >= MAX_NUM_WINDOWS)
    return;

  if (Debug_NT) {

  } else {
    for (row = -1; row < HEIGHT + 1; row++)
      for (col = -1; col < WIDTH + 1; col++) {
        CHAR(row, col) = 32;
        ATTR(row, col) = 7;
      }
  }

  OPEN = 0;
  CCOL = 0;
  CROW = 0;

  con_setcursor(0, 0);
  Mono_virtual_window_list[n] = -1;
}

void Debug_ConsolePrintf(int n, const char *format, ...) {
  static bool newline = false;
  char *ptr = Mono_buffer;
  std::va_list args;

  if (n >= MAX_NUM_VWINDOWS)
    return;
  if (n > 0) {
    n = Mono_virtual_window_list[n];
  }

  //	filter out messages
  if (n > MAX_NUM_WINDOWS || n < 0)
    return;

  // if (!OPEN) return;

  va_start(args, format);
  std::vsnprintf(ptr, MAX_MONO_LENGTH, format, args);

  if (strlen(ptr) >= MAX_MONO_LENGTH) {
    return;
  }

  if (OPEN) {
    if (Debug_NT) {
      DWORD cbReturned;
      DeviceIoControl(hNTMonoDriver, (DWORD)IOCTL_MONO_PRINT, ptr, strlen(ptr) + 1, NULL, 0, &cbReturned, 0);
    } else {
      while (*ptr)
        con_mputc(n, *ptr++);
    }
  }

  Debug_LogWrite(Mono_buffer);

  if (n == 0) {
    OutputDebugString(Mono_buffer);
    nw_TCPPrintf(n, Mono_buffer);
  }
}

void Debug_ConsolePrintfAt(int n, int row, int col, const char *format, ...) {
  char *ptr = Mono_buffer;
  int r, c;
  std::va_list args;

  if (Debug_NT)
    return;

  if (n >= MAX_NUM_VWINDOWS)
    return;
  if (n > 0) {
    n = Mono_virtual_window_list[n];
  }

  if (n > MAX_NUM_WINDOWS || n < 0)
    return;

  va_start(args, format);
  std::vsnprintf(ptr, MAX_MONO_LENGTH, format, args);

  //	if (n==MAX_NUM_WINDOWS)
  //		OutputDebugString (Mono_buffer);

  if (!OPEN)
    return;

  r = CROW;
  c = CCOL;
  CROW = row;
  CCOL = col;

  while (*ptr)
    con_mputc(n, *ptr++);

  CROW = r;
  CCOL = c;

  if (n == 0)
    OutputDebugString(Mono_buffer);
}

void con_mputc(int n, char c) {
  if (!OPEN)
    return;

  if (Debug_NT) {

    return;
  }

  switch (c) {
  case 8:
    if (CCOL > 0)
      CCOL--;
    break;
  case 9:
    CHAR(CROW, CCOL) = ' ';
    ATTR(CROW, CCOL) = XATTR(CROW, CCOL);
    XCHAR(CROW, CCOL) = ' ';
    CCOL++;
    while (CCOL % 4) {
      CHAR(CROW, CCOL) = ' ';
      ATTR(CROW, CCOL) = XATTR(CROW, CCOL);
      XCHAR(CROW, CCOL) = ' ';
      CCOL++;
    }
    break;
  case 10:
  case 13:
    CCOL = 0;
    CROW++;
    break;
  default:
    CHAR(CROW, CCOL) = c;
    ATTR(CROW, CCOL) = XATTR(CROW, CCOL);
    XCHAR(CROW, CCOL) = c;
    CCOL++;
  }

  if (CCOL >= WIDTH) {
    CCOL = 0;
    CROW++;
  }
  if (CROW >= HEIGHT) {
    CROW--;
    con_scroll(n);
  }

  con_setcursor(ROW + CROW, COL + CCOL);
}

void copy_row(int nwords, int16_t *src, int16_t *dest1, int16_t *dest2) {
// TODO: disabled for now to get the x64 build compiling
// do we even need all this old 'mono' stuff anymore?
#ifndef _WIN64
  __asm {
		mov ecx,nwords
		mov esi,src
		mov ebx,dest1
		mov edx,dest2
 		shr		ecx, 1
		jnc		even_num		
		mov		ax, [esi]		
		add		esi, 2			
		mov		[ebx], ax		
		add		ebx, 2			
		mov		[edx], ax		
		add		edx, 2			
		even_num:	cmp		ecx, 0				
		je			done
		rowloop:	mov		eax, [esi]			
		add		esi, 4				
		mov		[edx], eax			
		add		edx, 4				
		mov		[ebx], eax			
		add		ebx, 4				
		loop		rowloop				
		done:
  }
#endif // _WIN64
}

void con_scroll(int n) {
  register int row, col;

  if (!OPEN)
    return;
  if (Debug_NT)
    return;

  col = 0;
  for (row = 0; row < (HEIGHT - 1); row++)
    copy_row(WIDTH, (int16_t *)&XCHAR(row + 1, col), (int16_t *)&CHAR(row, col), (int16_t *)&XCHAR(row, col));

  //		for ( col = 0; col < WIDTH; col++ )
  //		{
  //			CHAR( row, col ) = XCHAR( row+1, col );
  //			ATTR( row, col ) = XATTR( row+1, col );
  //			XCHAR( row, col ) = XCHAR( row+1, col );
  //			XATTR( row, col ) = XATTR( row+1, col );
  //		}

  for (col = 0; col < WIDTH; col++) {
    CHAR(HEIGHT - 1, col) = ' ';
    ATTR(HEIGHT - 1, col) = XATTR(HEIGHT - 1, col);
    XCHAR(HEIGHT - 1, col) = ' ';
  }
}

void con_setcursor(int row, int col) {
  int pos = row * 80 + col;

  if (Debug_NT) {
    return;
  }

#ifdef OLD
  _outp(0x3b4, 15);
  _outp(0x3b5, pos & 0xFF);
  _outp(0x3b4, 14);
  _outp(0x3b5, (pos >> 8) & 0xff);
#endif
}

void con_drawbox(int n) {
  int16_t row, col;

  if (!OPEN)
    return;

  if (Debug_NT) {

    return;
  }

  for (row = 0; row < HEIGHT; row++) {
    CHAR(row, -1) = 179;
    CHAR(row, WIDTH) = 179;
    XCHAR(row, -1) = 179;
    XCHAR(row, WIDTH) = 179;
  }

  for (col = 0; col < WIDTH; col++) {
    CHAR(-1, col) = 196;
    CHAR(HEIGHT, col) = 196;
    XCHAR(-1, col) = 196;
    XCHAR(HEIGHT, col) = 196;
  }

  CHAR(-1, -1) = 218;
  CHAR(-1, WIDTH) = 191;
  CHAR(HEIGHT, -1) = 192;
  CHAR(HEIGHT, WIDTH) = 217;
  XCHAR(-1, -1) = 218;
  XCHAR(-1, WIDTH) = 191;
  XCHAR(HEIGHT, -1) = 192;
  XCHAR(HEIGHT, WIDTH) = 217;
}

void con_clear(int n) {
  int16_t row, col;

  if (!OPEN)
    return;

  if (Debug_NT) {
    DWORD cbReturned;

    DeviceIoControl(hNTMonoDriver, (DWORD)IOCTL_MONO_CLEAR_SCREEN, NULL, 0, NULL, 0, &cbReturned, 0);
    return;
  }

  for (row = 0; row < HEIGHT; row++)
    for (col = 0; col < WIDTH; col++) {
      CHAR(row, col) = 32;
      ATTR(row, col) = 7;
      XCHAR(row, col) = 32;
      XATTR(row, col) = 7;
    }
  CCOL = 0;
  CROW = 0;
}

#endif
