#version 460 core
out float FragmentColour;

in vec2 UV;

uniform sampler2D ssaoInput;

void main()
{
    vec2 texelSize = 1.0f / vec2(textureSize(ssaoInput, 0));
    float result = 0.0f;
    for (int x = -2; x < 2; ++x)
    {
        for(int y = -2; y < 2; ++y)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoInput, UV + offset).r;
        }
    }
    FragmentColour = result / (4.0f * 4.0f);
}