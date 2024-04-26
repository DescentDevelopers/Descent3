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

#ifndef LEVELGOALEXTERNAL_H_
#define LEVELGOALEXTERNAL_H_

#define LF_AUTO_END_LEVEL 0x01
#define LF_ALL_PRIMARIES_DONE 0x02

// Level Item Types
#define LIT_TERRAIN_CELL 0
#define LIT_INTERNAL_ROOM 1
#define LIT_OBJECT 2
#define LIT_TRIGGER 3
#define LIT_ANY_MINE 4

// Level Item Operations
#define LO_SET_SPECIFIED 0
#define LO_GET_SPECIFIED 1
#define LO_CLEAR_SPECIFIED 2

// Level Goal Flags
#define LGF_BLANK1 0x00000001
#define LGF_SECONDARY_GOAL 0x00000002
#define LGF_ENABLED 0x00000004
#define LGF_COMPLETED 0x00000008
#define LGF_TELCOM_LISTS 0x00000010
#define LGF_GB_DOESNT_KNOW_LOC 0x00000020
#define LGF_NOT_LOC_BASED 0x00000040
#define LGF_FAILED 0x00000080
#define LGF_COMP_ACTIVATE 0x00000100
#define LGF_COMP_ENTER 0x00000200
#define LGF_COMP_DESTROY 0x00000400
#define LGF_COMP_PLAYER_WEAPON 0x00000800
#define LGF_COMP_PLAYER 0x00001000
#define LGF_COMP_DALLAS 0x00002000

#define LGF_COMP_MASK                                                                                                  \
  (LGF_COMP_ACTIVATE | LGF_COMP_ENTER | LGF_COMP_DESTROY | LGF_COMP_PLAYER_WEAPON | LGF_COMP_PLAYER | LGF_COMP_DALLAS)

#define LGF_COMP_TERRAIN_MASK (LGF_COMP_ENTER | LGF_COMP_DALLAS)
#define LGF_COMP_ROOM_MASK (LGF_COMP_ENTER | LGF_COMP_DALLAS)
#define LGF_COMP_OBJECT_MASK (LGF_COMP_DESTROY | LGF_COMP_PLAYER_WEAPON | LGF_COMP_PLAYER | LGF_COMP_DALLAS)
#define LGF_COMP_TRIGGER_MASK (LGF_COMP_ACTIVATE | LGF_COMP_DALLAS)
#define LGF_COMP_MINE_MASK (LGF_COMP_ENTER | LGF_COMP_DALLAS)

#define MAX_GOAL_ITEMS 12
#define MAX_LEVEL_GOALS 32

#define MAX_GOAL_LISTS 4

#endif
