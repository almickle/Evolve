#pragma once
#include <DirectXMath.h>
#include "Types.h"

struct TextureBinding
{
	uint slot;
	AssetID data;
};

struct VectorBinding
{
	uint slot;
	DirectX::XMFLOAT3 data;
};

struct ScalarBinding
{
	uint slot;
	float data;
};