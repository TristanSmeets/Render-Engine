#version 460 core

out vec4 FragmentColour;
in vec2 UVs;

uniform sampler2D ColourTexture;

uniform struct ToneMapping {
    float exposure;
    int tmo;
} TMO_Properties;

uniform struct GammaCorrection {
    float correction;
    bool useGammaCorrection;
}GC_Properties;

uniform struct FXAA{
    vec2 texelStep;
    float lumaThreshold;
    float reductionMultiplier;
    float reductionMinimum;
    float maxSpan;

    bool fxaaOn;
    bool showEdges;
} FXAA_Properties;

/* Applies FXAA. 
* See Timothy Lottes FXAA white paper: https://developer.download.nvidia.com/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf
* This implementation based on: https://github.com/McNopper/OpenGL/blob/master/Example42/shader/fxaa.frag.glsl
*/
vec3 ApplyFXAA(in vec3 colour)
{
    vec3 Result = colour;
    if ( FXAA_Properties.fxaaOn == false)
    {
        return Result;
    }

    // Sample Neight texels. Offsets are adapted to OpenGL texture coordinates.
    vec3 rgbNW = textureOffset(ColourTexture, UVs, ivec2(-1, 1)).rgb;
    vec3 rgbNE = textureOffset(ColourTexture, UVs, ivec2(1, 1)).rgb;
    vec3 rgbSW = textureOffset(ColourTexture, UVs, ivec2(-1, -1)).rgb;
    vec3 rgbSE = textureOffset(ColourTexture, UVs, ivec2(1, -1)).rgb;

    // Luma value used in video games.
    // See http://en.wikipedia.org/wiki/Grayscale for more info.
    const vec3 toLuma = vec3(0.299, 0.587, 0.114);

    // Convert from RGB to luma.
    float lumaNW = dot(rgbNW, toLuma);
    float lumaNE = dot(rgbNE, toLuma);
    float lumaSW = dot(rgbSW, toLuma);
    float lumaSE = dot(rgbSE, toLuma);
    float lumaM = dot(colour, toLuma);

    // Gather minimum and maximum luma.
    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    if(lumaMax - lumaMin <= lumaMax * FXAA_Properties.lumaThreshold)
    {
        return Result;
    }

    // Sampling along the gradient.
    vec2 samplingDirection;
    samplingDirection.x = - ((lumaNW + lumaNE) - (lumaSW + lumaSE));
    samplingDirection.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    // Sampling step distace depends on the luma. The brighter the sampled texels, the smaller the final sampling step direction.
    // Areas that are brighter are less blurred than dark areas.
    float samplingDirectionReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * 0.25 * FXAA_Properties.reductionMultiplier, FXAA_Properties.reductionMinimum);

    // Factor for norming the sampling direction plus adding the brightness influence.
    float minSamplingDirectionFactor = 1.0 / (min(abs(samplingDirection.x), abs(samplingDirection.y)) + samplingDirectionReduce);

    // Calculate final sampling direction colour by reducing, clamping to a range and finally adapting to the texture size.
    samplingDirection = clamp(samplingDirection * minSamplingDirectionFactor, vec2(-FXAA_Properties.maxSpan), vec2(FXAA_Properties.maxSpan)) * FXAA_Properties.texelStep;

    // Inner samples on the tab.
    vec3 rgbSampleNegative = texture(ColourTexture, UVs + samplingDirection * (1.0/3.0 - 0.5)).rgb;
    vec3 rgbSamplePositive = texture(ColourTexture, UVs + samplingDirection * (2.0/3.0 - 0.5)).rgb;

    vec3 rgbTwoTab = (rgbSamplePositive + rgbSampleNegative) * 0.5;

    // Outer samples on the tab.
    vec3 rgbSampleNegativeOuter = texture(ColourTexture, UVs + samplingDirection * (-0.5)).rgb;
    vec3 rgbSamplePositiveOuter = texture(ColourTexture, UVs + samplingDirection * (0.5)).rgb;

    vec3 rgbFourTab = (rgbSamplePositiveOuter + rgbSampleNegativeOuter) * 0.25 + rgbTwoTab * 0.5;

    // Calculate luma for checking against the minimum and maximum value.
    float lumaFourTab = dot(rgbTwoTab, toLuma);

    // Are outer samples of the tab beyond the enge ...
    if(lumaFourTab < lumaMin || lumaFourTab > lumaMax)
    {
        // ... yes, so use only two samples.
        Result = rgbTwoTab;
    }
    else
    {
        // ... no, so use four samples
        Result = rgbFourTab;
    }

    if(FXAA_Properties.showEdges)
    {
        Result.r = 1.0;
    }

    return Result;
}

vec3 SimpleReinhard(in vec3 colour)
{
    return vec3(1.0f) - exp(-colour * TMO_Properties.exposure);
}

/* Uses Krzysztof Narkowicz' ACES approximation:
 * https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
 */
vec3 SimpleAces(in vec3 colour)
{
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    colour *= TMO_Properties.exposure;
   return clamp((colour * ( a * colour + b))/ (colour * (c * colour + d)+ e), 0.0, 1.0f);
}


vec3 HableToneMap(vec3 colour)
{
    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    return ((colour * (A * colour + C * B) + D * E) / (colour * (A * colour + B) + D * F)) - E / F;
}

/* Based on John Hable TMO.
 * https://64.github.io/tonemapping/
*/
vec3 HableFilmic(in vec3 colour)
{
    vec3 curr = HableToneMap(colour * TMO_Properties.exposure);

    vec3 W = vec3(11.2f);
    vec3 white_scale = vec3(1.0f) / HableToneMap(W);
    return curr * white_scale;
}

/* Apply tone mapping to the colour.
 * TODO: Play around with other TMO https://64.github.io/tonemapping/
 */
vec3 ApplyToneMapping(in vec3 colour)
{
    switch(TMO_Properties.tmo){
        case 1:
            return SimpleReinhard(colour);
        case 2:
            return SimpleAces(colour);
        case 3:
            return HableFilmic(colour);
        default:
        return colour;
    }
}

/* Correct for the monitor's non-linear output */
vec3 ApplyGammaCorrection(in vec3 colour)
{
    if(GC_Properties.useGammaCorrection == false)
    {
        return colour;
    }
    return pow(colour, vec3(1.0f / GC_Properties.correction));
}

void main()
{
    vec3 scene = texture(ColourTexture, UVs).rgb;
    vec3 endResult = vec3(0.0);
    
    endResult = ApplyFXAA(scene);
    endResult = ApplyToneMapping(endResult);
    endResult = ApplyGammaCorrection(endResult);
    FragmentColour = vec4(endResult, 1.0);
}
