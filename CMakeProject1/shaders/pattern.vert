#version 460 core

// a means attribute
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_n;
layout (location = 2) in vec2 a_st;

out vec3 v_n;
out vec3 v_wc;
out vec2 v_st;

uniform mat4 model, view, projection;
uniform mat3 normal;

void main()
{
    gl_Position = projection * view * model * vec4(a_pos, 1.0);
    v_wc = (model * vec4(a_pos, 1.0)).xyz;

    v_n = normalize(normal * a_n);
    v_st = a_st;
}
