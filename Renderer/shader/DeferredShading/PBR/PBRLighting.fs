#version 460 core
out vec4 FragmentColour;

struct Light
{
    vec3 Position;
    vec3 Colour;
    
    float Constant;
    float Linear;
    float Quadratic;
};

in vec2 UV;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gMetallicRoughnessAO;

//IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

//SSAO
uniform sampler2D ssao;

uniform float gammaCorrection;
uniform float exposure;

const int NumberOfLights = 10;
uniform Light lights[NumberOfLights];

uniform vec3 cameraPosition;
uniform vec3 nonMetallicReflectionColour;

const float PI = 3.14159265359f;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0f);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0f);
    float k = (r * r) / 8.0f;

    float nom   = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return nom / denom;
}


float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0f);
    float NdotL = max(dot(N, L), 0.0f);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0f - roughness), F0) - F0) * pow(1.0f - cosTheta, 5.0f);
}

void main()
{
    vec3 FragmentPosition = texture(gPosition, UV).rgb;
    vec3 Normal = texture(gNormal, UV).rgb;
    vec3 albedo = texture(gAlbedo, UV).rgb;
    float metallic = texture(gMetallicRoughnessAO, UV).r;
    float roughness = texture(gMetallicRoughnessAO, UV).g;
    float ao = texture(gMetallicRoughnessAO, UV).b;
    float ssao = texture(ssao, UV).r;

    vec3 viewDirection = normalize(cameraPosition - FragmentPosition);
    vec3 reflectionDirection = reflect(-viewDirection, Normal);

    vec3 F0 = nonMetallicReflectionColour;
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0f);
    for(int i = 0; i < NumberOfLights; ++i)
    {
        //Reflectance equation
        vec3 lightDirection = normalize(lights[i].Position - FragmentPosition);
        vec3 halfWayVector = normalize(viewDirection + lightDirection);
        float distance = length(lights[i].Position - FragmentPosition);
        float attenuation = 1.0f / (lights[i].Constant + lights[i].Linear * distance + lights[i].Quadratic * (distance * distance));
        vec3 radiance = lights[i].Colour * attenuation;

        //Cook-Torrance BRDF
        float normalDistributionFunction = DistributionGGX(Normal, halfWayVector, roughness);
        float geometry = GeometrySmith(Normal, viewDirection, lightDirection, roughness);
        vec3 fresnel = FresnelSchlick(max(dot(halfWayVector, viewDirection), 0.0f), F0);

        vec3 nominator = normalDistributionFunction * geometry * fresnel;
        float denominator = 4 * max(dot(Normal, viewDirection), 0.0f) * max(dot(Normal, lightDirection), 0.0f) + 0.001f;
        vec3 specular = nominator / denominator;

        vec3 kS = fresnel;
        vec3 kD = vec3(1.0f) - kS;
        kD *= (1.0f - metallic);

        float NdotL = max(dot(Normal, lightDirection), 0.0f);
        vec3 outgoingRadiance = (kD * albedo / PI + specular) * radiance * NdotL;
        Lo += outgoingRadiance;
    }

    vec3 Fresnel = FresnelSchlickRoughness(max(dot(Normal, viewDirection), 0.0f), F0, roughness);
    vec3 specularConstant = Fresnel;
    vec3 diffuseConstant = 1.0f - specularConstant;
    diffuseConstant *= 1.0f - metallic;

    vec3 irradiace = texture(irradianceMap, Normal).rgb;
    vec3 diffuse = irradiace * albedo;

    const float MaxReflectionLod = 4.0f;
    vec3 prefilterColour = textureLod(prefilterMap, reflectionDirection, roughness * MaxReflectionLod).rgb;
    vec2 brdf = texture(brdfLUT, vec2(max(dot(Normal, viewDirection), 0.0f), roughness)).rg;
    vec3 specular2 = prefilterColour * (Fresnel * brdf.x + brdf.y);

    vec3 ambient = (diffuseConstant * diffuse + specular2) * ao * ssao;
    vec3 colour = ambient + Lo;

    vec3 mapped = vec3(1.0f) - exp(-colour * exposure);
    //Gamma Correction
    mapped = pow(mapped, vec3(1.0f/ gammaCorrection));
    FragmentColour = vec4(mapped, 1.0f);
    // FragmentColour = vec4(vec3(ssao), 1.0f);
}