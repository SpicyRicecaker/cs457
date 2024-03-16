#version 330 core

out vec4 FragColor;
in vec3 v_color;

uniform float amplitude;

void main()
{
    FragColor = amplitude * vec4(v_color, 1.);
}
