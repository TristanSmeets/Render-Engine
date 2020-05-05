#version 460 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;

in vec3 FragmentPosition;
in vec3 Normal;
in vec2 uvs;

void main()
{
    gPosition = FragmentPosition;
    gNormal = normalize(Normal);
    gAlbedo.rgb = vec3(0.95f);
}