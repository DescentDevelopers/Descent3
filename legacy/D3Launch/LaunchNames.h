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

 * $Logfile: /DescentIII/Main/D3Launch/LaunchNames.h $
 * $Revision: 1.1.1.1 $
 * $Date: 2003-08-26 03:56:51 $
 * $Author: kevinb $
 *
 * 
 *
 * $Log: not supported by cvs2svn $
 * 
 * 21    6/18/99 4:42p Nate
 * Added non-LaserLock ROW version
 * 
 * 20    6/09/99 2:46p Nate
 * Minor changes for different builds
 * 
 * 19    5/21/99 3:38p Nate
 * Added changes for Rest of World release (new update directory path)
 * 
 * 18    5/07/99 11:24a Nate
 * Added support for a launcher config file
 * 
 * 17    4/15/99 5:02p Nate
 * Added localization support for the motd.txt file
 * 
 * 16    4/15/99 12:03p Nate
 * Added "Descent 3 Demo 2" build type
 * 
 * 15    4/08/99 1:13p Nate
 * Added Pentium III detection
 * 
 * 14    3/29/99 3:14p Nate
 * Added OEM_GENERIC
 * 
 * 13    3/19/99 10:18a Nate
 * Added OEM_GENERIC compile type
 * 
 * 12    3/15/99 3:05p Nate
 * Added readme viewer info
 * 
 * 11    3/12/99 3:29p Nate
 * Added more multi-language support
 * 
 * 10    3/02/99 5:45p Nate
 * Lots of little changes/fixes
 * 
 * 9     2/26/99 12:50p Nate
 * Changed OEM_Voodoo3 names
 * 
 * 8     2/24/99 1:46p Nate
 * Added multi-language support
 * 
 * 7     2/17/99 2:23p Nate
 * Added some OEM changes
 * 
 * 6     11/06/98 3:17p Nate
 * Added help created files
 * 
 * 5     10/08/98 6:23p Nate
 * Fixed a few bugs.
 * 
 * 4     10/01/98 3:51p Nate
 * Changed some file names and paths.
 * 
 * 3     9/29/98 6:06p Nate
 * Added UPDATED_VERSION_FNAME
 * 
 * 2     8/24/98 7:06p Nate
 * Added new AutoUpdate features, and fixed display glitches
 * 
 * 1     8/19/98 10:47a Nate
 *
 * $NoKeywords: $
 */

#ifndef LAUNCHNAMES_H
#define LAUNCHNAMES_H

// Localization defines
#define LANGUAGE_ENGLISH	0
#define LANGUAGE_GERMAN		1
#define LANGUAGE_SPANISH	2
#define LANGUAGE_ITALIAN	3
#define LANGUAGE_FRENCH		4

// Launcher's satellite resource DLL's
#define FRENCH_RESOURCE_DLL	"language\\launcher\\D3Launch_FRN.dll"
#define GERMAN_RESOURCE_DLL	"language\\launcher\\D3Launch_GER.dll"
#define ITALIAN_RESOURCE_DLL	"language\\launcher\\D3Launch_ITN.dll"
#define SPANISH_RESOURCE_DLL	"language\\launcher\\D3Launch_SPN.dll"

// LauncherUpdater's satellite resource DLL's
#define LU_FRENCH_RES_DLL		"language\\lnchupdr\\lnchupdr_FRN.dll"
#define LU_GERMAN_RES_DLL		"language\\lnchupdr\\lnchupdr_GER.dll"
#define LU_ITALIAN_RES_DLL		"language\\lnchupdr\\lnchupdr_ITN.dll"
#define LU_SPANISH_RES_DLL		"language\\lnchupdr\\lnchupdr_SPN.dll"

// Launcher help file info
#define LANGUAGE_HELP_PATH		"language\\launcher\\hlp\\"

// Language identification extensions
#define FRENCH_EXT				"_FRN"
#define GERMAN_EXT				"_GER"
#define ITALIAN_EXT				"_ITN"
#define SPANISH_EXT				"_SPN"

// Launcher config file info
#define LANGUAGE_LAUNCHER_PATH	"language\\launcher\\"
#define LAUNCHER_CONFIG_FNAME		"d3launch.cfg"

// Readme file info
#define LANGUAGE_README_PATH	"language\\readmes\\"
#define README_CONFIG_FNAME	"d3rmv.cfg"

// The Mutex identification name for the D3 Launcher process
#define D3_LAUNCHER_MUTEX_NAME	"D3LauncherMutexName"

// The filename of .RTPatch DLL
#define PATCH_DLL_FNAME			"patchw32.dll"

// The CD volume names
#if defined(DEMO)
#define CD1_VOLUME_NAME		"D3DEMO_1"
#define CD2_VOLUME_NAME		"D3DEMO_2"
#elif defined(DEMO2)
#define CD1_VOLUME_NAME		"D3DEMO_1"
#define CD2_VOLUME_NAME		"D3DEMO_2"
#elif defined(OEM_GENERIC)
#define CD1_VOLUME_NAME		"D3OEM_1"
#define CD2_VOLUME_NAME		"D3OEM_2"
#elif defined(OEM_VOODOO3)
#define CD1_VOLUME_NAME		"D3OEM_1"
#define CD2_VOLUME_NAME		"D3OEM_2"
#elif defined(OEM_KATMAI)
#define CD1_VOLUME_NAME		"D3OEM_1"
#define CD2_VOLUME_NAME		"D3OEM_2"
#else
#define CD1_VOLUME_NAME		"D3_1"
#define CD2_VOLUME_NAME		"D3_2"
#endif

// The filename of the ReadmeViewer
#define README_VIEWER_FNAME	"ReadmeViewer.exe"

// The filename of the Launcher executable (and help files)
#if defined(DEMO)
#define LAUNCHER_FNAME			"Descent 3 Demo.exe"
#define HELP_HLP_FNAME			"Descent 3 Demo.hlp"
#define HELP_CNT_FNAME			"Descent 3 Demo.cnt"
#define HELP_GID_FNAME			"DESCENT 3 DEMO.GID"
#define HELP_FTS_FNAME			"DESCENT 3 DEMO.FTS"
#elif defined(DEMO2)
#define LAUNCHER_FNAME			"Descent 3 Demo 2.exe"
#define HELP_HLP_FNAME			"Descent 3 Demo 2.hlp"
#define HELP_CNT_FNAME			"Descent 3 Demo 2.cnt"
#define HELP_GID_FNAME			"DESCENT 3 DEMO 2.GID"
#define HELP_FTS_FNAME			"DESCENT 3 DEMO 2.FTS"
#elif defined(OEM_GENERIC)
#define LAUNCHER_FNAME			"Descent 3 SOL ASCENT.exe"
#define HELP_HLP_FNAME			"Descent 3 SOL ASCENT.hlp"
#define HELP_CNT_FNAME			"Descent 3 SOL ASCENT.cnt"
#define HELP_GID_FNAME			"DESCENT 3 SOL ASCENT.GID"
#define HELP_FTS_FNAME			"DESCENT 3 SOL ASCENT.FTS"
#elif (defined(OEM_VOODOO3) && !defined(USE_ALL_VIDEO_OPTIONS))
#define LAUNCHER_FNAME			"Descent 3 SOL ASCENT for Voodoo3.exe"
#define HELP_HLP_FNAME			"Descent 3 SOL ASCENT for Voodoo3.hlp"
#define HELP_CNT_FNAME			"Descent 3 SOL ASCENT for Voodoo3.cnt"
#define HELP_GID_FNAME			"DESCENT 3 SOL ASCENT FOR VOODOO3.GID"
#define HELP_FTS_FNAME			"DESCENT 3 SOL ASCENT FOR VOODOO3.FTS"
#elif (defined(OEM_VOODOO3) && defined(USE_ALL_VIDEO_OPTIONS))
#define LAUNCHER_FNAME			"Descent 3 SOL ASCENT.exe"
#define HELP_HLP_FNAME			"Descent 3 SOL ASCENT.hlp"
#define HELP_CNT_FNAME			"Descent 3 SOL ASCENT.cnt"
#define HELP_GID_FNAME			"DESCENT 3 SOL ASCENT.GID"
#define HELP_FTS_FNAME			"DESCENT 3 SOL ASCENT.FTS"
#elif defined(OEM_KATMAI)
#define LAUNCHER_FNAME			"Descent 3 SOL ASCENT for Pentium III.exe"
#define HELP_HLP_FNAME			"Descent 3 SOL ASCENT for Pentium III.hlp"
#define HELP_CNT_FNAME			"Descent 3 SOL ASCENT for Pentium III.cnt"
#define HELP_GID_FNAME			"DESCENT 3 SOL ASCENT FOR PENTIUM III.GID"
#define HELP_FTS_FNAME			"DESCENT 3 SOL ASCENT FOR PENTIUM III.FTS"
#else
#define LAUNCHER_FNAME			"Descent 3.exe"
#define HELP_HLP_FNAME			"Descent 3.hlp"
#define HELP_CNT_FNAME			"Descent 3.cnt"
#define HELP_GID_FNAME			"DESCENT 3.GID"
#define HELP_FTS_FNAME			"DESCENT 3.FTS"
#endif

// The filenames of the game executables
#define GAME_EXECUTABLE_FNAME		"main.exe"
#define KATMAI_EXECUTABLE_FNAME	"main_p3.exe"
#define GAME_EXECUTABLE_SWITCH	"-launched"

// The filename of the replacement Launcher executable
#define NEW_LAUNCHER_FNAME		"launcher.new"

// The filename of the replacement Launcher executable
#define LAUNCHER_PATCH_FNAME	"launcher.rtp"

// The filename of the Launcher replacement program
#define REPLACER_FNAME			"lnchupdr.exe"

// the local version, MOTD, sites, and patch filenames
#define VERSION_LOC_FNAME		"version.nfo"
#define PATCH_LOC_FNAME			"patch.rtp"
#define SITES_LOC_FNAME			"sites.nfo"
#define MOTD_LOC_FNAME			"motd.txt"

// the Updated Version text file name
#define UPDATED_VERSION_FNAME	"version.new"

// the AutoUpdate source file paths (URLs)
#if defined(DEMO)
#define MOTD_PATH		"http://www.outrage.com/update/descent3demo/motd.txt"
#define VERSION_PATH	"http://www.outrage.com/update/descent3demo/version.nfo"
#elif defined(DEMO2)
#define MOTD_PATH		"http://www.outrage.com/update/descent3demo2/motd.txt"
#define VERSION_PATH	"http://www.outrage.com/update/descent3demo2/version.nfo"
#elif defined(OEM_GENERIC)
#define MOTD_PATH		"http://www.outrage.com/update/descent3_oem/motd.txt"
#define VERSION_PATH	"http://www.outrage.com/update/descent3_oem/version.nfo"
#elif defined(OEM_VOODOO3)
#define MOTD_PATH		"http://www.outrage.com/update/descent3_oem_v3/motd.txt"
#define VERSION_PATH	"http://www.outrage.com/update/descent3_oem_v3/version.nfo"
#elif defined(OEM_KATMAI)
#define MOTD_PATH		"http://www.outrage.com/update/descent3_oem_km/motd.txt"
#define VERSION_PATH	"http://www.outrage.com/update/descent3_oem_km/version.nfo"
#elif (defined(FULL_ROW_RELEASE) && !defined(ROW_NLL_RELEASE))
#define MOTD_PATH		"http://www.outrage.com/update/descent3_row/motd.txt"
#define VERSION_PATH	"http://www.outrage.com/update/descent3_row/version.nfo"
#elif (defined(FULL_ROW_RELEASE) && defined(ROW_NLL_RELEASE))
#define MOTD_PATH		"http://www.outrage.com/update/descent3_row_nll/motd.txt"
#define VERSION_PATH	"http://www.outrage.com/update/descent3_row_nll/version.nfo"
#elif defined(FULL_AUSSIE_RELEASE)
#define MOTD_PATH		"http://www.outrage.com/update/descent3_aussie/motd.txt"
#define VERSION_PATH	"http://www.outrage.com/update/descent3_aussie/version.nfo"
#else
#define MOTD_PATH		"http://www.outrage.com/update/descent3/motd.txt"
#define VERSION_PATH	"http://www.outrage.com/update/descent3/version.nfo"
#endif

// Localized motd paths (for full release only)
#if (defined(FULL_ROW_RELEASE) && !defined(ROW_NLL_RELEASE))
#define FRN_MOTD_PATH		"http://www.outrage.com/update/descent3_row/motd_FRN.txt"
#define GER_MOTD_PATH		"http://www.outrage.com/update/descent3_row/motd_GER.txt"
#define ITN_MOTD_PATH		"http://www.outrage.com/update/descent3_row/motd_ITN.txt"
#define SPN_MOTD_PATH		"http://www.outrage.com/update/descent3_row/motd_SPN.txt"
#elif (defined(FULL_ROW_RELEASE) && defined(ROW_NLL_RELEASE))
#define FRN_MOTD_PATH		"http://www.outrage.com/update/descent3_row_nll/motd_FRN.txt"
#define GER_MOTD_PATH		"http://www.outrage.com/update/descent3_row_nll/motd_GER.txt"
#define ITN_MOTD_PATH		"http://www.outrage.com/update/descent3_row_nll/motd_ITN.txt"
#define SPN_MOTD_PATH		"http://www.outrage.com/update/descent3_row_nll/motd_SPN.txt"
#else
#define FRN_MOTD_PATH		"http://www.outrage.com/update/descent3/motd_FRN.txt"
#define GER_MOTD_PATH		"http://www.outrage.com/update/descent3/motd_GER.txt"
#define ITN_MOTD_PATH		"http://www.outrage.com/update/descent3/motd_ITN.txt"
#define SPN_MOTD_PATH		"http://www.outrage.com/update/descent3/motd_SPN.txt"
#endif

// the Uninstallation path
#if defined(DEMO)
#define UNINSTALL_PATH "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Descent3Demo"
#elif defined(DEMO2)
#define UNINSTALL_PATH "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Descent3Demo2"
#elif defined(OEM_GENERIC)
#define UNINSTALL_PATH "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Descent3_OEM"
#elif defined(OEM_VOODOO3)
#define UNINSTALL_PATH "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Descent3_OEM_V3"
#elif defined(OEM_KATMAI)
#define UNINSTALL_PATH "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Descent3_OEM_KM"
#else
#define UNINSTALL_PATH "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Descent3"
#endif

// Registry Value Names
#define REG_COMPANY_NAME	"Outrage"

#if defined(DEMO)
#define REG_APP_NAME			"Descent3Demo";
#elif defined(DEMO2)
#define REG_APP_NAME			"Descent3Demo2";
#elif defined(OEM_GENERIC)
#define REG_APP_NAME			"Descent3_OEM";
#elif defined(OEM_VOODOO3)
#define REG_APP_NAME			"Descent3_OEM_V3";
#elif defined(OEM_KATMAI)
#define REG_APP_NAME			"Descent3_OEM_KM";
#elif defined(GAMEGAUGE)
#define REG_APP_NAME			"Descent3_GG";
#else
#define REG_APP_NAME			"Descent3";
#endif


#endif /* LAUNCHNAMES_H */
