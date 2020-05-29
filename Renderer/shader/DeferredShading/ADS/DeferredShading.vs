#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

out vec2 UV;
out vec2 ViewRay;

uniform float aspectRatio;
uniform float tanHalfFOV;

void main()
{
    UV = uv;
    ViewRay.x = position.x * aspectRatio * tanHalfFOV;
    ViewRay.y = position.y * tanHalfFOV;
    gl_Position = vec4(position, 1.0f);
}