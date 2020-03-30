#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

struct Material
{
    sampler2D Albedo;
    sampler2D Normal;
    sampler2D Metallic;
    sampler2D Roughness;
    sampler2D AO;
};

//IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

//Material parameters
uniform Material material;

//Lights
uniform vec3 lightPosition;
uniform vec3 lightColour;

//Camera position
uniform vec3 cameraPos;

const float PI = 3.14159265359;

vec3 GetNormalFromMap();
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 FresnelSchlick(float cosTheta, vec3 F0);
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);

void main()
{
    vec3 albedo     = texture(material.Albedo, TexCoords).rgb;
    float metallic  = texture(material.Metallic, TexCoords).r;
    float roughness = texture(material.Roughness, TexCoords).r;
    float ao        = texture(material.AO, TexCoords).r;

    vec3 normal = GetNormalFromMap();
    vec3 viewDirection = normalize(cameraPos - WorldPos);
    vec3 reflectionDirection = reflect(-viewDirection, normal);

    //Calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
    //If it's a metal use the albedo color as F0 (metallic workflow) 
    vec3 F0 = vec3(0.04f);
    F0 = mix(F0, albedo, metallic);

    //reflectance equation
    vec3 lightDirection = normalize(lightPosition - WorldPos);
    vec3 H = normalize(viewDirection + lightDirection);
    float distance = length(lightPosition - WorldPos);
    float attenuation = 1.0f / (distance * distance);
    vec3 radiance = lightColour * attenuation;

    //Cook-Torrance BRDF
    float normalDistributionFunction = DistributionGGX(normal, H, roughness);
    float geometry = GeometrySmith(normal, viewDirection, lightDirection, roughness);
    vec3 fresnel = FresnelSchlick(max(dot(H, viewDirection), 0.0f), F0);

    vec3 nominator = normalDistributionFunction * geometry * fresnel;
    float denominator = 4 * max(dot(normal, viewDirection), 0.0f) * max(dot(normal, lightDirection), 0.0f) + 0.001f;
    vec3 specular = nominator / denominator;

    //kS = equal to Fresnel. (kS = Specular component)
    vec3 kS = fresnel;
    //For energy conservation, the diffuse and specular light can't
    //be above 1.0f (unless the surface emits light); to preserve this
    //relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0f) - kS;
    //Multiply kD by the inverse metalness such that only non-metals
    //have diffuse lighting, or a linear bled if partly metal
    //(pure metals have no diffuse light).
    kD *= 1.0f - metallic;
    
    //Scale light by NdotL
    float NdotL = max(dot(normal, lightDirection), 0.0f);

    //add to outgoing radiance Lo.
    //note that we already multiplied the BRDF by the Fresnel (kS)
    //so we won't multiply by kS again
    vec3 Lo = vec3(0.04f) + (kD * albedo / PI + specular) * radiance * NdotL; 

    vec3 specularConstant = FresnelSchlickRoughness(max(dot(normal, viewDirection), 0.0f), F0, roughness);
    vec3 diffuseConstant = 1.0f - specularConstant;
    diffuseConstant *= 1.0f - metallic;

    vec3 irradiance = texture(irradianceMap, normal).rgb;
    vec3 diffuse = irradiance * albedo;
    
    //Sample both the pre-filter map and the BRDF LUT and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0f;
    vec3 prefilteredColor = textureLod(prefilterMap, reflectionDirection, roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf = texture(brdfLUT, vec2(max(dot(normal,viewDirection), 0.0f), roughness)).rg;
    vec3 specular2 = prefilteredColor * (fresnel * brdf.x + brdf.y);
    
    vec3 ambient = (diffuseConstant * diffuse + specular2) * ao;

    //vec3 ambient = vec3(0.03f) * albedo * ao;
    vec3 color = ambient + Lo;

    //Skipping tonemapping and gamma correction.
    //Those should be done in screen shader.

    FragColor = vec4(color, 1.0f);
}

vec3 GetNormalFromMap()
{
    vec3 tangentNormal = texture(material.Normal, TexCoords).xyz * 2.0f - 1.0f;

    vec3 Q1 = dFdx(WorldPos);
    vec3 Q2 = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N = normalize(Normal);
    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

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