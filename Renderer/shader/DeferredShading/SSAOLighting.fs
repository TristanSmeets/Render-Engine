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

    float Linear;
    float Quadratic;
};

const int NumberOfLights = 32;
uniform Light lights[NumberOfLights];
uniform vec3 viewPosition;

void main()
{
    vec3 FragmentPosition = texture(gPosition, UV).rgb;
    vec3 Normal = texture(gNormal, UV).rgb;
    vec3 DiffuseTextureColour = texture(gAlbedoSpecular, UV).rgb;
    float SpecularTextureColour = texture(gAlbedoSpecular, UV).a;
    float AmbientOcclusion = texture(ssao, UV).r;

    vec3 ambient = vec3(0.3f * DiffuseTextureColour * AmbientOcclusion);
    vec3 lighting = ambient;
    vec3 viewDirection = normalize(-FragmentPosition);

    for(int i = 0; i < NumberOfLights; ++i)
    {
        //Diffuse colour
        vec3 lightDirection = normalize(lights[i].Position - FragmentPosition);
        vec3 diffuse = max(dot(Normal, lightDirection), 0.0f) * DiffuseTextureColour * lights[i].Color;
        //Specular Colour
        vec3 halfwayDirection = normalize(lightDirection + viewDirection);
        float specularPower = pow(max(dot(Normal, halfwayDirection), 0.0f), 16.0f);
        vec3 specular = lights[i].Color * specularPower * SpecularTextureColour;
        //Attenuation
        float distance = length(lights[i].Position - FragmentPosition);
        float attenuation = 1.0f / (1.0f + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
        diffuse *= attenuation;
        specular *= attenuation;
        lighting += diffuse + specular;
    }

    vec3 result = vec3(1.0f) - exp(-lighting * 1.0f);
    result = pow(result, vec3(1.0f/ 2.2f));

    FragmentColour = vec4(result, 1.0f);
}
