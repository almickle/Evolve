#pragma once
#include <string>
#include <vector>
#include "Asset.h"
#include "DataStructures.h"
#include "GpuResourceManager.h"
#include "JsonSerializer.h"
#include "Types.h"

struct MeshData {
	std::vector<Vertex> vertices;
	std::vector<uint> indices;
};

class Mesh : public Asset {
public:
	Mesh( const std::string& name = "Mesh" )
		: Asset( AssetType::Mesh, name )
	{
	}
	Mesh( const std::vector<MeshData>& meshData, const std::string& name = "Mesh" )
		: Asset( AssetType::Mesh, name ),
		meshData( meshData )
	{
	}
	~Mesh() = default;
public:
	void Load( GpuResourceManager& resourceManager, JsonSerializer& serializer ) override;
	virtual std::string Serialize( JsonSerializer& serializer ) const override;
	virtual void Deserialize( JsonSerializer& serializer ) override;
private:
	std::vector<MeshData> meshData;
};