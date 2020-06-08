#version 460 core
out vec4 FragColour;

in vec2 UVs;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform sampler2D depthTexture;
uniform sampler2D blurredScene;
uniform float exposure;
uniform float gammaCorrection;

float inverseGamma = 1.0f / gammaCorrection;


//DOF
uniform float focalDistance;
uniform float focalRange;

float ComputeBlur(float depth, float focalDistance, float focalRange)
{
    float distance = abs(depth - focalDistance);
    float rangeCheck = focalRange/distance;
    return clamp(rangeCheck, 0.0f, 1.0f);    
}

vec4 DOF(float focalDistance, float focalRange, vec2 uv)
{
    float depth = texture(depthTexture, uv).a;
    float blur = ComputeBlur(depth, focalDistance, focalRange);
    vec3 blurColour = (texture(blurredScene, uv).rgb);
    vec3 normalColour = texture(scene, uv).rgb;
    vec3 finalColour = mix(blurColour, normalColour, blur);

    // return vec4(finalColour, 1.0f);
    return vec4(vec3(blur), 1.0f);
}

vec3 LinearToSRGB(vec3 colour)
{
    return pow(colour, vec3(inverseGamma));
}

void main()
{
    // vec3 hdrColour = texture(scene, UVs).rgb;
    vec3 hdrColour = vec3(DOF(focalDistance, focalRange, UVs));
    vec3 bloomColour = texture(bloomBlur, UVs).rgb;
    // vec3 blurColour = LinearToSRGB(texture(blurredScene, UVs).rgb);
    // vec3 blurColour = texture(blurredScene, UVs).rgb;

    // hdrColour += bloomColour;
    //Tone mapping
    vec3 result = vec3(1.0f) - exp(-hdrColour * exposure);
    //Gamma correction
    result = pow(result, vec3(1.0f / gammaCorrection));
    FragColour = vec4(result, 1.0f);
    // if(gl_FragCoord.x < 680.0f)
    // {
    //     FragColour = vec4(blurColour, 1.0f);
    // }
    // else
    // {
    //     FragColour = vec4(hdrColour, 1.0f);
    // }
}