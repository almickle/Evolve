//#include "Actor.h"
//
//DirectX::XMMATRIX Actor::GetTransform() const {
//    using namespace DirectX;
//    XMMATRIX S = XMMatrixScaling(scale.x, scale.y, scale.z);
//    XMMATRIX R = XMMatrixRotationQuaternion(XMLoadFloat4(&rotation));
//    XMMATRIX T = XMMatrixTranslation(position.x, position.y, position.z);
//    return S * R * T;
//}
//
//InstanceData Actor::GetInstanceData() const {
//    InstanceData data;
//    data.transform = GetTransform();
//    // Add more per-instance data here if needed
//    return data;
//}