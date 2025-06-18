#pragma once
#include <DirectXMath.h>

enum class LightType {
	Point,
	Directional,
	Spot
};

struct LightData {
	DirectX::XMFLOAT3 position;
	float intensity;
	DirectX::XMFLOAT3 color;
	LightType type;
	DirectX::XMFLOAT3 direction;
	float pad; // Padding for alignment
};

struct CameraData {
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 proj;
	DirectX::XMFLOAT3 position;
	float pad; // Padding for alignment
};

struct ActorTransformData {
	DirectX::XMFLOAT4X4 world;
};

struct SceneData {
	CameraData camera;
	LightData lights[8]; // Max lights
	unsigned int numLights;
	float pad[3];
};

struct SceneConstants {
	unsigned int instanceBufferStart;
	unsigned int bufferIndex;
};