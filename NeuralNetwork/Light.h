#pragma once
#include <DirectXMath.h>
#include "SceneData.h"

class Light {
public:
	Light( LightType type = LightType::Point );
public:
	void SetDirection( const DirectX::XMFLOAT3& dir ) { direction = dir; }
	void SetPosition( const DirectX::XMFLOAT3& pos ) { position = pos; }
	void SetColor( const DirectX::XMFLOAT3& col ) { color = col; }
	void SetIntensity( float i ) { intensity = i; }
	void SetType( LightType lightType ) { type = lightType; }
public:
	LightType GetType() const { return type; }
	const DirectX::XMFLOAT3& GetDirection() const { return direction; }
	const DirectX::XMFLOAT3& GetPosition() const { return position; }
	const DirectX::XMFLOAT3& GetColor() const { return color; }
	float GetIntensity() const { return intensity; }
private:
	LightType type;
	DirectX::XMFLOAT3 direction{ 0, -1, 0 };
	DirectX::XMFLOAT3 position{ 0, 0, 0 };
	DirectX::XMFLOAT3 color{ 1, 1, 1 };
	float intensity = 1.0f;
};