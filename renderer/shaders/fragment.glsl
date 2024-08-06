#version 330 core

in vec4 vertex_color;
in vec2 vertex_uv0;
in vec2 vertex_uv1;
in vec4 vertex_modelview_pos;

out vec4 out_color;

uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform int u_texture_enable;
uniform bool u_fog_enable;
uniform vec4 u_fog_color;
uniform float u_fog_start;
uniform float u_fog_end;

float branchless_invert_or_zero(in float value) {
    // sign() returns 1 if val > 0, -1 if val < 0, and 0 if val == 0
    float sign_squared = sign(value) * sign(value);
    // so this will either be 1/value, or 0/-1
    return sign_squared / (value + sign_squared - 1.0);
}

void main()
{
    out_color = vertex_color
        // take advantage of the fact that we're multiplying to make vec4(1) represent a "disabled"
        // texture sample. a real sample will always have component values <= 1, bool is defined
        // to cast to either 0 or 1, so taking the max() of the sample and an _inverted_ enable
        // signal lets this ignore a texture w/o branching. we use a bitfield to save bandwidth.
        * max(texture(u_texture0, vertex_uv0), vec4(float(!bool((u_texture_enable >> 0) & 1))))
        * max(texture(u_texture1, vertex_uv1), vec4(float(!bool((u_texture_enable >> 1) & 1))));

    float fog_factor = clamp((u_fog_end - length(vertex_modelview_pos)) * branchless_invert_or_zero(u_fog_end - u_fog_start), 0, 1);
    // out_color is unchanged when fog_factor is 1 (ie, fog distance == u_fog_start). thus, to disable,
    // fog_factor must also be 1. invert u_fog_enable (so that it is 1 when disabled) and take the max.
    fog_factor = max(fog_factor, float(!u_fog_enable));
    out_color = out_color * fog_factor + (1 - fog_factor) * u_fog_color;
}