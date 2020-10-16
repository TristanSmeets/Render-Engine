#version 460 core

#define MaximumLights 10
const float PI = 3.14159265358979323846;

layout (location = 0) out vec4 FragColour;
layout (location = 1) out vec4 BloomColour;

in vec2 TextureCoordinates;

uniform struct gTextures
{
    sampler2D gPosition;
    sampler2D gNormal;
    sampler2D gAlbedo;
    sampler2D gMRAO;
} GBuffer;

uniform struct LightInfo
{
    vec4 Position;
    vec3 Intensity;
} Lights[MaximumLights];
uniform int NumberOfLights = 10;

vec3 schlickFresnel(float lDotH, vec3 f0)
{
    return f0 + ( 1 - f0) * pow(1.0 - lDotH, 5);
}

float geoSmith(float dotProduct, float roughness)
{
    float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
    float denom = dotProduct * (1 - k) + k;
    return 1.0 / denom;
}

float ggxDistribution(float nDotH, float roughness)
{
    float alpha2 = roughness * roughness * roughness * roughness;
    float d = (nDotH * nDotH) * (alpha2 - 1) + 1;
    return alpha2 / (PI * d * d);
}

vec3 microfacetModel(int lightIndex, vec3 position, vec3 n)
{
    vec3 diffuseBRDF = texture(GBuffer.gAlbedo, TextureCoordinates).rgb;
    vec3 MRAO = texture(GBuffer.gMRAO, TextureCoordinates).rgb;
    vec3 l = vec3(0.0);
    vec3 lightI = Lights[lightIndex].Intensity;
    if(Lights[lightIndex].Position.w == 0.0)
    {
        l = normalize(Lights[lightIndex].Position.xyz);
    }
    else
    {
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
    vec3 specBRDF = 0.25 * ggxDistribution(nDotH, MRAO.g) * schlickFresnel(lDotH, f0) * geoSmith(nDotV, MRAO.g);

    return (diffuseBRDF + PI * specBRDF) * lightI * nDotL;
}

void main()
{
    vec3 sum = vec3(0);
    vec3 position = texture(GBuffer.gPosition, TextureCoordinates).rgb;
    vec3 normal = texture(GBuffer.gNormal, TextureCoordinates).rgb;

    for(int i = 0; i < NumberOfLights; ++i)
    {
        sum += microfacetModel(i, position, normal);
    }
    FragColour = vec4(sum, 1.0);
    
    float brightness = dot(sum, vec3( 0.2126f, 0.7152f, 0.0722f)); //some lumen value. Humans see green as the brightest.
    if(brightness > 1.0f)
    {
        BloomColour = vec4(sum, 1.0f);
    }
    else
    {
        BloomColour = vec4(vec3(0.0f), 1.0f);
    }
}