/*
 * $Logfile: /DescentIII/Main/linux/lnxcon_null.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2000/04/18 00:00:39 $
 * $Author: icculus $
 *
 * <insert description of file here>
 *
 * $Log: lnxcon_null.cpp,v $
 * Revision 1.1.1.1  2000/04/18 00:00:39  icculus
 * initial checkin
 *
 *
 * 1     4/22/99 2:04a Jeff
 *
 * 4     4/19/99 4:52p Jeff
 * removed useless calls to curses lib for keyboard setup
 *
 * 3     4/19/99 3:58a Jeff
 * got console working again (keyboard based off ddio)
 *
 * 2     4/14/99 1:59a Jeff
 * fixed case mismatched #includes
 *
 * 1     1/12/99 3:42a Jeff
 *
 * $NoKeywords: $
 */

#include "DDAccess.h"
#include "application.h"
#include "AppConsole.h"
#include "TaskSystem.h"
#include "mono.h"

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

// put some data up on the screen
void con_null_Puts(int window, const char *str);

void con_null_Printf(const char *fmt, ...) {
  char buffer[1024];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);
  con_null_Puts(0, buffer);
}

bool con_null_Input(char *buf, int buflen) { return false; }

void con_null_Defer(void) {}

bool con_null_Create(void) {
  printf("Descent 3 Dedicated Server\n");
  printf("Running in quiet mode.\n");
  printf("To Administer, you must telnet in to the dedicated server.\n");
  return true;
}

void con_null_Destroy(void) {}

// put some data up on the screen
void con_null_Puts(int window, const char *str) { mprintf((0, (char *)str)); }
