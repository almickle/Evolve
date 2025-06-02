#pragma once
#include <memory>
#include <DirectXMath.h>
#include "Mesh.h"

class Actor {
public:
    Actor(std::shared_ptr<Mesh> mesh,
          const DirectX::XMFLOAT3& position = {0, 0, 0},
          const DirectX::XMFLOAT3& velocity = {0, 0, 0},
          const DirectX::XMFLOAT3& acceleration = {0, 0, 0},
          const DirectX::XMFLOAT4& rotation = {0, 0, 0, 1},
          const DirectX::XMFLOAT3& scale = {1, 1, 1}) // Default: unit scale
        : mesh(std::move(mesh)), position(position), velocity(velocity), acceleration(acceleration), rotation(rotation), scale(scale) {}

    // Position
    void SetPosition(const DirectX::XMFLOAT3& pos) { position = pos; }
    const DirectX::XMFLOAT3& GetPosition() const { return position; }

    // Velocity
    void SetVelocity(const DirectX::XMFLOAT3& vel) { velocity = vel; }
    const DirectX::XMFLOAT3& GetVelocity() const { return velocity; }

    // Acceleration
    void SetAcceleration(const DirectX::XMFLOAT3& acc) { acceleration = acc; }
    const DirectX::XMFLOAT3& GetAcceleration() const { return acceleration; }

    // Rotation
    void SetRotation(const DirectX::XMFLOAT4& rot) { rotation = rot; }
    const DirectX::XMFLOAT4& GetRotation() const { return rotation; }

    // Scale
    void SetScale(const DirectX::XMFLOAT3& s) { scale = s; }
    const DirectX::XMFLOAT3& GetScale() const { return scale; }

    // Mesh
    void SetMesh(std::shared_ptr<Mesh> m) { mesh = std::move(m); }
    std::shared_ptr<Mesh> GetMesh() const { return mesh; }

private:
    std::shared_ptr<Mesh> mesh;
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 velocity;
    DirectX::XMFLOAT3 acceleration;
    DirectX::XMFLOAT4 rotation; // Quaternion (x, y, z, w)
    DirectX::XMFLOAT3 scale;    // Scale (x, y, z)
};