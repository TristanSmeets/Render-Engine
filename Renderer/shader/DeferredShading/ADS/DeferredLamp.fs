#version 460 core
layout (location = 0) out vec4 FragmentColour;
layout (location = 1) out vec4 BrightColour;


uniform vec3 lightColour;

void main()
{
    FragmentColour = vec4(lightColour, 1.0f);
    float brightness = dot(lightColour, vec3( 0.2126f, 0.7152f, 0.0722f));
    if(brightness > 1.0f)
    {
        BrightColour = vec4(lightColour, 1.0f);
    }
    else
    {
        BrightColour = vec4(vec3(0.0f), 1.0f);
    }
}