#pragma once
#include <array>
#include <string>
#include <vector>
#include "Asset.h"
#include "ShaderBindings.h"
#include "SystemManager.h"
#include "Types.h"

class JsonSerializer;
class GpuResourceManager;
class AssetManager;
class ConstantBuffer;
class DescriptorHeapManager;

class ShaderInstance :
	public Asset {
public:
	ShaderInstance( const AssetType& assetType, const std::string& name = "ShaderInstance" )
		: Asset( assetType, name )
	{
	}
	ShaderInstance( const AssetID& shader,
					const AssetType& assetType,
					const std::vector<TextureBinding>& textureBindings,
					const std::vector<VectorBinding>& vectorBindings,
					const std::vector<ScalarBinding>& scalarBindings,
					const std::string& name = "ShaderInstance" )
		: Asset( assetType, name ),
		shader( shader ),
		textureBindings( textureBindings ),
		vectorBindings( vectorBindings ),
		scalarBindings( scalarBindings )
	{
		AddAsset( shader );
	}
	virtual ~ShaderInstance() = default;
public:
	void Load( SystemManager* systemManager, JsonSerializer& serializer ) override;
	std::string Serialize( JsonSerializer& serializer ) const override;
	void Deserialize( JsonSerializer& serializer ) override;
public:
	void Update( AssetManager* assetManager, GpuResourceManager* resourceManager, DescriptorHeapManager* srvHeapManager );
	AssetID GetShader() { return shader; }
	std::vector<TextureBinding>& GetSrvBindings() { return textureBindings; }
	std::vector<VectorBinding>& GetVectorBindings() { return vectorBindings; }
	std::vector<ScalarBinding>& GetScalarBindings() { return scalarBindings; }
	std::array<ConstantBuffer*, 3> GetConstantBuffers( GpuResourceManager& resourceManager ) const;
protected:
	AssetID shader;
	std::vector<TextureBinding> textureBindings;
	std::vector<VectorBinding> vectorBindings;
	std::vector<ScalarBinding> scalarBindings;
protected:
	std::array<ResourceID, 3> constantBuffers;
};