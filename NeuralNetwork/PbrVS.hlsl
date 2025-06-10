
cbuffer SceneCB : register(b0)
{
    float4x4 view;
    float4x4 proj;
    float3 cameraPosition;
    float pad0;
};

StructuredBuffer<float4x4> gActorWorldMatrices : register(t0);

struct VSInput
{
    float3 position : POSITION;
    float3 normal   : NORMAL;
    uint   instanceID : SV_InstanceID;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 normal   : NORMAL;
    float2 uv       : UV;
    float3 tangent  : TANGENT;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    float4x4 world = gActorWorldMatrices[input.instanceID];
    float4 worldPos = mul(float4(input.position, 1.0f), world);
    output.worldPos = worldPos.xyz;
    output.normal = mul((float3x3)world, input.normal);
    float4 viewPos = mul(worldPos, view);
    output.position = mul(viewPos, proj);
    return output;
}