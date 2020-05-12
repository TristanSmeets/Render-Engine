#version 460 core
out vec4 FragmentColour;

in vec2 UV;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpecular;
uniform sampler2D ssao;

struct Light{
    vec3 Position;
    vec3 Color;

    float Constant;
    float Linear;
    float Quadratic;
};

const int NumberOfLights = 10;
uniform Light lights[NumberOfLights];
uniform samplerCube shadowCubeMaps[NumberOfLights];
uniform float farPlane;
uniform vec3 viewPosition;

uniform float ambientStrength;
uniform float shininess;

uniform float exposure;
uniform float gammaCorrection;

vec3 gridSamplingDisk[20] = vec3[](
    vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
    vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
    vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
    vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
    vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculation(vec3 fragmentPosition, samplerCube shadowCubeMap, vec3 lightPosition)
{
    vec3 fragmentToLight = fragmentPosition - lightPosition;

    float currentDepth = length(fragmentToLight);

    float shadow = 0.0f;
    float bias = 0.1f;
    int samples = 20;

    float viewDistance = length(viewPosition - fragmentPosition);
    float diskRadius = (1.0f + (viewDistance / farPlane)) / 25.0f;

    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(shadowCubeMap, fragmentToLight + gridSamplingDisk[i] * diskRadius).r;
        
        closestDepth *= farPlane;
        if(currentDepth - bias > closestDepth)
        {
            shadow += 1.0f;
        }
    }
    shadow /= float(samples);
    return shadow;
}

void main()
{
    vec3 FragmentPosition = texture(gPosition, UV).rgb;
    vec3 Normal = normalize(texture(gNormal, UV).rgb);
    vec3 DiffuseTextureColour = texture(gAlbedoSpecular, UV).rgb;
    float SpecularTextureColour = texture(gAlbedoSpecular, UV).a;
    float AmbientOcclusion = texture(ssao, UV).r;

    vec3 ambient = vec3(ambientStrength * DiffuseTextureColour * AmbientOcclusion);
    vec3 lighting = vec3(0.0f);
    vec3 viewDirection = normalize(viewPosition - FragmentPosition);

    for(int i = 0; i < NumberOfLights; ++i)
    {
        //Diffuse colour
        vec3 lightDirection = normalize(lights[i].Position - FragmentPosition);
        float diffuseImpact = max(dot(Normal, lightDirection), 0.0f);
        vec3 diffuse = lights[i].Color * diffuseImpact * DiffuseTextureColour;
        //Specular Colour
        vec3 halfWayVector = normalize(lightDirection + viewDirection);
        float specularPower = pow(max(dot(viewDirection, halfWayVector), 0.0f), shininess);
        vec3 specular = lights[i].Color * specularPower * SpecularTextureColour;
        //Attenuation
        float distance = length(FragmentPosition - lights[i].Position);
        float attenuation = 1.0f / (lights[i].Constant + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
        diffuse *= attenuation;
        specular *= attenuation;

        float shadow = ShadowCalculation(FragmentPosition, shadowCubeMaps[i], lights[i].Position);
        vec3 lightColour = diffuse + specular;
        lightColour *= (1.0f - shadow);
        lighting += lightColour;
    }

    vec3 result = ambient + lighting;
    vec3 mapped = vec3(1.0f) - exp(-result * exposure);
    mapped = pow(mapped, vec3(1.0f / gammaCorrection));
    FragmentColour = vec4(mapped, 1.0f);
}
