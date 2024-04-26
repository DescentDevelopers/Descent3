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

#ifndef ROBOTFIRESTRUCT_EXTERNAL_H_
#define ROBOTFIRESTRUCT_EXTERNAL_H_

#define MAX_WB_GUNPOINTS 8    // Maximum gun points for a gun battery   // Limitted by interface in Editor
#define MAX_WB_FIRING_MASKS 8 // Maximum firing masks (firing sequence) // Limitted by interface in Editor
#define MAX_WB_TURRETS 8      // Maximum number of turret// Arbitary

// Object type flags
#define WBF_SPRAY 1
#define WBF_ANIM_LOCAL 2
#define WBF_ANIM_FULL 4
#define WBF_ANIM_MASKS 6
#define WBF_RANDOM_FIRE_ORDER 8
#define WBF_GUIDED 16
#define WBF_USE_CUSTOM_FOV 32
#define WBF_ON_OFF 64
#define WBF_USE_CUSTOM_MAX_DIST 128
#define WBF_USER_TIMEOUT 256
#define WBF_FIRE_FVEC 512
#define WBF_AIM_FVEC 1024
#define WBF_FIRE_TARGET 2048

#define MAX_WB_UPGRADES 5

#define DWBF_ENABLED 1
#define DWBF_AUTOMATIC 2
#define DWBF_ANIMATING 4
#define DWBF_ANIM_FIRED 8
#define DWBF_QUAD 16
#define DWBF_UPGRADED 32

#endif
