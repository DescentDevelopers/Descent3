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

 * $Source: $
 * $Revision: 9 $
 * $Author: Jeff $
 * $Date: 7/13/99 10:04a $
 *
 * D3 Events moved from D3OsiEvents.h
 *
 * $Log: /DescentIII/Main/lib/d3events.h $
 *
 * 9     7/13/99 10:04a Jeff
 * text taunt token decoding
 *
 * 8     5/10/99 2:42a Jeff
 * added event/function to get the team of a connecting player
 *
 * 7     5/08/99 4:30a Jeff
 * fixed sequencing bug where clients never got a level end event for the
 * multiplayer games
 *
 * 6     3/22/99 6:20p Jeff
 * added 2 more audio taunts.  a mulitplayer event when someone plays an
 * audio taunt.  option to disable audio taunts.
 *
 * 5     2/07/99 1:19a Jeff
 * added new multiplayer game events EVT_GAMEOBJKILLED and
 * EVT_GAMEOBJDESTROYED
 *
 * 4     1/18/99 12:04a Jeff
 * added some new events
 *
 * 3     1/15/99 8:29p Jeff
 * updates to powerball
 *
 * 2     1/15/99 3:57a Jeff
 * added 2 new events (for weapon collisions)
 *
 * 1     1/08/99 4:14p Samir
 *
 */

#ifndef D3EVENTS_H
#define D3EVENTS_H

// game events.

#define EVT_GAMEPLAYERKILLED 0x500   // called in game script when a player gets killed
#define EVT_GAMEPLAYEREXPLODED 0x501 // called in game script when a player explodes
#define EVT_GAMECOLLIDE 0x502 // called in game script when 2 object collide (one of the two must be player or robot)
#define EVT_GAMEPLAYERCHANGESEG                                                                                        \
  0x503 // called when a player changes rooms or 'large' terrain cell (8x8 terrain cell block)
#define EVT_GAMEPLAYERENTERSGAME 0x504     // called when a player enters the game
#define EVT_GAMEPLAYERDISCONNECT 0x505     // called when a player disconnects from the game
#define EVT_GAMECREATED 0x506              // called when a server creates a game
#define EVT_GAMELEVELCHANGE 0x507          // called when the server changes a level
#define EVT_GAMELEVELSTART 0x508           // called when the server starts a new level
#define EVT_GAMEOBJECTSHIELDSCHANGED 0x510 // called when an object's shields changes (higher or lower)
#define EVT_GAMECHECKBAN 0x511         // called to the dlls with a network address to determine if a player is banned
#define EVT_GAMEDOCONTROLS 0x512       // called to the multiplayer games so they can do any controller related movement
#define EVT_GAMEWALLCOLLIDE 0x513      // called when there is a collision between an object and a wall
#define EVT_GAMEOBJCHANGESEG 0x514     // called when an object (other than OBJ_PLAYER) changes room/segment
#define EVT_GAMEOBJDESTROYED 0x515     // called when an object is about to be deleted
#define EVT_GAMEOBJKILLED 0x516        // called when an object was killed by another object
#define EVT_GAME_GET_PLAYER_TEAM 0x517 // called when the game needs to get the team of a player

// Multiplayer client events
#define EVT_CLIENT_INTERVAL 0x600           // called every frame
#define EVT_CLIENT_AI 0x601                 // called every frame for AI info
#define EVT_CLIENT_DAMAGED 0x602            // called when object is damaged
#define EVT_CLIENT_COLLIDE 0x603            // called when object collides with something
#define EVT_CLIENT_CREATED 0x604            // called when object created
#define EVT_CLIENT_DESTROY 0x605            // called when object destroyed
#define EVT_CLIENT_TIMER 0x606              // called when a timer event is signalled
#define EVT_CLIENT_USE 0x607                // called when item is selected for use from inventory
#define EVT_CLIENT_GAMEPLAYERKILLED 0x608   // called in game script when a player gets killed
#define EVT_CLIENT_GAMEPLAYEREXPLODED 0x609 // called in game script when a player explodes
#define EVT_CLIENT_GAMECOLLIDE                                                                                         \
  0x60A // called in game script when 2 object collide (one of the two must be player or robot)
#define EVT_CLIENT_GAMEPLAYERCHANGESEG                                                                                 \
  0x60B // called when a player changes rooms or 'large' terrain cell (8x8 terrain cell block)
#define EVT_CLIENT_GAMEPLAYERENTERSGAME 0x60C     // called when a player enters the game
#define EVT_CLIENT_GAMEPLAYERDISCONNECT 0x60D     // called when a player disconnects from the game
#define EVT_CLIENT_GAMECREATED 0x60E              // called when a multiplayer game is created
#define EVT_CLIENT_GAMELEVELCHANGE 0x60F          // called when the server changes a level
#define EVT_CLIENT_GAMESPECIALPACKET 0x610        // called when a special packet has arrived
#define EVT_CLIENT_HUD_INTERVAL 0x611             // called once a frame when the hud is being drawn
#define EVT_CLIENT_KEYPRESS 0x612                 // called whenever a keypress is made
#define EVT_CLIENT_INPUT_STRING 0x613             // called when a player types in a message
#define EVT_CLIENT_GAMELEVELSTART 0x614           // called when the server starts a new level
#define EVT_CLIENT_GAMELEVELEND 0x615             // called when the server ends a level
#define EVT_CLIENT_GAMEPOSTLEVELRESULTS 0x616     // called when the dll needs to display the post level results
#define EVT_CLIENT_GAMEOBJECTSHIELDSCHANGED 0x617 // called when an object's shields changes (higher or lower)
#define EVT_CLIENT_LEVELSTART EVT_CLIENT_CREATED
#define EVT_CLIENT_LEVELEND EVT_CLIENT_DESTROY
#define EVT_CLIENT_CHANGESEG 0x618                // called when an object changes rooms.
#define EVT_CLIENT_SHOWUI 0x619                   // The game is allowing the client DLL to show UI windows
#define EVT_WEAPONFIRED 0x620                     // called whenever a weapon is fired
#define EVT_CLIENT_GAMEPLAYERENTERSOBSERVER 0x621 // A player is entering observer mode
#define EVT_CLIENT_GAMEPLAYEREXITSOBSERVER 0x622  // A player is exiting observer mode
#define EVT_CLIENT_GETCOLOREDNAME 0x623           // DLL is telling game what color to draw the names in
#define EVT_GAME_INTERVAL 0x624                   // The interval frame for a game
#define EVT_GAME_DISCONNECTED 0x625               // Event called if you disconnect from the server
#define EVT_CLIENT_GAMEWALLCOLLIDE 0x626          // called when there is a collision between an object and a wall
#define EVT_CLIENT_GAMEOBJCHANGESEG 0x627         // called when an object (other than OBJ_PLAYER) changes room/segment
#define EVT_CLIENT_GAMEOBJDESTROYED 0x628         // called when an object is about to be killed
#define EVT_CLIENT_GAMEOBJKILLED 0x629            // called when an object was killed by another object
#define EVT_CLIENT_PLAYERPLAYSAUDIOTAUNT 0x62A    // called when a player is playing an audio taunt
#define EVT_CLIENT_DECODETEXTMACRO 0x62B          // called when a player uses a text macro
#endif
