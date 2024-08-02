#version 330 core

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