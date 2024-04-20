#include <stdarg.h>
#include <math.h>
#include <ctype.h>
#include "ddebug/debug.h"
#include "mem.h"
#include "ddio.h"

// void atexit(void *proc)
//{
//	mprintf((2, "atexit\n"));
//}

int stricmp(const char *s1, const char *s2) {
  char c1, c2;
  while (1) {
    c1 = tolower(*s1++);
    c2 = tolower(*s2++);
    if (c1 < c2)
      return -1;
    if (c1 > c2)
      return 1;
    if (c1 == 0)
      return 0;
  }
}
int strnicmp(const char *s1, const char *s2, int n) {
  int i;
  char c1, c2;
  for (i = 0; i < n; i++) {
    c1 = tolower(*s1++);
    c2 = tolower(*s2++);
    if (c1 < c2)
      return -1;
    if (c1 > c2)
      return 1;
    if (!c1)
      return 0;
  }
  return 0;
}
void Sleep(int millis) {
  longlong start = timer_GetMSTime();
  while (timer_GetMSTime() - start < millis)
    ;
}
#define USE_MALLOC

void HeapFree(HANDLE heap, int dummy, void *mptr) {
  if (mptr)
#ifdef USE_MALLOC
    free(mptr);
#else
    DisposePtr((char *)mptr);
#endif
}

void *HeapAlloc(HANDLE heap, int dummy, int size) {
  if (size <= 0)
    return NULL;
#ifdef USE_MALLOC
  return malloc(size);
#else
  return NewPtr(size);
#endif
}
void *HeapReAlloc(HANDLE heap, int dummy, void *oldblock, int size) {
  if (size <= 0)
    return NULL;
#ifdef USE_MALLOC
  return (realloc(oldblock, size));
#else
  SetPtrSize((char *)oldblock, size);
  return (oldblock);
#endif
}

void GlobalFree(void *mptr) {
  if (mptr)
    mem_free(mptr);
}
void *GlobalAlloc(int flags, int size) {
  if (size <= 0)
    return NULL;
  return mem_malloc(size);
}

void *GlobalLock(HGLOBAL hMem) { return hMem; }
char *strupr(char *string) {
  while (string && *string) {
    *string = toupper(*string);
    string++;
  }
  return string;
}
char *itoa(int value, char *string, int radix) {
  if (radix == 10) {
    sprintf(string, "%d", value);
  } else if (radix == 16) {
    sprintf(string, "%x", value);
  } else {
    DebugStr("\p!!!!!!!!!!!!!!!WARNING CALLING itoa WITHOUT 10 or 16 RADIX!!!!!!!!!!!!!!!!!!!!!!");
    sprintf(string, "%d", value);
  }
  return string;
}

void Debug_ConsoleRedirectMessages(int virtual_window, int physical_window) {}

int debug_level = 2;
void Debug_ConsolePrintf(int n, char *format, ...) {
  if (n >= debug_level) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
  }
  return;
}
