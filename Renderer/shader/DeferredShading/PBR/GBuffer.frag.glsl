#version 460 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec4 gMRAO;

in vec3 Position;
in vec3 Normal;
in vec2 TextureCoordinates;

uniform struct MaterialInfo
{
    sampler2D Albedo;
    sampler2D Normal;
    sampler2D MRAO;
} Material;

vec3 CalculateNormal()
{
    vec3 tangentNormal = texture(Material.Normal, UV).xyz * 2.0 - 1.0;

    vec3 Q1 = dFdx(Position);
    vec3 Q2 = dFdy(Position);
    vec2 st1 = dFdx(TextureCoordinates);
    vec2 st2 = dFdy(TextureCoordinates);

    vec3 N = normalize(Normal);
    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B = -normalize(cross(N,T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main()
{
    gPosition = Position;
    gNormal = CalculateNormal();
    gAlbedo.rgb = texture(Material.Albedo, TextureCoordinates).rgb;
    gMRAO.rgb = texture(Material.MRAO, TextureCoordinates).rgb;
}