// version must be prepended to this file

/*
* Descent 3
* Copyright (C) 2024 Descent Developers
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

in vec3 in_pos;
in vec4 in_color;
in vec2 in_uv0;
in vec2 in_uv1;

out vec4 vertex_color;
out vec2 vertex_uv0;
out vec2 vertex_uv1;
out vec4 vertex_modelview_pos;

uniform mat4 u_modelview;
uniform mat4 u_projection;

void main()
{
    vertex_modelview_pos = u_modelview * vec4(in_pos, 1);
    gl_Position = u_projection * vertex_modelview_pos;
    vertex_color = in_color;
    vertex_uv0 = in_uv0;
    vertex_uv1 = in_uv1;
}