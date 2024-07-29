#version 330 core

in vec4 vertex_color;
in vec2 vertex_uv0;
in vec2 vertex_uv1;

out vec4 out_color;

uniform sampler2D u_texture0;
uniform sampler2D u_texture1;

void main()
{
    out_color = vertex_color * texture(u_texture0, vertex_uv0) * texture(u_texture1, vertex_uv1);
}