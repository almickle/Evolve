#include "FPCamera.h"
#include <DirectXMath.h>

FPCamera::FPCamera() : Camera() {}

void FPCamera::MoveForward(float amount) {
    using namespace DirectX;
    XMVECTOR forward = XMVector3Normalize(XMLoadFloat3(&forwardVec));
    XMVECTOR pos = XMLoadFloat3(&position);
    pos = XMVectorAdd(pos, XMVectorScale(forward, amount));
    XMStoreFloat3(&position, pos);
}

void FPCamera::MoveRight(float amount) {
    using namespace DirectX;
    XMVECTOR right = XMVector3Normalize(XMLoadFloat3(&rightVec));
    XMVECTOR pos = XMLoadFloat3(&position);
    pos = XMVectorAdd(pos, XMVectorScale(right, amount));
    XMStoreFloat3(&position, pos);
}

void FPCamera::Rotate(float yawDelta, float pitchDelta) {
    using namespace DirectX;
    yaw += yawDelta * sensitivity;
    pitch -= pitchDelta * sensitivity;
    const float limit = XM_PIDIV2 - 0.01f;
    if (pitch > limit) pitch = limit;
    if (pitch < -limit) pitch = -limit;

    XMVECTOR forward = XMVectorSet(
        cosf(pitch) * sinf(yaw),
        sinf(pitch),
        cosf(pitch) * cosf(yaw),
        0.0f
    );
    XMVECTOR right = XMVector3Cross(XMVectorSet(0, 1, 0, 0), forward);
    XMStoreFloat3(&forwardVec, forward);
    XMStoreFloat3(&rightVec, right);
}

DirectX::XMMATRIX FPCamera::GetViewMatrix() const {
    using namespace DirectX;
    XMVECTOR pos = XMLoadFloat3(&GetPosition());
    XMVECTOR forward = XMVector3Normalize(XMLoadFloat3(&forwardVec));
    XMVECTOR upv = XMLoadFloat3(&up);
    XMVECTOR target = XMVectorAdd(pos, forward);
    return XMMatrixLookAtLH(pos, target, upv);
}