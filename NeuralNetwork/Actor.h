//#pragma once
//#include <memory>
//#include <DirectXMath.h>
//#include "Model.h"
//#include "InstanceManager.h" 
//
//class Actor {
//public:
//    Actor(ModelID modelId, InstanceID instanceId)
//        : modelId(modelId), instanceId(instanceId) {}
//public:
//    void SetPosition(const DirectX::XMFLOAT3& pos) { position = pos; }
//    void SetVelocity(const DirectX::XMFLOAT3& vel) { velocity = vel; }
//    void SetAcceleration(const DirectX::XMFLOAT3& acc) { acceleration = acc; }
//    void SetRotation(const DirectX::XMFLOAT4& rot) { rotation = rot; }
//    void SetScale(const DirectX::XMFLOAT3& s) { scale = s; }
//public:
//    const DirectX::XMFLOAT3& GetPosition() const { return position; }
//    const DirectX::XMFLOAT3& GetVelocity() const { return velocity; }
//    const DirectX::XMFLOAT3& GetAcceleration() const { return acceleration; }
//    const DirectX::XMFLOAT4& GetRotation() const { return rotation; }
//    const DirectX::XMFLOAT3& GetScale() const { return scale; }
//    ModelID GetModelID() const { return modelId; }
//    InstanceID GetInstanceID() const { return instanceId; }
//    DirectX::XMMATRIX GetTransform() const;
//    InstanceData GetInstanceData() const;
//private:
//    DirectX::XMFLOAT3 position = { 0, 0, 0 };
//    DirectX::XMFLOAT3 velocity = { 0, 0, 0 };
//    DirectX::XMFLOAT3 acceleration = { 0, 0, 0 };
//    DirectX::XMFLOAT4 rotation = { 0, 0, 0, 1 }; // Quaternion (x, y, z, w)
//    DirectX::XMFLOAT3 scale = { 1, 1, 1 };    // Scale (x, y, z)
//private:
//    ModelID modelId;
//    InstanceID instanceId;
//};