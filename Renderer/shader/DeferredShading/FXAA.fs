#version 460 core
out vec4 FragmentColour;

in vec2 UV;

uniform sampler2D colourTexture;
uniform vec3 inverseFilterTextureSize;
uniform float spanMax;
uniform float reduceMinimum;
uniform float reduceMultiplier;

//Constant value figured out by smarter people.
const vec3 luma = vec3(0.299f, 0.587f, 0.114f);

void main()
{
    vec2 textureOffset = inverseFilterTextureSize.xy;
    float lumaTopLeft = dot(luma, texture(colourTexture, UV + vec2(-1.0f,-1.0f) * textureOffset).xyz);
    float lumaTopRight = dot(luma, texture(colourTexture, UV + vec2(1.0f,-1.0f) * textureOffset).xyz);
    float lumaBottomLeft = dot(luma, texture(colourTexture, UV + vec2(-1.0f, 1.0f) * textureOffset).xyz);
    float lumaBottomRight = dot(luma, texture(colourTexture, UV + vec2(1.0f, 1.0f) * textureOffset).xyz);
    float lumaMiddel = dot(luma, texture(colourTexture, UV).xyz);

    vec2 blurDirection;
    blurDirection.x = -((lumaTopLeft + lumaTopRight) - (lumaBottomLeft + lumaBottomRight));
    blurDirection.y = (lumaTopLeft + lumaBottomLeft) - (lumaTopRight + lumaBottomRight);

    float directionReduce = max((lumaTopLeft + lumaTopRight + lumaBottomLeft + lumaBottomRight) * (reduceMultiplier * 0.25f), reduceMinimum);
    float inverseDirectionAdjustment = 1.0f/(min(abs(blurDirection.x), abs(blurDirection.y)) + directionReduce);

    blurDirection = min(vec2(spanMax), max(vec2(-spanMax), blurDirection * inverseDirectionAdjustment)) * textureOffset;

    // FragmentColour = vec4(vec3(directionReduce), 1.0f);

    vec3 result1 = 0.5f * (
        texture(colourTexture, UV + (blurDirection * vec2(1.0f/3.0f - 0.5f))).xyz +
        texture(colourTexture, UV + (blurDirection * vec2(2.0f/3.0f - 0.5f))).xyz);

    vec3 result2 = result1 * 0.5f + 0.25f * (
        texture(colourTexture, UV + (blurDirection * vec2(-0.5f))).xyz + 
        texture(colourTexture, UV + (blurDirection * vec2(0.5f))).xyz);

    float lumaMin = min(lumaMiddel, min(min(lumaTopLeft, lumaTopRight), min(lumaBottomLeft, lumaBottomRight)));
    float lumaMax = max(lumaMiddel, max(max(lumaTopLeft, lumaTopRight), max(lumaBottomLeft, lumaBottomRight)));
    float lumaResult2 = dot(luma, result2);

    if(lumaResult2 < lumaMin || lumaResult2 > lumaMax)
    {
        FragmentColour = vec4(result1, 1.0f);
    }
    else
    {
        FragmentColour = vec4(result2, 1.0f);
    }
}