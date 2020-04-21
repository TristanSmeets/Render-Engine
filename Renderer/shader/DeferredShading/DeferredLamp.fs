#version 460 core
layout (location = 0) out vec4 FragmentColour;

uniform vec3 lightColour;

void main()
{
    FragmentColour = vec4(lightColour, 1.0f);
}