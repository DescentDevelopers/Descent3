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

 *
 * Linux Main and any Linux specific code that MUST be in the main application
 * goes here.
 * $NoKeywords: $
 */

#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <map>

#ifndef WIN32
#include <unistd.h>
#include <csignal>
#endif

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "appdatabase.h"
#include "application.h"
#include "args.h"
#include "d3_version.h"
#include "ddio.h"
#include "descent.h"
#include "dedicated_server.h"
#include "init.h"
#include "log.h"
#include "config.h"

#ifdef WIN32
#include "debug.h"
#endif

std::filesystem::path orig_pwd;

static volatile char already_tried_signal_cleanup = 0;

void just_exit() {
  ddio_InternalClose(); // try to reset serial port.

  SDL_Quit();
#if defined(POSIX)
  sync(); // just in case.
#endif
  _exit(0);
}

#if defined(POSIX)
void fatal_signal_handler(int signum) {
  switch (signum) {
  case SIGHUP:
  case SIGTRAP:
  case SIGABRT:
  case SIGBUS:
  case SIGFPE:
  case SIGILL:
  case SIGQUIT:
  case SIGSEGV:
  case SIGTERM:
  case SIGVTALRM:
  case SIGINT:
    if (already_tried_signal_cleanup)
      LOG_WARNING << "Recursive signal cleanup! Hard exit! AHHGGGG!";
    else {
      already_tried_signal_cleanup = 1;
      LOG_WARNING.printf("SIGNAL %d caught, aborting", signum);
      just_exit();
    } // else
    break;
  default:
    break;
  }

  _exit(-10);
}

void install_signal_handlers() {
  struct sigaction fact{};

  memset(&fact, 0, sizeof(fact));
  fact.sa_handler = fatal_signal_handler;

  const std::map<int, std::string> signals = {
      {SIGHUP, "SIGHUP"},
      {SIGABRT, "SIGABRT"},
      {SIGINT, "SIGINT"},
      {SIGBUS, "SIGBUS"},
      {SIGFPE, "SIGFPE"},
      {SIGILL, "SIGILL"},
      {SIGQUIT, "SIGQUIT"},
      {SIGSEGV, "SIGSEGV"},
      {SIGTERM, "SIGTERM"},
      {SIGXCPU, "SIGXCPU"},
      {SIGXFSZ, "SIGXFSZ"},
      {SIGVTALRM, "SIGVTALRM"},
      {SIGTRAP, "SIGTRAP"},
  };

  for (const auto &signal : signals) {
    if (sigaction(signal.first, &fact, nullptr) != 0) {
      LOG_WARNING << "SIG: Unable to install " << signal.second;
    }
  }
}
#else
void install_signal_handlers() { SetUnhandledExceptionFilter(RecordExceptionInfo); }
#endif

//	---------------------------------------------------------------------------
//	Define our operating system specific extensions to the gameos system
//	---------------------------------------------------------------------------
class oeD3LnxApp final : public oeLnxApplication {
  bool shutdown, final_shutdown;
  int old_screen_mode;

public:
  oeD3LnxApp(unsigned flags) : oeLnxApplication(flags) {
    Descent = this;
    shutdown = false;
    final_shutdown = false;
  }
  virtual ~oeD3LnxApp() { final_shutdown = true; };

  void run() { Descent3(); };
};

class oeD3LnxDatabase final : public oeLnxAppDatabase {
public:
  oeD3LnxDatabase() {
    char path[_MAX_PATH];
    char netpath[_MAX_PATH];

    // put directories into database

#ifdef EDITOR
    create_record("D3Edit");
#else
    create_record("Descent3");
#endif

    char *dir = getenv("D3_LOCAL");
    char *netdir = getenv("D3_DIR");

    if (!dir)
      strcpy(path, "");
    else
      strcpy(path, dir);

    if (!netdir)
      strcpy(netpath, "");
    else
      strcpy(netpath, netdir);

    write("local directory", path, strlen(path) + 1);
    write("net directory", netpath, strlen(netpath) + 1);
    Database = this;
  }
};

bool sdlKeyFilter(const SDL_Event *event);
bool sdlMouseButtonUpFilter(const SDL_Event *event);
bool sdlMouseButtonDownFilter(const SDL_Event *event);
bool sdlMouseWheelFilter(const SDL_Event *event);
bool sdlMouseMotionFilter(const SDL_Event *event);

bool SDLCALL d3SDLEventFilter(void *userdata, SDL_Event *event) {
  switch (event->type) {
  case SDL_EVENT_KEY_UP:
  case SDL_EVENT_KEY_DOWN:
    return (sdlKeyFilter(event));
  case SDL_EVENT_JOYSTICK_BALL_MOTION:
  case SDL_EVENT_MOUSE_MOTION:
    return (sdlMouseMotionFilter(event));
  case SDL_EVENT_MOUSE_BUTTON_UP:
    return (sdlMouseButtonUpFilter(event));
  case SDL_EVENT_MOUSE_BUTTON_DOWN:
    return (sdlMouseButtonDownFilter(event));
  case SDL_EVENT_MOUSE_WHEEL:
    return (sdlMouseWheelFilter(event));
  case SDL_EVENT_QUIT:
    SDL_Quit();
    _exit(0);
    break;
  default:
    break;
  } // switch

  return (1);
}

//	---------------------------------------------------------------------------
//	Main
//		creates all the OS objects and then runs Descent 3.
//		this is all this function should do.
//	---------------------------------------------------------------------------
#ifdef WIN32
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szCmdLine, int nCmdShow) {
  strupr(szCmdLine);
  GatherArgs(szCmdLine);
  bool enable_winconsole = FindArg("-winconsole");
#else
int main(int argc, char *argv[]) {
  GatherArgs(argv);
  bool enable_winconsole = true;
#endif

  orig_pwd = std::filesystem::current_path();

  /* Set up the logging system */
#ifdef RELEASE
  plog::Severity log_level = plog::info;
#else
  plog::Severity log_level = plog::debug;
#endif

  int loglevel_farg = FindArg("-loglevel");
  if (loglevel_farg) {
    log_level = plog::severityFromString(GameArgs[loglevel_farg + 1]);
  }
  InitLog(log_level, FindArg("-logfile"), enable_winconsole);

  LOG_INFO.printf("Welcome to Descent 3 v%d.%d.%d %s", D3_MAJORVER, D3_MINORVER, D3_BUILD, D3_GIT_HASH);

#ifdef DEDICATED
  setenv("SDL_VIDEODRIVER", "dummy", 1);
#endif

  SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "0");
  int rc = SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
  if (!rc) {
    LOG_FATAL.printf("SDL: SDL_Init() failed: %d: %s!", rc, SDL_GetError());
    return (0);
  }

  // !!! FIXME: Don't use an event filter!
  SDL_SetEventFilter(d3SDLEventFilter, nullptr);
  install_signal_handlers();

  // Initialize our OS Object.  This could be a game dependant OS object, or a default OS object.
  // Once we create it, if successful, we can start the game.
  int flags = 0;

  if (!FindArgChar("-dedicated", 'd')) {
#ifndef DEDICATED
    // check for a renderer

    if (FindArgChar("-nomousegrab", 'm')) {
      flags |= APPFLAG_NOMOUSECAPTURE;
    ddio_MouseSetGrab(false);
      }

    if (!FindArg("-sharedmemory")) {
      flags |= APPFLAG_NOSHAREDMEMORY;
    }
    flags |= APPFLAG_WINDOWEDMODE;


#else
    LOG_FATAL << "Error: \"--dedicated\" or \"-d\" flag required";
    return 0;
#endif

  } else {
    // Dedicated Server Mode
    flags |= OEAPP_CONSOLE;

    // service flag overrides others here in the group
    if (FindArg("-service")) {
      flags |= APPFLAG_USESERVICE;
    }
  }

  bool run_d3 = true;
#if defined(POSIX)
  if (flags & APPFLAG_USESERVICE) {
    run_d3 = false;
    pid_t np = fork();
    if (np == -1) {
      LOG_WARNING << "Unable to fork process";
    }
    if (np == 0) {
      run_d3 = true;
      np = setsid();
      pid_t pp = getpid();
      LOG_INFO.printf("Successfully forked process [new sid=%d pid=%d]", np, pp);
    }
  }
#endif

  if (run_d3) {
    oeD3LnxApp d3(flags);
    oeD3LnxDatabase dbase;
    StartDedicatedServer();
    PreInitD3Systems();

    d3.init();
    d3.run();
  }

  just_exit();
  return 0;
}
