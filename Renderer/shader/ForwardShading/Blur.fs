#version 460 core
out vec4 FragColour;

in vec2 UVs;

uniform sampler2D image;

subroutine vec3 directionalBlur(vec3 startValue, vec2 texOffset);
subroutine uniform directionalBlur direction;

#define BlurLoops 10

uniform float weight[BlurLoops];

subroutine(directionalBlur) vec3 Horizontal(vec3 startValue, vec2 texOffset)
{
    vec3 result = startValue;

    for(int i = 0; i < BlurLoops; ++i)
    {
        result += texture(image, UVs + vec2(texOffset.x * i, 0.0f)).rgb * weight[i];
        result += texture(image, UVs - vec2(texOffset.x * i, 0.0f)).rgb * weight[i];
    }
    return result;
}

subroutine(directionalBlur) vec3 Vertical(vec3 startValue, vec2 texOffset)
{
    vec3 result = startValue;

    for(int i = 0; i < BlurLoops; ++i)
    {
        result += texture(image, UVs + vec2(0.0f, texOffset.y * i)).rgb * weight[i];
        result += texture(image, UVs - vec2(0.0f, texOffset.y * i)).rgb * weight[i];
    }
    return result;
}

void main()
{
    vec2 texOffset = 1.0f / textureSize(image, 0);
    vec3 startValue = texture(image, UVs).rgb * weight[0];
    vec3 colour = direction(startValue, texOffset);

    FragColour = vec4(colour, 1.0f);
}