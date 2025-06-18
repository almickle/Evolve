struct LightData
{
    float3 position;
    float intensity;
    float3 color;
    int type;
    float3 direction;
    float pad;
};

SamplerState samp : register(s0);

struct RootConstants
{
    uint instanceBufferStart;
    uint bufferIndex;
};

ConstantBuffer<RootConstants> root : register(b0);

StructuredBuffer<float4x4> staticBuffer : register(t0);

StructuredBuffer<float4x4> dynamicBuffer : register(t1);

Texture2D<float4> textures[] : register(t0);

cbuffer sceneData : register(b1)
{
    float4x4 view;
    float4x4 proj;
    float3 cameraPosition;
    float pad0;
    LightData lights[8];
    uint numLights = 1;
    float3 pad1;
};

cbuffer MaterialTextureSlots : register(b2)
{
    uint textureSlot_0;
    uint textureSlot_1;
    uint textureSlot_2;
    uint textureSlot_3;
    uint textureSlot_4;
    uint textureSlot_5;
    uint textureSlot_6;
    uint textureSlot_7;
    uint textureSlot_8;
    uint textureSlot_9;
    uint textureSlot_10;
    uint textureSlot_11;
    uint textureSlot_12;
    uint textureSlot_13;
    uint textureSlot_14;
    uint textureSlot_15;
    uint textureSlot_16;
    uint textureSlot_17;
    uint textureSlot_18;
    uint textureSlot_19;
    uint textureSlot_20;
    uint textureSlot_21;
    uint textureSlot_22;
    uint textureSlot_23;
    uint textureSlot_24;
    uint textureSlot_25;
    uint textureSlot_26;
    uint textureSlot_27;
    uint textureSlot_28;
    uint textureSlot_29;
    uint textureSlot_30;
    uint textureSlot_31;
    uint textureSlot_32;
    uint textureSlot_33;
    uint textureSlot_34;
    uint textureSlot_35;
    uint textureSlot_36;
    uint textureSlot_37;
    uint textureSlot_38;
    uint textureSlot_39;
    uint textureSlot_40;
    uint textureSlot_41;
    uint textureSlot_42;
    uint textureSlot_43;
    uint textureSlot_44;
    uint textureSlot_45;
    uint textureSlot_46;
    uint textureSlot_47;
    uint textureSlot_48;
    uint textureSlot_49;
    uint textureSlot_50;
    uint textureSlot_51;
    uint textureSlot_52;
    uint textureSlot_53;
    uint textureSlot_54;
    uint textureSlot_55;
    uint textureSlot_56;
    uint textureSlot_57;
    uint textureSlot_58;
    uint textureSlot_59;
    uint textureSlot_60;
    uint textureSlot_61;
    uint textureSlot_62;
    uint textureSlot_63;
    uint textureSlot_64;
    uint textureSlot_65;
    uint textureSlot_66;
    uint textureSlot_67;
    uint textureSlot_68;
    uint textureSlot_69;
    uint textureSlot_70;
    uint textureSlot_71;
    uint textureSlot_72;
    uint textureSlot_73;
    uint textureSlot_74;
    uint textureSlot_75;
    uint textureSlot_76;
    uint textureSlot_77;
    uint textureSlot_78;
    uint textureSlot_79;
    uint textureSlot_80;
    uint textureSlot_81;
    uint textureSlot_82;
    uint textureSlot_83;
    uint textureSlot_84;
    uint textureSlot_85;
    uint textureSlot_86;
    uint textureSlot_87;
    uint textureSlot_88;
    uint textureSlot_89;
    uint textureSlot_90;
    uint textureSlot_91;
    uint textureSlot_92;
    uint textureSlot_93;
    uint textureSlot_94;
    uint textureSlot_95;
    uint textureSlot_96;
    uint textureSlot_97;
    uint textureSlot_98;
    uint textureSlot_99;
    uint textureSlot_100;
    uint textureSlot_101;
    uint textureSlot_102;
    uint textureSlot_103;
    uint textureSlot_104;
    uint textureSlot_105;
    uint textureSlot_106;
    uint textureSlot_107;
    uint textureSlot_108;
    uint textureSlot_109;
    uint textureSlot_110;
    uint textureSlot_111;
    uint textureSlot_112;
    uint textureSlot_113;
    uint textureSlot_114;
    uint textureSlot_115;
    uint textureSlot_116;
    uint textureSlot_117;
    uint textureSlot_118;
    uint textureSlot_119;
    uint textureSlot_120;
    uint textureSlot_121;
    uint textureSlot_122;
    uint textureSlot_123;
    uint textureSlot_124;
    uint textureSlot_125;
    uint textureSlot_126;
    uint textureSlot_127;
};

cbuffer MaterialVectorSlots : register(b3)
{
    float4 vectorSlots[128];
}

cbuffer MaterialScalarSlots : register(b4)
{
    float scalarSlots[128];
}

struct VSInput
{
    float3 position   : POSITION;
    float3 normal     : NORMAL;
    float2 uv         : UV;
    float4 tangent    : TANGENT;
};

struct VSOutput
{
    float4 pixelPos   : SV_POSITION;
    float3 worldPos   : TEXCOORD0;
    float3 normal     : NORMAL;
    float2 uv         : UV;
    float4 tangent    : TANGENT;
};

static const float PI = 3.14159265359;

static float4x4 identity =
{
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};

uint GetTextureSlot(uint index)
{
    switch (index)
    {
        case 0:
            return textureSlot_0;
        case 1:
            return textureSlot_1;
        case 2:
            return textureSlot_2;
        case 3:
            return textureSlot_3;
        case 4:
            return textureSlot_4;
        case 5:
            return textureSlot_5;
        case 6:
            return textureSlot_6;
        case 7:
            return textureSlot_7;
        case 8:
            return textureSlot_8;
        case 9:
            return textureSlot_9;
        case 10:
            return textureSlot_10;
        case 11:
            return textureSlot_11;
        case 12:
            return textureSlot_12;
        case 13:
            return textureSlot_13;
        case 14:
            return textureSlot_14;
        case 15:
            return textureSlot_15;
        case 16:
            return textureSlot_16;
        case 17:
            return textureSlot_17;
        case 18:
            return textureSlot_18;
        case 19:
            return textureSlot_19;
        case 20:
            return textureSlot_20;
        case 21:
            return textureSlot_21;
        case 22:
            return textureSlot_22;
        case 23:
            return textureSlot_23;
        case 24:
            return textureSlot_24;
        case 25:
            return textureSlot_25;
        case 26:
            return textureSlot_26;
        case 27:
            return textureSlot_27;
        case 28:
            return textureSlot_28;
        case 29:
            return textureSlot_29;
        case 30:
            return textureSlot_30;
        case 31:
            return textureSlot_31;
        case 32:
            return textureSlot_32;
        case 33:
            return textureSlot_33;
        case 34:
            return textureSlot_34;
        case 35:
            return textureSlot_35;
        case 36:
            return textureSlot_36;
        case 37:
            return textureSlot_37;
        case 38:
            return textureSlot_38;
        case 39:
            return textureSlot_39;
        case 40:
            return textureSlot_40;
        case 41:
            return textureSlot_41;
        case 42:
            return textureSlot_42;
        case 43:
            return textureSlot_43;
        case 44:
            return textureSlot_44;
        case 45:
            return textureSlot_45;
        case 46:
            return textureSlot_46;
        case 47:
            return textureSlot_47;
        case 48:
            return textureSlot_48;
        case 49:
            return textureSlot_49;
        case 50:
            return textureSlot_50;
        case 51:
            return textureSlot_51;
        case 52:
            return textureSlot_52;
        case 53:
            return textureSlot_53;
        case 54:
            return textureSlot_54;
        case 55:
            return textureSlot_55;
        case 56:
            return textureSlot_56;
        case 57:
            return textureSlot_57;
        case 58:
            return textureSlot_58;
        case 59:
            return textureSlot_59;
        case 60:
            return textureSlot_60;
        case 61:
            return textureSlot_61;
        case 62:
            return textureSlot_62;
        case 63:
            return textureSlot_63;
        case 64:
            return textureSlot_64;
        case 65:
            return textureSlot_65;
        case 66:
            return textureSlot_66;
        case 67:
            return textureSlot_67;
        case 68:
            return textureSlot_68;
        case 69:
            return textureSlot_69;
        case 70:
            return textureSlot_70;
        case 71:
            return textureSlot_71;
        case 72:
            return textureSlot_72;
        case 73:
            return textureSlot_73;
        case 74:
            return textureSlot_74;
        case 75:
            return textureSlot_75;
        case 76:
            return textureSlot_76;
        case 77:
            return textureSlot_77;
        case 78:
            return textureSlot_78;
        case 79:
            return textureSlot_79;
        case 80:
            return textureSlot_80;
        case 81:
            return textureSlot_81;
        case 82:
            return textureSlot_82;
        case 83:
            return textureSlot_83;
        case 84:
            return textureSlot_84;
        case 85:
            return textureSlot_85;
        case 86:
            return textureSlot_86;
        case 87:
            return textureSlot_87;
        case 88:
            return textureSlot_88;
        case 89:
            return textureSlot_89;
        case 90:
            return textureSlot_90;
        case 91:
            return textureSlot_91;
        case 92:
            return textureSlot_92;
        case 93:
            return textureSlot_93;
        case 94:
            return textureSlot_94;
        case 95:
            return textureSlot_95;
        case 96:
            return textureSlot_96;
        case 97:
            return textureSlot_97;
        case 98:
            return textureSlot_98;
        case 99:
            return textureSlot_99;
        case 100:
            return textureSlot_100;
        case 101:
            return textureSlot_101;
        case 102:
            return textureSlot_102;
        case 103:
            return textureSlot_103;
        case 104:
            return textureSlot_104;
        case 105:
            return textureSlot_105;
        case 106:
            return textureSlot_106;
        case 107:
            return textureSlot_107;
        case 108:
            return textureSlot_108;
        case 109:
            return textureSlot_109;
        case 110:
            return textureSlot_110;
        case 111:
            return textureSlot_111;
        case 112:
            return textureSlot_112;
        case 113:
            return textureSlot_113;
        case 114:
            return textureSlot_114;
        case 115:
            return textureSlot_115;
        case 116:
            return textureSlot_116;
        case 117:
            return textureSlot_117;
        case 118:
            return textureSlot_118;
        case 119:
            return textureSlot_119;
        case 120:
            return textureSlot_120;
        case 121:
            return textureSlot_121;
        case 122:
            return textureSlot_122;
        case 123:
            return textureSlot_123;        
        case 124:
            return textureSlot_124;
        case 125:
            return textureSlot_125;
        case 126:
            return textureSlot_126;
        case 127:
            return textureSlot_127;
        default:
            return textureSlot_0;
    }
}

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    return a2 / (PI * denom * denom);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 TonemapACES(float3 color)
{
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return saturate((color * (a * color + b)) / (color * (c * color + d) + e));
}