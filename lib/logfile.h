
#ifndef LOGFILE_H
#define LOGFILE_H

void log_Enable(bool enable);
void log_Disable();

class logfile {
  void *fp;

public:
  logfile(); // simple constructor
  ~logfile();

  void start(const char *fname, const char *longname = 0); // restarts the logfile (opens a new one.)
  void end();

  void printf(const char *fmt, ...);
  void puts(const char *msg);
  void update(); // call to update logfile on disk (done by OS otherwise)
};

#endif
