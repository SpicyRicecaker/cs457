#version 460 core

out vec4 FragColor;

uniform float amplitude;

void main()
{
    FragColor = amplitude * vec4(1., 1., 1., 1.);
}
