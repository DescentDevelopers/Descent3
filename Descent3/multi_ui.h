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

#ifndef MULTI_UI_H
#define MULTI_UI_H

// The first multiplayer menu that the user will see...all multiplayer stuff is
// reached from this menu
// Returns true if we're starting a multiplayer game
int MainMultiplayerMenu();
int SearchMasterTrackerGameMenu();
int LoginMasterTrackerGameMenu();
int AutoConnectPXO();
int AutoConnectLANIP();
int AutoConnectHeat();
void DoMultiAllowed(void);
extern int MultiDLLGameStarting;
void MultiDoConfigLoad(void);
void MultiDoConfigSave(void);
void MultiGameOptionsMenu(int alloptions);
int ReturnMultiplayerGameMenu(void);
int MultiLevelSelection(void);
bool DoPlayerMouselookCheck(unsigned int flags);
#endif