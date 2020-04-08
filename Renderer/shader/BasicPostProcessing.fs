#version 460 core
out vec4 FragmentColour;

in vec2 UVs;

uniform sampler2D colourTexture;
uniform float gammaCorrection;
uniform float exposure;

void main()
{
    vec3 color = texture(colourTexture, UVs).rgb;

    //Exposure Tone mapping
    vec3 mapped = vec3(1.0f) - exp(-color * exposure);
    //Gamma Correction
    mapped = pow(mapped, vec3(1.0f/ gammaCorrection));
    FragmentColour = vec4(mapped, 1.0f);
}
