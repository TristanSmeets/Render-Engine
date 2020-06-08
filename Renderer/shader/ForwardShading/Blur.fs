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
// float weight[kernelSize] = float[] (0.196887, 0.201549, 0.203128, 0.201549, 0.196887);
//Weights from http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
float weight[kernelSize] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

subroutine(directionalBlur) vec3 Horizontal(vec3 startValue, vec2 texOffset, int level)
{
    vec3 result = startValue;

    for(int i = 1; i < kernelSize; ++i)
    {
        result += textureLod(image, UVs + vec2(texOffset.x * i, 0.0f), level).rgb * weight[i];
        result += textureLod(image, UVs - vec2(texOffset.x * i, 0.0f), level).rgb * weight[i];
    }
    return result;
}

subroutine(directionalBlur) vec3 Vertical(vec3 startValue, vec2 texOffset, int level)
{
    vec3 result = startValue;

    for(int i = 1; i < kernelSize; ++i)
    {
        result += textureLod(image, UVs + vec2(0.0f, texOffset.y * i), level).rgb * weight[i];
        result += textureLod(image, UVs - vec2(0.0f, texOffset.y * i), level).rgb * weight[i];
    }
    return result;
}

void main()
{
    // int level = MaxLod;
    // vec3 result = vec3(0.0f);

    // while(level >= 0)
    // {
    //     vec2 texOffset = 1.0f / textureSize(image, level);
    //     result += textureLod(image, UVs, level).rgb * weight[0];
    //     result += direction(result, texOffset, level);
    //     --level;
    // }
    // vec3 colour = result / (MaxLod + 1);

    
    vec2 texOffset = 1.0f / textureSize(image, MaxLod);
    vec3 result = textureLod(image, UVs, MaxLod).rgb * weight[0];
    result += direction(result, texOffset, MaxLod);
    FragColour = vec4(result, 1.0f);
}