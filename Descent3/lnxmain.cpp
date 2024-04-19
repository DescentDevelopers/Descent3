/*
 *
 * Linux Main and any Linux specific code that MUST be in the main application
 * goes here.
 * $NoKeywords: $
 */

#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>

#ifdef __PERMIT_MAKEHOG
#include <dirent.h>
#define _GNU_SOURCE
#include <fnmatch.h>
#endif

#include "SDL.h"
#include "program.h"
#include "mono.h"
#include "descent.h"
#include "application.h"
#include "appdatabase.h"
#include "pserror.h"
#include "args.h"
#include "init.h"
#include "renderer.h"

#include "ddio.h"
#include "ddvid.h"
#include "osiris_dll.h"
#include "loki_utils.h"

#if defined(MACOSX)
#include <SDL.h>
#endif

extern bool ddio_mouseGrabbed;
int no_debug_dialog = 0;
char *DMFCGetString(int d);
// void *x = (void *) DMFCGetString;   // just force a reference to dmfc.so ...

char *__orig_pwd = NULL;

bool linux_permit_gamma = false;

typedef struct {
  char *lng;
  char sht;
  char *comment;
} cmdLineArg;

static cmdLineArg d3ArgTable[] = {
#ifdef __PERMIT_LINUX_GLIDE
    {"rend_opengl", 'G', "Use OpenGL for 3D rendering."},
    {"rend_glide", 'L', "Use Glide for 3D rendering."},
#endif

#ifdef __PERMIT_PLAYMVE
    {"playmve", 'p', "Play a specified movie."},
#endif

#ifdef __PERMIT_MAKEHOG
    {"makehog", 'a', "Make a HOG archive from a list of names."},
    {"dumphog", 'A', "Dump a HOG archive to a directory."},
#endif

    {"glidelibrary", 'l', "Select Glide rendering library."},
    {"gllibrary", 'g', "Select OpenGL rendering library."},
    {"rocknride", 'r', "Enable Rock'n'Ride (http://www.rocknride.com)."},
    {"cobra", 'R', "Enable Cobra chair support."},

#if (!defined(DEMO))
    {"dedicated", 'd', "Run as a dedicated netgame server."},
    {"nointro", 'n', "Skip intro movie."},
#endif

    {"joystick", 'j', "Specify a joystick (number)."},
    {"nomousegrab", 'm', "Don't grab the mouse."},
    {"cdrom", 'C', "Specify a path to check for CD-ROM based files."},

    {"deadzone0", 'D', "Specify a joystick deadzone (0.0 to 1.0)"},

    {"gspyfile", 'S', "Specify a GameSpy config file."},
    {"timetest", 'T', "Run a demo benchmark."},
    {"fastdemo", 'Q', "Run demos as fast as possible."},
    {"framecap", 'F', "Specify a framecap (for dedicated server)."},

    {"tempdir", 'P', "Specify directory for temporary files."},

#if (defined(_USE_OGL_LISTS_OPTIONAL))
    {"gllists", '\0', "Use OpenGL lists."},
#endif

#ifdef __PERMIT_GL_LOGGING
    {"gllogging", '\0', "to be removed."},
#endif

    {"nomultitexture", 't', "Disable multitexturing."},
    {"nopackedpixels", 'x', "Disable packed pixels."},
    {"glfog", 'o', "Enable OpenGL fog."},
    {"nogamma", 'M', "Disable gamma support."},
    {"glinfo", 'I', "Display info about OpenGL library."}

#ifdef __CHECK_FOR_TOO_SLOW_RENDERING__
    {"nofpscheck", 'H', "Disable FPS checking."},
#endif
};

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

// #define DEDICATED
namespace {
extern "C" {
char game_version_buffer[150];
char *game_version = game_version_buffer;
}
} // namespace

// Given a device/drive, this marks it as the default CD-ROM drive
void ddio_MarkDefaultCDDrive(char *drive);
void ddio_InternalClose();        // needed for emergency cleanup.
void cdrom_system_shutdown(void); // needed for emergency cleanup.

#ifdef __PERMIT_LINUX_GLIDE
void glide_Close(void);
#endif

void just_exit(void) {
  ddio_InternalClose(); // try to reset serial port.
  cdrom_system_shutdown();

#ifdef __PERMIT_LINUX_GLIDE
  if (Renderer_type == RENDERER_GLIDE)
    glide_Close();
#endif

  SDL_Quit();
  sync(); // just in case.
  _exit(0);
}

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

  sync(); // just in case.
  _exit(-10);
}

void safe_signal_handler(int signum) {}

void install_signal_handlers(void) {
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
  create_record("Descent3");

  char *dir = getenv("D3_LOCAL");
  char *netdir = getenv("D3_DIR");

  if (!dir)
    strcpy(path, loki_getdatapath()); //"/usr/local/games/descent3");
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

static void register_d3_args(void) {
  loki_register_stdoptions();

  for (int i = 0; i < sizeof(d3ArgTable) / sizeof(d3ArgTable[0]); i++) {
    loki_registeroption(d3ArgTable[i].lng, d3ArgTable[i].sht, d3ArgTable[i].comment);
  } // for
} // register_d3_args

int sdlKeyFilter(const SDL_Event *event);
int sdlMouseButtonUpFilter(const SDL_Event *event);
int sdlMouseButtonDownFilter(const SDL_Event *event);
int sdlMouseMotionFilter(const SDL_Event *event);

int d3SDLEventFilter(const SDL_Event *event) {
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
  } // switch

  return (1);
}

void StartDedicatedServer();

#ifdef __PERMIT_MAKEHOG

static void hogfileRefresh(const char *x) { printf(" - %s\n", x); } // hogfileRefresh

int CreateNewHogFile(const char *hogname, int nfiles, const char **filenames, void (*UpdateFunction)(char *));

// hack of the century.
static void buildNewHogFromFileList(char *fileName) {
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  if (fileName == NULL) {
    printf("\n\nno filename specified.\n\n");
    return;
  } // if

  char **files = (char **)malloc(sizeof(char *) * 8000);
  int fCount = 0;

  FILE *f = fopen(fileName, "rb");
  if (f == NULL) {
    printf("\n\ncan't open [%s].\n\n", fileName);
    return;
  } // if

  int alreadyHere = 0;
  int i = 0;
  int j = 0;

  do {
    int ch = 0;
    files[i] = (char *)malloc(300);
    files[i][0] = '\0';
    for (j = 0; (ch != '\n') && (ch != EOF); j++) {
      ch = fgetc(f);
      files[i][j] = (char)ch;
    }

    files[i][j - 1] = '\0';

    // !!! trim spaces...

    if (files[i][0] == '\0')
      alreadyHere = 1;
    else {
      alreadyHere = 0;

      for (int n = 0; n < i; n++) {
        if (strcasecmp(files[n], files[i]) == 0)
          alreadyHere = 1;
      } // for

      if ((!alreadyHere) && (access(files[i], R_OK) != 0)) {
        alreadyHere = 1;
        DIR *d = opendir(".");
        struct dirent *ent = readdir(d);
        while (ent != NULL) {
          if (fnmatch(files[i], ent->d_name, FNM_CASEFOLD) == 0) {
            rename(ent->d_name, files[i]);
            ent = NULL;
            alreadyHere = 0; // ok, so this variable is misnamed...
          }                  // if
          else
            ent = readdir(d);
        } // while
        closedir(d);

        if (alreadyHere)
          printf("File [%s] is missing.\n", files[i]);
      } // if
    }   // if

    if (alreadyHere == 0) {
      printf(" - %s\n", files[i]);
      i++;
      fCount++;
    } else
      free(files[i]);
  } while (!feof(f));

  fclose(f);

  int swapped;
  do {
    swapped = 0;

    for (int y = 0; y < fCount - 1; y++) {
      if (strcasecmp(files[y], files[y + 1]) > 0) {
        char *tmp = files[y];
        files[y] = files[y + 1];
        files[y + 1] = tmp;
        swapped = 1;
      } // if
    }   // for
  } while (swapped);

  CreateNewHogFile("new.hog", i, (const char **)files, (void (*)(char *))hogfileRefresh);
} // buildNewHogFileFromList

#endif

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
int main(int argc, char *argv[]) {
  __orig_pwd = getcwd(NULL, 0);

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

  GatherArgs(argv);

  snprintf(game_version_buffer, sizeof(game_version_buffer), "%d.%d.%d%s%s", D3_MAJORVER, D3_MINORVER, D3_BUILD,
           LOKI_VERSION, GAME_VERS_EXT);

  loki_setgamename("descent3" GAME_NAME_EXT, game_version_buffer, "Descent 3");

  snprintf(game_version_buffer, sizeof(game_version_buffer),
           "\n\n"
           "Descent 3 Linux %s v%d.%d.%d%s\n"
           "Copyright (C) 1999 Outrage Entertainment, Inc.\n",
#ifdef DEDICATED
           "Dedicated Server",
#elif DEMO
           "Demo",
#else
           "Client",
#endif
           D3_MAJORVER, D3_MINORVER, D3_BUILD, LOKI_VERSION);

  game_version += 2; // skip those first newlines for loki_initialize.

  register_d3_args();
  loki_initialize(argc, argv, game_version_buffer);

  int x;

#ifdef __PERMIT_MAKEHOG
  x = FindArgChar("-dumphog", 'A');
  if (x) {
    if (x != 1)
      printf("  --dumphog must be first command if you use it.\n");
    else
      dump_hog_to_directory(argv[x + 1]);
    _exit(0);
  } // if

  x = FindArgChar("-makehog", 'a');
  if (x) {
    if (x != 1)
      printf("  --makehog must be first command if you use it.\n");
    else
      buildNewHogFromFileList(argv[x + 1]);
    _exit(0);
  } // if
#endif

  /*
      x = FindArg("-nettest");
      if (x)
      {
          if (x != 1)
              printf("  --nettest must be first command if you use it.\n");
          _exit(0);
      } // if
  */

  SDL_ClearError();
  int rc = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_CDROM);
  if (rc != 0) {
    fprintf(stderr, "SDL: SDL_Init() failed! rc == (%d).\n", rc);
    fprintf(stderr, "SDL_GetError() reports \"%s\".\n", SDL_GetError());
    return (0);
  } // if

  //    atexit(SDL_Quit);

  // !!! FIXME: Don't use an event filter!
  SDL_SetEventFilter(d3SDLEventFilter);
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
    void Osiris_CreateModuleInitStruct(tOSIRISModuleInit * st);
    unsigned int Osiris_CreateGameChecksum(void);

    Osiris_CreateModuleInitStruct(&Osiris_module_init);
    unsigned int checksum = Osiris_CreateGameChecksum();
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

#ifdef __PERMIT_LINUX_GLIDE
      int opengl_rend = (FindArgChar("+rend_opengl", 'G')) ? 1 : 0;
      int glide_rend = (FindArgChar("+rend_glide", 'L')) ? 1 : 0;

      if (!glide_rend && !opengl_rend) {
        // fprintf(stderr,"Error: --dedicated or renderer flag required\n");
        // return 0;

        // ryan sez: Glide by default, beeyatch.
        glide_rend = 1;
      }

      // check for multiple renderers defined
      if (glide_rend + opengl_rend > 1) {
        fprintf(stderr, "Error: Too many renderer's defined, use only one\n");
        return 0;
      }

      /*
              //print out renderer
              if(glide_rend)
              {
                      fprintf(stderr,"Renderer: GLiDE\n");
              }
              if(opengl_rend)
              {
                      fprintf(stderr,"Renderer: OpenGL\n");
              }
      */
#endif

      if (FindArgChar("-nomousegrab", 'm')) {
        flags |= APPFLAG_NOMOUSECAPTURE;
      }

      ddio_mouseGrabbed = (FindArgChar("-nomousegrab", 'm') == 0);

      if (!FindArg("-sharedmemory")) {
        flags |= APPFLAG_NOSHAREDMEMORY;
      }

#ifdef __PERMIT_LINUX_GLIDE
      if (FindArgChar("+rend_opengl", 'G')) {
        flags |= APPFLAG_WINDOWEDMODE;
      }
#else
      flags |= APPFLAG_WINDOWEDMODE;
#endif

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
      } else {
        if (FindArg("-svgalib")) {
          flags |= APPFLAG_USESVGA;
        }
      }
    }

    bool run_d3 = true;
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

    if (run_d3) {
      int cdrom_arg;
      cdrom_arg = FindArgChar("-cdrom", 'C');
      if (cdrom_arg > 0) {
        ddio_MarkDefaultCDDrive(GameArgs[cdrom_arg + 1]);
      }

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
