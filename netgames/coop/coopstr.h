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

// Defines for indexes into the string table for in-code strings

#ifndef __STRING_TABLE____
#define __STRING_TABLE____

#define TXT(index) GetStringFromTable(index)

// Returns a pointer to the string at the index location from the string table
// if it is a bad index given, then the pointer to the error string "ERROR MISSING STRING" is given

const char *GetStringFromTable(int index);

#define TXT_COOP TXT(0)          //"Coop"
#define TXT_STATGAMENAME TXT(1)  //"Co-op Descent 3"
#define TXT_PILOT TXT(2)         //"Pilot"
#define TXT_KILLS TXT(3)         //"Kills"
#define TXT_DEATHS TXT(4)        //"Deaths"
#define TXT_SUICIDES TXT(5)      //"Suicides"
#define TXT_PING TXT(6)          //"Ping"
#define TXT_WELCOME TXT(7)       //"Welcome to the Co-Op Game %s"
#define TXT_JOINED TXT(8)        //"%s has joined the game"
#define TXT_LEVELCOMPLETE TXT(9) //"Level Completed!"
#endif
