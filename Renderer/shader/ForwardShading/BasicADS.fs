#version 460 core
layout (location = 0) out vec4 FragmentColour;
layout (location = 1) out vec4 BrightColour;

in vec3 FragmentPosition;
in vec3 Normal;
in vec2 UV;

struct Light
{
    vec3 Position;
    vec3 Colour;

    //Attenuation
    float Constant;
    float Linear;
    float Quadratic;
};

struct Material
{
    sampler2D Diffuse;
    sampler2D Specular;

    float AmbientStrength;
    float Shininess;
};

#define MaximumLights 10
uniform Light lights[MaximumLights];
uniform Material material;
uniform vec3 viewPosition;
uniform samplerCube shadowCubeMaps[MaximumLights];
uniform float farPlane;

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
    vec3 diffuseColour = texture(material.Diffuse, UV).rgb;
    vec3 specularColour = texture(material.Specular, UV).rgb;
    vec3 normal = normalize(Normal);

    vec3 ambient = material.AmbientStrength * diffuseColour;
    vec3 lighting = vec3(0.0f);
    vec3 viewDirection = normalize(viewPosition - FragmentPosition);
    
    for(int i = 0; i < MaximumLights; ++i)
    {
        //Diffuse
        vec3 lightDirection = normalize(lights[i].Position - FragmentPosition);
        float diffuseImpact = max(dot(normal, lightDirection), 0.0f);
        vec3 diffuse = lights[i].Colour * diffuseImpact * diffuseColour;
        //Specular
        vec3 reflectDirection = reflect(-lightDirection, normal);
        float specularImpact = pow(max(dot(viewDirection, reflectDirection), 0.0f), material.Shininess);
        vec3 specular = lights[i].Colour * specularImpact * specularColour;
        //Attenuation
        float distance = length(FragmentPosition - lights[i].Position);
        float attenuation = 1.0f / (lights[i].Constant + lights[i].Linear * distance + lights[i].Quadratic * (distance * distance));
        diffuse *= attenuation;
        specular *= attenuation;

        //Shadow
        float shadow = ShadowCalculation(FragmentPosition, shadowCubeMaps[i], lights[i].Position);

        vec3 lightColour = diffuse + specular; 
        lightColour *= (1.0f - shadow);
        lighting += lightColour;
    }

    // vec3 result = ambient + lighting;
    vec3 result = lighting;
    // FragmentColour = vec4(result, 1.0f);
    FragmentColour = vec4(normal, 1.0f);

    float brightness = dot(result, vec3(0.2126f, 0.7152f, 0.0722f));
    if(brightness > 1.0f)
    {
        BrightColour = vec4(result, 1.0f);
    }
    else
    {
        BrightColour = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
}