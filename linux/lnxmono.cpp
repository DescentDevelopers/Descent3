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

/*
 * $Logfile: /DescentIII/Main/linux/lnxmono.cpp $
 * $Revision: 1.3 $
 * $Date: 2004/02/23 03:03:48 $
 * $Author: ryan $
 *
 * Linux monochrome routines
 *
 * $Log: lnxmono.cpp,v $
 * Revision 1.3  2004/02/23 03:03:48  ryan
 * Patched to compile with gcc3 and a modern linux distro...
 *
 * Revision 1.2  2000/05/29 05:39:59  icculus
 * Changed some exit() calls to _exit().
 *
 * Revision 1.1.1.1  2000/04/18 00:00:39  icculus
 * initial checkin
 *
 *
 * 13    8/22/99 5:52a Jeff
 * remote monochrome server code added
 *
 * 12    7/14/99 9:09p Jeff
 * added comment header
 *
 * $NoKeywords: $
 */

#include "debug.h"
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <memory.h>
#include <signal.h>

// -------- Start TCP/IP Mono Logging Section
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>

#define MAX_TCPLOG_LEN 2000
#define SOCKET int
#define SOCKADDR_IN sockaddr_in
#define SOCKADDR sockaddr
#define INVALID_SOCKET -1
static SOCKET tcp_log_sock;
static struct sockaddr_in tcp_log_addr;
static char tcp_log_buffer[MAX_TCPLOG_LEN];
static int Console_fd = -1;
static bool Mono_initialized = 0;
static bool Mono_use_real = false;
static bool Mono_use_window_remote = false;
static int Debug_logfile = 0;
bool Debug_print_block = false;

// ===============================
// pthread library functions
// ===============================
#include <pthread.h>
#include <dlfcn.h>

#include <algorithm>

#if DLOPEN_PTHREAD
typedef int (*pthread_create_fp)(pthread_t *__thread, __const pthread_attr_t *__attr, void *(*__start_routine)(void *),
                                 void *__arg);
typedef int (*pthread_detach_fp)(pthread_t __th);
typedef pthread_t (*pthread_self_fp)(void);

static pthread_create_fp dpthread_create = NULL;
static pthread_detach_fp dpthread_detach = NULL;
static pthread_self_fp dpthread_self = NULL;
#else
#define dpthread_create pthread_create
#define dpthread_detach pthread_detach
#define dpthread_self pthread_self
#endif

static unsigned long long Timer_sys_start_time = 0;
static unsigned long long Timer_accum = 0, Timer_high_mark = 0;

static float nw_TCPLoggingTimer(void) {
  unsigned long time_ms;
  unsigned long long ret;

  struct timeval t;
  gettimeofday(&t, NULL);

  ret = (unsigned long long)t.tv_sec * 1000000.0 + t.tv_usec;

  if (ret > Timer_high_mark) {
    Timer_high_mark = ret;
  } else {
    // timer roll over
    if (Timer_high_mark > 0)
      Timer_accum += (Timer_high_mark - Timer_sys_start_time);
    Timer_high_mark = ret;
    Timer_sys_start_time = ret;
    ret = 100; // give some time
  }

  time_ms = Timer_accum + ret - Timer_sys_start_time;

  return (float)((double)time_ms / ((double)1000000.0));
}

static bool nw_TCPLoggingTimer_Init(void) {
  struct timeval t;
  gettimeofday(&t, NULL);

  Timer_sys_start_time = (unsigned long long)t.tv_sec * 1000000.0 + t.tv_usec;
  Timer_accum = 0;
  Timer_high_mark = 0;
  return true;
}

static unsigned char outgoing_mono_packet[512];
static volatile int outgoing_packet_size = 0;
static volatile float outgoing_last_time_sent = 0;
static volatile bool outgoing_packet_flush = false;
static volatile bool outgoing_building_packet = false;

void *nw_TCPLoggingWorker(void *arg) {
  dpthread_detach(dpthread_self());

  while (1) {
    if (tcp_log_sock == INVALID_SOCKET)
      break;

    fd_set wfds;
    timeval timeout = {0, 0};

    FD_ZERO(&wfds);
    FD_SET(tcp_log_sock, &wfds);

    int sock_writable = select(tcp_log_sock + 1, NULL, &wfds, NULL, &timeout);

    if (sock_writable == -1) {
      fprintf(stdout, "Error blocking on Monochrome server port\n");
      _exit(1);
    }

    if (!sock_writable) {
      continue;
    }

    if (outgoing_packet_size == 0)
      continue;
    if (outgoing_building_packet)
      continue;

    int save_size = outgoing_packet_size;
    outgoing_packet_size = -1; // this informs the other thread that we are sending

    float curr_time = nw_TCPLoggingTimer();
    if (curr_time > (outgoing_last_time_sent + 0.05f)) {
      // time to send the packet
      outgoing_last_time_sent = curr_time;

      send(tcp_log_sock, outgoing_mono_packet, save_size, 0);

      save_size = 0;
    }

    outgoing_packet_size = save_size;
  }

  return NULL;
}

bool nw_InitTCPLogging(char *ip, unsigned short port) {
#if DLOPEN_PTHREAD
  // load up the pthread library
  void *lib = dlopen("libpthread.so", RTLD_GLOBAL | RTLD_NOW);
  if (!lib) {
    fprintf(stdout, "Error: Unable to load libpthread.so\n");
    _exit(1);
  }
  dpthread_create = (pthread_create_fp)dlsym(lib, "pthread_create");
  dpthread_detach = (pthread_detach_fp)dlsym(lib, "pthread_detach");
  dpthread_self = (pthread_self_fp)dlsym(lib, "pthread_self");
#endif

  unsigned long argp = 1;
  int addrlen = sizeof(SOCKADDR_IN);
  tcp_log_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (INVALID_SOCKET == tcp_log_sock) {
    return false;
  }

  memset(&tcp_log_addr, 0, sizeof(SOCKADDR_IN));
  tcp_log_addr.sin_family = AF_INET;
  tcp_log_addr.sin_addr.s_addr = INADDR_ANY;
  tcp_log_addr.sin_port = 0;

  if (bind(tcp_log_sock, (SOCKADDR *)&tcp_log_addr, sizeof(sockaddr))) {
    return false;
  }

  tcp_log_addr.sin_addr.s_addr = inet_addr(ip);
  tcp_log_addr.sin_port = htons(port);

  fd_set write_fs;
  FD_ZERO(&write_fs);
  FD_SET(tcp_log_sock, &write_fs);
  select(tcp_log_sock + 1, NULL, &write_fs, NULL, NULL);

  fprintf(stdout, "Connecting to monochrome server (%s:%d)...\n", ip, port);

  if (connect(tcp_log_sock, (SOCKADDR *)&tcp_log_addr, addrlen)) {
    int wserr;
    wserr = errno;
    fprintf(stdout, "Mono: Connect Failed...");

    switch (wserr) {
    case EBADF:
      fprintf(stdout, "EBADF\n");
      break;
    case EFAULT:
      fprintf(stdout, "EFAULT\n");
      break;
    case ENOTSOCK:
      fprintf(stdout, "ENOTSOCK\n");
      break;
    case EISCONN:
      fprintf(stdout, "EISCONN\n");
      break;
    case ECONNREFUSED:
      fprintf(stdout, "EREFUSED\n");
      break;
    case ETIMEDOUT:
      fprintf(stdout, "ETIMEDOUT\n");
      break;
    case ENETUNREACH:
      fprintf(stdout, "ENETUNREACH\n");
      break;
    default:
      fprintf(stdout, "Unknown error %d\n", wserr);
      break;
    }

    return false;
  }

  fprintf(stdout, "Monochrome server connected\n");

  // init the packet sent time
  pthread_t thread;
  nw_TCPLoggingTimer_Init();
  outgoing_last_time_sent = nw_TCPLoggingTimer();

  dpthread_create(&thread, NULL, nw_TCPLoggingWorker, NULL);

  return true;
}

void nw_TCPPrintf(int n, char *format, ...) { return; }

void nw_SendMonoPacket(unsigned char *data, int size) {
  if (tcp_log_sock == INVALID_SOCKET) {
    return;
  }

  if (size > 512) {
    fprintf(stdout, "Mono: Packet > 512 bytes\n");
    _exit(1);
  }

  outgoing_building_packet = true;
  while (outgoing_packet_size == -1)
    ;

  if (outgoing_packet_size + size > 512) {
    // crap...we need to flush
    fd_set wfds;
    FD_ZERO(&wfds);
    FD_SET(tcp_log_sock, &wfds);

    int sock_writable = select(tcp_log_sock + 1, NULL, &wfds, NULL, NULL);

    if (sock_writable == -1) {
      fprintf(stdout, "Error blocking on Monochrome server port\n");
      _exit(1);
    }

    send(tcp_log_sock, outgoing_mono_packet, outgoing_packet_size, 0);
    outgoing_packet_size = 0;
  }

  memcpy(&outgoing_mono_packet[outgoing_packet_size], data, size);
  outgoing_packet_size += size;

  outgoing_building_packet = false;
}

// ---------------- End TCP/IP Mono Logging Section

void Debug_LogClose();

bool Debug_Logfile(const char *filename) {
  if (Debug_logfile == -1) {
    Debug_logfile = open(filename, O_CREAT | O_WRONLY, S_IREAD | S_IWRITE);
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
    write(Debug_logfile, str, strlen(str));
}

void Debug_LogClose() {
  if (Debug_logfile > -1) {
    Debug_LogWrite("\nEND LOG");
    close(Debug_logfile);
    Debug_logfile = -1;
  }
}

// void Debug_SetSpewFilter(uint spew_filter)
void Debug_SetSpewFilter(uint) {}

// uint Debug_GetSpewFilter(uint spew_filter)
uint Debug_GetSpewFilter(uint) { return 0; }

void Debug_ConsoleExit() {
  if (Console_fd >= 0)
    close(Console_fd);
  Console_fd = -1;
}

#define MAX_ARGS 30
#define MAX_CHARS_PER_ARG 100
extern char GameArgs[MAX_ARGS][MAX_CHARS_PER_ARG];
int FindArg(char *which);

bool Debug_ConsoleInit() {
  int n = 0;

  if (Mono_initialized)
    return 1;

  // Only use monochrome if D3_MONO environment var is set
  atexit(Debug_ConsoleExit);

  Console_fd = open("/dev/omono", O_WRONLY);
  if (Console_fd >= 0) {
    Mono_use_real = true;
    Mono_initialized = 1;
  }

  int arg;
  if ((arg = FindArg("-monotcp")) != 0) {
    char address[256];
    strcpy(address, GameArgs[arg + 1]);
    char *port_ptr;
    port_ptr = strchr(address, ':');
    if (port_ptr) {
      *port_ptr = '\0';
      port_ptr++;
      unsigned short port;
      port = atoi(port_ptr);
      if (nw_InitTCPLogging(address, port)) {
        Mono_use_window_remote = true;
        Mono_initialized = 1;
      }
    }
  }
  return 1;
}

void Debug_ConsoleRedirectMessages(int virtual_window, int physical_window) {
  if (!Mono_initialized)
    return;

  /*
  0x02		Redirect Messages

                          Parameters:
                          Offset  Size  Value   Description
                          ======  ====  =====   ===========
                          0x01	0x01   0-7	  Virtual window handle to be associated with physical window
                          0x02	0x01   0-3	  Window handle to be redirected
  */
  // create the packet and send it off
  unsigned char packet[3];
  packet[0x00] = 0x02;            // control code
  packet[0x01] = virtual_window;  // virtual_handle
  packet[0x02] = physical_window; // physical_handle

  // send the packet
  if (Mono_use_real) {
    write(Console_fd, packet, 3);
  }
  if (Mono_use_window_remote) {
    nw_SendMonoPacket(packet, 3);
  }
}

void Debug_ConsoleOpen(int n, int row, int col, int width, int height, char *title) {
  if (!Mono_initialized)
    return;

  /*
  0x00		Open a window

                          Parameters:
                          Offset  Size  Value   Description
                          ======  ====  =====   ===========
                          0x01	0x01   0-3	  Window handle to be associated with window
                          0x02	0x01   0-23   Upper left corner row value
                          0x03	0x01   0-78   Upper left corner col value
                          0x04	0x01   1-79   Number of cols (width)
                          0x05	0x01   1-24   Number of rows (height)
                          0x06	0x01   1-255  Length of the title string
                          0x07->  [0x06] ----   Title string (not \0 terminated)
   */

  // create the packet and send it off
  unsigned char packet[512];
  int len = (title) ? strlen(title) : 1;
  packet[0x00] = 0x00;   // control code
  packet[0x01] = n;      // window_handle
  packet[0x02] = row;    // row
  packet[0x03] = col;    // col
  packet[0x04] = width;  // width
  packet[0x05] = height; // height
  packet[0x06] = len;
  if (title)
    memcpy(&packet[0x07], title, len);
  else
    packet[0x07] = ' ';

  // send the packet
  if (Mono_use_real) {
    write(Console_fd, packet, len + 7);
  }
  if (Mono_use_window_remote) {
    nw_SendMonoPacket(packet, len + 7);
  }
}

void Debug_ConsoleClose(int n) {
  if (!Mono_initialized)
    return;

  /*
   0x01		Closes a window

                          Parameters:
                          Offset  Size  Value   Description
                          ======  ====  =====   ===========
                          0x01	0x01   0-3	  Window handle to be associated with window

   */
  // create the packet and send it off
  unsigned char packet[2];
  packet[0x00] = 0x01; // control code
  packet[0x01] = n;    // window_handle

  // send the packet
  if (Mono_use_real) {
    write(Console_fd, packet, 2);
  }
  if (Mono_use_window_remote) {
    nw_SendMonoPacket(packet, 2);
  }
}

#ifdef _DEBUG
#define MAX_MONO_BUFFER 2048
#else
#define MAX_MONO_BUFFER 32
#endif
static char Mono_buffer[MAX_MONO_BUFFER];
void Debug_ConsolePrintf(int n, char *format, ...) {
  va_list marker;
  va_start(marker, format);
  int text_len = vsnprintf(Mono_buffer, MAX_MONO_BUFFER, format, marker);
  va_end(marker);

  if (n == 0) {
    printf("%s", Mono_buffer);

    int end = strlen(Mono_buffer) - 1;
    if ((end > 1) && (Mono_buffer[end] != 0x0a) && (Mono_buffer[end] != 0x0d)) {
      printf("\n");
    }
  }
  nw_TCPPrintf(n, Mono_buffer);

  if (!Mono_initialized)
    return;

  /*
  0x03		Window Print at cursor position

                          Parameters:
                          Offset  Size  Value   Description
                          ======  ====  =====   ===========
                          0x01	0x01   0-3	  Window handle to be associated with window
                          0x02    0x02   0-512 (Little Endian) Length of the text string
                          0x04	[0x02] ----   Text string (not \0 terminated)
  */

  // create the packet and send it off
  text_len = std::min(text_len, 512);
  unsigned char packet[518];
  packet[0x00] = 0x03; // control code
  packet[0x01] = n;    // window_handle
  *((unsigned short *)&packet[0x02]) = text_len;
  memcpy(&packet[0x04], Mono_buffer, text_len);

  // send the packet
  if (Mono_use_real) {
    write(Console_fd, packet, text_len + 4);
  }
  if (Mono_use_window_remote) {
    nw_SendMonoPacket(packet, text_len + 4);
  }
}

void Debug_ConsolePrintfAt(int n, int row, int col, char *format, ...) {
  if (!Mono_initialized)
    return;

  va_list marker;
  va_start(marker, format);
  int text_len = vsnprintf(Mono_buffer, MAX_MONO_BUFFER, format, marker);
  va_end(marker);

  /*
  0x04		Window Print at row/col given

                          Parameters:
                          Offset  Size  Value   Description
                          ======  ====  =====   ===========
                          0x01	0x01   0-3	  Window handle to be associated with window
                          0x02	0x01   0-24   Starting row (relative to window) of text
                          0x03	0x01   0-79   Starting col (relative to window) of text
                          0x04    0x02   0-512 (Little Endian) Length of the text string
                          0x06   [0x04]  ----   Text string (not \0 terminated)
  */
  // create the packet and send it off
  unsigned char packet[518];
  text_len = std::min(text_len, 512);
  packet[0x00] = 0x04; // control code
  packet[0x01] = n;    // window_handle
  packet[0x02] = row;  // row
  packet[0x03] = col;  // col
  *((unsigned short *)&packet[0x04]) = text_len;
  memcpy(&packet[0x06], Mono_buffer, text_len);

  // send the packet
  if (Mono_use_real) {
    write(Console_fd, packet, text_len + 6);
  }
  if (Mono_use_window_remote) {
    // we can't send these to a mono server, they flood us
    // nw_SendMonoPacket(packet,text_len+6);
  }
}
