#include <DirectXMath.h>
#include <DirectXMathConvert.inl>
#include <DirectXMathMatrix.inl>
#include <DirectXMathVector.inl>
#include "Camera.h"

Camera::Camera() = default;

DirectX::XMFLOAT4X4 Camera::GetProjectionMatrix() const
{
	using namespace DirectX;
	DirectX::XMFLOAT4X4 projection;
	XMStoreFloat4x4( &projection, XMMatrixTranspose( XMMatrixPerspectiveFovLH( fovY, aspect, nearZ, farZ ) ) );
	return projection;
}

DirectX::XMFLOAT4X4 Camera::GetViewMatrix() const
{
	using namespace DirectX;
	XMVECTOR pos = XMLoadFloat3( &GetPosition() );
	XMVECTOR forward = XMVector3Normalize( XMLoadFloat3( &forwardVec ) );
	XMVECTOR upv = XMVectorSet( 0, 0, 1, 0 ); // Z-up
	XMVECTOR target = XMVectorAdd( pos, forward );
	DirectX::XMFLOAT4X4 view;
	XMStoreFloat4x4( &view, XMMatrixTranspose( XMMatrixLookAtLH( pos, target, upv ) ) );
	return view;
}