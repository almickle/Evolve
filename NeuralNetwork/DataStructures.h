#pragma once
#include <DirectXMath.h>

// Light constant buffer
struct LightCB {
	DirectX::XMFLOAT3 direction;
	float intensity;
	DirectX::XMFLOAT3 color;
	int type;
	DirectX::XMFLOAT3 position;
	float pad; // Padding for alignment
};

// Camera constant buffer
struct CameraCB {
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 proj;
	DirectX::XMFLOAT3 position;
	float pad; // Padding for alignment
};

// Actor transform constant buffer (for world matrix creation)
struct ActorTransformCB {
	DirectX::XMFLOAT3 position;
	float pad1 = 0.0f; // Padding for alignment
	DirectX::XMFLOAT4 rotation; // Quaternion (x, y, z, w)
	DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };
	float pad2 = 0.0f; // Padding for alignment
};

// Main scene constant buffer
struct SceneCB {
	CameraCB camera;
	LightCB lights[8]; // Adjust as needed
	int numLights;
	int pad[3]; // Padding for 16-byte alignment
};

struct Vertex {
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
	DirectX::XMFLOAT4 tangent;
};