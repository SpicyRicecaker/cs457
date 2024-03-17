#version 460 core

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec3 v_N;
layout (location = 2) in vec3 v_ST;

uniform mat4 model, view, projection;

void main()
{
    gl_Position = projection * view * model * vec4(v_pos, 1.0);
}
