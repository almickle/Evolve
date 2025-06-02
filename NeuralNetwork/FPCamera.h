#pragma once
#include "Camera.h"

class FPCamera : public Camera {
public:
    FPCamera();
public:
    float GetSensitivity() const { return sensitivity; }
public:
    void SetSensitivity(float s) { sensitivity = s; }
public:
    void MoveForward(float amount) override;
    void MoveRight(float amount) override;
    void Rotate(float yawDelta, float pitchDelta) override;
public:
    DirectX::XMMATRIX GetViewMatrix() const override;
private:
    float sensitivity = 0.3f; // Default sensitivity
    DirectX::XMFLOAT3 forwardVec{ 0, 0, 1 };
    DirectX::XMFLOAT3 rightVec{ 1, 0, 0 };
};