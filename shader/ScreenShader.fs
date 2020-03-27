#version 460 core
out vec4 FragmentColour;

in vec2 TexCoords;

subroutine vec4 postProcessingType(in sampler2D screenTexture, in vec2 texCoords);
subroutine uniform postProcessingType postProcessing;

uniform sampler2D screenTexture;
uniform float gammaCorrection;
uniform float exposure;

const float offset = 1.0f / 300.0f;

vec2 offsets[9] = vec2[](
        vec2(-offset,   offset),    //top left
        vec2(0.0f,      offset),    //top center
        vec2(offset,    offset),    //top right
        vec2(-offset,   0.0f),      //center left
        vec2(0.0f,      0.0f),      //center center
        vec2(offset,    0.0f),      //center right
        vec2(-offset,   -offset),   //bottom left
        vec2(0.0f,      -offset),   //botton center
        vec2(offset,    -offset)    //bottom right
);

vec4 UseKernel(in sampler2D screenTexture, in vec2 texCoords, in float[9] kernel);
float[9] GetSharpenKernel();
float[9] GetBlurKernel();
float[9] GetEdgeKernel();

subroutine(postProcessingType) vec4 NormalOutput(in sampler2D screenTexture, in vec2 texCoords)
{
    return vec4(texture(screenTexture, texCoords).rgb, 1.0f);
}

subroutine(postProcessingType) vec4 InverseColours(in sampler2D screenTexture, in vec2 texCoords)
{
    return vec4(vec3(1.0f - texture(screenTexture,texCoords)), 1.0f);
}

subroutine(postProcessingType) vec4 WeightedGreyScale(in sampler2D screenTexture, in vec2 texCoords)
{
    vec3 colour = texture(screenTexture, texCoords).rbg;
    float average = 0.2126f * colour.r + 0.7152f * colour.g + 0.0722 * colour.b;
    return vec4(average, average, average, 1.0f);
}

subroutine(postProcessingType) vec4 Sharpen(in sampler2D screenTexture, in vec2 texCoords)
{
    float[9] kernel = GetSharpenKernel();
    return UseKernel(screenTexture, texCoords, kernel);
}

subroutine(postProcessingType) vec4 Blur(in sampler2D screenTexture, in vec2 texCoords)
{
    float[9] kernel = GetBlurKernel();
    return UseKernel(screenTexture, texCoords, kernel);
}

subroutine(postProcessingType) vec4 Edge(in sampler2D screenTexture, in vec2 texCoords)
{
    float[9] kernel = GetEdgeKernel();
    return UseKernel(screenTexture, texCoords, kernel);
}

void main()
{
    vec3 color = postProcessing(screenTexture, TexCoords).rgb;

    //Exposure tone mapping
    vec3 mapped = vec3(1.0f) - exp(-color * exposure);
    //Gamma correction
    mapped = pow(mapped, vec3(1.0f/ gammaCorrection));

    FragmentColour = vec4(mapped, 1.0f);
}

vec4 UseKernel(in sampler2D screenTexture, in vec2 texCoords, in float[9] kernel)
{
    vec3 sampleTex[9];
    for(int i = 0; i < 9; ++i)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0f);
    for(int i = 0; i < 9; ++i)
    {
        col += sampleTex[i] * kernel[i];
    }
    return vec4(col, 1.0f);
}

float[9] GetSharpenKernel()
{
    float kernel[9] = float[](
        -1, -1, -1,
        -1, 9, -1,
        -1, -1, -1
    );
    return kernel;
}

float[9] GetBlurKernel()
{
    float kernel[9] = float[](
        1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
        2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
        1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
    );
    return kernel;
}

float[9] GetEdgeKernel()
{
    float kernel[9] = float[](
        1, 1, 1,
        1, -8, 1,
        1, 1, 1
    );
    return kernel;
}
