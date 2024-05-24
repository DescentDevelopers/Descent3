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

#ifndef _LANCLIENT_HEADER
#define _LANCLIENT_HEADER
/*
 * $Logfile: /DescentIII/Main/lanclient/lanclient.h $
 * $Revision: 1.2 $
 * $Date: 2004/02/09 04:14:51 $
 * $Author: kevinb $
 *
 * LAN client DLL
 *
 * $Log: lanclient.h,v $
 * Revision 1.2  2004/02/09 04:14:51  kevinb
 * Added newlines to all headers to reduce number of warnings printed
 *
 * Made some small changes to get everything compiling.
 *
 * All Ready to merge the 1.5 tree.
 *
 * Revision 1.1.1.1  2000/04/18 00:00:38  icculus
 * initial checkin
 *
 *
 * 14    6/23/99 3:36p Samir
 * took out #define ubyte
 *
 * 13    3/02/99 5:50p Kevin
 * Ouch. Duplicate structures existed and were conflicting.
 *
 * 12    2/19/99 5:21p Kevin
 * Fixed some connection DLLs and a Direct Sound bug with threads.
 *
 * 11    9/15/98 12:42p Jason
 * got dedicated server actually working
 *
 * 10    8/19/98 11:50a Kevin
 * Got DirectPlay IPX working, and localized connection DLLs
 *
 * 9     8/07/98 12:39p Jeff
 * added "allowed ships" to multiplayer options
 *
 * 8     7/21/98 1:49p Kevin
 * IPX support and peer-peer option for multi
 *
 * 7     7/20/98 2:34p Kevin
 * Re-wrote the DLL wrapper, to allow for better expandability
 *
 * 5     6/24/98 6:40p Kevin
 * Added help to main dll menu
 *
 * 4     6/24/98 3:24p Kevin
 * Updated PXO screens with chat, etc.
 *
 * 3     6/18/98 4:49p Kevin
 * Updated multiplayer menus
 *
 * 2     6/01/98 10:10a Kevin
 * Added DLL connection interface and auto update DLL
 *
 * 1     5/28/98 12:14p Kevin
 *
 * 1     5/18/98 12:47p Kevin
 *
 * $NoKeywords: $
 */

#include "ui.h"

// #define ubyte uint8_t

#define TRACKER_MENU_W 256
#define TRACKER_MENU_H 256
#define TRACKER_MENU_X 320 - (TRACKER_MENU_W / 2)
#define TRACKER_MENU_Y 240 - (TRACKER_MENU_H / 2)

#define MAXTEXTITEMS 100
#define MAXNEWWINDOWS 5
#define MAXNEWGAMEWINDOWS 5
#define MAXUIBUTTONS 20
#define MAXUITEXTS 20
#define MAXEDITS 20
#define MAXLISTS 20

#include "con_dll.h"

int StartMultiplayerGameMenu();
void MultiplayerOptionsMenu();

namespace lanclient {

int LoginMasterTracker(void);
int MainMultiplayerMenu();
int SearchLANGameMenu();

void DoMTFrame(void);
void DoMTGameOver(void);
int MTVersionCheck(void);
void AutoLoginAndJoinGame(void);
} // namespace lanclient
#endif
