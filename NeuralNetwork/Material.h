#pragma once
#include <DirectXMath.h>
#include <string>
#include <vector>
#include "Asset.h"
#include "JsonSerializer.h"
#include "Types.h"

class GpuResourceManager;

struct TextureBinding
{
	uint slot;
	AssetID data;
};

struct VectorBinding
{
	uint slot;
	DirectX::XMFLOAT3 data;
};

struct ScalarBinding
{
	uint slot;
	float data;
};

class Material : public Asset {
public:
	Material( const std::string& name = "Material" )
		: Asset( AssetType::Material, name )
	{
	}
	Material( const AssetID& materialTemplate,
			  const std::vector<TextureBinding>& textureBindings,
			  const std::vector<VectorBinding>& vectorBindings,
			  const std::vector<ScalarBinding>& scalarBindings,
			  const std::string& name = "Material" )
		: Asset( AssetType::Material, name ),
		materialTemplate( materialTemplate ),
		textureBindings( textureBindings ),
		vectorBindings( vectorBindings ),
		scalarBindings( scalarBindings )
	{
		AddAsset( materialTemplate );
	}
	~Material() = default;
public:
	void Load( GpuResourceManager& resourceManager, JsonSerializer& serializer ) override;
	std::string Serialize( JsonSerializer& serializer ) const override;
	void Deserialize( JsonSerializer& serializer ) override;
private:
	AssetID materialTemplate;
	std::vector<TextureBinding> textureBindings;
	std::vector<VectorBinding> vectorBindings;
	std::vector<ScalarBinding> scalarBindings;
};