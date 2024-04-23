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

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "mono.h"
#include "descent.h"
#include "texture.h"
#include "application.h"
#include "appdatabase.h"
#include "pserror.h"
#include "args.h"
#include "init.h"
#include "dedicated_server.h"
#include "resource.h"

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

#if 0
const char *Polish_strings[] = {
	"Aby uruchomi‘ grˆ Descent 3 potrzebujesz Windows NT w wersji 4.0 lub nowszej.",
	"Descent 3 wymaga Windows 9x albo Windows NT w wersji 4.0 lub nowszej.",
	"",
	"Zanim uruchomisz grˆ, musisz zainstalowa‘ DirectX przy u¨yciu programu startowego.",
	"Musisz zainstalowa‘ co najmniej Service Pack 3, aby uruchomi‘ grˆ Descent 3 pod Windows NT 4.0.",
	"Nie uda3o siˆ odczyta‘ numeru wersji sterownik¢w DirectX.",
	"Descent 3 wymaga sterownik¢w DirectX 3 lub nowszych.",
	"Ta wersja Windows NT nie ma zainstalowanych sterownik¢w DirectX 3 lub nowszych.",
	"Tw¢j procesor musi obs3ugiwa‘ rozkazy Katmai, aby uruchomi‘ grˆ."
};
#endif

static int m_resource_language = 0;

#define VENDOR_INTEL 0
#define VENDOR_AMD 1
#define VENDOR_CYRIX 2
#define VENDOR_UMC 3
#define VENDOR_CENTAUR 4
#define VENDOR_NEXGEN 5
#define VENDOR_UNKNOWN 6

typedef struct {
  unsigned char family, model, mask;
  int CPUid_level;
  int vendor;
  unsigned int capability;
  char vendor_id[16];
} cpuinfo;

int no_debug_dialog = 0;

void getcpudata(cpuinfo *info);

//	---------------------------------------------------------------------------
//	Define our operating system specific extensions to the gameos system
//	---------------------------------------------------------------------------

class oeD3Win32App : public oeWin32Application {
  bool shutdown, final_shutdown;
  int old_screen_mode;
  HANDLE hAppMutex;

public:
  oeD3Win32App(unsigned flags, HInstance hinst) : oeWin32Application(PRODUCT_NAME, flags, hinst) {
    Descent = this;
    shutdown = false;
    final_shutdown = false;
    hAppMutex = CreateMutex(NULL, FALSE, "D3MainMutexName");
    if (!hAppMutex) {
      exit(1);
    }
    win32_SetResourceDLL(NULL);
  };

  virtual ~oeD3Win32App() {
    win32_SetResourceDLL(NULL);
    if (hAppMutex) {
      CloseHandle(hAppMutex);
      hAppMutex = NULL;
    }

    final_shutdown = true;
  };

  void run() { Descent3(); };

  //	returns 0 if we pass to default window handler.
  virtual int WndProc(HWnd hwnd, unsigned msg, unsigned wParam, long lParam) {
    if (final_shutdown) {
      return oeWin32Application::WndProc(hwnd, msg, wParam, lParam);
    }

    switch (msg) {
    case WM_ACTIVATEAPP: {
      if (wParam == false) {
        this->deactivate();

        if (!shutdown) {
          ShutdownD3();
          shutdown = true;
          ShowWindow((HWND)hwnd, SW_MINIMIZE);
        }
      } else {
        this->activate();

        if (shutdown) {
          ShowWindow((HWND)hwnd, SW_RESTORE);
          RestartD3();
          shutdown = false;
        }
      }
    } break;
    }

    return oeWin32Application::WndProc(hwnd, msg, wParam, lParam);
  }
};

class oeD3Win32Database : public oeWin32AppDatabase {
public:
  oeD3Win32Database();
};

//	---------------------------------------------------------------------------
//	D3WinDatabase operating system specific initialization

oeD3Win32Database::oeD3Win32Database() : oeWin32AppDatabase() {
  char path[_MAX_PATH];
  bool res;

  //	create descent III entry if it doesn't exit.

#if defined(EDITOR)
  lstrcat(m_Basepath, "\\D3Edit");
#elif defined(DEMO)
  lstrcat(m_Basepath, "\\Descent3Demo2");
#elif defined(OEM_V3)
  lstrcat(m_Basepath, "\\Descent3_OEM_V3");
#elif defined(OEM_AUREAL2)
  lstrcat(m_Basepath, "\\Descent3_OEM_A2");
#elif defined(OEM_KATMAI)
  lstrcat(m_Basepath, "\\Descent3_OEM_KM");
#elif defined(GAMEGAUGE)
  lstrcat(m_Basepath, "\\Descent3GG");
#elif defined(OEM)
  lstrcat(m_Basepath, "\\Descent3_OEM");
#else
  lstrcat(m_Basepath, "\\Descent3");
#endif

  res = lookup_record(m_Basepath);
  if (!res) {
    res = create_record(m_Basepath);
    if (!res) {
      Error("Failed to create registry key for %s", PRODUCT_NAME);
    }
  }

  // create version key.
  lstrcpy(path, m_Basepath);
  lstrcat(path, "\\Version");
  res = lookup_record(path);
  if (!res) {
    res = create_record(path);
    if (!res) {
      Error("Failed to create registry key for %s", PRODUCT_NAME);
    }
  }

#ifdef EDITOR // Maybe this code should be in the editor startup
  lstrcpy(path, m_Basepath);
  lstrcat(path, "\\editor");
  res = lookup_record(path);
  if (!res) {
    res = create_record(path);
    if (!res) {
      Error("Failed to create registry key for %s.", PRODUCT_NAME);
    }
  }
#endif

  res = lookup_record(m_Basepath);

  // Get net directory for manage system
  char netpath[_MAX_PATH];
  lstrcpy(netpath, "");
#ifndef EDITOR
  if (FindArg("-update")) // For the game-only build, require -update to update data
#endif
  // NOTE LINK TO ABOVE IF
  {
    char *netdir = getenv("D3_DIR");
    if (netdir)
      lstrcpy(netpath, netdir);
  }
  write("net directory", netpath, lstrlen(netpath) + 1);

  Database = this;
}

bool Win32JoystickCalibrate() {
  // sorry.
  PROCESS_INFORMATION pi;
  STARTUPINFO si;
  BOOL flag;
  // DWORD dwval;

  memset(&si, 0, sizeof(si));
  si.cb = sizeof(si);
  si.dwFlags = STARTF_USESHOWWINDOW;
  si.wShowWindow = SW_SHOW;

  flag = CreateProcess(NULL, "rundll32.exe shell32.dll,Control_RunDLL joy.cpl", NULL, NULL, FALSE,
                       NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
  if (!flag) {
    return false;
  } else {
    tWin32AppInfo appinfo;
    HWND hWnd;

    Descent->get_info(&appinfo);
    hWnd = (HWND)appinfo.hwnd;

    WaitForInputIdle(pi.hProcess, INFINITE);
    ShowWindow(hWnd, SW_MINIMIZE);
    Descent->delay(0.5f);

    while (WaitForSingleObject(pi.hProcess, 0) != WAIT_OBJECT_0) {
      extern void D3DeferHandler(bool is_active);
      D3DeferHandler(false);
    }

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    ShowWindow(hWnd, SW_MAXIMIZE);
    Descent->delay(0.5f);
  }

  return true;
}

#ifdef EDITOR
//	---------------------------------------------------------------------------
//	WinMainInitEditor
//		creates all the App objects for the editor.
//		this is all this function should do.
//	---------------------------------------------------------------------------

#pragma message("Compiling editor WinMain substitute function.")

void WinMainInitEditor(unsigned hwnd, unsigned hinst) {
  tWin32AppInfo appinfo;

  appinfo.hwnd = (HWnd)hwnd;
  appinfo.hinst = (HInstance)hinst;
  appinfo.flags = OEAPP_WINDOWED;

  Descent = new oeWin32Application(&appinfo);
  Database = new oeD3Win32Database;
}

#else

void D3End() {
  if (Descent) {
    delete Descent;
  }
}

// Localization defines
#define LANGUAGE_ENGLISH 0
#define LANGUAGE_GERMAN 1
#define LANGUAGE_SPANISH 2
#define LANGUAGE_ITALIAN 3
#define LANGUAGE_FRENCH 4

inline void MessageBoxStr(int id) {
  const char *txt = "Unknown string";

  id--;
  if (id >= 0 && id <= 8) {
    switch (m_resource_language) {
    case LANGUAGE_FRENCH:
      txt = French_strings[id];
      break;
    case LANGUAGE_GERMAN:
      txt = German_strings[id];
      break;
    case LANGUAGE_ITALIAN:
      txt = Italian_strings[id];
      break;
    case LANGUAGE_SPANISH:
      txt = Spanish_strings[id];
      break;
    default:
      txt = English_strings[id];
    }
  }

  //	win32_GetStringResource(id, str, sizeof(str));

  MessageBox(NULL, txt, "Outrage Error", MB_OK);
}

// Returns true if this machine can support the CPUID instruction
bool SupportsCPUID() {
  bool enabled = true;

  __try {

    _asm {
			pushad
			__emit 0x0f // CPUID
			__emit 0xa2 // CPUID
			popad
    }
  } __except (1) {
    enabled = false;
  }

  return enabled;
}

// Returns true if this machine can support katmai instructions
bool SupportsKatmai() {
  // Note: we don't do anything Katmai specific -- just always enable it.
  return true;
  /*
  cpuinfo info;
  getcpudata(&info);

  char buffer[584];
  sprintf(buffer,	"Vendor ID\t: %s\n"
                                  "CPU Family\t: %c\n"
                                  "Model\t\t: %d\n"
                                  "Is Katmai\t: %s (Really: %s)\n",
                                  info.vendor_id[0] ? info.vendor_id : "Unknown",
                                  info.family + '0',
                                  info.model,
                                  (info.capability&0x02000000)?"Yes":"No",
                                  ((info.capability&0x02000000)&&(info.vendor==VENDOR_INTEL) )?"Yes":"No");
  mprintf((0,buffer));
  return((info.capability&0x02000000)&&(info.vendor==VENDOR_INTEL) )?true:false;
  */
}

// DirectX check, NT 4.0, 5.0, Win95 check.
bool Win32SystemCheck(HINSTANCE hInst) {
  tWin32OS os;
  int major, minor, build;
  bool retval;

  os = oeWin32Application::version(&major, &minor, &build);

  if (os == WinNT) {
    if (major < 4) {
      MessageBoxStr(ID_TXT_WINNT);
      return false;
    }
  } else if (os != Win9x) {
    MessageBoxStr(ID_TXT_WIN32);
    return false;
  }

  //	perform DirectX system check.
  retval = false;

  // This function simply returns the interger version value of DirectX
  // installed on the user's system
  LONG lResult;
  HKEY hKey = NULL;
  int version_num = 0;

  lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\DirectX", NULL, KEY_QUERY_VALUE, &hKey);
  if (lResult == ERROR_SUCCESS) {
    char version[32];
    DWORD dwType, dwLen = 32;
    lResult = RegQueryValueEx(hKey, "Version", NULL, &dwType, (ubyte *)version, &dwLen);
    if (lResult == ERROR_SUCCESS) {
      version_num = atoi(strstr(version, ".") + 1);
    } else {
      int val;
      DWORD dwType, dwLen = 4;

      lResult = RegQueryValueEx(hKey, "InstalledVersion", NULL, &dwType, (ubyte *)&val, &dwLen);
      if (lResult == ERROR_SUCCESS) {
        version_num = val;
      }
    }
    RegCloseKey(hKey);
  } else {
    // if we don't have DSETUP.DLL and we are under Win95, then we're in trouble.
    if (os == Win9x) {
      MessageBoxStr(ID_TXT_DXVER);
      MessageBoxStr(ID_TXT_DX95);
      goto end_win32_check;
    }
  }

  // we should be either under NT 4 or greater, or 95
  if (version_num < 3) {
    if (os == WinNT) {
      if (major == 4) {
        MessageBoxStr(ID_TXT_DXNT4);
      } else {
        MessageBoxStr(ID_TXT_DXNTERR);
      }
      goto end_win32_check;
    } else {
      // Win95
      MessageBoxStr(ID_TXT_DX95);
      goto end_win32_check;
    }
  }

  if ((!SupportsKatmai()) || FindArg("-nopentium3")) {
    mprintf((0, "No Katmai detected.\n"));
    Katmai = false;
  } else {
    mprintf((0, "Katmai detected!\n"));
    Katmai = true;
  }

  retval = true;

end_win32_check:
  return retval;
}

void getcpudata(cpuinfo *info) {
  unsigned char family, model, mask;
  int level;
  unsigned int capability;
  unsigned int a, b, c;

  if (SupportsCPUID()) {
    _asm {
			pushad;

			xor eax,eax; // call CPUID with 0 -> return vendor ID

			__emit 0x0f; // CPUID
			__emit 0xa2; // CPUID

			mov level,eax; // save CPUID level
			mov a,ebx; // lo 4 chars
			mov b,edx; // next 4 chars
			mov c,ecx; // last 4 chars

			or eax,eax; // do we have processor info as well?
			je noinfo;

			mov eax,1; // Use the CPUID instruction to get CPU type
			__emit 0x0f; // CPUID
			__emit 0xa2; // CPUID

			mov cl,al; // save reg for future use
			and ah,15; // mask processor family
			mov family,ah;
			and al,240; // mask model
			shr al,4;
			mov model,al;
			and cl,15; // mask mask revision
			mov mask,cl;
			mov capability,edx;

		noinfo:
			popad;
    }

    // fill in the struct
    memset(info->vendor_id, 0, 16);
    memcpy(&info->vendor_id[0], &a, 4);
    memcpy(&info->vendor_id[4], &b, 4);
    memcpy(&info->vendor_id[8], &c, 4);
    info->vendor_id[15] = '\0';

    info->capability = capability;
    info->CPUid_level = level;
    info->family = family;
    info->mask = mask;
    info->model = model;

    if (!strcmp(info->vendor_id, "GenuineIntel"))
      info->vendor = VENDOR_INTEL;
    else if (!strcmp(info->vendor_id, "AuthenticAMD"))
      info->vendor = VENDOR_AMD;
    else if (!strcmp(info->vendor_id, "CyrixInstead"))
      info->vendor = VENDOR_CYRIX;
    else if (!strcmp(info->vendor_id, "UMC UMC UMC "))
      info->vendor = VENDOR_UMC;
    else if (!strcmp(info->vendor_id, "CentaurHauls"))
      info->vendor = VENDOR_CENTAUR;
    else if (!strcmp(info->vendor_id, "NexGenDriven"))
      info->vendor = VENDOR_NEXGEN;
    else {
      info->vendor = VENDOR_UNKNOWN;
      info->vendor_id[0] = '\0';
    }

  } else {
    info->capability = 0;
    info->CPUid_level = 0;
    info->family = 0;
    info->mask = 0;
    info->model = 0;
    info->vendor_id[0] = '\0';
    info->vendor = VENDOR_UNKNOWN;
  }
}

//	---------------------------------------------------------------------------
//	WinMain
//		creates all the OS objects and then runs Descent 3.
//		this is all this function should do.
//	---------------------------------------------------------------------------

// See below for real WinMain

int PASCAL HandledWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szCmdLine, int nCmdShow) {
  /*	initialize our OS Object.  This could be a game dependant OS object, or a default OS object.
          once we create it, if successful, we can start the game.
  */
  oeD3Win32App *d3;
  extern bool w32_mouseman_hack; // from winapp.cpp
  extern bool joy_chpro_hack;    // located in winjoy.cpp

  strupr(szCmdLine);
  GatherArgs(szCmdLine);

  // This must come AFTER the GatherArgs() call, because its constructer used FindArg()
  oeD3Win32Database dbase;

  no_debug_dialog = FindArg("-nocrashbox");

  // If this is a dedicated server, then start one!
  if (FindArg("-dedicated"))
    StartDedicatedServer();
#ifdef DEDICATED_ONLY
  else {
    MessageBox(NULL, "Error: -dedicated command line required", PRODUCT_NAME " Error", MB_OK);
    return 0;
  }

#endif

#ifndef GAMEGAUGE
  if (!FindArg("-launched") && !FindArg("-dedicated") && !FindArg("-timetest")) {
    MessageBox(NULL, "You cannot run this program directly.  Please run \"" PRODUCT_NAME ".exe\".",
               PRODUCT_NAME " Error", MB_OK);
    return 0; // pre init return
  }
#endif

  if (Dedicated_server) {
    d3 = new oeD3Win32App(OEAPP_CONSOLE, (HInstance)hInst);
  } else {
    unsigned int flags = OEAPP_FULLSCREEN;
#ifndef RELEASE // TODO: remove #ifndef when window mode is ready for primetime
    if (FindArg("-windowed")) {
      // switch to windowed mode instead
      flags = OEAPP_WINDOWED;
    }
#endif

    d3 = new oeD3Win32App(flags, (HInstance)hInst);
  }
  atexit(D3End);

  w32_mouseman_hack = false;
  joy_chpro_hack = false;

  if (FindArg("-mouseman")) {
    w32_mouseman_hack = true;
  }
  if (FindArg("-chpro")) {
    joy_chpro_hack = true;
  }

  // determine preinit language for resource strings
  int language = 0;
  dbase.read_int("LanguageType", &language);
  m_resource_language = language;

  if (!Win32SystemCheck(hInst))
    return 0;

  PreInitD3Systems();

  d3->init();
  d3->run();

  return 1;
}

// This is the winmain that tests for exceptions..
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szCmdLine, int nCmdShow) {

  int result = -1;

  __try {
    result = HandledWinMain(hInst, hPrevInst, szCmdLine, nCmdShow);
  } __except (RecordExceptionInfo(GetExceptionInformation(), "WinMain()")) {
  }
  return result;
}

#endif
