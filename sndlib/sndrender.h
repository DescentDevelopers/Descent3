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
 * $Source: $
 * $Revision: 3 $
 * $Author: Samir $
 * $Date: 8/11/99 3:12p $
 *
 *	Sound renderer system
 *
 * $Log: /DescentIII/Main/sndlib/sndrender.h $
 *
 * 3     8/11/99 3:12p Samir
 * fixes for aureal support.
 *
 * 2     4/01/99 4:28p Samir
 * hardware geometry integration if it's available.
 *
 * 1     3/28/99 3:25p Samir
 *
 */

#ifndef SNDRENDER_H
#define SNDRENDER_H

#define SOUND_RENDER_RADIUS 120.0f
#define SOUND_RENDER_ROOM_LIMIT 16

//	resets sound geometry system (called beginning of each level)
void SoundRenderReset();

// if this fails, renderer won't work, do emulation.
bool sound_render_start_frame();

// ends sound rendering
void sound_render_end_frame();

// render all rooms within a certain radius from player
// the radius is actually scaled according to orientation.  radius is maximum radius, really, which is forward
//	minimum radius will be rear view.
// returns list of rooms rendered (-1 = end of list!)
short *sound_render_audible_rooms(pos_state *listener_pos, float max_radius = SOUND_RENDER_RADIUS);

// render one room's geometry
void sound_render_room_geometry(int roomnum, int slot);

// check if a room exists in room list.
inline bool sound_render_room_in_list(short room, short *list) {
  sbyte idx = 0;
  while (list[idx] != -1) {
    if (room == list[idx]) {
      return true;
    }
    idx++;
  }
  return false;
}

#endif
