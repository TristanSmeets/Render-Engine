#version 330 core
out vec4 FragColour;

in vec3 WorldPos;

uniform samplerCube environmentMap;
uniform float roughness;

const float PI = 3.14159265359f;

float DistributionGGX(vec3 normal, vec3 H, float roughness);
float RadicalInverse_VdC(uint bits);
vec2 Hammersley(uint i, uint N);
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness);

void main()
{
    vec3 normal = normalize(WorldPos);

    //make the simplyfing assumption that V equals R equals the normal
    vec3 reflectionDirection = normal;
    vec3 viewDirection = reflectionDirection;

    const uint SAMPLE_COUNT = 1024u;
    vec3 prefilteredColour = vec3(0.0f);
    float totalWeight = 0.0f;

    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        //Generates a sample vector that's biased towards the preferred alignment direction (importance sampling).
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H = ImportanceSampleGGX(Xi, normal, roughness);
        vec3 lightDirection = normalize(2.0f * dot(viewDirection, H) * H - viewDirection);

        float NdotL = max(dot(normal, lightDirection), 0.0f);
        if(NdotL > 0.0f)
        {
            //sample from the environment's mip level based on roughness/pdf
            float distribution = DistributionGGX(normal, H, roughness);
            float NdotH = max(dot(normal, H), 0.0f);
            float HdotV = max(dot(H, viewDirection), 0.0f);
            float pdf = distribution * NdotH / (4.0f * HdotV) + 0.0001f;

            float resolution = 512.0f;
            float saTexel = 4.0f * PI / (6.0f * resolution * resolution);
            float saSample = 1.0f / (float(SAMPLE_COUNT) * pdf + 0.0001f);

            float mipLevel = roughness == 0.0f ? 0.0f : 0.5f * log2(saSample / saTexel);
            
            prefilteredColour += textureLod(environmentMap, lightDirection, mipLevel).rgb * NdotL;
            totalWeight += NdotL;
        }
    }

    prefilteredColour = prefilteredColour / totalWeight;

    FragColour = vec4(prefilteredColour, 1.0f);
}

float DistributionGGX(vec3 normal, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(normal, H), 0.0f);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;

    return nom / denom;
}

float RadicalInverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness * roughness;
    float phi = 2.0f * PI * Xi.x;
    float cosTheta = sqrt((1.0f - Xi.y) / (1.0f + (a * a - 1.0f) * Xi.y));
    float sinTheta = sqrt(1.0f - cosTheta * cosTheta);

    //from spherical coordinates to cartesian coordinates - halfway vector
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    //from tangent-space H vector to world-space sample vector
    vec3 up = abs(N.z) < 0.999f ? vec3 (0.0f, 0.0f, 1.0f) : vec3(1.0f, 0.0f, 0.0f);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);

    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}
