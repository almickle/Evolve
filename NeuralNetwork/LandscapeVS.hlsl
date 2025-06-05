cbuffer SceneCB : register(b0)
{
    float4x4 view;
    float4x4 proj;
    float3 cameraPosition;
    float pad0;
};

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD1;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    float4 worldPos = float4(input.position, 1.0f); // No transform
    output.worldPos = worldPos.xyz;
    output.normal = input.normal;
    output.texcoord = input.texcoord;
    float4 viewPos = mul(worldPos, view);
    output.position = mul(viewPos, proj);
    return output;
}