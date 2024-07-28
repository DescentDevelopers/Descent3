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

#ifndef WIN32
#include <unistd.h>
#include <csignal>
#endif

#include <SDL.h>

#include "program.h"
#include "descent.h"
#include "application.h"
#include "appdatabase.h"
#include "args.h"
#include "init.h"

#include "osiris_dll.h"

#include "log.h"

extern bool ddio_mouseGrabbed;

std::filesystem::path orig_pwd;

bool linux_permit_gamma = false;

static volatile char already_tried_signal_cleanup = 0;

#if (defined DEMO)
#define GAME_NAME_EXT "_demo"
#define GAME_VERS_EXT " Demo"
#elif (defined OEM)
#define GAME_NAME_EXT "_limited"
#define GAME_VERS_EXT " Limited Edition"
#else
#define GAME_NAME_EXT ""
#define GAME_VERS_EXT ""
#endif

void ddio_InternalClose();        // needed for emergency cleanup.

void just_exit(void) {
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
      fprintf(stderr, "Recursive signal cleanup! Hard exit! AHHGGGG!\n");
    else {
      already_tried_signal_cleanup = 1;
      fprintf(stderr, "SIGNAL %d caught, aborting\n", signum);
      just_exit();
    } // else
    break;
  case SIGXCPU:
  case SIGXFSZ:
    break;
  }

  _exit(-10);
}

void safe_signal_handler(int signum) {}

void install_signal_handlers() {
  struct sigaction sact, fact;

  memset(&sact, 0, sizeof(sact));
  memset(&fact, 0, sizeof(fact));
  sact.sa_handler = safe_signal_handler;
  fact.sa_handler = fatal_signal_handler;

  if (sigaction(SIGHUP, &fact, NULL))
    fprintf(stderr, "SIG: Unable to install SIGHUP\n");
  if (sigaction(SIGABRT, &fact, NULL))
    fprintf(stderr, "SIG: Unable to install SIGABRT\n");
  if (sigaction(SIGINT, &fact, NULL))
    fprintf(stderr, "SIG: Unable to install SIGINT\n");
  if (sigaction(SIGBUS, &fact, NULL))
    fprintf(stderr, "SIG: Unable to install SIGBUS\n");
  if (sigaction(SIGFPE, &fact, NULL))
    fprintf(stderr, "SIG: Unable to install SIGFPE\n");
  if (sigaction(SIGILL, &fact, NULL))
    fprintf(stderr, "SIG: Unable to install SIGILL\n");
  if (sigaction(SIGQUIT, &fact, NULL))
    fprintf(stderr, "SIG: Unable to install SIGQUIT\n");
  if (sigaction(SIGSEGV, &fact, NULL))
    fprintf(stderr, "SIG: Unable to install SIGSEGV\n");
  if (sigaction(SIGTERM, &fact, NULL))
    fprintf(stderr, "SIG: Unable to install SIGTERM\n");
  if (sigaction(SIGXCPU, &fact, NULL))
    fprintf(stderr, "SIG: Unable to install SIGXCPU\n");
  if (sigaction(SIGXFSZ, &fact, NULL))
    fprintf(stderr, "SIG: Unable to install SIGXFSZ\n");
  if (sigaction(SIGVTALRM, &fact, NULL))
    fprintf(stderr, "SIG: Unable to install SIGVTALRM\n");
  if (sigaction(SIGTRAP, &fact, NULL))
    fprintf(stderr, "SIG: Unable to install SIGTRAP\n");
}
#else
void install_signal_handlers() {}
#endif
//	---------------------------------------------------------------------------
//	Define our operating system specific extensions to the gameos system
//	---------------------------------------------------------------------------

class oeD3LnxApp : public oeLnxApplication {
  bool shutdown, final_shutdown;
  int old_screen_mode;

public:
  oeD3LnxApp(unsigned flags);
  virtual ~oeD3LnxApp() { final_shutdown = true; };

  void run() { Descent3(); };
};

oeD3LnxApp::oeD3LnxApp(unsigned flags) : oeLnxApplication(flags) {
  Descent = this;
  shutdown = false;
  final_shutdown = false;
}

class oeD3LnxDatabase : public oeLnxAppDatabase {
public:
  oeD3LnxDatabase();
};

//	---------------------------------------------------------------------------
//	D3LnxDatabase operating system specific initialization
oeD3LnxDatabase::oeD3LnxDatabase() : oeLnxAppDatabase() {
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
    strcpy(path, Base_directory);
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

int sdlKeyFilter(const SDL_Event *event);
int sdlMouseButtonUpFilter(const SDL_Event *event);
int sdlMouseButtonDownFilter(const SDL_Event *event);
int sdlMouseWheelFilter(const SDL_Event *event);
int sdlMouseMotionFilter(const SDL_Event *event);

int SDLCALL d3SDLEventFilter(void *userdata, SDL_Event *event) {
  switch (event->type) {
  case SDL_KEYUP:
  case SDL_KEYDOWN:
    return (sdlKeyFilter(event));
  case SDL_JOYBALLMOTION:
  case SDL_MOUSEMOTION:
    return (sdlMouseMotionFilter(event));
  case SDL_MOUSEBUTTONUP:
    return (sdlMouseButtonUpFilter(event));
  case SDL_MOUSEBUTTONDOWN:
    return (sdlMouseButtonDownFilter(event));
  case SDL_MOUSEWHEEL:
    return (sdlMouseWheelFilter(event));
  case SDL_QUIT:
    SDL_Quit();
    _exit(0);
    break;
  default: break;
  } // switch

  return (1);
}

void StartDedicatedServer();

#ifdef BETAEXPIRE
static void check_beta() {
  fprintf(stderr, "\n\n\n");
  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "***************************************************************************\n");

  if (time(NULL) > (BETAEXPIRE + 30 * 24 * 60 * 60)) {
    fprintf(stderr, "Thanks for participating in the Descent 3 beta test!\n"
                    "This beta copy has now expired.\n"
                    "Please visit http://www.lokigames.com/ for a non-beta release.\n");
    _exit(0);
  } // if
  else {
    fprintf(stderr, "Warning: This is a beta version of DESCENT 3.\n"
                    "Please report any bugs in fenris: http://fenris.lokigames.com/\n");
  } // else

  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "\n\n\n");
} // check_beta
#endif

//	---------------------------------------------------------------------------
//	Main
//		creates all the OS objects and then runs Descent 3.
//		this is all this function should do.
//	---------------------------------------------------------------------------
#ifdef WIN32
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szCmdLine, int nCmdShow) {
  strupr(szCmdLine);
  GatherArgs(szCmdLine);
#else
int main(int argc, char *argv[]) {
  GatherArgs(argv);
#endif

  orig_pwd = std::filesystem::current_path();

  /* Setup the logging system */
  InitLog();

  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

#ifdef BETAEXPIRE
  // IMPORTANT - TAKE ME OUT - FIXME -------------------------------
#warning "***********************************************************"
#warning "***********************************************************"
#warning "***********************************************************"
#warning "***********************************************************"
#warning "***********************************************************"
#warning "***********************************************************"
#warning "****   Please remove -DBETAEXPIRE from the Makefile! ******"
#warning "***********************************************************"
#warning "***********************************************************"
#warning "***********************************************************"
#warning "***********************************************************"
#warning "***********************************************************"
#warning "***********************************************************"
  check_beta();
#endif

#ifdef __DUMP_MVE_TO_DISK
  // IMPORTANT - TAKE ME OUT - FIXME -------------------------------
#warning "***********************************************************"
#warning "***********************************************************"
#warning "***********************************************************"
#warning "***********************************************************"
#warning "***********************************************************"
#warning "***********************************************************"
#warning "** Please remove -D__DUMP_MVE_TO_DISK from the Makefile! **"
#warning "***********************************************************"
#warning "***********************************************************"
#warning "***********************************************************"
#warning "***********************************************************"
#warning "***********************************************************"
#warning "***********************************************************"

  fprintf(stderr, "\n\n\n");
  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "    Warning, this binary dumps movies to disk. This is BAD.\n");
  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "***************************************************************************\n");
  fprintf(stderr, "\n\n\n");
#endif

  /*
      if ( (argv[1] != NULL) && (strcasecmp(argv[1], "--nettest")) )
          _exit(nettest_Main(argc, argv));
  */

  // rcg01152000 need this for mpeg playback currently.
  // rcg02232004 probably don't need this anymore.
  // if (getenv("SDL_VIDEO_YUV_HWACCEL") == NULL)
  //    putenv("SDL_VIDEO_YUV_HWACCEL=0");

  int x;

  /*
      x = FindArg("-nettest");
      if (x)
      {
          if (x != 1)
              printf("  --nettest must be first command if you use it.\n");
          _exit(0);
      } // if
  */

  #ifdef DEDICATED
  setenv("SDL_VIDEODRIVER", "dummy", 1);
  #endif

  int rc = SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
  if (rc != 0) {
    fprintf(stderr, "SDL: SDL_Init() failed! rc == (%d).\n", rc);
    fprintf(stderr, "SDL_GetError() reports \"%s\".\n", SDL_GetError());
    return (0);
  } // if

  //    atexit(SDL_Quit);

  // !!! FIXME: Don't use an event filter!
  SDL_SetEventFilter(d3SDLEventFilter, NULL);
  install_signal_handlers();

  // build the command line as one long string, seperated by spaces...
  /*
          char commandline[256];
          strcpy(commandline,"");
          for(int i=0;i<argc;i++){
          strcat(commandline,argv[i]);
          strcat(commandline," ");
          }

          GatherArgs (commandline);
  */
  int winArg = FindArgChar("-windowed", 'w');
  int fsArg = FindArgChar("-fullscreen", 'f');

  if ((fsArg) && (winArg)) {
    fprintf(stderr, "ERROR: %s AND %s specified!\n", GameArgs[winArg], GameArgs[fsArg]);
    return (0);
  } // if

  if (FindArg("-game_checksum")) {
    extern tOSIRISModuleInit Osiris_module_init;
    extern void Osiris_CreateModuleInitStruct(tOSIRISModuleInit * st);
    extern uint32_t Osiris_CreateGameChecksum(void);

    Osiris_CreateModuleInitStruct(&Osiris_module_init);
    uint32_t checksum = Osiris_CreateGameChecksum();
    printf("Descent 3\n");
    printf("Game Checksum: %u\n", checksum);
    return (0);
  } else {
    /*initialize our OS Object.  This could be a game dependant OS object, or a default OS object.
    once we create it, if successful, we can start the game.
    */
    int flags = 0;

    if (!FindArgChar("-dedicated", 'd')) {
#ifndef DEDICATED
      // check for a renderer

      if ((FindArgChar("-windowed", 'w')) && (FindArgChar("-fullscreen", 'f'))) {
        fprintf(stderr, "Error: Both windowed and fullscreen mode requested.");
        return 0;
      }

      if (FindArgChar("-nomousegrab", 'm')) {
        flags |= APPFLAG_NOMOUSECAPTURE;
      }

      ddio_mouseGrabbed = (FindArgChar("-nomousegrab", 'm') == 0);

      if (!FindArg("-sharedmemory")) {
        flags |= APPFLAG_NOSHAREDMEMORY;
      }
      flags |= APPFLAG_WINDOWEDMODE;

      if (!FindArg("-nodgamouse")) {
        flags |= APPFLAG_DGAMOUSE;
      }
#else
      fprintf(stderr, "Error: \"--dedicated\" or \"-d\" flag required\n");
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
        fprintf(stderr, "Unable to fork process\n");
      }
      if (np == 0) {
        run_d3 = true;
        np = setsid();
        pid_t pp = getpid();
        printf("Successfully forked process [new sid=%d pid=%d]\n", np, pp);
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
  }

  just_exit();
  return 0;
}
