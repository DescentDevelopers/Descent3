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

#ifndef __HOARD_AUX_H_
#define __HOARD_AUX_H_

struct tGameConfig {
  int min_hoard;
};

// game config send/receive
void SendGameConfig(int towho);

// Displays the Hoard game configuration dialog (Server)
extern void DisplayHoardConfigDialog(tGameConfig *);

#endif
