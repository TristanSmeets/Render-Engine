#version 460 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpecular;
layout (location = 3) out vec3 gViewPosition;
layout (location = 4) out vec3 gViewNormal;

in vec3 FragmentPosition;
in vec3 Normal;
in vec3 ViewPosition;
in vec3 ViewNormal;
in vec2 TextureCoordinates;

uniform sampler2D diffuse;
uniform sampler2D specular;

void main()
{
    // gPosition = FragmentPosition;
    gPosition = vec3(gl_FragCoord.z);
    gNormal = normalize(Normal);
    gViewPosition = ViewPosition;
    gViewNormal = ViewNormal;
    gAlbedoSpecular = vec4(texture(diffuse,TextureCoordinates).rgb, texture(specular, TextureCoordinates).r);
}