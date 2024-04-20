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

	left_height=cp[vlb].sy-cp[vlt].sy;
	if (left_height==0)
		left_height=1;


	Left_x=cp[vlt].sx;
	Left_z=cp[vlt].z;
	Delta_left_z=(cp[vlb].z-cp[vlt].z)/left_height;
	Delta_left_x=(cp[vlb].sx-cp[vlt].sx)/left_height;
	next_break_left = cp[vlb].sy;
	
