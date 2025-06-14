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
    uint numLights;
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
    uint   instanceId : SV_InstanceID;
};

struct VSOutput
{
    float4 position   : SV_POSITION;
    float3 worldPos   : TEXCOORD0;
    float3 normal     : NORMAL;
    float2 uv         : UV;
    float4 tangent    : TANGENT;
};  