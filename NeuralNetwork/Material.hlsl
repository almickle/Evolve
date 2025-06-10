
Texture2D<float4> textures[];
SamplerState samp;

struct Light
{
    float3 direction;
    float intensity;
    float3 color;
    int type;
    float3 position;
    float pad;
};

cbuffer SceneCB : register(b0)
{
    float4x4 view;
    float4x4 proj;
    float3 cameraPos;
    float pad0;
    Light lights[8];
    int numLights;
    float3 pad1;
};

cbuffer MaterialTextureSlots : register(b1)
{
    uint textureSlots[128];
}

cbuffer MaterialScalarSlots : register(b2)
{
    float scalarSlots[128];
}

cbuffer MaterialVectorSlots : register(b3)
{
    float4 vectorSlots[128];
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 normal   : NORMAL;
    float2 uv       : UV;
    float3 tangent  : TANGENT;
};


struct TextureSamplerNodeInput
{
    float2 uv;
};

struct TextureSamplerNodeOutput
{
    float3 color;
};

struct TextureSamplerNodeParameters
{
    uint textureParam0;
};

struct MaterialOutputNodeInput
{
    float3 albedo;
    float metallic;
    float roughness;
    float3 normal;
    float3 emissive;
    float alpha;
    float ambientOcculusion;
};

struct MaterialOutputNodeOutput
{
    float4 color;
};

TextureSamplerNodeOutput TextureSamplerNode(TextureSamplerNodeInput input, TextureSamplerNodeParameters parameters)
{
    TextureSamplerNodeOutput output;
    return output;
}

MaterialOutputNodeOutput MaterialOutputNode(MaterialOutputNodeInput input)
{
    MaterialOutputNodeOutput output;
    return output;
}

float4 main(VSOutput input) : SV_TARGET
{
    TextureSamplerNodeInput TextureSamplerNodeInputData1;
    TextureSamplerNodeInputData1.uv = input.uv;
    TextureSamplerNodeParameters TextureSamplerNodeParameterData1;
    TextureSamplerNodeParameterData1.textureParam0 = 0;
    TextureSamplerNodeOutput TextureSamplerNodeOutputData1 = TextureSamplerNode(TextureSamplerNodeInputData1, TextureSamplerNodeParameterData1);
    
    MaterialOutputNodeInput MaterialOutputNodeInputData0;
    MaterialOutputNodeInputData0.albedo = TextureSamplerNodeOutputData1.color;
    MaterialOutputNodeOutput MaterialOutputNodeOutputData0 = MaterialOutputNode(MaterialOutputNodeInputData0);
    return MaterialOutputNodeOutputData0.color;
}