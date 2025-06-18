#include "Common.hlsli"

VSOutput main(VSInput input, uint instanceId : SV_InstanceID)
{
    VSOutput output;
    float4x4 world = (root.bufferIndex == 0) ? staticBuffer[root.instanceBufferStart + instanceId] : dynamicBuffer[root.instanceBufferStart + instanceId];
    float4 worldPos = mul(float4(input.position, 1.0f), world);
    output.worldPos = worldPos.xyz;
    output.normal = mul((float3x3)world, input.normal);
    output.tangent = float4(mul((float3x3) world, input.tangent.xyz), input.tangent.w);
    float4 viewPos = mul(worldPos, view);
    output.pixelPos = mul(viewPos, proj);
    output.uv = input.uv;
    return output;
}