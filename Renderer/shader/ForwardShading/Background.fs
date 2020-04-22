#version 460 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColour;

in vec3 WorldPos;

uniform samplerCube environmentMap;

void main()
{
    FragColor = texture(environmentMap, WorldPos);
    BrightColour = vec4(vec3(0.0f), 1.0f);
}
