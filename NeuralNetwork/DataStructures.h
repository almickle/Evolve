#pragma once
#include <DirectXMath.h>

struct Vertex {
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
	DirectX::XMFLOAT4 tangent;
};