#version 460 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 textureCoordinates;

out vec3 Position;
out vec3 Normal;
out vec2 TextureCoordinates;

uniform struct Matrices {
    mat4 ModelView;
    mat4 Projection;
    mat4 MVP;
    mat3 Normal;
} Matrix;

void main()
{
    Normal = normalize(Matrix.Normal * vertexNormal);
    Position = (Matrix.ModelView * vec4(vertexPosition, 1.0)).xyz;
    TextureCoordinates = textureCoordinates;

    gl_Position = Matrix.mvp * vec4(vertexPosition, 1.0f);
}