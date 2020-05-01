#version 460 core
layout (location = 0) in vec3 positions;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 uvs;

out vec3 FragmentPosition;
out vec3 Normal;
out vec2 UV;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    FragmentPosition = vec3(model * vec4(positions, 1.0f));
    UV = uvs;
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalize(normalMatrix * normals);

    gl_Position = projection * view * model * vec4(positions, 1.0f);
}