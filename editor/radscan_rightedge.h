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

right_height = cp[vrb].sy - cp[vrt].sy;
if (right_height == 0)
  right_height = 1;

Delta_right_x = (cp[vrb].sx - cp[vrt].sx) / right_height;
Right_x = cp[vrt].sx;

Right_z = cp[vrt].z;
Delta_right_z = (cp[vrb].z - cp[vrt].z) / right_height;

next_break_right = cp[vrb].sy;
