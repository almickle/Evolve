struct LightData
{
    float3 position;
    float intensity;
    float3 color;
    int type;
    float3 direction;
};

SamplerState samp : register(s0);

StructuredBuffer<float4x4> instanceBuffers[2] : register(t0, space0);

Texture2D<float4> textures[] : register(t0, space1);

cbuffer constants : register(b0)
{
    uint instanceBufferStart;
    bool isStaticInstance;
}
cbuffer sceneData : register(b1)
{
    float4x4 view;
    float4x4 proj;
    float3 cameraPosition;
    float pad0;
    LightData lights[8];
    uint numLights = 1;
    float3 pad1;
};
cbuffer MaterialTextureSlots : register(b2)
{
    uint textureSlots[128];
}
cbuffer MaterialVectorSlots : register(b3)
{
    float4 vectorSlots[128];
}
cbuffer MaterialScalarSlots : register(b4)
{
    float scalarSlots[128];
}

struct VSInput
{
    float3 position   : POSITION;
    float3 normal     : NORMAL;
    float2 uv         : UV;
    float4 tangent    : TANGENT;
};

struct VSOutput
{
    float4 pixelPos   : SV_POSITION;
    float3 worldPos   : TEXCOORD0;
    float3 normal     : NORMAL;
    float2 uv         : UV;
    float4 tangent    : TANGENT;
};

static const float PI = 3.14159265359;

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    return a2 / (PI * denom * denom);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}