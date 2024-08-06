#version 330 core

layout (location = 0) in vec3 in_pos;
layout (location = 1) in vec4 in_color;
layout (location = 2) in vec2 in_uv0;
layout (location = 3) in vec2 in_uv1;

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