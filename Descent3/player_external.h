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
 * $Logfile: /DescentIII/main/player_external.h $
 * $Revision: 7 $
 * $Date: 5/23/99 3:06a $
 * $Author: Jason $
 *
 * Includes the flags, defines, etc. for player struct and functions (for DLL export)
 *
 * $Log: /DescentIII/main/player_external.h $
 *
 * 7     5/23/99 3:06a Jason
 * fixed bug with player rankings not being updated correctly
 *
 * 6     5/10/99 12:23a Chris
 * Fixed another hearing/seeing case.  :)  Buddy bot now is in the player
 * ship at respawn
 *
 * 5     4/24/99 6:45p Jeff
 * added functions for theif so he can steal things other than weapons
 *
 * 4     2/25/99 8:55p Jeff
 * Inventory supports level change persistant items.  Inventory supports
 * time-out objects.  Inventory Reset changed (takes a level of reset
 * now).  Quad lasers stay across level change (single player).  Guidebot
 * bug fixed (now back in ship on level start).  Quads time out when
 * spewed.  Invulnerability and cloak powerups no longer use game
 * event/callbacks, so they can be saved in game saves (moved to
 * MakePlayerInvulnerable and MakeObjectInvisible)
 *
 * 3     2/12/99 3:39p Jason
 * added client side interpolation
 *
 * 2     1/21/99 11:15p Jeff
 * pulled out some structs and defines from header files and moved them
 * into seperate header files so that multiplayer dlls don't require major
 * game headers, just those new headers.  Side effect is a shorter build
 * time.  Also cleaned up some header file #includes that weren't needed.
 * This affected polymodel.h, object.h, player.h, vecmat.h, room.h,
 * manage.h and multi.h
 *
 * $NoKeywords: $
 */

#ifndef __PLAYER_EXTERNAL_H_
#define __PLAYER_EXTERNAL_H_

#define N_PLAYER_GUNS 8

// Initial player stat values
#define INITIAL_ENERGY 100  // 100% energy to start
#define INITIAL_SHIELDS 100 // 100% shields to start

#define MAX_ENERGY 200 // go up to 200
#define MAX_SHIELDS 200

// Observer modes
#define OBSERVER_MODE_ROAM 0
#define OBSERVER_MODE_PIGGYBACK 1

// Values for special flags
#define PLAYER_FLAGS_INVULNERABLE 1          // Player is invincible
#define PLAYER_FLAGS_DYING 4                 // Is this player in the middle of dying?
#define PLAYER_FLAGS_DEAD 8                  //	The player is just sitting there dead.
#define PLAYER_FLAGS_UNUSED 16               // ????
#define PLAYER_FLAGS_UNUSED1 32              // ????
#define PLAYER_FLAGS_UNUSED2 64              // ????
#define PLAYER_FLAGS_UNUSED3 128             // ????
#define PLAYER_FLAGS_UNUSED4 256             // ????
#define PLAYER_FLAGS_UNUSED5 512             // ????
#define PLAYER_FLAGS_UNUSED6 1024            // ????
#define PLAYER_FLAGS_AFTERBURNER 4096        // Player has an afterburner
#define PLAYER_FLAGS_HEADLIGHT 8192          // Player has headlight boost
#define PLAYER_FLAGS_HEADLIGHT_STOLEN 16384  // is the headlight stolen?
#define PLAYER_FLAGS_AFTERBURN_ON 32768      // Player afterburner is engaged
#define PLAYER_FLAGS_CUSTOM_TEXTURE 65536    // Player has a custom texture
#define PLAYER_FLAGS_THRUSTED (1 << 17)      // Player has thrusted this frame
#define PLAYER_FLAGS_BULLSEYE (1 << 18)      // Bullseye reticle should light up
#define PLAYER_FLAGS_ZOOMED (1 << 19)        // Bullseye reticle should light up
#define PLAYER_FLAGS_REARVIEW (1 << 20)      // Play is using rearview
#define PLAYER_FLAGS_SEND_MOVEMENT (1 << 21) // We need to tell the server about our movement
#define PLAYER_FLAGS_PLAYSOUNDMSGFORINVULN                                                                             \
  (1 << 22) // A sound and hud message should be displayed when invulnerability wears off

// Variables for weapon_fire_flags
#define WFF_SPRAY 1   // a spray weapon is firing
#define WFF_ON_OFF 2  // an on/off weapon is firing
#define WFF_FIRED 128 // fired this frame

// Define the two player weapons
#define PW_PRIMARY 0
#define PW_SECONDARY 1

// These are used to mask out various controls, put in initially for the training system
#define PCBF_FORWARD 1
#define PCBF_REVERSE 2
#define PCBF_LEFT 4
#define PCBF_RIGHT 8
#define PCBF_UP 16
#define PCBF_DOWN 32
#define PCBF_PITCHUP 64
#define PCBF_PITCHDOWN 128
#define PCBF_HEADINGLEFT 256
#define PCBF_HEADINGRIGHT 512
#define PCBF_BANKLEFT 1024
#define PCBF_BANKRIGHT 2048
#define PCBF_PRIMARY 4096
#define PCBF_SECONDARY 8192
#define PCBF_AFTERBURNER 16384

#endif
