#version 330 core
out float FragColour;

in vec2 UV;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D noise;

uniform vec3 samples[64];
uniform int occlusionPower;
uniform int kernelSize;
uniform float radius;
uniform float bias;

const vec2 noiseScale = vec2(1280.0f/4.0f, 720.0f/4.0f);

uniform mat4 projection;

void main()
{
    //Get input for SSAO algorithm
    vec3 fragmentPosition = texture(gPosition, UV).xyz;
    vec3 normal = normalize(texture(gNormal, UV).rgb);
    vec3 randomVec = normalize(texture(noise, UV * noiseScale).xyz);
    //Create TBN change of basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    
    //Iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0f;
    for(int i = 0; i < kernelSize; ++i)
    {
        //Get sample position
        vec3 sample = TBN * samples[i];
        sample = fragmentPosition + sample * radius;

        //Project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(sample, 1.0f);
        offset = projection * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5f + 0.5f;

        //Get sample depth
        float sampleDepth = texture(gPosition, offset.xy).z;

        float rangeCheck = smoothstep(0.0f, 1.0f, radius / abs(fragmentPosition.z - sampleDepth));
        occlusion += (sampleDepth >= sample.z + bias ? 1.0f : 0.0) * rangeCheck;
    }
    occlusion = 1.0f - (occlusion / kernelSize);
    FragColour = pow(occlusion, occlusionPower);
}