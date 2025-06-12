Texture2D<float4> textures[];
SamplerState samp;
struct VSOutput
{
    float4 pixelPos : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 normal : NORMAL;
    float2 uv : UV;
    float4 tangent : TANGENT;
};
cbuffer MaterialTextureSlots : register(b1)
{
    uint textureSlots[128];
}

cbuffer MaterialVectorSlots : register(b3)
{
    float4 vectorSlots[128];
};

cbuffer MaterialScalarSlots : register(b2)
{
    float scalarSlots[128];
}

// Generated code for the material graph
struct MaterialOutputNodeInput
{
    float3 baseColor;
    float metallic;
    float roughness;
    float3 normal;
    float3 emissive;
    float opacity;
    float ambientOcculusion;
};
struct TextureSamplerNodeInput
{
    float2 uv;
};
struct MaterialOutputNodeOutput
{
    float4 color;
};
struct TextureSamplerNodeOutput
{
    float4 color;
};
struct MaterialOutputNodeParameters
{
};
struct TextureSamplerNodeParameters
{
    uint textureIndex;
};
MaterialOutputNodeOutput MaterialOutputNode(MaterialOutputNodeInput input, MaterialOutputNodeParameters parameters)
{
    MaterialOutputNodeOutput output;

    return output;
}
TextureSamplerNodeOutput TextureSamplerNode(TextureSamplerNodeInput input, TextureSamplerNodeParameters parameters)
{
    TextureSamplerNodeOutput output;
    output.color = textures[parameters.textureIndex].Sample(samp, input.uv);

    return output;
}
float4 main(VSOutput vertexData) : SV_TARGET
{
    TextureSamplerNodeInput TextureSamplerNodeInputData1;
    TextureSamplerNodeParameters TextureSamplerNodeParameterData1;
    TextureSamplerNodeParameterData1.textureIndex = textureSlots[0];
    TextureSamplerNodeOutput TextureSamplerNodeOutputData1 = TextureSamplerNode(TextureSamplerNodeInputData1, TextureSamplerNodeParameterData1);
    MaterialOutputNodeInput MaterialOutputNodeInputData0;
    MaterialOutputNodeParameters MaterialOutputNodeParameterData0;
    MaterialOutputNodeInputData0.baseColor = TextureSamplerNodeOutputData1.color;
    MaterialOutputNodeOutput MaterialOutputNodeOutputData0 = MaterialOutputNode(MaterialOutputNodeInputData0, MaterialOutputNodeParameterData0);
    return MaterialOutputNodeOutputData0.color;
};

struct Vector
{
    float3 value;
};

float3 vectorScale(Vector input)
{
    return input.value.x;
}