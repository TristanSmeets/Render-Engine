#version 460 core
layout (location = 0) in vec3 positions;
layout (location = 1) in vec2 textureCoordinates;

out vec2 TextureCoordinates;

void main()
{
    TextureCoordinates = textureCoordinates;
    gl_Position = vec4(positions, 1.0f);
}
