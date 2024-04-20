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
 * $Logfile: /DescentIII/Main/macmain.cpp $
 * $Revision: 1.1.1.1 $
 * $Date: 2003/08/26 03:56:30 $
 * $Author: kevinb $
 *
 * Macintosh Main and any macintosh specific code that MUST be in the main application
 * goes here.
 *
 * $Log: macmain.cpp,v $
 * Revision 1.1.1.1  2003/08/26 03:56:30  kevinb
 * initial 1.5 import
 *
 *
 * 6     4/12/00 6:41p Matt
 * From Duane for 1.4
 *
 * 5     3/21/00 11:56a Matt
 * Merge of Duane's post-1.3 changes.
 *
 * 3     10/22/99 11:13a Matt
 * Mac merge
 *
 * 4     5/18/97 6:08 PM Jeremy
 * changed compiler directive use for profiler, apparently __profile__ is
 * always defined for some reason under codewarrior (regardless of project
 * preferences setting)
 *
 * 3     5/17/97 6:51 PM Jeremy
 * implemented default values for database entries of net_directory,
 * local_directory, and user_name, and put support for code profiling into
 * place.
 *
 * 2     5/15/97 1:42 AM Jeremy
 * some initialization mprintf's and moved the keyboard handler into the
 * osd3macobject from the osmacobject
 *
 * 1     5/14/97 11:26 PM Jeremy
 * initial checkin
 *
 * $NoKeywords: $
 */

// =========================
// ANSI Headers
// =========================
#include <stdlib.h>
#include <string.h>

// =========================
// Metrowerks Headers
// =========================

#ifdef USE_PROFILER
#include <profiler.h>
// #include "SpotlightAPI.h"
#endif
#ifdef NEEDED
// =========================
// Macintosh System Headers
// =========================
#include <QuickDraw.h>
#include <Fonts.h>
#include <Windows.h>
#include <Menus.h>
#include <TextEdit.h>
#include <Dialogs.h>
#endif
#include <InSprocket.h>
#include <SoundSprocket.h>
#include <DrawSprocket.h>
#include "agl.h"
#include "glide.h"
#include <OpenTptInternet.h>

#include <console.h>

// =========================
// Descent 3 Headers
// =========================
#include "descent.h"
#include "gameos.h"
#include "pserror.h"
#include "mem.h"
#include "mono.h"
#include "ddio_mac.h"
#include "ddio.h"
#include "maccontroller.h"

#include "descent.h"
#include "gameos.h"
#include "texture.h"
#include "application.h"
#include "appdatabase.h"
#include "pserror.h"
#include "init.h"
#include "dedicated_server.h"
#include "args.h"
#include "game.h"

const char *English_strings[] = {"Descent 3 under Windows NT requires version 4.0 or greater of NT to run.",
                                 "Descent 3 requires  Windows 9x, NT 4.0 or greater to run.",
                                 "",
                                 "You must install DirectX through the Descent 3 Launcher before continuing.",
                                 "You must install at least Service Pack 3 to run Descent 3 under Windows NT 4.0.",
                                 "Failed to retrieve DirectX version.",
                                 "Descent 3 requires DirectX 3 or greater on this machine.",
                                 "This version of Windows NT doesn't have DirectX 3 or greater installed.",
                                 "Your processor and system must support Katmai to run this game."};

const char *French_strings[] = {
    "L'exTcution de Descent 3 sous Windows NT nTcessite la version 4.0 ou ultTrieure de NT.",
    "L'exTcution de Descent 3 nTcessite Windows 9x, NT 4.0 ou ultTrieur.",
    "",
    "Vous devez installer DirectX a partir de l'installateur Descent 3 avant de continuer.",
    "Vous devez installer au moins Service Pack 3 pour exTcuter Descent 3 sous Windows NT 4.0.",
    "+chec de la rTcupTration de DirectX.",
    "Descent 3 nTcessite DirectX 3 ou ultTrieur sur ce systFme.",
    "Cette version de Windows NT n'est pas munie de DirectX 3 ou ultTrieur.",
    "Votre processeur et votre systFme doivent prendre en charge Katmai pour exTcuter ce jeu."};

const char *German_strings[] = {
    "Descent3 unter Windows NT ben÷tigt fnr die Ausfnhrung die NT-Version 4.0 oder h÷her",
    "Descent 3 ben÷tigt fnr die Ausfnhrung Windows 9x, NT 4.0 oder h÷her.",
    "",
    "Sie mnssen DirectX nber den Descent 3 Starter installieren, bevor Sie fortsetzen.",
    "Sie mnssen mindestens Service Paket 3 installieren, um Descent 3 unter Windows NT 4.0 ausfnhren zu k÷nnen.",
    "Die DirectX Version konnte nicht abgerufen werden.",
    "Descent 3 ben÷tigt DirectX 3 oder h÷her auf diesem Computer.",
    "In dieser Windows NT Version ist DirectX 3 oder h÷her nicht installiert.",
    "Ihr Prozessor und System mu¯ Katmai unterstntzen, um dieses Spiel auszufnhren."};

const char *Italian_strings[] = {
    "Descent 3 per Windows NT richiede la versione NT 4.0 o superiore.",
    "Descent 3 funziona solo con Windows 9x, NT 4.0 o superiore.",
    "",
    "Prima di prosegure installare DirectX per mezzo di Descent 3 Launcher.",
    "Per utilizzare Descent 3 sotto Windows NT 4.0 occorre installare Service Pack 3 o sup.",
    "Versione di DirectX non trovata.",
    "Descent 3 richiede l'installazione di DirectX 3 o superiore.",
    "DirectX 3 o superiore non trovato in questa versione di Windows NT.",
    "Per questo gioco occorrono un processore e un sistema che supportino Katmai."};

const char *Spanish_strings[] = {"Descent 3 bajo Windows NT requiere version 4.0 o mejor para correr.",
                                 "Descent 3 requiere Windows 9x, NT 4.0 o mejor para correr.",
                                 "",
                                 "Debe instalar DirectX desde el lanzador de Descent 3 antes de continuar.",
                                 "Debe instalar por lo menos Service Pack 3 para correr Descent 3 bajo Windows NT 4.0.",
                                 "Falla en la detección de la versión de DirectX.",
                                 "Descent 3 requiere DirectX 3 o mejor en el ordenador.",
                                 "Esta versión de Windows NT no tiene DirectX 3 o mejor instalado.",
                                 "Vuestro procesador y ordenador deben soportar Katmai para correr este juego."};

//	---------------------------------------------------------------------------
//	Define our operating system specific extensions to the gameos system
//	---------------------------------------------------------------------------

class oeD3MacApp : public oeMacApplication {
  bool shutdown, final_shutdown;
  int old_screen_mode;

public:
  oeD3MacApp(const char *name, unsigned flags) : oeMacApplication(name, flags){};

  oeD3MacApp(void);

  virtual void os_init();
  virtual unsigned defer();

  virtual ~oeD3MacApp() { final_shutdown = true; };

  void run() { Descent3(); };
};

class oeD3MacDatabase : public oeMacAppDatabase {
public:
  oeD3MacDatabase() : oeMacAppDatabase() { Database = this; };
};

//	---------------------------------------------------------------------------
//	D3MacObject operating system specific initialization

oeD3MacApp::oeD3MacApp(void) {
  os_ConsoleInit();
  mprintf((0, "\n\n\n\n\n"));
  mprintf((0, "==========================\n"));
  mprintf((0, " A NEW D3 DAY HAS BEGUN!\n"));
  mprintf((0, "==========================\n\n\n\n"));

  mprintf((0, "Created D3 MacOS Object (Macintosh Toolbox Managers initialized).\n"));
}

void oeD3MacApp::os_init() { oeMacApplication::init(); }

unsigned oeD3MacApp::defer() {
  unsigned packet;

  packet = oeMacApplication::defer();

#ifdef DAJ_DEBUG
  MacKeyboardHandler();
  MacInSprocketHandler();
#endif
  return packet;
}

void D3End() {
  //	if (Descent) {
  //		delete Descent;
  //	}
}

void TestForExtensions(void) {
  if ((ProcPtr)ISpInit == (ProcPtr)kUnresolvedCFragSymbolAddress) {
    ::ShowCursor();
    ::Alert(150, NULL);
    ::ExitToShell();
  }
  //	NumVersion ISpVer = ISpGetVersion();

  if ((ProcPtr)DSpStartup == (ProcPtr)kUnresolvedCFragSymbolAddress) {
    ::ShowCursor();
    ::Alert(151, NULL);
    ::ExitToShell();
  }
  //	NumVersion DSpVer = DSpGetVersion();

  if ((ProcPtr)SSpListener_New == (ProcPtr)kUnresolvedCFragSymbolAddress) {
    ::ShowCursor();
    ::Alert(152, NULL);
    ::ExitToShell();
  }
  //	NumVersion SMVer = SndSoundManagerVersion();

#ifdef USE_OPENGL
  if ((ProcPtr)aglChoosePixelFormat == (ProcPtr)kUnresolvedCFragSymbolAddress) {
    ::ShowCursor();
    ::Alert(153, NULL);
    ::ExitToShell();
  }
#endif
#ifdef USE_GLIDE
  if ((ProcPtr)grGlideInit == (ProcPtr)kUnresolvedCFragSymbolAddress) {
    ::ShowCursor();
    ::Alert(154, NULL);
    ::ExitToShell();
  }
#endif
  if ((ProcPtr)InitOpenTransport == (ProcPtr)kUnresolvedCFragSymbolAddress) {
    ::ShowCursor();
    ::Alert(155, NULL);
    ::ExitToShell();
  }
  /*
          if((ProcPtr)OpenMovieFile == (ProcPtr)kUnresolvedCFragSymbolAddress) {
                  ::ShowCursor();
                  ::Alert(156, NULL);
                  ::ExitToShell();
          }
  */
}

//	---------------------------------------------------------------------------
//	MacMain
//		creates all the OS objects and then runs Descent 3.
//		this is all this function should do.
//	---------------------------------------------------------------------------
int sioux_inited = false;
int main(int argc, char *argv[]) {
  EventRecord event;
  int found;

  MaxApplZone();

  FlushEvents(everyEvent, 0);
  InitGraf(&qd.thePort);
  InitFonts();
  InitWindows();
  InitMenus();
  TEInit();
  InitDialogs(0);

  InitCursor();

  setup_sioux();
  mprintf((0, "debug started\n"));
  sioux_inited = true;

  TestForExtensions();

//	GetNextEvent(0, &event);	// brings mac app to the front
#if 0	
//#ifdef DAJ_DEBUG
	//DAJ spocket test
	GetOSEvent(everyEvent - diskMask, &event);
	if (event.modifiers & controlKey) {
		argc = ccommand(&argv);
		for(int i=1;i<argc; i++)
			GatherArgs(argv[i]);
	} 
	if (event.modifiers & optionKey)
	{
		inSprocket_Init();
		inSprocket_Configure();
	}
#endif
  atexit(D3End);

  interrupt_Init();

  mem_Init();

  oeD3MacApp *d3;
  oeD3MacDatabase dbase;

  if (Dedicated_server)
    d3 = new oeD3MacApp("Descent 3", OEAPP_CONSOLE);
  else
    d3 = new oeD3MacApp("Descent 3", OEAPP_FULLSCREEN);

  Descent = d3;

  // ¥ Initialize Metrowerks Code Profiler
  bool profilerInitted = false;
#ifdef USE_PROFILER
  if (!ProfilerInit(collectDetailed, PPCTimeBase, 1024, 50)) {
    mprintf((1, "Profiler Initialized.\n"));
    profilerInitted = true;
    ProfilerSetStatus(0);
  } else {
    mprintf((1, "Profiler not initialized\n"));
  }
#endif

  // ¥ Here we go!!!
  PreInitD3Systems();

  d3->init();
  d3->run();

// ¥ Close the Metrowerks Code Profiler
#ifdef USE_PROFILER
  if (profilerInitted) {
    mprintf((2, "Dumping profiler results and closing profiler\n"));
    ProfilerDump("\p::Z_D3_0");
    ProfilerTerm();
  }
#endif

  delete d3; // DAJ LEAKFIX

  interrupt_Close();
  FlushEvents(everyEvent, 0);

  return 0;
}
