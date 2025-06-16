#pragma once
#include <array>
#include <string>
#include <vector>
#include "Asset.h"
#include "ConstantBuffer.h"
#include "GpuResourceManager.h"
#include "JsonSerializer.h"
#include "ShaderBindings.h"
#include "SystemManager.h"
#include "Types.h"

class Material :
	public Asset {
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
	void Load( SystemManager* systemManager ) override;
	std::string Serialize( JsonSerializer& serializer ) const override;
	void Deserialize( JsonSerializer& serializer ) override;
public:
	AssetID GetMaterialTemplate() { return materialTemplate; }
public:
	std::array<ConstantBuffer*, 3> GetConstantBuffers( GpuResourceManager& resourceManager ) const
	{
		std::array<ConstantBuffer*, 3> buffers{};
		for( size_t i = 0; i < constantBuffers.size(); ++i ) {
			buffers[i] = static_cast<ConstantBuffer*>( resourceManager.GetResource( constantBuffers[i] ) );
		}
		return buffers;
	}
private:
	AssetID materialTemplate;
	std::vector<TextureBinding> textureBindings;
	std::vector<VectorBinding> vectorBindings;
	std::vector<ScalarBinding> scalarBindings;
private:
	std::array<ResourceID, 3> constantBuffers;
};