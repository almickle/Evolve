#include "Common.hlsli"
struct MaterialOutputNodeInput
{
    float4 baseColor;
    float metallic;
    float roughness;
    float4 normal;
    float4 emissive;
    float opacity;
    float ambientOcculusion;
};
struct TextureSamplerNodeInput
{
    float4 uv;
};
struct VertexDataNodeInput
{
};
struct VectorScaleNodeInput
{
    float4 vec;
    float scale;
};
struct NormalMapNodeInput
{
    float4 color;
};
struct VectorBreakNodeInput
{
    float4 vec;
};
struct ScalarParameterNodeInput
{
};
struct MaterialOutputNodeOutput
{
    float4 color;
};
struct TextureSamplerNodeOutput
{
    float4 color;
};
struct VertexDataNodeOutput
{
    float4 pixelPos;
    float4 worldPos;
    float4 normal;
    float4 uv;
    float4 tangent;
};
struct VectorScaleNodeOutput
{
    float4 vec;
};
struct NormalMapNodeOutput
{
    float4 normal;
};
struct VectorBreakNodeOutput
{
    float x;
    float y;
    float z;
};
struct ScalarParameterNodeOutput
{
    float value;
};
struct MaterialOutputNodeParameters
{
};
struct TextureSamplerNodeParameters
{
    uint textureIndex;
};
struct VertexDataNodeParameters
{
};
struct VectorScaleNodeParameters
{
};
struct NormalMapNodeParameters
{
};
struct VectorBreakNodeParameters
{
};
struct ScalarParameterNodeParameters
{
    uint scalarIndex;
};
MaterialOutputNodeOutput MaterialOutputNode(MaterialOutputNodeInput input, MaterialOutputNodeParameters parameters, VSOutput vertexData)
{
    MaterialOutputNodeOutput output;
    
            // Camera and position
    float3 viewDir = normalize(cameraPosition - vertexData.worldPos);

            // PBR constants
    float3 F0 = lerp(float3(0.04, 0.04, 0.04), input.baseColor.rgb, input.metallic);

    float3 Lo = float3(0, 0, 0);

            // Loop over lights
            [unroll(numLights)]
    for (uint i = 0; i < numLights; ++i)
    {
        LightData light = lights[i];
        float3 L = normalize(light.position - vertexData.worldPos);
        float3 H = normalize(viewDir + L);

                // Attenuation (simple, can be improved)
        float distance = length(light.position - vertexData.worldPos);
        float attenuation = 1.0 / (distance * distance);

                // Diffuse
        float NdotL = max(dot(input.normal.xyz, L), 0.0);

                // Cook-Torrance BRDF
        float NDF = DistributionGGX(input.normal.xyz, H, input.roughness);
        float G = GeometrySmith(input.normal.xyz, viewDir, L, input.roughness.r);
        float3 F = FresnelSchlick(max(dot(H, viewDir), 0.0), F0);

        float3 numerator = NDF * G * F;
        float denominator = 4 * max(dot(input.normal.xyz, viewDir), 0.0) * NdotL + 0.001;
        float3 specular = numerator / denominator;

        float3 kS = F;
        float3 kD = 1.0 - kS;
        kD *= 1.0 - input.metallic;

        float3 irradiance = light.color * light.intensity * attenuation;

        Lo += (kD * input.baseColor.rgb / PI + specular) * irradiance * NdotL;
    }

            // Ambient
    float3 ambient = input.ambientOcculusion * input.baseColor.rgb * 0.03;

            // Final color
    float3 color = ambient + Lo + input.emissive.rgb;
    color = color / (color + 1.0); // HDR tonemapping
    color = pow(color, 1.0 / 2.2); // Gamma correction

    output.color = float4(color, input.opacity);

    return output;
}
TextureSamplerNodeOutput TextureSamplerNode(TextureSamplerNodeInput input, TextureSamplerNodeParameters parameters, VSOutput vertexData)
{
    TextureSamplerNodeOutput output;
    output.color = textures[parameters.textureIndex].Sample(samp, input.uv.xy);

    return output;
}
VertexDataNodeOutput VertexDataNode(VertexDataNodeInput input, VertexDataNodeParameters parameters, VSOutput vertexData)
{
    VertexDataNodeOutput output;
    output.pixelPos = vertexData.pixelPos;
    output.worldPos = float4(vertexData.worldPos, 0.0f);
    output.normal = float4(vertexData.normal, 0.0f);
    output.uv = float4(vertexData.uv, 0.0f, 0.0f);
    output.tangent = vertexData.tangent;

    return output;
}
VectorScaleNodeOutput VectorScaleNode(VectorScaleNodeInput input, VectorScaleNodeParameters parameters, VSOutput vertexData)
{
    VectorScaleNodeOutput output;
    output.vec = float4(input.vec.xyz * input.scale, 0.0f);

    return output;
}
NormalMapNodeOutput NormalMapNode(NormalMapNodeInput input, NormalMapNodeParameters parameters, VSOutput vertexData)
{
    NormalMapNodeOutput output;
    float3 T = normalize(vertexData.tangent.xyz);
    float3 N = normalize(vertexData.normal.xyz);
    float3 B = normalize(cross(N, T) * vertexData.tangent.w);
    float3x3 TBN = float3x3(T, B, N);
    output.normal = float4(mul(TBN, input.color.rgb), 0.0f);

    return output;
}
VectorBreakNodeOutput VectorBreakNode(VectorBreakNodeInput input, VectorBreakNodeParameters parameters, VSOutput vertexData)
{
    VectorBreakNodeOutput output;
    output.x = input.vec.x;
    output.y = input.vec.y;
    output.z = input.vec.z;

    return output;
}
ScalarParameterNodeOutput ScalarParameterNode(ScalarParameterNodeInput input, ScalarParameterNodeParameters parameters, VSOutput vertexData)
{
    ScalarParameterNodeOutput output;
    output.value = scalarSlots[parameters.scalarIndex];

    return output;
}
float4 main(VSOutput vertexData) : SV_TARGET
{
    VertexDataNodeInput VertexDataNodeInputData4;
    VertexDataNodeParameters VertexDataNodeParameterData4;
    VertexDataNodeOutput VertexDataNodeOutputData4 = VertexDataNode(VertexDataNodeInputData4, VertexDataNodeParameterData4, vertexData);
    ScalarParameterNodeInput ScalarParameterNodeInputData8;
    ScalarParameterNodeParameters ScalarParameterNodeParameterData8;
    ScalarParameterNodeParameterData8.scalarIndex = scalarSlots[0];
    ScalarParameterNodeOutput ScalarParameterNodeOutputData8 = ScalarParameterNode(ScalarParameterNodeInputData8, ScalarParameterNodeParameterData8, vertexData);
    VectorScaleNodeInput VectorScaleNodeInputData5;
    VectorScaleNodeInputData5.vec = VertexDataNodeOutputData4.uv;
    VectorScaleNodeInputData5.scale = ScalarParameterNodeOutputData8.value;
    VectorScaleNodeParameters VectorScaleNodeParameterData5;
    VectorScaleNodeOutput VectorScaleNodeOutputData5 = VectorScaleNode(VectorScaleNodeInputData5, VectorScaleNodeParameterData5, vertexData);
    TextureSamplerNodeInput TextureSamplerNodeInputData1;
    TextureSamplerNodeInputData1.uv = VectorScaleNodeOutputData5.vec;
    TextureSamplerNodeParameters TextureSamplerNodeParameterData1;
    TextureSamplerNodeParameterData1.textureIndex = textureSlots[0];
    TextureSamplerNodeOutput TextureSamplerNodeOutputData1 = TextureSamplerNode(TextureSamplerNodeInputData1, TextureSamplerNodeParameterData1, vertexData);
    TextureSamplerNodeInput TextureSamplerNodeInputData2;
    TextureSamplerNodeInputData2.uv = VectorScaleNodeOutputData5.vec;
    TextureSamplerNodeParameters TextureSamplerNodeParameterData2;
    TextureSamplerNodeParameterData2.textureIndex = textureSlots[1];
    TextureSamplerNodeOutput TextureSamplerNodeOutputData2 = TextureSamplerNode(TextureSamplerNodeInputData2, TextureSamplerNodeParameterData2, vertexData);
    TextureSamplerNodeInput TextureSamplerNodeInputData3;
    TextureSamplerNodeInputData3.uv = VectorScaleNodeOutputData5.vec;
    TextureSamplerNodeParameters TextureSamplerNodeParameterData3;
    TextureSamplerNodeParameterData3.textureIndex = textureSlots[2];
    TextureSamplerNodeOutput TextureSamplerNodeOutputData3 = TextureSamplerNode(TextureSamplerNodeInputData3, TextureSamplerNodeParameterData3, vertexData);
    VectorBreakNodeInput VectorBreakNodeInputData7;
    VectorBreakNodeInputData7.vec = TextureSamplerNodeOutputData2.color;
    VectorBreakNodeParameters VectorBreakNodeParameterData7;
    VectorBreakNodeOutput VectorBreakNodeOutputData7 = VectorBreakNode(VectorBreakNodeInputData7, VectorBreakNodeParameterData7, vertexData);
    NormalMapNodeInput NormalMapNodeInputData6;
    NormalMapNodeInputData6.color = TextureSamplerNodeOutputData3.color;
    NormalMapNodeParameters NormalMapNodeParameterData6;
    NormalMapNodeOutput NormalMapNodeOutputData6 = NormalMapNode(NormalMapNodeInputData6, NormalMapNodeParameterData6, vertexData);
    MaterialOutputNodeInput MaterialOutputNodeInputData0;
    MaterialOutputNodeInputData0.baseColor = TextureSamplerNodeOutputData1.color;
    MaterialOutputNodeInputData0.roughness = VectorBreakNodeOutputData7.x;
    MaterialOutputNodeInputData0.normal = NormalMapNodeOutputData6.normal;
    MaterialOutputNodeInputData0.metallic = 0.000000;
    MaterialOutputNodeInputData0.emissive = float4(0.000000, 0.000000, 0.000000, 0.000000);
    MaterialOutputNodeInputData0.opacity = 1.000000;
    MaterialOutputNodeInputData0.ambientOcculusion = 1.000000;
    MaterialOutputNodeParameters MaterialOutputNodeParameterData0;
    MaterialOutputNodeOutput MaterialOutputNodeOutputData0 = MaterialOutputNode(MaterialOutputNodeInputData0, MaterialOutputNodeParameterData0, vertexData);
    return MaterialOutputNodeOutputData0.color;
};