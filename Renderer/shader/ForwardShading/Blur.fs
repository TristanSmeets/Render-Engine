#version 460 core
out vec4 FragColour;

in vec2 UVs;

uniform sampler2D image;

subroutine vec3 directionalBlur();
subroutine uniform directionalBlur direction;

uniform int PixelOffset[5] = int[](0, 1, 2, 3, 4);
uniform float weight[5];

subroutine(directionalBlur) vec3 Horizontal()
{
    ivec2 pixel = ivec2(gl_FragCoord.xy);
    vec4 sum = texelFetch(image, pixel, 0) * weight[0];

    for(int i = 1; i < 5; ++i)
    {
        sum += texelFetchOffset(image, pixel, 0, ivec2(PixelOffset[i], 0)) * weight[i];
        sum += texelFetchOffset(image, pixel, 0, ivec2(-PixelOffset[i], 0)) * weight[i];
    }

    return sum.rgb;
}

subroutine(directionalBlur) vec3 Vertical()
{
    ivec2 pixel = ivec2(gl_FragCoord.xy);
    vec4 sum = texelFetch(image, pixel, 0) * weight[0];
    for(int i = 1; i < 5; ++i)
    {
        sum += texelFetchOffset(image, pixel, 0, ivec2(0, PixelOffset[i])) * weight[i];
        sum += texelFetchOffset(image, pixel, 0, ivec2(0, -PixelOffset[i])) * weight[i];
    }
    return sum.rgb;
}

void main()
{
    FragColour = vec4(direction(), 1.0f);
}