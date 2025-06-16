#pragma once
#include <string>
#include <vector>
#include "Asset.h"
#include "JsonSerializer.h"
#include "ShaderBindings.h"
#include "SystemManager.h"
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
	void Load( SystemManager* systemManager ) override;
	std::string Serialize( JsonSerializer& serializer ) const override;
	void Deserialize( JsonSerializer& serializer ) override;
public:
	AssetID GetModifierTemplate() { return modifierTemplate; }
private:
	AssetID modifierTemplate;
	std::vector<TextureBinding> textureBindings;
	std::vector<VectorBinding> vectorBindings;
	std::vector<ScalarBinding> scalarBindings;
};

