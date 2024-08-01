#version 150 core

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

in vec4 vertex_color;
in vec2 vertex_uv0;
in vec2 vertex_uv1;

out vec4 out_color;

uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform int u_texture_enable;

void main()
{
    out_color = vertex_color
        // take advantage of the fact that we're multiplying to make vec4(1) represent a "disabled"
        // texture sample. a real sample will always have component values <= 1, bool is defined
        // to cast to either 0 or 1, so taking the max() of the sample and an _inverted_ enable
        // signal lets this ignore a texture w/o branching. we use a bitfield to save bandwidth.
        * max(texture(u_texture0, vertex_uv0), vec4(float(!bool((u_texture_enable >> 0) & 1))))
        * max(texture(u_texture1, vertex_uv1), vec4(float(!bool((u_texture_enable >> 1) & 1))));
}