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

#include <DrawSprocket.h>

#define DSP_640x480 0
#define DSP_800x600 1
#define DSP_1024x768 2
#define DSP_1152x870 3
// #define DSP_1280x960	4

extern DSpContextAttributes gDSpContextAttributes[];
extern DSpContextReference gDSpContext[];
extern short current_context;
