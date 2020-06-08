#version 460 core
out vec3 FragColour;

in vec2 UVs;

uniform sampler2D scene;
uniform sampler2D depthTexture;
uniform sampler2D blurredScene;

//DOF parameters
uniform float rangeCutoff;
uniform float focalDistance;
uniform float focalRange;

float ComputeBlur(float depth, float focalDistance, float focalRange)
{
    float distance = abs(depth - focalDistance);
    float rangeCheck = focalRange / distance;

    if(rangeCheck < rangeCutoff)
    {
        return 0.0f;
    }
    else
    {
        return clamp(rangeCheck, 0.0f, 1.0f);
    }
}

vec3 CalculateDepthOfField(float focalDistance, float focalRange, vec2 uv)
{
    float depth = texture(depthTexture, uv).a;
    float blur = ComputeBlur(depth, focalDistance, focalRange);
    vec3 blurColour = texture(blurredScene, uv).rgb;
    vec3 normalColour = texture(scene, uv).rgb;
    return mix(blurColour, normalColour, blur);
}

void main()
{
    FragColour = CalculateDepthOfField(focalDistance, focalRange, UVs);
}