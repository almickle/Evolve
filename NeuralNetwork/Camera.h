#pragma once
#include <DirectXMath.h>

class Camera {
public:
    Camera();

    void SetPosition(const DirectX::XMFLOAT3& pos) { position = pos; }
    void SetTarget(const DirectX::XMFLOAT3& tgt) { target = tgt; }
    void SetUp(const DirectX::XMFLOAT3& upVec) { up = upVec; }
    void SetFovY(float fov) { fovY = fov; }
    void SetAspect(float a) { aspect = a; }
    void SetNearZ(float n) { nearZ = n; }
    void SetFarZ(float f) { farZ = f; }

    const DirectX::XMFLOAT3& GetPosition() const { return position; }
    const DirectX::XMFLOAT3& GetTarget() const { return target; }
    const DirectX::XMFLOAT3& GetUp() const { return up; }
    float GetFovY() const { return fovY; }
    float GetAspect() const { return aspect; }
    float GetNearZ() const { return nearZ; }
    float GetFarZ() const { return farZ; }

    DirectX::XMMATRIX GetViewMatrix() const;
    DirectX::XMMATRIX GetProjMatrix() const;

private:
    DirectX::XMFLOAT3 position{0, 0, -5};
    DirectX::XMFLOAT3 target{0, 0, 0};
    DirectX::XMFLOAT3 up{0, 1, 0};
    float fovY = DirectX::XM_PIDIV4;
    float aspect = 2.333f; // 21:9
    float nearZ = 0.1f;
    float farZ = 1000.0f;
};