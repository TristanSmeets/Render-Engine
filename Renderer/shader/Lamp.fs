#version 330 core
out vec4 FragColor;

in vec3 Normal;

uniform vec3 lightColour; 

void main()
{
    vec3 normal = normalize(Normal);
    FragColor = vec4(normal, 1.0f);
    //FragColor = vec4(lightColour, 1.0f);
}
