//#pragma once
//#include "Camera.h"
//
//class FPCamera : public Camera {
//public:
//	FPCamera();
//
//	float GetSensitivity() const { return sensitivity; }
//	float GetSpeed() const { return speed; }
//
//	void SetSensitivity( float s ) { sensitivity = s; }
//	void SetSpeed( float s ) { speed = s; }
//	void SetForwardVec( const DirectX::XMFLOAT3& forward ) { forwardVec = forward; }
//
//	void MoveForward( float amount ) override;
//	void MoveRight( float amount ) override;
//	void Rotate( float yawDelta, float pitchDelta ) override;
//
//	DirectX::XMFLOAT4X4 GetViewMatrix() const override;
//
//private:
//	float sensitivity = 0.3f; // Default sensitivity
//	float speed = 1.0f;       // Default movement speed
//	DirectX::XMFLOAT3 forwardVec{ 0, 0, 1 };
//	DirectX::XMFLOAT3 rightVec{ 1, 0, 0 };
//};