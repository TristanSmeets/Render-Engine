#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec3 Normal;

uniform vec3 lightColour; 

void main()
{
    float brightness = dot(lightColour, vec3(0.2126f, 0.7152f, 0.0722f));
    if(brightness > 1.0f)
    {
        BrightColor = vec4(FragColor.rgb, 1.0f);
        //FragColor = vec4(brightness,0.0f, 0.0f, 1.0f);
    }
    else
    {
        BrightColor = vec4(vec3(0.0f),1.0f);
        //FragColor = vec4(0.0f, brightness, 0.0f, 1.0f);
    }
    FragColor = vec4(lightColour, 1.0f);
}
