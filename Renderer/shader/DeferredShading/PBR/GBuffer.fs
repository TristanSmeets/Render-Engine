#version 460 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec3 gMetallicRoughnessAO;
layout (location = 4) out vec3 gViewNormal;

in vec3 FragmentPosition;
in vec3 ViewPosition;
in vec3 Normal;
in vec3 ViewNormal;
in vec2 UV;

struct Material
{
    sampler2D Albedo;
    sampler2D Normal;
    sampler2D Metallic;
    sampler2D Roughness;
    sampler2D AO;
};

uniform Material material;
uniform float roughness;

subroutine float RoughnessType(in float roughness);
subroutine uniform RoughnessType roughnessType;

subroutine(RoughnessType) float UsingRoughness(in float roughness)
{
    return clamp(roughness, 0.0f, 1.0f);
}

subroutine(RoughnessType) float UsingSmoothness(in float roughness)
{
    return 1.0f - clamp(roughness, 0.0f, 1.0f);
}

vec3 GetNormalFromMap(vec3 normal)
{
    vec3 tangentNormal = texture(material.Normal, UV).xyz * 2.0f - 1.0f;

    vec3 Q1 = dFdx(FragmentPosition);
    vec3 Q2 = dFdy(FragmentPosition);
    vec2 st1 = dFdx(UV);
    vec2 st2 = dFdy(UV);

    vec3 N = normalize(normal);
    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
};

void main()
{
    gPosition = ViewPosition;
    // gNormal = GetNormalFromMap(ViewNormal);
    // gPosition = FragmentPosition;
    gNormal = GetNormalFromMap(Normal);
    gAlbedo.rgb = texture(material.Albedo, UV).xyz;
    gMetallicRoughnessAO.r = texture(material.Metallic, UV).r;
    float outputRoughness = roughness + texture(material.Roughness, UV).r;
    gMetallicRoughnessAO.g = roughnessType(outputRoughness);
    gMetallicRoughnessAO.b = texture(material.AO, UV).r;
    gViewNormal = GetNormalFromMap(ViewNormal);
}
