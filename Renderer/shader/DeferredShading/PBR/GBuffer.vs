#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

out vec3 FragmentPosition;
out vec3 Normal;
out vec2 UV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // vec4 worldPosition = view * model * vec4(position, 1.0f);
    // FragmentPosition = worldPosition.xyz;
    FragmentPosition = vec3(model * vec4(position, 1.0f));
    UV = uv;
    Normal = mat3(model) * normal;

    gl_Position = projection * view * vec4(FragmentPosition, 1.0f);
}