#pragma once
#include <string>
#include <vector>
#include "Asset.h"
#include "GpuResourceManager.h"
#include "JsonSerializer.h"
#include "Types.h"

class Model : public Asset {
public:
	Model( const std::string& name = "Model" )
		: Asset( AssetType::Model, name )
	{
	}
	~Model() = default;
public:
	void Load( GpuResourceManager& resourceManager, JsonSerializer& serializer ) override;
	std::string Serialize( JsonSerializer& serializer ) const override;
	void Deserialize( JsonSerializer& serializer ) override;
private:
	AssetID meshId;
	std::vector<AssetID> materialSlots;
};

