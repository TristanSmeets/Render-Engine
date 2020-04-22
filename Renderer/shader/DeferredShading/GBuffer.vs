#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textureCoordinates;

out vec3 FragmentPosition;
out vec3 Normal;
out vec2 TextureCoordinates;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPos = model * vec4(position, 1.0f);

    FragmentPosition = worldPos.xyz;
    TextureCoordinates = textureCoordinates;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalMatrix * normal;

    gl_Position = projection * view * worldPos;
}
