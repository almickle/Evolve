#include <cmath>
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

void Camera::MoveLeft( float amount )
{
	using namespace DirectX;
	XMVECTOR right = XMVector3Normalize( XMLoadFloat3( &rightVec ) );
	XMVECTOR pos = XMLoadFloat3( &position );
	pos = XMVectorSubtract( pos, XMVectorScale( right, amount * speed ) );
	XMStoreFloat3( &position, pos );
}

void Camera::MoveRight( float amount )
{
	using namespace DirectX;
	XMVECTOR right = XMVector3Normalize( XMLoadFloat3( &rightVec ) );
	XMVECTOR pos = XMLoadFloat3( &position );
	pos = XMVectorAdd( pos, XMVectorScale( right, amount * speed ) );
	XMStoreFloat3( &position, pos );
}

void Camera::MoveUp( float amount )
{
	using namespace DirectX;
	// Up is always (0, 0, 1) in Z-up convention
	XMVECTOR up = XMVectorSet( 0, 0, 1, 0 );
	XMVECTOR pos = XMLoadFloat3( &position );
	pos = XMVectorAdd( pos, XMVectorScale( up, amount * speed ) );
	XMStoreFloat3( &position, pos );
}

void Camera::MoveDown( float amount )
{
	using namespace DirectX;
	// Up is always (0, 0, 1) in Z-up convention
	XMVECTOR up = XMVectorSet( 0, 0, 1, 0 );
	XMVECTOR pos = XMLoadFloat3( &position );
	pos = XMVectorSubtract( pos, XMVectorScale( up, amount * speed ) );
	XMStoreFloat3( &position, pos );
}

void Camera::MoveForward( float amount )
{
	using namespace DirectX;
	XMVECTOR forward = XMVector3Normalize( XMLoadFloat3( &forwardVec ) );
	XMVECTOR pos = XMLoadFloat3( &position );
	pos = XMVectorAdd( pos, XMVectorScale( forward, amount * speed ) );
	XMStoreFloat3( &position, pos );
}

void Camera::MoveBack( float amount )
{
	using namespace DirectX;
	XMVECTOR forward = XMVector3Normalize( XMLoadFloat3( &forwardVec ) );
	XMVECTOR pos = XMLoadFloat3( &position );
	pos = XMVectorSubtract( pos, XMVectorScale( forward, amount * speed ) );
	XMStoreFloat3( &position, pos );
}

void Camera::Rotate( float yawDelta, float pitchDelta )
{
	using namespace DirectX;
	yaw += yawDelta * sensitivity;
	pitch -= pitchDelta * sensitivity;
	const float limit = XM_PIDIV2 - 0.01f;
	if( pitch > limit ) pitch = limit;
	if( pitch < -limit ) pitch = -limit;

	// Z-up: Forward is Y, Up is Z
	XMVECTOR forward = XMVectorSet(
		cosf( yaw ) * cosf( pitch ), // X
		sinf( yaw ) * cosf( pitch ), // Y
		sinf( pitch ),             // Z
		0.0f
	);
	XMVECTOR up = XMVectorSet( 0, 0, 1, 0 );
	XMVECTOR right = XMVector3Cross( up, forward );
	XMStoreFloat3( &forwardVec, forward );
	XMStoreFloat3( &rightVec, right );
}
