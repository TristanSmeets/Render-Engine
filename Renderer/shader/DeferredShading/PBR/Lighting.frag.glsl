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

uniform struct LightTextures
{
    samplerCube Irradiance;
    samplerCube Prefilter;
    sampler2D BrdfLUT;
} IBL;

uniform sampler2D ssaoTexture;

uniform mat4 InverseView;

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

vec3 getEnvironmentLight(in vec3 position, in vec3 normal)
{
    vec3 v = normalize(-position);
    vec3 WSNormal = mat3(InverseView) * normal;
    vec3 MRAO = texture(GBuffer.gMRAO, TextureCoordinates).rgb;
    vec3 Albedo = texture(GBuffer.gAlbedo, TextureCoordinates).rgb;

    //schlickFresnel
    float roughness = MRAO.g;
    vec3 F0 = mix(vec3(0.04), Albedo, MRAO.r);
    //vec3 schlick = schlickFresnel(dot(normal, v), F0);

    float cosTheta = max(dot(normal, v), 0.0);
    vec3 roughnessFresnel = F0 + (max(vec3(1.0f - roughness), F0) - F0) * pow(1.0f - cosTheta, 5.0f);

    vec3 diffuseConstant = 1.0f - roughnessFresnel;
    diffuseConstant *= 1 - MRAO.r;

    //Irradiance
    vec3 irradiance = texture(IBL.Irradiance, WSNormal).rgb;
    vec3 diffuse = irradiance * texture(GBuffer.gAlbedo, TextureCoordinates).rgb;

    //Prefilter
    vec3 reflectionDir = reflect(-v, normal);
    reflectionDir = mat3(InverseView) * reflectionDir;

    const float MaxReflectionLod = 4.0;
    vec3 prefilter = textureLod(IBL.Prefilter, reflectionDir, roughness * MaxReflectionLod).rgb;
    
    vec2 brdf = texture(IBL.BrdfLUT, vec2(max(dot(normal, v),0.0), roughness)).rg;
    vec3 specular = prefilter * (roughnessFresnel * brdf.x + brdf.y);

    return diffuseConstant * diffuse + specular;
}

void main()
{
    vec3 sum = vec3(0);
    vec3 position = texture(GBuffer.gPosition, TextureCoordinates).rgb;
    vec3 normal = texture(GBuffer.gNormal, TextureCoordinates).rgb;
    float ssao = texture(ssaoTexture, TextureCoordinates).r;

    for(int i = 0; i < NumberOfLights; ++i)
    {
        sum += microfacetModel(i, position, normal);
    }
    sum = sum + (getEnvironmentLight(position, normal) * ssao);

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