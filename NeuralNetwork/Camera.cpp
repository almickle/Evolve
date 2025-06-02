#include "Camera.h"
#include <DirectXMath.h>

Camera::Camera() = default;

DirectX::XMMATRIX Camera::GetProjMatrix() const {
    using namespace DirectX;
    return XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
}