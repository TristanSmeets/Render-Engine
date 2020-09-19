#version 460 core

#define MaximumLights 10
const float PI = 3.14159265358979323846;

in vec3 Position;
in vec3 Normal;
in vec2 UV;

layout (location = 0) out vec4 FragColour;

uniform struct MaterialInfo {
    sampler2D Albedo;
    sampler2D Normal;
    sampler2D MRAO;
} Material;

uniform struct LightInfo {
    vec4 Position;
    vec3 Intensity;
} Lights[MaximumLights];
uniform int NumberOfLights = 10;

vec3 schlickFresnel(float lDotH, vec3 f0) {
    return f0 + ( 1 - f0) * pow(1.0 - lDotH, 5);
}

float geomSmith(float dotProduct, float roughness) {
    float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
    float denom = dotProduct * (1 - k) + k;
    return 1.0 / denom;
}

float ggxDistribution (float nDotH, float roughness) {
    float alpha2 = roughness * roughness * roughness * roughness;
    float d = (nDotH * nDotH) * (alpha2 - 1) + 1;
    return alpha2 / (PI * d * d);
}

vec3 microfacetModel( int lightIndex, vec3 position, vec3 n) {
    vec3 diffuseBRDF = texture(Material.Albedo, UV).rgb;
    vec3 MRAO = texture(Material.MRAO, UV).rgb;
    vec3 l = vec3(0.0);
    vec3 lightI = Lights[lightIndex].Intensity;
    if(Lights[lightIndex].Position.w == 0.0){
        l = normalize(Lights[lightIndex].Position.xyz);
    }
    else{
        l = Lights[lightIndex].Position.xyz - position;
        float dist = length(l);
        l = normalize(l);
        lightI /= (dist * dist);
    }

    vec3 v = normalize(-position);
    vec3 h = normalize(v + l);
    vec3 f0 = mix(vec3(0.04), diffuseBRDF, MRAO.r);

    float nDotH = dot(n, h);
    float lDotH = dot(l, h);
    float nDotL = max(dot(n, l), 0.0);
    float nDotV = dot(n, v);
    vec3 specBRDF = 0.25 * ggxDistribution(nDotH, MRAO.g) * schlickFresnel(lDotH, f0) * geomSmith(nDotV, MRAO.g);

    return (diffuseBRDF + PI * specBRDF) * lightI * nDotL;
}

vec3 CalculateNormal()
{
    vec3 tangentNormal = texture(Material.Normal, UV).xyz *2.0f - 1.0f;

    vec3 Q1 = dFdx(Position);
    vec3 Q2 = dFdy(Position);
    vec2 st1 = dFdx(UV);
    vec2 st2 = dFdy(UV);

    vec3 N = normalize(Normal);
    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B = -normalize(cross(N,T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main()
{
    vec3 sum = vec3(0);
    vec3 normal = CalculateNormal();
    for(int i = 0; i < NumberOfLights; ++i)
    {
        sum += microfacetModel(i, Position, normal);
    }
    FragColour = vec4(sum, 1.0);
}