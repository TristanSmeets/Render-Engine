#version 460 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 UV;

out vec2 UVs;

void main()
{
    UVs = UV;
    gl_Position = vec4(Position, 1.0f);
}
