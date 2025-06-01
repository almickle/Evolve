#include "Camera.h"
#include <DirectXMath.h>

Camera::Camera() = default;

DirectX::XMMATRIX Camera::GetViewMatrix() const {
    using namespace DirectX;
    XMVECTOR pos = XMLoadFloat3(&position);
    XMVECTOR tgt = XMLoadFloat3(&target);
    XMVECTOR upv = XMLoadFloat3(&up);
    return XMMatrixLookAtLH(pos, tgt, upv);
}

DirectX::XMMATRIX Camera::GetProjMatrix() const {
    using namespace DirectX;
    return XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
}