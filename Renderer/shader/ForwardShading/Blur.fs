#version 460 core
out vec4 FragColour;

in vec2 UVs;

uniform sampler2D image;
uniform int MaxLod;

subroutine vec3 directionalBlur(vec3 startValue, vec2 texOffset, int level);
subroutine uniform directionalBlur direction;

#define kernelSize 5

//Weights retrieved from Gaussian Kernel Calculator at
//http://dev.theomader.com/gaussian-kernel-calculator/
//Using Sigma 8.0 and kernel size 5
float weight[kernelSize] = float[] (0.187691, 0.206038, 0.212543, 0.206038, 0.187691);

subroutine(directionalBlur) vec3 Horizontal(vec3 startValue, vec2 texOffset, int level)
{
    vec3 result = startValue;

    for(int i = 0; i < kernelSize; ++i)
    {
        result += textureLod(image, UVs + vec2(texOffset.x * i, 0.0f), level).rgb * weight[i];
        result += textureLod(image, UVs - vec2(texOffset.x * i, 0.0f), level).rgb * weight[i];
    }
    return result;
}

subroutine(directionalBlur) vec3 Vertical(vec3 startValue, vec2 texOffset, int level)
{
    vec3 result = startValue;

    for(int i = 0; i < kernelSize; ++i)
    {
        result += textureLod(image, UVs + vec2(0.0f, texOffset.y * i), level).rgb * weight[i];
        result += textureLod(image, UVs - vec2(0.0f, texOffset.y * i), level).rgb * weight[i];
    }
    return result;
}

void main()
{
    vec3 result = vec3(0.0f);
    vec2 texOffset = 1.0f / textureSize(image, MaxLod);
    result += textureLod(image, UVs, MaxLod).rgb * weight[0];
    result += direction(result, texOffset, MaxLod);

    vec3 colour = result / kernelSize;
    FragColour = vec4(result, 1.0f);
}