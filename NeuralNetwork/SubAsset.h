#pragma once
#include <string>
#include <vector>
#include "Types.h"

class SubAsset {
public:
	SubAsset( SubAssetType subType = SubAssetType::Unknown, const std::string& debugName = "SubAsset" )
		: subType( subType ), debugName( debugName )
	{
	}
	virtual ~SubAsset() = default;
	const std::vector<ResourceID>& GetResourceIDs() const { return resourceIDs; }
	void AddResource( const ResourceID& id ) { resourceIDs.push_back( id ); }
protected:
	SubAssetType subType;
	std::string debugName;
	std::vector<ResourceID> resourceIDs;
};