#pragma once
#include <string>
#include <vector>
#include "Asset.h"
#include "JsonSerializer.h"
#include "ShaderBindings.h"
#include "Types.h"

class Modifier :
	public Asset
{
public:
	Modifier( const std::string& name = "Modifier" )
		: Asset( AssetType::Modifier, name )
	{
	}
	~Modifier() = default;
public:
	void Load( GpuResourceManager& resourceManager, JsonSerializer& serializer ) override;
	std::string Serialize( JsonSerializer& serializer ) const override;
	void Deserialize( JsonSerializer& serializer ) override;
private:
	AssetID modifierTemplate;
	std::vector<TextureBinding> textureBindings;
	std::vector<VectorBinding> vectorBindings;
	std::vector<ScalarBinding> scalarBindings;
};

