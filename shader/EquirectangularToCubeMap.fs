#version 330 core
out vec4 FragColour;

in vec3 WorldPos;

uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591f, 0.3183f);

vec2 SampleShericalMap(vec3 position);

void main()
{
    vec2 uv = SampleShericalMap(normalize(WorldPos));
    vec3 colour = texture(equirectangularMap, uv).rgb;

    FragColour = vec4(colour, 1.0f);
}

vec2 SampleShericalMap(vec3 position)
{
    vec2 uv = vec2(atan(position.z, position.x), asin(position.y));
    uv *= invAtan;
    uv += 0.5f;
    return uv;
}
