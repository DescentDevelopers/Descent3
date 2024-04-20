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

#ifndef MATCEN_EXTERNAL_H_
#define MATCEN_EXTERNAL_H_

#define MAX_PROD_TYPES 8
#define MAX_SPAWN_PNTS 4

#define MATCEN_ERROR -1

#define MSTAT_DISABLED 1
#define MSTAT_ACTIVE 2
#define MSTAT_ACTIVE_PAUSE 4
#define MSTAT_CREATE_OBJ_FRAME 8
#define MSTAT_NEVER_PROD 16
#define MSTAT_DONE_PROD 32
#define MSTAT_RANDOM_PROD_ORDER 64
#define MSTAT_PROD_TILL_DONE 128
#define MSTAT_PROD_ONE_PAUSE 256
#define MSTAT_PROD_ONE_DISABLE 512
#define MSTAT_MANUAL_UPDATE_CREATE_PNT 1024
#define MSTAT_COMPUTE_CREATE_PNT_EVERY_FRAME 2048
#define MSTAT_NOT_HURT_PLAYER 4096

#define MMODE_NOTPROD 0
#define MMODE_PREPROD 1
#define MMODE_POSTPROD 2

// MATCEN NOTE:  Make sure to add the name of the effect to the list
// in matcen.cpp
#define NUM_MATCEN_EFFECTS 4
#define MEFFECT_LINE_LIGHTNING 0
#define MEFFECT_LINE_SINE_WAVE 1
#define MEFFECT_PROCEDURAL_LIGHTNING 2
#define MEFFECT_NONE

#define MAX_MATCEN_CONTROL_TYPES 5
#define MPC_SCRIPT 0
#define MPC_WHILE_PLAYER_NEAR 1
#define MPC_AFTER_PLAYER_NEAR 2
#define MPC_WHILE_PLAYER_VISIBLE 3
#define MPC_AFTER_PLAYER_VISIBLE 4

#define MAX_MATCEN_SOUNDS 3
#define MATCEN_ACTIVE_SOUND 0
#define MATCEN_DISABLE_SOUND 1
#define MATCEN_PROD_SOUND 2

#define MT_OBJECT 0
#define MT_ROOM 1
#define MT_UNASSIGNED 2

#endif