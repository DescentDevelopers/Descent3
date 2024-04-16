#include "logfile.h"
#include "pstring.h"
#include <stdio.h>
#include <stdarg.h>

#ifdef _DEBUG
static bool log_enable = true;
#else
static bool log_enable = false;
#endif

void log_Enable(bool enable) { log_enable = true; }

void log_Disable() { log_enable = false; }

logfile::logfile() { fp = NULL; }

logfile::~logfile() { end(); }

// restarts the logfile (opens a new one.)
void logfile::start(const char *fname, const char *longname) {
  if (log_enable) {
    try {
      fp = (FILE *)fopen(fname, "wt");
      logfile::printf("%s\n", longname);
    } catch (...) {
      fp = NULL;
    }
  }
}

void logfile::end() {
  if (fp) {
    try {
      fclose((FILE *)fp);
      fp = NULL;
    } catch (...) {
      fp = NULL;
    }
  }
}

void logfile::printf(const char *fmt, ...) {
  if (fp && fmt) {
    char msg[256];
    va_list arglist;
    va_start(arglist, fmt);
    Pvsprintf(msg, sizeof(msg), fmt, arglist);
    va_end(arglist);

    logfile::puts(msg);
  }
}

void logfile::puts(const char *msg) {
  if (fp && msg) {
    try {
      fputs(msg, (FILE *)fp);
    } catch (...) {
      end();
    }
  }
}

void logfile::update() {
  if (fp) {
    try {
      fflush((FILE *)fp);
    } catch (...) {
      end();
    }
  }
}
