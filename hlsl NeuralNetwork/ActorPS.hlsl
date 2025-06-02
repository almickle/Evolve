
struct VSOutput
{
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 normal   : NORMAL;
};

float4 main(VSOutput input) : SV_TARGET
{
    float3 lightDir = normalize(float3(0.5f, 1.0f, -0.5f));
    float NdotL = max(dot(normalize(input.normal), lightDir), 0.0f);
    float3 color = float3(0.7f, 0.8f, 1.0f) * NdotL + 0.1f;
    return float4(color, 1.0f);
}