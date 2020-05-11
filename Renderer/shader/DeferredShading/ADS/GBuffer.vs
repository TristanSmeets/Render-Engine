#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textureCoordinates;

out vec3 FragmentPosition;
out vec3 Normal;
out vec3 ViewPosition;
out vec2 TextureCoordinates;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 viewPosition = view * model * vec4(position, 1.0f);

    ViewPosition = viewPosition.xyz;

    FragmentPosition = vec3(model * vec4(position, 1.0f));
    TextureCoordinates = textureCoordinates;

    mat3 normalMatrix = transpose(inverse(mat3(view * model)));
    // mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalize(normalMatrix * normal);

    gl_Position = projection * viewPosition;
}
