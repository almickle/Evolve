#pragma once
#include <DirectXMath.h>

class Camera {
public:
	Camera();
public:
	void SetPosition( const DirectX::XMFLOAT3& pos ) { position = pos; }
	void SetFovY( float fov ) { fovY = fov; }
	void SetAspect( float a ) { aspect = a; }
	void SetNearZ( float n ) { nearZ = n; }
	void SetFarZ( float f ) { farZ = f; }
	void SetForwardVec( const DirectX::XMFLOAT3& forward ) { forwardVec = forward; }
public:
	const DirectX::XMFLOAT3& GetPosition() const { return position; }
	const DirectX::XMFLOAT3& GetUp() const { return up; }
	float GetFovY() const { return fovY; }
	float GetAspect() const { return aspect; }
	float GetNearZ() const { return nearZ; }
	float GetFarZ() const { return farZ; }
public:
	DirectX::XMFLOAT4X4 GetViewMatrix() const;
	DirectX::XMFLOAT4X4 GetProjectionMatrix() const;
public:
	void MoveLeft( float amount = 1.0f );
	void MoveRight( float amount = 1.0f );
	void MoveForward( float amount = 1.0f );
	void MoveBack( float amount = 1.0f );
	void MoveUp( float amount = 1.0f );
	void MoveDown( float amount = 1.0f );
	void Rotate( float yawDelta, float pitchDelta );
protected:
	DirectX::XMFLOAT3 position{ 0, 0, 0 };
	DirectX::XMFLOAT3 forwardVec{ 0, 1, 0 };
	DirectX::XMFLOAT3 rightVec{ 1, 0, 0 };
	DirectX::XMFLOAT3 up{ 0, 0, 1 };
	float fovY = DirectX::XM_PIDIV4;
	float aspect = 2.333f;	  // 21:9
	float nearZ = 0.1f;
	float farZ = 1000.0f;
	float yaw = 0.0f;
	float pitch = 0.0f;
	float sensitivity = 0.003f; // Default sensitivity
	float speed = 0.1f;         // Default movement speed
};