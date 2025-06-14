#pragma once
#include <string>
#include "Asset.h"
#include "JsonSerializer.h"
#include "Types.h"

class ModifierTemplate :
	public Asset
{
public:
	ModifierTemplate( const std::string& name = "ModifierTemplate" )
		: Asset( AssetType::ModifierTemplate, name )
	{
	}
public:
	std::string GetShaderCode();
	void Load( GpuResourceManager& resourceManager, JsonSerializer& serializer ) override;
	std::string Serialize( JsonSerializer& serializer ) const override;
	void Deserialize( JsonSerializer& serializer ) override;
};

