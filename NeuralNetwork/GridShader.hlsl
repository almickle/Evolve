struct VSOutput
{
    float4 pixelPos : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 normal : NORMAL;
    float2 uv : UV;
    float4 tangent : TANGENT;
};

cbuffer GridParams : register(b2)
{
    float4x4 invViewProj; // inverse view-projection matrix
    float2 screenResolution; // screen resolution (width, height)
    float gridSpacing = 1.0; // spacing between grid lines
    float gridThickness = 0.02; // thickness of grid lines
    float3 gridColor = float3(0.8, 0.8, 0.8); // color of grid lines
    float pad;
    float3 cameraPosition;
};

float4 main(VSOutput input) : SV_Target
{
    // NDC space [-1, 1]
    float2 uv = input.pixelPos.xy / screenResolution * 2.0 - 1.0;
    float4 clip = float4(uv, 1, 1);

    // Unproject to world
    float4 worldPos = mul(invViewProj, clip);
    worldPos /= worldPos.w;

    float3 rayDir = normalize(worldPos.xyz - cameraPosition);
    float3 rayOrigin = cameraPosition;

    // Ray-plane intersection (plane at y = 0)
    float t = -rayOrigin.y / rayDir.y;
    float3 hitPoint = rayOrigin + t * rayDir;

    // Compute grid line intensity
    float2 gridCoord = hitPoint.xz / gridSpacing;
    float2 cell = abs(frac(gridCoord - 0.5) - 0.5); // center-aligned lines
    float2 lines = smoothstep(0.0, gridThickness, gridThickness - cell);

    float grid = max(lines.x, lines.y); // combine X and Z lines

    // Final color
    float3 color = lerp(gridColor, float3(1, 1, 1), grid);
    return float4(color, 1.0);
}
