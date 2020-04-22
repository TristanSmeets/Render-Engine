#version 460 core
out vec4 FragmentColour;

in vec2 UV;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpecular;

struct Light{
    vec3 Position;
    vec3 Color;

    float Linear;
    float Quadratic;
    float Radius;
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

    vec3 lighting = DiffuseTextureColour * 0.1f; //Hard-coded ambient component;
    vec3 viewDirection = normalize(viewPosition - FragmentPosition);

    for(int i = 0; i < NumberOfLights; ++i)
    {
        float distance = length(lights[i].Position - FragmentPosition);
        if(distance < lights[i].Radius)
        {
            //Diffuse colour
            vec3 lightDirection = normalize(lights[i].Position - FragmentPosition);
            vec3 diffuse = max(dot(Normal, lightDirection), 0.0f) * DiffuseTextureColour * lights[i].Color;
            //Specular Colour
            vec3 halfwayDirection = normalize(lightDirection + viewDirection);
            float specularPower = pow(max(dot(Normal, halfwayDirection), 0.0f), 16.0f);
            vec3 specular = lights[i].Color * specularPower * SpecularTextureColour;
            //Attenuation
            float attenuation = 1.0f / (1.0f + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
            diffuse *= attenuation;
            specular *= attenuation;
            lighting += diffuse + specular;
        }
    }

    vec3 result = vec3(1.0f) - exp(-lighting * 1.0f);
    result = pow(result, vec3(1.0f/ 2.2f));

    FragmentColour = vec4(result, 1.0f);
    //FragmentColour = texture(gPosition, UV);
    
}
