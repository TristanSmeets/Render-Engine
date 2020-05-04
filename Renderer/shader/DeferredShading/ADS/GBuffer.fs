#version 460 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpecular;

in vec3 FragmentPosition;
in vec3 Normal;
in vec2 TextureCoordinates;

uniform sampler2D diffuse;
uniform sampler2D specular;

void main()
{
    gPosition = FragmentPosition;
    gNormal = Normal;
    gAlbedoSpecular.rbg = texture(diffuse, TextureCoordinates).rgb;
    gAlbedoSpecular.a = texture(specular, TextureCoordinates).r;
}