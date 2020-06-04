#version 460 core
out vec4 FragColour;

in vec2 UVs;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform sampler2D depthTexture;
uniform float exposure;
uniform float gammaCorrection;

//DOF
const int NumberOfTaps = 4;
uniform float focalDistance;
uniform float focalRange;
uniform float maxCoC;
uniform vec2 tapOffset[NumberOfTaps];

float ComputeBlur(float depth, float focalDistance, float focalRange)
{
    float blur = abs(depth - focalDistance) * focalRange;
    return clamp(blur, 0.0f, 1.0f);
}

vec4 DOF(float focalDistance, float focalRange, float maxCoC, vec2 tapOffset[NumberOfTaps], vec2 uv)
{
    float depth = texture(depthTexture, uv).a;
    float blur = ComputeBlur(depth, focalDistance, focalRange);
    vec4 colourSum = texture(scene, uv);
    float sizeCoC = blur * maxCoC;
    float totalContrib = 1.0f;

    for(int i = 0; i < NumberOfTaps; ++i)
    {
        vec2 tapUV = uv + tapOffset[i] * sizeCoC;
        vec4 tapColour = texture(scene, tapUV);
        float tapDepth = texture(depthTexture, tapUV).a;
        float tapContrib = (tapDepth > depth) ? 1.0f : blur;
        colourSum += tapContrib;
        totalContrib += tapContrib;
    }
    return colourSum / totalContrib;
}

void main()
{
    // vec3 hdrColour = texture(scene, UVs).rgb;
    vec3 hdrColour = vec3(DOF(focalDistance, focalRange, maxCoC, tapOffset, UVs));
    vec3 bloomColour = texture(bloomBlur, UVs).rgb;

    hdrColour += bloomColour;
    //Tone mapping
    vec3 result = vec3(1.0f) - exp(-hdrColour * exposure);
    //Gamma correction
    result = pow(result, vec3(1.0f / gammaCorrection));
    // FragColour = vec4(result, 1.0f);
    // FragColour = vec4(vec3(ComputeBlur(texture(depthTexture, UVs).a, focalDistance, focalRange)), 1.0f);
    FragColour = DOF(focalDistance, focalRange, maxCoC, tapOffset, UVs);
    // FragColour = vec4(vec3(texture(depthTexture, UVs).a),1.0f);
}