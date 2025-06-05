struct VSOutput
{
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD1;
};

float4 main(VSOutput input) : SV_TARGET
{
    // Simple diffuse color based on normal for now
    float3 lightDir = normalize(float3(0.5, 0.5, 1));
    float NdotL = max(dot(normalize(input.normal), lightDir), 0.0);
    float3 baseColor = float3(0.3, 0.7, 0.3); // Greenish
    return float4(baseColor * NdotL, 1.0);
}