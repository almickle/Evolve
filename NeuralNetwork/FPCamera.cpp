//#include <cmath>
//#include <DirectXMath.h>
//#include <DirectXMathConvert.inl>
//#include <DirectXMathMatrix.inl>
//#include <DirectXMathVector.inl>
//#include "Camera.h"
//#include "FPCamera.h"
//
//FPCamera::FPCamera() : Camera() {}
//
//void FPCamera::MoveForward( float amount )
//{
//	using namespace DirectX;
//	XMVECTOR forward = XMVector3Normalize( XMLoadFloat3( &forwardVec ) );
//	XMVECTOR pos = XMLoadFloat3( &position );
//	pos = XMVectorAdd( pos, XMVectorScale( forward, amount * speed ) );
//	XMStoreFloat3( &position, pos );
//}
//
//void FPCamera::MoveRight( float amount )
//{
//	using namespace DirectX;
//	XMVECTOR right = XMVector3Normalize( XMLoadFloat3( &rightVec ) );
//	XMVECTOR pos = XMLoadFloat3( &position );
//	pos = XMVectorAdd( pos, XMVectorScale( right, amount * speed ) );
//	XMStoreFloat3( &position, pos );
//}
//
//void FPCamera::Rotate( float yawDelta, float pitchDelta )
//{
//	using namespace DirectX;
//	yaw += yawDelta * sensitivity;
//	pitch -= pitchDelta * sensitivity;
//	const float limit = XM_PIDIV2 - 0.01f;
//	if( pitch > limit ) pitch = limit;
//	if( pitch < -limit ) pitch = -limit;
//
//	// Z-up: Forward is Y, Up is Z
//	XMVECTOR forward = XMVectorSet(
//		cosf( yaw ) * cosf( pitch ), // X
//		sinf( yaw ) * cosf( pitch ), // Y
//		sinf( pitch ),             // Z
//		0.0f
//	);
//	XMVECTOR up = XMVectorSet( 0, 0, 1, 0 );
//	XMVECTOR right = XMVector3Cross( up, forward );
//	XMStoreFloat3( &forwardVec, forward );
//	XMStoreFloat3( &rightVec, right );
//}
//
//DirectX::XMFLOAT4X4 FPCamera::GetViewMatrix() const
//{
//	using namespace DirectX;
//	XMVECTOR pos = XMLoadFloat3( &GetPosition() );
//	XMVECTOR forward = XMVector3Normalize( XMLoadFloat3( &forwardVec ) );
//	XMVECTOR upv = XMVectorSet( 0, 0, 1, 0 ); // Z-up
//	XMVECTOR target = XMVectorAdd( pos, forward );
//	DirectX::XMFLOAT4X4 view;
//	XMStoreFloat4x4( &view, XMMatrixLookAtLH( pos, target, upv ) );
//	return view;
//}