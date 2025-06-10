#pragma once
#include <string>
#include "Asset.h"
#include "Types.h"

class Mesh : public Asset {
public:
	Mesh( const std::string& name = "Mesh" )
		: Asset( AssetType::Mesh, name )
	{
	}
	~Mesh() = default;
public:
	void Load( const std::string& path, GpuResourceManager& resourceManager ) override;
};