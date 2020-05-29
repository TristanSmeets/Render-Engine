#version 460 core
layout (location = 0) out vec4 FragmentColour;
layout (location = 1) out vec4 BrightColour;

in vec2 UV;
in vec2 ViewRay;

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
uniform float nearPlane;
uniform vec3 viewPosition;
uniform mat4 projection;

uniform float ambientStrength;
uniform float shininess;

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

float LinearizeDepth(float depth)
{
    float z = depth * 2.0f - 1.0f;
    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));
}

float CalculateViewZ(vec2 uv)
{
    float Depth = LinearizeDepth(texture(gPosition, uv).r) / farPlane;
    return projection[3][2] / (2 * Depth - 1 - projection[2][2]);
}

void main()
{
    // vec3 FragmentPosition = texture(gPosition, UV).rgb;
    float ViewZ = CalculateViewZ(UV);
    float ViewX = ViewRay.x * ViewZ;
    float ViewY = ViewRay.y * ViewZ;

    vec3 FragmentPosition = vec3(ViewX, ViewY, ViewZ);
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
    // FragmentColour = vec4(result, 1.0f);
    FragmentColour = vec4(FragmentPosition, 1.0f);

    float brightness = dot(result, vec3( 0.2126f, 0.7152f, 0.0722f));
    if(brightness > 1.0f)
    {
        BrightColour = vec4(result, 1.0f);
    }
    else
    {
        BrightColour = vec4(vec3(0.0f), 1.0f);
    }
}
