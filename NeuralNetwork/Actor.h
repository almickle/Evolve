#pragma once
#include <DirectXMath.h>
#include "Types.h"

class Actor {
public:
	Actor( const AssetID& modelId )
		:modelId( modelId )
	{
	}
	Actor() = default;
	~Actor() = default;
public:
	void SetPosition( const DirectX::XMFLOAT3& pos ) { position = pos; }
	void SetVelocity( const DirectX::XMFLOAT3& vel ) { velocity = vel; }
	void SetAcceleration( const DirectX::XMFLOAT3& acc ) { acceleration = acc; }
	void SetRotation( const DirectX::XMFLOAT4& rot ) { rotation = rot; }
	void SetScale( const DirectX::XMFLOAT3& s ) { scale = s; }
	void SetModel( const AssetID& model ) { modelId = model; }
public:
	const DirectX::XMFLOAT3& GetPosition() const { return position; }
	const DirectX::XMFLOAT3& GetVelocity() const { return velocity; }
	const DirectX::XMFLOAT3& GetAcceleration() const { return acceleration; }
	const DirectX::XMFLOAT4& GetRotation() const { return rotation; }
	const DirectX::XMFLOAT3& GetScale() const { return scale; }
	const AssetID GetModelID() const { return modelId; }
	const DirectX::XMFLOAT4X4 GetTransform() const;
private:
	DirectX::XMFLOAT3 position = { 0, 0, 0 };
	DirectX::XMFLOAT4 rotation = { 0, 0, 0, 1 }; // Quaternion (x, y, z, w)
	DirectX::XMFLOAT3 scale = { 1, 1, 1 };
private:
	DirectX::XMFLOAT3 velocity = { 0, 0, 0 };
	DirectX::XMFLOAT3 acceleration = { 0, 0, 0 };
private:
	AssetID modelId;
};