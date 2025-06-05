struct Light
{
    float3 direction;
    float  intensity;
    float3 color;
    int    type;      // 0 = directional, 1 = point (expand as needed)
    float3 position;  // for point lights
    float  pad;       // padding for alignment
};

cbuffer SceneCB : register(b0)
{
    float4x4 view;
    float4x4 proj;
    float3   cameraPos;
    float    pad0;
    Light lights[8]; // Match max light count with C++ side
    int      numLights;
    int3   pad1;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 normal   : NORMAL;
};

float4 main(VSOutput input) : SV_TARGET
{
    float3 normal = normalize(input.normal);
    float3 color = float3(0, 0, 0);

    for (int i = 0; i < numLights; ++i)
    {
        float NdotL = 0.0f;
        if (lights[i].type == 0) // Directional
        {
            float3 lightDir = normalize(-lights[i].direction);
            NdotL = max(dot(normal, lightDir), 0.0f);
        }
        else if (lights[i].type == 1) // Point
        {
            float3 lightDir = normalize(lights[i].position - input.worldPos);
            NdotL = max(dot(normal, lightDir), 0.0f);
        }
        color += lights[i].color * lights[i].intensity * NdotL;
    }

    color = color * float3(0.7f, 0.8f, 1.0f) + 0.1f;
    return float4(color, 1.0f);
}