#version 460 core
layout (location = 0) in vec3 positions;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 uvs;

out vec3 FragmentPosition;
out vec3 Normal;
out vec2 UV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 viewPosition = view * model * vec4(positions, 1.0f);
    FragmentPosition = viewPosition.xyz;
    UV = uvs;

    mat3 normalMatrix = transpose(inverse(mat3(view * model)));
    Normal = normalMatrix * normals;

    gl_Position = projection * viewPosition;
}