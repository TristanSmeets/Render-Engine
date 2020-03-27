#version 330 core
layout (location = 0) in vec3 position;

out vec3 WorldPos;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    WorldPos = position;
    mat4 rotationView = mat4(mat3(view));
    vec4 clipPosition = projection * rotationView * vec4(WorldPos, 1.0f);

    gl_Position = clipPosition.xyww;
}
