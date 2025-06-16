#include <DirectXMath.h>
#include <DirectXMathConvert.inl>
#include <DirectXMathMatrix.inl>
#include "Actor.h"

const DirectX::XMFLOAT4X4 Actor::GetTransform() const
{
	using namespace DirectX;
	XMMATRIX S = XMMatrixScaling( scale.x, scale.y, scale.z );
	XMMATRIX R = XMMatrixRotationQuaternion( XMLoadFloat4( &rotation ) );
	XMMATRIX T = XMMatrixTranslation( position.x, position.y, position.z );
	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4( &world, S * R * T );
	return world;
}