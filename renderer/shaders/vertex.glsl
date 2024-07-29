#version 330 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec4 in_color;
layout (location = 2) in vec2 in_uv0;
layout (location = 3) in vec2 in_uv1;

out vec4 vertex_color;
out vec2 vertex_uv0;
out vec2 vertex_uv1;

uniform mat4 u_transform;

void main()
{
    gl_Position = u_transform * vec4(in_pos, 1);
    vertex_color = in_color;
    vertex_uv0 = in_uv0;
    vertex_uv1 = in_uv1;
}