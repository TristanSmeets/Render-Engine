#version 460 core
out vec4 FragColour;

in vec2 UVs;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform float exposure;

void main()
{
    const float gamma = 2.2f;
    vec3 hdrColour = texture(scene, UVs).rgb;
    vec3 bloomColour = texture(bloomBlur, UVs).rgb;

    hdrColour += bloomColour;
    //Tone mapping
    vec3 result = vec3(1.0f) - exp(-hdrColour * exposure);
    //Gamma correction
    result = pow(result, vec3(1.0f / gamma));
    FragColour = vec4(result, 1.0f);
}