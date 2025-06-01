#pragma once
#include <DirectXMath.h>

struct CameraCB {
    DirectX::XMFLOAT4X4 view;
    DirectX::XMFLOAT4X4 proj;
    DirectX::XMFLOAT3 position;
    float pad0;
};

struct LightCB {
    DirectX::XMFLOAT3 direction;
    float intensity;
    DirectX::XMFLOAT3 color;
    int type; // 0=Directional, 1=Point, 2=Spot
    DirectX::XMFLOAT3 position;
    float pad1;
};

constexpr int MaxLights = 8;

struct SceneCB {
    CameraCB camera;
    LightCB lights[MaxLights];
    int numLights;
    float pad[3]; // Padding for 16-byte alignment
};